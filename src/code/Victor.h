note::note (char ty, char ti, int e, bool h) {
           type=ty;
           time=ti;
           end=e;
           hit=h;
           }
note::note () {
           }

highway::highway (char ChartFileName[], int tw=50, int hyperspeed=0,char control[]="ZXCVB", char pck[]="", int loc=SIZEX/2, int w=175, int h=2*SIZEY/3) {
                FILE *chartfile;
                char string[100];
                int i;
                
                location=loc;
                width=w;
                height=h;
                timing_window=tw;
                time_delay=300+1200/(hyperspeed+1);

                for (size=0;control[size];size++);
                fretstate=new short int[size];
                lastfretstate=new short int[size];
                fret=new char[++size];
                for (i=0;i<size;i++) fret[i]=control[i];
                
                for (size=0;pck[size];size++);
                pickstate=new short int[size];
                lastpickstate=new short int[size];
                pick=new char[++size];
                for (i=0;i<size;i++) pick[i]=pck[i];

                chartfile=fopen(ChartPath(ChartFileName), "rb");
                
                if (chartfile==NULL) Error ("Chart File non-existent");
                if (CheckIntegrity(chartfile, "Chrt.fle-chck|fr_corrupt%%4&$32@&*  5%%^ 1123581321")==0) Error ("Chart File corrupted");
                
                fread (&bpm, sizeof(int), 1, chartfile);
                fread (&size, sizeof(int), 1, chartfile);
                chart=new note[(size>50000)?(size=0):(size)];
                for (i=0;i<size;i++) {
                    fread (&chart[i].type, sizeof (char), 1, chartfile);
                    fread (&chart[i].time, sizeof (int), 1, chartfile);
                    fread (&chart[i].end, sizeof (int), 1, chartfile);
                    chart[i].hit=0;
                    }

                if (CheckIntegrity(chartfile, "End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847")==0) Error ("Chart File corrupted");

                progress=score=streak=0;
                multiplier=1;
                }

highway::~highway () {
                  delete[] fretstate;
                  delete[] lastfretstate;
                  delete[] fret;
                  delete[] pickstate;
                  delete[] lastpickstate;
                  delete[] pick;
                  delete[] chart;
                  }
                 
int highway::refresh(int time) {
                         int i, picked;
                         char fretaux, lastfretaux;
                         for (i=0;fret[i];i++) {
                             lastfretstate[i]=fretstate[i]!=0;
                             fretstate[i]=GetAsyncKeyState(fret[i])!=0;
                             }
                         for (i=0;pick[i];i++) {
                             lastpickstate[i]=pickstate[i]!=0;
                             pickstate[i]=GetAsyncKeyState(pick[i])!=0;
                             }
                         while (time-chart[progress].end>timing_window) {
                               if (chart[progress].hit==false) streak=0;
                               progress++;
                               }
                         for (int j=progress;chart[j].time-time<timing_window&&j<size;j++) {
                             for (i=0, picked=0;pick[i];i++) if (pickstate[i]&&!lastpickstate[i]) picked=1;
                             if (chart[j].hit==true) { //sustain
                                               for (i=0, fretaux=0;fret[i];i++) fretaux+=(fretstate[i]!=0)<<i;
                                               if (fretaux^chart[j].type==0) {
                                                                             chart[j].time=time; //pontua��o?
                                                                             }
                                               else chart[j].time=chart[j].end;
                                               }
                             else if (!pick[0]||picked) {
                                for (i=0, fretaux=0, lastfretaux=0;fret[i];i++) {
                                    fretaux+=(fretstate[i]==1)<<i;
                                    lastfretaux+=lastfretstate[i]<<i;
                                    }
                                if (pick[0]==0) {
                                     if ((lastfretaux^fretaux!=0)&&(fretaux^chart[j].type==0)) {
                                                                                              chart[j].hit=true;
                                                                                              for (i=0;fret[i];i++) fretstate[i]=2*(fretstate[i]!=0);
                                                                                              }
                                     }
                                else if (picked) {
                                     if (fretaux^chart[j].type==0) {
                                                                   chart[j].hit=1;
                                                                   for (i=0;!pickstate[i]||lastpickstate[i];i++);
                                                                   lastpickstate[i]=1;
                                                                   }
                                     }
                                }
                             }
                         return 0;
}

void *AllocateFile (char file_name[], size_t &size) {
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

void Error (char string[]) {
     outtextxy(SIZEX/2-50,SIZEY/2, string);
     while (kbhit()) getch();
     getch();
     exit(1);
}

bool CheckIntegrity (FILE *chartfile, char CheckString[]) {
     int i;
     for (i=1;CheckString[i-1];i++);
     char string[i];
     fread (string, sizeof(char), i, chartfile);
     for (i=0;CheckString[i]&&string[i]==CheckString[i];i++);
     return !CheckString[i];
}
