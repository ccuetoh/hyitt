//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#ifndef RUBEN_PLAYER_H
#define RUBEN_PLAYER_H

#include "../stream/audio_stream.h"

#include "Bass/bass.h"

class Player {
    public:
        explicit Player(AudioStream audioStream);

        static void init();
        static void cleanup();

        void play();
        void pause() const;
        void resume() const;

        double getPosition() const;
        void setPosition(int seconds) const;
        void rewind(int seconds) const;
        void advance(int seconds) const;

        void wait() const;
        void waitResume() const;

        int getState() const;
        bool isPlaying() const;
        bool isPaused() const;
        bool isActive() const;
        bool isStalled() const;

private:
        AudioStream stream;
        DWORD onichan;
};

#endif //RUBEN_PLAYER_H
