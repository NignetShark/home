//
// Created by paul on 21/03/2021.
//

#include <exceptions/alexa_policy_exception.hpp>
#include <exceptions/alexa_processing_exception.hpp>
#include "alexa_server.hpp"

void main_post_request(const httplib::Request &req, httplib::Response &res) {
    std::cout << "New POST request" << std::endl;
    std::cout << req.body << std::endl;
    std::string res_body;

    try {
        Alexa::get().invokeSkill(req.body, res_body);
        res.set_content(res_body, "application/json");
    } catch (AlexaPolicyException const& e) {
        res.status = 401;
        res.set_content("You are not allowed. This incident will be reported.", "text/plain");
        // TODO: Report incident
    } catch (AlexaProcessingException const& e) {
        res.status = 500;
        res.set_content("Alexa skill internal error.", "text/plain");
    } catch (std::exception const& e) {
        res.status = 500;
        res.set_content("Internal error.", "text/plain");
    }
}

void main_get_request(const httplib::Request &req, httplib::Response &res) {
    std::cout << "New GET request" << std::endl;
    res.set_content("The server is up.", "text/plain");
}


AlexaServer::AlexaServer(const char* host, int port) : server(), port(port), host(host) {
}

void AlexaServer::run() {
    std::cout << "Start listening on " << host << ":" << std::to_string(port) << std::endl;
    server.Post("/", main_post_request);
    server.Get("/", main_get_request);
    server.listen(host, port);
}


