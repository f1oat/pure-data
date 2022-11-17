set(PD_MAJOR_VERSION 0)
set(PD_MINOR_VERSION 53)
set(PD_BUGFIX_VERSION 0)
set(PD_BUILD_SUFFIX "ceammc")
set(PD_TEXT_VERSION_SHORT "${PD_MAJOR_VERSION}.${PD_MINOR_VERSION}.${PD_BUGFIX_VERSION}")
set(PD_TEXT_VERSION_FULL  "${PD_TEXT_VERSION_SHORT}_${PD_BUILD_SUFFIX}")

set(CEAMMC_DISTRIB_VERSION "2022.11")

if(APPLE)
   exec_program(sw_vers ARGS -productVersion OUTPUT_VARIABLE MACOSX_VERSION)
   string(REGEX MATCH "[0-9]+.[0-9]+" MACOSX_VERSION ${MACOSX_VERSION})

   if(WITH_DOUBLE_PRECISION)
       set(PD_MACOSX_BUNDLE_SUFFIX "ceammc_${CEAMMC_DISTRIB_VERSION}-double")
       set(PD_FLOAT_SIZE 64)
   else()
       set(PD_MACOSX_BUNDLE_SUFFIX "ceammc_${CEAMMC_DISTRIB_VERSION}")
       set(PD_FLOAT_SIZE 32)
   endif()

   set(PD_MACOSX_APP     "Pd-${PD_MACOSX_BUNDLE_SUFFIX}-macosx-${MACOSX_VERSION}.app")
   set(PD_MACOSX_ZIP     "Pd-${PD_MACOSX_BUNDLE_SUFFIX}-macosx-${MACOSX_VERSION}.zip")
   set(PD_MACOSX_DMG     "Pd-${PD_MACOSX_BUNDLE_SUFFIX}-macosx-${MACOSX_VERSION}.dmg")
   set(PD_MACOSX_DMG_APP "Pd-${PD_MACOSX_BUNDLE_SUFFIX}.app") # short name in DMG volume

   if(WITH_PD_INSTANCE)
       set(MT_SUFFIX "-mt")
   else()
       set(MT_SUFFIX "")
   endif()

   set(CEAMMC_EXTERNAL_NAME "ceammc-${CEAMMC_LIB_VERSION}-macosx-${MACOSX_VERSION}-pd-${PD_TEXT_VERSION_SHORT}-f${PD_FLOAT_SIZE}${MT_SUFFIX}.tar.gz")

elseif(UNIX)
    set(LINUX_DEBIAN_PACKAGE_NAME "pd-ceammc")
    set(LINUX_DEBIAN_PACKAGE_FILE_NAME "${LINUX_DEBIAN_PACKAGE_NAME}-${CEAMMC_DISTRIB_VERSION}-${PD_TEXT_VERSION_SHORT}_${LSB_CODENAME}_${LSB_PROCESSOR_ARCH}.deb")
elseif(WIN32)

    if(WITH_DOUBLE_PRECISION)
        set(PD_FLOAT_SIZE 64)
    else()
        set(PD_FLOAT_SIZE 32)
    endif()

    if(WITH_PD_INSTANCE)
        set(MT_SUFFIX "-mt")
    else()
        set(MT_SUFFIX "")
    endif()

    set(CEAMMC_EXTERNAL_NAME "ceammc-${CEAMMC_LIB_VERSION}-win-pd-${PD_TEXT_VERSION_SHORT}-f${PD_FLOAT_SIZE}${MT_SUFFIX}.zip")

endif()
