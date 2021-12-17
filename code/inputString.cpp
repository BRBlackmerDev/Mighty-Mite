
#define  INPUT_STRING__CHAR_MAX_COUNT  ( 128 )
struct INPUT_STRING {
    uint32 maxChar;
    uint32 nChar;
    uint32 cursor;
    char   string[ INPUT_STRING__CHAR_MAX_COUNT ];
};

internal void
addChar( INPUT_STRING * i, char c ) {
    if( ( i->nChar < i->maxChar ) && ( i->cursor < i->maxChar ) ) {
        for( int32 iter = ( int32 )i->nChar; iter >= ( int32 )i->cursor; iter-- ) {
            i->string[ iter + 1 ] = i->string[ iter ];
        }
        
        i->string[ i->cursor ] = c;
        i->cursor++;
        i->string[ ++i->nChar ] = 0;
    }
}

internal void
deleteChar_Backspace( INPUT_STRING * i ) {
    if( ( i->nChar > 0 ) && ( i->cursor > 0 ) ) {
        for( uint32 iter = i->cursor; iter <= i->nChar; iter++ ) {
            i->string[ iter - 1 ] = i->string[ iter ];
        }
        i->string[ --i->nChar ] = 0;
        i->cursor--;
    }
}

internal void
deleteChar_delete( INPUT_STRING * i ) {
    if( ( i->nChar > 0 ) && ( i->cursor < i->nChar ) ) {
        for( uint32 iter = i->cursor; iter < ( i->nChar - 1 ); iter++ ) {
            i->string[ iter ] = i->string[ iter + 1 ];
        }
        i->string[ --i->nChar ] = 0;
    }
}

internal void
clear( INPUT_STRING * i ) {
    memset( i->string, 0, i->nChar );
    i->nChar  = 0;
    i->cursor = 0;
}

internal void
InitInputString( INPUT_STRING * i, uint32 maxChar = INPUT_STRING__CHAR_MAX_COUNT ) {
    Assert( maxChar <= INPUT_STRING__CHAR_MAX_COUNT );
    
    clear( i );
    i->maxChar = maxChar;
    i->nChar    = 0;
    i->cursor   = 0;
}

internal void
setCursor( INPUT_STRING * i, int32 cursor_index ) {
    i->cursor = Clamp( cursor_index, ( int32 )0, ( int32 )i->nChar );
}