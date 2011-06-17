class note {
      public:
        char type;
        int time;
        int end;
        bool hit;
        note (char ty, char ti, int e, bool h);
        note ();
        bool DrawNote (int time, ...); //not yet implemented (smair)
      };

class music {
        irrklang::ISound* sound;
        irrklang::ISoundSource* source;
        unsigned int start;
      public:
        char *filename, *title, *artist;
        music (FILE* songs);
        music (char flnm[], char ttl[], char artst[]);
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
        note *chart;
        music *MusicStream;
        int progress, bpm, size;
        int time_delay, timing_window;
      public:
        int score, multiplier, streak;
        highway (music *stream, int tw, int hyperspeed, char control[], char pck[], int loc, int w, int h);
        ~highway ();
        int refresh(); //in progress
        void draw(...);    //not yet implemented
      };

class background; //to-do
