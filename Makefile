CC = gcc
LDFLAGS = -lGL -lGLU -lGLEW `sdl-config --libs` -lm -ldl -L./bass/x64/ -lbass
CFLAGS = -Wall -O3 `sdl-config --cflags` -fstrict-aliasing -fprofile-use -I./bass/
OBJECTS = vidthing.o shadermanager.o texturemanager.o matrixlib.o framebuffermanager.o camera.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

vidthing: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

debug:	CFLAGS= -Wall -O0 -g `sdl-config --cflags` -fstrict-aliasing -I./bass/
debug: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o vidthing-$@ $(LDFLAGS)

valgrind:	CFLAGS= -Wall -O0 -g `sdl-config --cflags` -fstrict-aliasing -D DEBUGTIMESTEP -I./bass/
valgrind: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
purge:
	@echo purging oop
	@rm -f $(OBJECTS)
	@rm -f vidthing
	@rm -f vidthing-debug
	@rm -f vidthing-valgrind
	@rm -f vidthing-profile
	@rm -f ./*.gcda
run: $(TARGET)
	LD_LIBRARY_PATH=./bass/x64/:$LD_LIBRARY_PATH ./vidthing ./D*
