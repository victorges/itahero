#include <graphics.h>
#include <irrKlang.h>

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
     strcat (location, ".ogg");
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
    initwindow(SIZEX, SIZEY, "ITA Hero");
    highway *a=new highway("Arterial");
    ISoundEngine* engine = createIrrKlangDevice();
    ISoundSource* source=engine->addSoundSourceFromMemory(file, size, "music.ogg");
    
    free(file);
    
    ISound* music = engine->play2D(source, false, false, true, true);
    
    ISoundEffectControl* fx=music->getSoundEffectControl();
    //fx->enableWavesReverbSoundEffect();
    music->setIsPaused();
    Sleep(1000);
    music->setIsPaused(0);
    while (!(music->isFinished()));
    engine->removeSoundSource(source);
    engine->drop();
    delete a;
    getch();
    closegraph();
    return 0;
}
