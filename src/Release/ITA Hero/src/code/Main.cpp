#include <irrKlang/irrKlang.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_net.h>
#include <math.h>

enum en_instrument {GUITAR, BASS, DRUMS};

enum en_difficulty {EASY, MEDIUM, HARD, EXPERT};

enum en_extras {HYPERSPEED, PRECISION, GODMODE, ALLHOPO, PRACTICE=9};

enum en_notes {GREEN, RED, YELLOW, BLUE, ORANGE, STARPOWER};

#define NERROR 5
#define PI 3.14159265359

class {
    int resolutionx;
    int resolutiony;
    int sizex;
    int sizey;
    public:
        int sizx() {return sizex;}
        int sizy() {return sizey;}
    friend int main (int argc, char *argv[]);
    } video;
#define SIZEX (video.sizx())
#define SIZEY (video.sizy())

#include "Functions.h"
#include "Classes.h"
#include "Victor.h"
#include "Smaira.h"

bool CheckEsc() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym==SDLK_ESCAPE) return true;
                    break;
                case SDL_ACTIVEEVENT:
                    if (event.active.state&SDL_APPINPUTFOCUS) {
                        SDL_ShowCursor(SDL_ENABLE);
                        while (event.type!=SDL_ACTIVEEVENT||!(event.active.state&SDL_APPINPUTFOCUS)||!event.active.gain) SDL_PollEvent(&event);
                        SDL_ShowCursor(SDL_DISABLE);
                        return true;
                        }
                    break;
                case SDL_QUIT: exit(0); break;
                }
            }
    return false;
}

void PreSong (bool &menu, CDrawer *screen, CMusic *song, CHighway *players[], int nPlayers) {
    SDL_Event event;
    song->effect("start");
    for (int i=SDL_GetTicks();SDL_GetTicks()-i<1357;) {
        screen->Flip();
        screen->clear();
        for (int j=0;j<nPlayers;j++) players[j]->draw(song->time()+(int)((int)(SDL_GetTicks()-i-4357)*song->speed()), NULL, SIZEY+1+(1357-(SDL_GetTicks()-i))*players[j]->get_height()/1357);
        if (CheckEsc()) {
            screen->clear();
            for (int j=0;j<nPlayers;j++) players[j]->draw(song->time(), SDL_GetKeyState(NULL));
            song->stop();
            menu=true;
            return;
            }
        }
    song->effect("ticks");
    for (int i=SDL_GetTicks();SDL_GetTicks()-i<3000;) {
        screen->Flip();
        screen->clear();
        for (int j=0;j<nPlayers;j++) players[nPlayers-j-1]->draw(song->time()+(int)((int)(SDL_GetTicks()-i-3000)*song->speed()), SDL_GetKeyState(NULL));
        if (CheckEsc()) {
            screen->clear();
            for (int j=0;j<nPlayers;j++) players[j]->draw(song->time(), SDL_GetKeyState(NULL));
            song->stop();
            menu=true;
            return;
            }
        }
}

void Rewind (bool &menu, CDrawer *screen, CMusic *song, CHighway *players[], int nPlayers=1) {
    int backup=song->time(), i;
    SDL_Event event;
    song->play();
    for (i=0;song->time()>20&&i<70;i++) {
        screen->Flip();
        screen->clear();
        song->settime(backup-(int)(15*(i+1)*song->speed()));
        for (int i=0;i<nPlayers;i++) players[nPlayers-i-1]->draw();
        if (CheckEsc()) {
            screen->clear();
            for (int j=0;j<nPlayers;j++) players[nPlayers-j-1]->draw(song->time(), SDL_GetKeyState(NULL));
            menu=true;
            song->settime(backup);
            return;
            }
        }
    if (i<70) {
        song->pause();
        int dt=(70-i)*15;
        while (i++<70) {
            screen->Flip();
            screen->clear();
            for (int j=0;j<nPlayers;j++) players[nPlayers-j-1]->draw(backup-(int)(15*(i+1)*song->speed()));
            if (CheckEsc()) {
                screen->clear();
                for (int j=0;j<nPlayers;j++) players[nPlayers-j-1]->draw(song->time(), SDL_GetKeyState(NULL));
                menu=true;
                song->settime(backup);
                return;
                }
            }
        for (i=SDL_GetTicks();SDL_GetTicks()-i<dt;) {
            screen->Flip();
            screen->clear();
            for (int j=0;j<nPlayers;j++) players[j]->draw(song->time()+(int)((int)((SDL_GetTicks()-i-dt))*song->speed()), SDL_GetKeyState(NULL));
            if (CheckEsc()) {
                screen->clear();
                for (int j=0;j<nPlayers;j++) players[nPlayers-j-1]->draw(song->time(), SDL_GetKeyState(NULL));
                menu=true;
                song->settime(backup);
                return;
                }
            }
        song->play();
        }
    while (song->time()<backup) {
        screen->Flip();
        screen->clear();
        for (int i=0;i<nPlayers;i++) players[nPlayers-i-1]->draw(song->time(), SDL_GetKeyState(NULL));
        if (CheckEsc()) {
            screen->clear();
            for (int j=0;j<nPlayers;j++) players[nPlayers-j-1]->draw(song->time(), SDL_GetKeyState(NULL));
            menu=true;
            song->settime(backup);
            return;
            }
        }
}

