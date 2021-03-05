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

# Utility rule file for rocksdb.

# Include the progress variables for this target.
include CMakeFiles/rocksdb.dir/progress.make

CMakeFiles/rocksdb: CMakeFiles/rocksdb-complete


CMakeFiles/rocksdb-complete: ../deps/src/rocksdb-stamp/rocksdb-install
CMakeFiles/rocksdb-complete: ../deps/src/rocksdb-stamp/rocksdb-mkdir
CMakeFiles/rocksdb-complete: ../deps/src/rocksdb-stamp/rocksdb-download
CMakeFiles/rocksdb-complete: ../deps/src/rocksdb-stamp/rocksdb-update
CMakeFiles/rocksdb-complete: ../deps/src/rocksdb-stamp/rocksdb-patch
CMakeFiles/rocksdb-complete: ../deps/src/rocksdb-stamp/rocksdb-configure
CMakeFiles/rocksdb-complete: ../deps/src/rocksdb-stamp/rocksdb-build
CMakeFiles/rocksdb-complete: ../deps/src/rocksdb-stamp/rocksdb-install
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Completed 'rocksdb'"
	/usr/bin/cmake -E make_directory /home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles
	/usr/bin/cmake -E touch /home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles/rocksdb-complete
	/usr/bin/cmake -E touch /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-done

../deps/src/rocksdb-stamp/rocksdb-install: ../deps/src/rocksdb-stamp/rocksdb-build
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "No install step for 'rocksdb'"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && /usr/bin/cmake -E echo_append
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && /usr/bin/cmake -E touch /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-install

../deps/src/rocksdb-stamp/rocksdb-mkdir:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Creating directories for 'rocksdb'"
	/usr/bin/cmake -E make_directory /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb
	/usr/bin/cmake -E make_directory /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb
	/usr/bin/cmake -E make_directory /home/xeniro/fisco-bco/FISCO-BCOS/deps
	/usr/bin/cmake -E make_directory /home/xeniro/fisco-bco/FISCO-BCOS/deps/tmp
	/usr/bin/cmake -E make_directory /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp
	/usr/bin/cmake -E make_directory /home/xeniro/fisco-bco/FISCO-BCOS/deps/src
	/usr/bin/cmake -E touch /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-mkdir

../deps/src/rocksdb-stamp/rocksdb-download: ../deps/src/rocksdb-stamp/rocksdb-urlinfo.txt
../deps/src/rocksdb-stamp/rocksdb-download: ../deps/src/rocksdb-stamp/rocksdb-mkdir
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Performing download step (download, verify and extract) for 'rocksdb'"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src && /usr/bin/cmake -P /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-download-RelWithDebInfo.cmake
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src && /usr/bin/cmake -E touch /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-download

../deps/src/rocksdb-stamp/rocksdb-update: ../deps/src/rocksdb-stamp/rocksdb-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "No update step for 'rocksdb'"
	/usr/bin/cmake -E echo_append
	/usr/bin/cmake -E touch /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-update

../deps/src/rocksdb-stamp/rocksdb-patch: ../deps/src/rocksdb-stamp/rocksdb-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Performing patch step for 'rocksdb'"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && sed -i "s#-march=native#-march=x86-64 -mtune=generic -fvisibility=hidden -fvisibility-inlines-hidden  #g" CMakeLists.txt
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && sed -i 464d CMakeLists.txt
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && sed -i 739,749d CMakeLists.txt
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && sed -i 805,813d CMakeLists.txt
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && sed -i "s#-Werror##g" CMakeLists.txt
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && /usr/bin/cmake -E touch /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-patch

../deps/src/rocksdb-stamp/rocksdb-configure: ../deps/tmp/rocksdb-cfgcmd.txt
../deps/src/rocksdb-stamp/rocksdb-configure: ../deps/src/rocksdb-stamp/rocksdb-update
../deps/src/rocksdb-stamp/rocksdb-configure: ../deps/src/rocksdb-stamp/rocksdb-patch
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Performing configure step for 'rocksdb'"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && /usr/bin/cmake -P /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-configure-RelWithDebInfo.cmake
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && /usr/bin/cmake -E touch /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-configure

../deps/src/rocksdb-stamp/rocksdb-build: ../deps/src/rocksdb-stamp/rocksdb-configure
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Performing build step for 'rocksdb'"
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && /usr/bin/cmake -Dmake=$(MAKE) -P /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-build-RelWithDebInfo.cmake
	cd /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb && /usr/bin/cmake -E touch /home/xeniro/fisco-bco/FISCO-BCOS/deps/src/rocksdb-stamp/rocksdb-build

rocksdb: CMakeFiles/rocksdb
rocksdb: CMakeFiles/rocksdb-complete
rocksdb: ../deps/src/rocksdb-stamp/rocksdb-install
rocksdb: ../deps/src/rocksdb-stamp/rocksdb-mkdir
rocksdb: ../deps/src/rocksdb-stamp/rocksdb-download
rocksdb: ../deps/src/rocksdb-stamp/rocksdb-update
rocksdb: ../deps/src/rocksdb-stamp/rocksdb-patch
rocksdb: ../deps/src/rocksdb-stamp/rocksdb-configure
rocksdb: ../deps/src/rocksdb-stamp/rocksdb-build
rocksdb: CMakeFiles/rocksdb.dir/build.make

.PHONY : rocksdb

# Rule to build all files generated by this target.
CMakeFiles/rocksdb.dir/build: rocksdb

.PHONY : CMakeFiles/rocksdb.dir/build

CMakeFiles/rocksdb.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/rocksdb.dir/cmake_clean.cmake
.PHONY : CMakeFiles/rocksdb.dir/clean

CMakeFiles/rocksdb.dir/depend:
	cd /home/xeniro/fisco-bco/FISCO-BCOS/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xeniro/fisco-bco/FISCO-BCOS /home/xeniro/fisco-bco/FISCO-BCOS /home/xeniro/fisco-bco/FISCO-BCOS/build /home/xeniro/fisco-bco/FISCO-BCOS/build /home/xeniro/fisco-bco/FISCO-BCOS/build/CMakeFiles/rocksdb.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/rocksdb.dir/depend

