#!/bin/bash
# Install cspice toolkit in the Isis root directory and create a symbolic link to the library
# so the linker can find it
cd $ISISROOT
wget http://naif.jpl.nasa.gov/pub/naif/toolkit//C/PC_Linux_GCC_64bit/packages/cspice.tar.Z
tar xzf cspice.tar.Z
rm cspice.tar.Z
ln -s cspice.a cspice/lib/libcspice.a
mkdir cspice/include/naif
mv cspice/include/* naif
