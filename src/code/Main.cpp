#include <graphics/graphics.h>
#include <irrKlang/irrKlang.h>
#include <SDL/SDL.h>

#define GUITAR 0
#define BASS 1
#define DRUMS 2

#define HYPERSPEED 0
#define PRECISION 1
#define GODMODE 2
#define ALLHOPO 3

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

int main (int argc, char *argv[]) {
    size_t size;
    char string[200];
    int nSongs;
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    initwindow(SIZEX, SIZEY, "ITA Hero");
    SDL_Init(SDL_INIT_VIDEO);
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
    int playersextras[4][10]={{1, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}; //extras: hyperspeed[0], precision mode[1], godmode[2], always hopo[3]
    
    menu *startmenu=new menu(" - Main Menu");
    startmenu->addOpt("Singleplayer");
    startmenu->addOpt("Multiplayer");
    startmenu->addOpt("Practice");
    startmenu->addOpt("Options");
    startmenu->addOpt("Exit");

    while (!startmenu->lastopt()) {
        while (kbhit()) getch();
        startmenu->navigate();
        switch (startmenu->opts()[0]) {
            case 'S':
                {
                    menu *songmenu=new menu(" - Choose song to play");
                    for (int i=0;i<nSongs;i++) {
                        sprintf (string, "%s - %s", songs[i]->artist, songs[i]->title);
                        songmenu->addOpt(string);
                        }
                    songmenu->addOpt("Cancel");

                    bool stay=1;
                    while (stay) {
                        songmenu->navigate();
                        if (!songmenu->lastopt()) {
                            music *ChosenSong=songs[songmenu->opt()-1];
                            menu *instrm=new menu(" - Choose instrument");
                            if (ChosenSong->isInstrumentAvaliable(GUITAR)) instrm->addOpt("Guitar");
                            if (ChosenSong->isInstrumentAvaliable(BASS)) instrm->addOpt("Bass");
                            if (ChosenSong->isInstrumentAvaliable(DRUMS)) instrm->addOpt("Drums");
                            instrm->addOpt("Back");
                            instrm->navigate();
                            if (!instrm->lastopt()) {
                                int instrument;
                                switch (instrm->opts()[0]) {
                                    case 'G': instrument=GUITAR; break;
                                    case 'B': instrument=BASS; break;
                                    case 'D': instrument=DRUMS; break;
                                    }
                                ChosenSong->load(engine);
                                highway *player=new highway (ChosenSong, instrument, playersextras[0], playersfret[0], playerspick[0]);
                                PlaySong (ChosenSong, &player);
                                ChosenSong->unload(engine);
                                delete player;
                                stay=0;
                                }
                            delete instrm;
                            }
                        else stay=0;
                        }
                    delete songmenu;
                }
                break;
            case 'M':
                {
                    int nPlayers;
                    menu *ordmenu=new menu (" - How many people are going to play?");
                    ordmenu->addOpt("2");
                    ordmenu->addOpt("3");
                    ordmenu->addOpt("4");
                    ordmenu->addOpt("Cancel");
                    ordmenu->navigate();
                    nPlayers=ordmenu->opt()+1;
                    if (!ordmenu->lastopt()) {
                        delete ordmenu;
                        ordmenu=new menu(" - Choose song to play");
                        for (int i=0;i<nSongs;i++) {
                            sprintf (string, "%s - %s", songs[i]->artist, songs[i]->title);
                            ordmenu->addOpt(string);
                            }
                        ordmenu->addOpt("Cancel");
                        bool stay=1;
                        while (stay) {
                            ordmenu->navigate();
                            if (!ordmenu->lastopt()) {
                                music *ChosenSong=songs[ordmenu->opt()-1];
                                menu *instrm;
                                int i, instrument[nPlayers];
                                for (i=0;i>=0&&i<nPlayers;i++) {
                                    sprintf (string, " - Choose instrument for Player %d", i+1);
                                    instrm=new menu(string);
                                    if (ChosenSong->isInstrumentAvaliable(GUITAR)) instrm->addOpt("Guitar");
                                    if (ChosenSong->isInstrumentAvaliable(BASS)) instrm->addOpt("Bass");
                                    if (ChosenSong->isInstrumentAvaliable(DRUMS)) instrm->addOpt("Drums");
                                    instrm->addOpt("Back");
                                    instrm->navigate();
                                    if (instrm->lastopt()) i-=2;
                                    else
                                        switch (instrm->opts()[0]) {
                                            case 'G': instrument[i]=GUITAR; break;
                                            case 'B': instrument[i]=BASS; break;
                                            case 'D': instrument[i]=DRUMS; break;
                                            }
                                    delete instrm;
                                    }
                                if (i==nPlayers) {
                                    ChosenSong->load(engine);
                                    highway *players[nPlayers];
                                    for (int j=0;j<nPlayers;j++) players[j]=new highway (ChosenSong, instrument[j], playersextras[j], playersfret[j], playerspick[j], 50+(1+2*j)*SIZEX/(2*nPlayers));
                                    PlaySong (ChosenSong, players, nPlayers);
                                    ChosenSong->unload(engine);
                                    for (int j=0;j<nPlayers;j++) delete players[j];
                                    stay=0;
                                    }
                                }
                            else stay=0;
                            }
                        }
                    delete ordmenu;
                }
                break;

            case 'O':
                {
                    menu *options=new menu("Options");
                    options->addOpt("Controls");
                    options->addOpt("Extras");
                    options->addOpt("Back");
                    while (!options->lastopt()) {
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
