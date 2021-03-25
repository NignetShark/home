//
// Created by paul on 21/03/2021.
//

#ifndef HOME_ALEXA_SERVER_HPP
#define HOME_ALEXA_SERVER_HPP


#include <string>
#include <http/httplib.h>
#include "alexa.hpp"

class AlexaServer {
private:
    httplib::Server server;
    const int port;
    const char* host;

public:
    explicit AlexaServer(const char* host, int port);
    void run();
};

void main_post_request(const httplib::Request &req, httplib::Response &res);

#endif //HOME_ALEXA_SERVER_HPP