long long int PlaySong (CDrawer *screen, CMusic *song, CHighway *players[], int nPlayers=1) {
    SDL_Event event;
    Uint8* keyboard=SDL_GetKeyState(NULL);
    long long int actualscore = 0;
    bool done=false, bmenu=false;
    CMenu *pause=new CMenu (screen, "Pause Menu", SIZEX/2, SIZEY/2);
    pause->addOpt("Resume");
    pause->addOpt("Restart");
    pause->addOpt("Exit");
    while (SDL_PollEvent(&event));
    PreSong(bmenu, screen, song, players, nPlayers);
    song->play();
    while (!done) {
        screen->Flip();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN: case SDL_KEYUP:
                    if (event.type==SDL_KEYDOWN&&event.key.keysym.sym==SDLK_ESCAPE) bmenu=true;
                    keyboard=SDL_GetKeyState(NULL);
                    break;
                case SDL_ACTIVEEVENT:
                    if (event.active.state&SDL_APPINPUTFOCUS) {
                        SDL_ShowCursor(SDL_ENABLE);
                        bmenu=true;
                        }
                    break;
                case SDL_QUIT: exit(0); break;
                }
            }

        if (song->isFinished()) {
            done=true;
            if (!players[0]->practice) {
                song->effect("winning");
                CMenu rock(screen, "You Rock!", SIZEX/2, SIZEY/2);
                CSprite** rockeff=new CSprite*[nPlayers];
                for (int i=0;i<nPlayers;i++) rockeff[i]=new CSprite(screen, players[i]);
                for (int i=SDL_GetTicks();SDL_GetTicks()<i+3500;) {
                    screen->clear();
                    for (int j=0;j<nPlayers;j++) {
                        if (((SDL_GetTicks()/100)+j)%2) rockeff[j]->starpower();
                        rockeff[j]->animate();
                        }
                    rock.print();
                    screen->Flip();
                    }
                for (int i=0;i<nPlayers;i++) delete rockeff[i];
                delete[] rockeff;
                screen->clear();
                }
            else {
                char string[100];
                song->pause();
                sprintf (string, "Section Ended. Percentage hit: %.1f", players[0]->percentage());
               CMenu pract(screen, string, SIZEX/2, SIZEY/2);
                pract.addOpt("Retry");
                pract.addOpt("Exit");
                while (pract.navigate()||pract.cancel());
                switch (pract.opt()) {
                    case 1:
                        song->reload();
                        for (int i=0;i<nPlayers;i++) players[i]->reset();
                        PreSong(bmenu, screen, song, players, nPlayers);
                        song->play();
                        done=0;
                        break;
                    }
                }
            }

        if (bmenu&&!done) {
                bmenu=false;
                song->pause();
                pause->effect("menuenter");
                while (pause->navigate());
                switch (pause->opt()) {
                    case 1:
                        Rewind(bmenu, screen, song, players, nPlayers);
                        break;
                    case 2:
                        song->reload();
                        for (int i=0;i<nPlayers;i++) players[i]->reset();
                        PreSong(bmenu, screen, song, players, nPlayers);
                        song->play();
                        break;
                    case 3:
                        return -1;
                        break;
                    }
                if (pause->cancel()) Rewind(bmenu, screen, song, players, nPlayers);
                pause->setopt(1);
                }
        else if (!done) {
            screen->clear();
            for (int i=0;i<nPlayers;i++) {
                players[nPlayers-i-1]->refresh(keyboard);
                if (!players[nPlayers-i-1]->alive()) done=true;
                }
            if (done) {
                song->pause();
               CMenu lost(screen, "You Lost!", SIZEX/2, SIZEY/2);
                lost.addOpt("Retry");
                lost.addOpt("Exit");
                while (lost.navigate()||lost.cancel());
                if (lost.opt()==1) {
                    song->reload();
                    for (int i=0;i<nPlayers;i++) players[i]->reset();
                    PreSong(bmenu, screen, song, players, nPlayers);
                    song->play();
                    done=0;
                    }
                }
            }
        }
    
    delete pause;

    if(song->isFinished())
        for(int i=0; i<nPlayers; i++) actualscore+=players[i]->score/10000;
    else
        actualscore = -1;
    screen->textxy("6", 0, 0);
    screen->Flip();
    return actualscore;
}

