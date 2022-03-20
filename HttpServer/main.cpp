#include <stdio.h>

#include "http_server.h"
#include "settings.h"
#include "request_handlers.h"
#include "config.h"

static constexpr unsigned short int DefaultPort = 80;

struct Options
{
	unsigned short int port = 0;
	int addressFamily = 0;
	std::string serverFiles;
	bool allowInternet = false;
	std::unordered_map<CaseInsensitiveString, CaseInsensitiveString> proxies;
};

static void ParseArguments(int argc, char *argv[], Options *out);
static void Help();

int main(int argc, char *argv[])
{
	Options options;
	ParseArguments(argc - 1, argv + 1, &options);

	const char *famstr;
	if (options.addressFamily == AF_INET)
		famstr = "INET";
	else if (options.addressFamily == AF_INET6)
		famstr = "INET6";
	else
		famstr = "UNKN";

	printf("Server Settings:\n");
	printf("  ServerName: %s\n", SERVER_NAME);
	printf("  Port: %hu\n", options.port);
	printf("  AddressFamily: %s\n", famstr);
	printf("  ServerFiles: \"%s\"\n", options.serverFiles.c_str());
	printf("  AllowInternet: %s\n\n", options.allowInternet ? "true" : "false");

	printf("Initialize server...\n");

	WSADATA wsaData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed\n");
		return 1;
	}

	HTTPServer httpServer(options.serverFiles);
	//httpServer.CreateResourceProxy("/", "/index.html");
	for (auto it : options.proxies)
		httpServer.CreateResourceProxy(it.first, it.second);

	//printf("Opening server on port %hu using address family %s\n", options.port, famstr);

	Server server;
	if (!server.Open(options.port, options.addressFamily))
	{
		printf("Failed to open server on port %hu\n", options.port);

		WSACleanup();
		return 1;
	}

	if (!httpServer.Bind(&server))
	{
		printf("Failed to make server an HTTP server\n");

		server.Close();
		WSACleanup();
		return 1;
	}

	httpServer.SetRequestHandler(METHOD_GET, &HandleGETRequest);

	printf("Dispatch server...\n");
	if (!httpServer.DispatchServer())
	{
		printf("Failed to dispatch server\n");

		httpServer.Close();
		WSACleanup();
		return 1;
	}

	printf("Done!\n");
	/*if (!httpServer.WaitUntilFinish())
	{
		printf("Failed to wait until server finishes\n");
		
		WSACleanup();
		return 1;
	}*/

	char buf[1024];
	do
	{
		printf("> ");
		int filled = scanf_s("%1023s", buf, (unsigned)_countof(buf));
		if (filled == EOF)
		{
			printf("EOF encountered, closing server...\n");
			httpServer.Close();

			printf("Waiting for server to exit...\n");
			httpServer.WaitUntilFinish();

			break;
		}
		else if (filled > 0)
		{
			if (equalsIgnoreCase(buf, "shutdown") || equalsIgnoreCase(buf, "exit"))
			{
				printf("Closing server...\n");
				httpServer.Close();

				printf("Waiting for server to exit...\n");
				httpServer.WaitUntilFinish();

				break;
			}
			else if (equalsIgnoreCase(buf, "quit"))
			{
				printf("Forcefully exiting...\n");
				printf("Exiting without cleanup!\n");
				exit(1);
				break;
			}
			else if (equalsIgnoreCase(buf, "rstat"))
			{
				printf("%-32s %-14s %-15s\n", "[Path]", "[DRAM Usage]", "[Mapped Size]");
				for (auto it : httpServer.GetResources())
				{
					printf("%-32s %-14zu %-15zu\n", it.first.cstr(), it.second->GetDRAMUsage(), it.second->GetMemoryMappedSize());
				}
			}
			else if (equalsIgnoreCase(buf, "help"))
			{
				printf("Commands:\n");
				printf("  shutdown | exit     Shuts down the server\n");
				printf("  quit                Forcefully exits the application\n");
				printf("  rstat               Prints resource statistics\n");
			}
			else
			{
				printf("?\n");
			}
		}
	} while (true);

	//httpServer.Close();
	WSACleanup();

	printf("Exiting normally!\n");

	return 0;
}

