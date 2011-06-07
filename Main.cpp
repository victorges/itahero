#include <graphics.h>
#include <windows.h>

#define SIZEX 1366
#define SIZEY 700

//Functions.cpp
void *AllocateFile (char file_name[]);
void strcat (char destiny[], char add[]);
char *SoundFilePath (char filename[]);
char *ChartPath (char filename[]);

#include "Classes.h"
#include "Victor.h"
#include "Smaira.h"


int main () {
    initwindow(SIZEX, SIZEY, "ITA Hero");
    {
                      highway *a=new highway("Arterial");
                      delete a;
    }
    closegraph();
    return 0;
}
