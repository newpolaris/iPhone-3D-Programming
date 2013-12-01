CC = g++

OPENGLES_INCLUDE = -I./include -I./lib/esUtil
OPENGLES_LINBRARY = -L./lib/Debug

CFLAGS:= $(OPENGLES_INCLUDE) -g -O0
LDFLAGS:= $(OPENGLES_LINBRARY) -lesUtil -lEGL -lGLESv2 -mwindows

MAIN_SOURCES= HelloTriangle.cpp
SOURCES= Classes\ApplicationEngine.cpp \
		 Classes\RenderingEngine.ES2.cpp \
		 Classes\ParametricSurface.cpp

OBJECTS=$(SOURCES:.cpp=.o) 
OUT=-o HelloTriangle

.cpp.o:
	$(CC) $^ $(CFLAGS) -c -o $@

all: HelloTriangle.cpp $(OBJECTS)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) $(OUT)
	cp ./lib/Debug/libEGL.dll .
	cp ./lib/Debug/libGLESv2.dll .

clean:
	rm HelloTriangle.exe $(OBJECTS) libEGL.dll libGLESv2.dll

Classes\RenderingEngine.ES2.o: Classes\RenderingEngine.ES2.cpp Shaders/Simple.frag Shaders/Simple.vert
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -c -o $@

