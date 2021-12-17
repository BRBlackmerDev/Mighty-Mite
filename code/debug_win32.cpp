
//----------
// Debug declarations
//----------

#if DEBUG_BUILD

internal void CONSOLE_STRING( const char * string );

struct DEBUG_SYSTEM {
    boo32 ErrorOccurred;
    RENDER_PASS * Pass_Game;
    RENDER_PASS * Pass_UI;
    FONT        * Font;
    vec2       AtPos;
    flo32      advanceHeight;
};
DEBUG_SYSTEM GlobalVar_DebugSystem = {};

#define DISPLAY_VALUE( Type, name ) {\
char __Debug_string[ 2048 ] = {};\
serialize_##Type( __Debug_string, #name, name );\
DISPLAY_STRING( __Debug_string );\
}
#define CONSOLE_VALUE( Type, name ) {\
char __Debug_string[ 2048 ] = {};\
serialize_##Type( __Debug_string, #name, name );\
strcat( __Debug_string, "\n" );\
OutputDebugString( __Debug_string );\
}
#define CONSOLE_VALUE_HEX( Type, name ) {\
char __Debug_string[ 2048 ] = {};\
serialize_hex_##Type( __Debug_string, #name, name );\
strcat( __Debug_string, "\n" );\
OutputDebugString( __Debug_string );\
}

#else
#define DISPLAY_STRING( str )
#define DISPLAY_VALUE( Type, name )
#define CONSOLE_STRING( str )

internal void PROFILE_string( char * string ) {}
internal void PROFILE_tooltip( char * string, vec2 Position ) {}

#endif

//----------
// Debug functions
//----------

#if DEBUG_BUILD
#define DEBUG_SYSTEM__MAX_VERTEX_COUNT		( 2097152 )
#define DEBUG_SYSTEM__MAX_MEMORY_SIZE    _MB( 2 )
#define DEBUG_SYSTEM__SIZE_IN_BYTES  		( sizeof( RENDER_SYSTEM ) + DEBUG_SYSTEM__MAX_MEMORY_SIZE )

internal void
CONSOLE_STRING( const char * string ) {
	OutputDebugString( string );
	OutputDebugString( "\n" );
}

#if 0
internal void
DEBUG_addTask( ASSET_ID TextureID, rect Bound, vec4 Color ) {
    RENDERER * Renderer = GlobalVar_DebugSystem.Renderer;
    
    int32 vertex_index    = Renderer->vertex_Count[ 0 ];
    int32 vertex_Count    = 4;
	DEBUG_VERTEX * vertex = ( ( DEBUG_VERTEX * )Renderer->vertex_ptr[ 0 ] ) + vertex_index;
    
    vec2 pos[ 4 ] = {
        Vec2( Bound.Left,  Bound.Bottom ),
        Vec2( Bound.Right, Bound.Bottom ),
        Vec2( Bound.Left,  Bound.Top ),
        Vec2( Bound.Right, Bound.Top ),
    };
    
    vec2 tex[ 4 ] = {
        Vec2( 0.0f, 0.0f ),
        Vec2( 1.0f, 0.0f ),
        Vec2( 0.0f, 1.0f ),
        Vec2( 1.0f, 1.0f ),
    };
    
    *vertex++ = DebugVertex( pos[ 0 ], tex[ 0 ] );
    *vertex++ = DebugVertex( pos[ 1 ], tex[ 1 ] );
    *vertex++ = DebugVertex( pos[ 2 ], tex[ 2 ] );
    *vertex++ = DebugVertex( pos[ 3 ], tex[ 3 ] );
    
    addVertex( Renderer, 0, vertex_Count );
    addObject( Renderer, RenderPassID_Debug, RenderObject( meshDrawType_triangleStrip, vertex_index, vertex_Count, TextureID, Color ) );
}
#endif

