//
// Created by paul on 24/03/2021.
//

#ifndef HOME_DEVICE_EXCEPTION_HPP
#define HOME_DEVICE_EXCEPTION_HPP


#include <stdexcept>

class DeviceException : public std::logic_error {
public:
    explicit DeviceException(const std::string& what);
};


#endif //HOME_DEVICE_EXCEPTION_HPP
