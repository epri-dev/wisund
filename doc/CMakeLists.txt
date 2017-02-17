cmake_minimum_required(VERSION 3.0)

# Add API Reference generation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(
        "${testmode_SOURCE_DIR}/doc/doxygen.conf.in"
        "${CMAKE_BINARY_DIR}/doxygen.conf"
        @ONLY
    )
    configure_file(
        "${testmode_SOURCE_DIR}/doc/hardware.jpg"
        "${CMAKE_BINARY_DIR}/doc/hardware.jpg"
        COPYONLY
    )
    configure_file(
        "${testmode_SOURCE_DIR}/doc/epri-logo.jpg"
        "${CMAKE_BINARY_DIR}/doc/epri-logo.jpg"
        COPYONLY
    )
    add_custom_target(
        doc
        "${DOXYGEN_EXECUTABLE}"
        "${CMAKE_BINARY_DIR}/doxygen.conf"
        DEPENDS "${CMAKE_BINARY_DIR}/doc/blocks.dot.svg"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating API Reference documentation..." VERBATIM
    )

    # There must be a better way to do this!
    add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/doc/blocks.dot.svg"
        COMMAND ${DOXYGEN_DOT_EXECUTABLE} -Tsvg ${testmode_SOURCE_DIR}/doc/blocks.dot -o${CMAKE_BINARY_DIR}/doc/blocks.dot.svg
        DEPENDS "blocks.dot"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating drawings using graphviz..." 
    )
endif(DOXYGEN_FOUND)

