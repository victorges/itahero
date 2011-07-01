drawer::drawer (SDL_Surface *surf):surface(surf), keycolor(SDL_MapRGBA ( surface->format, 0, 255, 255, 255 )) {}

drawer::drawer (int width, int height, int bpp, Uint32 flags):
    surface(SDL_SetVideoMode(width, height, bpp, flags)), keycolor(SDL_MapRGBA ( surface->format, 0, 255, 255, 255 )) {}

drawer::drawer (Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask):
    surface(SDL_CreateRGBSurface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask)),
    keycolor(SDL_MapRGBA ( surface->format, 0, 255, 255, 255 )) {}
    
void drawer::setcolor (Uint8 R, Uint8 G, Uint8 B, Uint8 A) {
    maincolor=SDL_MapRGBA(surface->format, R, G, B, A);
}

void drawer::setcolor (Uint32 color) {
    maincolor=color;
}

void drawer::Flip () {
    SDL_Flip(surface);
    }

void drawer::clear () {
    SDL_Rect all;
    all.x=0;
    all.y=0;
    all.h=surface->w-1;
    all.w=surface->h-1;
    SDL_FillRect(surface, &all, 0);
}

void note::operator()(FILE *chartfile) {
    fread (&type, sizeof (char), 1, chartfile);
    fread (&time, sizeof (int), 1, chartfile);
    fread (&end, sizeof (int), 1, chartfile);
    hit=false;
    hold=end>time;
    chord=0;
    for (int j=0;j<5;j++) if ((type>>j)%2) chord++;
}


menu::menu (char head[]="", int x=50, int y=50): locx(x), locy(y), selected(0), start(NULL), end(NULL), nOpt(0) {
    int size;
    for (size=0;head[size];size++);
    header=new char[++size];
    for (int i=0;i<size;i++) header[i]=head[i];
    sizex=textwidth(header);
    sizey=textheight(header);
    }

menu::~menu () {
    option *aux;
    while (start) {
        aux=start->next;
        delete start;
        start=aux;
        }
}

void menu::addOpt (char content[]) {
    int size;
    if (end==NULL) {
        start=end=new option;
        if (end==NULL) Error ("Error loading menu");
        }
    else {
        end->next=new option;
        if (end->next==NULL) Error ("Error loading menu");
        end=end->next;
        }
    end->next=NULL;
    for (size=0;content[size];size++);
    end->content=new char[++size];
    if (end->content==NULL) Error ("Error loading menu");
    for (int i=0;i<size;i++) end->content[i]=content[i];
    sizey+=textheight(end->content);
    if (textwidth(end->content)>sizex) sizex=textwidth(end->content);
    nOpt++;
}

bool menu::cancel() {
    return (nOpt&&selected==nOpt);
}

int menu::opt() {
    return selected+1;
}

char* menu::opts() {
    option *aux=start;
    int n=selected;
    while (n--) aux=aux->next;
    if (aux) return aux->content;
    return "";
}

/*bool menu::navigate () {
    print();
    swapbuffers();
    if (kbhit()) {
        char c=getch();
        switch (c) {
            case 72: selected=(selected+nOpt-1)%nOpt; return 1;
            case 80: selected=(selected+nOpt+1)%nOpt; return 1;
            case 27: case 8: selected=nOpt; case 13: return 0;
            }
        }
    return 1;
}*/

bool menu::navigate () {  //SDL
    print();
    swapbuffers();
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type==SDL_KEYDOWN)
            switch (event.key.keysym.sym) {
                case SDLK_UP: selected=(selected+nOpt-1)%nOpt; return 1;
                case SDLK_DOWN: selected=(selected+nOpt+1)%nOpt; return 1;
                case SDLK_ESCAPE: case SDLK_BACKSPACE: selected=nOpt; case SDLK_RETURN: return 0;
                }
        else if (event.type==SDL_QUIT) exit(0);
        }
    return 1;
}


music::music (FILE *songs, irrklang::ISoundEngine *eng): sound(0), FX(0), source(0), start(0), limit(16), engine(eng) {
          char string[1000];
          int i;

          sfscanf (songs, "FILE=");
          fgets(string, 1000, songs);
          for (i=1;string[i-1]!='\n';i++);
          string[i-1]=0;
          filename=new char[i];
          for (i=0;!i||string[i-1];i++) filename[i]=string[i];

          sfscanf (songs, "TITLE=");
          fgets(string, 1000, songs);
          for (i=1;string[i-1]!='\n';i++);
          string[i-1]=0;
          title=new char[i];
          for (i=0;!i||string[i-1];i++) title[i]=string[i];

          sfscanf (songs, "ARTIST=");
          fgets(string, 1000, songs);
          for (i=1;string[i-1]!='\n';i++);
          string[i-1]=0;
          artist=new char[i];
          for (i=0;!i||string[i-1];i++) artist[i]=string[i];
}

