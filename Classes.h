class note {
      public:
        char type;
        int time;
        int end;
        bool hit;
        void DrawNote (); //not yet implemented
      };

class highway {
        note *chart;
        int progress, bpm, size;
        int left, right;
        char *fret, *pick;
      public:
        int score, multiplier, streak;
        highway (char ChartFileName[], char control[], char pck[], int l, int r);
        ~highway ();
        int refresh(); //not yet implemented  
        void draw();    //not yet implemented
      };
