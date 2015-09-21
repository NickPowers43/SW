CC=em++

SOURCES2:=$(wildcard *.cpp) $(wildcard ../SW/*.cpp)
SOURCES:=$(wildcard *.cpp) $(wildcard ../SW/*.cpp)
CFLAGS=__EMSCRIPTEN__
OTHERFLAGS= -g -s ASSERTIONS=2 -Werror -s FULL_ES2=1 -O2 --llvm-opts 2 -s EXPORTED_FUNCTIONS=@$(EXPORTS_FILE) $(LDFLAGS) -s DEMANGLE_SUPPORT=1 --js-opts 0 -s SAFE_HEAP=1 -s USE_SDL=2 
LDFLAGS=-std=c++11
IDIR=-I../
EXPORTS_FILE=makefile_exports.txt
ODIR=
OUTPUT=../SW_Client_Web/public_html/SW_Client.js
SARGS=-s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s STB_IMAGE=1 -s EXPORTED_FUNCTIONS="['_main', '_Initialize', '_Update', '_OnConnectionMade', '_HandleClose', '_HandleMessage']"

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(SOURCES) 
	$(CC) -DEMSCRIPTEN $(SOURCES) $(IDIR) --bind -g $(SARGS) --js-library library_mySocket.js -o $(OUTPUT) --preload-file data

clean:
	rm $(OUTPUT)