# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/xeniro/fisco-bco/FISCO-BCOS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xeniro/fisco-bco/FISCO-BCOS/build

# Include any dependencies generated for this target.
include libeventfilter/CMakeFiles/eventfilter.dir/depend.make

# Include the progress variables for this target.
include libeventfilter/CMakeFiles/eventfilter.dir/progress.make

# Include the compile flags for this target's objects.
include libeventfilter/CMakeFiles/eventfilter.dir/flags.make

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o: libeventfilter/CMakeFiles/eventfilter.dir/flags.make
libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o: ../libeventfilter/EventLogFilter.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o -c /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter/EventLogFilter.cpp

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/eventfilter.dir/EventLogFilter.cpp.i"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter/EventLogFilter.cpp > CMakeFiles/eventfilter.dir/EventLogFilter.cpp.i

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/eventfilter.dir/EventLogFilter.cpp.s"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter/EventLogFilter.cpp -o CMakeFiles/eventfilter.dir/EventLogFilter.cpp.s

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o.requires:

.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o.requires

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o.provides: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o.requires
	$(MAKE) -f libeventfilter/CMakeFiles/eventfilter.dir/build.make libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o.provides.build
.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o.provides

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o.provides.build: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o


libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o: libeventfilter/CMakeFiles/eventfilter.dir/flags.make
libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o: ../libeventfilter/EventLogFilterManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o -c /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter/EventLogFilterManager.cpp

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.i"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter/EventLogFilterManager.cpp > CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.i

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.s"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter/EventLogFilterManager.cpp -o CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.s

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o.requires:

.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o.requires

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o.provides: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o.requires
	$(MAKE) -f libeventfilter/CMakeFiles/eventfilter.dir/build.make libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o.provides.build
.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o.provides

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o.provides.build: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o


libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o: libeventfilter/CMakeFiles/eventfilter.dir/flags.make
libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o: ../libeventfilter/EventLogFilterParams.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o -c /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter/EventLogFilterParams.cpp

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.i"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter/EventLogFilterParams.cpp > CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.i

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.s"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter/EventLogFilterParams.cpp -o CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.s

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o.requires:

.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o.requires

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o.provides: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o.requires
	$(MAKE) -f libeventfilter/CMakeFiles/eventfilter.dir/build.make libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o.provides.build
.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o.provides

libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o.provides.build: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o


# Object files for target eventfilter
eventfilter_OBJECTS = \
"CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o" \
"CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o" \
"CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o"

# External object files for target eventfilter
eventfilter_EXTERNAL_OBJECTS =

libeventfilter/libeventfilter.a: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o
libeventfilter/libeventfilter.a: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o
libeventfilter/libeventfilter.a: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o
libeventfilter/libeventfilter.a: libeventfilter/CMakeFiles/eventfilter.dir/build.make
libeventfilter/libeventfilter.a: libeventfilter/CMakeFiles/eventfilter.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libeventfilter.a"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && $(CMAKE_COMMAND) -P CMakeFiles/eventfilter.dir/cmake_clean_target.cmake
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/eventfilter.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libeventfilter/CMakeFiles/eventfilter.dir/build: libeventfilter/libeventfilter.a

.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/build

libeventfilter/CMakeFiles/eventfilter.dir/requires: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilter.cpp.o.requires
libeventfilter/CMakeFiles/eventfilter.dir/requires: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterManager.cpp.o.requires
libeventfilter/CMakeFiles/eventfilter.dir/requires: libeventfilter/CMakeFiles/eventfilter.dir/EventLogFilterParams.cpp.o.requires

.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/requires

libeventfilter/CMakeFiles/eventfilter.dir/clean:
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter && $(CMAKE_COMMAND) -P CMakeFiles/eventfilter.dir/cmake_clean.cmake
.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/clean

libeventfilter/CMakeFiles/eventfilter.dir/depend:
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xeniro/fisco-bco/FISCO-BCOS /home/xeniro/fisco-bco/FISCO-BCOS/libeventfilter /home/xeniro/fisco-bco/FISCO-BCOS/build /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter /home/xeniro/fisco-bco/FISCO-BCOS/build/libeventfilter/CMakeFiles/eventfilter.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libeventfilter/CMakeFiles/eventfilter.dir/depend

