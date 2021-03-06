CDrawer::CDrawer (SDL_Surface *surf): surface(surf), background(NULL) {
    keycolor=SDL_MapRGBA ( surface->format, 0, 255, 255, 255 );
    font=NULL;
    settextstyle();
}

CDrawer::CDrawer (int width, int height, int bpp, Uint32 flags): surface(SDL_SetVideoMode(width, height, bpp, flags)), background(NULL) {
    keycolor=SDL_MapRGBA ( surface->format, 0, 255, 255, 255 );
    font=NULL;
    settextstyle();
}

CDrawer::CDrawer (Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask): surface(SDL_CreateRGBSurface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask)), background(NULL) {
    keycolor=SDL_MapRGBA ( surface->format, 0, 255, 255, 255 );
    font=NULL;
    settextstyle();
}

void CDrawer::load_background(CDrawer *background=NULL) {
    if (background==NULL) {
        this->background=NULL;
        return;
        }
    this->background=background;
    this->background->resize(surface->w,surface->h);
}

void CDrawer::settextstyle (char fnt[], SDL_Color *tcolor, int tsize) {
    TTF_CloseFont (font);
    if (tcolor==NULL) textcolor.r=textcolor.g=textcolor.b=255;
    else textcolor=*tcolor;
    textsize=tsize;
    font=TTF_OpenFont (FilePath("Font/", fnt, ".ttf"), textsize);
    if (font==NULL) {
        printf ("Error loading font");
        settextstyle();
        }
}

int CDrawer::textheight(char string[]) {
    int h;
    TTF_SizeText(font, string, NULL, &h);
    return h;
}

int CDrawer::textwidth(char string[]) {
    int w;
    TTF_SizeText(font, string, &w, NULL);
    return w;
}

int CDrawer::get_height() {
    return surface->h;
}

int CDrawer::get_width() {
    return surface->w;
}

void CDrawer::setalpha (Uint8 A) {
    SDL_SetAlpha(surface, SDL_SRCALPHA, A);
}

void CDrawer::setkey (Uint32 color) {
    SDL_SetColorKey (surface, SDL_SRCCOLORKEY, keycolor = color);
}

void CDrawer::setcolor (Uint8 R, Uint8 G, Uint8 B, Uint8 A) {
    maincolor=SDL_MapRGBA(surface->format, R, G, B, A);
}

Uint32 CDrawer::color (Uint8 R, Uint8 G, Uint8 B, Uint8 A) {
    return SDL_MapRGBA(surface->format, R, G, B, A);
}

void CDrawer::Flip () {
    SDL_Flip(surface);
}

void CDrawer::clear () {
    if (background==NULL) SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    else background->apply_surface(0, 0, this);
}

void CNote::operator()(FILE *chartfile) {
    fread (&type, sizeof (char), 1, chartfile);
    fread (&time, sizeof (int), 1, chartfile);
    fread (&end, sizeof (int), 1, chartfile);
    hit=false;
    hold=end>time;
    chord=0;
    for (int j=0;j<5;j++) if ((type>>j)%2) chord++;
}

irrklang::ISoundEngine* CMenu::engine;

void CMenu::loadfx (irrklang::ISoundEngine* eng) {
    engine=eng;
    void *soundfile;
    size_t size;
    char string[100];

    soundfile=AllocateFile(FilePath("FX/", "menumove", ".ogg"), size);
    if (soundfile==NULL) Error ("Menu FX File not found");
    if (engine->addSoundSourceFromMemory(soundfile, size, "menumove")==NULL) Error ("Error loading Menu FX File");
    free (soundfile);

    soundfile=AllocateFile(FilePath("FX/", "menuenter", ".ogg"), size);
    if (soundfile==NULL) Error ("Menu FX File not found");
    if (engine->addSoundSourceFromMemory(soundfile, size, "menuenter")==NULL) Error ("Error loading Menu FX File");
    free (soundfile);

    soundfile=AllocateFile(FilePath("FX/", "menuback", ".ogg"), size);
    if (soundfile==NULL) Error ("Menu FX File not found");
    if (engine->addSoundSourceFromMemory(soundfile, size, "menuback")==NULL) Error ("Error loading Menu FX File");
    free (soundfile);
}

void CMenu::unloadfx () {
    engine->removeSoundSource("menumove");
    engine->removeSoundSource("menuenter");
    engine->removeSoundSource("menuback");
}

void CMenu::effect (char name[]) {
    engine->play2D(name);
}

CMenu::CMenu (CDrawer *vsl, char head[]="", int x=3*SIZEX/8, int y=3*SIZEY/4): visual(vsl), locx(x), locy(y), selected(0), start(NULL), end(NULL), nOpt(0) {
    int size;
    visual->settextstyle("lazy", NULL, 35);
    for (size=0;head[size];size++);
    header=new char[++size];
    for (int i=0;i<size;i++) header[i]=head[i];
    sizex=visual->textwidth(header)+10;
    sizey=visual->textheight(header);
    }

CMenu::~CMenu () {
    option *aux;
    while (start) {
        aux=start->next;
        delete start;
        start=aux;
        }
}

