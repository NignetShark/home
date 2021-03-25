//
// Created by paul on 22/03/2021.
//

#include "device.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <crc32/crc32.hpp>
#include <cstring>

#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <thread>
#include <exception/device_exception.hpp>


#define AES_BLOCK_SIZE 16
#define MAGIC_HEADER 0x000055aa
#define MAGIC_FOOTER 0x0000aa55
#define DEVICE_PORT 6668
#define PROTOCOL_VERSION_BYTES_33 "3.3"

static const char PROTOCOL_33_HEADER[] = {'3', '.', '3', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

typedef union {
    struct { // Big Endian !!!!!
        uint32_t magic;
        uint32_t seq_no;
        uint32_t cmd;
        uint32_t length;
    };
    char data[4 * sizeof(uint32_t)];
} header_send_t;

typedef union {
    struct {
        uint32_t crc;
        uint32_t magic;
    };
    char data[2 * sizeof(uint32_t)];
} footer_t;

void dump(char buff[], size_t size) {
    for (int i = 0; i < size; i++) {
        std::cout << buff[i];
    }
    std::cout << std::endl;
}

void dump_hex(uint8_t buff[], size_t size) {
    for (int i = 0; i < size; i++) {
        //std::cout << std::hex << buff[i] << " ";
        printf("%x ", buff[i]);
    }
    std::cout << std::endl;
}

Device::Device(const char *device_id, const char *device_ip, const char local_key[AES_BLOCK_SIZE]) : device_id(
        device_id), device_ip(device_ip) {
    AES_init_ctx(&this->ctx, (const uint8_t *) local_key);
}


Device::Device(device_config_t &config) : device_ip(config.device_ip.c_str()), device_id(config.device_id.c_str()) {
    AES_init_ctx(&this->ctx, (const uint8_t *) config.local_key.c_str());
}

void Device::sock_read(int sock, char *buffer, unsigned int size) {
    int recv_size = recv(sock, buffer, size, 0);
    if (recv_size != size)
        throw DeviceException("Unable to read TCP socket : required " + std::to_string(size) + " bytes got " +
                              std::to_string(recv_size) + " bytes");
}

std::string Device::parse_message(int sock) {
    header_recv_t header;

    sock_read(sock, header.data, sizeof(header_recv_t));

    // Add to crc before big / little endian conversion.
    uint32_t crc = CRC32::get().update(0, header.data, sizeof(header_recv_t));

    // Convert big endian to hardware endianness
    header.seq_no = ntohl(header.seq_no);
    header.cmd = ntohl(header.cmd);
    header.length = ntohl(header.length);
    header.ret_code = ntohl(header.ret_code);

    // Get payload
    size_t payload_size = header.length - sizeof(uint32_t) - sizeof(footer_t);
    char payload[payload_size];
    sock_read(sock, payload, payload_size);

    // Update CRC
    crc = CRC32::get().update(crc, payload, payload_size);

    // Get footer
    footer_t footer;
    sock_read(sock, footer.data, sizeof(footer_t));

    // Check CRC
    if (crc != ntohl(footer.crc)) throw std::logic_error("Checksum failed");


    uint8_t *start_payload_ptr = (uint8_t *) payload;

    if (strncmp(payload, PROTOCOL_VERSION_BYTES_33, strlen(PROTOCOL_33_HEADER)) == 0) {
        start_payload_ptr += sizeof(PROTOCOL_33_HEADER);
        payload_size -= sizeof(PROTOCOL_33_HEADER);
    }

    // Decrypt payload
    if (payload_size % AES_BLOCK_SIZE != 0) throw std::logic_error("Payload must be a multiple of the key size.");

    uint8_t *payload_ptr = start_payload_ptr;
    for (int i = 0; i < (payload_size / AES_BLOCK_SIZE); i++) {
        AES_ECB_decrypt(&this->ctx, payload_ptr);
        payload_ptr += AES_BLOCK_SIZE;
    }


    // Return payload without padding
    return std::string((char *) start_payload_ptr, payload_size - (uint8_t) start_payload_ptr[payload_size - 1]);
}

uint8_t *Device::generate_message(std::string &payload, uint32_t cmd, uint32_t &size) {
    bool use_33 = (cmd != CMD_DP_QUERY);
    uint8_t padding_cnt = AES_BLOCK_SIZE - (payload.size() % AES_BLOCK_SIZE);

    size = payload.size() + padding_cnt + sizeof(header_send_t) + sizeof(footer_t);
    if (use_33) size += sizeof(PROTOCOL_33_HEADER);

    uint8_t *data = new uint8_t[size];

    // Add header
    header_send_t *header = (header_send_t *) ((void *) data);
    header->magic = htonl(MAGIC_HEADER);
    header->cmd = htonl(cmd);
    header->seq_no = htonl(3); // TODO change
    header->length = htonl(size - sizeof(header_send_t));


    // Copy payload string to data
    uint8_t *payload_ptr = (uint8_t *) ((void *) data) + sizeof(header_send_t);

    if (use_33) {
        // Add 3.3 header before payload
        std::memcpy(payload_ptr, PROTOCOL_33_HEADER, sizeof(PROTOCOL_33_HEADER));
        payload_ptr += sizeof(PROTOCOL_33_HEADER);
    }

    std::memcpy(payload_ptr, payload.c_str(), payload.length());

    // Add padding
    for (uint8_t *ptr = payload_ptr + payload.size(); ptr < payload_ptr + payload.size() + padding_cnt; ptr++) {
        *ptr = padding_cnt;
    }

    // Payload encryption
    for (int i = 0; i < (payload.size() + padding_cnt) / AES_BLOCK_SIZE; i++) {
        AES_ECB_encrypt(&this->ctx, payload_ptr);
        payload_ptr += AES_BLOCK_SIZE;
    }

    // TODO: ajouter "3.3" avant les données chiffrées

    // Compute CRC
    uint32_t crc = CRC32::get().update(0, data, size - sizeof(footer_t));

    // Add footer
    footer_t *footer = (footer_t *) ((void *) payload_ptr);
    footer->magic = htonl(MAGIC_FOOTER);
    footer->crc = htonl(crc);

    return data;
}

uint8_t *Device::generate_payload(uint32_t cmd, uint32_t &size, nlohmann::json *data) {
    nlohmann::json json_payload = nlohmann::json::object();
    //json_payload["gwId"] = this->device_id;
    json_payload["devId"] = this->device_id;
    json_payload["uid"] = this->device_id;
    json_payload["t"] = std::to_string(std::time(0));

    if (data != nullptr) {
        json_payload["dps"] = *data;
    }

    std::string payload = json_payload.dump();

    std::cout << "Send : " << payload << std::endl;
    return generate_message(payload, cmd, size);
}

std::string Device::send_and_receive(uint8_t *message, uint32_t size) {
    int sock;
    struct sockaddr_in device_addr = {};
    device_addr.sin_family = AF_INET;
    device_addr.sin_port = htons(DEVICE_PORT);
    if (inet_pton(AF_INET, this->device_ip, &device_addr.sin_addr) <= 0) throw DeviceException("Invalid address");
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) throw DeviceException("Sock creation failed");
    if (connect(sock, (struct sockaddr *) &device_addr, sizeof(device_addr)) < 0)
        throw DeviceException("Connection failed");
    if (send(sock, message, size, 0) != size) throw DeviceException("Send failed");

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return parse_message(sock);
}

void Device::read_config(std::unordered_map<std::string, device_config_t> &devices, const std::string &config_path) {
    std::ifstream file = std::ifstream(config_path);
    if(!file.is_open()) throw DeviceException("Unable to open config file.");

    nlohmann::json root = nlohmann::json::array();
    file >> root;
    file.close();

    for(int i = 0; i < root.size(); i++) {
        devices[root[i]["name"]] = {.device_id = root[i]["id"], .device_ip = root[i]["ip"], .local_key=root[i]["key"]};
    }
}



