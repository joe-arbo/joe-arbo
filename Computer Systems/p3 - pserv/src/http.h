#ifndef _HTTP_H
#define _HTTP_H

#include <jwt.h>
#include <stdbool.h>

#include "buffer.h"
struct bufio;

enum http_method {
    HTTP_GET,
    HTTP_POST,
    HTTP_UNKNOWN
};

enum http_version {
    HTTP_1_0,
    HTTP_1_1
};

enum http_response_status {
    HTTP_OK = 200,
    HTTP_PARTIAL_CONTENT = 206,
    HTTP_BAD_REQUEST = 400,
    HTTP_PERMISSION_DENIED = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_REQUEST_TIMEOUT = 408,
    HTTP_REQUEST_TOO_LONG = 414,
    HTTP_INTERNAL_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501,
    HTTP_SERVICE_UNAVAILABLE = 503
};

struct http_transaction {
    /* request related fields */
    enum http_method req_method;
    enum http_version req_version;
    size_t req_path;        // expressed as offset into the client's bufio.
    size_t req_body;        // ditto
    int req_content_len;    // content length of request body

    char* req_cookie;
    char* req_connection;
    bool reqRange;
    int reqRangeStart;
    int reqRangeEnd;

    /* response related fields */
    enum http_response_status resp_status;
    buffer_t resp_headers;
    buffer_t resp_body;

    struct http_client *client;
};

struct http_client {
    struct bufio *bufio;
};

void http_setup_client(struct http_client *, struct bufio *bufio);
bool http_handle_transaction(struct http_client *);
void http_add_header(buffer_t * resp, char* key, char* fmt, ...);

#endif /* _HTTP_H */
