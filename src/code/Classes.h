struct CNote {
      char type;
      int time;
      int end;
      bool hit;  //1 se ja foi acertada
      bool hold; //1 se for nota longa (0 se nao for, ou se for nota longa e nao tiver sido segurada ate o fim)
      bool hopo; //1 se for hammer-on ou pull-off
      char chord; //numero de notas da nota (se for um acorde vai ter mais de uma nota)
      void operator()(FILE *chartfile); //le do arquivo
      };

class CDrawer {
    private:
      Uint32 maincolor;
      Uint32 keycolor;
      SDL_Surface *surface;
      CDrawer *background;
      TTF_Font *font;
      SDL_Color textcolor;
      int textsize;
      Uint32 &get_pixel ( int x, int y);
      void check_unlock ();
      void check_lock ();
   public:
      CDrawer (SDL_Surface *surface); //cria CDrawer para uma surface já pronta
      CDrawer (int width, int height, int bpp, Uint32 flags); //cria surface para ser a tela
      CDrawer (Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask); //cria surface qualquer
      CDrawer (char filename[]);
      ~CDrawer ();

      void load_background(CDrawer *background);

      void settextstyle (char font[]="lazy", SDL_Color *textcolor=NULL, int textsize=15);
      int textheight(char string[]);
      int textwidth(char string[]);
      void textxy ( char message[], int x, int y );

      char *draw_name(int x, int y);
      void draw_highscore(int x, int y, char *filename, int nplayers);

      void resize ( int new_w, int new_h = 0 );

      Uint8 get_pixel_color (int x, int y, char c);
      int get_height();
      int get_width();
        
      void put_pixel (int x, int y, Uint32 pixel);
      void apply_surface (int x, int y, CDrawer *destination, int top=0, SDL_Rect *clip=NULL);
      void line (int ini_x, int ini_y, int end_x, int end_y, Uint32 color);
      void rectangle (int left, int top, int right, int bottom, Uint32 color);
      void bar (int left, int top, int right, int bottom, Uint32 color);
      void parallelogram (int x1, int y1, int x2, int y2, int w, Uint32 color);
        
      Uint32 color (Uint8 R, Uint8 G, Uint8 B, Uint8 A=255);
      void setcolor (Uint8 R, Uint8 G, Uint8 B, Uint8 A=255);
      void Flip();
      void clear();
};

class CMenu {
    private:
      char *header;
      static irrklang::ISoundEngine *engine;
      struct option {
            char *content;
            option *next;
            } *start, *end;
      int selected, nOpt, locx, locy, sizex, sizey;
      CDrawer *visual;
    public:
      static void loadfx (irrklang::ISoundEngine *engine);
      static void unloadfx ();
      static void effect (char name[]);
      CMenu (CDrawer *vsl, char head[], int x, int y);
      ~CMenu ();
      void addOpt (char content[]);
      bool cancel();
      void setopt(int sel);
      int opt();    //retorna posiçao escolhida apos navigate()
      char* opts();  //retorna string da opção escolhida
      bool navigate();
      void print();
    };

class CMusic {
    private:
      irrklang::ISoundEngine *engine;
      irrklang::ISound* sound;
      irrklang::ISoundSource* source, *errorsource[NERROR];
      irrklang::ISoundEffectControl* FX;
      unsigned int start, section, limit;
    public:
      char *title, *artist, *filename;
      CMusic (FILE* songs, irrklang::ISoundEngine *eng);
      ~CMusic ();
      void load (float speed, int starts, int ends);
      void reload ();
      void unload ();
      bool isInstrumentAvaliable (en_instrument instrument);
      bool isFinished ();
      void preview (bool active, int sixteenth);
      bool play (float volume=1.0);
      void include_record(char *name, long long int score, int nplayers);
      bool pause ();
      int time ();
      void stop();

      void settime(int time);
      void settimerel(int dt);
      float speed();

      void effect(char string[]);
      void error();
        
        
      void starpower(bool active);
      void hitting(char instrument, bool active);
      void check_record_file(int nplayes);
      };
class CSprite;
class CHighway {
    private:
      int width, height;
      char *fret, *pick, spkey, instrument;
      short int *fretstate, *pickstate;
      int *color;
      CNote *chart;
      CMusic *MusicStream;
      en_difficulty difficulty;
      int start, progress, bpm, size;
      int time_delay, timing_window;
      long long int basescore;
      bool allhopo, godmode;
      int streak, rockmeter, starpower;
      CDrawer *visual, *hway;
      int note_w, note_h;
      int note_width(int dt=0, bool check=1);
      int position3d (int dt, bool check=1);
      int notex (int note, int dt=0, bool check=1);
      static CDrawer ***notes, ***hopos, **presser, **presserp;
      CDrawer *rockmart, *spbar, *spbarfilled;
      CSprite* animation;
      int lasttime;
    public:
      bool practice;
      int location;
      int get_height() {return height;}
      long long int score;
      static bool load();
      static void unload();
      CHighway (CDrawer *vsl, CMusic *MusicStream, en_instrument instrument, en_difficulty difficulty, int *extras, char *fret, char *pick, char spkey, int location, int width, int height);
      ~CHighway ();
      void reset();
      void draw (int time, Uint8* keyboard, int bottom);
      int multiplier ();
      long long int refresh(Uint8* keyboard);
      float percentage();
      bool alive();
    friend class CSprite;
    };

class CFrame {
          private:
            CDrawer* frame;
            CFrame* next;
          public:
            CFrame (char filename[]);
            CFrame* create_next(char filename[]);
            CFrame* get_next();
            void draw(int posx, int posy);
          };

class CSprite {
    private:
      CFrame* fire;
      CFrame* lightning;

      CFrame** notes;
      CFrame* spower;
      CDrawer* screen;
      CHighway* linked;
    public:
      CSprite(CDrawer* screen, CHighway *link);
      ~CSprite();
      void refresh();
      void hit(int note);
      void starpower();
    };

CSprite::CSprite (CDrawer* screen, CHighway *link): screen(screen), linked(link) {
    char string[100];
    CFrame* aux;

    aux=fire=new CFrame(FilePath("Images/Sprites/", "fire1", ".png"));
    for (int i=2;i<=5;i++) {
        sprintf(string, "fire%d", i);
        aux=aux->create_next(FilePath("Images/Sprites/", string, ".png"));
        }

    aux=lightning=new CFrame(FilePath("Images/Sprites/", "lightning1", ".png"));
    for (int i=2;i<=2;i++) {
        sprintf(string, "lightninge%d", i);
        aux=aux->create_next(FilePath("Images/Sprites/", string, ".png"));
        }

    notes=new CFrame*[5];
    for (int i=0;i<5;i++) notes[i]=NULL;

    spower=NULL;
}

CSprite::~CSprite() {
    CFrame* aux;

    aux=fire;
    while (fire!=NULL) {
        aux=fire->get_next();
        delete fire;
        fire=aux;
        }

    aux=lightning;
    while (lightning!=NULL) {
        aux=lightning->get_next();
        delete lightning;
        lightning=aux;
        }

    delete[] notes;
}

void CSprite::hit(int fret) {
    notes[fret]=fire;
}

void CSprite::starpower() {
    spower=lightning;
}

CFrame::CFrame (char filename[]) {
    frame=new CDrawer (filename);
    next=NULL;
}

CFrame* CFrame::create_next(char filename[]) {
    return next=new CFrame(filename);
}

CFrame* CFrame::get_next() {
    return next;
}
