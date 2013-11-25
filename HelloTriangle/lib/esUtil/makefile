CC = gcc

OPENGLES_INCLUDE = -I../include -I.
OPENGLES_LINBRARY = -L../lib/Debug

CFLAGS:= $(OPENGLES_INCLUDE)
LDFLAGS:= $(OPENGLES_LINBRARY)
SOURCES=esShader.c esShapes.c esTransform.c esUtil.c Win32/esUtil_TGA.c Win32/esUtil_win32.c
OBJECTS=$(SOURCES:.c=.o) 

TARGET:= libesUtil.a

all: $(TARGET)

.c.o:
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -c -o $@

$(TARGET): $(OBJECTS)
	ar cr $(TARGET) $^

clean:
	rm -f $(OBJECTS) $(TARGET)

esShader.o: esShader.c esUtil.o
esUtil.o: esUtil.c Win32/esUtil_win32.o
