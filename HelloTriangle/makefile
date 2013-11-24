CC = g++

OPENGLES_INCLUDE = -I./include -I./include/esUtil
OPENGLES_LINBRARY = -L./lib/ -L./lib/Debug

CFLAGS:= $(OPENGLES_INCLUDE) -g -O0
LDFLAGS:= $(OPENGLES_LINBRARY) -lesUtil -lEGL -lGLESv2 -mwindows

SOURCES=HelloTriangle.cpp Classes\RenderingEngine2.cpp
OBJECTS=$(SOURCES:.cpp=.o) 
OUT=-o HelloTriangle

.cpp.o:
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -c -o $@

all: HelloTriangle.cpp Classes\RenderingEngine2.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) $(OUT)
	cp ./lib/Debug/libEGL.dll .
	cp ./lib/Debug/libGLESv2.dll .

clean:
	rm HelloTriangle.exe Classes\RenderingEngine2.o libEGL.dll libGLESv2.dll

Classes\RenderingEngine2.o: Classes\RenderingEngine2.cpp Shaders/Simple.frag Shaders/Simple.vert
