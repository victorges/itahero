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
              int nnotes;
              for (int i=0;pick[i];i++) {
                  lastpickstate[i]=pickstate[i];
                  pickstate[i]=GetAsyncKeyState(fret[i]);
                  }
              for (nnotes=0;fret[nnotes];nnotes++) {
                  lastfretstate[nnotes]=fretstate[nnotes];
                  fretstate[nnotes]=GetAsyncKeyState(fret[nnotes]);
                  }
              return 0;
              }
