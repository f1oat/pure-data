#!/bin/bash

DIR="../../../../.."
BUILD="Release"
SUBDIR=$(echo $BUILD | tr '[:upper:]' '[:lower:]')
ARCH=$(echo $MSYSTEM | cut -c6-)
TYPE="single"
ROOT="/opt/local/pd/mingw${ARCH}/${SUBDIR}/${TYPE}"
export PKG_CONFIG_PATH=/opt/local/lib/pkgconfig

cmake -G Ninja \
	-DCMAKE_BUILD_TYPE=$BUILD \
	-DCMAKE_INSTALL_PREFIX=${ROOT} \
	-DWITH_FFTW=ON \
	-DENABLE_TESTS=ON \
	-DWITH_EXT_LYONPOTPOURRI=OFF \
	-DWITH_EXT_FFTEASE=OFF \
	-DWITH_EXT_FLEXT=OFF \
	-DWITH_SFIZZ=ON \
	-DWITH_FAUST=OFF \
	-DWITH_TTS_RHVOICE=OFF \
	-DWITH_BENCHMARK=OFF \
	-DRHVoice_ROOT=/usr/local \
	$DIR
