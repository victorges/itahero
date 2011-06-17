#include "graphics\include\graphics.h"
#include "irrKlang\include\irrKlang.h"

#include <stdio.h>

#define SIZEX 1366
#define SIZEY 700

#include "Functions.h"
#include "Classes.h"
#include "Victor.h"
#include "Smaira.h"

void strcat (char destiny[], char add[]) {
     int i, j;
     for (i=0;destiny[i]!=0;i++);
     for (j=0;add[j]!=0;j++) destiny[i+j]=add[j];
     destiny[i+j]=0;
}
char *SoundFilePath (char filename[]) {
     static char location[100];
     location[0]=0;
     strcat (location, "Sound\\");
     strcat (location, filename);
     strcat (location, ".mp3");
     return location;
}
char *ChartPath (char filename[]) {
     static char location[100];
     location[0]=0;
     strcat (location, "Chart\\");
     strcat (location, filename);
     strcat (location, ".chart");
     return location;
}

using namespace irrklang;
int main () {
    size_t size;
    void *file=AllocateFile(SoundFilePath("escape"), size);
    char string[50];
    highway *a=new highway("Arterial");
    ISoundEngine* engine = createIrrKlangDevice();
    initwindow(SIZEX, SIZEY, "ITA Hero");
    engine->addSoundSourceFromMemory(file, size, "escape.mp3");
    free(file);
    ISound* music = engine->play2D("escape.mp3", true, false, true, ESM_AUTO_DETECT, true);
    ISoundEffectControl* fx=music->getSoundEffectControl();
    //fx->enableWavesReverbSoundEffect();
    while (!(music->isFinished()));
    closegraph();
    music->drop();
    delete a;
    return 0;
}
