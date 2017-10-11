cmake_minimum_required(VERSION 3.0)

# Add API Reference generation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(
        "${wisund_SOURCE_DIR}/doc/doxygen.conf.in"
        "${CMAKE_BINARY_DIR}/doxygen.conf"
        @ONLY
    )
    configure_file(
        "${wisund_SOURCE_DIR}/doc/hardware.jpg"
        "${CMAKE_BINARY_DIR}/doc/hardware.jpg"
        COPYONLY
    )
    configure_file(
        "${wisund_SOURCE_DIR}/doc/epri-logo.jpg"
        "${CMAKE_BINARY_DIR}/doc/epri-logo.jpg"
        COPYONLY
    )
    configure_file(
        "${wisund_SOURCE_DIR}/doc/header.tex"
        "${CMAKE_BINARY_DIR}/doc/header.tex"
        COPYONLY
    )
    add_custom_target(
        doc
        "${DOXYGEN_EXECUTABLE}"
        "${CMAKE_BINARY_DIR}/doxygen.conf"
        DEPENDS "${CMAKE_BINARY_DIR}/doc/blocks.svg"
        DEPENDS "${CMAKE_BINARY_DIR}/doc/blocks.pdf"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating API Reference documentation..." VERBATIM
    )
    add_custom_target(
        pdf
        DEPENDS "${CMAKE_BINARY_DIR}/doc/latex/refman.pdf"
    )
    add_custom_command(
        OUTPUT  "${CMAKE_BINARY_DIR}/doc/latex/refman.pdf"
        COMMAND "make"
        DEPENDS "doc"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/doc/latex"
        COMMENT "Generating PDF format Reference documentation..." VERBATIM
    )

    # There must be a better way to do this!
    add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/doc/blocks.svg"
        COMMAND ${DOXYGEN_DOT_EXECUTABLE} -Tsvg ${wisund_SOURCE_DIR}/doc/blocks.dot -o${CMAKE_BINARY_DIR}/doc/blocks.svg
        DEPENDS "blocks.dot"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating drawings using graphviz..." 
    )
    add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/doc/blocks.pdf"
        COMMAND ${DOXYGEN_DOT_EXECUTABLE} -Tpdf ${wisund_SOURCE_DIR}/doc/blocks.dot -o${CMAKE_BINARY_DIR}/doc/blocks.pdf
        DEPENDS "blocks.dot"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating drawings using graphviz..." 
    )
endif(DOXYGEN_FOUND)
