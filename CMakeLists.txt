cmake_minimum_required(VERSION 3.17)
project(hyitt)

set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG c34ddb9b3de2a22fdbd5d318d8b7d1997e6ca0bf)
FetchContent_MakeAvailable(cpr)

include_directories(include)
link_directories(lib)

add_executable(hyitt hyitt/main.cpp hyitt/video/video.h hyitt/video/video.cpp hyitt/video/quality.h hyitt/player/player.h hyitt/player/player.cpp hyitt/stream/audio_stream.cpp hyitt/stream/audio_stream.h hyitt/player/interface.cpp hyitt/player/interface.h hyitt/decoder/decoder.cpp hyitt/decoder/decoder.h include/Duktape/duktape.c hyitt/exceptions.h)

set(LINK_LIBS ${CONAN_LIBS} cpr::cpr basswebm bass)

target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})