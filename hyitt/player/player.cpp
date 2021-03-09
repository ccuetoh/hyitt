//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#include <iostream>
#include <utility>

#include "player.h"
#include "../exceptions.h"

Player::Player(AudioStream audioStream) {
    stream = std::move(audioStream);
}

void Player::init() {
    BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, 1);
    BASS_Init(-1, 44100, 0, nullptr, nullptr);
}

void Player::play() {
    auto c = BASS_StreamCreateURL(stream.getUrl().c_str(), 0, BASS_STREAM_AUTOFREE, nullptr, nullptr);
    if (c == 0) {
        int err_code = BASS_ErrorGetCode();
        if (err_code == 2) {
            throw NetworkException(0);
        }

        throw BassException(err_code);
    }

    onichan = c;
    BASS_ChannelPlay(c, TRUE);
}

void Player::pause() const {
    BASS_ChannelPause(onichan);
}

void Player::resume() const {
    BASS_ChannelPlay(onichan, false);
}

void Player::cleanup() {
    BASS_Free();
}

void Player::wait() const {
    while (true) {
        if (!isActive()) {
            break;
        }
    }
}

void Player::waitResume() const {
    while (true) {
        auto status = BASS_ChannelIsActive(onichan);
        if (status == BASS_ACTIVE_PLAYING) {
            break;
        }
    }
}

int Player::getState() const {
    return BASS_ChannelIsActive(onichan);
}

bool Player::isPlaying() const {
    return getState() == BASS_ACTIVE_PLAYING;
}

bool Player::isStalled() const {
    return getState() == BASS_ACTIVE_STALLED;
}

bool Player::isPaused() const {
    auto status = getState();
    return (status == BASS_ACTIVE_PAUSED || status == BASS_ACTIVE_PAUSED_DEVICE);
}

bool Player::isActive() const {
    int status = getState();
    if (status != BASS_ACTIVE_PLAYING && status != BASS_ACTIVE_STALLED && status != BASS_ACTIVE_PAUSED && status != BASS_ACTIVE_STOPPED) {
        return false;
    }

    return true;
}

double Player::getPosition() const {
    QWORD posBytes = BASS_ChannelGetPosition(onichan, BASS_POS_BYTE);
    return BASS_ChannelBytes2Seconds(onichan, posBytes);
}

void Player::setPosition(int seconds) const {
    BASS_ChannelSetPosition(onichan, BASS_ChannelSeconds2Bytes(onichan, seconds), BASS_POS_BYTE);
}

void Player::rewind(int seconds) const {
    setPosition(max(getPosition() - seconds, 0));
}

void Player::advance(int seconds) const {
    setPosition(int(getPosition()) + seconds);
}
