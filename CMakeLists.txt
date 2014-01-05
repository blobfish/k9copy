project(k9copy)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

SET (BACKLITE FALSE)

#SET(CMAKE_BUILD_TYPE Debug)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckVariableExists)
INCLUDE(CheckTypeSize)
INCLUDE(CMakeDetermineSystem)
INCLUDE(CheckIncludeFiles)
INCLUDE(FindLibMpeg2)
INCLUDE(FindXine)
INCLUDE(FindAvcodec)
INCLUDE(FindAvformat)
INCLUDE(FindFFmpegScale)
#INCLUDE(FindLibDvdread)

set (VERSION 2.3.8)



# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
CHECK_INCLUDE_FILES(inttypes.h      HAVE_INTTYPES_H)                    # inttypes.h
CHECK_INCLUDE_FILES(stddef.h        HAVE_STDDEF_H)                      # stddef.h
CHECK_INCLUDE_FILES(stdint.h        HAVE_STDINT_H)                      # stdint.h
CHECK_INCLUDE_FILES(stdlib.h        HAVE_STDLIB_H)                      # stdlib.h
CHECK_INCLUDE_FILES(string.h        HAVE_STRING_H)                      # string.h
CHECK_INCLUDE_FILES(sys/stat.h      HAVE_SYS_STAT_H)                    # stat.h
CHECK_INCLUDE_FILES(sys/types.h     HAVE_SYS_TYPES_H)                   # types.h
CHECK_INCLUDE_FILES(unistd.h        HAVE_UNISTD_H)                      # unistd.h
CHECK_INCLUDE_FILES(malloc.h        HAVE_MALLOC_H)                      # malloc.h

FINDFFMPEGSCALE("swscale")


CONFIGURE_FILE(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)


# do config checks
########################################
# check dependencies
########################################
MESSAGE(STATUS "Checking dependencies" )
FINDXINE("xine.h" "xine")
FINDLIBMPEG2("mpeg2dec/mpeg2.h" "mpeg2")
FIND_LIBRARY(LIBMPEG2CONVERT_LIBRARY NAMES mpeg2convert PATHS )
IF (LIBMPEG2CONVERT_LIBRARY)
      MESSAGE(STATUS "Found library mpeg2convert: ${LIBMPEG2CONVERT_LIBRARY}")
ELSE (LIBMPEG2CONVERT_LIBRARY)
      MESSAGE(FATAL_ERROR "Could not find mpeg2convert")
ENDIF (LIBMPEG2CONVERT_LIBRARY)

FIND_LIBRARY(LIBDL_LIBRARY NAMES "dl" PATHS )
IF (LIBDL_LIBRARY)
      MESSAGE(STATUS "Found library libdl: ${LIBDL_LIBRARY}")
ELSE (LIBDL_LIBRARY)
  FIND_LIBRARY(LIBDL_LIBRARY NAMES "c" PATHS )
  MESSAGE(STATUS "Found library libc: ${LIBDL_LIBRARY}")
ENDIF (LIBDL_LIBRARY)
#FINDLIBDVDREAD("dvdread/ifo_types.h" "dvdread")


FINDAVFORMAT("avformat")
FINDAVCODEC("avcodec")

add_definitions( -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)
add_subdirectory( src/dvdnav)
add_subdirectory( src/core )
add_subdirectory( src/mpeg2)
add_subdirectory(src/vamps)
add_subdirectory(src/devices)
add_subdirectory(src/mplayer)
add_subdirectory(src/assistant)
add_subdirectory(src/import)
add_subdirectory(src/batch)
add_subdirectory(icons)
add_subdirectory(data)
add_subdirectory(po)
add_subdirectory(doc)

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/dvdnav
 ${CMAKE_CURRENT_SOURCE_DIR}/src/devices
 ${CMAKE_CURRENT_SOURCE_DIR}/src/core
 ${CMAKE_CURRENT_SOURCE_DIR}/src/vamps
 ${CMAKE_CURRENT_SOURCE_DIR}/src/backup
 ${CMAKE_CURRENT_SOURCE_DIR}/src/mpeg2
 ${CMAKE_CURRENT_SOURCE_DIR}/src/main
 ${CMAKE_CURRENT_SOURCE_DIR}/src/mplayer
 ${CMAKE_CURRENT_SOURCE_DIR}/src/assistant
 ${CMAKE_CURRENT_SOURCE_DIR}/src/import
 ${CMAKE_CURRENT_SOURCE_DIR}/src/player
 ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitles
 ${CMAKE_CURRENT_SOURCE_DIR}/src/dvdread
 ${CMAKE_CURRENT_SOURCE_DIR}/src/batch
${KDE4_INCLUDES} ${QT_INCLUDES}
#${LIBDVDREAD_INCLUDE_DIR}/dvdread
 ${AVCODEC_INCLUDE_DIR}
 ${AVFORMAT_INCLUDE_DIR}
 ${FFMPEGSCALE_INCLUDE_DIR}
 ${FFMPEG_INCLUDE_DIR}
)


