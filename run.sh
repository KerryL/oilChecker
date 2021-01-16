#!/bin/bash
# cd to oilChecker directory so when we autostart we know where we are
cd /home/pi/Projects/oilChecker

export LD_LIBRARY_PATH="/usr/local/lib/:$LD_LIBRARY_PATH"
#ldd bin/oilChecker
./bin/oilChecker oilChecker.rc