int main (int argc, char *argv[]) {
    SDL_Init (SDL_INIT_EVERYTHING);
    TTF_Init ();
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

    size_t size;
    char string[200];
    int nSongs;

    CMusic** songs;
    
    char playersfret[4][5]={{SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_b}, {SDLK_g, SDLK_h, SDLK_j, SDLK_k, SDLK_l}, {SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t}}; //extras: hyperspeed[0], precision mode[1], godmode[2], always hopo[3], practice[9]
    char playerssp[4]={SDLK_SPACE, SDLK_RETURN, SDLK_TAB};
    char playerspick[4][2]={"", "", "", ""};
    int playersextras[4][10]={{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    bool fullscreen=false;
    CSprite::active=true;

    {//load data
        FILE *reader;
        reader=fopen(FilePath("Sound/", "songs", ".dat"), "r");
        if (reader==NULL) Error ("Soundlist file not found");
        sfscanf (reader, "[SONGS=");
        fscanf (reader, "%d", &nSongs);
        sfscanf (reader, "]\n");
        songs=new CMusic*[nSongs];

        for (int i=0;i<nSongs-1;i++) {
            songs[i]=new CMusic (reader, engine);
            char c;
            sfscanf (reader, "~\n");
            }
        songs[nSongs-1]=new CMusic (reader, engine);
        sfscanf (reader, "[/SONGS]");
        if (fscanf (reader, "%s", string)!=EOF) Error ("Songlist file corrupted");
        fclose (reader);

        reader=fopen(FilePath("SAVEDATA/", "data", ".dat"), "rb");
        if (reader!=NULL) {
            fread(playersfret, sizeof(char), 4*5, reader);
            fread(playerssp, sizeof(char), 4, reader);
            fread(playerspick, sizeof(char), 4*2, reader);
            fread(playersextras, sizeof(char), 4*10, reader);
            fread(&fullscreen, sizeof(bool), 1, reader);
            fread(&CSprite::active, sizeof(bool), 1, reader);
            fclose(reader);
            for (int i=0;i<4;i++) {
                for (int j=0;j<6;j++) {
                    playersfret[i][j]^='c';
                    }
                }
            for (int i=0;i<4;i++) playerssp[i]^='c';
            for (int i=0;i<4;i++) {
                for (int j=0;j<2;j++) {
                    playerspick[i][j]^='c';
                    }
                }
            }
    } //end of loading data

    CDrawer *screen;
    video.resolutionx=SDL_GetVideoInfo()->current_w;
    video.resolutiony=SDL_GetVideoInfo()->current_h;
    if (fullscreen) {
        video.sizex=video.resolutionx;
        video.sizey=video.resolutiony;
        screen = new CDrawer(SIZEX, SIZEY, 32, SDL_HWSURFACE | SDL_FULLSCREEN);
        }
    else {
        video.sizex=1600>video.resolutionx?video.resolutionx:1600;
        video.sizey=850>video.resolutiony?video.resolutiony:850;
        screen = new CDrawer(SIZEX, SIZEY, 32, SDL_HWSURFACE );
        }

    screen->setcolor(0, 0, 0);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_SetCaption ( "ITA Hero", NULL );

    CDrawer *wallpaper=new CDrawer(FilePath("Image/", "wallpaper", ".png"));
    screen->load_background(wallpaper);
    CDrawer *logo=new CDrawer(FilePath("Image/", "Ita Hero", ".png"));
    CMenu::loadfx(engine);

    srand(SDL_GetTicks());
    int menumusic=2;
    songs[menumusic]->load(1.0, 2);
    songs[menumusic]->play(0.6);
    {
        SDL_Event event;
        bool skip=false;
        for (int i=0, sttime=songs[menumusic]->time();!skip&&i<SIZEY;i++) {
            wallpaper->apply_surface(0, 0, screen, SIZEY-i);
            screen->Flip();
            while (songs[menumusic]->time()-sttime<5000*i/SIZEY) CHighway::load();
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym==SDLK_ESCAPE||event.key.keysym.sym==SDLK_RETURN) {
                            skip=true;
                            SDL_Delay(500);
                            }
                        break;
                    case SDL_ACTIVEEVENT:
                        if (event.active.state&SDL_APPINPUTFOCUS) {
                            SDL_ShowCursor(SDL_ENABLE);
                            songs[menumusic]->pause();
                            while (event.type!=SDL_ACTIVEEVENT||!(event.active.state&SDL_APPINPUTFOCUS)||!event.active.gain) SDL_PollEvent(&event);
                            SDL_ShowCursor(SDL_DISABLE);
                            songs[menumusic]->play();
                            }
                        break;
                    case SDL_QUIT: exit(0); break;
                    }
                }
            }
        for (int i=-logo->get_width(), sttime=songs[menumusic]->time();!skip && i<SIZEX-logo->get_width();i+=10) {
            screen->clear();
            logo->apply_surface(i, (screen->get_height()-logo->get_height())/2, screen);
            screen->Flip();
            while (songs[menumusic]->time()-sttime<1000*(i+logo->get_width())/SIZEX) CHighway::load();
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym==SDLK_ESCAPE||event.key.keysym.sym==SDLK_RETURN) {
                            skip=true;
                            SDL_Delay(500);
                            }
                        break;
                    case SDL_ACTIVEEVENT:
                        if (event.active.state&SDL_APPINPUTFOCUS) {
                            SDL_ShowCursor(SDL_ENABLE);
                            songs[menumusic]->pause();
                            while (event.type!=SDL_ACTIVEEVENT||!(event.active.state&SDL_APPINPUTFOCUS)||!event.active.gain) SDL_PollEvent(&event);
                            SDL_ShowCursor(SDL_DISABLE);
                            songs[menumusic]->play();
                            }
                        break;
                    case SDL_QUIT: exit(0); break;
                    }
                }
            }
        wallpaper->apply_surface(0, 0, screen);
        logo->apply_surface(SIZEX-logo->get_width(), (screen->get_height()-logo->get_height())/2, screen);
        while (SDL_PollEvent(&event));
        CMenu anykey(screen, "Press any key to start", SIZEX/2, 4*SIZEY/5);
        int sttime=SDL_GetTicks();
        event.type=SDL_KEYUP;
        while (event.type!=SDL_KEYDOWN) {
            anykey.print();
            screen->Flip();
            SDL_PollEvent(&event);
            if (event.type==SDL_ACTIVEEVENT&&event.active.state&SDL_APPINPUTFOCUS) {
                    return 0;
                    SDL_ShowCursor(SDL_ENABLE);
                    while (event.type!=SDL_ACTIVEEVENT||!(event.active.state&SDL_APPINPUTFOCUS)||event.active.gain) SDL_PollEvent(&event);
                    SDL_ShowCursor(SDL_DISABLE);
                    }
            if (event.type==SDL_QUIT) exit(0);
            if ((SDL_GetTicks()-sttime)/500%2) screen->setcolor(0, 0, 0);
            else screen->setcolor(223, 159, 26);
            CHighway::load();
            }
    }
    screen->setcolor(0, 0, 0);
    delete wallpaper;
    wallpaper=new CDrawer(FilePath("Image/", "wallpaper", ".png"));
    logo->apply_surface(wallpaper->get_width()-logo->get_width(), (wallpaper->get_height()-logo->get_height())/2, wallpaper);
    screen->load_background(wallpaper);
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev));
    }

    screen->clear();
    CMenu *startmenu=new CMenu(screen, " - Main Menu");
    startmenu->addOpt("Singleplayer");
    startmenu->addOpt("Multiplayer");
    startmenu->addOpt("Practice");
    startmenu->addOpt("Options");
    startmenu->addOpt("Exit");
    songs[menumusic]->pause();
    songs[menumusic]->play(0.55);
    bool done=0;
    while (!done) {
        while (startmenu->navigate())
            if (songs[menumusic]->isFinished()) {
                songs[menumusic]->unload();
                menumusic=rand()%nSongs;
                songs[menumusic]->load(1.0, 3);
                songs[menumusic]->play(0.55);
                }
        screen->clear();
        switch (startmenu->opts()[0]) {
            case 'S':
                {
                    long long int fscore;
                   CMenu *diffic=new CMenu(screen, " - Choose difficulty");
                    diffic->addOpt("Easy");
                    diffic->addOpt("Medium");
                    diffic->addOpt("Hard");
                    diffic->addOpt("Expert");
                    while (diffic->navigate()) {
                        if (songs[menumusic]->isFinished()) {
                            songs[menumusic]->unload();
                            menumusic=rand()%nSongs;
                            songs[menumusic]->load(1.0, 3);
                            songs[menumusic]->play(0.5);
                            }
                        }
                    screen->clear();
                    if (!diffic->cancel()) {
                        songs[menumusic]->unload();
                        en_difficulty difficulty;
                        switch (diffic->opt()) {
                            case 1: difficulty=EASY; break;
                            case 2: difficulty=MEDIUM; break;
                            case 3: difficulty=HARD; break;
                            case 4: difficulty=EXPERT; break;
                            }
                        CMenu *songmenu=new CMenu(screen, " - Choose song to play");
                        for (int i=0;i<nSongs;i++) {
                            sprintf (string, "%s - %s", songs[i]->artist, songs[i]->title);
                            songmenu->addOpt(string);
                            }

                        bool stay=1;
                        CMusic *ChosenSong=NULL;
                        while (stay) {
                            while (songmenu->navigate()) {
                                if (ChosenSong!=songs[songmenu->opt()-1]) {
                                    if (ChosenSong!=NULL) ChosenSong->preview(false);
                                    ChosenSong=songs[songmenu->opt()-1];
                                    }
                                ChosenSong->preview(true);
                                }
                            screen->clear();
                            if (!songmenu->cancel()) {
                               CMenu *instrm=new CMenu(screen, " - Choose instrument");
                                if (ChosenSong->isInstrumentAvaliable(GUITAR)) instrm->addOpt("Guitar");
                                if (ChosenSong->isInstrumentAvaliable(BASS)) instrm->addOpt("Bass");
                                if (ChosenSong->isInstrumentAvaliable(DRUMS)) instrm->addOpt("Drums");
                                while (instrm->navigate()) ChosenSong->preview(true);
                                screen->clear();
                                if (!instrm->cancel()) {
                                    en_instrument instrument;
                                    switch (instrm->opts()[0]) {
                                        case 'G': instrument=GUITAR; break;
                                        case 'B': instrument=BASS; break;
                                        case 'D': instrument=DRUMS; break;
                                        }
                                    ChosenSong->preview(false);
                                    ChosenSong->load();
                                    screen->load_background(NULL);
                                    CHighway *player=new CHighway (screen, ChosenSong, instrument, difficulty, playersextras[0], playersfret[0], playerspick[0], playerssp[0], SIZEX/2, 7*SIZEX/20);
                                    fscore = PlaySong (screen, ChosenSong, &player);
                                    ChosenSong->check_record_file(1);
                                    if(fscore!=-1 && !playersextras[0][GODMODE]){
                                        screen->settextstyle("lazy", NULL, 60);
                                        char *stringaux;
                                        stringaux = screen->draw_name(9*SIZEX/20, SIZEY/6);
                                        ChosenSong->include_record(stringaux, fscore, 1);
                                        SDL_Delay(1000);
                                        screen->settextstyle("lazy", NULL, 36);
                                        }
                                    screen->draw_highscore(4*SIZEX/10, SIZEY/4, ChosenSong->filename, 1);
                                    
                                    screen->load_background(wallpaper);
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
                    delete diffic;
                }
                break;
            case 'M':
                {
                    int nPlayers;
                    long long int fscore;
                   CMenu *ordmenu=new CMenu (screen, " - How many people are going to play?");
                    ordmenu->addOpt("2 players");
                    ordmenu->addOpt("3 players");
                    ordmenu->addOpt("4 players");
                    while(ordmenu->navigate()) {
                        if (songs[menumusic]->isFinished()) {
                            songs[menumusic]->unload();
                            menumusic=rand()%nSongs;
                            songs[menumusic]->load(1.0, 3);
                            songs[menumusic]->play(0.5);
                            }
                        }
                    screen->clear();
                    nPlayers=ordmenu->opt()+1;
                    if (!ordmenu->cancel()) {
                        int i;
                        en_difficulty difficulty[nPlayers];
                        for (i=0;i>=0&&i<nPlayers;i++) {
                            delete ordmenu;
                            sprintf (string, " - Choose difficulty for Player %d", i+1);
                            ordmenu=new CMenu(screen, string);
                            ordmenu->addOpt("Easy");
                            ordmenu->addOpt("Medium");
                            ordmenu->addOpt("Hard");
                            ordmenu->addOpt("Expert");
                            while(ordmenu->navigate()) {
                                if (songs[menumusic]->isFinished()) {
                                    songs[menumusic]->unload();
                                    menumusic=rand()%nSongs;
                                    songs[menumusic]->load(1.0, 3);
                                    songs[menumusic]->play(0.5);
                                    }
                                }
                            screen->clear();
                            if (ordmenu->cancel()) i-=2;
                            else
                                switch (ordmenu->opt()) {
                                    case 1: difficulty[i]=EASY; break;
                                    case 2: difficulty[i]=MEDIUM; break;
                                    case 3: difficulty[i]=HARD; break;
                                    case 4: difficulty[i]=EXPERT; break;
                                    }
                            }
                        if (i==nPlayers) {
                            songs[menumusic]->unload();
                            delete ordmenu;
                            ordmenu=new CMenu(screen, " - Choose song to play");
                            for (i=0;i<nSongs;i++) {
                                sprintf (string, "%s - %s", songs[i]->artist, songs[i]->title);
                                ordmenu->addOpt(string);
                                }
                            bool stay=1;
                            CMusic *ChosenSong=NULL;
                            while (stay) {
                                while (ordmenu->navigate()) {
                                    if (ChosenSong!=songs[ordmenu->opt()-1]) {
                                        if (ChosenSong!=NULL) ChosenSong->preview(false);
                                        ChosenSong=songs[ordmenu->opt()-1];
                                        }
                                    ChosenSong->preview(true);
                                    }
                                screen->clear();
                                if (!ordmenu->cancel()) {
                                   CMenu *instrm;
                                    en_instrument instrument[nPlayers];
                                    for (i=0;i>=0&&i<nPlayers;i++) {
                                        sprintf (string, " - Choose instrument for Player %d", i+1);
                                        instrm=new CMenu(screen, string);
                                        if (ChosenSong->isInstrumentAvaliable(GUITAR)) instrm->addOpt("Guitar");
                                        if (ChosenSong->isInstrumentAvaliable(BASS)) instrm->addOpt("Bass");
                                        if (ChosenSong->isInstrumentAvaliable(DRUMS)) instrm->addOpt("Drums");
                                        while(instrm->navigate()) ChosenSong->preview(true);
                                        screen->clear();
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
                                        CHighway *players[nPlayers];
                                        screen->load_background(NULL);
                                        for (int j=0;j<nPlayers;j++) players[j]=new CHighway (screen, ChosenSong, instrument[j], difficulty[j], playersextras[j], playersfret[j], playerspick[j], playerssp[j], 50+(1+2*j)*SIZEX/(2*nPlayers), ((SIZEX-20*i)/i>7*SIZEX/20)?(7*SIZEX/20):((SIZEX-20*i)/i));
                                        fscore = PlaySong (screen, ChosenSong, players, nPlayers);
                                        
                                        bool testgm = true;
                                        for(int k = 0; k<nPlayers; k++) if(playersextras[k][GODMODE]) testgm = false;
                                        ChosenSong->check_record_file(nPlayers);
                                        if(fscore!=-1 && testgm){
                                            screen->settextstyle("lazy", NULL, 60);
                                            char *stringaux;
                                            stringaux = screen->draw_name(9*SIZEX/20, SIZEY/6);
                                            ChosenSong->include_record(stringaux, fscore, nPlayers);
                                            SDL_Delay(1000);
                                            screen->settextstyle("lazy", NULL, 36);
                                            }
                                        screen->draw_highscore(4*SIZEX/10, SIZEY/4, ChosenSong->filename, nPlayers);
                                        screen->load_background(wallpaper);
                                        ChosenSong->unload();
                                        for (int j=0;j<nPlayers;j++) delete players[j];
                                        stay=0;
                                        }
                                    }
                                else stay=0;
                                }
                            if (ChosenSong!=NULL) ChosenSong->preview(false);
                            }
                        }
                    delete ordmenu;
                }
                break;
            case 'P':
                {
                    songs[menumusic]->unload();
                   CMenu *songmenu=new CMenu(screen, " - Choose song to practice");
                    for (int i=0;i<nSongs;i++) {
                        sprintf (string, "%s - %s", songs[i]->artist, songs[i]->title);
                        songmenu->addOpt(string);
                        }

                    bool stay=1;
                    CMusic *ChosenSong=NULL;
                    while (stay) {
                        while (songmenu->navigate()) {
                            if (ChosenSong!=songs[songmenu->opt()-1]) {
                                if (ChosenSong!=NULL) ChosenSong->preview(false);
                                ChosenSong=songs[songmenu->opt()-1];
                                }
                            ChosenSong->preview(true);
                            }
                        screen->clear();
                        if (!songmenu->cancel()) {
                           CMenu *instrm=new CMenu(screen, " - Choose instrument");
                            if (ChosenSong->isInstrumentAvaliable(GUITAR)) instrm->addOpt("Guitar");
                            if (ChosenSong->isInstrumentAvaliable(BASS)) instrm->addOpt("Bass");
                            if (ChosenSong->isInstrumentAvaliable(DRUMS)) instrm->addOpt("Drums");
                            while (instrm->navigate()) ChosenSong->preview(true);
                            screen->clear();
                            if (!instrm->cancel()) {
                                en_instrument instrument;
                                switch (instrm->opts()[0]) {
                                    case 'G': instrument=GUITAR; break;
                                    case 'B': instrument=BASS; break;
                                    case 'D': instrument=DRUMS; break;
                                    }
                               CMenu *diffic=new CMenu(screen, " - Choose difficulty");
                                diffic->addOpt("Easy");
                                diffic->addOpt("Medium");
                                diffic->addOpt("Hard");
                                diffic->addOpt("Expert");
                                while (diffic->navigate()) ChosenSong->preview(true);
                                screen->clear();
                                if (!diffic->cancel()) {
                                    en_difficulty difficulty;
                                    switch (diffic->opt()) {
                                        case 1: difficulty=EASY; break;
                                        case 2: difficulty=MEDIUM; break;
                                        case 3: difficulty=HARD; break;
                                        case 4: difficulty=EXPERT; break;
                                        }
                                   CMenu *speed=new CMenu(screen, " - Choose speed");
                                    speed->addOpt("Slowest");
                                    speed->addOpt("Slower");
                                    speed->addOpt("Slow");
                                    speed->addOpt("Full Speed");
                                    while (speed->navigate()) ChosenSong->preview(true);
                                    screen->clear();
                                    if (!speed->cancel()) {
                                       CMenu *section=new CMenu (screen, " - Choose section (starting point)");
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
                                        screen->clear();
                                        if (!section->cancel()) {
                                            delete section;
                                            section=new CMenu (screen, " - Choose section (ending point)");
                                            for (int i=from+1;i<=16;i++) {
                                                sprintf (string, "Section %d", i);
                                                section->addOpt(string);
                                                }
                                            for (to=from;section->navigate();) {
                                                if (to!=section->opt()+from-1) to=section->opt()+from-1;
                                                ChosenSong->preview(true, to);
                                                }
                                            screen->clear();
                                            if (!section->cancel()) {
                                                ChosenSong->preview(false);
                                                switch (speed->opt()) {
                                                    case 1: ChosenSong->load(0.4, from, to+1); break;
                                                    case 2: ChosenSong->load(0.6, from, to+1); break;
                                                    case 3: ChosenSong->load(0.8, from, to+1); break;
                                                    case 4: ChosenSong->load(1.0, from, to+1); break;
                                                    }
                                                playersextras[0][PRACTICE]=1;
                                                screen->load_background(NULL);
                                                CHighway *player=new CHighway (screen, ChosenSong, instrument, difficulty, playersextras[0], playersfret[0], playerspick[0]);
                                                PlaySong (screen, ChosenSong, &player);
                                                screen->load_background(wallpaper);
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
                                delete diffic;
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
                    CMenu *options=new CMenu(screen, " - Options");
                    options->addOpt("Controls");
                    options->addOpt("Extras");
                    options->addOpt("Video");
                    options->addOpt("Back");
                    bool doneopt=0;
                    while (!doneopt) {
                        screen->clear();
                        while (options->navigate());
                        switch (options->opts()[0]) {
                            case 'C':
                                {
                                   CMenu *ordmenu;
                                    bool donecontrol=0;
                                    while (!donecontrol) {
                                        screen->clear();
                                        ordmenu = new CMenu (screen, " - Choose player to edit");
                                        ordmenu->addOpt("Player 1");
                                        ordmenu->addOpt("Player 2");
                                        ordmenu->addOpt("Player 3");
                                        ordmenu->addOpt("Player 4");
                                        while (ordmenu->navigate()) {
                                            if (songs[menumusic]->isFinished()) {
                                                songs[menumusic]->unload();
                                                menumusic=rand()%nSongs;
                                                songs[menumusic]->load(1.0, 3);
                                                songs[menumusic]->play(0.5);
                                                }
                                            }
                                        if (!ordmenu->cancel()) {
                                            int Chosen=ordmenu->opt()-1;
                                            delete ordmenu;
                                            bool doneconfig=0;
                                            int selected=0;
                                            while (!doneconfig) {
                                                screen->clear();
                                                ordmenu=new CMenu (screen, " - Configure keyboard");
                                                sprintf (string, "Green fret: %s", SDL_GetKeyName((SDLKey)playersfret[Chosen][GREEN]));
                                                ordmenu->addOpt(string);
                                                sprintf (string, "Red fret: %s", SDL_GetKeyName((SDLKey)playersfret[Chosen][RED]));
                                                ordmenu->addOpt(string);
                                                sprintf (string, "Yellow fret: %s", SDL_GetKeyName((SDLKey)playersfret[Chosen][YELLOW]));
                                                ordmenu->addOpt(string);
                                                sprintf (string, "Blue fret: %s", SDL_GetKeyName((SDLKey)playersfret[Chosen][BLUE]));
                                                ordmenu->addOpt(string);
                                                sprintf (string, "Orange fret: %s", SDL_GetKeyName((SDLKey)playersfret[Chosen][ORANGE]));
                                                ordmenu->addOpt(string);
                                                sprintf (string, "Starpower: %s", SDL_GetKeyName((SDLKey)playerssp[Chosen]));
                                                ordmenu->addOpt(string);
                                                sprintf (string, "Pick: %s", playerspick[Chosen][0]?"Yes":"No");
                                                ordmenu->addOpt(string);
                                                if (playerspick[Chosen][0]) {
                                                    sprintf (string, "Pick key: %s", SDL_GetKeyName((SDLKey)playerspick[Chosen][0]));
                                                    ordmenu->addOpt(string);
                                                    }
                                                ordmenu->setopt(selected);
                                                while (ordmenu->navigate()) {
                                                    if (songs[menumusic]->isFinished()) {
                                                        songs[menumusic]->unload();
                                                        menumusic=rand()%nSongs;
                                                        songs[menumusic]->load(1.0, 3);
                                                        songs[menumusic]->play(0.5);
                                                        }
                                                    }
                                                selected=ordmenu->opt();
                                                if (!ordmenu->cancel()) {
                                                    char *dest=NULL;
                                                    switch (ordmenu->opt()) {
                                                        case 1: dest=&playersfret[Chosen][GREEN]; break;
                                                        case 2: dest=&playersfret[Chosen][RED]; break;
                                                        case 3: dest=&playersfret[Chosen][YELLOW]; break;
                                                        case 4: dest=&playersfret[Chosen][BLUE]; break;
                                                        case 5: dest=&playersfret[Chosen][ORANGE]; break;
                                                        case 6: dest=&playerssp[Chosen]; break;
                                                        case 7:
                                                            if (playerspick[Chosen][0]) {
                                                                playerspick[Chosen][0]=0;
                                                                break;
                                                                }
                                                            else playerspick[Chosen][1]=0;
                                                        case 8: dest=&playerspick[Chosen][0]; break;
                                                        }
                                                    if (dest!=NULL) {
                                                       CMenu aux(screen, "Press key to map", SIZEX/2, SIZEY/2);
                                                        aux.navigate();
                                                        SDL_Event event;
                                                        SDL_WaitEvent(&event);
                                                        while (event.type!=SDL_KEYDOWN) SDL_WaitEvent(&event);
                                                        *dest=event.key.keysym.sym;
                                                        }
                                                    }
                                                else doneconfig=1;
                                                delete ordmenu;
                                                }
                                            }
                                        else donecontrol=1;
                                        }
                                    delete ordmenu;
                                }
                                break;
                            case 'E':
                                {
                                   CMenu *ordmenu;
                                    bool donextras=0;
                                    while (!donextras) {
                                        screen->clear();
                                        ordmenu = new CMenu (screen, " - Choose player to edit");
                                        ordmenu->addOpt("Player 1");
                                        ordmenu->addOpt("Player 2");
                                        ordmenu->addOpt("Player 3");
                                        ordmenu->addOpt("Player 4");
                                        while (ordmenu->navigate()) {
                                            if (songs[menumusic]->isFinished()) {
                                                songs[menumusic]->unload();
                                                menumusic=rand()%nSongs;
                                                songs[menumusic]->load(1.0, 3);
                                                songs[menumusic]->play(0.5);
                                                }
                                            }
                                        if (!ordmenu->cancel()) {
                                            int Chosen=ordmenu->opt()-1;
                                            delete ordmenu;
                                            bool doneconfig=0;
                                            int selected=0;
                                            while (!doneconfig) {
                                                screen->clear();
                                                ordmenu= new CMenu (screen, " - Extras");
                                                sprintf (string, "Hyperspeed: %d", playersextras[Chosen][HYPERSPEED]);
                                                ordmenu->addOpt(string);
                                                sprintf (string, "Precision Level: %d", playersextras[Chosen][PRECISION]);
                                                ordmenu->addOpt(string);
                                                sprintf (string, "God mode: %s", playersextras[Chosen][GODMODE]?"Yes":"No");
                                                ordmenu->addOpt(string);
                                                sprintf (string, "Always Hammmer-on: %s", playersextras[Chosen][ALLHOPO]?"Yes":"No");
                                                ordmenu->addOpt(string);
                                                ordmenu->setopt(selected);
                                                while (ordmenu->navigate()) {
                                                    if (songs[menumusic]->isFinished()) {
                                                        songs[menumusic]->unload();
                                                        menumusic=rand()%nSongs;
                                                        songs[menumusic]->load(1.0, 3);
                                                        songs[menumusic]->play(0.5);
                                                        }
                                                    }
                                                selected=ordmenu->opt();
                                                if (!ordmenu->cancel()) {
                                                    switch (ordmenu->opt()) {
                                                        case 1: playersextras[Chosen][HYPERSPEED]=(playersextras[Chosen][HYPERSPEED]+1)%6; break;
                                                        case 2: playersextras[Chosen][PRECISION]=(playersextras[Chosen][PRECISION]+1)%3; break;
                                                        case 3: playersextras[Chosen][GODMODE]^=1; break;
                                                        case 4: playersextras[Chosen][ALLHOPO]^=1; break;
                                                        }
                                                    }
                                                else doneconfig=1;
                                                delete ordmenu;
                                                }
                                            }
                                        else donextras=1;
                                        }
                                    delete ordmenu;
                                }
                                break;
                            case 'V':
                                {
                                    CMenu *videomenu;
                                    bool donevideo=0;
                                    int selected=0;
                                    while (!donevideo) {
                                        screen->clear();
                                        videomenu= new CMenu (screen, " - Video");
                                        sprintf (string, "Fullscreen: %s", fullscreen?"Yes":"No");
                                        videomenu->addOpt(string);
                                        sprintf (string, "Visual effects: %s", CSprite::active?"Yes":"No");
                                        videomenu->addOpt(string);
                                        videomenu->addOpt("Back");
                                        videomenu->setopt(selected);
                                        while (videomenu->navigate()) {
                                            if (songs[menumusic]->isFinished()) {
                                                songs[menumusic]->unload();
                                                menumusic=rand()%nSongs;
                                                songs[menumusic]->load(1.0, 3);
                                                songs[menumusic]->play(0.5);
                                                }
                                            }
                                        selected=videomenu->opt();
                                        if (!videomenu->cancel()) {
                                            switch (videomenu->opt()) {
                                                case 1:
                                                    fullscreen^=1;
                                                    delete screen;
                                                    if (fullscreen) {
                                                        video.sizex=video.resolutionx;
                                                        video.sizey=video.resolutiony;
                                                        screen = new CDrawer(SIZEX, SIZEY, 32, SDL_HWSURFACE | SDL_FULLSCREEN);
                                                        }
                                                    else {
                                                        video.sizex=1600>video.resolutionx?video.resolutionx:1600;
                                                        video.sizey=850>video.resolutiony?video.resolutiony:850;
                                                        screen = new CDrawer(SIZEX, SIZEY, 32, SDL_HWSURFACE );
                                                        }
                                                    screen->setcolor(0, 0, 0);
                                                    screen->load_background(wallpaper);
                                                    delete startmenu;
                                                    startmenu=new CMenu(screen, " - Main Menu");
                                                    startmenu->addOpt("Singleplayer");
                                                    startmenu->addOpt("Multiplayer");
                                                    startmenu->addOpt("Practice");
                                                    startmenu->addOpt("Options");
                                                    startmenu->addOpt("Exit");
                                                    delete options;
                                                    options=new CMenu(screen, " - Options");
                                                    options->addOpt("Controls");
                                                    options->addOpt("Extras");
                                                    options->addOpt("Video");
                                                    options->addOpt("Back");
                                                    break;
                                                case 2: CSprite::active^=1; break;
                                                case 3: donevideo=1; break;
                                                }
                                            }
                                        else donevideo=1;
                                        delete videomenu;
                                        }
                                }
                                break;

                            case 'B': doneopt=1; break;
                            }
                        if (options->cancel()) doneopt=1;
                        }
                    screen->clear();
                    delete options;
                }
                break;
            case 'E': done=1; break;
            }
        if (startmenu->cancel()) done=1;
        screen->clear();
        }
    delete startmenu;

    FILE *writer;
    writer=fopen(FilePath("SAVEDATA/", "data", ".dat"), "wb");
    if (writer!=NULL) {
        for (int i=0;i<4;i++) {
            for (int j=0;j<6;j++) {
                playersfret[i][j]^='c';
                }
            }
        for (int i=0;i<4;i++) playerssp[i]^='c';
        for (int i=0;i<4;i++) {
            for (int j=0;j<2;j++) {
                playerspick[i][j]^='c';
                }
            }
        fwrite(playersfret, sizeof(char), 4*5, writer);
        fwrite(playerssp, sizeof(char), 4, writer);
        fwrite(playerspick, sizeof(char), 4*2, writer);
        fwrite(playersextras, sizeof(char), 4*10, writer);
        fwrite(&fullscreen, sizeof(bool), 1, writer);
        fwrite(&CSprite::active, sizeof(bool), 1, writer);
        fclose(writer);
        }

    CMenu::unloadfx();
    CHighway::unload();
    engine->drop();
    SDL_Quit ();
    TTF_Quit ();
    return 0;
}
