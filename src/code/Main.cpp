#include <graphics/graphics.h>
#include <irrKlang/irrKlang.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define GUITAR 0
#define BASS 1
#define DRUMS 2

#define HYPERSPEED 0
#define PRECISION 1
#define GODMODE 2
#define ALLHOPO 3
#define PRACTICE 9

#define NERROR 5

int SIZEX;
int SIZEY;

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

void PlaySong (SDL_Surface *screen, music *song, highway *players[], int nPlayers=1) {
    song->play();
    SDL_Event event;
    Uint8* keyboard;
    bool done=false;
    SDL_Rect all;
    all.x=0;
    all.y=0;
    all.h=SIZEY-1;
    all.w=SIZEX-1;
    while (!done) {
        swapbuffers();
        SDL_Flip(screen);
        SDL_FillRect(screen, &all, 0);
        cleardevice();
        for (int i=0;i<nPlayers;i++) {
            players[i]->refresh(keyboard);
            if (!players[i]->alive()) done=true;
            }
        if (song->isFinished()) done=true;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN: case SDL_KEYUP: keyboard=SDL_GetKeyState(NULL); break;
                case SDL_QUIT: exit(0); break;
                }
            }
        }
}

int main (int argc, char *argv[]) {

    SDL_Init (SDL_INIT_EVERYTHING);
    TTF_Init ();
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    //SIZEX=SDL_GetVideoInfo()->current_w;
    //SIZEY=SDL_GetVideoInfo()->current_h;
    SIZEX=1600>getmaxwidth()?getmaxwidth():1600;
    SIZEY=850>getmaxheight()?getmaxheight():850;

    size_t size;
    char string[200];
    int nSongs;
   
    initwindow(SIZEX, SIZEY, "ITA Hero", (getmaxwidth()-SIZEX)/2, (getmaxheight()-SIZEY-50)/2, true);
    SDL_Surface *screen;
    screen = SDL_SetVideoMode(SIZEX, SIZEY, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    //screen = SDL_SetVideoMode(SIZEX, SIZEY, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

    SDL_WM_SetCaption ( "ITA Hero", NULL );

//load song list
    FILE *reader;
    reader=fopen(FilePath("Sound/", "songs", ".dat"), "r");
    if (reader==NULL) Error ("Soundlist file not found");
    sfscanf (reader, "[SONGS=");
    fscanf (reader, "%d", &nSongs);
    sfscanf (reader, "]\n");

    music* songs[nSongs];

    for (int i=0;i<nSongs-1;i++) {
        songs[i]=new music (reader, engine);
        char c;
        sfscanf (reader, "~\n");
        }
    songs[nSongs-1]=new music (reader, engine);
    sfscanf (reader, "[/SONGS]");
    if (fscanf (reader, "%s", string)!=EOF) Error ("Soundlist file corrupted");
    fclose (reader);
//end of loading of song list

    char playersfret[4][6]={{SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_b}, {SDLK_g, SDLK_h, SDLK_j, SDLK_k, SDLK_l}, {SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t}, {SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5}};
    char playerspick[4][3]={"", "", "", ""};
    int playersextras[4][10]={{1, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}; //extras: hyperspeed[0], precision mode[1], godmode[2], always hopo[3], practice[9]

    menu *startmenu=new menu(" - Main Menu");
    startmenu->addOpt("Singleplayer");
    startmenu->addOpt("Multiplayer");
    startmenu->addOpt("Practice");
    startmenu->addOpt("Options");
    startmenu->addOpt("Exit");
    bool done=0;
    while (!done) {
        while (startmenu->navigate());
        cleardevice(); swapbuffers(); cleardevice();
        switch (startmenu->opts()[0]) {
            case 'S':
                {
                    menu *songmenu=new menu(" - Choose song to play");
                    for (int i=0;i<nSongs;i++) {
                        sprintf (string, "%s - %s", songs[i]->artist, songs[i]->title);
                        songmenu->addOpt(string);
                        }

                    bool stay=1;
                    music *ChosenSong=NULL;
                    while (stay) {
                        while (songmenu->navigate()) {
                            if (ChosenSong!=songs[songmenu->opt()-1]) {
                                if (ChosenSong!=NULL) ChosenSong->preview(false);
                                ChosenSong=songs[songmenu->opt()-1];
                                }
                            ChosenSong->preview(true);
                            }
                        cleardevice(); swapbuffers(); cleardevice();
                        if (!songmenu->cancel()) {
                            menu *instrm=new menu(" - Choose instrument");
                            if (ChosenSong->isInstrumentAvaliable(GUITAR)) instrm->addOpt("Guitar");
                            if (ChosenSong->isInstrumentAvaliable(BASS)) instrm->addOpt("Bass");
                            if (ChosenSong->isInstrumentAvaliable(DRUMS)) instrm->addOpt("Drums");
                            while (instrm->navigate()) ChosenSong->preview(true);
                            cleardevice(); swapbuffers(); cleardevice();
                            if (!instrm->cancel()) {
                                int instrument;
                                switch (instrm->opts()[0]) {
                                    case 'G': instrument=GUITAR; break;
                                    case 'B': instrument=BASS; break;
                                    case 'D': instrument=DRUMS; break;
                                    }
                                ChosenSong->preview(false);
                                ChosenSong->load();
                                highway *player=new highway (screen, ChosenSong, instrument, playersextras[0], playersfret[0], playerspick[0]);
                                PlaySong (screen, ChosenSong, &player);
                                ChosenSong->unload();
                                ChosenSong=NULL;
                                delete player;
                                stay=0;
                                }
                            delete instrm;
                            }
                        else stay=0;
                        }
                    if (ChosenSong!=NULL) ChosenSong->preview(false);
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
                    while(ordmenu->navigate());
                    cleardevice(); swapbuffers(); cleardevice();
                    nPlayers=ordmenu->opt()+1;
                    if (!ordmenu->cancel()) {
                        delete ordmenu;
                        ordmenu=new menu(" - Choose song to play");
                        for (int i=0;i<nSongs;i++) {
                            sprintf (string, "%s - %s", songs[i]->artist, songs[i]->title);
                            ordmenu->addOpt(string);
                            }
                        bool stay=1;
                        music *ChosenSong=NULL;
                        while (stay) {
                            while (ordmenu->navigate()) {
                                if (ChosenSong!=songs[ordmenu->opt()-1]) {
                                    if (ChosenSong!=NULL) ChosenSong->preview(false);
                                    ChosenSong=songs[ordmenu->opt()-1];
                                    }
                                ChosenSong->preview(true);
                                }
                            cleardevice(); swapbuffers(); cleardevice();
                            if (!ordmenu->cancel()) {
                                menu *instrm;
                                int i, instrument[nPlayers];
                                for (i=0;i>=0&&i<nPlayers;i++) {
                                    sprintf (string, " - Choose instrument for Player %d", i+1);
                                    instrm=new menu(string);
                                    if (ChosenSong->isInstrumentAvaliable(GUITAR)) instrm->addOpt("Guitar");
                                    if (ChosenSong->isInstrumentAvaliable(BASS)) instrm->addOpt("Bass");
                                    if (ChosenSong->isInstrumentAvaliable(DRUMS)) instrm->addOpt("Drums");
                                    while(instrm->navigate()) ChosenSong->preview(true);
                                    cleardevice(); swapbuffers(); cleardevice();
                                    if (instrm->cancel()) i-=2;
                                    else
                                        switch (instrm->opts()[0]) {
                                            case 'G': instrument[i]=GUITAR; break;
                                            case 'B': instrument[i]=BASS; break;
                                            case 'D': instrument[i]=DRUMS; break;
                                            }
                                    delete instrm;
                                    }
                                if (i==nPlayers) {
                                    ChosenSong->preview(false);
                                    ChosenSong->load();
                                    highway *players[nPlayers];
                                    for (int j=0;j<nPlayers;j++) players[j]=new highway (screen, ChosenSong, instrument[j], playersextras[j], playersfret[j], playerspick[j], 50+(1+2*j)*SIZEX/(2*nPlayers));
                                    PlaySong (screen, ChosenSong, players, nPlayers);
                                    ChosenSong->unload();
                                    for (int j=0;j<nPlayers;j++) delete players[j];
                                    stay=0;
                                    }
                                }
                            else stay=0;
                            }
                        if (ChosenSong!=NULL) ChosenSong->preview(false);
                        }
                    delete ordmenu;
                }
                break;
            case 'P':
                {
                    menu *songmenu=new menu(" - Choose song to practice");
                    for (int i=0;i<nSongs;i++) {
                        sprintf (string, "%s - %s", songs[i]->artist, songs[i]->title);
                        songmenu->addOpt(string);
                        }

                    bool stay=1;
                    music *ChosenSong=NULL;
                    while (stay) {
                        while (songmenu->navigate()) {
                            if (ChosenSong!=songs[songmenu->opt()-1]) {
                                if (ChosenSong!=NULL) ChosenSong->preview(false);
                                ChosenSong=songs[songmenu->opt()-1];
                                }
                            ChosenSong->preview(true);
                            }
                        cleardevice(); swapbuffers(); cleardevice();
                        if (!songmenu->cancel()) {
                            menu *instrm=new menu(" - Choose instrument");
                            if (ChosenSong->isInstrumentAvaliable(GUITAR)) instrm->addOpt("Guitar");
                            if (ChosenSong->isInstrumentAvaliable(BASS)) instrm->addOpt("Bass");
                            if (ChosenSong->isInstrumentAvaliable(DRUMS)) instrm->addOpt("Drums");
                            while (instrm->navigate()) ChosenSong->preview(true);
                            cleardevice(); swapbuffers(); cleardevice();
                            if (!instrm->cancel()) {
                                int instrument;
                                switch (instrm->opts()[0]) {
                                    case 'G': instrument=GUITAR; break;
                                    case 'B': instrument=BASS; break;
                                    case 'D': instrument=DRUMS; break;
                                    }
                                menu *speed=new menu(" - Choose speed");
                                speed->addOpt("Slowest");
                                speed->addOpt("Slower");
                                speed->addOpt("Slow");
                                speed->addOpt("Full Speed");
                                while (speed->navigate()) ChosenSong->preview(true);
                                cleardevice(); swapbuffers(); cleardevice();
                                if (!speed->cancel()) {
                                    menu *section=new menu (" - Choose section (starting point)");
                                    int from, to;
                                    for (int i=1;i<=16;i++) {
                                        sprintf (string, "Section %d", i);
                                        section->addOpt(string);
                                        }
                                    ChosenSong->preview(false);
                                    for (from=-1;section->navigate();) {
                                        if (from!=section->opt()-1) from=section->opt()-1;
                                        ChosenSong->preview(true, from);
                                        }
                                    cleardevice(); swapbuffers(); cleardevice();
                                    if (!section->cancel()) {
                                        delete section;
                                        section=new menu (" - Choose section (ending point)");
                                        for (int i=from+1;i<=16;i++) {
                                            sprintf (string, "Section %d", i);
                                            section->addOpt(string);
                                            }
                                        for (to=from;section->navigate();) {
                                            if (to!=section->opt()+from-1) to=section->opt()+from-1;
                                            ChosenSong->preview(true, to);
                                            }
                                        cleardevice(); swapbuffers(); cleardevice();
                                        if (!section->cancel()) {
                                            ChosenSong->preview(false);
                                            switch (speed->opt()) {
                                                case 1: ChosenSong->load(0.4, from, to+1); break;
                                                case 2: ChosenSong->load(0.6, from, to+1); break;
                                                case 3: ChosenSong->load(0.8, from, to+1); break;
                                                case 4: ChosenSong->load(1.0, from, to+1); break;
                                                }
                                            playersextras[0][PRACTICE]=1;
                                            highway *player=new highway (screen, ChosenSong, instrument, playersextras[0], playersfret[0], playerspick[0]);
                                            PlaySong (screen, ChosenSong, &player);
                                            ChosenSong->unload();
                                            delete player;
                                            playersextras[0][PRACTICE]=0;
                                            stay=0;
                                            }
                                        }
                                    delete section;
                                    }
                                delete speed;
                                }
                            delete instrm;
                            }
                        else stay=0;
                        }
                    if (ChosenSong!=NULL) ChosenSong->preview(false);
                    delete songmenu;
                }
                break;
            case 'O':
                {
                    menu *options=new menu(" - Options");
                    options->addOpt("Controls");
                    options->addOpt("Extras");
                    options->addOpt("Back");
                    bool doneopt=0;
                    while (!doneopt) {
                        while (options->navigate());
                        switch (options->opts()[0]) {
                            case 'C': break;
                            case 'E': break;
                            case 'B': doneopt=1; break;
                            }
                        if (options->cancel()) doneopt=1;
                        }
                    cleardevice(); swapbuffers(); cleardevice();
                    delete options;
                }
                break;
            case 'E': done=1; break;
            }
        if (startmenu->cancel()) done=1;
        cleardevice(); swapbuffers(); cleardevice();
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
    SDL_Quit ();
    TTF_Quit ();
    //closegraph();
    return 0;
}
