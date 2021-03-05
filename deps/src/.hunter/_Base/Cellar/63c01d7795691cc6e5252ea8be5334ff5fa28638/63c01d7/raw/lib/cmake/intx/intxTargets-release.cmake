#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "intx::intx" for configuration "Release"
set_property(TARGET intx::intx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(intx::intx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libintx.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS intx::intx )
list(APPEND _IMPORT_CHECK_FILES_FOR_intx::intx "${_IMPORT_PREFIX}/lib/libintx.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
