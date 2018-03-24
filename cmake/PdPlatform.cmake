include(TestBigEndian)
include(FindLibDL)
include(windres)
test_big_endian(IS_BIG_ENDIAN)
if(NOT ${IS_BIG_ENDIAN})
    add_definitions(-DLITTLE_ENDIAN=0x0001 -DBYTE_ORDER=LITTLE_ENDIAN)
endif()

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads)

set(PLATFORM_LINK_LIBRARIES)


if(UNIX AND NOT APPLE)
    add_definitions(-D_GNU_SOURCE)
    list(APPEND PLATFORM_LINK_LIBRARIES m)
    list(APPEND PLATFORM_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
    set(LINUX True)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    option(WITH_ASAN "Use Address Sanitizer for Clang" OFF)

    if(WITH_ASAN)
        set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
        set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
        set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer -fsanitize=address")
        set (CMAKE_LINKER_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer -fsanitize=address")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
        set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
    endif()
endif()


if(WIN32)
    find_program(WISH_PATH
        NAMES wish86.exe wish.exe
        PATHS C:/Tcl/bin)

    if(NOT WISH_PATH)
        message(FATAL_ERROR "wish.exe not found")
    else()
        message(STATUS "wish found: ${WISH_PATH}")
        get_filename_component(WISHAPP ${WISH_PATH} NAME)
        get_filename_component(WISH_BINDIR ${WISH_PATH} DIRECTORY)
    endif()

    # install wish.exe to bin directory
    install(PROGRAMS ${WISH_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    include(InstallRequiredSystemLibraries)

    # install tcl.dll
    find_file(TCLDLL_PATH NAMES tcl86.dll PATHS ${WISH_BINDIR})
    if(TCLDLL_PATH)
        install(PROGRAMS ${TCLDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install tk.dll
    find_file(TKDLL_PATH NAMES tk86.dll PATHS ${WISH_BINDIR})
    if(TKDLL_PATH)
        install(PROGRAMS ${TKDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install zlib.dll
    find_file(ZLIBDLL_PATH NAMES zlib1.dll PATHS ${WISH_BINDIR})
    if(ZLIBDLL_PATH)
        install(PROGRAMS ${ZLIBDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install tcl/tk libs
    function(install_tcl_dir name)
        # get normalized path
        get_filename_component(_path "${WISH_BINDIR}/../lib/${name}" ABSOLUTE)
        if(EXISTS ${_path})
            message(STATUS "Found TCL package: ${name} at ${_path}")
            install(DIRECTORY "${_path}" DESTINATION "${PD_LIBTCL_INSTALL_PATH}")
        else()
            message(STATUS "TCL package not found: ${name}")
        endif()
    endfunction()

    install_tcl_dir(tcl8)
    install_tcl_dir(tcl8.6)
    install_tcl_dir(tk8.6)
    install_tcl_dir(dde1.4)
    install_tcl_dir(tcllib1.18)
    # try different tooltip location
    install_tcl_dir(tklib0.6)
    install_tcl_dir(tklib0.6/tooltip)
    install_tcl_dir(tooltip)
    install_tcl_dir(reg1.3)

    # pthreadGC-3.dll
    find_file(PTHREADGC_DLL
        NAMES pthreadGC-3.dll pthreadGC-2.dll winpthread-1.dll libwinpthread-1.dll
        PATHS /mingw/bin /mingw/i686-w64-mingw32/bin)
    if(PTHREADGC_DLL)
        message(STATUS "found thread lib: ${PTHREADGC_DLL}")
        install(FILES ${PTHREADGC_DLL} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install libsndfile-1.dll
    find_file(SNDFILEDLL_PATH NAMES libsndfile-1.dll PATHS ${WISH_BINDIR})
    if(SNDFILEDLL_PATH)
        install(PROGRAMS ${SNDFILEDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # on MSYS2 libsndfile builded with speex support
    # install libspeex-1.dll
    find_file(SPEEXDLL_PATH NAMES libspeex-1.dll PATHS ${WISH_BINDIR})
    if(SPEEXDLL_PATH)
        install(PROGRAMS ${SPEEXDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install libFLAC-8.dll
    find_file(FLACDLL_PATH NAMES libFLAC-8.dll PATHS ${WISH_BINDIR})
    if(FLACDLL_PATH)
        install(PROGRAMS ${FLACDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install libogg-0.dll
    find_file(OGGDLL_PATH NAMES libogg-0.dll PATHS ${WISH_BINDIR})
    if(OGGDLL_PATH)
        install(PROGRAMS ${OGGDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install libvorbis-0.dll
    find_file(VORBISDLL_PATH NAMES libvorbis-0.dll PATHS ${WISH_BINDIR})
    if(VORBISDLL_PATH)
        install(PROGRAMS ${VORBISDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install libvorbisenc-2.dll
    find_file(VORBISENCDLL_PATH NAMES libvorbisenc-2.dll PATHS ${WISH_BINDIR})
    if(VORBISENCDLL_PATH)
        install(PROGRAMS ${VORBISENCDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install libmodplug-1.dll
    find_file(MODPLUGDLL_PATH NAMES libmodplug-1.dll PATHS ${WISH_BINDIR})
    if(MODPLUGDLL_PATH)
        install(PROGRAMS ${MODPLUGDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install libfftw3f-3.dll
    find_file(FFTWDLL_PATH NAMES libfftw3f-3.dll PATHS ${WISH_BINDIR})
    if(FFTWDLL_PATH)
        install(PROGRAMS ${FFTWDLL_PATH} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # mingw runtime libs

    get_filename_component(_MINGW_PATH ${CMAKE_CXX_COMPILER} PATH)
    set(_MINGW_SYSROOT1 ${_MINGW_PATH}/../i686-w64-mingw32/bin )

    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-libgcc-file-name OUTPUT_VARIABLE _LIBGCC_FNAME)
    get_filename_component(_LIBGCC_PATH ${_LIBGCC_FNAME} DIRECTORY)
    set(_MINGW_SYSROOT2 ${_LIBGCC_PATH}/../../../../i686-w64-mingw32/lib)

    # C++ runtime
    find_file(LIBSTDXX_DLL
        NAME libstdc++-6.dll
        PATHS ${_MINGW_SYSROOT1} ${_MINGW_SYSROOT2})

    if(LIBSTDXX_DLL)
        message(STATUS "MinGW runtime: ${LIBSTDXX_DLL} found")
        list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${LIBSTDXX_DLL})
    endif()

    # C runtime
    find_file(LIBGCC_RUNTIME_DLL
        NAME libgcc_s_sjlj-1.dll libgcc_s_dw2-1.dll
        PATHS ${_MINGW_SYSROOT1} ${_MINGW_SYSROOT2})

    if(LIBGCC_RUNTIME_DLL)
        message(STATUS "MinGW runtime: ${LIBGCC_RUNTIME_DLL} found")
        list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${LIBGCC_RUNTIME_DLL})
    endif()

    # thread
    find_file(LIBWINPTHREAD
        NAME libwinpthread-1.dll
        PATHS ${_MINGW_SYSROOT1} ${_MINGW_SYSROOT2} ${_MINGW_SYSROOT2}/../bin)

    if(LIBWINPTHREAD)
        message(STATUS "MinGW runtime: ${LIBWINPTHREAD} found")
        list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${LIBWINPTHREAD})
    endif()

    include(InstallRequiredSystemLibraries)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        DESTINATION bin
        COMPONENT Libraries)

    # libportaudio-2.dll
    find_file(LIBPORTAUDIO_DLL NAMES libportaudio-2.dll PATHS /usr/bin /usr/local/bin)
    if(LIBPORTAUDIO_DLL)
        install(FILES ${LIBPORTAUDIO_DLL} DESTINATION ${PD_EXE_INSTALL_PATH})
    endif()

    # install README, LICENSE.txt to root
    install(FILES
            ${CMAKE_SOURCE_DIR}/README.txt
            ${CMAKE_SOURCE_DIR}/LICENSE.txt
        DESTINATION
            ${CMAKE_INSTALL_PREFIX})

    if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
        message("Building for UWP")
        set(FIPS_UWP 1)
    else()
        set(FIPS_UWP 0)
    endif()

    if (FIPS_UWP)
        set(CMAKE_CXX_STANDARD_LIBRARIES "WindowsApp.lib")
        set(CMAKE_C_STANDARD_LIBRARIES "WindowsApp.lib")
    else()
        set(CMAKE_CXX_STANDARD_LIBRARIES "-lkernel32 -lgdi32 -lole32 -lwinmm -luuid -lwsock32 -lws2_32")
        set(CMAKE_C_STANDARD_LIBRARIES ${CMAKE_CXX_STANDARD_LIBRARIES})
    endif()

    add_definitions(-DPD_INTERNAL -DWINVER=0x0502 -D_WIN32_WINNT=0x0502)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mms-bitfields")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mms-bitfields -Wno-incompatible-ms-struct")

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mms-bitfields -O2 -funroll-loops -fomit-frame-pointer")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mms-bitfields -O2 -funroll-loops -fomit-frame-pointer -Wno-incompatible-ms-struct")

    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols -lpthread")
    set(CMAKE_EXE_LINKER_FLAGS "-shared-libgcc -lpthread")

    list(APPEND PLATFORM_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
endif()

if(APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -funroll-loops -fomit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops -fomit-frame-pointer")

    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG -ffast-math -funroll-loops -fomit-frame-pointer")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -ffast-math -funroll-loops -fomit-frame-pointer")

    set(BUNDLE "Pd-${PD_MACOSX_BUNDLE_SUFFIX}.app")
    set(BUNDLE_FULL_PATH "${PROJECT_BINARY_DIR}/dist/${BUNDLE}")
    set(MAKE_BUNDLE_SCRIPT ${PROJECT_BINARY_DIR}/dist/build_mac.sh)

    # copy and substitute variables to Info.plist
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/dist)
    configure_file(${PROJECT_SOURCE_DIR}/ceammc/gui/Info.plist ${PROJECT_BINARY_DIR}/dist/Info.plist)

    set(CUSTOM_TCL "/Library/Frameworks/Tcl.framework")
    set(CUSTOM_TK  "/Library/Frameworks/Tk.framework")
    set(CUSTOM_WISH "${CUSTOM_TK}/Versions/8.6/Resources/Wish.app")

    set(SYSTEM_TCL     "/System/Library/Frameworks/Tcl.framework")
    set(SYSTEM_TK      "/System/Library/Frameworks/Tk.framework")
    set(SYSTEM_WISH_V5 "${SYSTEM_TK}/Versions/8.5/Resources/Wish.app")
    set(SYSTEM_WISH_V4 "${SYSTEM_TK}/Versions/8.4/Resources/Wish.app")

    set(IS_SYSTEM_TK 0)
    set(TK_VERSION "")

    if(EXISTS ${CUSTOM_TCL} AND EXISTS ${CUSTOM_TK} AND EXISTS ${CUSTOM_WISH})
        set(TCL_PATH "${CUSTOM_TCL}")
        set(TK_PATH  "${CUSTOM_TK}")
        set(WISH_APP "${CUSTOM_WISH}")
        set(TK_VERSION "8.6")
    elseif(EXISTS ${SYSTEM_TCL} AND EXISTS ${SYSTEM_TK})
        set(TCL_PATH "${SYSTEM_TCL}")
        set(TK_PATH  "${SYSTEM_TK}")

        if(EXISTS ${SYSTEM_WISH_V5})
            set(WISH_APP "${SYSTEM_WISH_V5}")
            set(TK_VERSION "8.5")
        elseif(EXISTS ${SYSTEM_WISH_V4})
            set(WISH_APP "${SYSTEM_WISH_V4}")
            set(TK_VERSION "8.4")
        else()
            message(FATAL_ERROR "Wish.app not found in \"${SYSTEM_TK}\"")
        endif()
        set(IS_SYSTEM_TK 1)
    else()
        message(FATAL_ERROR "TCL not found")
    endif()

    message(STATUS "found Tcl: ${TCL_PATH}")
    message(STATUS "found Tk:  ${TK_PATH}")
    message(STATUS "found Wish.app: ${WISH_APP}")
    message(STATUS "found Tcl/Tk version: ${TK_VERSION}")

    add_custom_command(
        OUTPUT ${MAKE_BUNDLE_SCRIPT}
        COMMAND ${CMAKE_COMMAND}
            -DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
            -DPROJECT_BINARY_DIR="${PROJECT_BINARY_DIR}"
            -DBUNDLE=${BUNDLE_FULL_PATH}
            -DWISH_APP=${WISH_APP}
            -DTCL_PATH=${TCL_PATH}
            -DLEAPMOTION_LIBRARY=${LEAPMOTION_LIBRARY}
            -DDYLIBBUNDLER="${CMAKE_BINARY_DIR}/ceammc/distrib/mac/dylibbundler"
            -DTK_PATH=${TK_PATH}
            -DIS_SYSTEM_TK=${IS_SYSTEM_TK}
            -DTK_VERSION=${TK_VERSION}
            -P ${PROJECT_SOURCE_DIR}/cmake/cmake-build-mac.cmake)

    add_custom_command(
        OUTPUT ${BUNDLE_FULL_PATH}
        COMMAND sh ${PROJECT_BINARY_DIR}/dist/build_mac.sh
        COMMAND ${CMAKE_COMMAND}
            -DBUNDLE=${BUNDLE_FULL_PATH}
            -P ${PROJECT_SOURCE_DIR}/cmake/bundle.cmake
        DEPENDS pd)

    # app target
    # `make app` creates MacOSX bundle
    add_custom_target(app DEPENDS ${MAKE_BUNDLE_SCRIPT} ${BUNDLE_FULL_PATH})

    add_custom_target(codesign
        COMMAND sh ${PROJECT_SOURCE_DIR}/mac/codesign.sh ${BUNDLE_FULL_PATH}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/mac)

    add_custom_target(dmg
        COMMAND
            sh ${PROJECT_SOURCE_DIR}/mac/dmg.sh
            ${BUNDLE_FULL_PATH}
            "${PD_MACOSX_BUNDLE_SUFFIX}-macosx-${MACOSX_VERSION}"
            ${PROJECT_SOURCE_DIR}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS app)

    find_program(GPGENV_EXE NAMES gpgenv)

    if(GPGENV_EXE)
        add_custom_target(deploy
            COMMAND
                ${PROJECT_BINARY_DIR}/dist/github_upload_release.sh
                "${PROJECT_BINARY_DIR}/Pd_${PD_MACOSX_BUNDLE_SUFFIX}-macosx-${MACOSX_VERSION}.dmg"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            DEPENDS dmg src-zip src-tar ceammc_lib)
    endif()
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -funroll-loops -fomit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops -fomit-frame-pointer")

    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG -ffast-math -funroll-loops -fomit-frame-pointer")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -ffast-math -funroll-loops -fomit-frame-pointer")
endif()
