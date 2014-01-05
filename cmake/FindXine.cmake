MACRO (FINDXINE _include _lib)

FIND_PATH(XINE_INCLUDE_DIR ${_include} PATHS)
FIND_LIBRARY(XINE_LIBRARY NAMES ${_lib} PATHS ) 

IF (XINE_INCLUDE_DIR AND XINE_LIBRARY)
   SET(INC_FOUND TRUE)
ELSE (XINE_INCLUDE_DIR AND XINE_LIBRARY)
   SET(INC_FOUND FALSE)
ENDIF (XINE_INCLUDE_DIR AND XINE_LIBRARY)


IF (INC_FOUND)
      MESSAGE(STATUS "Found library ${_lib}: ${XINE_LIBRARY}")
      MESSAGE(STATUS "Found include ${_include} : ${XINE_INCLUDE_DIR}")
ELSE (INC_FOUND)
      MESSAGE(FATAL_ERROR "Could not find ${_lib}")
ENDIF (INC_FOUND)
ENDMACRO (FINDXINE _include _lib )