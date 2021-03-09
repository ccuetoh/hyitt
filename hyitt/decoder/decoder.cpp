//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#include <iostream>
#include <regex>

#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string_regex.hpp"
#include "boost/regex.h"
#include "Duktape/duktape.h"

#include "decoder.h"
#include "../exceptions.h"
#include "../video/video.h"

static const boost::regex FUNC_NAME_REGEX[] {
        boost::regex(R"(\b[cs]\s*&&\s*[adf]\.set\([^,]+\s*,\s*encodeURIComponent\s*\(\s*(?<sig>[a-zA-Z0-9$]+)\()"),
        boost::regex(R"(\b[a-zA-Z0-9]+\s*&&\s*[a-zA-Z0-9]+\.set\([^,]+\s*,\s*encodeURIComponent\s*\(\s*(?<sig>[a-zA-Z0-9$]+)\()"),
        boost::regex(R"(\bm=(?<sig>[a-zA-Z0-9$]{2})\(decodeURIComponent\(h\.s\)\))"),
        boost::regex(R"(\bc&&\(c=(?<sig>[a-zA-Z0-9$]{2})\(decodeURIComponent\(c\)\))"),
        boost::regex(R"((?:\b|[^a-zA-Z0-9$])(?<sig>[a-zA-Z0-9$]{2})\s*=\s*function\(\s*a\s*\)\s*{\s*a\s*=\s*a\.split\(\s*""\s*\);[a-zA-Z0-9$]{2}\.[a-zA-Z0-9$]{2}\(a,\d+\))"),
        boost::regex(R"((?:\b|[^a-zA-Z0-9$])(?<sig>[a-zA-Z0-9$]{2})\s*=\s*function\(\s*a\s*\)\s*{\s*a\s*=\s*a\.split\(\s*""\s*\))"),
        boost::regex(R"((?<sig>[a-zA-Z0-9$]+)\s*=\s*function\(\s*a\s*\)\s*{\s*a\s*=\s*a\.split\(\s*""\s*\))"),
};

Decoder::Decoder(const std::string& vidId) {
    videoId = vidId;

    playerUrl = getPlayerUrl();
    playerJs = getPlayer();
    sigFuncName = getSigFuncName();
    sigFuncDef = findSigFunc();
    helperFuncsDef = findHelperFuncs();
}

std::string Decoder::decode(const std::string &signatureCipher) {
    duk_context *duk_ctx = duk_create_heap_default();

    auto sigData = parseQueryData(signatureCipher);
    std::string s = sigData["s"];
    std::string sp = sigData["sp"];
    std::string url = sigData["url"];

    duk_eval_string(duk_ctx, jsCallString(s).c_str());
    std::string res = duk_get_string(duk_ctx, -1);

    duk_destroy_heap(duk_ctx);

    url = curl_easy_unescape(curl_easy_init(), url.c_str(), 0, nullptr);
    return url + "&" + sp + "=" + res;
}

std::string Decoder::getPlayerUrl() {
    const std::string URL = "https://www.youtube.com/embed/" + videoId;

    cpr::Session session;
    session.SetVerifySsl(false);
    session.SetUrl(URL);

    cpr::Response r = session.Get();
    if (r.status_code != 200) {
        throw NetworkException(r.status_code);
    }

    std::string html = r.text;
    std::string jsUrl = Decoder::getJsUrl(html);

    return "https://www.youtube.com/" + jsUrl;
}

std::string Decoder::getJsUrl(const std::string& html) {
    std::vector<std::string> urlStart;
    boost::algorithm::split_regex(urlStart, html, boost::regex(R"("jsUrl":")")) ;
    if (urlStart.size() < 2) {
        throw SignatureException();
    }

    std::vector<std::string> url;
    boost::split(url, urlStart[1].substr(1, urlStart[0].size()), boost::is_any_of("\""));
    if (url.size() < 2){
        throw SignatureException();
    }

    return url[0];
}

std::string Decoder::getPlayer() {
    cpr::Session session;
    session.SetVerifySsl(false);
    session.SetUrl(playerUrl);

    cpr::Response r = session.Get();
    if (r.status_code != 200) {
        throw NetworkException(r.status_code);
    }

    return r.text;
}

std::string Decoder::getSigFuncName() {
    boost::smatch m;
    for (auto &exp: FUNC_NAME_REGEX) {
        if (boost::regex_search(playerJs, m, exp)) {
            return m["sig"];
        }
    }

    throw SignatureException();
}

std::string Decoder::findSigFunc() {
    auto term = sigFuncName + R"(=function\(a\)\{)";

    std::vector<std::string> sects;
    boost::split_regex(sects, playerJs, boost::regex(term));
    if (sects.size() < 2) {
        throw SignatureException();
    }

    std::vector<std::string> forwardSects;
    boost::split_regex(forwardSects, sects[1], boost::regex("\\};"));
    if (forwardSects.size() < 2) {
        throw SignatureException();
    }

    return sigFuncName + "=function(a){" + forwardSects[0] + "};";
}

std::string Decoder::findHelperFuncs() {
    std::vector<std::string> sects;
    boost::split_regex(sects, sigFuncDef, boost::regex(R"(a=a.split\(""\);)"));
    if (sects.size() < 2) {
        throw SignatureException();
    }

    auto varName = sects[1].substr(0, 2);
    auto term = "var " + varName + "=\\{";

    std::vector<std::string> declSects;
    boost::split_regex(declSects, playerJs, boost::regex(term));
    if (sects.size() < 2) {
        throw SignatureException();
    }

    std::vector<std::string> forwardSects;
    boost::split_regex(forwardSects, declSects[1], boost::regex("\\};"));
    if (forwardSects.size() < 2) {
        throw SignatureException();
    }

    return "var " + varName + "={" + forwardSects[0] + "};";
}

std::string Decoder::jsCallString(const std::string& s) {
    return helperFuncsDef+sigFuncDef+sigFuncName+"(decodeURIComponent(\"" + s + "\"))";
}
