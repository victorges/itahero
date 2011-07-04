
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "SDL_net.h"
#include "chat.h"

/* This is easy enough.  we setup all the network stuff, then create net_thread
 which waits for connections or other events and deals with them. In the meantime
 the main loop deals with drawing the screen and waiting for the user to press
 'esc' to quit the server. When we quit, we kill our thread and quit the program*/

using namespace std;

SDL_Surface *screen,*back;
bool finished = false;

SDL_Thread *net_thread=NULL;
static TCPsocket servsock = NULL;
static SDLNet_SocketSet socketset = NULL;
static struct {
	int active;
	TCPsocket sock;
	IPaddress peer;
	Uint8 name[256+1];
} people[GAME_MAXPEOPLE];

//************IMAGE STUFF****************
SDL_Surface * ImageLoad(char *file)
{
  SDL_Surface *temp1, *temp2;
  temp1 = SDL_LoadBMP(file);
  temp2 = SDL_DisplayFormat(temp1);
  SDL_FreeSurface(temp1);
  return temp2;
}
// Blit an image on the screen surface
void DrawIMG(SDL_Surface *img, int x, int y)
{
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  SDL_BlitSurface(img, NULL, screen, &dest);
}
//******************************************

void findInactivePersonSlot(int &which){
    /* Look for inactive person slot */
	for ( which=0; which<GAME_MAXPEOPLE; ++which ) {
	    if ( people[which].sock && ! people[which].active ) {
	        /* Kick them out.. */
	        unsigned char data = GAME_BYE;
	        SDLNet_TCP_Send(people[which].sock, &data, 1);
	        SDLNet_TCP_DelSocket(socketset, people[which].sock);
	        SDLNet_TCP_Close(people[which].sock);
            #ifdef DEBUG
            fprintf(stderr, "Killed inactive socket %d\n", which);
            #endif
	        break;
        }
	}
}

void roomFull(TCPsocket newsock){
     /* No more room... */
     char data = GAME_BYE;
     
     SDLNet_TCP_Send(newsock, &data, 1);
     SDLNet_TCP_Close(newsock);
     
     #ifdef DEBUG
     fprintf(stderr, "Connection refused -- chat room full\n");
     #endif
}

void addInactiveSocket(int which, TCPsocket newsock){
     /* Add socket as an inactive person */
     people[which].sock = newsock;
     people[which].peer = *SDLNet_TCP_GetPeerAddress(newsock);
     SDLNet_TCP_AddSocket(socketset, people[which].sock);
     #ifdef DEBUG
     fprintf(stderr, "New inactive socket %d\n", which);
     #endif
}

void HandleServer(void)
{
	int which;
	TCPsocket newsock;
	
     newsock = SDLNet_TCP_Accept(servsock);
	if ( newsock == NULL ) {
		return;
	}

	/* Look for unconnected person slot */
	for ( which=0; which<GAME_MAXPEOPLE; ++which ) {
		if ( ! people[which].sock ) {
			break;
		}
	}
	if ( which == GAME_MAXPEOPLE ) {
		findInactivePersonSlot(which);
	}
	if ( which == GAME_MAXPEOPLE ) {
		roomFull(newsock);
	} else {
		addInactiveSocket(which, newsock);
	}
}

/* Send a "new client" notification */
void SendNew(int about, int to)
{
	char data[512];
	int n;

	n = strlen((char *)people[about].name)+1;
	data[0] = GAME_ADD;
	data[GAME_ADD_SLOT] = about;
	memcpy(&data[GAME_ADD_HOST], &people[about].peer.host, 4);
	memcpy(&data[GAME_ADD_PORT], &people[about].peer.port, 2);
	data[GAME_ADD_NLEN] = n;
	memcpy(&data[GAME_ADD_NAME], people[about].name, n); //if more info, add it here on next line ie appearance/colour
	SDLNet_TCP_Send(people[to].sock, data, GAME_ADD_NAME+n);
}

void SendID(int which){
    cout << "Lets try sending this! " <<which<<endl;
    char data[512];
    data[0] = GAME_ID;
    data[1] = which;
    SDLNet_TCP_Send(people[which].sock,data,GAME_ID_LEN+1);
}

void notifyAllConnectionClosed(char data[], int which){
    /* Notify all active clients */
    #ifdef DEBUG
	    fprintf(stderr, "Closing socket %d (was%s active)\n",
			           which, people[which].active ? "" : " not");
        #endif
    if ( people[which].active ) {
		people[which].active = 0;
		data[0] = GAME_DEL;
		data[GAME_DEL_SLOT] = which;
		for (int i=0; i<GAME_MAXPEOPLE; ++i ) {
			if ( people[i].active ) {
				SDLNet_TCP_Send(people[i].sock,data,GAME_DEL_LEN);
			}
		}
    }
}

void deleteConnection(int which){
     SDLNet_TCP_DelSocket(socketset, people[which].sock);
     SDLNet_TCP_Close(people[which].sock);
     people[which].sock = NULL;
}

