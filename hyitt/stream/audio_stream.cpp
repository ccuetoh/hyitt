//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#include <iostream>

#include "audio_stream.h"

AudioStream::AudioStream(const std::string& streamUrl, std::string streamMimeType, AudioQuality audioQuality) {
    quality = audioQuality;
    url = streamUrl;
    mimeType = std::move(streamMimeType);
}

const std::string &AudioStream::getMimeType() const {
    return mimeType;
}

AudioQuality AudioStream::getQuality() const {
    return quality;
}

const std::string &AudioStream::getUrl() const {
    return url;
}

bool AudioStream::isValid() const {
    return !(url.empty() || mimeType.empty() || quality == AudioQuality::None);
}
