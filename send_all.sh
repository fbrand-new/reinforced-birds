#!/bin/bash
scp -r ./src/* fbrand@argo.ictp.it:/home/fbrand/code_tiles/src/
scp -r ./include/* fbrand@argo.ictp.it:/home/fbrand/code_tiles/include/
scp -r ./main.cpp fbrand@argo.ictp.it:/home/fbrand/code_tiles/main.cpp
scp -r ./makefile fbrand@argo.ictp.it:/home/fbrand/code_tiles/makefile
scp config.h fbrand@argo.ictp.it:/home/fbrand/code_tiles/.
scp launch.sh fbrand@argo.ictp.it:/home/fbrand/code_tiles/.
#scp code/launch.sh fbrand@argo.ictp.it:/home/fbrand/code/launch.sh