music::~music () {
                delete[] filename;
                delete[] title;
                delete[] artist;
                }

void music::load (float speed=1.0, int starts=0, int ends=16) {
     void *soundfile;
     size_t size;
     soundfile=AllocateFile(FilePath("Sound/", filename, ".ogg"), size);
     if (soundfile==NULL) Error ("Sound File not found");
     source=engine->addSoundSourceFromMemory(soundfile, size, title);
     free (soundfile);
     if (source==NULL) Error ("Error loading Sound File");
     source->setStreamMode(irrklang::ESM_AUTO_DETECT);
     sound=engine->play2D(source, false, true, true, true);
     FX=sound->getSoundEffectControl();
     sound->setPlayPosition (starts*sound->getPlayLength()/16);
     limit=ends;
     section=starts;
     for (int i=0;i<NERROR;i++) {
            char string[100];
            sprintf (string, "error%d", i+1);
            soundfile=AllocateFile(FilePath("FX/", string, ".ogg"), size);
            if (soundfile==NULL) Error ("Error Sound File not found");
            errorsource[i]=engine->addSoundSourceFromMemory(soundfile, size, string);
            free (soundfile);
            if (source==NULL) Error ("Error loading Error Sound File");
            errorsource[i]->setStreamMode(irrklang::ESM_AUTO_DETECT);
            }
    sound->setPlaybackSpeed (speed);
    start=clock()*1000/CLOCKS_PER_SEC;
}

void music::reload () {
    sound->setIsPaused(true);
    sound->setPlayPosition(section*sound->getPlayLength()/16);
}

void music::unload () {
     if (sound) {
                if (!sound->isFinished() && !sound->getIsPaused()) sound->stop();
                sound->drop();
                sound=0;
                FX=0;
                start=0;
                }
     engine->removeSoundSource(source);
     for (int i=0;i<NERROR;i++) engine->removeSoundSource (errorsource[i]);
}

bool music::isInstrumentAvaliable (en_instrument instrument) {
    char extension[20]="";
    strcat (extension, "_");
    switch (instrument) {
        case DRUMS: strcat (extension,"dru"); break;
        case BASS: strcat (extension, "bas"); break;
        case GUITAR: strcat (extension, "gui"); break;
        }
    strcat (extension, ".chart");
    FILE *file=fopen(FilePath("Chart/", filename, extension), "rb");
    fclose(file);
    return file!=NULL;
}

bool music::isFinished () {
     return sound->isFinished()||sound->getPlayPosition()>limit*sound->getPlayLength()/16;
}

void music::preview (bool active, int sixteenth=8) {
    static int current=sixteenth;
    if (active) {
        if (sound==NULL) {
            load();
            sound->setPlayPosition(sixteenth*sound->getPlayLength()/16);
            sound->setVolume(0.0);
            sound->setIsPaused(false);
            }
        else if (current!=sixteenth) {
            current=sixteenth;
            sound->setPlayPosition(sixteenth*sound->getPlayLength()/16);
            sound->setVolume(0.0);
            }
        else if (sound->getPlayPosition()>(sixteenth+1)*sound->getPlayLength()/16) {
            if (sound->getVolume()>0.0) sound->setVolume(sound->getVolume()-0.0005);
            else sound->setPlayPosition(sound->getPlayLength()/2);
            }
        else if (sound->getVolume()<0.4) sound->setVolume(sound->getVolume()+0.001);
        }
    else if (sound!=NULL) unload();
}

bool music::play () {
     if (!sound||!sound->getIsPaused()) return false;
     sound->setIsPaused(false);
     sound->setVolume(1.0);
     for (unsigned int last=sound->getPlayPosition() ; last==sound->getPlayPosition() ; start=(clock()*1000/CLOCKS_PER_SEC-(unsigned int)((float)sound->getPlayPosition()/sound->getPlaybackSpeed())));
     return true;
}

void music::error () {
    int sel=rand()%NERROR;
    engine->play2D(errorsource[sel], false, false, false, true)->setVolume(0.8);
}

void music::lose() {
    engine->play2D(FilePath("FX/", "losing", ".ogg"));
}

