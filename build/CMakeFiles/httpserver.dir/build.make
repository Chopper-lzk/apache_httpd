# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.23.0/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.23.0/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kevin/httpserver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kevin/httpserver/build

# Include any dependencies generated for this target.
include CMakeFiles/httpserver.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/httpserver.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/httpserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/httpserver.dir/flags.make

CMakeFiles/httpserver.dir/src/buildmark.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/buildmark.c.o: ../src/buildmark.c
CMakeFiles/httpserver.dir/src/buildmark.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/httpserver.dir/src/buildmark.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/buildmark.c.o -MF CMakeFiles/httpserver.dir/src/buildmark.c.o.d -o CMakeFiles/httpserver.dir/src/buildmark.c.o -c /Users/kevin/httpserver/src/buildmark.c

CMakeFiles/httpserver.dir/src/buildmark.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/buildmark.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/buildmark.c > CMakeFiles/httpserver.dir/src/buildmark.c.i

CMakeFiles/httpserver.dir/src/buildmark.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/buildmark.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/buildmark.c -o CMakeFiles/httpserver.dir/src/buildmark.c.s

CMakeFiles/httpserver.dir/src/config.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/config.c.o: ../src/config.c
CMakeFiles/httpserver.dir/src/config.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/httpserver.dir/src/config.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/config.c.o -MF CMakeFiles/httpserver.dir/src/config.c.o.d -o CMakeFiles/httpserver.dir/src/config.c.o -c /Users/kevin/httpserver/src/config.c

CMakeFiles/httpserver.dir/src/config.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/config.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/config.c > CMakeFiles/httpserver.dir/src/config.c.i

CMakeFiles/httpserver.dir/src/config.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/config.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/config.c -o CMakeFiles/httpserver.dir/src/config.c.s

CMakeFiles/httpserver.dir/src/core.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/core.c.o: ../src/core.c
CMakeFiles/httpserver.dir/src/core.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/httpserver.dir/src/core.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/core.c.o -MF CMakeFiles/httpserver.dir/src/core.c.o.d -o CMakeFiles/httpserver.dir/src/core.c.o -c /Users/kevin/httpserver/src/core.c

CMakeFiles/httpserver.dir/src/core.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/core.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/core.c > CMakeFiles/httpserver.dir/src/core.c.i

CMakeFiles/httpserver.dir/src/core.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/core.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/core.c -o CMakeFiles/httpserver.dir/src/core.c.s

CMakeFiles/httpserver.dir/src/log.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/log.c.o: ../src/log.c
CMakeFiles/httpserver.dir/src/log.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/httpserver.dir/src/log.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/log.c.o -MF CMakeFiles/httpserver.dir/src/log.c.o.d -o CMakeFiles/httpserver.dir/src/log.c.o -c /Users/kevin/httpserver/src/log.c

CMakeFiles/httpserver.dir/src/log.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/log.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/log.c > CMakeFiles/httpserver.dir/src/log.c.i

CMakeFiles/httpserver.dir/src/log.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/log.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/log.c -o CMakeFiles/httpserver.dir/src/log.c.s

CMakeFiles/httpserver.dir/src/main.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/main.c.o: ../src/main.c
CMakeFiles/httpserver.dir/src/main.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/httpserver.dir/src/main.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/main.c.o -MF CMakeFiles/httpserver.dir/src/main.c.o.d -o CMakeFiles/httpserver.dir/src/main.c.o -c /Users/kevin/httpserver/src/main.c

CMakeFiles/httpserver.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/main.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/main.c > CMakeFiles/httpserver.dir/src/main.c.i

CMakeFiles/httpserver.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/main.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/main.c -o CMakeFiles/httpserver.dir/src/main.c.s

CMakeFiles/httpserver.dir/src/mpm_common.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/mpm_common.c.o: ../src/mpm_common.c
CMakeFiles/httpserver.dir/src/mpm_common.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/httpserver.dir/src/mpm_common.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/mpm_common.c.o -MF CMakeFiles/httpserver.dir/src/mpm_common.c.o.d -o CMakeFiles/httpserver.dir/src/mpm_common.c.o -c /Users/kevin/httpserver/src/mpm_common.c

CMakeFiles/httpserver.dir/src/mpm_common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/mpm_common.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/mpm_common.c > CMakeFiles/httpserver.dir/src/mpm_common.c.i

CMakeFiles/httpserver.dir/src/mpm_common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/mpm_common.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/mpm_common.c -o CMakeFiles/httpserver.dir/src/mpm_common.c.s

CMakeFiles/httpserver.dir/src/request.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/request.c.o: ../src/request.c
CMakeFiles/httpserver.dir/src/request.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/httpserver.dir/src/request.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/request.c.o -MF CMakeFiles/httpserver.dir/src/request.c.o.d -o CMakeFiles/httpserver.dir/src/request.c.o -c /Users/kevin/httpserver/src/request.c

CMakeFiles/httpserver.dir/src/request.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/request.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/request.c > CMakeFiles/httpserver.dir/src/request.c.i

CMakeFiles/httpserver.dir/src/request.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/request.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/request.c -o CMakeFiles/httpserver.dir/src/request.c.s

CMakeFiles/httpserver.dir/src/util.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/util.c.o: ../src/util.c
CMakeFiles/httpserver.dir/src/util.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/httpserver.dir/src/util.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/util.c.o -MF CMakeFiles/httpserver.dir/src/util.c.o.d -o CMakeFiles/httpserver.dir/src/util.c.o -c /Users/kevin/httpserver/src/util.c

