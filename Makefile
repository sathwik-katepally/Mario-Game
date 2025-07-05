# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I/opt/homebrew/include -I/usr/local/include

# Emscripten settings for WASM
EMCXX = emcc
EMCXXFLAGS = -std=c++17 -O2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY=1 -s EXPORTED_FUNCTIONS='["_main"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' --preload-file assets --shell-file shell.html

# Libraries
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -L/opt/homebrew/lib -L/usr/local/lib

# Source files
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Target executable
TARGET = mario_game
WASM_TARGET = mario_game.html

# Default target
all: $(TARGET)

# Build native version
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

# Build WASM version
wasm: shell.html $(WASM_TARGET)

$(WASM_TARGET): $(SOURCES)
	$(EMCXX) $(EMCXXFLAGS) $(SOURCES) -o $(WASM_TARGET)

# Compile source files to object files
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(SRCDIR)/*.o $(TARGET) mario_game.html mario_game.js mario_game.wasm mario_game.data

# Clean WASM files specifically
clean-wasm:
	rm -f mario_game.html mario_game.js mario_game.wasm mario_game.data

.PHONY: all wasm clean clean-wasm 