########### install files ###############
#
IF (NOT BACKLITE)
kde4_create_handbook(index.docbook INSTALL_DESTINATION ${HTML_INSTALL_DIR}/en SUBDIR k9copy)
ENDIF (NOT BACKLITE)