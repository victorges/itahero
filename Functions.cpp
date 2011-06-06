#include <stdio.h>

char __location[100];
void *AllocateFile (char file_name[]) {
     size_t size;
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
