//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#ifndef RUBEN_AUDIO_STREAM_H
#define RUBEN_AUDIO_STREAM_H

#include <string>

#include "json/json.h"

#include "../video/quality.h"

class AudioStream {
    public:
        explicit AudioStream(const std::string& url, std::string mimeType, AudioQuality audioQuality);
        AudioStream() { quality = AudioQuality::None; };

        AudioQuality quality;
        std::string url;
        std::string mimeType;

        const std::string &getMimeType() const;
        AudioQuality getQuality() const;
        const std::string &getUrl() const;

        bool isValid() const;
};

#endif //RUBEN_AUDIO_STREAM_H
