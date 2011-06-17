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


int main () {
    size_t size;
    char string[50];
    music *test=new music("Arterial", "Arterial Black", "Drist");
    initwindow(SIZEX, SIZEY, "ITA Hero");
    highway *a=new highway(test);
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    test->load(engine);
    test->play();
    
    while (!test->isFinished()) {
          moveto(0, 0);
          a->refresh();
          }

    delete a;
    getch();
    closegraph();
    return 0;
}
