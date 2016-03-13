#!/bin/bash
g++ -ggdb `pkg-config --cflags opencv` -o `basename facerec_video.cpp .cpp` facerec_video.cpp `pkg-config --libs opencv`
