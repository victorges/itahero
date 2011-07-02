struct note {
        char type;
        int time;
        int end;
        bool hit;  //1 se ja foi acertada
        bool hold; //1 se for nota longa (0 se nao for, ou se for nota longa e nao tiver sido segurada ate o fim)
        bool hopo; //1 se for hammer-on ou pull-off
        char chord; //numero de notas da nota (se for um acorde vai ter mais de uma nota)
        void operator()(FILE *chartfile); //le do arquivo
      };

class drawer {
     private:
        Uint32 maincolor;
        Uint32 keycolor;
        SDL_Surface *surface;
        TTF_Font *font;
        SDL_Color textcolor;
        int textsize;
        Uint32 &get_pixel ( int x, int y);
        void check_unlock ();
        void check_lock ();
     public:
        drawer (SDL_Surface *surface); //cria drawer para uma surface já pronta
        drawer (int width, int height, int bpp, Uint32 flags); //cria surface para ser a tela
        drawer (Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask); //cria surface qualquer
        drawer (char filename[]);
        ~drawer ();
        
        void settextstyle (char font[], SDL_Color *textcolor, int textsize);
        int textheight(char string[]);
        int textwidth(char string[]);
        void textxy ( char message[], int x, int y ); 
        
        drawer* resize ( int ini_w, int ini_h, int end_w, int end_h );

        Uint8 get_pixel_color (int x, int y, char c);
        void put_pixel (int x, int y, Uint32 pixel);
        void apply_surface (int x, int y,drawer *destination, SDL_Rect *clip);
        void line (int ini_x, int ini_y, int end_x, int end_y, Uint32 color);
        void rectangle (int left, int top, int right, int bottom, Uint32 color);
        void bar (int left, int top, int right, int bottom, Uint32 color);
        
        Uint32 color (Uint8 R, Uint8 G, Uint8 B, Uint8 A);
        void setcolor (Uint8 R, Uint8 G, Uint8 B, Uint8 A);
        void Flip();
        void clear();
};

class menu {
    private:
      char *header;
      struct option {
            char *content;
            option *next;
            } *start, *end;
      int selected, nOpt, locx, locy, sizex, sizey;
      drawer *visual;
      void print();
    public:
      menu (drawer *vsl, char head[], int x, int y);
      ~menu ();
      void addOpt (char content[]);
      bool cancel();
      int opt();    //retorna posiçao escolhida apos navigate()
      char* opts();  //retorna string da opção escolhida
      bool navigate();
    };

class music {
      private:
        irrklang::ISoundEngine *engine;
        irrklang::ISound* sound;
        irrklang::ISoundSource* source, *errorsource[NERROR];
        irrklang::ISoundEffectControl* FX;
        unsigned int start, section, limit;
        char *filename;
      public:
        char *title, *artist;
        music (FILE* songs, irrklang::ISoundEngine *eng);
        ~music ();
        void load (float speed, int starts, int ends);
        void reload ();
        void unload ();
        bool isInstrumentAvaliable (en_instrument instrument);
        bool isFinished ();
        void preview (bool active, int sixteenth);
        bool play ();
        void error();
        void lose();
        void starpower(bool active);
        void hitting(char instrument, bool active);
        bool pause ();
        int time ();
      friend class highway;
      };

class highway {
      private:
        int location, width, height;
        char *fret, *pick, instrument;
        short int *fretstate, *pickstate;
        int *color;
        note *chart;
        music *MusicStream;
        int progress, bpm, size;
        int time_delay, timing_window;
        bool allhopo, godmode, practice;
        long long int basescore, score;
        int streak, rockmeter;
        drawer *visual;
        drawer **notes, **hopos, *art;
        void draw (int time);
      public:
        highway (drawer *vsl, music *MusicStream, en_instrument instrument, en_difficulty difficulty, int *extras, char *fret, char *pick, int location, int width, int height, int *color);
        ~highway ();
        void reset();
        int multiplier ();
        int preliminary(); //to-do | usa antes de começar a tocar a música
        long long int refresh(Uint8* keyboard);
        bool alive();
    };

class background; //to-do

void menu::print () {
    option *aux=start;
    visual->bar(locx-sizex/2-10, locy-sizey/2-10, locx+sizex/2+10, locy+sizey/2+10, 0);
    visual->textxy(header, locx-sizex/2, locy-sizey/2);
    
    int y=visual->textheight(header);
    for (int curr=0;aux;curr++) {
        char string[100]="";
        if (selected==curr) {
            strcat(string, " ");
            }
        strcat (string, aux->content);
        visual->textxy(string, locx-sizex/2, locy-sizey/2+y);
        y+=visual->textheight(aux->content);
        aux=aux->next;
        }
}

