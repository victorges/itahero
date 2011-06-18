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

void sfscanf (FILE *file, char CheckString[]) {
     int i;
     for (i=0;CheckString[i];i++);
     char string[i];
     for (int j=0;j<i;j++) fscanf (file, "%c", &string[j]);
     for (i=0;CheckString[i]&&string[i]==CheckString[i];i++);
     if (CheckString[i]) Error ("Soundlist file corrupted");
}

int main () {
    size_t size;
    char string[50];
    int nSongs;
    initwindow(SIZEX, SIZEY, "ITA Hero");
    
    FILE *reader;
    reader=fopen("Sound\\songs.dat", "r");
    if (reader==NULL) Error ("Soundlist file not found");
    sfscanf (reader, "[SONGS=");
    fscanf (reader, "%d", &nSongs);
    sfscanf (reader, "]\n");
    music* songs[nSongs];
    for (int i=0;i<nSongs-1;i++) {
        songs[i]=new music (reader);
        char c;
        sfscanf (reader, "~\n");
        }
    songs[nSongs-1]=new music (reader);
    sfscanf (reader, "[/SONGS]");
    if (fscanf (reader, "%s", string)!=EOF) Error ("Soundlist file corrupted");
    fclose (reader);
    
    music* playing=songs[1];
    highway *a=new highway(playing);
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    playing->load(engine);
    playing->play();
    
    while (!playing->isFinished()) {
          swapbuffers();
          cleardevice();
          a->refresh();
          printf ("%d %d %d\n", a->score, a->streak, a->multiplier());
          }

    delete a;
    getch();
    closegraph();
    return 0;
}