void CMenu::print () {
    option *aux=start;

    visual->settextstyle("lazy", NULL, 35);
    
    visual->bar(locx-sizex/2-10, locy-sizey/2-10, locx+sizex/2+10, locy+sizey/2+10, 0);
    visual->rectangle(locx-sizex/2-11, locy-sizey/2-11, locx+sizex/2+11, locy+sizey/2+11, visual->color(100, 100, 100));
    visual->textxy(header, locx-sizex/2, locy-sizey/2);

    int y=visual->textheight(header);
    for (int curr=0;aux;curr++) {
        if (curr==selected) visual->bar(locx-sizex/2-5, locy-sizey/2+y-2, locx-sizex/2+visual->textwidth(aux->content)+5, locy-sizey/2+y+visual->textheight(aux->content)+2, visual->color(223, 159, 26));
        visual->textxy(aux->content, locx-sizex/2, locy-sizey/2+y);
        y+=visual->textheight(aux->content);
        aux=aux->next;
        }
    visual->Flip();
}

void CMenu::addOpt (char content[]) {
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
    
    sizey+=visual->textheight(end->content);
    if (locy-sizey/2-10<0) locy=sizey/2+110;
    if (locy+sizey/2+10>SIZEY) locy=SIZEY-sizey/2-110;
    if (visual->textwidth(end->content)+10>sizex) sizex=visual->textwidth(end->content)+10;
    if (locx-sizex/2-10<0) locx=sizex/2+110;
    nOpt++;
}

bool CMenu::cancel() {
    return (nOpt&&selected==nOpt);
}

int CMenu::opt() {
    return selected+1;
}

void CMenu::setopt(int sel) {
    if (sel>0&&(--sel)<nOpt) selected=sel;
}

char* CMenu::opts() {
    option *aux=start;
    int n=selected;
    while (n--) aux=aux->next;
    if (aux) return aux->content;
    return "";
}

bool CMenu::navigate () {  //SDL
    CHighway::load();
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP: effect("menumove"); selected=(selected+nOpt-1)%nOpt; return 1;
                    case SDLK_DOWN: effect("menumove"); selected=(selected+nOpt+1)%nOpt; return 1;
                    case SDLK_ESCAPE: case SDLK_BACKSPACE: effect("menuback"); selected=nOpt; return 0;
                    case SDLK_RETURN: effect("menuenter"); return 0;
                    }
                break;
            case SDL_ACTIVEEVENT:
                if (event.active.state&SDL_APPINPUTFOCUS) {
                    SDL_ShowCursor(SDL_ENABLE);
                    while (event.type!=SDL_ACTIVEEVENT||!(event.active.state&SDL_APPINPUTFOCUS)||!event.active.gain) SDL_PollEvent(&event);
                    SDL_ShowCursor(SDL_DISABLE);
                    }
                break;
            case SDL_QUIT: exit(0); break;
            }
        }
    print();
    return 1;
}


CMusic::CMusic (FILE *songs, irrklang::ISoundEngine *eng): sound(0), FX(0), source(0), start(0), limit(16), engine(eng) {
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

CMusic::~CMusic () {
                delete[] filename;
                delete[] title;
                delete[] artist;
                }

void CMusic::load (float speed=1.0, int starts=0, int ends=16) {
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
            if (errorsource[i]==NULL) Error ("Error loading Error Sound File");
            errorsource[i]->setStreamMode(irrklang::ESM_AUTO_DETECT);
            }
    sound->setPlaybackSpeed (speed);
    start=SDL_GetTicks();
}

void CMusic::reload () {
    sound->setIsPaused(true);
    sound->setPlayPosition(section*sound->getPlayLength()/16);
}

