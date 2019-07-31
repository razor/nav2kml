CC=gcc
CFLAGS=-Wall

nav2kml: nav2kml.c
	$(CC) -g $(CFLAGS) -o nav2kml nav2kml.c
	strip nav2kml
	chmod +x nav2kml
all: nav2kml
clean:
	rm -rf nav2kml
