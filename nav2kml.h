#define OPTSTR "t:i:o:"
#define MODE_AID 0
#define MODE_FIX 1
#define KML_HEADER "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
#define KML_END "</kml>"
#define KML_FOLDER_TPL "<Folder>\n<name>%s</name>\n<description>%s</description>\n"
#define KML_FOLDER_END "</Folder>\n"
#define PLACEMARK_TPL "<Placemark>\n<name>%s</name>\n<description>%s</description>\n<Point><coordinates>%s, %s</coordinates></Point></Placemark>\n"
#define KHZ "KHz"
#define MHZ "MHz"

typedef struct navaid {
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
	//Col 61 Class              H High Altitude/Long Range             N NDB             T Terminal/Short RangeNote: If NDB frequency is above 999.99 KHz then thefrequecy field still starts in col 55 and C is col 62
	char name[25];
	char id[5];
	char type[5];
	char lat[11];
	char lon[11];
	char freq[12];
} navaid;

typedef struct navfix {
	//Col 1-5 & 25-30 Fix Name dd.dddddd  
	//Col 32-40 Latitude degrees (-Lat for South, sign Col 31)ddd.dddddd  
	//Col 41-51 Longitude degrees (-Lon for West, decimal always Col 45)          
	//	Note: The duplicate name fields may be the result how the FAA          provides data, where there are many more fixes defined than provide          
	//	in the airac data. For example, most terminal data is not included.          This data includes airway crossing, radar service boundaries, etc.
	char name[6];
	char name2[6];
	char lat[11];
	char lon[11];
} navfix;

void usage();
int convert(const char *ifn, const char *ofn, int mode);
int parseaid(const char *line, void *navobj);
int parsefix(const char *line, void *navobj);
int writekmlhead(FILE *optfile, int mode);
int writeaid(FILE *optfile, void *navobj);
int writefix(FILE *optfile, void *navobj);
int writekmlend(FILE *optfile);