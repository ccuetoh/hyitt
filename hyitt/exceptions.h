//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#ifndef RUBEN_EXCEPTIONS_H
#define RUBEN_EXCEPTIONS_H

#include "boost/lexical_cast.hpp"

struct BassException : public std::exception
{
    int err_code;

    explicit BassException(int code) { err_code = code; }

    std::string what() {
        return "Unable to start audio playback (BASS ERR " + boost::lexical_cast<std::string>(err_code) + ")";
    }
};

struct NoStreamsException : public std::exception
{
    std::string what() {
        return "No suitable streams found or invalid video";
    }
};

struct SignatureException : public std::exception
{
    std::string what() {
        return "Unable to retrieve the video's signature data";
    }
};

struct NetworkException : public std::exception
{
    int err_code;

    explicit NetworkException(int code) { err_code = code; }

    std::string what() {
        return "Unable to establish connection (HTTP ERR " + boost::lexical_cast<std::string>(err_code) + ")";
    }
};


#endif //RUBEN_EXCEPTIONS_H
