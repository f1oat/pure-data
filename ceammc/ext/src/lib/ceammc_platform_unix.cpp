/*****************************************************************************
 * Copyright 2017 Serge Poltavsky. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#include "ceammc_platform_unix.h"

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fnmatch.h>
#include <libgen.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace ceammc {
namespace platform {

    bool unix_is_path_relative(const char* path)
    {
        return path[0] != '/';
    }

    std::string unix_basename(const char* path)
    {
        char* p = strdup(path);
        char* bn = ::basename(p);
        if (!bn)
            return "";

        std::string res(bn);
        free(p);
        return res;
    }

    std::string unix_dirname(const char* path)
    {
        char* p = strdup(path);
        char* dir = ::dirname(p);
        if (!dir)
            return "";

        std::string res(dir);
        free(p);
        return res;
    }

    bool unix_fnmatch(const char* pattern, const char* str)
    {
        return ::fnmatch(pattern, str, 0) == 0;
    }

    bool unix_path_exists(const char* path)
    {
        return ::access(path, R_OK) == 0;
    }

    bool unix_mkdir(const char* path, int flags)
    {
        if (flags < 0)
            flags = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

        if (::mkdir(path, static_cast<mode_t>(flags)) == 0)
            return true;

        std::cerr << "[unix_mkdir] error: " << strerror(errno) << "\n";
        return false;
    }

    bool unix_rmdir(const char* path)
    {
        return ::rmdir(path) == 0;
    }

    std::string unix_home_directory()
    {
        const char* homedir = 0;

        if ((homedir = ::getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }

        return homedir ? homedir : "";
    }

    bool unix_is_dir(const char* path)
    {
        struct stat statbuf;
        if (::stat(path, &statbuf) != -1) {
            if (S_ISDIR(statbuf.st_mode))
                return true;
            else
                return false;
        }
        return false;
    }

    void unix_sleep_ms(unsigned int ms)
    {
        ::usleep(ms * 1000);
    }

    Either<NetAddressList> unix_hostnametoip(const char* name, NetAddressType type)
    {
        struct addrinfo* result = NULL;
        struct addrinfo hints;
        char address[INET6_ADDRSTRLEN];

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = (type == ADDR_IPV4) ? PF_INET : PF_INET6;
        hints.ai_protocol = IPPROTO_TCP;

        int error = getaddrinfo(name, NULL, &hints, &result);

        if (error)
            return PlatformError(error, gai_strerror(error));

        NetAddressList res;

        for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next) {
            int error = getnameinfo(ptr->ai_addr,
                ptr->ai_addrlen,
                address, sizeof(address), NULL, 0, NI_NUMERICHOST);

            if (error)
                return PlatformError(error, gai_strerror(error));

            res.push_back(address);
        }

        freeaddrinfo(result);
        return res;
    }
}
}