set (devices_SRCS
   src/devices/k9cddrive.cpp
)

set (subtitles_SRCS
   src/subtitles/k9subtitle2pgm.cpp
)

set (libdvdnav_SRCS
   src/dvdnav/decoder.c
   src/dvdnav/dvdnav.c
   src/dvdnav/highlight.c
   src/dvdnav/navigation.c
   src/dvdnav/read_cache.c
   src/dvdnav/remap.c
   src/dvdnav/searching.c
   src/dvdnav/settings.c
   src/dvdnav/vm.c
   src/dvdnav/vmcmd.c
)

set (vamps_SRCS
  src/vamps/k9fifo.cpp
  src/vamps/k9requant.cpp
  src/vamps/k9vamps.cpp
)

set (dvdread_SRCS
   src/dvdread/dvd_input.c
   src/dvdread/dvd_reader.c
   src/dvdread/dvd_udf.c
   src/dvdread/ifo_read.c
   src/dvdread/md5.c
   src/dvdread/nav_read.c
)


set (core_SRCS
  src/core/k9dvd.cpp
  src/core/k9dvdtitle.cpp
  src/core/k9dvdtitleset.cpp
  src/core/k9dvdchapter.cpp
  src/core/k9dvdsize.cpp
  src/core/k9process.cpp
  src/core/k9tools.cpp
  src/core/k9dvdread.cpp
  src/core/k9cellcopylist.cpp
  src/core/k9cell.cpp
  src/core/k9dvdprogress.cpp
  src/core/k9ifo2.cpp
  src/core/k9titleencopt.cpp
  src/core/k9config.cpp
  src/core/k9saveimage.cpp
  src/core/k9drawimage.cpp
  src/core/k9progress.cpp
  src/core/k9burnprogress.cpp
  src/core/k9audiocodecs.cpp
  src/core/k9videocodecs.cpp
  src/core/k9burndvd.cpp
  src/core/k9dvdbackupinterface.cpp
  src/core/k9dialogs.cpp
  src/core/k9log.cpp
  src/core/k9logview.cpp
  )
kde4_add_ui_files(core_SRCS
  src/core/dvdprogress.ui
  src/core/progress.ui
  src/core/k9logview.ui

  )

set (backup_SRCS
   src/backup/k9dvdbackup.cpp
   src/backup/k9script.cpp
   src/backup/k9backupdlg.cpp
   src/backup/k9dvdauthor.cpp
   src/backup/k9mp4enc.cpp
   src/backup/k9mp4dlg.cpp
   src/backup/k9execcopy.cpp
   src/backup/k9convertaudio.cpp
   src/player/k9play.cpp
)

kde4_add_ui_files(backup_SRCS
   src/backup/backupdlg.ui
   src/backup/mp4dlg.ui
)

set (mpeg2_SRCS
   src/mpeg2/k9decodethread.cpp
   src/mpeg2/k9plaympeg2.cpp
   src/mpeg2/kdecmpeg2.cpp
)

set (mplayer_SRCS
   src/mplayer/k9mplayer.cpp
   src/mplayer/k9xine.cpp
   src/mplayer/k9phonon.cpp
)

kde4_add_ui_files(mplayer_SRCS
   src/mplayer/mplayer.ui
   src/mplayer/phonon.ui
)


set (import_SRCS
   src/import/k9chapteredit.cpp
   src/import/k9avidecode.cpp
   src/import/k9avifile.cpp
   src/import/k9import.cpp
   src/import/k9lvitemimport.cpp
   src/import/k9menubutton.cpp
   src/import/k9menu.cpp
   src/import/k9menuedit.cpp
   src/import/k9menuscene.cpp
   src/import/k9newdvd.cpp
   src/import/k9newtitle.cpp
   src/import/k9title.cpp
   src/import/k9processlist.cpp
   src/import/k9importfiles.cpp
)

kde4_add_ui_files(import_SRCS
   src/import/chapterEdit.ui
   src/import/import.ui
   src/import/menuEdit.ui
   src/import/newTitle.ui
   src/import/processList.ui
)
set (assistant_SRCS
   src/assistant/k9assistant.cpp
   src/assistant/k9astsource.cpp
   src/assistant/k9astdestination.cpp
   src/assistant/k9asttitles.cpp
   src/assistant/k9aststreams.cpp
   src/assistant/k9astmp4.cpp
   src/assistant/k9astmpeg.cpp
   src/assistant/k9astdvdoption.cpp
)

kde4_add_ui_files(assistant_SRCS
   src/assistant/astsource.ui
   src/assistant/astdestination.ui
   src/assistant/asttitles.ui
   src/assistant/aststreams.ui
   src/assistant/astmp4.ui
   src/assistant/astmpeg.ui
   src/assistant/astdvdoption.ui
)


