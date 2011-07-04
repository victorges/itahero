#include <SDL/SDL.h>
#include <SDL/SDL_Net.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    FILE *ff = fopen("debugclient.txt", "w");
    FILE *in = fopen("entrada.txt", "r");
    
    IPaddress remoteip;
    TCPsocket newsock;
    bool quit = false;
    char buffer[512];
    int len;
    
    if(SDLNet_ResolveHost(&remoteip,"LucasSmaira",9999)==-1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(1);
    }
    newsock = SDLNet_TCP_Open(&remoteip);
    if(!newsock) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }
    
	//while (!quit){
    for(int i = 0; i<10; i++){
		fprintf(ff, "Write something:\n>");
		//fscanf(in, " %s", buffer);
        if(i==9) sprintf(buffer, "quit");
        else sprintf(buffer, "%d ", i);
        

		len = strlen(buffer) + 1;
		if(SDLNet_TCP_Send(newsock, (void *)buffer, len) < len){
			fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			exit(1);
		}

		if(strcmp(buffer, "exit") == 0)
			quit = true;
		if(strcmp(buffer, "quit") == 0)
			quit = true;
	}

	SDLNet_TCP_Close(newsock);

    fclose(ff);
    fclose(in);
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
