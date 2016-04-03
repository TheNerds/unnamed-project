#!/bin/bash
#
# edschneider
# Compile shared librarie for linkage with program in C for mixed mode with C++ functions
#

#g++ -ggdb `pkg-config --cflags opencv` -o `basename facerec_video.cpp .cpp` facerec_video.cpp `pkg-config --libs opencv`

#g++ -c -Wall -Werror -fPIC CPPfile.cpp
g++ -c -Werror -fPIC facerec_video_lib.cpp
g++ -ggdb `pkg-config --cflags opencv` -shared -o libfacerec_video.so facerec_video_lib.o `pkg-config --libs opencv`