void ParseArguments(int argc, char *argv[], Options *out)
{
	struct
	{
		bool valid;

		bool help;
		const char *settings;
		const char *port;
		const char *address;
		const char *files;
		bool internetspec;
	} args;
	memset(&args, 0, sizeof(args));
	args.valid = true;
	args.settings = "settings.ini";

	//memset(out, 0, sizeof(Options));

	for (int i = 0; i < argc; i++)
	{
		if (equalsIgnoreCase(argv[i], "--help") || equalsIgnoreCase(argv[i], "-h"))
			args.help = true;
		else if (equalsIgnoreCase(argv[i], "--settings") || equalsIgnoreCase(argv[i], "-s"))
		{
			i++;
			if (i == argc)
			{
				args.valid = false;
				break;
			}
			args.settings = argv[i];
		}
		else if (equalsIgnoreCase(argv[i], "--port") || equalsIgnoreCase(argv[i], "-p"))
		{
			i++;
			if (i == argc)
			{
				args.valid = false;
				break;
			}
			args.port = argv[i];
		}
		else if (equalsIgnoreCase(argv[i], "--address") || equalsIgnoreCase(argv[i], "-a"))
		{
			i++;
			if (i == argc)
			{
				args.valid = false;
				break;
			}
			args.address = argv[i];
		}
		else if (equalsIgnoreCase(argv[i], "--files") || equalsIgnoreCase(argv[i], "-f"))
		{
			i++;
			if (i == argc)
			{
				args.valid = false;
				break;
			}
			args.files = argv[i];
		}
		else if (equalsIgnoreCase(argv[i], "--internet") || equalsIgnoreCase(argv[i], "-i"))
			args.internetspec = true;
		else
		{
			Help();
			exit(1);
		}
	}

	if (!args.valid || args.help)
	{
		Help();
		exit(!args.valid);
	}

	if (args.settings)
	{
		ConfigFile config(args.settings);

		const ConfigFile::Section *section = config.FindSection("server");
		if (section)
		{
			const ConfigFile::Value *value;

			value = section->FindValue("port");
			if (value)
				out->port = (unsigned short int)value->intValue;
			
			value = section->FindValue("family");
			if (value)
			{
				if (equalsIgnoreCase(value->stringValue, "inet"))
					out->addressFamily = AF_INET;
				else if (equalsIgnoreCase(value->stringValue, "inet6"))
					out->addressFamily = AF_INET6;
			}

			value = section->FindValue("files");
			if (value)
				out->serverFiles = value->stringValue;

			value = section->FindValue("internet");
			if (value)
				out->allowInternet = value->boolValue;
		}

		section = config.FindSection("resource.proxies");
		if (section)
		{
			for (auto pair : section->GetValues())
				out->proxies[pair.first] = pair.second.stringValue;
		}
	}

	if (args.port)
	{
		char *end;
		out->port = strtol(args.port, &end, 10);
	}

	if (args.address)
	{
		out->addressFamily = 0;
		if (equalsIgnoreCase(args.address, "inet"))
			out->addressFamily = AF_INET;
		else if (equalsIgnoreCase(args.address, "inet6"))
			out->addressFamily = AF_INET6;
	}

	if (args.files)
		out->serverFiles = args.files;
	
	if (args.internetspec)
		out->allowInternet = true;
}

void Help()
{
	printf("HttpServer [options...]\n");
	printf("Where [options...] include:\n");
	printf("--help -h       Displays this help message.\n");
	printf("--setings -s    Sets the settings file to use, default settings.ini\n");
	printf("--port -p       Sets the port to host the server on, default 80.\n");
	printf("--address -a    [inet | inet6]\n");
	printf("                Sets the address family to use, IPv4 or IPv6\n");
	printf("--files -f      Sets the directory to search for server resources in\n");
	printf("--internet -i   Allow internet connection\n");
}
