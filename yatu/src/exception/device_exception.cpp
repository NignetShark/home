//
// Created by paul on 24/03/2021.
//

#include "exception/device_exception.hpp"

DeviceException::DeviceException(const std::string &what) : std::logic_error(what) {

}
