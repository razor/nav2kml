CC=gcc

nav2kml: nav2kml.c
	$(CC) -g -o nav2kml nav2kml.c
#	strip nav2kml
	chmod +x nav2kml
all: nav2kml
clean:
	rm -rf nav2kml
