menu::menu (char head[]="", int x=50, int y=50): locx(x), locy(y), selected(0), start(NULL), end(NULL), nOpt(0) {
    int size;
    for (size=0;head[size];size++);
    header=new char[++size];
    for (int i=0;i<size;i++) header[i]=head[i];
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
    end->next=0;
    for (size=0;content[size];size++);
    end->content=new char[++size];
    if (end->content==NULL) Error ("Error loading menu");
    for (int i=0;i<size;i++) end->content[i]=content[i];
    nOpt++;
}

bool menu::lastopt() {
    return (nOpt&&selected==nOpt-1);
}

int menu::opt() {
    return selected+1;
}

char* menu::opts() {
    option *aux=start;
    int n=selected;
    while (n--) aux=aux->next;
    return aux->content;
}

bool menu::navigate () {
    char c;
    if (kbhit()) {
        c=getch();
        switch (c) {
            case 72: selected=(selected+nOpt-1)%nOpt; break;
            case 80: selected=(selected+nOpt+1)%nOpt; break;
            case 27: selected=nOpt-1; break;
            }
        }
    cleardevice();
    print();
    swapbuffers();
    return !(c==13||c==27);
}

/*void menu::navigate () {  //SDL
    char c;
    SDL_Event event;
    while (event.key.keysym.sym!=SDLK_ESCAPE&&event.key.keysym.sym!=SDLK_RETURN) {
        while (SDL_PollEvent(&event)) {
            if (event.type==SDL_KEYDOWN)
                switch (event.key.keysym.sym) {
                    case SDLK_UP: selected=(selected+nOpt-1)%nOpt; break;
                    case SDLK_DOWN: selected=(selected+nOpt+1)%nOpt; break;
                    }
            }
        cleardevice();
        print();
        swapbuffers();
        }
    if (event.key.keysym.sym==SDLK_ESCAPE) selected=nOpt-1;
}*/


