//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#pragma once

#ifndef RUBEN_VIDEO_H
#define RUBEN_VIDEO_H

#include <string>
#include <map>

#include "quality.h"
#include "../stream/audio_stream.h"

class Video {
    public:
        explicit Video(const std::string& urlOrId);

        std::string getId() const;
        void setId(std::string newId);

        std::map<std::string, std::string> getInfo();
        std::vector<AudioStream> getStreams();
        std::string getTitle();
        unsigned long getLength();


        void useURL(std::string url);
        bool isSet() const;

        const std::map<std::string, std::string> &getCache() const;
        void setCache(const std::map<std::string, std::string> &newCache);

    private:
        std::string id;
        std::map<std::string, std::string> infoCache;
};

std::map<std::string, std::string> parseQueryData(std::string data);

#endif //RUBEN_VIDEO_H