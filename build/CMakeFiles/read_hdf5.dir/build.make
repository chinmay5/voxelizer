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
CMAKE_SOURCE_DIR = /home/chinmay/softwares/mesh-voxelization

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chinmay/softwares/mesh-voxelization/build

# Include any dependencies generated for this target.
include CMakeFiles/read_hdf5.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/read_hdf5.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/read_hdf5.dir/flags.make

CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o: CMakeFiles/read_hdf5.dir/flags.make
CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o: ../examples/read_hdf5.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/chinmay/softwares/mesh-voxelization/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o -c /home/chinmay/softwares/mesh-voxelization/examples/read_hdf5.cpp

CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chinmay/softwares/mesh-voxelization/examples/read_hdf5.cpp > CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.i

CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chinmay/softwares/mesh-voxelization/examples/read_hdf5.cpp -o CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.s

CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o.requires:

.PHONY : CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o.requires

CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o.provides: CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o.requires
	$(MAKE) -f CMakeFiles/read_hdf5.dir/build.make CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o.provides.build
.PHONY : CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o.provides

CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o.provides.build: CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o


# Object files for target read_hdf5
read_hdf5_OBJECTS = \
"CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o"

# External object files for target read_hdf5
read_hdf5_EXTERNAL_OBJECTS =

../bin/read_hdf5: CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o
../bin/read_hdf5: CMakeFiles/read_hdf5.dir/build.make
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/libboost_system.so
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/libboost_program_options.so
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_cpp.so
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5.so
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/libpthread.so
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/libsz.so
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/libz.so
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/libdl.so
../bin/read_hdf5: /usr/lib/x86_64-linux-gnu/libm.so
../bin/read_hdf5: CMakeFiles/read_hdf5.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/chinmay/softwares/mesh-voxelization/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/read_hdf5"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/read_hdf5.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/read_hdf5.dir/build: ../bin/read_hdf5

.PHONY : CMakeFiles/read_hdf5.dir/build

CMakeFiles/read_hdf5.dir/requires: CMakeFiles/read_hdf5.dir/examples/read_hdf5.cpp.o.requires

.PHONY : CMakeFiles/read_hdf5.dir/requires

CMakeFiles/read_hdf5.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/read_hdf5.dir/cmake_clean.cmake
.PHONY : CMakeFiles/read_hdf5.dir/clean

CMakeFiles/read_hdf5.dir/depend:
	cd /home/chinmay/softwares/mesh-voxelization/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chinmay/softwares/mesh-voxelization /home/chinmay/softwares/mesh-voxelization /home/chinmay/softwares/mesh-voxelization/build /home/chinmay/softwares/mesh-voxelization/build /home/chinmay/softwares/mesh-voxelization/build/CMakeFiles/read_hdf5.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/read_hdf5.dir/depend

