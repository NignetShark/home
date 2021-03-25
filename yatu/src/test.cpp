//
// Created by paul on 22/03/2021.
//


#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <device.hpp>
#include <ctime>
#include "aes.hpp"

typedef union {
    struct { // Big Endian !!!!!
        uint32_t magic;
        uint32_t seq_no;
        uint32_t cmd;
        uint32_t length;
        uint32_t ret_code;
    };
    char data[5 * sizeof(uint32_t)];
} header_recv_t;

std::unordered_map<std::string, device_config_t> devices;


int main() {
    Device::read_config(devices, "../../settings/sensitive/tuya_keys.json");

    Device device(devices["cuisine"]);
    //Device device("device_id", "device_ip", "local_key");

    uint32_t size;
    std::string recv_payload;

    uint8_t* status = device.generate_payload(CMD_DP_QUERY, size);
    recv_payload = device.send_and_receive(status, size);
    std::cout << recv_payload << std::endl;
    delete status;

    nlohmann::json send_payload = nlohmann::json::parse(R"({"20":true,"21":"white","22":1000})");
    uint8_t* data = device.generate_payload(7, size, &send_payload);

    recv_payload = device.send_and_receive(data, size);
    std::cout << recv_payload << std::endl;
    delete data;
}

