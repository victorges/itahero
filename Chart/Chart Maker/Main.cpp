#include <stdio.h>
#include <time.h>
#include <conio.c>
#include "../../src/irrKlang/include/irrKlang.h"

struct note {
       char type;
       unsigned int time;
       unsigned int end;
       };
       
void *AllocateFile (char file_name[], unsigned int &size) {
     FILE *file;
     void *ret;
     file=fopen(file_name, "rb");
     if (file==NULL) return NULL;
     fseek (file, 0, 2);
     size=ftell(file);
     ret=new char[size];
     if (ret==NULL) return NULL;
     fseek (file, 0, 0);
     fread (ret, size, 1, file);
     fclose(file);
     return ret;
}

void strcat (char destiny[], char add[]) {
     int i, j;
     for (i=0;destiny[i]!=0;i++);
     for (j=0;add[j]!=0;j++) destiny[i+j]=add[j];
     destiny[i+j]=0;
}

char location[100];
char *SoundFilePath (char filename[]) {
     location[0]=0;
     strcat (location, "..\\..\\Sound\\");
     strcat (location, filename);
     strcat (location, ".ogg");
     return location;
}
char *ChartPath (char filename[]) {
     location[0]=0;
     strcat (location, "..\\new_");
     strcat (location, filename);
     strcat (location, ".chart");
     return location;
}

void sfscanf (FILE *file, char CheckString[]) {
     int i;
     for (i=0;CheckString[i];i++);
     char string[i];
     for (int j=0;j<i;j++) fscanf (file, "%c", &string[j]);
     for (i=0;CheckString[i]&&string[i]==CheckString[i];i++);
     if (CheckString[i]) printf ("Soundlist file corrupted");
}

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

music::music (FILE *songs) {
          char string[1000];
          int i;
          
          sfscanf (songs, "FILE=");
          fgets(string, 1000, songs);
          for (i=1;string[i-1]!='\n';i++);
          string[i-1]=0;
          filename=new char[i];
          for (i=0;!i||string[i-1];i++) filename[i]=string[i];
          
          sfscanf (songs, "TITLE=");
          fgets(string, 1000, songs);
          for (i=1;string[i-1]!='\n';i++);
          string[i-1]=0;
          title=new char[i];
          for (i=0;!i||string[i-1];i++) title[i]=string[i];
          
          sfscanf (songs, "ARTIST=");
          fgets(string, 1000, songs);
          for (i=1;string[i-1]!='\n';i++);
          string[i-1]=0;
          artist=new char[i];
          for (i=0;!i||string[i-1];i++) artist[i]=string[i];
          
          sound=0;
          start=0;
}

music::~music () {
                delete[] filename;
                delete[] title;
                delete[] artist;
                }

void music::load (irrklang::ISoundEngine* engine) {
     void *soundfile;
     size_t size;
     soundfile=AllocateFile(SoundFilePath(filename), size);
     source=engine->addSoundSourceFromMemory(soundfile, size, title);
     free (soundfile);
     source->setStreamMode(irrklang::ESM_AUTO_DETECT);
     sound=engine->play2D(source, false, true, true, true);
}

void music::unload (irrklang::ISoundEngine* engine) {
     if (sound) {
                if (!sound->isFinished() && !sound->getIsPaused()) sound->stop();
                sound->drop();
                sound=0;
                start=0;
                }
     engine->removeSoundSource(source);
}

bool music::isFinished () {
     return sound->isFinished();
}

bool music::play () {
     if (!sound||!sound->getIsPaused()) return false;
     sound->setIsPaused(false);
     for (unsigned int last=sound->getPlayPosition() ; last==sound->getPlayPosition() ; start=clock()*1000/CLOCKS_PER_SEC-sound->getPlayPosition());
     return true;
}

bool music::pause () {
     if (!sound||sound->getIsPaused()) return false;
     sound->setIsPaused(true);
     start=0;
     return true;
}

int music::time () {
         if (start==0) return ~0;
         return clock()*1000/CLOCKS_PER_SEC-start;
}

