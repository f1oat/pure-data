@echo off

cd %APPVEYOR_BUILD_FOLDER%

echo Compiler: %COMPILER%
echo Architecture: %MSYS2_ARCH%
echo Platform: %PLATFORM%
echo MSYS2 directory: %MSYS2_DIR%
echo MSYS2 system: %MSYSTEM%
echo Bits: %BIT%

@echo on
set "PATH=C:\%MSYS2_DIR%\%MSYSTEM%\bin;C:\%MSYS2_DIR%\usr\bin;%PATH%"
bash -lc "pacman -S --needed --noconfirm pacman-mirrors"
bash -lc "pacman -S --needed --noconfirm git"
REM Update
bash -lc "pacman -Syu --noconfirm"

REM build tools
bash -lc "pacman -S --needed --noconfirm mingw-w64-${MSYS2_ARCH}-toolchain make patch"
REM dependencies
bash -lc "pacman -S --needed --noconfirm mingw-w64-${MSYS2_ARCH}-{cmake,glib2,libmodplug,tcllib,tcl,tk,tklib,portaudio,fftw,libsndfile,boost,dlfcn}"

bash -lc "echo ${APPVEYOR_BUILD_FOLDER}"
bash -lc "ls ${APPVEYOR_BUILD_FOLDER}"
bash -lc "mkdir ${APPVEYOR_BUILD_FOLDER}/build"
bash -lc "cd ${APPVEYOR_BUILD_FOLDER}/build && cmake -G 'MSYS Makefiles' -DCMAKE_BUILD_TYPE=Release -DWITH_FFTW=ON -DCMAKE_INSTALL_PREFIX=/opt/local/release/pd ${APPVEYOR_BUILD_FOLDER}"
bash -lc "ls ${APPVEYOR_BUILD_FOLDER}/build"
bash -lc "cd ${APPVEYOR_BUILD_FOLDER}/build && make -j3 && make -j3 install && ./fix_dll_deps.sh && ./win_fix_pddoc_key_modifiers.sh"
bash -lc "cd ${APPVEYOR_BUILD_FOLDER}/build && make inno && make ceammc_lib"