music::music (FILE *songs, irrklang::ISoundEngine *eng): sound(0), FX(0), source(0), start(0), engine(eng) {
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

void music::load (float speed=1.0) {
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

bool music::isInstrumentAvaliable (int instrument) {
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
     return sound->isFinished();
}

void music::preview (bool active) {
    if (active&&sound==NULL) {
        load();
        sound->setPlayPosition(sound->getPlayLength()/2);
        sound->setVolume(0.3);
        sound->setIsPaused(false);
        }
    else if (!active&&sound!=NULL) unload();
}

bool music::play () {
     if (!sound||!sound->getIsPaused()) return false;
     sound->setVolume(0.8);
     sound->setIsPaused(false);
     for (unsigned int last=sound->getPlayPosition() ; last==sound->getPlayPosition() ; start=clock()*1000/CLOCKS_PER_SEC-sound->getPlayPosition());
     return true;
}

void music::error () {
    int sel=rand()%NERROR;
    engine->play2D(errorsource[sel]);
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
         if (start==0) return ~0;
         if (sound->getIsPaused()) return clock()*1000/CLOCKS_PER_SEC-sound->getPlayPosition();
         return (int)((clock()*1000/CLOCKS_PER_SEC-start)*sound->getPlaybackSpeed());
}
      

highway::highway (music* stream, char instr, int *xtras, char frt[]="ZXCVB", char pck[]="", int loc=SIZEX/2, int w=175, int h=2*SIZEY/3, int col[]=0):
                  MusicStream(stream), instrument(instr), time_delay(300+1200/(xtras[HYPERSPEED]+1)), timing_window(100/(xtras[PRECISION]+1)), godmode(xtras[GODMODE]), location(loc), width(w), height(h), basescore(1), progress(0), score(0), streak(0), rockmeter(500) {
                FILE *chartfile=NULL;
                int i;
                
                if (col==0) {
                    int col_default[]={COLOR(40,200,10), COLOR(200, 0, 0), COLOR(247, 236, 40), COLOR(10, 10, 200), COLOR(255, 102, 0)};
                    col=col_default;
                }

                for (size=0;frt[size];size++);
                fretstate=new short int[size];
                lastfretstate=new short int[size];
                color=new int[size];
                for (i=0;i<size;i++) color[i]=col[i];
                fret=new char[++size];
                for (i=0;i<size;i++) fret[i]=frt[i];
                
                for (size=0;pck[size];size++);
                pickstate=new short int[size];
                lastpickstate=new short int[size];
                pick=new char[++size];
                for (i=0;i<size;i++) pick[i]=pck[i];


                while (instrument>=0&&chartfile==NULL) {
                    char extension[20]="";
                    strcat (extension, "_");
                    switch (instrument) {
                        case DRUMS: strcat (extension,"dru"); break;
                        case BASS: strcat (extension, "bas"); break;
                        case GUITAR: strcat (extension, "gui"); break;
                        }
                    strcat (extension, ".chart");
                    chartfile=fopen(FilePath("Chart/", MusicStream->filename, extension), "rb");
                    if (chartfile==NULL) {
                        switch (instrument) {
                            case DRUMS: printf ("Drums "); break;
                            case BASS: printf ("Bass "); break;
                            case GUITAR: printf ("Guitar "); break;
                            }
                        printf ("chart file for %s not abaliable.\n", MusicStream->title);
                        instrument--;
                        }
                    }
                
                if (chartfile==NULL) Error ("No chart files are avaliable");
                
                CheckChartIntegrity(chartfile, "Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321");
                
                fread (&bpm, sizeof(int), 1, chartfile);
                fread (&size, sizeof(int), 1, chartfile);
                chart=new note[(size>50000)?(size=0):(size)];
                for (i=0;i<size;i++) {
                    fread (&chart[i].type, sizeof (char), 1, chartfile);
                    fread (&chart[i].time, sizeof (int), 1, chartfile);
                    fread (&chart[i].end, sizeof (int), 1, chartfile);
                    chart[i].hit=false;
                    chart[i].hold=chart[i].end>chart[i].time;
                    chart[i].chord=0;
                    if (xtras[ALLHOPO]||(instrument!=DRUMS&&i>1&&(chart[i].time-chart[i-1].end)<30000/bpm&&chart[i].type!=chart[i-1].type)) chart[i].hopo=true;
                    else chart[i].hopo=false;
                    for (int j=0;fret[j];j++) if ((chart[i].type>>j)%2) chart[i].chord++;
                    }

                CheckChartIntegrity(chartfile, "End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847");
                }

highway::~highway () {
                  delete[] fretstate;
                  delete[] lastfretstate;
                  delete[] fret;
                  delete[] pickstate;
                  delete[] lastpickstate;
                  delete[] pick;
                  delete[] chart;
                  delete[] color;
                  }

int highway::multiplier () {
    if (streak>30) return 4;
    return (streak/10)+1;
}

long long int highway::refresh () {
                    int time=MusicStream->time();
                    int i;
                    char fretaux;
                    bool picked;

                    if (time==~0) return score/10000;

                    for (i=0;fret[i];i++) {
                        lastfretstate[i]=fretstate[i];
                        fretstate[i]=GetAsyncKeyState(fret[i])!=0;
                        }
                    for (i=0;pick[i];i++) {
                        lastpickstate[i]=pickstate[i];
                        pickstate[i]=GetAsyncKeyState(pick[i])!=0;
                        }

                    while (progress<size&&time-chart[progress].time>timing_window*SIZEY/time_delay) {
                        if (chart[progress].hit==false) {
                            rockmeter-=20;
                            basescore+=(1000000+(chart[progress].end-chart[progress].time)*bpm)*chart[progress].chord;
                            if (streak>0) MusicStream->error();
                            streak=0;
                            MusicStream->hitting(instrument, false);
                            }
                        progress++;
                        }
                    if (chart[progress].time-time>time_delay&&chart[progress-1].end<time) MusicStream->hitting(instrument);
                    picked=false;
                    for (i=0, fretaux=0;fret[i];i++) fretaux+=fretstate[i]<<i;
                    for (i=0, picked=0;pick[i];i++) if ((pickstate[i]^lastpickstate[i])&pickstate[i]) picked=true;
                    if (!i) for (i=0;fret[i];i++) if ((fretstate[i]^lastfretstate[i])&fretstate[i]) picked=true;

                    for (int j=progress;picked&&j<size&&chart[j].time-time<timing_window;j++) {     //notas normais/acordes
                            if (!chart[j].hit) {
                                    if (((chart[j].chord-1)&&((fretaux^(chart[j].type))==0))||
                                        (!(chart[j].chord-1) &&(((pick[0]&&((fretaux^(chart[j].type))<chart[j].type)))||
                                                               ((!pick[0]&&(fretaux^(chart[j].type))&(chart[j].type))==0)))) {
                                           rockmeter+=21-rockmeter/50;
                                           chart[j].hit=true;
                                           MusicStream->hitting(instrument);
                                           basescore+=(1000000+(chart[j].end-chart[j].time)*bpm)*chart[j].chord;
                                           streak++;
                                           score+=1000000*chart[j].chord*multiplier();
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
                        for (i=0, fretaux=0;fret[i];i++) fretaux+=((fretstate[i]^lastfretstate[i])&fretstate[i])<<i;
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
                    for (i=0, fretaux=0;fret[i];i++) fretaux+=fretstate[i]<<i;                  //hopo
                    for (i=0;fret[i];i++) {
                        if (!picked) picked=fretstate[i]^lastfretstate[i];
                        else picked=!(fretstate[i]&lastfretstate[i]);
                    }
                    for (int j=progress;picked&&j<size&&chart[j].time-time<timing_window;j++) {
                        if (chart[j-1].hit&&!chart[j].hit&&chart[j].hopo==true) {
                                    if (((chart[j].chord-1)&&((fretaux^(chart[j].type))==0))||(!(chart[j].chord-1)&&((fretaux^(chart[j].type))&(chart[j].type))==0)) {
                                        rockmeter+=21-rockmeter/50;
                                        chart[j].hit=true;
                                        MusicStream->hitting(instrument);
                                        basescore+=(1000000+(chart[j].end-chart[j].time)*bpm)*chart[j].chord;
                                        streak++;
                                        score+=1000000*chart[j].chord*multiplier();
                                        picked=false;
                                        }
                                    }
                        }
                    if (rockmeter>1000) rockmeter=1000;
                    if (rockmeter<0) rockmeter=0;

                    draw(time);
                    return score/10000;
}

bool highway::alive() {
    if (!godmode&&rockmeter==0) MusicStream->lose();
    return godmode||rockmeter>0;
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
     showerrorbox (string);
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
