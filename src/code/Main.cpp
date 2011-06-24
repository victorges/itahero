#include <graphics.h>
#include <irrKlang.h>

#define GUITAR 0
#define BASS 1
#define DRUMS 2

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

void PlaySong (music *song, highway *players[], int nPlayers=1) {
    song->play();
    bool continuous=true;
    while (continuous) {
        swapbuffers();
        cleardevice();
        for (int i=0;i<nPlayers;i++) {
            players[i]->refresh();
            if (!players[i]->alive()) continuous=false;
            }
        if (song->isFinished()) continuous=false;
        }
}

int main () {
    size_t size;
    char string[200];
    int nSongs;
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    initwindow(SIZEX, SIZEY, "ITA Hero");

//load song list
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
//end of loading of song list

    char playersfret[4][6]={"ZXCVB", "QWERT", "GHJKL", {VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, 0}};
    char playerspick[4][3]={"", "", "", ""};
    int playershs[4]={0}; //hyperspeed
    
    menu *startmenu=new menu("Main Menu");
    startmenu->addOpt("Singleplayer");
    startmenu->addOpt("Multiplayer");
    startmenu->addOpt("Options");
    startmenu->addOpt("Exit");

    while (!startmenu->done()) {
        while (kbhit()) getch();
        startmenu->navigate();
        switch (startmenu->opt()) {
            case 1:
                {
                    menu *songmenu=new menu("Choose song to play");
                    for (int i=0;i<nSongs;i++) {
                        sprintf (string, "%s - %s", songs[i]->artist, songs[i]->title);
                        songmenu->addOpt(string);
                        }
                    songmenu->addOpt("Cancel");

                    bool stay=1;
                    while (stay) {
                        songmenu->navigate();
                        if (!songmenu->done()) {
                            menu *instrm=new menu("Choose instrument to play");
                            if (songs[songmenu->opt()-1]->isInstrumentAvaliable(GUITAR)) instrm->addOpt("Guitar");
                            if (songs[songmenu->opt()-1]->isInstrumentAvaliable(BASS)) instrm->addOpt("Bass");
                            if (songs[songmenu->opt()-1]->isInstrumentAvaliable(DRUMS)) instrm->addOpt("Drums");
                            instrm->addOpt("Back");
                            instrm->navigate();
                            if (!instrm->done()) {
                                int instrument;
                                switch (instrm->opt()) {
                                    case 1:
                                        if (songs[songmenu->opt()-1]->isInstrumentAvaliable(GUITAR)) instrument=GUITAR;
                                        else if (songs[songmenu->opt()-1]->isInstrumentAvaliable(BASS)) instrument=BASS;
                                        else instrument=DRUMS;
                                        break;
                                    case 2:
                                        if (!songs[songmenu->opt()-1]->isInstrumentAvaliable(GUITAR)||!songs[songmenu->opt()-1]->isInstrumentAvaliable(BASS)) instrument=DRUMS;
                                        else instrument=BASS;
                                        break;
                                    case 3: instrument=DRUMS; break;
                                    }
                                songs[songmenu->opt()-1]->load(engine);
                                highway *player=new highway (songs[songmenu->opt()-1], instrument, 100, playershs[0], playersfret[0], playerspick[0]);
                                PlaySong (songs[songmenu->opt()-1], &player);
                                songs[songmenu->opt()-1]->unload(engine);
                                delete player;
                                stay=0;
                                }
                            else stay=1;
                            delete instrm;
                            }
                        }
                    delete songmenu;
                }
                break;
            case 3:
                {
                    menu *options=new menu("Options");
                    options->addOpt("Opt1");
                    options->addOpt("Opt2");
                    options->addOpt("Back");
                    while (!options->done()) {
                        options->navigate();
                        switch (options->opt()) {
                            case 1: break;
                            case 2: break;
                            }
                        }
                    delete options;
                }
                break;
            }
        }
    delete startmenu;

    /*int i=4;
    music* playing=songs[i];
    highway *players[4];
    players[0]=new highway(playing, GUITAR, 100, playershs[0], playersfret[0], playerspick[0], 0, 300);
    players[1]=new highway(playing, BASS, 100, playershs[1], playersfret[1], playerspick[1], 0, SIZEX/2+50);
    players[2]=new highway(playing, DRUMS, 100, playershs[2], playersfret[2], playerspick[2], 0, SIZEX-200);
    playing->load(engine);
    //Sleep(1000);
    //while (a->preliminary());
    //playing->play();
    PlaySong (playing, players, 3);
    while (1) {
          swapbuffers();
          cleardevice();
          a->refresh();
          b->refresh();
          c->refresh();
          if (kbhit()||playing->isFinished()) {
                if (playing->isFinished()||getch()==27) {
                    playing->unload(engine);
                    delete a;
                    delete b;
                    delete c;
                    i=(i+1)%nSongs;
                    playing=songs[i];
                    a=new highway (playing, GUITAR, 100, 0, "ZXCVB", "", 0, 300);
                    b=new highway(playing, BASS, 100, 1, "QWERT", "", 0, SIZEX/2+50);
                    c=new highway(playing, DRUMS, 100, 2, "GHJKL", "", 0, SIZEX-200);
                    playing->load(engine);
                    Sleep(1000);
                    playing->play();
                    }
                }
          }*/
          
    engine->drop();
    closegraph();
    return 0;
}
