include(CheckIncludeFiles)
include(CheckFunctionExists)
include(FindLibDL)

if(LIBDL_FOUND)
    set(HAVE_LIBDL 1)
endif()

check_include_file(alloca.h HAVE_ALLOCA_H)
check_include_file(arpa/inet.h HAVE_ARPA_INET_H)
check_include_file(bstring.h HAVE_BSTRING_H)
check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(errno.h HAVE_ERRNO_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(fnmatch.h HAVE_FNMATCH_H)
check_include_file(getopt.h HAVE_GETOPT_H)
check_include_file(libgen.h HAVE_LIBGEN_H)
check_include_file(limits.h  HAVE_LIMITS_H)
check_include_file(math.h HAVE_MATH_H)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(netinet/in.h HAVE_NETINET_IN_H)
check_include_file(netinet/tcp.h HAVE_NETINET_TCP_H)
check_include_file(pthread.h HAVE_PTHREAD_H)
check_include_file(signal.h HAVE_SIGNAL_H)
check_include_file(stdarg.h HAVE_STDARG_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdio.h HAVE_STDIO_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(sys/ioctl.h HAVE_SYS_IOCTL_H)
check_include_file(sys/mman.h HAVE_SYS_MMAN_H)
check_include_file(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(unistd.h HAVE_UNISTD_H)

check_function_exists(nanosleep HAVE_NANOSLEEP)
check_function_exists(setenv HAVE_SETENV)

find_package(ModPlug)
if(MODPLUG_FOUND)
    set(CEAMMC_HAVE_MODPLUG ON)
endif()

# FluidSynth
find_package(GLIB)
if(GLIB_FOUND)
    set(CEAMMC_HAVE_FLUIDSYNTH ON)
    set(WITH_FLUIDSYNTH TRUE)
    #include paths
    list(APPEND FLUIDSYNTH_INCLUDES
        ${GLIB_INCLUDES}
        ${PROJECT_BINARY_DIR}/ceammc/extra/fluidsynth
        ${PROJECT_SOURCE_DIR}/ceammc/extra/fluidsynth/fluidsynth/include)
    # libs
    list(APPEND FLUIDSYNTH_LIBRARIES fluidsynth ${GLIB_LIBRARIES})
else()
    set(WITH_FLUIDSYNTH FALSE)
    message(WARNING "Glib is not found: no fluidsynth build")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PD_LONGINTTYPE "long long")
endif()

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)
add_definitions(-include ${PROJECT_BINARY_DIR}/config.h)
