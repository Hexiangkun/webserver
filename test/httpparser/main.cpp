#include <stdio.h>
#include "code/net/http/llhttp.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>


int on_message_begin(llhttp_t* _) {
  (void)_;
  printf("\n***MESSAGE BEGIN***\n\n");
  return 0;
}

int on_headers_complete(llhttp_t* _) {
  (void)_;
  printf("\n***HEADERS COMPLETE***\n\n");
  return 0;
}

int on_message_complete(llhttp_t* _) {
  (void)_;
  printf("\n***MESSAGE COMPLETE***\n\n");
  return 0;
}

int on_url(llhttp_t* _, const char* at, size_t length) {
    (void)_;
    printf("Url: %s\n", at);
    printf("Url: %.*s\n", (int)length, at);
    return 0;
}

int on_header_field(llhttp_t* _, const char* at, size_t length) {
  (void)_;
  printf("Header field: %.*s\n", (int)length, at);
  return 0;
}

int on_header_value(llhttp_t* _, const char* at, size_t length) {
  (void)_;
  printf("Header value: %.*s\n", (int)length, at);
  return 0;
}

int on_body(llhttp_t* _, const char* at, size_t length) {
  (void)_;
  printf("Body: %.*s\n", (int)length, at);
  return 0;
}


int main()
{
    const char *buf;
    int i;
    float start, end;
    size_t parsed;

    llhttp_t parser;
	llhttp_settings_t settings;

	/*Initialize user callbacks and settings */
	llhttp_settings_init(&settings);

    settings.on_message_begin = on_message_begin;
    settings.on_header_field = on_header_field;
    settings.on_header_value = on_header_value;
    settings.on_url = on_url;
    settings.on_body = on_body;
    settings.on_headers_complete = on_headers_complete;
    settings.on_message_complete = on_message_complete;

    buf = "GET http://admin.omsg.cn/uploadpic/2016121034000012.png HTTP/1.1\r\nHost:\tadmin.omsg.cn\r\nAccept: */*\r\nConnection: Keep-Alive\r\n\r\n";

    start = (float)clock()/CLOCKS_PER_SEC;
    for (i = 0; i < 1; i++) {
        llhttp_init(&parser, HTTP_REQUEST, &settings);  
        enum llhttp_errno err  = llhttp_execute(&parser, buf, strlen(buf)); 
        if (err == HPE_OK) {
		    fprintf(stdout, "Successfully parsed!\n");
        } else {
            fprintf(stderr, "Parse error: %s %s\n", llhttp_errno_name(err), parser.reason);
        }
    }
    end = (float)clock()/CLOCKS_PER_SEC;
    printf("Elapsed %f seconds.\n", (end - start));

    buf="HTTP/1.1 200 OK\r\n"
        "Date: Tue, 04 Aug 2009 07:59:32 GMT\r\n"
        "Server: Apache\r\n"
        "X-Powered-By: Servlet/2.5 JSP/2.1\r\n"
        "Content-Type: text/xml; charset=utf-8\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
        "  <SOAP-ENV:Body>\n"
        "    <SOAP-ENV:Fault>\n"
        "       <faultcode>SOAP-ENV:Client</faultcode>\n"
        "       <faultstring>Client Error</faultstring>\n"
        "    </SOAP-ENV:Fault>\n"
        "  </SOAP-ENV:Body>\n"
        "</SOAP-ENV:Envelope>";

    llhttp_init(&parser, HTTP_RESPONSE,&settings); 
    enum llhttp_errno err  = llhttp_execute(&parser, buf, strlen(buf)); 
    if (err == HPE_OK) {
        fprintf(stdout, "Successfully parsed!\n");
    } else {
        fprintf(stderr, "Parse error: %s %s\n", llhttp_errno_name(err), parser.reason);
    }
    return 0;
}