int main() {
    using namespace irrklang;
    ISoundEngine* engine=createIrrKlangDevice();
    note chart[5000];
    unsigned int start, time, size;
    int note[5], chartcont=0, bpm, i, chords, sustain, nSongs;
    short int keystate[5], lastkeystate[5];
    char comandos[]="ZXCVB", song;
    char string[50];
    void *SoundFile;
    
    FILE *reader;
    reader=fopen("..\\..\\Sound\\songs.dat", "r");
    if (reader==NULL) printf ("Soundlist file not found");
    sfscanf (reader, "[SONGS=");
    fscanf (reader, "%d", &nSongs);
    sfscanf (reader, "]\n");
    music* songs[nSongs];
    for (int i=0;i<nSongs-1;i++) {
        songs[i]=new music (reader);
        char c;
        sfscanf (reader, "~\n");
        }
    songs[nSongs-1]=new music (reader);
    sfscanf (reader, "[/SONGS]");
    if (fscanf (reader, "%s", string)!=EOF) printf ("Soundlist file corrupted");
    fclose (reader);
    
    printf ("\n\nChoose the song to chart\n");
    for (i=0;i<nSongs;i++) printf ("%d %s - %s\n", i, songs[i]->artist, songs[i]->title);
    while (song<'0'||song>='0'+nSongs) song=getch();
    song-='0';
    printf ("Set bpm: ");
    scanf ("%d", &bpm);
    printf ("Insert time tolerance for chords in milisec: ");
    scanf ("%d", &chords);
    printf ("Insert time tolerance for sustains in milisec: ");
    scanf ("%d", &sustain);
    songs[song]->load(engine);
    for (int j=0;j<5;j++) lastkeystate[j]=keystate[j]=0;
    printf ("\nStarting in\n");
    for (int i=3;Sleep(1000), i>0;i--) printf ("%d\n", i);
    songs[song]->play();
    start=clock()*1000/CLOCKS_PER_SEC-songs[song]->time();
    while (!songs[song]->isFinished()) {
          if (kbhit()) {
                       time=clock()*1000/CLOCKS_PER_SEC-start;
                       for (i=0;i<5;i++) {
                           lastkeystate[i]=keystate[i]!=0;
                           keystate[i]=GetAsyncKeyState(comandos[i])!=0;
                           if (keystate[i]!=lastkeystate[i]) {
                                                             if (keystate[i]) {
                                                                              if (chartcont>0&&time-chart[chartcont-1].time<chords&&chart[chartcont-1].type!=1<<i) {
                                                                                                                       chartcont--;
                                                                                                                       printf ("Chord %d\n\n", chart[chartcont].type+=1<<i);
                                                                                                                       }
                                                                              else printf ("Note %d\nTime %d\n\n", chart[chartcont].type=1<<i, chart[chartcont].time=chart[chartcont].end=time);
                                                                              note[i]=chartcont++;
                                                                              }
                                                             else {
                                                                  if (time-chart[note[i]].time<sustain) chart[note[i]].end=chart[note[i]].time;
                                                                  else printf ("Sustain %d ending at %d\n\n", chart[note[i]].type, chart[note[i]].end=time);
                                                                  }
                                                             }
                           }
                       for (i=0;i<5&&!keystate[i];i++);
                       if (i==5) while (kbhit()) getch();
                       }
          }
    printf ("Saving File");
    FILE *saida, *other;
    saida=fopen (ChartPath(songs[song]->filename), "wb");
    other=fopen ("check.txt", "w");
    fwrite ("Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321", sizeof("Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"), 1, saida);
    fprintf (other, "Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"); 
    fwrite (&bpm, sizeof(int), 1, saida);
    fprintf (other, "\n%d", bpm); 
    fwrite (&chartcont, sizeof(int), 1, saida);
    fprintf (other, "\n%d", chartcont);
    for (i=0;i<chartcont;i++) {
        fwrite (&chart[i].type, sizeof(char), 1, saida);
        fwrite (&chart[i].time, sizeof(unsigned int), 1, saida);
        fwrite (&chart[i].end, sizeof(unsigned int), 1, saida);
        fprintf (other, "\n%d %d %d", chart[i].type, chart[i].time, chart[i].end);
        }
    fwrite ("End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847", sizeof("End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847"), 1, saida);
    fprintf (other, "\nEnd''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847");
    printf ("\nSuccessful. Exiting.");
    fclose (saida);
    return 0;
}
