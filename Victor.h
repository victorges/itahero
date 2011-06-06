highway::highway (char ChartFileName[], char control[]="ZXCVB", char pck[]="", int l=SIZEX/2-175, int r=SIZEX/2+175) {
                FILE *chartfile;
                char string[100];
                int i;
                for (size=0;control[size];size++);
                fret=new char[++size];
                for (i=0;i<size;i++) fret[i]=control[i];
                for (size=0;pck[size];size++);
                pick=new char[++size];
                for (i=0;i<size;i++) pick[i]=pck[i];
                left=l;
                right=r;
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
                progress=0;
                }

highway::~highway () {
                 delete[] fret;
                 delete[] pick;
                 delete[] chart;
                 }
