class menu {
      char *header;
      struct option {
            char *content;
            option *next;
            } *start;
      option *end;
      int selected, nOpt, locx, locy;
      void print();
    public:
      menu (char head[], int x, int y);
      ~menu ();
      bool addOpt (char content[]);
      bool lastopt();
      int opt();    //retorna posiçao escolhida apos navigate()
      char* opts();  //retorna string da opção escolhida
      void navigate();
    };

struct note {
        char type;
        int time;
        int end;
        bool hit;  //1 se ja foi acertada
        bool hold; //1 se for nota longa (0 se nao for, ou se for nota longa e nao tiver sido segurada ate o fim)
        bool hopo; //1 se for hammer-on ou pull-off
        char chord; //numero de notas da nota (se for um acorde vai ter mais de uma nota)
      };

class music {
        irrklang::ISoundEngine *engine;
        irrklang::ISound* sound;
        irrklang::ISoundSource* source, *errorsource[10];
        unsigned int start;
      public:
        irrklang::ISoundEffectControl* FX;
        char *filename, *title, *artist;
        music (FILE* songs, irrklang::ISoundEngine *eng);
        ~music ();
        void load ();
        void unload ();
        bool isInstrumentAvaliable (int instrument);
        bool isFinished ();
        int getPlayLength();
        bool play ();
        void error();
        void lose();
        void starpower(bool active);
        void hitting(bool active);
        bool pause ();
        int time ();
      };

class highway {
      private:
        int location, width, height;
        char *fret, *pick, instrument;
        short int *fretstate, *pickstate, *lastfretstate, *lastpickstate;
        int *color;
        note *chart;
        music *MusicStream;
        int progress, bpm, size;
        int time_delay, timing_window;
        bool godmode;
        long long int basescore, score;
        int streak, rockmeter;
        void draw (int time);
      public:
        highway (music *stream, char instr, int *xtras, char control[], char pck[], int loc, int w, int h, int col[]); //in progress
        ~highway (); //in progress
        int multiplier (); //done
        int preliminary(); //to-do | usa antes de começar a tocar a música
        long long int refresh(); //in progress
        bool alive();
    };
    
class background; //to-do


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
            sprintf (string, "%d", rockmeter);
            outtext(string);
}
/*backup
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
            sprintf (string, "%d  x%d\n", streak, multiplier());
            outtext(string);
}
        }*/
