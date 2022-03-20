#include "http_connection.h"

#include "util.h"

static constexpr char NewLine[] = "\r\n";
static constexpr int NewLineLength = sizeof(NewLine) - 1;
static constexpr char HeaderSeparator[] = ":";
static constexpr int HeaderSeparatorLength = sizeof(HeaderSeparator) - 1;

static constexpr int BufferSize = 8192;

HTTPConnection::~HTTPConnection()
{
	Close();
}

bool HTTPConnection::Bind(ClientConnection *connection)
{
	if (m_connection || !connection) return false;
	m_connection = connection;
	return true;
}

void HTTPConnection::Close()
{
	if (m_connection)
	{
		delete m_connection;
		m_connection = nullptr;
	}
}

HTTPRequest *HTTPConnection::GetNextRequest()
{
	if (!m_connection) return nullptr;

	char buffer[BufferSize];
	int headerlen;

	do
	{
		int len = m_connection->ReadBytes(buffer, BufferSize);
		if (len <= 0)
		{
			// connection closed/error
			return nullptr;
		}

		m_buffer.Append(buffer, len);

		char *buf = m_buffer.GetElements();
		int bufsize = m_buffer.Size();

		
		int lastind, currind = 0;
		do
		{
			lastind = currind;
			currind = FindFirstOf(buf, bufsize, NewLine, NewLineLength);
			if (currind == -1) break; // need to read more

			// double NewLine means go to content field
			//if (currind - lastind == sizeof(NewLine))
			//	goto exit_main;

			buf += currind + NewLineLength;
			bufsize -= currind + NewLineLength;

			// double NewLine means go to message body
			if (equals(buf, NewLine))
			{
				buf += NewLineLength;
				headerlen = buf - m_buffer.GetElements();
				goto exit_main;
			}
		} while (true);
	} while (true);

exit_main:
	HTTPRequest *request = new HTTPRequest();
	std::unordered_map<CaseInsensitiveString, std::string> &headers = request->m_headers;

	// parse the header
	char *headerdata = new char[headerlen + 1];
	memcpy(headerdata, m_buffer.GetElements(), headerlen);
	headerdata[headerlen] = 0;

	std::vector<std::string> lines;
	Tokenize(headerdata, NewLine, &lines);

	delete[] headerdata;

	if (lines.size() == 0)
	{
		delete request;
		return nullptr;
	}

	// parse: [METHOD] [URI] HTTP/1.1
	std::string &info = lines[0];
	std::vector<std::string> tokens;

	Tokenize((char *)info.c_str(), " ", &tokens);
	if (tokens.size() != 3)
	{
		delete request;
		return nullptr;
	}

	request->m_method = GetMethodFromString(tokens[0].c_str());
	if (request->m_method == METHOD_NONE)
	{
		delete request;
		return nullptr;
	}

	request->m_uri.Parse(tokens[1]);

	if (!equalsIgnoreCase(tokens[2], "HTTP/1.1"))
	{
		delete request;
		return nullptr;
	}

	// parse headers

	for (size_t lidx = 1; lidx < lines.size(); lidx++)
	{
		const std::string &line = lines[lidx];
		int ind = FindFirstOf(line.c_str(), line.size(), HeaderSeparator, HeaderSeparatorLength);
		if (ind == -1) continue;

		CaseInsensitiveString key(line.substr(0, ind));
		std::string value(line.c_str() + ind + 1);

		headers[key] = Trim(value);
	}


	// test whether this has content, if so copy it
	auto it = headers.find("Content-Length");
	if (it != headers.end())
	{
		char *end;
		request->m_contentlen = strtol(it->second.c_str(), &end, 10);
		if (request->m_contentlen > 0)
		{
			request->m_content = new char[request->m_contentlen];
			int len = m_connection->ReadBytes(request->m_content, request->m_contentlen);

			if (len < request->m_contentlen)
			{
				delete request;
				return nullptr;
			}
		}
	}

	request->m_source = this;

	// shift the buffer over
	size_t requestsize = headerlen + request->m_contentlen;
	m_buffer.ShiftBack(requestsize);// = StringBuilder(m_buffer, requestsize);

	return request;
}

int HTTPConnection::SendResponse(const HTTPResponse *response)
{
	if (!m_connection) return false;

	const size_t contentLength = response->GetContentLength();
	StringBuilder data(contentLength);

	data.Append("HTTP/1.1").Append(' ');
	data.Append(std::to_string(response->GetCode()).c_str()).Append(' ');
	data.Append(response->GetReason()).Append(NewLine);

	for (auto p : response->GetHeaders())
	{
		data.Append(p.first.cstr());
		data.Append(':').Append(' ');
		data.Append(p.second.c_str());

		data.Append(NewLine);
	}

	data.Append(NewLine);
	if (contentLength > 0)
		data.Append(response->GetContent(), contentLength);

	char *const out = data.GetElements();
	const int writelen = (int)data.Size();

	return m_connection->WriteBytes(out, writelen);
}

const char *GetMethodString(int method)
{
	static const char *METHODS[] = {
		"CONNECT",
		"DELETE",
		"GET",
		"HEAD",
		"OPTIONS",
		"POST",
		"PUT",
		"TRACE"
	};

	if (method < 0 || method >= METHOD_COUNT)
		return "UnknownMethod";

	return METHODS[method];
}

int GetMethodFromString(const char *str)
{
	if (equalsIgnoreCase(str, "CONNECT"))
		return METHOD_CONNECT;
	else if (equalsIgnoreCase(str, "DELETE"))
		return METHOD_DELETE;
	else if (equalsIgnoreCase(str, "GET"))
		return METHOD_GET;
	else if (equalsIgnoreCase(str, "HEAD"))
		return METHOD_HEAD;
	else if (equalsIgnoreCase(str, "OPTIONS"))
		return METHOD_OPTIONS;
	else if (equalsIgnoreCase(str, "POST"))
		return METHOD_POST;
	else if (equalsIgnoreCase(str, "PUT"))
		return METHOD_PUT;
	else if (equalsIgnoreCase(str, "TRACE"))
		return METHOD_TRACE;
	return METHOD_NONE;
}
