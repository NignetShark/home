//
// Created by paul on 21/03/2021.
//

#ifndef HOME_ALEXA_EXCEPTION_HPP
#define HOME_ALEXA_EXCEPTION_HPP

#include <stdexcept>

class AlexaException : public std::logic_error {
public:
    explicit AlexaException(const std::string& what);
};


#endif //HOME_ALEXA_EXCEPTION_HPP
