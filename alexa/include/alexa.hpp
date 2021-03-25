#ifndef ALEXA_H
#define ALEXA_H

// a simple json library
//#include "simple_json_cpp/JSONObject.h"
//#include "simple_json_cpp/JSONArray.h"
#include <set>
#include "nlohmann/json.hpp"


#define ALEXA_SETTINGS_PATH "../settings/sensitive/alexa_settings.json"

class Alexa {
private:
    std::set<std::string> allowed_usr_id;
    std::set<std::string> allowed_app_id;

    static std::string get_app_id(nlohmann::json& req);
    static std::string get_usr_id(nlohmann::json& req);
    static std::string get_request_type(nlohmann::json& req);
    static std::string get_intent_name(nlohmann::json& req);
    static void create_plaintext(std::string& text, nlohmann::json& res);
    static void create_response(std::string text, nlohmann::json& res);

    void check_policy(nlohmann::json& req_json);
    Alexa(const std::string& settings_path);

public:
    static Alexa& get();
    void invokeSkill(const std::string& req_body, std::string& res_body);
};

#endif