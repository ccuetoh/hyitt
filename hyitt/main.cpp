//
// HYITT - Hear Youtube in the Terminal
// Created by Camilo Hernández and released under the MIT Licence
//

#include <iostream>

#include "exceptions.h"
#include "video/video.h"
#include "player/player.h"
#include "player/interface.h"

#define NAME "HYITT"
#define EXEC_NAME "hyitt"

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cout << "Usage: " << EXEC_NAME << " <id|link> [--high|--medium|--low]" << std::endl;
        return -1;
    }

    AudioQuality preferredQuality = AudioQuality::Medium; // Default to medium
    if (argc == 3) {
        std::string qualityArg = argv[2];

        if (qualityArg == "--high") {
            preferredQuality = AudioQuality::High;
        }
        if (qualityArg == "--low") {
            preferredQuality = AudioQuality::Low;
        }
        // Medium is not needed as it's the default
    }

    Video video(argv[1]);

    std::vector<AudioStream> streams;
    try {
        streams = video.getStreams();
    } catch (NoStreamsException &e) { // No compatible streams or bad URL
        std::cout << e.what() << std::endl;
        return -1;
    }


    AudioStream s;
    AudioStream backup;
    for (const auto& stream: streams) {
        if (stream.getMimeType() != "audio/webm") {
            continue;
        }

        if (stream.getQuality() == preferredQuality) {
            s = stream;
            break;
        }
        if (stream.getQuality() < preferredQuality && !backup.isValid()) {
            backup = stream;
        }
    }

    if (!s.isValid() && !backup.isValid()) {
        std::cout << "No suitable streams found" << std::endl;
        return -1;
    }  else {
        try {
            if (s.isValid()) {
                Player player(s);
                Interface tli(video, player);
                tli.start();
            } else {
                Player player(backup);
                Interface tli(video, player);
                tli.start();
            }
        } catch (NetworkException &e) { // Something went wrong with an HTTP request
            std::cout << e.what() << std::endl;
            return -1;
        } catch (SignatureException &e) { // Unable to decode the signature
            std::cout << e.what() << std::endl;
            return -1;
        } catch (BassException &e) { // BASS reported an error
            std::cout << e.what() << std::endl;
            return -1;
        }
    }

    return 0;
}
