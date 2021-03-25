//
// Created by paul on 20/03/2021.
//

#include "alexa_server.hpp"


int main() {
    AlexaServer server("127.0.0.1", 5000);
    server.run();
}

