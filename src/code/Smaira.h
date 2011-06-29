Uint32 drawer::get_pixel ( SDL_Surface *source, int x, int y ){
    Uint32 *pixels = ( Uint32 * ) source->pixels;
    
    return pixels [ ( y * source->w ) + x ];
}

void drawer::put_pixel ( SDL_Surface *destination, int x, int y, Uint32 pixel ){
    Uint32 *pixels = ( Uint32 * ) destination->pixels;
    
    pixels[ ( y * destination->w ) + x ] = pixel;
}


void drawer::line ( SDL_Surface *destination, int ini_x, int ini_y, int end_x, int end_y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255 ){
    if ( end_x != ini_x ){
        if ( end_x < ini_x ){
            end_x ^= ini_x ^= end_x ^= ini_x;
            end_y ^= ini_y ^= end_y ^= ini_y;
        }
        for ( int i = ini_x; i <= end_x; i++ )
            put_pixel ( destination, i, ini_y + ( i - ini_x ) * ( end_y - ini_y ) / ( end_x - ini_x ), SDL_MapRGBA ( destination->format, red, green, blue, alpha ) );
    }
    else{
        if ( end_y < ini_y )
            end_y ^= ini_y ^= end_y ^= ini_y;
        for ( int i = ini_y; i <= end_y; i++)
            put_pixel ( destination, ini_x, i, SDL_MapRGBA ( destination->format, red, green, blue, alpha ) );
    }
}

void drawer::rectangle ( SDL_Surface *destination, int left, int top, int right, int bottom, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255 ){
    line ( destination, left, top, right, top, red, green, blue, alpha );
    line ( destination, right, top, right, bottom, red, green, blue, alpha );
    line ( destination, right, bottom, left, bottom, red, green, blue, alpha );
    line ( destination, left, bottom, left, top, red, green, blue, alpha );
}

void drawer::bar ( SDL_Surface *destination, int left, int top, int right, int bottom, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255 ){
    for ( int i = left; i <= right; i++ )
        line ( destination, i, top, i, bottom, red, green, blue, alpha );
}

Uint8 drawer::get_pixel_color ( SDL_Surface *source, int x, int y, char c ){
    SDL_PixelFormat *fmt;
    Uint32 temp, pixel;
    Uint8 color;
    
    fmt = source->format;
    SDL_LockSurface ( source );
    pixel = get_pixel ( source, x, y );
    SDL_UnlockSurface ( source );
    
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

void drawer::check_unlock ( SDL_Surface *source ){
    if ( SDL_MUSTLOCK ( source ) )
        SDL_UnlockSurface ( source );
}

void drawer::check_lock ( SDL_Surface *source ){
    if ( SDL_MUSTLOCK ( source ) )
        SDL_LockSurface ( source );
}

SDL_Surface* drawer::load_image ( char *name ){
    SDL_Surface *loaded = NULL;
    SDL_Surface *optimized = NULL;
    
    loaded = IMG_Load ( name );
    if ( loaded != NULL ){
        optimized = SDL_DisplayFormat ( loaded );
        if ( optimized != NULL )
            SDL_SetColorKey ( optimized,SDL_SRCCOLORKEY,SDL_MapRGB ( optimized->format, 0, 255, 255 ) );
    }
    SDL_FreeSurface ( loaded );
    
    return optimized;
}

void drawer::apply_surface ( int x, int y,SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL ){
    SDL_Rect offset;
    
    offset.x = x;
    offset.y = y;
    
    SDL_BlitSurface ( source, clip, destination, &offset );
}
