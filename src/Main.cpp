#include "graphics\include\graphics.h"
#include "irrKlang\include\irrKlang.h"

#include <stdio.h>

#define SIZEX 1366
#define SIZEY 700

//Functions.cpp
void *AllocateFile (char file_name[], size_t &size);
void strcat (char destiny[], char add[]);
char *SoundFilePath (char filename[]);
char *ChartPath (char filename[]);

#include "Classes.h"
#include "Victor.h"
#include "Smaira.h"

using namespace irrklang;
int main () {
    size_t size;
    void *file=AllocateFile(SoundFilePath("escape"), size);
    char string[50];
    initwindow(SIZEX, SIZEY, "ITA Hero");
    ISoundEngine* engine = createIrrKlangDevice();
    engine->addSoundSourceFromMemory(file, size, "escape.mp3");
    ISound* music = engine->play2D("escape.mp3", true);
    {
                      highway *a=new highway("Arterial");
                      delete a;
    }
    getch();
    closegraph();
    return 0;
}