set (main_SRCS
   src/main/k9main.cpp
   src/main/k9langselect.cpp
   src/main/k9playbackoptions.cpp
   src/main/k9updatefactor.cpp
   src/main/kviewmpeg2.cpp
   src/main/k9cropselect.cpp
   src/main/k9titlefactor.cpp
   src/main/k9mp4title.cpp
   src/main/k9settings.cpp
   src/main/k9copy.cpp
   src/main/kconfigdlg.cpp
   src/main/k9prefdvd.cpp
   src/main/k9prefmencoder.cpp
   src/main/k9mencodercmdgen.cpp
   src/main/k9ffmpegcmdgen.cpp
   src/main/k9prefmpeg4.cpp
   src/main/k9prefpreview.cpp
   src/main/k9prefauthor.cpp
   src/main/k9prefdirs.cpp
   src/batch/k9batch.cpp
)

set (player_SRCS
        src/player/main.cpp
)

kde4_add_ui_files(main_SRCS
   src/main/k9mainw.ui
   src/main/configDlg.ui
   src/main/playbackoptionsw.ui
   src/main/prefDVD.ui
   src/main/prefMPEG4.ui
   src/main/prefMencoder.ui
   src/main/prefAuthor.ui
   src/main/prefpreview.ui
   src/main/langselectw.ui
   src/main/viewmpeg2.ui
   src/main/titlefactor.ui
   src/main/mencoderCmdGen.ui
   src/main/ffmpegCmdGen.ui
   src/main/prefDirs.ui
)

set (batch_SRCS
      src/batch/main.cpp
)

set (k9copylib_SRCS
   ${libdvdnav_SRCS}
   ${core_SRCS}
   ${vamps_SRCS}
   ${mpeg2_SRCS}
   ${subtitles_SRCS}
   ${dvdread_SRCS}
   ${backup_SRCS}
)

set(k9copy_SRCS
   ${devices_SRCS}
   ${mplayer_SRCS}
   ${import_SRCS}
   ${main_SRCS}
   ${assistant_SRCS}
   main.cpp

 )

set(k9play_SRCS
  ${player_SRCS}
 )


set (k9xineplayer_SRCS
   src/xine/main.cpp
   src/xine/k9xineplayer.cpp
)



kde4_add_library(k9copylib STATIC ${k9copylib_SRCS})
target_link_libraries(k9copylib ${KDE4_KDEUI_LIBS} ${KDE4_KDESU_LIBS} ${KDE4_KIO_LIBS} ${LIBMPEG2_LIBRARY} ${LIBMPEG2CONVERT_LIBRARY} ${LIBDL_LIBRARY})

kde4_add_ui_files(k9copy_SRCS )

kde4_add_executable(k9copy ${k9copy_SRCS})
kde4_add_executable(k9play ${k9play_SRCS})
kde4_add_executable(k9xineplayer ${k9xineplayer_SRCS})

 MESSAGE(STATUS "Found library solid: ${KDE4_SOLID_LIBS}")

target_link_libraries(k9copy k9copylib ${KDE4_KDEUI_LIBS} ${PHONON_LIBRARY} ${KDE4_SOLID_LIBS})
target_link_libraries(k9play k9copylib ${KDE4_KDEUI_LIBS}  )
target_link_libraries(k9xineplayer ${KDE4_KDEUI_LIBS} ${XINE_LIBRARY} ${X11_X11_LIB} )

install(TARGETS k9copy DESTINATION ${BIN_INSTALL_DIR} )
install(TARGETS k9play DESTINATION ${BIN_INSTALL_DIR} )
install(TARGETS k9xineplayer DESTINATION ${BIN_INSTALL_DIR} )
########### install files ###############

install( FILES k9copy.desktop k9copy_assistant.desktop DESTINATION  ${XDG_APPS_INSTALL_DIR} )
#install( FILES k9copy.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES k9copyui.rc  DESTINATION  ${DATA_INSTALL_DIR}/k9copy )

install(FILES k9copy_open.desktop DESTINATION ${DATA_INSTALL_DIR}/solid/actions)
install(FILES k9copy_assistant_open.desktop DESTINATION ${DATA_INSTALL_DIR}/solid/actions)

kde4_install_icons( ${ICON_INSTALL_DIR} )

############package###############
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "8")
SET(CPACK_PACKAGE_NAME "k9copy")
SET(CPACK_SOURCE_IGNORE_FILES
"moc_"
"\\\\.a"
"\\\\.moc"
"\\\\.gmo"
"\\\\.ts"
"k9copy$"
"k9play$"
"k9xineplayer$"
"ui_"
"\\\\.gz"
"\\\\.tar.Z"
"\\\\.tar.bz2"
"\\\\.tar.xz"
"\\\\~$"
"\\\\kdevelop"
"\\\\automoc"
"Makefile$"
"TODO"
"CMakeCache.txt"
"install_manifest.txt"
"cmake_install.cmake"
"cmake_"
"CTest"
"CPack"
"CMakeLists.txt.user"
"/CVS/"
"/CMakeTmp/"
"/CMakeFiles/"
"/_CPack_Packages/"
"/templates/"
"/\\\\.svn/"
"\\\\.swp$"
"\\\\.#"
"/#"
"build"
)
#SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};K9Copy;ALL;/" )
INCLUDE(CPack)

