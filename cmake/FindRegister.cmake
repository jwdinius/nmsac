#.rst:
# FindRegister
# -------------
#
# Find Register
#
# Find the Register C++ library
#
#   find_package(Register REQUIRED)
#   include_directories(${Register_INCLUDE_DIRS})
#   add_executable(foo foo.cc)
#   target_link_libraries(foo ${Register_LIBRARIES})
#
# This module sets the following variables::
#
#   Register_FOUND - set to true if the library is found
#   Register_INCLUDE_DIRS - list of required include directories
#   Register_LIBRARIES - list of libraries to be linked
include(FindPackageHandleStandardArgs)

# UNIX paths are standard, no need to specify them.
find_library(Register_LIBRARY
	NAMES registration
	PATHS "$ENV{ProgramFiles}/lib"
)
find_path(Register_INCLUDE_DIR
	NAMES correspondences/correspondences.hpp registration/registration.hpp.hpp
	PATHS "$ENV{ProgramFiles}"
)

find_package_handle_standard_args(Register
	REQUIRED_VARS Register_LIBRARY Register_INCLUDE_DIR
)

if(Register_FOUND)
	set(Register_INCLUDE_DIRS ${Register_INCLUDE_DIR})
	set(Register_LIBRARIES ${Register_LIBRARY})
endif()

# Hide internal variables
mark_as_advanced(
	Register_INCLUDE_DIR
	Register_LIBRARY
)
