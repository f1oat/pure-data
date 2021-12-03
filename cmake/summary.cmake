include(TargetArch)
target_architecture(SUMMARY_TARGET_ARCH)

message(STATUS "===========================================")
message(STATUS "Configure summary:")
message(STATUS "===========================================")
if(APPLE)
message(STATUS "  MacOSX version       ${MACOSX_VERSION}")
endif()
message(STATUS "  Pd version:          ${PD_TEXT_VERSION_FULL}")
message(STATUS "  Tk version:          ${TK_VERSION}")
message(STATUS "  Distrib version:     ${CEAMMC_DISTRIB_VERSION}")
message(STATUS "  ceammc version:      ${CEAMMC_LIB_VERSION}")
message(STATUS "  C compiler:          ${CMAKE_C_COMPILER}")
message(STATUS "  C++ compiler:        ${CMAKE_CXX_COMPILER}")
message(STATUS "  Arch:                ${SUMMARY_TARGET_ARCH}")
if(WITH_DOUBLE_PRECISION)
message(STATUS "  float precision:     64")
else()
message(STATUS "  float precision:     32")
endif()
message(STATUS "  build type:          ${CMAKE_BUILD_TYPE}")
message(STATUS "  PD_INSTANCE:         ${WITH_PD_INSTANCE}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
message(STATUS "  CFLAGS:              ${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
message(STATUS "  CXXFLAGS:            ${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
message(STATUS "  CFLAGS:              ${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS}")
message(STATUS "  CXXFLAGS:            ${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS}")
endif()

if(APPLE)
message(STATUS "  MacOSX sysroot:      ${CMAKE_OSX_SYSROOT}")
message(STATUS "  MacOSX min-version:  ${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()
message(STATUS "                                    ")
message(STATUS "  Audio:")
message(STATUS "       alsa:        ${WITH_ALSA}")
message(STATUS "       dummy:       ${WITH_DUMMY_AUDIO}")
message(STATUS "       jack:        ${WITH_JACK}")
message(STATUS "       portaudio:   ${WITH_PORTAUDIO}")
message(STATUS "       oss:         ${WITH_OSS}")

if(WIN32)
message(STATUS "       mmio:        ${WITH_MMIO}")
endif()

message(STATUS "  MIDI:")
message(STATUS "       alsa:        ${WITH_ALSA_MIDI}")
message(STATUS "       dummy:       ${WITH_DUMMY_MIDI}")
message(STATUS "       portmidi:    ${WITH_PORTMIDI}")
message(STATUS "       oss:         ${WITH_OSS}")
if(APPLE)
message(STATUS "       coremidi:    ${WITH_COREMIDI}")
endif()
message(STATUS "")
message(STATUS "  Libraries:")
message(STATUS "       fftw3:       ${WITH_FFTW} (WITH_FFTW=${WITH_FFTW})")
message(STATUS "       modplug:     ${WITH_MODPLUG} (WITH_MODPLUG=${WITH_MODPLUG})")
message(STATUS "       libsndfile:  ${LIBSNDFILE_FOUND}")
message(STATUS "       LeapMotion:  ${LEAPMOTION_FOUND}")
message(STATUS "       Faust:       ${WITH_FAUST}")
message(STATUS "       LuaJIT:      ${WITH_LUAJIT}")
message(STATUS "       FluidSynth:  ${WITH_FLUIDSYNTH} (WITH_FLUIDSYNTH=${WITH_FLUIDSYNTH})")
message(STATUS "       Sfizz:       ${WITH_SFIZZ}")
message(STATUS "       flite:       ${WITH_TTS_FLITE} (WITH_TTS_FLITE=${WITH_TTS_FLITE})")
message(STATUS "       libusb:      ${LIBUSB_1_FOUND}")
message(STATUS "  3rd party externals:")
message(STATUS "       lyonpotpourri: ${WITH_EXT_LYONPOTPOURRI} (WITH_EXT_LYONPOTPOURRI=${WITH_EXT_LYONPOTPOURRI})")
message(STATUS "       FFTease:       ${WITH_EXT_FFTEASE} (WITH_EXT_FFTEASE=${WITH_EXT_FFTEASE})")
message(STATUS "       [autotune~]:   ${WITH_EXT_AUTOTUNE} (WITH_EXT_AUTOTUNE=${WITH_EXT_AUTOTUNE})")
message(STATUS "       flext:         ${WITH_EXT_FLEXT} (WITH_EXT_FLEXT=${WITH_EXT_FLEXT})")
message(STATUS "===========================================")
message(STATUS "Build info:")
message(STATUS "===========================================")
message(STATUS "  Coverage:         ${WITH_COVERAGE}")
message(STATUS "  Benchmark:        ${WITH_BENCHMARK}")
message(STATUS "  clang-tidy:       ${WITH_CLANG_TIDY} (WITH_CLANG_TIDY=${WITH_CLANG_TIDY})")
message(STATUS "  Experimental:     ${WITH_EXPERIMENTAL}")
message(STATUS "  Tests:            ${ENABLE_TESTS}")
message(STATUS "  Install prefix:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  External suffix:  ${PD_EXTERNAL_EXTENSION}")
if(PD_MACOSX_DMG)
message(STATUS "  Dmg:              ${PD_MACOSX_DMG}")
endif()
if(CEAMMC_EXTERNAL_NAME)
message(STATUS "  External:         ${CEAMMC_EXTERNAL_NAME}")
endif()
message(STATUS "")