void music::starpower(bool active=true) {
    if (active) if (!FX->isDistortionSoundEffectEnabled()) FX->enableDistortionSoundEffect();
    else if (FX->isDistortionSoundEffectEnabled()) FX->disableDistortionSoundEffect();
}

void music::hitting(char instrument, bool active=true) {
    if (active)
        switch (instrument) {
            case GUITAR: if (FX->isParamEqSoundEffectEnabled()) FX->disableParamEqSoundEffect(); break;
            case DRUMS: if (FX->isCompressorSoundEffectEnabled()) FX->disableCompressorSoundEffect(); break;
            case BASS: if (FX->isParamEqSoundEffectEnabled()) FX->disableParamEqSoundEffect(); break;
            }
    else
        switch (instrument) {
            case GUITAR:  if (!FX->isParamEqSoundEffectEnabled()) FX->enableParamEqSoundEffect(3000, 12, -15); break;
            case DRUMS: if (!FX->isCompressorSoundEffectEnabled()) FX->enableCompressorSoundEffect(-12); break;
            case BASS: if (!FX->isParamEqSoundEffectEnabled()) FX->enableParamEqSoundEffect(100, 12, -15); break;
            }
}

bool music::pause () {
     if (!sound||sound->getIsPaused()) return false;
     sound->setIsPaused(true);
     start=0;
     return true;
}

int music::time () {
         if (sound==NULL) Error ("Asked for status with no song playing");
         if (sound->getIsPaused()) return sound->getPlayPosition();
         return (int)((clock()*1000/CLOCKS_PER_SEC-start)*sound->getPlaybackSpeed());
}


highway::highway (SDL_Surface *screen, music* stream, en_instrument instr, en_difficulty difficulty, int *extras, char frt[]="ZXCVB", char pck[]="", int loc=SIZEX/2, int w=175, int h=2*SIZEY/3, int col[]=0):
                  visual(new drawer(screen)), MusicStream(stream), instrument(instr), time_delay(1000/(difficulty+1)+1200/(extras[HYPERSPEED]+1)), timing_window(100/(extras[PRECISION]+1)), godmode(extras[GODMODE]), allhopo(extras[ALLHOPO]), practice(extras[PRACTICE]), location(loc), width(w), height(h), basescore(1), progress(0), score(0), streak(0), rockmeter(500)
                {
                FILE *chartfile=NULL;
                int i;
                if (col==0) {
                    int col_default[]={COLOR(40,200,10), COLOR(200, 0, 0), COLOR(247, 236, 40), COLOR(10, 10, 200), COLOR(255, 102, 0)};
                    col=col_default;
                }

                fretstate=new short int[5];
                color=new int[5];
                for (i=0;i<5;i++) color[i]=col[i];
                fret=new char[5];
                for (i=0;i<5;i++) fret[i]=frt[i];

                for (size=0;pck[size];size++);
                pickstate=new short int[size];
                pick=new char[++size];
                for (i=0;i<size;i++) pick[i]=pck[i];


                {
                    char extension[20]="";
                    strcat (extension, "_");
                    switch (instrument) {
                        case DRUMS: strcat (extension,"dru"); break;
                        case BASS: strcat (extension, "bas"); break;
                        case GUITAR: strcat (extension, "gui"); break;
                        }
                    strcat (extension, ".chart");
                    chartfile=fopen(FilePath("Chart/", MusicStream->filename, extension), "rb");
                }

                if (chartfile==NULL) Error ("No chart files are avaliable");

                CheckChartIntegrity(chartfile, "Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321");

                fread (&bpm, sizeof(int), 1, chartfile);
                fread (&size, sizeof(int), 1, chartfile);
                chart=new note[(size>50000)?(size=0):(size)];
                for (i=0;i<size;i++) {
                    bool cond;
                    do {
                        chart[i](chartfile);
                        if (i==0) break;
                        
                        cond=false;
                        switch (difficulty) {
                            case EASY:
                                for (int j=4;chart[i].chord>1;j++) {
                                    if ((chart[i].type&~(1<<j))!=chart[i].type) {
                                        chart[i].type=chart[i].type&~(1<<j);
                                        chart[i].chord--;
                                        }
                                    }
                                if (chart[i].type>=(1<<3)) chart[i].type=chart[i].type>>2;
                                if (chart[i].time-chart[i-1].time<300) {
                                    cond=true;
                                    size--;
                                    }
                                break;
                            case MEDIUM:
                                for (int j=4;chart[i].chord>2;j++) {
                                    if (chart[i].type&(~(1<<j))!=chart[i].type) {
                                        chart[i].type=chart[i].type&(~(1<<j));
                                        chart[i].chord--;
                                        }
                                    }
                                if (chart[i].type>=(1<<4)) chart[i].type=chart[i].type>>1;
                                if (chart[i].time-chart[i-1].time<200||(chart[i].hopo==2&&chart[i].time-chart[i-1].time<300)) {
                                    cond=true;
                                    size--;
                                }
                                break;
                            case HARD:
                                if (chart[i].time-chart[i-1].time<80||(chart[i].hopo>=2&&chart[i].time-chart[i-1].time<150)) {
                                    cond=true;
                                    size--;
                                    }
                                break;
                            }
                        } while (cond&&i<size);
                    chart[i].hopo=(allhopo||(instrument!=DRUMS&&i>1&&(chart[i].time-chart[i-1].end)<30000/bpm&&chart[i].type!=chart[i-1].type));
                    }
                for (int time=MusicStream->time() ; progress<size&&time-chart[progress].time>timing_window ; progress++);
                CheckChartIntegrity(chartfile, "End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847");
}

