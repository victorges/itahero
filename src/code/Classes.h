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
        Uint32 &get_pixel ( int x, int y);
        Uint32 maincolor;
        Uint32 keycolor;
        void check_unlock ();
        void check_lock ();
        SDL_Surface* load_image (char *name);
        void apply_surface (int x, int y,SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip);
     public:
        SDL_Surface *surface;
        drawer (SDL_Surface *surface); //cria drawer para uma surface j� pronta
        drawer (int width, int height, int bpp, Uint32 flags); //cria surface para ser a tela
        drawer (Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask); //cria surface qualquer
        drawer (char filename[]);
        ~drawer ();
        drawer* resize ( int ini_w, int ini_h, int end_w, int end_h );
        Uint8 get_pixel_color (int x, int y, char c);
        void put_pixel (int x, int y, Uint32 pixel);
        void line (int ini_x, int ini_y, int end_x, int end_y, Uint32 color);
        void rectangle (int left, int top, int right, int bottom, Uint32 color);
        void bar (int left, int top, int right, int bottom, Uint32 color);
        void setcolor (Uint8 R, Uint8 G, Uint8 B, Uint8 A);
        void setcolor (Uint32 color);
        void Flip();
        void clear();
    friend class highway;
};

class menu {
    private:
      char *header;
      struct option {
            char *content;
            option *next;
            } *start, *end;
      int selected, nOpt, locx, locy, sizex, sizey;
      void print();
    public:
      menu (char head[], int x, int y);
      ~menu ();
      void addOpt (char content[]);
      bool cancel();
      int opt();    //retorna posi�ao escolhida apos navigate()
      char* opts();  //retorna string da op��o escolhida
      bool navigate();
    friend class drawer;
    };

class music {
      private:
        irrklang::ISoundEngine *engine;
        irrklang::ISound* sound;
        irrklang::ISoundSource* source, *errorsource[NERROR];
        unsigned int start, section, limit;
        char *filename;
      public:
        irrklang::ISoundEffectControl* FX;
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
        void draw (int time);
      public:
        highway (SDL_Surface *visual, music *MusicStream, en_instrument instrument, en_difficulty difficulty, int *extras, char *fret, char *pick, int location, int width, int height, int *color); //in progress
        ~highway (); //in progress
        void reset();
        int multiplier (); //done
        int preliminary(); //to-do | usa antes de come�ar a tocar a m�sica
        long long int refresh(Uint8* keyboard); //in progress
        bool alive();
    };

class background; //to-do

void menu::print () { //temporaria
    option *aux=start;
    setfillstyle(SOLID_FILL, BLACK);
    bar (locx-10, locy-10, locx+sizex+10, locy+sizey+10);
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

void highway::draw (int time) { //temporaria, copiada do prototipo (pode editar a vontade, mas mant�m o backup)
            int j=progress;

            setcolor (WHITE);
            line (location-180, SIZEY, location-180, SIZEY-SIZEY);
            visual->line( location-180, SIZEY-1, location-180, 0, SDL_MapRGBA (visual->surface->format, 255, 255, 255, 255));
            line (location+170, SIZEY, location+170, 0);
            visual->line( location+170, SIZEY-1, location+170, 0, SDL_MapRGBA(visual->surface->format, 255, 255, 255, 255));
            setcolor (DARKGRAY);
            for (j=(time/(60*1000)/bpm)*(60*1000)/bpm;j<time+time_delay;j+=(60*1000)/bpm) {
                line (location-180, SIZEY-((j-time)*(SIZEY-100)/time_delay+100), location+170, SIZEY-((j-time)*(SIZEY-100)/time_delay+100));
                visual->line(location-180, SIZEY-((j-time)*(SIZEY-100)/time_delay+100), location+170, SIZEY-((j-time)*(SIZEY-100)/time_delay+100), SDL_MapRGBA(visual->surface->format,RED_VALUE(DARKGRAY), GREEN_VALUE(DARKGRAY), BLUE_VALUE(DARKGRAY), 255));
                }
            for (j=0;j<5;j++) {
                setcolor(color[j]);
                rectangle (location+70*j-175, SIZEY-100, location+70*j-115, SIZEY-140);
                visual->rectangle ( location+70*j-175, SIZEY-100, location+70*j-115, SIZEY-140, SDL_MapRGBA(visual->surface->format,GetRValue(color[j]), GetGValue(color[j]), GetBValue(color[j]), 255));
                if (fretstate[j]) {
                    setfillstyle(SOLID_FILL, COLOR (GetRValue(color[j])<40?0:(GetRValue(color[j])-40), GetGValue(color[j])<40?0:(GetGValue(color[j])-40), GetBValue(color[j])<40?0:(GetBValue(color[j])-40)));
                    bar (location+70*j-173, SIZEY-101, location+70*j-116, SIZEY-138);
                    visual->bar (location+70*j-173, SIZEY-101, location+70*j-116, SIZEY-138, SDL_MapRGBA(visual->surface->format,GetRValue(color[j]), GetGValue(color[j]), GetBValue(color[j]), 255));
                    }
                }
            for (j=progress;j>0&&time-chart[j].end<time_delay*100/SIZEY;j--);
            for (;chart[j].time-time<time_delay;j++) {
                if (chart[j].hit==0||chart[j].end>chart[j].time)
                    for (int i=0;i<5;i++)
                        if ((chart[j].type>>i)%2) {
                            if (chart[j].end>chart[j].time) {
                                if (!chart[j].hold) setfillstyle (SOLID_FILL, LIGHTGRAY);
                                else setfillstyle(SOLID_FILL, color[i]);
                                bar (location+70*i-175+25, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+20), location+70*i-175+35, SIZEY-((chart[j].end-time)*(SIZEY-100)/time_delay+100+20));
                                visual->bar (location+70*i-175+25, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+20), location+70*i-175+35, SIZEY-((chart[j].end-time)*(SIZEY-100)/time_delay+100+20), SDL_MapRGBA(visual->surface->format,GetRValue(color[i]), GetGValue(color[i]), GetBValue(color[i]), 255));
                                }
                            setfillstyle(SOLID_FILL, color[i]);
                            if (!chart[j].hit&&!chart[j].hopo) {
                                bar(location+70*i-175, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100), location+70*i-175+60, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+40));
                                visual->bar ( location+70*i-175, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100), location+70*i-175+60, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+40), SDL_MapRGBA(visual->surface->format, GetRValue(color[i]), GetGValue(color[i]), GetBValue(color[i]), 255));
                                }
                            if (!chart[j].hit&&chart[j].hopo) {
                                bar(location+70*i-165, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100), location+70*i-165+50, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+40));
                                visual->bar (location+70*i-165, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100), location+70*i-165+50, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+40), SDL_MapRGBA(visual->surface->format, GetRValue(color[i]), GetGValue(color[i]), GetBValue(color[i]), 255));
                                }
                            }
                }
            char string[50];
            setcolor (WHITE);
            if (!practice) {
                sprintf (string, "%.1f  %d", (float)score/basescore, score/10000);
                moveto (location-300, SIZEY-140);
                outtext(string);
                moverel(-textwidth(string), textheight(string));
                sprintf (string, "%d  x%d", streak, multiplier());
                outtext(string);
                moverel(-textwidth(string), textheight(string));
                sprintf (string, "%d", rockmeter);
                outtext(string);
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

void highway::draw (int time) { //temporaria, copiada do prototipo (pode editar a vontade, mas mant�m o backup)
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
