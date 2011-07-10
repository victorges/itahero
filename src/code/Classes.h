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
      void setalpha(Uint8 A);
      void setkey(Uint32 color);
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
      void draw (int time=-1, Uint8* keyboard=NULL, int bottom=SIZEY);
      int multiplier ();
      long long int refresh(Uint8* keyboard);
      float percentage();
      bool alive();
    friend class CSprite;
    };

struct CFrame {
            CDrawer* frame;
            CFrame* next;
            };

class CSprite {
    private:
      CFrame* fire;
      CFrame* lightning;
      CFrame* blow;

      struct node {
            CFrame* current;
            int time, delay;
            } **notes;
      node *spower;
      node **sustain;
      CDrawer* screen;
      CHighway* linked;
    public:
      CSprite(CDrawer* screen, CHighway *link);
      ~CSprite();
      void animate();
      void hit(int note);
      void sustainon(int fret);
      void letgo(int fret);
      void starpower();
    };

class CNet {
    private:
      IPaddress ip, *remoteip, clientip;
      TCPsocket sock, newsock;
      char buffer[512];
      const char *hostname;
      Uint16 port;
    public:
      CNet(Uint16 gate = 9999);
      CNet(char *name, Uint16 gate = 9999);
      ~CNet();
      void waitconnection();
      void closeconnection();
      void writemessage(char *message);
      bool readmessage();
    };

CSprite::CSprite (CDrawer* screen, CHighway *link): screen(screen), linked(link) {
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
    for (int i=0;i<5;i++)
        if ((fret>>i)%2) {
            notes[i]->current=fire;
            notes[i]->time=SDL_GetTicks();
            notes[i]->delay=35;
            }
}

void CSprite::sustainon(int fret) {
    for (int i=0;i<5;i++)
        if ((fret>>i)%2) {
            sustain[i]->current=blow;
            sustain[i]->time=SDL_GetTicks();
            sustain[i]->delay=20;
            }
}

void CSprite::letgo(int fret) {
    for (int i=0;i<5;i++)
        if ((fret>>i)%2) sustain[i]->current=NULL;
}

void CSprite::starpower() {
    spower->current=lightning;
    spower->time=SDL_GetTicks();
    spower->delay=50;
}
