file(GLOB_RECURSE VERT ${SOURCE_DIR}/shaders/*.vert)
file(GLOB_RECURSE FRAG ${SOURCE_DIR}/shaders/*.frag)

file( COPY ${VERT} ${FRAG} DESTINATION ${DESTINATION_DIR}/shaders )