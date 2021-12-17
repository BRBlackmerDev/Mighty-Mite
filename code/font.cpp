
#ifdef	STD_INCLUDE_DECL

#define FONT__START_CHAR      ( '!' )
#define FONT__END_CHAR        ( '~' )
#define FONT__CHAR_COUNT      ( '~' - '!' + 1 )

struct FONT_CHAR {
    vec2 Dim;
    vec2 offset;
    vec2 texCoord_min;
    vec2 texCoord_max;
    flo32 advanceWidth;
};

struct FONT {
    flo32 advanceWidth_space;
    flo32 ascent;
    flo32 descent;
    flo32 lineGap;
    flo32 advanceHeight;
    
    FONT_CHAR alphabet[ FONT__CHAR_COUNT ];
    
    uint32 * Texture_data;
    uint32   Texture_Width;
    uint32   Texture_Height;
};

enum TEXT_ALIGNMENT {
    TextAlign_default,
    
    TextAlign_BottomLeft,
    TextAlign_BottomCenter,
    TextAlign_BottomRight,
    
    TextAlign_CenterLeft,
    TextAlign_Center,
    TextAlign_CenterRight,
    
    TextAlign_TopLeft,
    TextAlign_TopCenter,
    TextAlign_TopRight,
    
    TextAlign_BaseLeft,
    TextAlign_BaseCenter,
    TextAlign_BaseRight,
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

// NOTE: This expects MONOSPACE FONTS only!
internal flo32
GetWidth( FONT * Font, const char * string ) {
    uint32 nChar = ( uint32 )strlen( string );
    flo32 Result = Font->advanceWidth_space * ( flo32 )nChar;
    return Result;
}

internal vec2
GetDim( FONT * Font, const char * string ) {
    vec2 Result = {};
    Result.x = GetWidth( Font, string );
    Result.y = Font->ascent + Font->descent;
    return Result;
}

internal vec2
newLine( FONT * Font, uint32 nLines = 1 ) {
    // NOTE: This is just a hacky utility function so I don't have to do this everytime: Vec2( 0.0f, Font->advanceHeight * ( flo32 )nLines )
    vec2 Result = {};
    Result.y = -Font->advanceHeight * ( flo32 )nLines;
    return Result;
}

internal vec2
getOffsetFromAlignment( FONT * Font, const char * string, vec2 scale, TEXT_ALIGNMENT align ) {
    vec2 Result = {};
    
    flo32 string_Width = GetWidth( Font, string );
    flo32 Font_ascent  = Font->ascent;
    flo32 Font_descent = Font->descent;
	
    flo32 Width     = string_Width * scale.x;
    flo32 HalfWidth = Width        * 0.5f;
    
    flo32 ascent  = Font_ascent  * scale.y;
    flo32 descent = Font_descent * scale.y;
    
    flo32 Height     = ascent + descent;
    flo32 HalfHeight = Height * 0.5f;
    
    switch( align ) {
        case TextAlign_BottomLeft:   { Result = Vec2(       0.0f, descent ); } break;
        case TextAlign_BottomCenter: { Result = Vec2( -HalfWidth, descent ); } break;
        case TextAlign_BottomRight:  { Result = Vec2(     -Width, descent ); } break;
        
        case TextAlign_CenterLeft:   { Result = Vec2(       0.0f, -HalfHeight + descent ); } break;
        case TextAlign_Center:       { Result = Vec2( -HalfWidth, -HalfHeight + descent ); } break;
        case TextAlign_CenterRight:  { Result = Vec2(     -Width, -HalfHeight + descent ); } break;
        
        case TextAlign_TopLeft:      { Result = Vec2(       0.0f, -ascent ); } break;
        case TextAlign_TopCenter:    { Result = Vec2( -HalfWidth, -ascent ); } break;
        case TextAlign_TopRight:     { Result = Vec2(     -Width, -ascent ); } break;
        
        case TextAlign_default:
        case TextAlign_BaseLeft:     {} break;
        case TextAlign_BaseCenter:   { Result = Vec2( -HalfWidth, 0.0f ); } break;
        case TextAlign_BaseRight:    { Result = Vec2(     -Width, 0.0f ); } break;
        
        default: { InvalidCodePath; } break;
    };
    
    return Result;
}

internal rect
GetBound( FONT * Font, const char * string, TEXT_ALIGNMENT align, vec2 Position, vec2 scale ) {
    Position   += getOffsetFromAlignment( Font, string, scale, align );
    Position.y += Font->ascent * scale.y;
    vec2 Dim = GetDim( Font, string ) * scale;
    rect Result = RectTLD( Position, Dim );
    return Result;
}

internal FONT
LoadFont( flo32 Font_Height ) {
    FONT Result = {};
    
    char File_Path[ 128 ] = {};
    sprintf( File_Path, "../../asset/LiberationMono.ttf" );
    
    FILE * FontFile = fopen( File_Path, "rb" );
    if( FontFile ) {
        fseek( FontFile, 0, SEEK_END );
        uint32 FontFile_bytes = ftell( FontFile );
        fseek( FontFile, 0, SEEK_SET );
        
        uint8 * memory = ( uint8 * )calloc( FontFile_bytes, 1 );
        uint32 bytesRead = ( uint32 )fread( memory, 1, FontFile_bytes, FontFile );
        Assert( FontFile_bytes == bytesRead );
        
        stbtt_Fontinfo FontInfo = {};
        stbtt_InitFont( &FontInfo, ( uint8 * )memory, 0 );
        flo32 scale = stbtt_ScaleForPixelHeight( &FontInfo, Font_Height );
        
        int32 ascent = 0;
        int32 descent = 0;
        int32 lineGap = 0;
        stbtt_GetFontVMetrics( &FontInfo, &ascent, &descent, &lineGap );
        
        Result.ascent  = fabsf( scale * ( flo32 )ascent  );
        Result.descent = fabsf( scale * ( flo32 )descent );
        Result.lineGap = fabsf( scale * ( flo32 )lineGap );
        Result.advanceHeight = Result.ascent + Result.descent + Result.lineGap;
        
        int32 Cell_maxWidth = 0;
        int32 Cell_maxHeight = 0;
        
        int32 maxIndex = FONT__CHAR_COUNT;
        for( int32 index = 0; index < maxIndex; index++ ) {
            int32 x0 = 0;
            int32 x1 = 0;
            int32 y0 = 0;
            int32 y1 = 0;
            
            int32 char_index = index + '!';
            
            stbtt_GetCodepointBitmapBox( &FontInfo, char_index, scale, scale, &x0, &y0, &x1, &y1 );
            
            int32 Width  = x1 - x0;
            int32 Height = y1 - y0;
            
            Cell_maxWidth  = MaxValue( Cell_maxWidth, Width );
            Cell_maxHeight = MaxValue( Cell_maxHeight, Height );
        }
        
        // 1-pixel apron around char
        Cell_maxWidth  += 1;
        Cell_maxHeight += 1;
        
        int32 Cell_xCount = 10;
        int32 Cell_yCount = 10;
        int32 Cell_Count  = Cell_xCount * Cell_yCount;
        int32 pixel_Count = Cell_Count * Cell_maxWidth * Cell_maxHeight;
        uint32 * Texture_data = ( uint32 * )calloc( pixel_Count, sizeof( uint32 ) );
        
        uint32 Texture_Width  = Cell_maxWidth  * Cell_xCount;
        uint32 Texture_Height = Cell_maxHeight * Cell_yCount;
        uint32 xPixel = Texture_Width;
        uint32 yPixel = Texture_Height;
        
        int32 advanceWidth    = 0;
        int32 LeftSideBearing = 0;
        
        int32 char_Count = FONT__CHAR_COUNT;
        for( int32 index = 0; index < char_Count; index++ ) {
            int32 Width   = 0;
            int32 Height  = 0;
            int32 xOffset = 0;
            int32 yOffset = 0;
            
            int32 char_index = index + FONT__START_CHAR;
            
            uint8 * bitmap = stbtt_GetCodepointBitmap( &FontInfo, 0, scale, char_index, &Width, &Height, &xOffset, &yOffset );
            stbtt_GetCodepointHMetrics( &FontInfo, char_index, &advanceWidth, &LeftSideBearing );
            
            int32 xCell = index % Cell_yCount;
            int32 yCell = index / Cell_yCount;
            int32 xMin = xCell * Cell_maxWidth;
            int32 yMin = yCell * Cell_maxHeight;
            
            FONT_CHAR _char = {};
            _char.Dim				= Vec2( ( flo32 )Width, 	( flo32 )Height );
            _char.offset			= Vec2( ( flo32 )xOffset, 	( flo32 )-yOffset );
            _char.texCoord_min.x = ( flo32 )xMin / ( flo32 )xPixel;
            _char.texCoord_min.y = ( flo32 )yMin / ( flo32 )yPixel;
            _char.texCoord_max.x = ( flo32 )( xMin + Width  ) / ( flo32 )xPixel;
            _char.texCoord_max.y = ( flo32 )( yMin + Height ) / ( flo32 )yPixel;
            _char.advanceWidth	= ( flo32 )( ( int32 )( ( flo32 )advanceWidth * scale ) );
            Result.alphabet[ index ] = _char;
            
            uint32 * destCell = Texture_data + ( ( index / Cell_yCount ) * xPixel * Cell_maxHeight ) + ( ( index % Cell_yCount ) * Cell_maxWidth );
            
            uint8  * srcRow  = bitmap + ( ( Height - 1 ) * Width );
            uint32 * destRow = destCell;
            for( int32 y = 0; y < Height; y++ ) {
                uint8 * srcPixel  = srcRow;
                uint8 * destPixel = ( uint8 * )destRow;
                for( int32 x = 0; x < Width; x++ ) {
                    uint8 alpha = *srcPixel++;
                    
                    flo32 _alpha   = ( flo32 )alpha / 255.0f;
                    flo32 _alphaSq = _alpha * _alpha;
                    uint8 rgb = ( uint8 )( _alphaSq * 255.0f );
                    
                    *destPixel++ = rgb;
                    *destPixel++ = rgb;
                    *destPixel++ = rgb;
                    *destPixel++ = alpha;
                }
                srcRow  -= Width;
                destRow += xPixel;
            }
            
            stbtt_FreeBitmap( bitmap, 0 );
        }
        
        stbtt_GetCodepointHMetrics( &FontInfo, ' ', &advanceWidth, &LeftSideBearing );
        Result.advanceWidth_space = ( ( flo32 )advanceWidth * scale );
        
        free( memory );
        
        Result.Texture_data   = Texture_data;
        Result.Texture_Width  = Texture_Width;
        Result.Texture_Height = Texture_Height;
    } else {
        InvalidCodePath;
    }
    
    return Result;
}

#endif	// STD_INCLUDE_FUNC