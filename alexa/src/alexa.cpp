#include <fstream>
#include <exceptions/alexa_processing_exception.hpp>
#include <exceptions/alexa_policy_exception.hpp>
#include "alexa.hpp"

Alexa::Alexa(const std::string& settings_path) : allowed_app_id(), allowed_usr_id() {
    std::ifstream file = std::ifstream(settings_path);
    if(!file.is_open()) throw AlexaProcessingException("Unable to read the settings.");

    nlohmann::json json;
    file >> json;

    for(const auto& element : json["policy"]["allowed_app_id"]) {
        std::string app_id;
        element.get_to(app_id);
        this->allowed_app_id.insert(app_id);
    }

    for(const auto& element : json["policy"]["allowed_usr_id"]) {
        std::string app_id;
        element.get_to(app_id);
        this->allowed_usr_id.insert(app_id);
    }

    file.close();
}

std::string Alexa::get_app_id(nlohmann::json& req) {
    std::string app_id;
    req["session"]["application"]["applicationId"].get_to(app_id);
    return app_id;
}

std::string Alexa::get_usr_id(nlohmann::json &req) {
    std::string usr_id;
    req["session"]["user"]["userId"].get_to(usr_id);
    return usr_id;
}

std::string Alexa::get_request_type(nlohmann::json& req) {
    std::string type;
    req["request"]["type"].get_to(type);
    return type;
}

std::string Alexa::get_intent_name(nlohmann::json& req) {
    std::string name;
    req["request"]["intent"]["name"].get_to(name);
    return name;
}

void Alexa::create_plaintext(std::string& text, nlohmann::json& res) {
    res["type"] = "PlainText";
    res["text"] = text;
}

void Alexa::create_response(std::string text, nlohmann::json& res) {
    nlohmann::json outputSpeech = nlohmann::json::object();
    create_plaintext(text, outputSpeech);

    nlohmann::json res_content = nlohmann::json::object();
    res_content["outputSpeech"] = outputSpeech;
    res_content["shouldEndSession"] = true;

    res["version"] = "1.0";
    res["response"] = res_content;
}

void Alexa::invokeSkill(const std::string& req_body, std::string& res_body) {
    nlohmann::json req_json = nlohmann::json::parse(req_body);
    nlohmann::json res_json = nlohmann::json::object(); // empty object
    std::string app_id = get_app_id(req_json);

    // Allow only known applications and users
    check_policy(req_json);

    std::string req_type = get_request_type(req_json);
    if (req_type == "LaunchRequest") {
        create_response("You invoked this skill with a launch request.", res_json);
    } else if (req_type == "IntentRequest") {
        create_response("Vous avez invoqué la skill avec " + get_intent_name(req_json) + ".", res_json);
    } else {
        create_response("You invoked this skill with an unknown request.", res_json);
    }

    res_body = res_json.dump();
}

void Alexa::check_policy(nlohmann::json &req_json) {
    if (this->allowed_app_id.find(get_app_id(req_json)) == this->allowed_app_id.end()) {
        throw AlexaPolicyException("Application not allowed");
    }

    if (this->allowed_usr_id.find(get_usr_id(req_json)) == this->allowed_usr_id.end()) {
        throw AlexaPolicyException("User not allowed");
    }
}

Alexa &Alexa::get() {
    static Alexa alexa(ALEXA_SETTINGS_PATH);
    return alexa;
}