void highway::draw (int time) { //temporaria, copiada do prototipo (pode editar a vontade, mas mantém o backup)
            int j=progress;

            notes[0]->apply_surface (0, 0, visual, NULL);
            visual->line( location-180, SIZEY-1, location-180, 0, visual->color(255, 255, 255, 255));
            visual->line( location+170, SIZEY-1, location+170, 0, visual->color(255, 255, 255, 255));
            for (j=(time/(60*1000)/bpm)*(60*1000)/bpm;j<time+time_delay;j+=(60*1000)/bpm) {
                visual->line(location-180, SIZEY-((j-time)*(SIZEY-100)/time_delay+100), location+170, SIZEY-((j-time)*(SIZEY-100)/time_delay+100), visual->color(127, 127, 127, 255));
                }
            for (j=0;j<5;j++) {
                visual->rectangle ( location+70*j-175, SIZEY-100, location+70*j-115, SIZEY-140, color[j]);
                if (fretstate[j]) {
                    visual->bar (location+70*j-173, SIZEY-101, location+70*j-116, SIZEY-138, color[j]);
                    }
                }
            for (j=progress;j>0&&time-chart[j].end<time_delay*100/SIZEY;j--);
            for (;chart[j].time-time<time_dela




            y;j++) {
                if (chart[j].hit==0||chart[j].end>chart[j].time)
                    for (int i=0;i<5;i++)
                        if ((chart[j].type>>i)%2) {
                            if (chart[j].end>chart[j].time) {
                                visual->bar (location+70*i-175+25, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+20), location+70*i-175+35, SIZEY-((chart[j].end-time)*(SIZEY-100)/time_delay+100+20),color[i]);
                                }
                            if (!chart[j].hit&&!chart[j].hopo) {
                                visual->bar ( location+70*i-175, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100), location+70*i-175+60, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+40), color[i]);
                                }
                            if (!chart[j].hit&&chart[j].hopo) {
                                visual->bar (location+70*i-165, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100), location+70*i-165+50, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+40), color[i]);
                                }
                            }
                }
            char string[50];
            if (!practice) {
                int posx=location-300, posy=SIZEY-140;
                visual->settextstyle("lazy", NULL, 20);
                sprintf (string, "%lld,%lld  %lld", score/basescore%10, (10*score/basescore%10), score/10000);
                visual->textxy (string, posx-5, posy);
                posy+=visual->textheight(string);
                sprintf (string, "%d  x%d", streak, multiplier());
                visual->textxy (string, posx, posy);
                posy+=visual->textheight(string);
                sprintf (string, "%d", rockmeter);
                visual->textxy (string, posx, posy);
                }
}
/*backup
void menu::print () { //temporaria
    option *aux=start;
    moveto(locx,locy);
    outtext(header);
    int y=textheight(header);
    for (int curr=0;aux;curr++) {
        moveto (locx, locy+y);
        if (selected==curr) outtext(" ");
        outtext (aux->content);
        y+=textheight(aux->content);
        aux=aux->next;
        }
}

void highway::draw (int time) { //temporaria, copiada do prototipo (pode editar a vontade, mas mantém o backup)
            int j=progress;

            setcolor (WHITE);
            line (location-180, SIZEY-0, location-180, SIZEY-SIZEY);
            line (location+170, SIZEY-0, location+170, SIZEY-SIZEY);
            setcolor (DARKGRAY);
            for (j=(time/(60*1000)/bpm)*(60*1000)/bpm;j<time+time_delay;j+=(60*1000)/bpm) line (location-180, SIZEY-((j-time)*(SIZEY-100)/time_delay+100), location+170, SIZEY-((j-time)*(SIZEY-100)/time_delay+100));

            for (j=0;fret[j];j++) {
                setcolor(color[j]);
                rectangle (location+70*j-175, SIZEY-100, location+70*j-115, SIZEY-140);
                if (fretstate[j]) {
                    setfillstyle(SOLID_FILL, COLOR (GetRValue(color[j])<40?0:(GetRValue(color[j])-40), GetGValue(color[j])<40?0:(GetGValue(color[j])-40), GetBValue(color[j])<40?0:(GetBValue(color[j])-40)));
                    bar (location+70*j-173, SIZEY-101, location+70*j-116, SIZEY-138);
                    }
                }
            for (j=progress;j>0&&time-chart[j].end<time_delay*100/SIZEY;j--);
            for (;chart[j].time-time<time_delay;j++) {
                if (chart[j].hit==0||chart[j].end>chart[j].time)
                    for (int i=0;fret[i];i++)
                        if ((chart[j].type>>i)%2) {
                            if (chart[j].end>chart[j].time) {
                                if (!chart[j].hold) setfillstyle (SOLID_FILL, LIGHTGRAY);
                                else setfillstyle(SOLID_FILL, color[i]);
                                bar (location+70*i-175+25, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+20), location+70*i-175+35, SIZEY-((chart[j].end-time)*(SIZEY-100)/time_delay+100+20));
                                }
                            setfillstyle(SOLID_FILL, color[i]);
                            if (!chart[j].hit&&!chart[j].hopo) bar(location+70*i-175, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100), location+70*i-175+60, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+40));
                            if (!chart[j].hit&&chart[j].hopo) bar(location+70*i-165, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100), location+70*i-165+50, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+40));
                            }
                }
            char string[50];
            setcolor (WHITE);
            sprintf (string, "%.1f  %d", (float)score/basescore, score/10000);
            moveto (location-300, SIZEY-140);
            outtext(string);
            moverel(-textwidth(string), textheight(string));
            sprintf (string, "%d  x%d", streak, multiplier());
            outtext(string);
            moverel(-textwidth(string), textheight(string));
            if (!practice) {
                sprintf (string, "%d", rockmeter);
                outtext(string);
                }
}*/
