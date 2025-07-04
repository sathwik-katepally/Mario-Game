CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf
TARGET = mario_game
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Detect OS for different SDL2 paths
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    # macOS
    CXXFLAGS += -I/opt/homebrew/include -I/usr/local/include
    LIBS += -L/opt/homebrew/lib -L/usr/local/lib
endif

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)

install-deps:
	@echo "Installing SDL2 dependencies..."
	@if command -v brew >/dev/null 2>&1; then \
		brew install sdl2 sdl2_image sdl2_ttf; \
	elif command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get update && sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev; \
	else \
		echo "Please install SDL2, SDL2_image, and SDL2_ttf manually"; \
	fi

.PHONY: clean install-deps 