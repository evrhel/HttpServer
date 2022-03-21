#pragma once

#include "http_connection.h"

HTTPResponse *HandleGETRequest(const HTTPRequest *request);

HTTPResponse *HandleOPTIONSRequest(const HTTPRequest *request);

HTTPResponse *HandlePOSTRequest(const HTTPRequest *request);