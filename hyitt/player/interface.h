//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#ifndef RUBEN_INTERFACE_H
#define RUBEN_INTERFACE_H


#include <condition_variable>
#include "player.h"
#include "../video/video.h"

class Interface {
    public:
        explicit Interface(Video video, Player player);
        void start();
        void draw();
        static std::string prettyTruncate(std::string str, int max = 50, std::string fadeOff = "...");
        static std::string prettyPosition(int secs);
        static std::string getProgressBar(int position, int length, int status);

    private:
        Player player;
        Video video;
};


#endif //RUBEN_INTERFACE_H
