class note {
      public:
        char type;
        int time;
        int end;
        bool hit;
        void DrawNote (); //not yet implemented
      };

class highway {
        int location, width, height;
        char *fret, *pick;
        short int *fretstate, *pickstate, *lastfretstate, *lastpickstate;
        note *chart;
        int progress, bpm, size;
        int time_delay, timing_window;
      public:
        int score, multiplier, streak;
        highway (char ChartFileName[], int tw, int hyperspeed, char control[], char pck[], int loc, int w, int h);
        ~highway ();
        int refresh(int time); //in progress
        void draw(int time);    //not yet implemented
      };

class background; //to-do
