# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/misakihanayo/bzcompiler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/misakihanayo/bzcompiler/cmake-build-default

# Include any dependencies generated for this target.
include CMakeFiles/lexer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lexer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lexer.dir/flags.make

../tmp/lex1.cpp: ../src/phase1/lexical_analyzer.l
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/misakihanayo/bzcompiler/cmake-build-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "[FLEX][lexer] Building scanner with flex 2.5.35"
	cd /Users/misakihanayo/bzcompiler && /usr/bin/flex -o/Users/misakihanayo/bzcompiler/tmp/lex1.cpp /Users/misakihanayo/bzcompiler/src/phase1/lexical_analyzer.l

CMakeFiles/lexer.dir/src/test/test_lexer.cpp.o: CMakeFiles/lexer.dir/flags.make
CMakeFiles/lexer.dir/src/test/test_lexer.cpp.o: ../src/test/test_lexer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/misakihanayo/bzcompiler/cmake-build-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/lexer.dir/src/test/test_lexer.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lexer.dir/src/test/test_lexer.cpp.o -c /Users/misakihanayo/bzcompiler/src/test/test_lexer.cpp

CMakeFiles/lexer.dir/src/test/test_lexer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lexer.dir/src/test/test_lexer.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/misakihanayo/bzcompiler/src/test/test_lexer.cpp > CMakeFiles/lexer.dir/src/test/test_lexer.cpp.i

CMakeFiles/lexer.dir/src/test/test_lexer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lexer.dir/src/test/test_lexer.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/misakihanayo/bzcompiler/src/test/test_lexer.cpp -o CMakeFiles/lexer.dir/src/test/test_lexer.cpp.s

CMakeFiles/lexer.dir/tmp/lex1.cpp.o: CMakeFiles/lexer.dir/flags.make
CMakeFiles/lexer.dir/tmp/lex1.cpp.o: ../tmp/lex1.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/misakihanayo/bzcompiler/cmake-build-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/lexer.dir/tmp/lex1.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lexer.dir/tmp/lex1.cpp.o -c /Users/misakihanayo/bzcompiler/tmp/lex1.cpp

CMakeFiles/lexer.dir/tmp/lex1.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lexer.dir/tmp/lex1.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/misakihanayo/bzcompiler/tmp/lex1.cpp > CMakeFiles/lexer.dir/tmp/lex1.cpp.i

CMakeFiles/lexer.dir/tmp/lex1.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lexer.dir/tmp/lex1.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/misakihanayo/bzcompiler/tmp/lex1.cpp -o CMakeFiles/lexer.dir/tmp/lex1.cpp.s

# Object files for target lexer
lexer_OBJECTS = \
"CMakeFiles/lexer.dir/src/test/test_lexer.cpp.o" \
"CMakeFiles/lexer.dir/tmp/lex1.cpp.o"

# External object files for target lexer
lexer_EXTERNAL_OBJECTS =

lexer: CMakeFiles/lexer.dir/src/test/test_lexer.cpp.o
lexer: CMakeFiles/lexer.dir/tmp/lex1.cpp.o
lexer: CMakeFiles/lexer.dir/build.make
lexer: CMakeFiles/lexer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/misakihanayo/bzcompiler/cmake-build-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable lexer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lexer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lexer.dir/build: lexer

.PHONY : CMakeFiles/lexer.dir/build

CMakeFiles/lexer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lexer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lexer.dir/clean

CMakeFiles/lexer.dir/depend: ../tmp/lex1.cpp
	cd /Users/misakihanayo/bzcompiler/cmake-build-default && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/misakihanayo/bzcompiler /Users/misakihanayo/bzcompiler /Users/misakihanayo/bzcompiler/cmake-build-default /Users/misakihanayo/bzcompiler/cmake-build-default /Users/misakihanayo/bzcompiler/cmake-build-default/CMakeFiles/lexer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lexer.dir/depend

