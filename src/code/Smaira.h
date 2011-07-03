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

drawer::~drawer (){
    SDL_FreeSurface ( surface );
    TTF_CloseFont(font);
}

drawer::drawer (char filename[]){
    SDL_Surface *loaded = NULL;
    
    loaded = IMG_Load ( filename );
    if ( loaded != NULL ){
        surface = SDL_DisplayFormat ( loaded );
        if ( surface != NULL )
            SDL_SetColorKey ( surface,SDL_SRCCOLORKEY, keycolor = SDL_MapRGBA ( surface->format, 0, 255, 255, 255 ) );
    }
    font=NULL;
    settextstyle();
    SDL_FreeSurface ( loaded );
}

Uint32 &drawer::get_pixel ( int x, int y ){
    Uint32 *pixels = ( Uint32 * ) surface->pixels;
    
    return pixels [ ( y * surface->w ) + x ];
}

void drawer::put_pixel ( int x, int y, Uint32 pixel ){
    if(x>=0 && x<surface->w && y>=0 && y<surface->h)
        get_pixel ( x, y ) = pixel;
}

void drawer::apply_surface ( int x, int y, drawer *destination, SDL_Rect *clip = NULL ){
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
        if(y<0){
            if(y+surface->h >= 0){
                clip2.y = -y;
                clip2.h = y + surface->h;
                offset.y=0;
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

void drawer::textxy ( char message[], int x, int y ){
    if (font==NULL) Error ("Font not found");
    drawer *text = new drawer (TTF_RenderText_Solid ( font, message, textcolor ));
    text->apply_surface ( x, y, this);
    delete text;
}

void drawer::line ( int ini_x, int ini_y, int end_x, int end_y, Uint32 color = 0 ){
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

void drawer::parallelogram (int x1, int y1, int x2, int y2, int w, Uint32 color){
    if(color == 0) color = this->maincolor;
    for( int i = 0; i<w; i++)
        line(x1+i, y1, x2+i, y2, color);
}

void drawer::rectangle ( int left, int top, int right, int bottom, Uint32 color = 0 ){
    if(color == 0) color = this->maincolor;
    line ( left, top, right, top, color );
    line ( right, top, right, bottom, color );
    line ( right, bottom, left, bottom, color );
    line ( left, bottom, left, top, color );
}

void drawer::bar ( int left, int top, int right, int bottom, Uint32 color = 0 ){
    if(color == 0) color = this->maincolor;
    for ( int i = left; i <= right; i++ )
        line ( i, top, i, bottom, color );
}

Uint8 drawer::get_pixel_color ( int x, int y, char c ){
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

void drawer::check_unlock (){
    if ( SDL_MUSTLOCK ( surface ) )
        SDL_UnlockSurface ( surface );
}

void drawer::check_lock (){
    if ( SDL_MUSTLOCK ( surface ) )
        SDL_LockSurface ( surface );
}

double Lanczos(double x, int Radius){
  if (x == 0.0) return 1.0;
  if (x <= -Radius || x >= Radius) return 0.0;
  double tp = x * PI;
  return Radius * sin(tp) * sin(tp / Radius) / (tp * tp);
}

void drawer::resize(int new_w, int new_h)
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
