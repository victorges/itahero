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

char location[100];
char *SoundFilePath (char filename[]) {
     location[0]=0;
     strcat (location, "Sound\\");
     strcat (location, filename);
     strcat (location, ".wav");
     return location;
}
char *ChartPath (char filename[]) {
     location[0]=0;
     strcat (location, "Chart\\");
     strcat (location, filename);
     strcat (location, ".chart");
     return location;
}

class highway {
      struct note {
             char type;
             int time;
             int end;
             bool hit;
             } chart[5000];
      int progress, bpm;
      int left, right;
      char control[6], pick[3];
      public:
        highway (char ChartFileName[], int l=SIZEX/2-175, int r=SIZEX/2+175, char ctrl[]="ZXCVB", char pck[]=0) {
                FILE *chartfile;
                char string[100];
                int i, size;
                for (i=0;ctrl[i];i++) control[i]=ctrl[i];
                for (i=0;pck[i];i++) pick[i]=pck[i];
                left=l;
                right=r;
                chartfile=fopen(ChartPath(ChartFileName), "rb");
                fread (string, sizeof("Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"), 1, chartfile);
                for (i=0;"Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"[i]&&string[i]=="Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"[i];i++);
                if ("Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321"[i]) {
                                                         sprintf (string, "Chart File corrupted");
                                                         outtextxy(0,0, string);
                                                         while (kbhit()) getch();
                                                         getch();
                                                         exit(1);
                                                         }
                fread (&bpm, sizeof(int), 1, chartfile);
                fread (&size, sizeof(int), 1, chartfile);
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
