struct note {
        char type;
        int time;
        int end;
        bool hit;
        bool hold;
      };

class music {
        irrklang::ISound* sound;
        irrklang::ISoundSource* source;
        unsigned int start;
      public:
        char *filename, *title, *artist;
        music (FILE* songs);
        ~music ();
        void load (irrklang::ISoundEngine* engine);
        void unload (irrklang::ISoundEngine* engine);
        bool isFinished ();
        bool play ();
        bool pause ();
        int time ();
      };
      
class highway {
      private:
        int location, width, height;
        char *fret, *pick;
        short int *fretstate, *pickstate, *lastfretstate, *lastpickstate;
        int *color;
        note *chart;
        music *MusicStream;
        int progress, bpm, size;
        int time_delay, timing_window;
      public:
        int score, multiplier, streak;
        highway (music *stream, int tw, int hyperspeed, int col[], char control[], char pck[], int loc, int w, int h);
        ~highway ();
        int refresh(); //in progress

        void highway::draw () { //temporaria, copiada do prototipo
            int time=MusicStream->time();
            int j=progress;
            while (j>0&&time-chart[j].end<time_delay*100/SIZEY) j--;
            for (;chart[j].time-time<time_delay;j++) {
                if (chart[j].hit==0||chart[j].hold==1)
                    for (int i=0;fret[i];i++)
                        if ((chart[j].type>>i)%2) {
                            setfillstyle(SOLID_FILL, color[i]);
                            setcolor(color[i]);
                            line (SIZEX/2+70*i-175+30, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+20), SIZEX/2+70*i-175+30, SIZEY-((chart[j].end-time)*(SIZEY-100)/time_delay+100+20));
                            if (!chart[j].hit) bar(SIZEX/2+70*i-175, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100), SIZEX/2+70*i-175+60, SIZEY-((chart[j].time-time)*(SIZEY-100)/time_delay+100+40));
                            }
                }
            for (j=0;fret[j];j++) {
                setcolor(color[j]);
                rectangle (SIZEX/2+70*j-175, SIZEY-100, SIZEX/2+70*j-115, SIZEY-140);
                }
        }

      };

class background; //to-do
