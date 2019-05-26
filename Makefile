CXX = clang++
#CXX = g++
DEBUG_FLAGS = -ggdb -O0 -DDEBUG
WARNINGS = -Wall -Wno-unknown-warning-option -Wno-unused-but-set-variable -Wno-unused-variable -Wno-missing-braces 
FLAGS = $(WARNINGS) -std=c++14 -Iinc $(DEBUG_FLAGS)
PROGRAM_NAME = bin/potter
SOURCE_FILE = src/potter_main.cpp
TERMINAL = st
SOURCE_FILES = $(shell find src/ -name '*.cpp')
HEADERS = $(shell find src/ -name '*.h')

.PHONY: default run clean debug valgrind

default: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(SOURCE_FILES) $(HEADERS)
	mkdir -p bin
	$(CXX) $(FLAGS) $(SOURCE_FILE) -o $(PROGRAM_NAME) -pthread

clean:
	rm $(PROGRAM_NAME)

run: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)
	feh out.png

debug: $(PROGRAM_NAME)
	gdb ./$(PROGRAM_NAME)

valgrind: $(PROGRAM_NAME)
	$(TERMINAL) gdb $(PROGRAM_NAME) &
	valgrind --vgdb-error=0 --suppressions=useable.supp $(PROGRAM_NAME)
