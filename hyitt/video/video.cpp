//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#include <string>
#include <map>
#include <iostream>
#include <vector>

#include "json/json.h"
#include "boost/algorithm/string.hpp"
#include "cpr/cpr.h"

#include "video.h"
#include "../decoder/decoder.h"
#include "../exceptions.h"

Video::Video(const std::string& urlOrId) {
    if (urlOrId.find("youtube") != std::string::npos) {
        useURL(urlOrId);
    } else {
        setId(urlOrId);
    }
}

std::string Video::getId() const {
    return id;
}

void Video::setId(std::string newId) {
    id = std::move(newId);
}

void Video::useURL(std::string url) {
    std::vector<std::string> urlSplits;
    boost::split(urlSplits, url, boost::is_any_of("?"));

    std::map<std::string, std::string> values = parseQueryData(urlSplits[urlSplits.size()-1]);

    for(auto & value : values) {
        if (value.first == "v") {
            setId(value.second);
        }
    }
}

bool Video::isSet() const {
    if (getId().empty()) {
        return false;
    }

    return true;
}

std::map<std::string, std::string> Video::getInfo() {
    const std::string URL = "https://www.youtube.com/get_video_info?video_id=" + getId();

    cpr::Session session;
    session.SetUrl(URL);
    session.SetVerifySsl(false);

    cpr::Response r = session.Get();
    if (r.status_code != 200) {
        throw NetworkException(r.status_code);
    }

    auto outer = parseQueryData(r.text);
    std::map<std::string, std::string> result;

    for(auto & value : outer) {
        if (value.first == "player_response") {
            char *unescapedCStr = curl_easy_unescape(curl_easy_init(), value.second.c_str(), 0, nullptr);
            std::string unescaped = std::string(unescapedCStr);

            result.insert({ value.first, unescaped });
        } else if (value.first == "fflags") {
            char *unescapedCStr = curl_easy_unescape(curl_easy_init(), value.second.c_str(), 0, nullptr);
            std::string unescaped = std::string(unescapedCStr);

            std::map<std::string, std::string> inner = parseQueryData(unescaped);
            for (auto & innerVal:  inner){
                result.insert({ value.first+"."+innerVal.first, innerVal.second });
            }
        } else {
            result.insert( { value.first, value.second } );
        }
    }

    if (getCache().empty()) {
        setCache(result);
    }

    return result;
}

const std::map<std::string, std::string> &Video::getCache() const {
    return infoCache;
}

void Video::setCache(const std::map<std::string, std::string> &newCache) {
    Video::infoCache = newCache;
}

std::vector<AudioStream> Video::getStreams() {
    std::map<std::string, std::string> info;
    if (getCache().empty()) {
        info = getInfo();
    } else {
        info = getCache();
    }

    std::vector<AudioStream> streams;

    auto it = info.find("player_response");
    if (it == info.end()) {
        throw NoStreamsException();
    }

    Json::Value playerResponse;
    std::stringstream (it->second) >> playerResponse;

    Json::Value formats = playerResponse["streamingData"]["adaptiveFormats"];
    if (formats.isNull()) {
        throw NoStreamsException();
    }

    bool useSig = false;
    if (formats[0]["url"].isNull()) {
        useSig = true;
    }

    Decoder decoder;
    if (useSig) {
        decoder = Decoder(getId());
    }

    for (auto & format : formats) {
        AudioQuality quality = toAudioQuality(format["audioQuality"].asString());

        std::string url;
        if (useSig) {
            url = decoder.decode(format["signatureCipher"].asString());
        } else {
            url = format["url"].asString();
        }

        std::string rawMime = format["mimeType"].asString();
        std::vector<std::string> parts;
        boost::split(parts, rawMime, boost::is_any_of(";"));

        std::string mimeType;
        if (parts.size() > 1) {
            mimeType = parts[0];
        }

        auto stream = AudioStream(url, mimeType, quality);
        if (!stream.isValid()) {
            continue;
        }

        streams.emplace_back(stream);
    }

    return streams;
}

std::string Video::getTitle() {
    std::map<std::string, std::string> info;
    if (getCache().empty()) {
        info = getInfo();
    } else {
        info = getCache();
    }

    auto it = info.find("player_response");
    if (it == info.end()) {
        throw NoStreamsException();
    }

    Json::Value playerResponse;
    std::stringstream (it->second) >> playerResponse;

    std::string title = playerResponse["videoDetails"]["title"].asString();
    boost::replace_all(title, "+", " ");

    return title;
}

unsigned long Video::getLength() {
    std::map<std::string, std::string> info;
    if (getCache().empty()) {
        info = getInfo();
    } else {
        info = getCache();
    }

    auto it = info.find("player_response");
    if (it == info.end()) {
        throw NoStreamsException();
    }

    Json::Value playerResponse;
    std::stringstream (it->second) >> playerResponse;

    return stoi(playerResponse["videoDetails"]["lengthSeconds"].asString());
}

std::map<std::string, std::string> parseQueryData(std::string data) {
    std::vector<std::string> params;
    boost::split(params, data, boost::is_any_of("&")); // Get the parameter pairs ex.: v=oYwGUa2tzL3

    std::map<std::string, std::string> results;

    for (std::string param: params) {
        std::vector<std::string> valMappings;
        boost::split(valMappings, param, boost::is_any_of("=")); // Get key-value mappings: [v, oYwGUa2tzL3]

        if (valMappings.size() != 2) {
            continue;
        }

        results.insert({valMappings[0], valMappings[1]});
    }

    return results;
}