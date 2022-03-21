#include "request_handlers.h"

#include "http_server.h"

static constexpr char HTMLFormat[] =
"<!DOCTYPE html>"
"<html>"
"<head>"
"<title>404 Not Found</title>"
"</head>"
"<body>"
"<div>"
"<h1>404 Not Found</h1>"
"<p>For resource: %s</p>"
"</div>"
"</body>"
"</html>";


HTTPResponse *HandleGETRequest(const HTTPRequest *request)
{
	HTTPServer *server = (HTTPServer *)request->GetSource()->GetHTTPServer();
	
	const URI &uri = request->GetURI();
	HTTPResource *rsrc = server->FindHTTPResource(uri.GetPath());

	//printf("Requested resource %s (physical location %s)\n", uri.GetPath().c_str(), rsrc ? rsrc->GetLocation().c_str() : "unknown");

	HTTPResponse *res = new HTTPResponse();

	if (!rsrc || !rsrc->Request())
	{
		res->SetCode(404);
		res->SetReason("Not Found");

		constexpr char NotFoundText[] = "404 Not Found";
		res->AppendContent(NotFoundText, sizeof(NotFoundText) - 1);
		res->SetContentType("text/plain");

		return res;
	}

	const char *data = rsrc->GetData();
	size_t len = rsrc->GetDataLength();
	const char *contentType = rsrc->GetContentType();

	res->SetCode(200);
	res->SetReason("OK");
	res->AppendContent(data, len);
	res->SetContentType(contentType);

	rsrc->Done();

	return res;
}

HTTPResponse *HandleOPTIONSRequest(const HTTPRequest *request)
{
	HTTPServer *server = (HTTPServer *)request->GetSource()->GetHTTPServer();

	HTTPResponse *response = new HTTPResponse();

	const std::string *origin = request->GetHeader("Origin");
	if (origin)
	{
		response->AddHeader("Access-Control-Allow-Origin", *origin);
		response->AddHeader("Vary", "Origin");
	}
	else
		response->AddHeader("Access-Control-Allow-Origin", "*");

	server->GenerateAllowHeader(response);

	//response->AddHeader("Access-Control-Allow-Origin", "*");

	response->SetCode(RESP_NO_CONTENT);
	response->SetReason("No Content");

	return response;
}

HTTPResponse *HandlePOSTRequest(const HTTPRequest *request)
{
	HTTPServer *server = (HTTPServer *)request->GetSource()->GetHTTPServer();

	HTTPResponse *response = new HTTPResponse();

	response->SetCode(RESP_CREATED);
	response->SetReason("Created");

	constexpr const char ResponseContent[] = "{\"success\":\"true\"}";
	response->AppendContent(ResponseContent, sizeof(ResponseContent) - 1);

	response->SetContentType("application/json");

	return response;
}
