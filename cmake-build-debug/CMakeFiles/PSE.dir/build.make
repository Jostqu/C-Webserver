# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /snap/clion/69/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/69/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bjoern/CLionProjects/PSE

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bjoern/CLionProjects/PSE/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/PSE.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/PSE.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/PSE.dir/flags.make

CMakeFiles/PSE.dir/echo_server.c.o: CMakeFiles/PSE.dir/flags.make
CMakeFiles/PSE.dir/echo_server.c.o: ../echo_server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bjoern/CLionProjects/PSE/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/PSE.dir/echo_server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/PSE.dir/echo_server.c.o   -c /home/bjoern/CLionProjects/PSE/echo_server.c

CMakeFiles/PSE.dir/echo_server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/PSE.dir/echo_server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/bjoern/CLionProjects/PSE/echo_server.c > CMakeFiles/PSE.dir/echo_server.c.i

CMakeFiles/PSE.dir/echo_server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/PSE.dir/echo_server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/bjoern/CLionProjects/PSE/echo_server.c -o CMakeFiles/PSE.dir/echo_server.c.s

CMakeFiles/PSE.dir/string.c.o: CMakeFiles/PSE.dir/flags.make
CMakeFiles/PSE.dir/string.c.o: ../string.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bjoern/CLionProjects/PSE/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/PSE.dir/string.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/PSE.dir/string.c.o   -c /home/bjoern/CLionProjects/PSE/string.c

CMakeFiles/PSE.dir/string.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/PSE.dir/string.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/bjoern/CLionProjects/PSE/string.c > CMakeFiles/PSE.dir/string.c.i

CMakeFiles/PSE.dir/string.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/PSE.dir/string.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/bjoern/CLionProjects/PSE/string.c -o CMakeFiles/PSE.dir/string.c.s

CMakeFiles/PSE.dir/hash.c.o: CMakeFiles/PSE.dir/flags.make
CMakeFiles/PSE.dir/hash.c.o: ../hash.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bjoern/CLionProjects/PSE/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/PSE.dir/hash.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/PSE.dir/hash.c.o   -c /home/bjoern/CLionProjects/PSE/hash.c

CMakeFiles/PSE.dir/hash.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/PSE.dir/hash.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/bjoern/CLionProjects/PSE/hash.c > CMakeFiles/PSE.dir/hash.c.i

CMakeFiles/PSE.dir/hash.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/PSE.dir/hash.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/bjoern/CLionProjects/PSE/hash.c -o CMakeFiles/PSE.dir/hash.c.s

CMakeFiles/PSE.dir/response.c.o: CMakeFiles/PSE.dir/flags.make
CMakeFiles/PSE.dir/response.c.o: ../response.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bjoern/CLionProjects/PSE/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/PSE.dir/response.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/PSE.dir/response.c.o   -c /home/bjoern/CLionProjects/PSE/response.c

CMakeFiles/PSE.dir/response.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/PSE.dir/response.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/bjoern/CLionProjects/PSE/response.c > CMakeFiles/PSE.dir/response.c.i

CMakeFiles/PSE.dir/response.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/PSE.dir/response.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/bjoern/CLionProjects/PSE/response.c -o CMakeFiles/PSE.dir/response.c.s

# Object files for target PSE
PSE_OBJECTS = \
"CMakeFiles/PSE.dir/echo_server.c.o" \
"CMakeFiles/PSE.dir/string.c.o" \
"CMakeFiles/PSE.dir/hash.c.o" \
"CMakeFiles/PSE.dir/response.c.o"

# External object files for target PSE
PSE_EXTERNAL_OBJECTS =

PSE: CMakeFiles/PSE.dir/echo_server.c.o
PSE: CMakeFiles/PSE.dir/string.c.o
PSE: CMakeFiles/PSE.dir/hash.c.o
PSE: CMakeFiles/PSE.dir/response.c.o
PSE: CMakeFiles/PSE.dir/build.make
PSE: CMakeFiles/PSE.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bjoern/CLionProjects/PSE/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable PSE"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PSE.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/PSE.dir/build: PSE

.PHONY : CMakeFiles/PSE.dir/build

CMakeFiles/PSE.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PSE.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PSE.dir/clean

CMakeFiles/PSE.dir/depend:
	cd /home/bjoern/CLionProjects/PSE/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bjoern/CLionProjects/PSE /home/bjoern/CLionProjects/PSE /home/bjoern/CLionProjects/PSE/cmake-build-debug /home/bjoern/CLionProjects/PSE/cmake-build-debug /home/bjoern/CLionProjects/PSE/cmake-build-debug/CMakeFiles/PSE.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PSE.dir/depend

