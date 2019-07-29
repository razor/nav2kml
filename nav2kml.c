#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "nav2kml.h"

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char *argv[]) {
	int opt;
	char ifn[255];
	char ofn[255];
	int mode;

	while ((opt = getopt(argc, argv, OPTSTR)) != -1) {
		if (opt == 't') {
			if (!strcmp(optarg, "aid")) {
				mode = MODE_AID;
			}
			else if (!strcmp(optarg, "fix")) {
				mode = MODE_FIX;
			}
			else {
				usage();
				return 1;
			}
		}

		if (opt == 'i') {
			strncpy(ifn, optarg, 255);
		}

		if (opt == 'o') {
			strncpy(ofn, optarg, 255);
		}
	}

	convert(ifn, ofn, mode);

}

int convert(const char *ifn, const char *ofn, int mode) {
	char *line = NULL;
	FILE *iptfile = fopen(ifn, "r");
	if (!iptfile) {
		printf("Error: open input file failed.\n");
	}
	FILE *optfile = fopen(ofn, "w");
	if (!optfile) {
		printf("Error: open output file failed.\n");
	}

	void *navobj = NULL;
	int (*parsefunc)(const char *, void *obj);
	int(*writefunc)(FILE *, void *obj);

	navaid aid;
	navfix fix;

	switch (mode) {
	case MODE_AID:
		navobj = (void *)&aid;
		parsefunc = &parseaid;
		writefunc = &writeaid;
		break;
	case MODE_FIX:
		navobj = (void *)&fix;
		parsefunc = &parsefix;
		writefunc = &writefix;
		break;
	}
	
	int ret = 0;
	ssize_t nread = 0;
	size_t len = 0;

	writekmlhead(optfile, mode);

	while (nread = getline(&line, &len, iptfile)) {
		if (nread == -1) {
			break;
		}
		*(line + nread - 1) = 0;
		if (*line == ';') continue;
		ret = parsefunc(line, navobj);
		ret = writefunc(optfile, navobj);
	}
	writekmlend(optfile);
	free(line);
	fclose(iptfile);
	fclose(optfile);

}

void usage() {
	printf("usage: nav2kml -t aid/fix -i inputfile -o outputfile.\n");
}

int parseaid(const char *line, void *navobj) {
	//COL 1-24 Facility Name IIII
	//COL 25-28 ID TTTT
	//COL 30-33 Type
	//		ILS  Insturment Landing System (Localizer)             ILSD ILS/DME             NDB  Nondirectional Beacon
	//      NDBM NDB/Locator Middle Marker (LMM)             NDBO NDB/Locator Outer Marker (LOM)             
	//      MARI Unknown - seems to be same as MHW class NDB             VOR  VHF Omnidirectional Radio             
	//      VORD VOR/DME (no separate code for VORTAC) dd.dddddd  
	//COL 34-43 Latitude (-Lat for South) ddd.dddddd 
	//COL 44-54 Longitude ( -Lon for West) fff.ff     
	//COL 55-60 Frequency (MHz for ILS/VOR KHz for NDB) See Note Below C          
	//Col 61 Class
	navaid *aid = (navaid *)navobj;
	memset(aid, 0, sizeof(navaid));

	memcpy(aid->name, line, 24);
	memcpy(aid->id, line + 24, 4);
	memcpy(aid->type, line + 28, 4);
	memcpy(aid->lat, line + 33, 10);
	memcpy(aid->lon, line + 43, 10);

	//printf("%s, %s, %s, %s\n", aid->id, aid->type, aid->lat, aid->lon);
	return 0;
}
int parsefix(const char *line, void *navobj) {
	//Col 1-5 & 25-30 Fix Name dd.dddddd  
	//Col 32-40 Latitude degrees (-Lat for South, sign Col 31)ddd.dddddd  
	//Col 41-51 Longitude degrees (-Lon for West, decimal always Col 45)       
	navfix *fix = (navfix *)navobj;
	memset(fix, 0, sizeof(navfix));

	memcpy(fix->name, line, 5);
	memcpy(fix->name2, line + 24, 5);
	memcpy(fix->lat, line + 30, 10);
	memcpy(fix->lon, line + 40, 10);

	//printf("%s, %s, %s\n", fix->name, fix->lat, fix->lon);
	return 0;
}

int writekmlhead(FILE *optfile, int mode) {
	int ret = 0;
	char buf[512];
	ret = fputs(KML_HEADER, optfile);
	if (!ret) return -1;
	snprintf(buf, sizeof(buf), KML_FOLDER_TPL, (mode == MODE_AID) ? "AID" : "FIX", "");
	ret = fputs(buf, optfile);
	if (!ret) return -1;
}

int writeaid(FILE *optfile, void *navobj) {
	char buf[512];
	int ret = 0;

	navaid *aid = (navaid *)navobj;
	snprintf(buf, sizeof(buf), PLACEMARK_TPL, aid->id, aid->name, aid->lon, aid->lat);
	ret = fputs(buf, optfile);
	if (!ret) return -1;
}

int writefix(FILE *optfile, void *navobj) {
	char buf[512];
	int ret = 0;

	navfix *fix = (navfix *)navobj;
	snprintf(buf, sizeof(buf), PLACEMARK_TPL, fix->name2, fix->name, fix->lon, fix->lat);
	ret = fputs(buf, optfile);
	if (!ret) return -1;
}

int writekmlend(FILE *optfile) {
	int ret = 0;
	ret = fputs(KML_FOLDER_END, optfile);
	if (!ret) return -1;
	ret = fputs(KML_END, optfile);
	if (!ret) return -1;
}
