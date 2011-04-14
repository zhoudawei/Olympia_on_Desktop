SET(DEFAULT 0)
SET(DEPENDS 1)

SET(WEBKIT_FEATURES "")

SET(HTML_FEATURES "")
SET(SVG_FEATURES "")

MACRO(WEBKIT_SEPARATE_FEATURES _name _value)
        FOREACH(f ${ARGN})
                LIST(APPEND ${f}_FEATURES "${_name}=${_value}")
        ENDFOREACH()
ENDMACRO(WEBKIT_SEPARATE_FEATURES)

MACRO(WEBKIT_FEATURE_CHECK_DEPS _deps _deps_ok)
        SET(${_deps_ok} TRUE)

	LIST(LENGTH ${_deps} ${_deps_ok}_LIST_SIZE)
	IF (${${_deps_ok}_LIST_SIZE} GREATER 1)
		FOREACH(_dep ${${_deps}})
			IF (NOT ${_dep})
				SET(${_deps_ok} FALSE)
				BREAK()
			ENDIF ()
		ENDFOREACH ()
	ELSE ()
		IF (NOT ${_deps})
			SET(${_deps_ok} FALSE)
		ENDIF ()
	ENDIF ()
ENDMACRO()

MACRO(WEBKIT_FEATURE _feature _description _type _args)
	IF (${_type} EQUAL ${DEPENDS})
                WEBKIT_FEATURE_CHECK_DEPS(${_args} DEPS_OK_${_feature})
		OPTION(${_feature} "${_description}" DEPS_OK_${_feature})
	ELSEIF (${_type} EQUAL ${DEFAULT})
		OPTION(${_feature} "${_description}" ${_args})
	ENDIF ()
	
	LIST(APPEND WEBKIT_FEATURES ${_feature})

	IF (${_feature})
                ADD_DEFINITIONS(-D${_feature}=1)
		SET(${_feature}_VALUE 1)
		WEBKIT_SEPARATE_FEATURES(${_feature} 1 ${ARGN})
		LIST(APPEND FEATURE_DEFINES ${_feature})
	ELSE ()
                ADD_DEFINITIONS(-D${_feature}=0)
		SET(${_feature}_VALUE 0)
		WEBKIT_SEPARATE_FEATURES(${_feature} 0 ${ARGN})
	ENDIF ()
ENDMACRO()

MACRO(WEBKIT_PRINT_FEATURES)
	MESSAGE(STATUS "Enabled features:")

	SET(MAX_FEATURE_LENGTH 0)
	FOREACH (_feature ${WEBKIT_FEATURES})
		STRING(LENGTH ${_feature} FEATURE_LENGTH)
		IF (FEATURE_LENGTH GREATER MAX_FEATURE_LENGTH)
			SET(MAX_FEATURE_LENGTH ${FEATURE_LENGTH})
		ENDIF ()
	ENDFOREACH ()

	SET(ODD 0)
	FOREACH (_feature ${WEBKIT_FEATURES})
		STRING(LENGTH ${_feature} FEATURE_LENGTH)
		SET(SPACES "")

		IF (ODD)
			FOREACH (IGNORE RANGE ${FEATURE_LENGTH} ${MAX_FEATURE_LENGTH})
				SET(SPACES "${SPACES} ")
			ENDFOREACH ()
			SET(ODD 0)
		ELSE ()
			FOREACH (IGNORE RANGE ${FEATURE_LENGTH} ${MAX_FEATURE_LENGTH})
				SET(SPACES "${SPACES}.")
			ENDFOREACH ()
			SET(ODD 1)
		ENDIF ()

		IF (${_feature})
			MESSAGE(STATUS "  ${_feature} ${SPACES} ON")
		ELSE ()
			MESSAGE(STATUS "  ${_feature} ${SPACES} OFF")
		ENDIF ()
	ENDFOREACH ()

	UNSET(FEATURE_LENGTH)
	UNSET(MAX_FEATURE_LENGTH)
	UNSET(ODD)
ENDMACRO()