internal void
DrawString_noErrorCheck( RENDER_PASS * Pass, FONT * Font, const char * string, vec2 Position, vec2 scale, vec4 Color ) {
    VERTEX1_BUFFER * buffer = Pass->textBuffer;
    VERTEX1 * vertex = buffer->vertex_Start + buffer->nVertex;
    
    uint32 bVertex = buffer->nVertex;
    
    int32 nChar = ( int32 )strlen( string );
    
    vec2 AtPos = Position;
    for( int32 iChar = 0; iChar < nChar; iChar++ ) {
        char c = string[ iChar ];
        if( c != ' ' ) {
            int32 char_index = c - FONT__START_CHAR;
            FONT_CHAR _char = Font->alphabet[ char_index ];
            
            vec2 offset = _char.offset * scale;
            vec2 Dim    = _char.Dim    * scale;
            
            flo32 Left   = AtPos.x + offset.x;
            flo32 Right  = Left    + Dim.x;
            flo32 Top    = AtPos.y + offset.y;
            flo32 Bottom = Top     - Dim.y;
            rect Bound = Rect( Left, Bottom, Right, Top );
            
            vec3 pos[ 4 ] = {
                Vec3( Bound.Left,  Bound.Bottom, 0.0f ),
                Vec3( Bound.Right, Bound.Bottom, 0.0f ),
                Vec3( Bound.Left,  Bound.Top,    0.0f ),
                Vec3( Bound.Right, Bound.Top,    0.0f ),
            };
            
            vec2 tex[ 4 ] = {
                Vec2( _char.texCoord_min.x, _char.texCoord_min.y ),
                Vec2( _char.texCoord_max.x, _char.texCoord_min.y ),
                Vec2( _char.texCoord_min.x, _char.texCoord_max.y ),
                Vec2( _char.texCoord_max.x, _char.texCoord_max.y ),
            };
            
            *vertex++ = Vertex1( pos[ 0 ], tex[ 0 ] );
            *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
            *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
            *vertex++ = Vertex1( pos[ 3 ], tex[ 3 ] );
            *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
            *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
            
            buffer->nVertex += 6;
            
            AtPos.x += ( _char.advanceWidth * scale.x );
        } else {
            AtPos.x += ( Font->advanceWidth_space * scale.x );
        }
    }
    
    TEXT_RENDER_OBJECT object = {};
    object.bVertex  = bVertex;
    object.nVertex  = buffer->nVertex - bVertex;
    object.modColor = Color;
    
    TEXT_RENDER_OBJECT_LIST * textList = &Pass->textList;
    textList->object[ textList->nObjects++ ] = object;
}

internal void
DrawString( RENDER_PASS * Pass, FONT * Font, const char * string, vec2 Position, vec2 scale, vec4 Color ) {
    VERTEX1_BUFFER * buffer = Pass->textBuffer;
    VERTEX1 * vertex = buffer->vertex_Start + buffer->nVertex;
    
    TEXT_RENDER_OBJECT_LIST * textList = &Pass->textList;
    
    int32 nChar = ( int32 )strlen( string );
    int32 charLeft = ( ( buffer->maxVertex - buffer->nVertex ) / 6 );
    
    if( ( textList->nObjects < textList->maxObjects ) && ( nChar < charLeft ) ) {
        uint32 bVertex = buffer->nVertex;
        
        vec2 AtPos = Position;
        for( int32 iChar = 0; iChar < nChar; iChar++ ) {
            char c = string[ iChar ];
            if( c != ' ' ) {
                int32 char_index = c - FONT__START_CHAR;
                FONT_CHAR _char = Font->alphabet[ char_index ];
                
                vec2 offset = _char.offset * scale;
                vec2 Dim    = _char.Dim    * scale;
                
                flo32 Left   = AtPos.x + offset.x;
                flo32 Right  = Left    + Dim.x;
                flo32 Top    = AtPos.y + offset.y;
                flo32 Bottom = Top     - Dim.y;
                rect Bound = Rect( Left, Bottom, Right, Top );
                
                vec3 pos[ 4 ] = {
                    Vec3( Bound.Left,  Bound.Bottom, 0.0f ),
                    Vec3( Bound.Right, Bound.Bottom, 0.0f ),
                    Vec3( Bound.Left,  Bound.Top,    0.0f ),
                    Vec3( Bound.Right, Bound.Top,    0.0f ),
                };
                
                vec2 tex[ 4 ] = {
                    Vec2( _char.texCoord_min.x, _char.texCoord_min.y ),
                    Vec2( _char.texCoord_max.x, _char.texCoord_min.y ),
                    Vec2( _char.texCoord_min.x, _char.texCoord_max.y ),
                    Vec2( _char.texCoord_max.x, _char.texCoord_max.y ),
                };
                
                *vertex++ = Vertex1( pos[ 0 ], tex[ 0 ] );
                *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
                *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
                *vertex++ = Vertex1( pos[ 3 ], tex[ 3 ] );
                *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
                *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
                
                buffer->nVertex += 6;
                
                AtPos.x += ( _char.advanceWidth * scale.x );
            } else {
                AtPos.x += ( Font->advanceWidth_space * scale.x );
            }
        }
        
        TEXT_RENDER_OBJECT object = {};
        object.bVertex  = bVertex;
        object.nVertex  = buffer->nVertex - bVertex;
        object.modColor = Color;
        
        textList->object[ textList->nObjects++ ] = object;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        
        if( textList->nObjects >= textList->maxObjects ) {
            sprintf( str, "ERROR! Text Object List ran out of objects for rendering text! Max Count = %u", textList->maxObjects );
            CONSOLE_STRING( str );
        }
        if( nChar >= charLeft ) {
            sprintf( str, "ERROR! Text Vertex Buffer ran out of space for vertices for rendering text! Max Count = %u", buffer->maxVertex );
            CONSOLE_STRING( str );
        }
    }
}

