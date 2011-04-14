MACRO(INCLUDE_IF_EXISTS _file)
    IF (EXISTS ${_file})
        MESSAGE(STATUS "Using platform-specific CMakeLists: ${_file}")
	INCLUDE(${_file})
    ELSE ()
        MESSAGE(STATUS "Platform-specific CMakeLists not found: ${_file}")
    ENDIF ()
ENDMACRO()

# Append the given dependencies to the source file
MACRO(ADD_SOURCE_DEPENDENCIES _source _deps)
    GET_SOURCE_FILE_PROPERTY (_tmp ${_source} OBJECT_DEPENDS)
    IF (NOT _tmp)
        SET (_tmp "")
    ENDIF ()

    FOREACH (f ${_deps})
        LIST(APPEND _tmp "${f}")
    ENDFOREACH ()

    SET_SOURCE_FILES_PROPERTIES (${_source} PROPERTIES OBJECT_DEPENDS "${_tmp}")
    UNSET(_tmp)
ENDMACRO()

MACRO(WEBKIT_WRAP_SOURCELIST _input)
    IF (WTF_PLATFORM_QT)
        FOREACH (_file ${ARGN})
            GET_FILENAME_COMPONENT(_basename ${_file} NAME_WE)
            SET(_moc_filename ${DERIVED_SOURCES_DIR}/${_basename}.moc)

            FILE(READ ${_file} _contents)

            STRING(REGEX MATCHALL "#include[ ]+\"${_basename}\\.moc\"" _match "${_contents}")
            IF (_match)
                QT4_GENERATE_MOC(${_file} ${_moc_filename})
                ADD_SOURCE_DEPENDENCIES(${_file} ${_moc_filename})
            ENDIF()
        ENDFOREACH ()
    ENDIF ()
ENDMACRO()
