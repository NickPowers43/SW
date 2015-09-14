CC=em++

SOURCES2:=$(wildcard *.cpp) $(wildcard ../SW/*.cpp)
SOURCES:=main.cpp GraphicsCommon.cpp SW_Client.cpp NetworkWriter.cpp NetworkReader.cpp TileChunk.cpp TileChunks.cpp Vessel.cpp Player.cpp $(wildcard ../SW/*.cpp)
CFLAGS=__EMSCRIPTEN__
OTHERFLAGS= -g -s ASSERTIONS=2 -Werror -s FULL_ES2=1 -O2 --llvm-opts 2 -s EXPORTED_FUNCTIONS=@$(EXPORTS_FILE) $(LDFLAGS)
LDFLAGS=-std=c++11
IDIR=-I../
EXPORTS_FILE=makefile_exports.txt
ODIR=
OUTPUT=../SW_Client_Web/public_html/js/SW_Client.js

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(SOURCES) 
	$(CC) -DEMSCRIPTEN $(SOURCES) $(IDIR) --bind -s SAFE_HEAP=1 --js-library library_mySocket.js -s EXPORTED_FUNCTIONS="['_Initialize', '_Update', '_OnConnectionMade', '_HandleClose', '_HandleMessage']" -o $(OUTPUT)

clean:
	rm $(OUTPUT)