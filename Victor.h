note::note (char ty, char ti, int e, bool h) {
           type=ty;
           time=ti;
           end=e;
           hit=h;
           }
highway::highway (char ChartFileName[], int tw=50, int hyperspeed=0,char control[]="ZXCVB", char pck[]="", int loc=SIZEX/2, int w=175, int h=2*SIZEX/3) {
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
                if (chartfile==NULL) {
                                     sprintf (string, "Chart File non-existent");
                                     outtextxy(SIZEX/2-50,SIZEY/2, string);
                                     while (kbhit()) getch();
                                     getch();
                                     exit(1);
                                     }
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
                chart=new note[(size>50000)?(size=0):(size)];
                for (i=0;i<size;i++) {
                    fread (&chart[i].type, sizeof (char), 1, chartfile);
                    fread (&chart[i].time, sizeof (int), 1, chartfile);
                    fread (&chart[i].end, sizeof (int), 1, chartfile);
                    chart[i].hit=0;
                    }
                fread (string, sizeof("End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847"), 1, chartfile);
                for (i=0;"End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847"[i]&&string[i]=="End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847"[i];i++);
                if ("End''off/chartnw|enofile|checsotrirnugpted$$33&8!@/ 1@1$ 144847"[i]) {
                                                         sprintf (string, "Chart File corrupted");
                                                         outtextxy(SIZEX/2-50,SIZEY/2, string);
                                                         while (kbhit()) getch();
                                                         getch();
                                                         exit(1);
                                                         }

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
                             pickstate[i]=GetAsyncKeyState(fret[i])!=0;
                             }
                         while (time-chart[progress].end>timing_window) {
                               if (!chart[progress].hit) streak=0;
                               progress++;
                               }
                         for (int j=progress;chart[j].time-time<timing_window;j++) {
                             for (i=0, picked=0;pick[i];i++) if (pickstate[i]&&!lastpickstate[i]) picked=1;
                             if (!pick[0]||picked||chart[j].hit) {
                                for (i=0, fretaux=0, lastfretaux=0;fret[i];i++) {
                                    fretaux+=(fretstate[i]==1)<<i;
                                    lastfretaux+=lastfretstate[i]<<i;
                                    }
                                if (chart[j].hit) {
                                                  if (fretaux^chart[j].type==0) {
                                                                                chart[j].time=time;
                                                                                }
                                                  else chart[j].end=chart[j].time;
                                                  }
                                else if (!pick[0]) {
                                     
              
              return 0;
              }
