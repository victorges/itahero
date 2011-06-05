#include <graphics.h>
#include <windows.h>

#define SIZEX 1366
#define SIZEY 700

void *AllocateFile (char file_name[]) {
     unsigned int size;
     FILE *file;
     void *ret;
     file=fopen(file_name, "rb");
     if (file==NULL) return NULL;
     fseek (file, 0, 2);
     size=ftell(file);
     ret=malloc(size);
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

char __location[100];
char *SoundFilePath (char filename[]) {
     __location[0]=0;
     strcat (__location, "Sound\\");
     strcat (__location, filename);
     strcat (__location, ".wav");
     return __location;
}
char *ChartPath (char filename[]) {
     __location[0]=0;
     strcat (__location, "Chart\\");
     strcat (__location, filename);
     strcat (__location, ".chart");
     return __location;
}

class highway {
        struct note {
               char type;
               int time;
               int end;
               bool hit;
               } *chart;
        int progress, bpm, size;
        int left, right;
        char *fret, *pick;
      public:
        highway (char ChartFileName[], char control[]="ZXCVB", char pck[]="", int l=SIZEX/2-175, int r=SIZEX/2+175) {
                FILE *chartfile;
                char string[100];
                int i;
                for (size=0;control[size];size++);
                fret=(char*)malloc(sizeof(char)*(++size));
                for (i=0;i<size;i++) fret[i]=control[i];
                for (size=0;pck[size];size++);
                pick=(char*)malloc(sizeof(char)*(++size));
                for (i=0;i<size;i++) pick[i]=pck[i];
                left=l;
                right=r;
                chartfile=fopen(ChartPath(ChartFileName), "rb");
                fread (string, sizeof("Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"), 1, chartfile);
                for (i=0;"Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"[i]&&string[i]=="Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"[i];i++);
                if ("Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"[i]) {
                                                         sprintf (string, "Chart File corrupted");
                                                         outtextxy(SIZEX/2-50,SIZEY/2, string);
                                                         while (kbhit()) getch();
                                                         getch();
                                                         exit(1);
                                                         }
                fread (&bpm, sizeof(int), 1, chartfile);
                fread (&size, sizeof(int), 1, chartfile);
                chart=(note*)malloc(sizeof(note)*size);
                for (i=0;i<size;i++) {
                    fread (&chart[i].type, sizeof (char), 1, chartfile);
                    fread (&chart[i].time, sizeof (int), 1, chartfile);
                    fread (&chart[i].end, sizeof (int), 1, chartfile);
                    chart[i].hit=0;
                    }
                progress=0;
                }
      };

int main () {
    initwindow(SIZEX, SIZEY, "ITA Hero");
    closegraph();
    return 0;
}