CMakeFiles/httpserver.dir/src/util.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/util.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/util.c > CMakeFiles/httpserver.dir/src/util.c.i

CMakeFiles/httpserver.dir/src/util.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/util.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/util.c -o CMakeFiles/httpserver.dir/src/util.c.s

CMakeFiles/httpserver.dir/src/util_charset.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/util_charset.c.o: ../src/util_charset.c
CMakeFiles/httpserver.dir/src/util_charset.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/httpserver.dir/src/util_charset.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/util_charset.c.o -MF CMakeFiles/httpserver.dir/src/util_charset.c.o.d -o CMakeFiles/httpserver.dir/src/util_charset.c.o -c /Users/kevin/httpserver/src/util_charset.c

CMakeFiles/httpserver.dir/src/util_charset.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/util_charset.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/util_charset.c > CMakeFiles/httpserver.dir/src/util_charset.c.i

CMakeFiles/httpserver.dir/src/util_charset.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/util_charset.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/util_charset.c -o CMakeFiles/httpserver.dir/src/util_charset.c.s

CMakeFiles/httpserver.dir/src/util_debug.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/util_debug.c.o: ../src/util_debug.c
CMakeFiles/httpserver.dir/src/util_debug.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/httpserver.dir/src/util_debug.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/util_debug.c.o -MF CMakeFiles/httpserver.dir/src/util_debug.c.o.d -o CMakeFiles/httpserver.dir/src/util_debug.c.o -c /Users/kevin/httpserver/src/util_debug.c

CMakeFiles/httpserver.dir/src/util_debug.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/util_debug.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/util_debug.c > CMakeFiles/httpserver.dir/src/util_debug.c.i

CMakeFiles/httpserver.dir/src/util_debug.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/util_debug.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/util_debug.c -o CMakeFiles/httpserver.dir/src/util_debug.c.s

CMakeFiles/httpserver.dir/src/util_time.c.o: CMakeFiles/httpserver.dir/flags.make
CMakeFiles/httpserver.dir/src/util_time.c.o: ../src/util_time.c
CMakeFiles/httpserver.dir/src/util_time.c.o: CMakeFiles/httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/httpserver.dir/src/util_time.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/httpserver.dir/src/util_time.c.o -MF CMakeFiles/httpserver.dir/src/util_time.c.o.d -o CMakeFiles/httpserver.dir/src/util_time.c.o -c /Users/kevin/httpserver/src/util_time.c

CMakeFiles/httpserver.dir/src/util_time.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/httpserver.dir/src/util_time.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevin/httpserver/src/util_time.c > CMakeFiles/httpserver.dir/src/util_time.c.i

CMakeFiles/httpserver.dir/src/util_time.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/httpserver.dir/src/util_time.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevin/httpserver/src/util_time.c -o CMakeFiles/httpserver.dir/src/util_time.c.s

# Object files for target httpserver
httpserver_OBJECTS = \
"CMakeFiles/httpserver.dir/src/buildmark.c.o" \
"CMakeFiles/httpserver.dir/src/config.c.o" \
"CMakeFiles/httpserver.dir/src/core.c.o" \
"CMakeFiles/httpserver.dir/src/log.c.o" \
"CMakeFiles/httpserver.dir/src/main.c.o" \
"CMakeFiles/httpserver.dir/src/mpm_common.c.o" \
"CMakeFiles/httpserver.dir/src/request.c.o" \
"CMakeFiles/httpserver.dir/src/util.c.o" \
"CMakeFiles/httpserver.dir/src/util_charset.c.o" \
"CMakeFiles/httpserver.dir/src/util_debug.c.o" \
"CMakeFiles/httpserver.dir/src/util_time.c.o"

# External object files for target httpserver
httpserver_EXTERNAL_OBJECTS =

httpserver: CMakeFiles/httpserver.dir/src/buildmark.c.o
httpserver: CMakeFiles/httpserver.dir/src/config.c.o
httpserver: CMakeFiles/httpserver.dir/src/core.c.o
httpserver: CMakeFiles/httpserver.dir/src/log.c.o
httpserver: CMakeFiles/httpserver.dir/src/main.c.o
httpserver: CMakeFiles/httpserver.dir/src/mpm_common.c.o
httpserver: CMakeFiles/httpserver.dir/src/request.c.o
httpserver: CMakeFiles/httpserver.dir/src/util.c.o
httpserver: CMakeFiles/httpserver.dir/src/util_charset.c.o
httpserver: CMakeFiles/httpserver.dir/src/util_debug.c.o
httpserver: CMakeFiles/httpserver.dir/src/util_time.c.o
httpserver: CMakeFiles/httpserver.dir/build.make
httpserver: /usr/local/lib/lib/libapr-2.dylib
httpserver: CMakeFiles/httpserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/kevin/httpserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Linking C executable httpserver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/httpserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/httpserver.dir/build: httpserver
.PHONY : CMakeFiles/httpserver.dir/build

CMakeFiles/httpserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/httpserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/httpserver.dir/clean

CMakeFiles/httpserver.dir/depend:
	cd /Users/kevin/httpserver/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kevin/httpserver /Users/kevin/httpserver /Users/kevin/httpserver/build /Users/kevin/httpserver/build /Users/kevin/httpserver/build/CMakeFiles/httpserver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/httpserver.dir/depend

