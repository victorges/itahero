int min(int a, int b){
    return (a<b) ? a : b;
}

double min(double a, double b){
    return (a<b) ? a : b;
}

int max(int a, int b){
    return (a>b) ? a : b;
}

double max(double a, double b){
    return (a>b) ? a : b;
}

void strcpy(char olds[], char news[]){
    int i = 0;
    while(news[i]!=0){
        olds[i] = news[i];
        i++;
    }
    olds[i] = 0;
}

CNet::~CNet(){
    SDLNet_Quit();
}

CNet::CNet (Uint16 gate): port(gate){
    SDLNet_Init();
    if(SDLNet_ResolveHost(&ip, NULL, port)==-1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(1);
    }
    sock=SDLNet_TCP_Open(&ip);
    if(!sock) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(1);
    }
    hostname = SDLNet_ResolveIP(&ip);
}

CNet::CNet (char *name, Uint16 gate): port(gate), hostname(name){
    if(SDLNet_ResolveHost(&clientip, hostname, port)==-1) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(1);
    }
    newsock = SDLNet_TCP_Open(&clientip);
    if(!newsock) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(1);
    }
}

void CNet::waitconnection(){
    bool quit = false;
    while(!quit)
        if(newsock = SDLNet_TCP_Accept(sock)){
            remoteip = SDLNet_TCP_GetPeerAddress(newsock);
            if(!remoteip) {
                fprintf(stderr, "SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
                fprintf(stderr, "This may be a server socket.\n");
                exit(1);
            }
            quit = true;
        }
}

void CNet::closeconnection(){
    SDLNet_TCP_Close(newsock);
}

bool CNet::readmessage(){
    if (SDLNet_TCP_Recv(newsock, buffer, 512)>0)
        return true;
    return false;
}

void CNet::writemessage(char *message){
    int len;
    sprintf (buffer, message);
    len = strlen(buffer) + 1;
		if(SDLNet_TCP_Send(newsock, (void *)buffer, len) < len){
			fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			exit(1);
		}
}

CDrawer::~CDrawer (){
    SDL_FreeSurface ( surface );
    TTF_CloseFont(font);
}

CDrawer::CDrawer (char filename[]): background(NULL) {
    SDL_Surface *loaded = NULL;
    
    loaded = IMG_Load ( filename );
    if (loaded==NULL) Error ("Can't load image file for game");
    if ( loaded != NULL ){
        surface = SDL_DisplayFormat ( loaded );
        if ( surface != NULL )
            SDL_SetColorKey ( surface,SDL_SRCCOLORKEY, keycolor = SDL_MapRGBA ( surface->format, 0, 255, 255, 255 ) );
    }
    font=NULL;
    settextstyle();
    SDL_FreeSurface ( loaded );
}

Uint32 &CDrawer::get_pixel ( int x, int y ){
    Uint32 *pixels = ( Uint32 * ) surface->pixels;
    
    return pixels [ ( y * surface->w ) + x ];
}

void CDrawer::put_pixel ( int x, int y, Uint32 pixel ){
    if(x>=0 && x<surface->w && y>=0 && y<surface->h)
        get_pixel ( x, y ) = pixel;
}

void CDrawer::apply_surface ( int x, int y, CDrawer *destination, int top, SDL_Rect *clip ){
    SDL_Rect clip2;
    SDL_Rect offset;
    offset.x=x;
    offset.y=y;
    bool changex = false, changey = false;
    if( clip == NULL ){
        if(x<0){
            if(x+surface->w >= 0){
                clip2.x = -x;
                clip2.w = x + surface->w;
                offset.x=0;
                changex = true;
            }
            else
                return;
        }
        if(y<top){
            if(y+surface->h >= top){
                clip2.y = top-y;
                clip2.h = y - top + surface->h;
                offset.y=top;
                changey = true;
            }
            else
                return;
        }
        if(x + surface->w >= destination->surface->w){
            if(x < destination->surface->w )
                clip2.w = changex==true ? destination->surface->w - clip2.x : destination->surface->w - x;
            else
                return;
        }
        if(y + surface->h >= destination->surface->h){
            if(y < destination->surface->h )
                clip2.h = changey==true ? destination->surface->h - clip2.y : destination->surface->h - y;
            else
                return;
        }
        if(changex == true && changey == false){
            clip2.y = 0;
            clip2.h = surface->h;
            changey = true;
        }
        if(changex == false && changey == true){
            clip2.x = 0;
            clip2.w = surface->w;
            changex = true;
        }
    }

    if(changex == false)
        SDL_BlitSurface ( surface, clip, destination->surface, &offset );
    else
        SDL_BlitSurface ( surface, &clip2, destination->surface, &offset );
}

void CDrawer::textxy ( char message[], int x, int y ){
    if (font==NULL) Error ("Font not found");
    CDrawer *text = new CDrawer (TTF_RenderText_Blended ( font, message, textcolor ));
    text->apply_surface ( x, y, this);
    delete text;
}

void CDrawer::line ( int ini_x, int ini_y, int end_x, int end_y, Uint32 color = 0 ){
    if(color == 0) color = this->maincolor;
    if ( end_x != ini_x ){
        if ( end_x < ini_x ){
            end_x ^= ini_x ^= end_x ^= ini_x;
            end_y ^= ini_y ^= end_y ^= ini_y;
        }
        for ( int i = ini_x; i <= end_x; i++ )
            put_pixel ( i, ini_y + ( i - ini_x ) * ( end_y - ini_y ) / ( end_x - ini_x ), color );
        if ( end_y != ini_y ){
            if ( end_y < ini_y ){
                end_x ^= ini_x ^= end_x ^= ini_x;
                end_y ^= ini_y ^= end_y ^= ini_y;
            }
            for ( int i = ini_y; i <= end_y; i++ )
                put_pixel ( ini_x + ( i - ini_y ) * ( end_x - ini_x ) / ( end_y - ini_y ), i, color );
        }
    }
    else{
        if ( end_y < ini_y )
            end_y ^= ini_y ^= end_y ^= ini_y;
        for ( int i = ini_y; i <= end_y; i++)
            put_pixel ( ini_x, i, color );
    }
}

void CDrawer::parallelogram (int x1, int y1, int x2, int y2, int w, Uint32 color){
    if(color == 0) color = this->maincolor;
    for( int i = 0; i<w; i++)
        line(x1+i, y1, x2+i, y2, color);
}

void CDrawer::rectangle ( int left, int top, int right, int bottom, Uint32 color = 0 ){
    if(color == 0) color = this->maincolor;
    line ( left, top, right, top, color );
    line ( right, top, right, bottom, color );
    line ( right, bottom, left, bottom, color );
    line ( left, bottom, left, top, color );
}

void CDrawer::bar ( int left, int top, int right, int bottom, Uint32 color = 0 ){
    if(color == 0) color = maincolor;
    /*for ( int i = left; i <= right; i++ )
        line ( i, top, i, bottom, color );*/
    SDL_Rect rect;
    rect.x=left;
    rect.y=top;
    rect.w=right-left;
    rect.h=bottom-top;
    SDL_FillRect(surface, &rect, color);
}

Uint8 CDrawer::get_pixel_color ( int x, int y, char c ){
    SDL_PixelFormat *fmt;
    Uint32 temp, pixel;
    Uint8 color;
    
    fmt = surface->format;
    SDL_LockSurface ( surface );
    pixel = get_pixel ( x, y );
    SDL_UnlockSurface ( surface );
    
    switch ( c ){
        case 'r':
            temp = pixel & fmt->Rmask;
            temp = temp >> fmt->Rshift;
            temp = temp << fmt->Rloss;
            break;
        case 'g':
            temp = pixel & fmt->Gmask;
            temp = temp >> fmt->Gshift;
            temp = temp << fmt->Gloss;
            break;
        case 'b':
            temp = pixel & fmt->Bmask;
            temp = temp >> fmt->Bshift;
            temp = temp << fmt->Bloss;
            break;
        case 'a':
            temp = pixel & fmt->Amask;
            temp = temp >> fmt->Ashift;
            temp = temp << fmt->Aloss;
            break;
        default:
            exit ( 1 );
            break;
    }
    color = ( Uint8 ) temp;
    
    return color;
}

int CHighway::position3d (int dt, bool check) {
    if (check&&dt>time_delay) dt=time_delay;
    int y = visual->get_height()-(dt*(height-100)/time_delay+140*SIZEY/1080);
    double a = (3*height*(height-y)-(height-y)*(height-y))/(3.0*height);
    if (dt<=time_delay) return (int)(height-a-0.5);
    else {
        if (y>visual->get_height()-3*height/2) return (int)(height-a-0.5);
        else return position3d(time_delay)-note_h*note_width(time_delay)/note_width(0);
        }
}

void CDrawer::check_unlock (){
    if ( SDL_MUSTLOCK ( surface ) )
        SDL_UnlockSurface ( surface );
}

void CDrawer::check_lock (){
    if ( SDL_MUSTLOCK ( surface ) )
        SDL_LockSurface ( surface );
}

char* CDrawer::draw_name(int x, int y){
    SDL_Event event;
    int cnt = 0;
    char *name, aux[2];
    name = (char*)malloc(4*sizeof(char));
    aux[0] = 'A';
    aux[1] = 0;
    int initx=x-textwidth("Initials:e"), inity=y;
    textxy("Initials:", initx ,inity);
    textxy(aux, x, y);
    while(cnt<3) {
        if (SDL_PollEvent(&event)) {
            if (event.type==SDL_KEYDOWN){
                switch (event.key.keysym.sym){
                    case SDLK_UP:
                        bar(x, y, x+textwidth(aux), y+textheight(aux));
                        aux[0] = (aux[0]-'A'+25)%26 + 'A';
                        textxy("Initials:", initx ,inity);
                        textxy(aux, x, y);
                        break;
                    case SDLK_DOWN:
                        bar(x, y, x+textwidth(aux), y+textheight(aux));
                        aux[0] = (aux[0]-'A'+1)%26 + 'A';
                        textxy("Initials:", initx ,inity);
                        textxy(aux, x, y);
                        break;
                    case SDLK_BACKSPACE:
                        if(cnt!=0){
                            bar(x, y, x+textwidth(aux), y+textheight(aux));
                            textxy("Initials:", initx ,inity);
                            cnt--;
                            aux[0] = name[cnt];
                            x = x-textwidth(aux);
                        }
                        break;
                    case SDLK_RETURN:
                        name[cnt] = aux[0];
                        x=x+textwidth(aux);
                        cnt++;
                        aux[0] = 'A';
                        if(cnt!=3) textxy(aux, x, y);
                        textxy("Initials:", initx ,inity);
                        break;
                }
            }
        }
        else if (event.type==SDL_QUIT) exit(0);
        Flip();
    }
    name[3] = 0;
    return name;
}

void CMusic::check_record_file(int nplayers){
    FILE *record = NULL;
    char aux[2], musicname[20];
    strcpy(musicname, filename);
    sprintf(aux, "%d", nplayers);
    strcat(musicname, aux);
    record = fopen(FilePath("SAVEDATA/", musicname, ".rec"), "rb");
    if(record != NULL){
        fclose(record);
        return;
    }
    fclose(record);
    record = fopen(FilePath("SAVEDATA/", musicname, ".rec"), "wb");
    char string[] = "AAA";
    long long int pto = 0;
    for(int i=0; i<10; i++){
        fwrite(string, sizeof(char), 3, record);
        fwrite(&pto, sizeof(long long int), 1, record);
    }
    fclose(record);
}

void CMusic::include_record(char *name, long long int score, int nplayers){
    char nome[10][4], aux[2], musicname[20];
    strcpy(musicname, filename);
    long long int pto[10];
    int i;
    sprintf(aux, "%d", nplayers);
    strcat(musicname, aux);
    FILE *record = fopen(FilePath("SAVEDATA/", musicname, ".rec"), "rb");
    for(int i = 0; i<10; i++){
        fread(nome[i], sizeof(char), 3, record);
        nome[i][3]=0;
        fread(&pto[i], sizeof(long long int), 1, record);
    }
    fclose(record);
    for(i = 0; i<10; i++)
        if(score > pto[i]){
            for(int j = 9; j>i; j--){
                nome[j][0] = nome[j-1][0];
                nome[j][1] = nome[j-1][1];
                nome[j][2] = nome[j-1][2];
                pto[j] = pto[j-1];
            }
            strcpy(nome[i], name);
            pto[i] = score;
            i=10;
        }
    record = fopen(FilePath("SAVEDATA/", musicname, ".rec"), "wb");
    for(int i=0; i<10; i++){
        fwrite(nome[i], sizeof(char), 3, record);
        fwrite(&pto[i], sizeof(long long int), 1, record);
    }
    fclose(record);
}

void CDrawer::draw_highscore(int x, int y, char *filename, int nplayers){
    char nome[10][4], string[20], aux[2], musicname[20];
    strcpy(musicname, filename);
    long long int pto[10];
    sprintf(aux, "%d", nplayers);
    strcat(musicname, aux);
    FILE *record = fopen(FilePath("SAVEDATA/", musicname, ".rec"), "rb");
    for(int i = 0; i<10; i++){
        fread(nome[i], sizeof(char), 3, record);
        nome[i][3]=0;
        fread(&pto[i], sizeof(long long int), 1, record);
    }
    fclose(record);
    bar(0, 0, SIZEX, SIZEY);
    textxy("   HIGHSCORE", x, y);
    int h=textheight("HIGHSCORE");
    for(int i = 0; i<10; i++){
        sprintf(string, "%s %lld", nome[i], pto[i]);
        textxy(string, x, y+(i+2)*h);
    }
    Flip();
    SDL_Event event;
    bool quit = false;
    while(!quit){
        SDL_PollEvent(&event);
        if (event.type==SDL_KEYDOWN){
             if (event.key.keysym.sym == SDLK_RETURN){
                    quit = true;
                    break;
            }
        }
        else if (event.type==SDL_QUIT) exit(0);
    }
}

double Lanczos(double x, int Radius){
  if (x == 0.0) return 1.0;
  if (x <= -Radius || x >= Radius) return 0.0;
  double tp = x * PI;
  return Radius * sin(tp) * sin(tp / Radius) / (tp * tp);
}

void CDrawer::resize(int new_w, int new_h)
{
    SDL_Surface * destination;
    
    if(new_h == 0)
        new_h = new_w*surface->h/surface->w;

    if (surface->w == new_w && surface->h == new_h){
        destination = SDL_DisplayFormat(surface);
        SDL_FreeSurface(surface);
        surface = destination;
        return;
    }

    Uint32 rmask = 0x000000ff,
        gmask = 0x0000ff00,
        bmask = 0x00ff0000,
        amask = 0xff000000;

    destination = SDL_CreateRGBSurface(SDL_HWSURFACE, new_w, new_h, surface->format->BitsPerPixel, rmask, gmask, bmask, amask );
    SDL_Surface * temp = SDL_ConvertSurface(surface,destination->format,0);
    SDL_FreeSurface(surface);
    surface = temp;

    const double blur = 1.0;
    double factor  = destination->w / (double)surface->w;
    double scale   = min(factor, 1.0) / blur;
    int FilterRadius;
    FilterRadius = 3;
    if (scale < 0.67) FilterRadius = 2;
    if (scale <= 0.5) FilterRadius = 1;        
    double support = FilterRadius / scale; 

    double *contribution_x;
    contribution_x = (double*)malloc(min((int)(size_t)surface->w, 5+(size_t)(2*support))*sizeof(double));

    Uint32 ** tmp = new Uint32 * [surface->h];
    for (int i = 0 ; i < surface->h; i++)
        tmp[i] = new Uint32 [destination->w];

    if (support <= 0.5){
        support = 0.5 + 1E-12;
        scale = 1.0;
    }

    for (int x = 0; x < destination->w; ++x){
        double center = (x + 0.5) / factor;
        size_t start = (size_t)max(center - support + 0.5, (double)0);
        size_t stop  = (size_t)min(center + support + 0.5, (double)surface->w);
        double density = 0.0;
        size_t nmax = stop - start;
        double s = start - center + 0.5;
        double point[4] = {0,0,0,0};
        Uint8 v;
        double diff;

        for (int y = 0; y < surface->h; y++){                  
            for (size_t n = 0; n < nmax; ++n){
                if (y == 0){
                    contribution_x[n] = Lanczos (s * scale, FilterRadius);                
                    density += contribution_x[n];
                    s++;
                }
                Uint8 * p = (Uint8 *)surface->pixels + y * surface->pitch + (start+n) * 4;
                for (int c = 0; c < 4; c++)
                    point[c] += p[c] * contribution_x[n];
            }
            Uint8 * p = (Uint8 *)&tmp[y][x];
            for (size_t c = 0; c < 4; c++){
                if (density != 0.0 && density != 1.0)
                    point[c] /= density;
                if (point[c] < 0)
                    point[c] = 0;
                if (point[c] > 255)
                    point[c] = 255;
	            v = (Uint8) point[c];
	            diff = point[c] - (double)v;
	            if (diff < 0)
		            diff = -diff;
	            if (diff >= 0.5)
                    v++;
	            p[c] = v;
                point[c] = 0;
            }
        }
    }

    factor  = destination->h / (double)surface->h;
    scale   = min(factor, 1.0) / blur;
    FilterRadius = 3;
    if (scale < 0.67) FilterRadius = 2;
    if (scale <= 0.5) FilterRadius = 1;
    support = FilterRadius / scale;

    double *contribution_y;
    contribution_y = (double*)malloc(min((int)(size_t)surface->h, 5+(size_t)(2*support))*sizeof(double));

    if (support <= 0.5){
        support = 0.5 + 1E-12;
        scale = 1.0;
    }

    for (int y = 0; y<destination->h; ++y){
        double center = (y + 0.5) / factor;
        size_t start = (size_t)max(center - support + 0.5, (double)0);
        size_t stop  = (size_t)min(center + support + 0.5, (double)surface->h);
        double density = 0.0;
        size_t nmax = stop-start;
        double s = start - center+0.5;
        double point[4] = {0,0,0,0};
        Uint8 v;
        double diff;
        
        for (int x=0; x<destination->w; x++){
            for (size_t n=0; n<nmax; ++n){
                if (x == 0){
                    contribution_y[n] = Lanczos(s * scale, FilterRadius);
                    density += contribution_y[n];
                    s++;
                }
                Uint8 * p = (Uint8 *)&tmp[start+n][x];
                for (int c = 0; c < 4; c++)
                    point[c] += p[c] * contribution_y[n];
            }
            Uint8 * p = (Uint8 *)destination->pixels + y * destination->pitch + x * 4;
            for (size_t c = 0; c < 4; c++){
                if (density != 0.0 && density != 1.0)
                    point[c] /= density;
                if (point[c] < 0)
                    point[c] = 0;
                if (point[c] > 255)
                    point[c] = 255;
                v = (Uint8) point[c];
	            diff = point[c] - (double)v;
	            if (diff < 0)
		            diff = -diff;
	            if (diff >= 0.5)
			        v++;
	            p[c] = v;
                point[c] = 0;
            }
        }
    }

    for (int i = 0 ; i < surface->h; i++)
        delete [] tmp[i];
    delete [] tmp;

    SDL_FreeSurface(temp);
    temp = SDL_DisplayFormat(destination);            

    SDL_FreeSurface(destination);
    surface = temp;
}
