//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#ifndef RUBEN_DECODER_H
#define RUBEN_DECODER_H

#include <string>

#include "cpr/cpr.h"
#include "boost/format.hpp"

class Decoder {
    public:
        explicit Decoder() = default;
        explicit Decoder(const std::string& vidId);
        std::string decode(const std::string& signatureCipher);

    private:
        std::string videoId;
        std::string playerUrl;
        std::string playerJs;
        std::string sigFuncName;
        std::string sigFuncDef;
        std::string helperFuncsDef;

        std::string getPlayerUrl();
        static std::string getJsUrl(const std::string& html);
        std::string getPlayer();

        std::string getSigFuncName();
        std::string findSigFunc();
        std::string findHelperFuncs();

        std::string jsCallString(const std::string& s);
};


#endif //RUBEN_DECODER_H
