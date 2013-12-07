#!/bin/sh
# sync.sh downloads the latest version of ISIS to the current directory
rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::x86-64_linux_RHEL6/isis .
rsync -azv --delete --partial isisdist.astrogeology.usgs.gov::isis3data/data/base data/
