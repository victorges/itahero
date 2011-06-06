#include <graphics.h>
#include <windows.h>

#define SIZEX 1366
#define SIZEY 700

#include "Classes.h"

void *AllocateFile (char file_name[]);
void strcat (char destiny[], char add[]);
char *SoundFilePath (char filename[]);
char *ChartPath (char filename[]);

highway::highway(char ChartFileName[], char control[], char pick[], int left, int right);
highway::~highway();

int main () {
    initwindow(SIZEX, SIZEY, "ITA Hero");
    {
                      highway &a=*new highway("Arterial");
                      delete &a;
    }
    closegraph();
    return 0;
}
