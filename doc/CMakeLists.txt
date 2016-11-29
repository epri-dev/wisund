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
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating API Reference documentation..." VERBATIM
    )
endif(DOXYGEN_FOUND)