highway::~highway () {
                  delete[] fretstate;
                  delete[] fret;
                  delete[] pickstate;
                  delete[] pick;
                  delete[] chart;
                  delete[] color;
                  }

void highway::reset () {
        delete[] chart;
        
        basescore=1;
        progress=score=streak=0;
        rockmeter=500;

        FILE *chartfile;
        {
            char extension[20]="";
            strcat (extension, "_");
            switch (instrument) {
                case DRUMS: strcat (extension,"dru"); break;
                case BASS: strcat (extension, "bas"); break;
                case GUITAR: strcat (extension, "gui"); break;
                }
            strcat (extension, ".chart");
            chartfile=fopen(FilePath("Chart/", MusicStream->filename, extension), "rb");
        }

        if (chartfile==NULL) Error ("No chart files are avaliable");

        CheckChartIntegrity(chartfile, "Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321");

        fread (&bpm, sizeof(int), 1, chartfile);
        fread (&size, sizeof(int), 1, chartfile);
        chart=new note[(size>50000)?(size=0):(size)];
        for (int i=0;i<size;i++) {
            chart[i](chartfile);
            chart[i].hopo=(allhopo||(instrument!=DRUMS&&i>1&&(chart[i].time-chart[i-1].end)<30000/bpm&&chart[i].type!=chart[i-1].type));
            }
        for (int time=MusicStream->time() ; progress<size&&time-chart[progress].time>timing_window ; progress++);
        CheckChartIntegrity(chartfile, "End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847");
}

int highway::multiplier () {
    if (streak>30) return 4;
    return (streak/10)+1;
}

