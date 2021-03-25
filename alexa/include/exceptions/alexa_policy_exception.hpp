//
// Created by paul on 21/03/2021.
//

#ifndef HOME_ALEXA_POLICY_EXCEPTION_HPP
#define HOME_ALEXA_POLICY_EXCEPTION_HPP


#include "alexa_exception.hpp"

class AlexaPolicyException : public AlexaException {
public:
    explicit AlexaPolicyException(const std::string &what);
};


#endif //HOME_ALEXA_POLICY_EXCEPTION_HPP
