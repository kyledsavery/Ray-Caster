all: raycast

raycast: raycast.c sceneParser.c v3math.c ppmrw.c
	gcc raycast.c sceneParser.c v3math.c ppmrw.c -o raycast -lm

clean:
	rm -rf *~ raycast