void CMusic::unload () {
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

bool CMusic::isInstrumentAvaliable (en_instrument instrument) {
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

bool CMusic::isFinished () {
     if (sound==NULL) return 1;
     return sound->isFinished()||sound->getPlayPosition()>limit*sound->getPlayLength()/16;
}

void CMusic::preview (bool active, int sixteenth=8) {
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
        else if (sound->getVolume()<0.4) sound->setVolume(sound->getVolume()+0.002);
        }
    else if (sound!=NULL) unload();
}

bool CMusic::play (float volume) {
     if (!sound||!sound->getIsPaused()) return false;
     sound->setVolume(volume);
     sound->setIsPaused(false);
     for (unsigned int last=sound->getPlayPosition() ; last==sound->getPlayPosition() ; start=(SDL_GetTicks()-(unsigned int)((float)sound->getPlayPosition()/sound->getPlaybackSpeed())));
     return true;
}

bool CMusic::pause () {
     if (!sound||sound->getIsPaused()) return false;
     sound->setIsPaused(true);
     start=0;
     return true;
}

void CMusic::settime(int atime) {
    int dt=atime-time();
    if (atime>0&&atime<sound->getPlayLength()) sound->setPlayPosition(atime);
    else if (atime<0) sound->setPlayPosition(0);
    else if (atime>sound->getPlayLength()) sound->setPlayPosition(sound->getPlayLength());
    if (sound->getIsPaused()==false) start-=dt;
}

void CMusic::settimerel(int dt) {
    if (time()+dt>0&&time()+dt<sound->getPlayLength()) sound->setPlayPosition(time()+dt);
    else if (time()+dt<0) sound->setPlayPosition(0);
    else if (time()+dt<sound->getPlayLength()) sound->setPlayPosition(sound->getPlayLength());
    if (sound->getIsPaused()==false) start-=dt;
}

float CMusic::speed() {
    return sound->getPlaybackSpeed();
}

void CMusic::error () {
    int sel=rand()%NERROR;
    engine->play2D(errorsource[sel], false, false, false, true)->setVolume(0.8);
}

void CMusic::effect(char string[]) {
    engine->play2D(FilePath("FX/", string, ".ogg"));
}

void CMusic::starpower(bool active=true) {
    if (active) {
        if (!FX->isFlangerSoundEffectEnabled()) FX->enableFlangerSoundEffect();
        }
    else if (FX->isFlangerSoundEffectEnabled()) FX->disableFlangerSoundEffect();
}

void CMusic::hitting(char instrument, bool active=true) {
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

int CMusic::time () {
         if (sound==NULL) Error ("Asked for status with no song playing");
         if (sound->getIsPaused()) return sound->getPlayPosition();
         if (sound->isFinished()) return sound->getPlayLength();
         if ((int)((SDL_GetTicks()-start)*sound->getPlaybackSpeed())<sound->getPlayPosition()    ||   (int)((SDL_GetTicks()-start)*sound->getPlaybackSpeed())>sound->getPlayPosition()+100) {
                for (unsigned int last=sound->getPlayPosition() ; last==sound->getPlayPosition() ; start=(SDL_GetTicks()-(unsigned int)((float)sound->getPlayPosition()/sound->getPlaybackSpeed())));
                }
         return (int)((SDL_GetTicks()-start)*sound->getPlaybackSpeed());
}

void CMusic::stop () {
    engine->setAllSoundsPaused();
}

CDrawer*** CHighway::notes=NULL;
CDrawer*** CHighway::hopos=NULL;
CDrawer** CHighway::presser=NULL;
CDrawer** CHighway::presserp=NULL;

bool CHighway::load() {
    static int i=30;
    if (i<150||(notes==NULL&&hopos==NULL)) {
        if (notes==NULL&&hopos==NULL) {
            i=30;
            notes=new CDrawer**[300];
            for (int i=0;i<300;i++) notes[i]=NULL;
            hopos=new CDrawer**[300];
            for (int i=0;i<300;i++) hopos[i]=NULL;
            }

        notes[i]=new CDrawer*[6];
        notes[i][GREEN]=new CDrawer(FilePath("Image/", "green", ".png"));
        notes[i][GREEN]->resize(i);
        notes[i][RED]=new CDrawer(FilePath("Image/", "red", ".png"));
        notes[i][RED]->resize(i);
        notes[i][YELLOW]=new CDrawer(FilePath("Image/", "yellow", ".png"));
        notes[i][YELLOW]->resize(i);
        notes[i][BLUE]=new CDrawer(FilePath("Image/", "blue", ".png"));
        notes[i][BLUE]->resize(i);
        notes[i][ORANGE]=new CDrawer(FilePath("Image/", "orange", ".png"));
        notes[i][ORANGE]->resize(i);
        notes[i][STARPOWER]=new CDrawer(FilePath("Image/", "spower", ".png"));
        notes[i][STARPOWER]->resize(i);
        
        hopos[i]=new CDrawer*[6];
        hopos[i][GREEN]=new CDrawer(FilePath("Image/", "greenhopo", ".png"));
        hopos[i][GREEN]->resize(i);
        hopos[i][RED]=new CDrawer(FilePath("Image/", "redhopo", ".png"));
        hopos[i][RED]->resize(i);
        hopos[i][YELLOW]=new CDrawer(FilePath("Image/", "yellowhopo", ".png"));
        hopos[i][YELLOW]->resize(i);
        hopos[i][BLUE]=new CDrawer(FilePath("Image/", "bluehopo", ".png"));
        hopos[i][BLUE]->resize(i);
        hopos[i][ORANGE]=new CDrawer(FilePath("Image/", "orangehopo", ".png"));
        hopos[i][ORANGE]->resize(i);
        hopos[i][STARPOWER]=new CDrawer(FilePath("Image/", "spowerhopo", ".png"));
        hopos[i][STARPOWER]->resize(i);

        i++;
        }
}

void CHighway::unload() {
    if (notes!=NULL||hopos!=NULL) {
        if (notes) {
            for (int i=0;i<300;i++) {
                if (notes[i]) {
                    for (int j=0;j<6;j++) {
                        delete notes[i][j];
                        notes[i][j]=NULL;
                        }
                    delete[] notes[i];
                    notes[i]=NULL;
                    }
                }
            delete[] notes;
            notes=NULL;
            }
        if (hopos) {
            for (int i=0;i<300;i++) {
                if (hopos[i]) {
                    for (int j=0;j<6;j++) {
                        delete hopos[i][j];
                        hopos[i][j]=NULL;
                        }
                    delete[] hopos[i];
                    hopos[i]=NULL;
                    }
                }
            delete[] hopos;
            hopos=NULL;
            }
        }
}

CHighway::CHighway (CDrawer *vsl, CMusic* stream, en_instrument instr, en_difficulty diff, int *extras, char frt[]="ZXCVB", char pck[]="", char spk=SDLK_SPACE, int loc=SIZEX/2, int w=SIZEX/3, int h=2*SIZEY/3):
                  visual(vsl), MusicStream(stream), instrument(instr), difficulty(diff), time_delay(1000/(difficulty+1)+1200/(extras[HYPERSPEED]+1)), timing_window(150/(extras[PRECISION]+1)), godmode(extras[GODMODE]), allhopo(extras[ALLHOPO]), practice(extras[PRACTICE]), location(loc), width(w), height(h), basescore(1), starpower(0), progress(0), score(0), streak(0), rockmeter(500)
        {
        if (presser==NULL&&presserp==NULL) {
            presser=new CDrawer*[5];
            presserp=new CDrawer*[5];

            presser[GREEN]=new CDrawer(FilePath("Image/", "greenpresser", ".png"));
            presser[GREEN]->resize(width/5-28);
            presser[RED]=new CDrawer(FilePath("Image/", "redpresser", ".png"));
            presser[RED]->resize(width/5-28);
            presser[YELLOW]=new CDrawer(FilePath("Image/", "yellowpresser", ".png"));
            presser[YELLOW]->resize(width/5-28);
            presser[BLUE]=new CDrawer(FilePath("Image/", "bluepresser", ".png"));
            presser[BLUE]->resize(width/5-28);
            presser[ORANGE]=new CDrawer(FilePath("Image/", "orangepresser", ".png"));
            presser[ORANGE]->resize(width/5-28);

            presserp[GREEN]=new CDrawer(FilePath("Image/", "greenpresserpressed", ".png"));
            presserp[GREEN]->resize(width/5-28);
            presserp[RED]=new CDrawer(FilePath("Image/", "redpresserpressed", ".png"));
            presserp[RED]->resize(width/5-28);
            presserp[YELLOW]=new CDrawer(FilePath("Image/", "yellowpresserpressed", ".png"));
            presserp[YELLOW]->resize(width/5-28);
            presserp[BLUE]=new CDrawer(FilePath("Image/", "bluepresserpressed", ".png"));
            presserp[BLUE]->resize(width/5-28);
            presserp[ORANGE]=new CDrawer(FilePath("Image/", "orangepresserpressed", ".png"));
            presserp[ORANGE]->resize(width/5-28);
            }

        {
            char string[100];
            if (notes==NULL) notes=new CDrawer**[300];
            if (hopos==NULL) hopos=new CDrawer**[300];
            for (int i=(width/5-8)/3-10;i<width/5+10;i++) {
                visual->clear();
                sprintf (string, "Loading... %d per cent", 1+100*(i-((width/5-8)/3-10))/((width/5+10)-((width/5-8)/3-10)));
                visual->textxy(string, SIZEX/2-visual->textwidth(string)/2, 3*SIZEY/4);
                visual->Flip();

                if (notes[i]==NULL) {
                    notes[i]=new CDrawer*[6];
                    notes[i][GREEN]=new CDrawer(FilePath("Image/", "green", ".png"));
                    notes[i][GREEN]->resize(i);
                    notes[i][RED]=new CDrawer(FilePath("Image/", "red", ".png"));
                    notes[i][RED]->resize(i);
                    notes[i][YELLOW]=new CDrawer(FilePath("Image/", "yellow", ".png"));
                    notes[i][YELLOW]->resize(i);
                    notes[i][BLUE]=new CDrawer(FilePath("Image/", "blue", ".png"));
                    notes[i][BLUE]->resize(i);
                    notes[i][ORANGE]=new CDrawer(FilePath("Image/", "orange", ".png"));
                    notes[i][ORANGE]->resize(i);
                    notes[i][STARPOWER]=new CDrawer(FilePath("Image/", "spower", ".png"));
                    notes[i][STARPOWER]->resize(i);
                    }
                if (hopos[i]==NULL) {
                    hopos[i]=new CDrawer*[6];
                    hopos[i][GREEN]=new CDrawer(FilePath("Image/", "greenhopo", ".png"));
                    hopos[i][GREEN]->resize(i);
                    hopos[i][RED]=new CDrawer(FilePath("Image/", "redhopo", ".png"));
                    hopos[i][RED]->resize(i);
                    hopos[i][YELLOW]=new CDrawer(FilePath("Image/", "yellowhopo", ".png"));
                    hopos[i][YELLOW]->resize(i);
                    hopos[i][BLUE]=new CDrawer(FilePath("Image/", "bluehopo", ".png"));
                    hopos[i][BLUE]->resize(i);
                    hopos[i][ORANGE]=new CDrawer(FilePath("Image/", "orangehopo", ".png"));
                    hopos[i][ORANGE]->resize(i);
                    hopos[i][STARPOWER]=new CDrawer(FilePath("Image/", "spowerhopo", ".png"));
                    hopos[i][STARPOWER]->resize(i);
                    }
                }
        }
        rockmart=new CDrawer(FilePath("Image/", "rockmeter", ".png"));
        rockmart->resize(rockmart->get_width(), height-150);

        spbar=new CDrawer(FilePath("Image/", "starpower", ".png"));
        spbar->resize(spbar->get_width(), (height-150)/3);
        spbarfilled=new CDrawer(FilePath("Image/", "starpowerfilled", ".png"));
        spbarfilled->resize(spbarfilled->get_width(), (height-150)/3);

        note_w=(width/5-8);
        note_h=notes[note_w][GREEN]->get_height();

        animation=new CSprite(visual, this);

        FILE *chartfile=NULL;
        int i;
        int col[]={visual->color(40,200,10), visual->color(200, 0, 0), visual->color(247, 236, 40), visual->color(10, 10, 200), visual->color(255, 102, 0)};

        fretstate=new short int[5];
        color=new int[5];
        for (i=0;i<5;i++) color[i]=col[i];
        fret=new char[5];
        for (i=0;i<5;i++) {
            fretstate[i]=0;
            fret[i]=frt[i];
            }

        for (size=0;pck[size];size++);
        pickstate=new short int[size];
        pick=new char[++size];
        for (i=0;i<size;i++) {
            pickstate[i]=0;
            pick[i]=pck[i];
            }

        spkey=spk;

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
        chart=new CNote[(size>50000)?(size=0):(size)];
        for (i=0;i<size;i++) {
            bool cond;
            do {
                chart[i](chartfile);
                if (i==0) break;

                cond=false;
                switch (difficulty) {
                    case EASY:
                        for (int j=4;chart[i].chord>1;j--) {
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
                        for (int j=4;chart[i].chord>2;j--) {
                            if ((chart[i].type&~(1<<j))!=chart[i].type) {
                                chart[i].type=chart[i].type&(~(1<<j));
                                chart[i].chord--;
                                }
                            }
                        if (chart[i].type>=(1<<4)) chart[i].type=chart[i].type>>1;
                        if (chart[i].time-chart[i-1].time<200||(chart[i].chord>1&&chart[i].time-chart[i-1].time<300)) {
                            cond=true;
                            size--;
                        }
                        break;
                    case HARD:
                        if (chart[i].time-chart[i-1].time<100||(chart[i].chord>=2&&chart[i].time-chart[i-1].time<150)) {
                            cond=true;
                            size--;
                            }
                        break;
                    }
                } while (cond&&i<size);
            chart[i].hopo=(allhopo||(instrument!=DRUMS&&i>1&&(chart[i].time-chart[i-1].end)<30000/bpm&&chart[i].type!=chart[i-1].type));
            }
        for (int time=MusicStream->time() ; progress<size&&time-chart[progress].time>timing_window ; progress++);
        start=progress;
        CheckChartIntegrity(chartfile, "End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847");
}

CHighway::~CHighway () {
        delete[] fretstate;
        delete[] fret;
        delete[] pickstate;
        delete[] pick;
        delete[] chart;
        delete[] color;
        if (presser!=NULL&&presserp!=NULL) {
            for (int i=0;i<5;i++) {
                delete presser[i];
                delete presserp[i];
                }
            delete[] presser;
            presser=NULL;
            delete[] presserp;
            presserp=NULL;
            }
        delete rockmart;
        delete spbar;
        delete spbarfilled;
}

void CHighway::reset () {
        delete[] chart;
        
        basescore=1;
        progress=score=streak=0;
        rockmeter=500;
        starpower=0;

        for (int i=0;i<5;i++) fretstate[i]=0;
        for (int i=0;pick[i];i++) pickstate[i]=0;

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
        chart=new CNote[(size>50000)?(size=0):(size)];
        for (int i=0;i<size;i++) {
            bool cond;
            do {
                chart[i](chartfile);
                if (i==0) break;

                cond=false;
                switch (difficulty) {
                    case EASY:
                        for (int j=4;chart[i].chord>1;j--) {
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
                        for (int j=4;chart[i].chord>2;j--) {
                            if ((chart[i].type&~(1<<j))!=chart[i].type) {
                                chart[i].type=chart[i].type&(~(1<<j));
                                chart[i].chord--;
                                }
                            }
                        if (chart[i].type>=(1<<4)) chart[i].type=chart[i].type>>1;
                        if (chart[i].time-chart[i-1].time<200||(chart[i].chord>1&&chart[i].time-chart[i-1].time<300)) {
                            cond=true;
                            size--;
                        }
                        break;
                    case HARD:
                        if (chart[i].time-chart[i-1].time<100||(chart[i].chord>=2&&chart[i].time-chart[i-1].time<150)) {
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

long long int CHighway::refresh (Uint8* keyboard) {
                    int time=MusicStream->time();
                    int i;
                    char fretaux, newfretstate[5], newpickstate[5];
                    bool picked;

                    if (time==~0) return score/10000;

                    for (i=0;i<5;i++) newfretstate[i]=keyboard[fret[i]]!=0;
                    for (i=0;pick[i];i++) newpickstate[i]=keyboard[pick[i]]!=0;

                    if (keyboard[spkey]!=0&&!(starpower%2)&&starpower>50000) {
                        starpower++;
                        MusicStream->starpower();
                        animation->starpower();
                        }

                    while (progress<size&&time-chart[progress].time>timing_window) {
                        if (chart[progress].hit==false) {
                            rockmeter-=20/(starpower%2+1);
                            basescore+=(1000000+(chart[progress].end-chart[progress].time)*bpm)*chart[progress].chord;
                            if (streak||progress==0) MusicStream->hitting(instrument, false);
                            streak=0;
                            }
                        progress++;
                        }
                    if (streak>0) MusicStream->hitting(instrument);

                    for (i=0, fretaux=0;i<5;i++) fretaux+=newfretstate[i]<<i;
                    for (i=0, picked=false;pick[i];i++) if ((newpickstate[i]^pickstate[i])&newpickstate[i]) picked=true;
                    if (!i) for (i=0;i<5;i++) if ((newfretstate[i]^fretstate[i])&newfretstate[i]) picked=true;

                    for (int j=progress;picked&&j<size&&chart[j].time-time<timing_window;j++) {     //notas normais/acordes
                            if (!chart[j].hit) {
                                    if (((chart[j].chord-1)&&((fretaux^(chart[j].type))==0))||
                                        (!(chart[j].chord-1) &&(fretaux^(chart[j].type))<chart[j].type)) {
                                           rockmeter+=(21-rockmeter/50)*(starpower%2*3+1);
                                           starpower+=(multiplier()>1)*2*(((starpower+1)%2)*(2+rockmeter*rockmeter/2500)*(timing_window-abs(chart[j].time-time))*chart[j].chord/timing_window);
                                           chart[j].hit=true;
                                           animation->hit(chart[j].type);
                                           if (chart[j].end > chart[j].time) animation->sustainon(chart[j].type);
                                           MusicStream->hitting(instrument);
                                           basescore+=(1000000+(chart[j].end-chart[j].time)*bpm)*chart[j].chord;
                                           streak++;
                                           score+=(starpower%2+1)*(timing_window-abs(chart[j].time-time))*1100000*chart[j].chord*multiplier()/timing_window;
                                           picked=chart[j].hopo=false;
                                           }
                                    }
                            }

                    for (int j=progress;j<size&&chart[j].time-time<timing_window;j++) {             //sustain
                        if (chart[j].hit&&chart[j].hold) {
                                        if (streak>0&&(((fretaux^(chart[j].type))&(chart[j].type))==0)&&chart[j].time<chart[j].end) {
                                                    score+=(starpower%2+1)*((time-chart[j].time))*chart[j].chord*multiplier()*bpm;
                                                    chart[j].time=time;
                                                    fretaux^=chart[j].type;
                                                }
                                        else {
                                            chart[j].hold=false;
                                            animation->letgo(chart[j].type);
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
                        rockmeter-=20/(starpower%2+1);
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
                                        rockmeter+=(21-rockmeter/50)*(starpower%2*3+1);
                                        starpower+=(multiplier()>1)*2*(((starpower+1)%2)*(2+rockmeter*rockmeter/2500)*(timing_window-abs(chart[j].time-time))*chart[j].chord/timing_window);
                                        chart[j].hit=true;
                                        animation->hit(chart[j].type);
                                        if (chart[j].end > chart[j].time) animation->sustainon(chart[j].type);
                                        MusicStream->hitting(instrument);
                                        basescore+=(1000000+(chart[j].end-chart[j].time)*bpm)*chart[j].chord;
                                        streak++;
                                        score+=(starpower%2+1)*(timing_window-abs(chart[j].time-time))*1100000*chart[j].chord*multiplier()/timing_window;
                                        picked=false;
                                        }
                                    }
                        }
                    if (rockmeter>1000) rockmeter=1000;
                    if (rockmeter<0) rockmeter=0;

                    for (i=0;i<5;i++) fretstate[i]=newfretstate[i];
                    for (i=0;pick[i];i++) pickstate[i]=newpickstate[i];

                    if (starpower%2) {
                        starpower-=(time-lasttime)*bpm/60*2;
                        MusicStream->starpower();
                        }
                    if (starpower<0) {
                        starpower=0;
                        MusicStream->starpower(false);
                        }
                    if (starpower>100000) starpower=100000;

                    draw(time);

                    lasttime=time;
                    return score/10000;
}

void CHighway::draw (int time, Uint8* keyboard, int bottom) {
        bottom=SIZEY-bottom;
        int i, j;

        char fretstate[5];
        for (i=0;i<5;i++) fretstate[i]=this->fretstate[i];

        if (time==-1) time=MusicStream->time();
        if (keyboard!=NULL) {
            for (i=0;i<5;i++) fretstate[i]=keyboard[fret[i]]!=0;
            if (chart[progress].time-time>timing_window) for (i=0;i<5;i++) this->fretstate[i]=fretstate[i];
            }

        visual->line(notex(GREEN, -1000)-5, position3d(-1000)-bottom, notex(GREEN, time_delay)-5, position3d(time_delay)-bottom, visual->color(255, 255, 255, 255));
        visual->line(notex(ORANGE, -1000)+note_width(-1000)+5, position3d(-1000)-bottom, notex(ORANGE, time_delay)+note_width(time_delay)+5, position3d(time_delay)-bottom, visual->color(255, 255, 255, 255));
        for (int i=0;i<5;i++) visual->line(notex(i)+note_width()/2, position3d(0)-bottom, notex(i, time_delay)+note_width(time_delay)/2, position3d(time_delay)-bottom, visual->color(20, 20, 20));
        for (j=time-time%(60*1000/bpm), i=0;j<time+time_delay;j+=(60*1000)/bpm, i++) {
            visual->line(notex(GREEN, j-time)-5, position3d(j-time)-bottom, notex(ORANGE, j-time)+note_width(j-time)+5, position3d(j-time)-bottom, visual->color(40, 40, 40, 255));
            if ((j/(60*1000/bpm))%4==0) {
                visual->line(notex(GREEN, j-time)-5, position3d(j-time)-bottom, notex(ORANGE, j-time)+note_width(j-time)+5, position3d(j-time)-bottom, visual->color(127, 127, 127, 255));
                visual->line(notex(GREEN, j-time)-5, position3d(j-time)+1-bottom, notex(ORANGE, j-time)+note_width(j-time)+5, position3d(j-time)+1-bottom, visual->color(127, 127, 127, 255));
                }
            }
        if (bottom) return;
        for (j=0;j<5;j++) {
            if (fretstate[j]) presserp[j]->apply_surface(notex(j), position3d(0)-bottom, visual);
            else presser[j]->apply_surface(notex(j), position3d(0)-bottom, visual);
            }
        for (j=progress;j>start&&position3d(chart[j].end-time)<visual->get_height();j--);

        while (j<size&&position3d(chart[j].time-time, 0)>position3d(time_delay)-note_h*note_width(time_delay)/note_width(0)) {
            if (chart[j].hit==false||chart[j].end>chart[j].time)
                for (int i=0;i<5;i++)
                    if ((chart[j].type>>i)%2) {
                        if (chart[j].end>chart[j].time) {
                            if (chart[j].hold) {
                                if (starpower%2) visual->parallelogram (notex(i, chart[j].time-time)+note_width(chart[j].time-time)/2-3, position3d(chart[j].time-time)-bottom, notex(i, chart[j].end-time)+note_width(chart[j].end-time)/2-3, position3d(chart[j].end-time)-bottom, 6, visual->color(0, 255, 255));
                                else visual->parallelogram (notex(i, chart[j].time-time)+note_width(chart[j].time-time)/2-3, position3d(chart[j].time-time)-bottom, notex(i, chart[j].end-time)+note_width(chart[j].end-time)/2-3, position3d(chart[j].end-time)-bottom, 6, color[i]);
                                }
                            else visual->parallelogram (notex(i, chart[j].time-time)+note_width(chart[j].time-time)/2-3, position3d(chart[j].time-time)-bottom, notex(i, chart[j].end-time)+note_width(chart[j].end-time)/2-3, position3d(chart[j].end-time)-bottom, 6, visual->color(127, 127, 127));
                            }
                        if (position3d(chart[j].time-time)<visual->get_height()) {
                            if (!chart[j].hit&&!chart[j].hopo) {
                                if (starpower%2) notes[note_width(chart[j].time-time, 0)][STARPOWER]->apply_surface(notex(i, chart[j].time-time, 0), position3d(chart[j].time-time, 0)-bottom, visual, position3d(time_delay)-bottom);
                                else notes[note_width(chart[j].time-time, 0)][i]->apply_surface(notex(i, chart[j].time-time, 0), position3d(chart[j].time-time, 0)-bottom, visual, position3d(time_delay)-bottom);
                                }
                            else if (!chart[j].hit) {
                                if (starpower%2) hopos[note_width(chart[j].time-time, 0)][STARPOWER]->apply_surface(notex(i, chart[j].time-time, 0), position3d(chart[j].time-time, 0)-bottom, visual, position3d(time_delay)-bottom);
                                else hopos[note_width(chart[j].time-time, 0)][i]->apply_surface(notex(i, chart[j].time-time, 0), position3d(chart[j].time-time, 0)-bottom, visual, position3d(time_delay)-bottom);
                                }
                            }
                        }
            j++;
            }
        animation->animate();
        
        if (!practice) {
            char string[50];
            visual->settextstyle("lazy", NULL, 25);
            int posx=notex(GREEN)-visual->textwidth("9999999"), posy=SIZEY-140;
            sprintf (string, "%lld", score/10000);
            visual->textxy (string, posx-5, posy);
            posy+=visual->textheight(string);
            sprintf (string, "%d", streak);
            visual->textxy (string, posx, posy);
            posy+=visual->textheight(string);
            sprintf (string, "x%d", multiplier()*(starpower%2+1));
            visual->textxy(string, posx, posy);
            rockmart->apply_surface(notex(GREEN)-20, SIZEY-150-rockmart->get_height(), visual, SIZEY-150-rockmart->get_height()*rockmeter/1000);
            spbarfilled->apply_surface (notex(ORANGE)+note_width()+10, SIZEY-150-spbarfilled->get_height(), visual, SIZEY-150-spbarfilled->get_height()*starpower/100000);
            spbar->apply_surface (notex(ORANGE)+note_width()+10, SIZEY-150-spbar->get_height(), visual);
            }

}

int CHighway::multiplier () {
    if (streak>30) return 4;
    return (streak/10)+1;
}

float CHighway::percentage() {
    int n=0;
    for (int i=start;i<progress;i++) if (chart[i].hit) n++;
    return (float)100*n/(progress-start);
}

int CHighway::note_width(int dt, bool check) {
    return (note_w-2*(visual->get_height()-position3d(dt, check))*(note_w)/(3*height));
}

int CHighway::notex (int note, int dt, bool check) {
    if (note<GREEN||note>ORANGE) return 0;
    return location-(5*note_width(dt, check)/2)+(note_width(dt, check)*note);
}

bool CHighway::alive() {
    if (!godmode&&!practice&&rockmeter==0) MusicStream->effect("losing");
    return godmode||practice||rockmeter>0;
}

bool CSprite::active;
CSprite::CSprite (CDrawer* screen, CHighway *link): screen(screen), linked(link) {
    if (!active) return;
    char string[100];
    CFrame* aux;

    aux=fire=new CFrame;
    aux->frame=new CDrawer (FilePath("Image/Sprite/", "fire1", ".png"));
    aux->frame->resize(linked->presser[0]->get_width()*3/4);
    aux->frame->setkey(aux->frame->color(0, 0, 0, 255));
    for (int i=2;i<=5;i++) {
        sprintf(string, "fire%d", i);
        aux->next=new CFrame;
        aux=aux->next;
        aux->next=NULL;
        aux->frame=new CDrawer (FilePath("Image/Sprite/", string, ".png"));
        aux->frame->resize(linked->presser[0]->get_width()*3/4);
        aux->frame->setkey(aux->frame->color(0, 0, 0, 255));
        }

    aux=lightning=new CFrame;
    aux->frame=new CDrawer (FilePath("Image/Sprite/", "lightning1", ".png"));
    aux->frame->resize(3*linked->note_width());
    aux->frame->setkey(aux->frame->color(0, 0, 0, 255));
    for (int i=2;i<=2;i++) {
        sprintf(string, "lightning%d", i);
        aux->next=new CFrame;
        aux=aux->next;
        aux->next=NULL;
        aux->frame=new CDrawer (FilePath("Image/Sprite/", string, ".png"));
        aux->frame->resize(3*linked->note_width());
        aux->frame->setkey(aux->frame->color(0, 0, 0, 255));
        }

    aux=blow=new CFrame;
    aux->frame=new CDrawer (FilePath("Image/Sprite/", "sustain1", ".png"));
    aux->frame->resize(linked->presser[0]->get_width()*3/4);
    aux->frame->setkey(aux->frame->color(0, 0, 0, 255));
    aux=blow->next=new CFrame;
    aux->frame=new CDrawer (FilePath("Image/Sprite/", "sustain2", ".png"));
    aux->frame->resize(linked->presser[0]->get_width()*3/4);
    aux->frame->setkey(aux->frame->color(0, 0, 0, 255));
    aux->next=blow;

    notes=new node*[5];
    for (int i=0;i<5;i++) {
        notes[i]=new node;
        notes[i]->current=NULL;
        }
    spower=new node;
    spower->current=NULL;

    sustain=new node*[5];
    for (int i=0;i<5;i++) {
        sustain[i]=new node;
        sustain[i]->current=NULL;
        }
}

CSprite::~CSprite() {
    if (!active) return;
    CFrame* aux;

    aux=fire;
    while (fire!=NULL) {
        aux=fire->next;
        delete fire;
        fire=aux;
        }

    aux=lightning;
    while (lightning!=NULL) {
        aux=lightning->next;
        delete lightning;
        lightning=aux;
        }

    delete[] notes;
    delete[] sustain;
}

void CSprite::animate() {
    if (!active) return;
    for (int i=0;i<5;i++) {
        if (notes[i]->current && SDL_GetTicks() > notes[i]->time+notes[i]->delay) {
            notes[i]->current=notes[i]->current->next;
            notes[i]->time=SDL_GetTicks();
            notes[i]->delay-=5;
            }
        if (sustain[i]->current && SDL_GetTicks() > sustain[i]->time+sustain[i]->delay) {
            sustain[i]->current=sustain[i]->current->next;
            sustain[i]->time=SDL_GetTicks();
            sustain[i]->delay=30;
            }
        }

    if (spower->current && SDL_GetTicks() > spower->time+spower->delay) {
        spower->current=spower->current->next;
        spower->time=SDL_GetTicks();
        spower->delay=50;
        }

    for (int i=0;i<5;i++) {
        if (sustain[i]->current) sustain[i]->current->frame->apply_surface(linked->notex(i)+sustain[i]->current->frame->get_width()/5, linked->position3d(0)+linked->presser[i]->get_height()*3/4-sustain[i]->current->frame->get_height(), screen);
        if (notes[i]->current) notes[i]->current->frame->apply_surface(linked->notex(i)+notes[i]->current->frame->get_width()/5, linked->position3d(0)+linked->presser[i]->get_height()*3/4-notes[i]->current->frame->get_height(), screen);
        }
    if (spower->current) spower->current->frame->apply_surface((linked->notex(2)+linked->notex(3))/2-spower->current->frame->get_width()/2, linked->position3d(0)+linked->note_h-spower->current->frame->get_height(), screen);
}

void CSprite::hit(int fret) {
    if (!active) return;
    for (int i=0;i<5;i++)
        if ((fret>>i)%2) {
            notes[i]->current=fire;
            notes[i]->time=SDL_GetTicks();
            notes[i]->delay=35;
            }
}

void CSprite::sustainon(int fret) {
    if (!active) return;
    for (int i=0;i<5;i++)
        if ((fret>>i)%2) {
            sustain[i]->current=blow;
            sustain[i]->time=SDL_GetTicks();
            sustain[i]->delay=20;
            }
}

void CSprite::letgo(int fret) {
    if (!active) return;
    for (int i=0;i<5;i++)
        if ((fret>>i)%2) sustain[i]->current=NULL;
}

void CSprite::starpower() {
    if (!active) return;
    spower->current=lightning;
    spower->time=SDL_GetTicks();
    spower->delay=50;
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
    SDL_Quit ();
    TTF_Quit ();
    fprintf (stderr, "%s", string);
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
