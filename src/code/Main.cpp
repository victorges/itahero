#include <graphics.h>
#include <irrKlang.h>

const int SIZEX=1600>getmaxwidth()?getmaxwidth():1600;
const int SIZEY=850>getmaxheight()?(getmaxheight()-50):850;

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

char *FilePath (char prefix[], char filename[], char suffix[]) {
     static char location[100];
     location[0]=0;
     strcat (location, prefix);
     strcat (location, filename);
     strcat (location, suffix);
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
    reader=fopen(FilePath("Sound/", "songs", ".dat"), "r");
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

    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    int i=0;
    music* playing=songs[i];
    highway *a=new highway(playing, 100, 1, "ZXCVB", "", 0, 400);
    highway *b=new highway(playing, 100, 0, "GHJKL", "", 0, SIZEX-400);
    playing->load(engine);
    //Sleep(1000);
    //while (a->preliminary());
    playing->play();
    while (1) {
          swapbuffers();
          cleardevice();
          a->refresh();
          b->refresh();
          if (kbhit()||playing->isFinished()) {
                if (playing->isFinished()||getch()==27) {
                    playing->unload(engine);
                    delete a;
                    delete b;
                    i=(i+1)%nSongs;
                    playing=songs[i];
                    a=new highway (playing, 100, 1, "ZXCVB", "", 0, 400);
                    b=new highway(playing, 100, 0, "GHJKL", "", 0, SIZEX-400);
                    playing->load(engine);
                    Sleep(1000);
                    playing->play();
                    }
                }
          }

    delete a;
    getch();
    closegraph();
    return 0;
}