long long int highway::refresh (Uint8* keyboard) {
                    int time=MusicStream->time();
                    int i;
                    char fretaux, newfretstate[5], newpickstate[5];
                    bool picked;
                    
                    if (time==~0) return score/10000;

                    for (i=0;i<5;i++) {
                        //newnewfretstate[i]=GetAsyncKeyState(fret[i])!=0;
                        newfretstate[i]=keyboard[fret[i]]!=0;
                        }
                    for (i=0;pick[i];i++) {
                        //newpickstate[i]=GetAsyncKeyState(pick[i])!=0;
                        newpickstate[i]=keyboard[pick[i]]!=0;
                        }

                    while (progress<size&&time-chart[progress].time>timing_window) {
                        if (chart[progress].hit==false) {
                            rockmeter-=20;
                            basescore+=(500000+(chart[progress].end-chart[progress].time)*bpm)*chart[progress].chord;
                            if (streak>0) MusicStream->error();
                            streak=0;
                            MusicStream->hitting(instrument, false);
                            }
                        progress++;
                        }
                    if (chart[progress].time-time>time_delay&&chart[progress-1].end<time) MusicStream->hitting(instrument);

                    for (i=0, fretaux=0;i<5;i++) fretaux+=newfretstate[i]<<i;
                    for (i=0, picked=false;pick[i];i++) if ((newpickstate[i]^pickstate[i])&newpickstate[i]) picked=true;
                    if (!i) for (i=0;i<5;i++) if ((newfretstate[i]^fretstate[i])&newfretstate[i]) picked=true;

                    for (int j=progress;picked&&j<size&&chart[j].time-time<timing_window;j++) {     //notas normais/acordes
                            if (!chart[j].hit) {
                                    if (((chart[j].chord-1)&&((fretaux^(chart[j].type))==0))||
                                        (!(chart[j].chord-1) &&(fretaux^(chart[j].type))<chart[j].type)) {
                                           rockmeter+=21-rockmeter/50;
                                           chart[j].hit=true;
                                           MusicStream->hitting(instrument);
                                           basescore+=(500000+(chart[j].end-chart[j].time)*bpm)*chart[j].chord;
                                           streak++;
                                           score+=500000*chart[j].chord*multiplier();
                                           picked=chart[j].hopo=false;
                                           }
                                    }
                            }

                    for (int j=progress;j<size&&chart[j].time-time<timing_window;j++) {             //sustain
                        if (chart[j].hit&&chart[j].hold) {
                                        if (streak>0&&(((fretaux^(chart[j].type))&(chart[j].type))==0)&&chart[j].time<chart[j].end) {
                                                    score+=((time-chart[j].time))*chart[j].chord*multiplier()*bpm;
                                                    chart[j].time=time;
                                                    fretaux^=chart[j].type;
                                                }
                                        else {
                                            chart[j].hold=false;
                                            if (chart[j].time<chart[j].end) MusicStream->hitting(instrument, false);
                                            }
                                        }
                        }
                    if (!pick[0]) {
                        for (i=0, fretaux=0;i<5;i++) {
                            fretaux+=((newfretstate[i]^fretstate[i])&newfretstate[i])<<i;
                            if (fretaux&&newfretstate[i]) fretaux=newfretstate[i]<<i;
                            }
                        for (int j=progress;picked&&j<size&&chart[j].time-time<timing_window;j++) {
                            if (!chart[j].hit&&((chart[j].type)&fretaux)!=0) picked=false;
                            }
                        }
                    for (int j=progress;picked&&j<size&&chart[j].time-time<timing_window;j++) {
                        if (chart[j].hit&&chart[j].hopo) picked=chart[j].hopo=false;
                        }
                    if (picked) {
                        rockmeter-=20;
                        streak=0;
                        MusicStream->error();
                        }
                    picked=false;
                    for (i=0, fretaux=0;i<5;i++) fretaux+=newfretstate[i]<<i;                  //hopo
                    for (i=0;i<5;i++) {
                        if (!picked) picked=newfretstate[i]^fretstate[i];
                        else picked=!(newfretstate[i]&fretstate[i]);
                    }
                    for (int j=progress;picked&&j<size&&chart[j].time-time<timing_window;j++) {
                        if (chart[j-1].hit&&!chart[j].hit&&chart[j].hopo) {
                                    if (((chart[j].chord-1)&&((fretaux^(chart[j].type))==0))||
                                        (!(chart[j].chord-1) &&(fretaux^(chart[j].type))<chart[j].type)) {
                                        rockmeter+=21-rockmeter/50;
                                        chart[j].hit=true;
                                        MusicStream->hitting(instrument);
                                        basescore+=(500000+(chart[j].end-chart[j].time)*bpm)*chart[j].chord;
                                        streak++;
                                        score+=500000*chart[j].chord*multiplier();
                                        picked=false;
                                        }
                                    }
                        }
                    if (rockmeter>1000) rockmeter=1000;
                    if (rockmeter<0) rockmeter=0;

                    for (i=0;i<5;i++) fretstate[i]=newfretstate[i];
                    for (i=0;pick[i];i++) pickstate[i]=newpickstate[i];

                    draw(time);
                    return score/10000;
}

bool highway::alive() {
    if (!godmode&&!practice&&rockmeter==0) MusicStream->lose();
    return godmode||practice||rockmeter>0;
}

void *AllocateFile (char file_name[], size_t &size) {
     FILE *file;
     void *ret;
     file=fopen(file_name, "rb");
     if (file==NULL) return NULL;
     fseek (file, 0, 2);
     size=ftell(file);
     ret=new char[size];
     if (ret==NULL) return NULL;
     fseek (file, 0, 0);
     fread (ret, size, 1, file);
     fclose(file);
     return ret;
}

void Error (char string[]) {
     closegraph();
     MessageBox(GetActiveWindow(), string, NULL, MB_OK);
     exit(1);
}

void CheckChartIntegrity (FILE *chartfile, char CheckString[]) {
     int i;
     for (i=1;CheckString[i-1];i++);
     char string[i];
     fread (string, sizeof(char), i, chartfile);
     for (i=0;CheckString[i]&&string[i]==CheckString[i];i++);
     if (CheckString[i]) Error ("Chart File corrupted");
}
