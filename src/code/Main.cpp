#include <graphics/graphics.h>
#include <irrKlang/irrKlang.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

//#define FULLSCREEN

enum en_instrument {GUITAR, BASS, DRUMS};

enum en_difficulty {EASY, MEDIUM, HARD, EXPERT};

enum en_extras {HYPERSPEED, PRECISION, GODMODE, ALLHOPO, PRACTICE=9};

#define NERROR 5

const int *resolutionx;
const int *resolutiony;
#define SIZEX (*resolutionx)
#define SIZEY (*resolutiony)

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
    Uint8* keyboard=SDL_GetKeyState(NULL);
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
        if (keyboard[SDLK_ESCAPE]) {
                        song->pause();
                        menu *pause=new menu ("Pause Menu", SIZEX/2, SIZEY/2);
                        pause->addOpt("Resume");
                        pause->addOpt("Restart");
                        pause->addOpt("Exit");
                        while (pause->navigate());
                        switch (pause->opt()) {
                            case 2:
                                song->reload();
                                for (int i=0;i<nPlayers;i++) players[i]->reset();
                                break;
                            case 3:
                                done=1;
                                break;
                            }
                        if (pause->cancel()) keyboard[SDLK_ESCAPE]=0;
                        delete pause;
                        song->play();
                        }

        }
}

int main (int argc, char *argv[]) {

    SDL_Init (SDL_INIT_EVERYTHING);
    TTF_Init ();
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    
    #ifndef FULLSCREEN
    resolutionx=new const int (1600>SDL_GetVideoInfo()->current_w?SDL_GetVideoInfo()->current_w:1600);
    resolutiony=new const int (850>SDL_GetVideoInfo()->current_h?SDL_GetVideoInfo()->current_h:850);
    #else
    resolutionx=new const int (SDL_GetVideoInfo()->current_w);
    resolutiony=new const int (SDL_GetVideoInfo()->current_h);
    #endif
    
    size_t size;
    char string[200];
    int nSongs;
   
    initwindow(SIZEX, SIZEY, "ITA Hero", (SDL_GetVideoInfo()->current_w-SIZEX)/2, (SDL_GetVideoInfo()->current_h-SIZEY-50)/2, true);
    SDL_Surface *screen;
    #ifndef FULLSCREEN
    screen = SDL_SetVideoMode(SIZEX, SIZEY, 32, SDL_HWSURFACE);
    #else
    screen = SDL_SetVideoMode(SIZEX, SIZEY, 32, SDL_HWSURFACE | SDL_FULLSCREEN);
    #endif

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
    int playersextras[4][10]={{0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}; //extras: hyperspeed[0], precision mode[1], godmode[2], always hopo[3], practice[9]

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
                                en_instrument instrument;
                                switch (instrm->opts()[0]) {
                                    case 'G': instrument=GUITAR; break;
                                    case 'B': instrument=BASS; break;
                                    case 'D': instrument=DRUMS; break;
                                    }
                                ChosenSong->preview(false);
                                ChosenSong->load();
                                highway *player=new highway (screen, ChosenSong, instrument, EXPERT, playersextras[0], playersfret[0], playerspick[0]);
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
                                int i;
                                en_instrument instrument[nPlayers];
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
                                    for (int j=0;j<nPlayers;j++) players[j]=new highway (screen, ChosenSong, instrument[j], EXPERT, playersextras[j], playersfret[j], playerspick[j], 50+(1+2*j)*SIZEX/(2*nPlayers));
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
                                en_instrument instrument;
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
                                            highway *player=new highway (screen, ChosenSong, instrument, EXPERT, playersextras[0], playersfret[0], playerspick[0]);
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

    engine->drop();
    SDL_Quit ();
    TTF_Quit ();
    closegraph();
    return 0;
}
