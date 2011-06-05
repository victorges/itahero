#include <graphics.h>
#include <windows.h>

#define SIZEX 1366
#define SIZEY 700

#include "Functions.h"
#include "Classes.h"

void *AllocateFile (char file_name[]);
void strcat (char destiny[], char add[]);
char *SoundFilePath (char filename[]);
char *ChartPath (char filename[]);

highway::highway(char ChartFileName[], char control[], char pck[], int l, int r);
highway::~highway();

int main () {
    initwindow(SIZEX, SIZEY, "ITA Hero");
    highway &player=*(new highway ("Arterial"));
    closegraph();
    return 0;
}
