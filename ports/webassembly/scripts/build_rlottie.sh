#!/bin/bash
# Based on https://docs.lvgl.io/8.0/libs/rlottie.html
SCRIPTSDIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd /tmp
rm -rf rlottie_workdir
mkdir rlottie_workdir
cd rlottie_workdir
git init
git remote add origin https://github.com/Samsung/rlottie.git
git fetch origin 327fb7dbaad225555d5ca567b9adee9ce5f879f4 --depth=1
git reset --hard FETCH_HEAD
git apply ${SCRIPTSDIR}/rlottie.patch
mkdir lv_build
cd lv_build
EMSCRIPTEN_SYSROOT=${EMSDK}/upstream/emscripten/cache/sysroot
emcmake cmake .. -DLIB_INSTALL_DIR=${EMSCRIPTEN_SYSROOT}/lib -DCMAKE_INSTALL_PREFIX=${EMSCRIPTEN_SYSROOT}
emmake make -j$(nproc)
emmake make install
