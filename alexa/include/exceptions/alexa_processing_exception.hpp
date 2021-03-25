//
// Created by paul on 21/03/2021.
//

#ifndef HOME_ALEXA_PROCESSING_EXCEPTION_HPP
#define HOME_ALEXA_PROCESSING_EXCEPTION_HPP


#include "alexa_exception.hpp"

class AlexaProcessingException : public AlexaException {
public:
    AlexaProcessingException(const std::string &what);
};


#endif //HOME_ALEXA_PROCESSING_EXCEPTION_HPP
