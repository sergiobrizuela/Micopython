#!/bin/bash
set -e
cd /tmp
if [ -d ftbuild ]; then rm -r ftbuild; fi
mkdir ftbuild
cd ftbuild
wget https://download.savannah.gnu.org/releases/freetype/freetype-2.11.0.tar.gz
tar xzf freetype-2.11.0.tar.gz
cd freetype-2.11.0
mkdir build
cd build
emcmake cmake ..
emmake make -j $(nproc)
emmake make install
echo "Built and installed FreeType successfully."