internal void
DrawString( RENDER_PASS * Pass, FONT * Font, const char * string, vec2 Position, TEXT_ALIGNMENT align, vec4 Color ) {
    vec2 offset = getOffsetFromAlignment( Font, string, Vec2( 1.0f, 1.0f ), align );
    DrawString( Pass, Font, string, Position + offset, Vec2( 1.0f, 1.0f ), Color );
}

internal void
DrawString( RENDER_PASS * Pass, FONT * Font, const char * string, vec2 Position, TEXT_ALIGNMENT align, vec2 scale, vec4 Color ) {
    vec2 offset = getOffsetFromAlignment( Font, string, scale, align );
    DrawString( Pass, Font, string, Position + offset, scale, Color );
}

internal void
DEBUG_DrawString( char * string, vec2 Position, vec4 Color ) {
    RENDER_PASS * Pass = GlobalVar_DebugSystem.Pass_UI;
	FONT        * Font = GlobalVar_DebugSystem.Font;
    
    DrawString( Pass, Font, string, Position, Vec2( 1.0f, 1.0f ), Color );
}

#if 0
internal void
DEBUG_DrawRect( rect Bound, vec4 Color ) {
	DEBUG_addTask( assetID_Texture_whiteTexture, Bound, Color );
}
#endif

internal void
DISPLAY_STRING( char * string, vec4 Color = COLOR_WHITE ) {
	DEBUG_DrawString( string, GlobalVar_DebugSystem.AtPos, Color );
	GlobalVar_DebugSystem.AtPos.y += GlobalVar_DebugSystem.advanceHeight;
}

internal void
DISPLAY_STRING( char * string, vec2 Position ) {
	DEBUG_DrawString( string, Position, COLOR_BLACK );
}

internal void
PROFILE_string( char * string ) {
	DEBUG_DrawString( string, GlobalVar_DebugSystem.AtPos, COLOR_WHITE );
	GlobalVar_DebugSystem.AtPos.y += GlobalVar_DebugSystem.advanceHeight;
}

internal void
PROFILE_tooltip( char * string, vec2 Position ) {
	DEBUG_DrawString( string, Position, COLOR_WHITE );
}

internal void serialize_int8  ( char * buffer, const char * name, int8   value ) {
    sprintf( buffer, "%s: %hhd",    name, value ); }
internal void serialize_uint8 ( char * buffer, const char * name, uint8  value ) {
    sprintf( buffer, "%s: %hhu",    name, value ); }
internal void serialize_int16 ( char * buffer, const char * name, int16  value ) { sprintf( buffer, "%s: %hd",    name, value ); }
internal void serialize_int32 ( char * buffer, const char * name, int32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_uint16( char * buffer, const char * name, uint16 value ) { sprintf( buffer, "%s: %hu",    name, value ); }
internal void serialize_uint32( char * buffer, const char * name, uint32 value ) { sprintf( buffer, "%s: %u",     name, value ); }
internal void serialize_uint64( char * buffer, const char * name, uint64 value ) { sprintf( buffer, "%s: %llu",   name, value ); }
internal void serialize_boo32 ( char * buffer, const char * name, boo32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_flo32 ( char * buffer, const char * name, flo32  value ) { sprintf( buffer, "%s: %f",     name, value ); }
internal void serialize_vec2  ( char * buffer, const char * name, vec2   value ) { sprintf( buffer, "%s: %f, %f", name, value.x, value.y ); }
internal void serialize_vec3  ( char * buffer, const char * name, vec3   value ) { sprintf( buffer, "%s: %f, %f, %f", name, value.x, value.y, value.z ); }
internal void serialize_vec4  ( char * buffer, const char * name, vec4   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.x, value.y, value.z, value.w ); }
internal void serialize_quat  ( char * buffer, const char * name, quat   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.x, value.y, value.z, value.w ); }
internal void serialize_rect  ( char * buffer, const char * name, rect   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.Left, value.Bottom, value.Right, value.Top ); }
internal void serialize_MEMORY ( char * buffer, const char * name, MEMORY memory ) { sprintf( buffer, "%s: base: %llx, size: %llu, used: %llu", name, ( uint64 )memory.base, memory.size, memory.used ); }

internal void serialize_hex_uint32( char * buffer, const char * name, uint32 value ) { sprintf( buffer, "%s: %X",     name, value ); }

internal void serialize_UINT32_PAIR( char * buffer, const char * name, UINT32_PAIR value ) {
    sprintf( buffer, "%s: %u, %u", name, value.x, value.y );
}

#endif // DEBUG_BUILD