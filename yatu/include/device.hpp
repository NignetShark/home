//
// Created by paul on 22/03/2021.
//

#ifndef HOME_DEVICE_HPP
#define HOME_DEVICE_HPP


#include <cstdint>
#include <fstream>
#include "aes.hpp"
#include <nlohmann/json.hpp>

#define CMD_DP_QUERY 10
#define CMD_CONTROL 7

typedef struct {
    std::string device_id;
    std::string device_ip;
    std::string local_key;
} device_config_t;

class Device {
private:
    //const char* local_key;
    const char* device_id;
    const char* device_ip;

    struct AES_ctx ctx = {};

    static void sock_read(int sock, char *buffer, unsigned int size);
public:
    explicit Device(device_config_t& config);
    Device(const char* device_id, const char* device_ip, const char local_key[16]);

    static void read_config(std::unordered_map<std::string, device_config_t> &devices, const std::string &config_path);

    std::string parse_message(int sock);

    uint8_t *generate_payload(uint32_t cmd, uint32_t &size, nlohmann::json* data = nullptr);

    uint8_t* generate_message(std::string &payload, uint32_t cmd, uint32_t& size);

    std::string send_and_receive(uint8_t *message, uint32_t size);
};


#endif //HOME_DEVICE_HPP
