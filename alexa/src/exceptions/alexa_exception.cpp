//
// Created by paul on 21/03/2021.
//

#include "exceptions/alexa_exception.hpp"

AlexaException::AlexaException(const std::string &what) : std::logic_error(what){

}
