class note {
      public:
        char type;
        int time;
        int end;
        bool hit;
        void PrintNote (); //not yet implemented
      };

class highway {
        note *chart;
        int progress, bpm, size;
        int left, right;
        char *fret, *pick;
      public:
        highway (char ChartFileName[], char control[], char pck[], int l, int r);
        ~highway ();
      };
