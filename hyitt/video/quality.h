//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#pragma once

#ifndef RUBEN_QUALITY_H
#define RUBEN_QUALITY_H

#include <string>

enum AudioQuality {
    None,
    Low,
    Medium,
    High
};

static AudioQuality toAudioQuality(const std::string& str) {
    if (str == "AUDIO_QUALITY_LOW") {
        return AudioQuality::Low;
    } else if (str == "AUDIO_QUALITY_MEDIUM") {
        return AudioQuality::Medium;
    } else if (str == "AUDIO_QUALITY_HIGH") {
        return AudioQuality::High;
    } else {
        return AudioQuality::None;
    }
}

static std::ostream& operator<<(std::ostream& os, AudioQuality q)
{
    switch(q) {
        case High   :
            os << "High";
            break;
        case Medium:
            os << "Medium";
            break;
        case Low :
            os << "Low";
            break;
        default  :
            os << "None";
            break;
    }
    return os;
}

#endif //RUBEN_QUALITY_H
