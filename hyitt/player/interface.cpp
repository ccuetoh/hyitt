//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#include <utility>
#include <iostream>
#include <windows.h>

#include "interface.h"

#include "boost/thread.hpp"

Interface::Interface(Video video, Player player) : player(std::move(player)), video(std::move(video)) { }

void Interface::start() {
    boost::thread t1([&]{
        player.waitResume(); // Wait until the video starts
        short prevPauseKeyState = max(GetKeyState(VK_MEDIA_PLAY_PAUSE), 0);
        short prevPrevKeyState = max(GetKeyState(VK_MEDIA_PREV_TRACK), 0);
        short prevNextKeyState = max(GetKeyState(VK_MEDIA_NEXT_TRACK), 0);

        while (player.isActive()) {
            draw();

            short currentPauseKeyState = max(GetKeyState(VK_MEDIA_PLAY_PAUSE), 0);
            bool isPauseKeyStateChanged = prevPauseKeyState != currentPauseKeyState;
            prevPauseKeyState = currentPauseKeyState;

            if (isPauseKeyStateChanged) { // Media Pause/Play - Pause/Play
                if (player.isPlaying()) {
                    player.pause();
                } else {
                    player.resume();
                }
            }

            short currentPrevKeyState = max(GetKeyState(VK_MEDIA_PREV_TRACK), 0);
            bool isPrevKeyStateChanged = prevPrevKeyState != currentPrevKeyState;
            prevPrevKeyState = currentPrevKeyState;

            if (isPrevKeyStateChanged) { // Media Prev - Rewind 30
                player.rewind(30);
            }

            short currentNextKeyState = max(GetKeyState(VK_MEDIA_NEXT_TRACK), 0);
            bool isNextKeyStateChanged = prevNextKeyState != currentNextKeyState;
            prevNextKeyState = currentNextKeyState;

            if (isNextKeyStateChanged) { // Media Next - Advance 30
                player.advance(30);
            }

            // boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
        }
    });

    Player::init();

    player.play();
    player.wait();

    Player::cleanup();
}

void Interface::draw() {
    std::cout <<
        Interface::prettyPosition(player.getPosition()) <<  "/" << Interface::prettyPosition(video.getLength())
        << "  " << Interface::getProgressBar(player.getPosition(), video.getLength(), player.getState())
        << "  " << Interface::prettyTruncate(video.getTitle())
        << '\r' << std::flush;
}

std::string Interface::prettyTruncate(std::string str, int max, std::string fadeOff) {
    if (str.length() < max) {
        return str;
    }

    int cut = (max - int(fadeOff.length())) - 1;
    if (cut >= str.length()) {
        return fadeOff;
    }

    return str.substr(0, cut) + fadeOff;
}

std::string Interface::prettyPosition(int secs) {
    const int HOUR =  3600;
    const int MIN = 60;

    int hrs = secs / HOUR;
    int remain = secs % HOUR;
    int mins = remain / MIN;
    remain %= MIN;

    std::string res;
    if (int(hrs) > 0) {
        std::string str_hrs = std::to_string(hrs);
        if (str_hrs.length() < 2) {
            res += "0" + str_hrs + ":";
        } else {
            res += str_hrs + ":";
        }

    }

    std::string str_mins = std::to_string(mins);
    if (str_mins.length() < 2) {
        res += "0" + str_mins + ":";
    } else {
        res += str_mins + ":";
    }

    std::string str_remain = std::to_string(remain);
    if (str_remain.length() < 2) {
        res += "0" + str_remain;
    } else {
        res += str_remain;
    }

    return res;
}

std::string Interface::getProgressBar(int position, int length, int status) {
    const std::string FILLED = "#";
    const std::string UNFILLED = " ";

    switch (status) {
        case BASS_ACTIVE_PAUSED:
            return "[            PAUSED            ]";
        case BASS_ACTIVE_STALLED:
            return "[          BUFFERING           ]";
        case BASS_ACTIVE_STOPPED:
            return "[           STOPPED            ]";
    }

    const int BAR_LENGTH = 30;

    int bars;
    if (length == 0) {
        bars = 0;
    } else {
        bars = (double(position) / double(length)) * double(BAR_LENGTH);
    }

    int counter = 0;

    std::string res = "[";

    for (int x = 0; x < BAR_LENGTH; x++) {
        if (counter < bars) {
            res += FILLED;

            counter++;
            continue;
        }

        res += UNFILLED;
    }

    return res + "]";
}
