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
    }
    else{
        if ( end_y < ini_y )
            end_y ^= ini_y ^= end_y ^= ini_y;
        for ( int i = ini_y; i <= end_y; i++)
            put_pixel ( ini_x, i, color );
    }
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

void drawer::resize ( int ini_w, int ini_h, int end_w, int end_h ){
    int add_w, add_h;
    drawer *resized = new drawer( SDL_HWSURFACE, end_w, ini_h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, 255 );
    drawer *resized2 = new drawer( SDL_HWSURFACE, end_w, ini_h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, 255 );;
    
    add_w = add_h = 0;
    check_lock ();
    
    if ( end_w == ini_w )
        for ( int i = 0; i < ini_h; i++ )
            for ( int j = 0; j < ini_w; j++ )
                resized->put_pixel ( j, i, get_pixel ( j, i ) );
    else if ( end_w > ini_w ){
        if ( end_w > 2 * ini_w ){
            add_w = end_w - 2 * ini_w;
            end_w = 2 * ini_w;
        }
        for ( int i = 0, actpass; i < ini_h; i++ ){
            actpass = ini_w / ( end_w - ini_w );
            for ( int j = 0, count = 0, column = 0; j < end_w; j++ ){
                if ( count == actpass ){
                    Uint8 red, green, blue, alpha;
                    red = ( Uint8 ) ( ( ( Uint32 ) get_pixel_color ( column, i, 'r' ) + ( Uint32 ) get_pixel_color ( column + 1, i, 'r' ) ) / 2 );
                    green = ( Uint8 ) ( ( ( Uint32 ) get_pixel_color ( column, i, 'g' ) + ( Uint32 ) get_pixel_color ( column + 1, i, 'g' ) ) / 2 );
                    blue = ( Uint8 ) ( ( ( Uint32 ) get_pixel_color ( column, i, 'b' ) + ( Uint32 ) get_pixel_color ( column + 1, i, 'b' ) ) / 2 );
                    alpha = ( Uint8 ) ( ( ( Uint32 ) get_pixel_color ( column, i, 'a' ) + ( Uint32 ) get_pixel_color ( column, i, 'a' ) ) / 2 );
                    resized->put_pixel ( j, i, SDL_MapRGBA ( surface->format, red, green, blue, alpha ) );
                    count = 0;
                    if ( end_w - ini_w - j + column - 1 != 0 )
                        actpass = ( ini_w - column ) / ( end_w - ini_w - j + column - 1 );
                    else
                        actpass = end_w + 1;
                }
                else{
                    resized->put_pixel ( j, i, get_pixel ( column, i ) );
                    column++;
                    count++;
                }
            }
        }
    }
    else{
        for ( int i = 0, actpass; i < ini_h; i++ ){
            actpass = ini_w / end_w;
            for ( int j = 0, count = 0; count < end_w - 1; j += actpass ){
                resized->put_pixel ( count, i, get_pixel ( j, i ) );
                count++;
                actpass = ( ini_w - j ) / ( end_w - count );
            }
        }
    }
    
    check_unlock ();
    resized->check_lock ();
    
    if ( end_h == ini_h )
        for ( int i = 0; i < ini_h; i++ )
            for ( int j = 0; j < end_w; j++ )
                resized2->put_pixel ( j, i, resized->get_pixel ( j, i ) );
    else if ( end_h > ini_h ){
        if ( end_h > 2 * ini_h ){
            add_h = end_h - 2 * ini_h;
            end_h = 2 * ini_h;
        }
        for ( int i = 0, actpass; i < end_w; i++ ){
            actpass = ini_h / ( end_h - ini_h );
            for ( int j = 0, count = 0, row = 0; j < end_h; j++ ){
                if ( count == actpass ){
                    Uint8 red, green, blue, alpha;
                    red = ( Uint8 ) ( ( ( Uint32 ) resized->get_pixel_color ( i, row, 'r' ) + ( Uint32 ) resized->get_pixel_color ( i, row + 1, 'r' ) ) / 2 );
                    green = ( Uint8 ) ( ( ( Uint32 ) resized->get_pixel_color ( i, row, 'g' ) + ( Uint32 ) resized->get_pixel_color ( i, row + 1, 'g' ) ) / 2 );
                    blue = ( Uint8 ) ( ( ( Uint32 ) resized->get_pixel_color ( i, row, 'b' ) + ( Uint32 ) resized->get_pixel_color ( i, row + 1, 'b' ) ) / 2 );
                    alpha = ( Uint8 ) ( ( ( Uint32 ) resized->get_pixel_color ( i, row, 'a' ) + ( Uint32 ) resized->get_pixel_color ( i, row + 1, 'a' ) ) / 2 );
                    resized2->put_pixel ( i, j, SDL_MapRGBA ( resized->surface->format, red, green, blue, alpha ) );
                    count = 0;
                    if ( end_h - ini_h - j + row - 1 != 0 )
                        actpass = ( ini_h - row ) / ( end_h - ini_h - j + row - 1 );
                    else
                        actpass = end_h + 1;
                }
                else{
                    resized2->put_pixel ( i, j, resized->get_pixel ( i, row ) );
                    row++;
                    count++;
                }
            }
        }
    }
   else{
        for ( int i = 0, actpass; i < end_w; i++ ){
            actpass = ini_h / end_h;
            for ( int j = 0, count = 0; count < end_h - 1; j += actpass ){
                resized2->put_pixel ( i, count, resized->get_pixel ( i, j ) );
                count++;
                actpass = ( ini_h - j ) / ( end_h - count );
            }
        }
    }
    
    resized->check_unlock ();
    delete resized;
    
    if( add_w != 0 || add_h != 0 )
        resized2->resize( end_w, end_h, end_w + add_w, end_h + add_h );
        
    surface = resized2->surface;
}