void HandleClient(int which)
{
	char data[512];

	/* Has the connection been closed? */
	if ( SDLNet_TCP_Recv(people[which].sock, data, 512) <= 0 ) {
		notifyAllConnectionClosed(data, which);
		deleteConnection(which);
		
	} else {
		switch (data[0]) {
			case GAME_HELLO: {
				/* Yay!  An active connection */
				memcpy(&people[which].peer.port,
						&data[GAME_HELLO_PORT], 2);
				memcpy(people[which].name,
						&data[GAME_HELLO_NAME], 256);
				people[which].name[256] = 0;
				
                #ifdef DEBUG
                       fprintf(stderr, "Activating socket %d (%s)\n",
				       which, people[which].name);
                #endif
                
				/* Notify all active clients */
				for (int i=0; i<GAME_MAXPEOPLE; ++i ) {
					if ( people[i].active ) {
						SendNew(which, i);
					}
				}

				/* Notify about all active clients */
				people[which].active = 1;
				for (int i=0; i<GAME_MAXPEOPLE; ++i ) {
					if ( people[i].active ) {
						SendNew(i, which);
					}
				}
				
				/*Tell player which one in the slot they are */
				SendID(which);
			}
			break;
			default: {
				/* Unknown packet type?? */;
			}
			break;
		}
	}
}

void checkIfFinished(){
    // Check if we have some interesting events...
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        // Has a key been pressed down?
        if(event.type == SDL_KEYDOWN)
        {
            // If it was escape then quit
            if(event.key.keysym.sym == SDLK_ESCAPE)
            {
                //we are quiting the server now
                finished = 1;
            }
        }
    }
}

static void cleanup(int exitcode)
{
	if ( servsock != NULL ) {
		SDLNet_TCP_Close(servsock);
		servsock = NULL;
	}
	if ( socketset != NULL ) {
		SDLNet_FreeSocketSet(socketset);
		socketset = NULL;
	}
	SDLNet_Quit();
	SDL_Quit();
	cout<<"Exitcode "<<exitcode<<endl;
	exit(exitcode);
}

void initSDL(){
     /* Initialize SDL */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
            fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
            exit(1);
	}

	/* Set a 640x480 video mode*/
	screen = SDL_SetVideoMode(640, 480, 0, SDL_SWSURFACE);
	if ( screen == NULL ) {
        fprintf(stderr, "Couldn't set video mode: %s\n",SDL_GetError());
		SDL_Quit();
        exit(1);
	}
}

void initSDLNet(){
     /* Initialize the network */
	if ( SDLNet_Init() < 0 ) {
		fprintf(stderr, "Couldn't initialize net: %s\n",
						SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}
}

void initChannels(){
     /* Initialize the channels */
	for (int i=0; i<GAME_MAXPEOPLE; ++i ) {
		people[i].active = 0;
		people[i].sock = NULL;
	}
}

void allocateSockets(){
     /* Allocate the socket set */
	socketset = SDLNet_AllocSocketSet(GAME_MAXPEOPLE+1);
	if ( socketset == NULL ) {
		fprintf(stderr, "Couldn't create socket set: %s\n",
						SDLNet_GetError());
		cleanup(2);
	}
}

void createServerSocket(){
     IPaddress serverIP;
     
     /* Create the server socket */
	SDLNet_ResolveHost(&serverIP, NULL, GAME_PORT);
    printf("Server IP: %x, %d\n", serverIP.host, serverIP.port);
	servsock = SDLNet_TCP_Open(&serverIP);
	if ( servsock == NULL ) {
		fprintf(stderr, "Couldn't create server socket: %s\n",
						SDLNet_GetError());
		cleanup(2);
	}
	SDLNet_TCP_AddSocket(socketset, servsock);
}

void initServer(){
    initSDL();
    initSDLNet(); 
	initChannels();
    allocateSockets();
    createServerSocket();
}

// the network input loop
int net_thread_main(void *data)
{
    while(!finished){
        
        /* Wait for events */
        /*This function here is why we need a thread if we are doing keyboard 
         input and drawing to the screen. This function waits until something
         interesting happens. So it will sit at this line until a user connects 
         or something */
		SDLNet_CheckSockets(socketset, ~0); 

		/* Check for new connections */
		if ( SDLNet_SocketReady(servsock) ) {
			HandleServer();
		}

		/* Check for events on existing clients */
		for (int i=0; i<GAME_MAXPEOPLE; ++i ) {
			if ( SDLNet_SocketReady(people[i].sock) ) {
				HandleClient(i);
			}
		}
    }
    return(0);
}

int main(int argc, char *argv[])
{
	initServer();
	cout<<"INIT'd"<<endl;
	
	//create net_thread to deal with network events
	net_thread=SDL_CreateThread(net_thread_main,servsock);
	if(!net_thread)
	{
		printf("SDL_CreateThread: %s\n",SDL_GetError());
		cleanup(9);
	}
	
	 // We also load in all the graphics...
    back = ImageLoad("data/bg.bmp");
    
	/* Loop, waiting for network events */
	while ( !finished ) {
        //for clean exiting
		checkIfFinished();
		if(finished){
            //kill net_thread as it may be stuck waiting for events
            cout <<"Attempting to kill"<<endl;
            SDL_KillThread(net_thread);
        }
		//draw background
	    DrawIMG(back, 0,0);
	   // Filp the screen - if you dont, you wont see anything!
        SDL_Flip(screen);
        
		
	}
	//make sure this thread is dead, before cleanup
	cout<<"Waiting for thread to be dead"<<endl;
	SDL_WaitThread(net_thread,NULL);
	cleanup(0);

	return 0;
}

