# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/melodic/James_workspace/parallel/signal/mytbf

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/melodic/James_workspace/parallel/signal/mytbf/build

# Include any dependencies generated for this target.
include CMakeFiles/MyExecutable.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/MyExecutable.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/MyExecutable.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MyExecutable.dir/flags.make

CMakeFiles/MyExecutable.dir/src/mytbf.c.o: CMakeFiles/MyExecutable.dir/flags.make
CMakeFiles/MyExecutable.dir/src/mytbf.c.o: ../src/mytbf.c
CMakeFiles/MyExecutable.dir/src/mytbf.c.o: CMakeFiles/MyExecutable.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/melodic/James_workspace/parallel/signal/mytbf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/MyExecutable.dir/src/mytbf.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/MyExecutable.dir/src/mytbf.c.o -MF CMakeFiles/MyExecutable.dir/src/mytbf.c.o.d -o CMakeFiles/MyExecutable.dir/src/mytbf.c.o -c /home/melodic/James_workspace/parallel/signal/mytbf/src/mytbf.c

CMakeFiles/MyExecutable.dir/src/mytbf.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MyExecutable.dir/src/mytbf.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/melodic/James_workspace/parallel/signal/mytbf/src/mytbf.c > CMakeFiles/MyExecutable.dir/src/mytbf.c.i

CMakeFiles/MyExecutable.dir/src/mytbf.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MyExecutable.dir/src/mytbf.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/melodic/James_workspace/parallel/signal/mytbf/src/mytbf.c -o CMakeFiles/MyExecutable.dir/src/mytbf.c.s

CMakeFiles/MyExecutable.dir/src/main.c.o: CMakeFiles/MyExecutable.dir/flags.make
CMakeFiles/MyExecutable.dir/src/main.c.o: ../src/main.c
CMakeFiles/MyExecutable.dir/src/main.c.o: CMakeFiles/MyExecutable.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/melodic/James_workspace/parallel/signal/mytbf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/MyExecutable.dir/src/main.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/MyExecutable.dir/src/main.c.o -MF CMakeFiles/MyExecutable.dir/src/main.c.o.d -o CMakeFiles/MyExecutable.dir/src/main.c.o -c /home/melodic/James_workspace/parallel/signal/mytbf/src/main.c

CMakeFiles/MyExecutable.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MyExecutable.dir/src/main.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/melodic/James_workspace/parallel/signal/mytbf/src/main.c > CMakeFiles/MyExecutable.dir/src/main.c.i

CMakeFiles/MyExecutable.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MyExecutable.dir/src/main.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/melodic/James_workspace/parallel/signal/mytbf/src/main.c -o CMakeFiles/MyExecutable.dir/src/main.c.s

# Object files for target MyExecutable
MyExecutable_OBJECTS = \
"CMakeFiles/MyExecutable.dir/src/mytbf.c.o" \
"CMakeFiles/MyExecutable.dir/src/main.c.o"

# External object files for target MyExecutable
MyExecutable_EXTERNAL_OBJECTS =

MyExecutable: CMakeFiles/MyExecutable.dir/src/mytbf.c.o
MyExecutable: CMakeFiles/MyExecutable.dir/src/main.c.o
MyExecutable: CMakeFiles/MyExecutable.dir/build.make
MyExecutable: CMakeFiles/MyExecutable.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/melodic/James_workspace/parallel/signal/mytbf/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable MyExecutable"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MyExecutable.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MyExecutable.dir/build: MyExecutable
.PHONY : CMakeFiles/MyExecutable.dir/build

CMakeFiles/MyExecutable.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MyExecutable.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MyExecutable.dir/clean

CMakeFiles/MyExecutable.dir/depend:
	cd /home/melodic/James_workspace/parallel/signal/mytbf/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/melodic/James_workspace/parallel/signal/mytbf /home/melodic/James_workspace/parallel/signal/mytbf /home/melodic/James_workspace/parallel/signal/mytbf/build /home/melodic/James_workspace/parallel/signal/mytbf/build /home/melodic/James_workspace/parallel/signal/mytbf/build/CMakeFiles/MyExecutable.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MyExecutable.dir/depend
