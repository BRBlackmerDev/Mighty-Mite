
//----------
// CELL TOOLS
//----------

internal UINT32_PAIR
GetCell( CAMERA_STATE * Camera, vec2 App_Dim, vec2 pos ) {
    vec2 t = Clamp01( pos / App_Dim ) - Vec2( 0.5f, 0.5f );
    vec2 P = Camera->Pos + Camera->Dim * t;
    
    int32 xCell = ( int32 )( P.x / TILE_WIDTH  );
    int32 yCell = ( int32 )( P.y / TILE_HEIGHT );
    xCell = Clamp( xCell, 0, EDITOR__GRID_MAX_WIDTH  - 1 );
    yCell = Clamp( yCell, 0, EDITOR__GRID_MAX_HEIGHT - 1 );
    
    UINT32_PAIR Result = UInt32Pair( ( uint32 )xCell, ( uint32 )yCell );
    return Result;
}

internal UINT32_PAIR
GetCellIndexFromPos( vec2 Pos ) {
    int32 xCell = ( int32 )( Pos.x / TILE_WIDTH  );
    int32 yCell = ( int32 )( Pos.y / TILE_HEIGHT );
    xCell = Clamp( xCell, 0, EDITOR__GRID_MAX_WIDTH  - 1 );
    yCell = Clamp( yCell, 0, EDITOR__GRID_MAX_HEIGHT - 1 );
    
    UINT32_PAIR Result = UInt32Pair( ( uint32 )xCell, ( uint32 )yCell );
    return Result;
}

internal UINT32_PAIR
GetCell( vec2 pos, uint32 xMin, uint32 yMin ) {
    int32 xCell = ( int32 )( pos.x / TILE_WIDTH  );
    int32 yCell = ( int32 )( pos.y / TILE_HEIGHT );
    xCell = Clamp( xCell, 0, EDITOR__GRID_MAX_WIDTH  - 1 );
    yCell = Clamp( yCell, 0, EDITOR__GRID_MAX_HEIGHT - 1 );
    
    UINT32_PAIR Result = UInt32Pair( ( uint32 )xCell - xMin, ( uint32 )yCell - yMin );
    return Result;
}

internal UINT32_PAIR
GetCell( vec2 pos, UINT32_PAIR min ) {
    UINT32_PAIR Result = GetCell( pos, min.x, min.y );
    return Result;
}

internal rect
GetCellRectFromIndex( uint32 xCell, uint32 yCell ) {
    vec2 P      = Vec2( ( flo32 )xCell, ( flo32 )yCell ) * TILE_DIM;
    rect Result = RectBLD( P, TILE_DIM );
    return Result;
}

internal rect
GetCellRectFromIndex( UINT32_PAIR Cell ) {
    rect Result = GetCellRectFromIndex( Cell.x, Cell.y );
    return Result;
}

internal rect
GetCellRectFromPos( vec2 Pos ) {
    UINT32_PAIR Cell   = GetCellIndexFromPos( Pos );
    rect        Result = GetCellRectFromIndex( Cell );
    return Result;
}

internal rect
GetBound( UINT32_QUAD Bound ) {
    rect A = GetCellRectFromIndex( Bound.min );
    rect B = GetCellRectFromIndex( Bound.max );
    rect Result = RectMM( GetBL( A ), GetTR( B ) );
    return Result;
}

internal vec2
GetCellRectCFromIndex( UINT32_PAIR Cell ) {
    rect R = GetCellRectFromIndex( Cell );
    
    vec2 Result = GetCenter( R );
    return Result;
}

internal vec2
GetCellRectBLFromIndex( UINT32_PAIR Cell ) {
    rect R = GetCellRectFromIndex( Cell );
    
    vec2 Result = GetBL( R );
    return Result;
}

internal vec2
GetCellRectBCFromIndex( UINT32_PAIR Cell ) {
    rect R = GetCellRectFromIndex( Cell );
    
    vec2 Result = GetBC( R );
    return Result;
}

internal vec2
GetCellTL( UINT32_PAIR Cell ) {
    rect R = GetCellRectFromIndex( Cell );
    
    vec2 Result = GetTL( R );
    return Result;
}

internal vec2
GetCellRectCFromPos( vec2 Pos ) {
    UINT32_PAIR Cell   = GetCellIndexFromPos( Pos );
    vec2        Result = GetCellRectCFromIndex( Cell );
    return Result;
}

internal vec2
GetCellRectBLFromPos( vec2 Pos ) {
    UINT32_PAIR Cell   = GetCellIndexFromPos( Pos );
    vec2        Result = GetCellRectBLFromIndex( Cell );
    return Result;
}

internal vec2
GetCellRectBCFromPos( vec2 Pos ) {
    UINT32_PAIR Cell   = GetCellIndexFromPos( Pos );
    vec2        Result = GetCellRectBCFromIndex( Cell );
    return Result;
}

internal vec2
GetCellTL( vec2 pos ) {
    UINT32_PAIR Cell   = GetCellIndexFromPos( pos );
    vec2        Result = GetCellTL( Cell );
    return Result;
}

internal vec2
GetWorldPos( uint32 xCell, uint32 yCell ) {
    vec2 Result = Vec2( ( flo32 )xCell, ( flo32 )yCell ) * TILE_DIM;
    return Result;
}

internal vec2
GetWorldPos( UINT32_PAIR Cell ) {
    vec2 Result = GetWorldPos( Cell.x, Cell.y );
    return Result;
}

internal vec2
GetWorldPos( CAMERA_STATE * Camera, vec2 App_Dim, vec2 pos ) {
    vec2 t = Clamp01( pos / App_Dim ) - Vec2( 0.5f, 0.5f );
    vec2 P = Camera->Pos + Camera->Dim * t;
    return P;
}

//----------
// EDITOR
//----------

internal void
ResetLayout( LAYOUT_STATE * Layout, APP_STATE * AppState ) {
    for( uint32 iLevel = 0; iLevel < Layout->nLevel; iLevel++ ) {
        LAYOUT_LEVEL * Level = Layout->Level + iLevel;
        Level->Show = false;
    }
    
    CAMERA_STATE * Camera = &Layout->Camera;
    { // Init Camera
        flo32 AspectRatio = 1920.0f / 1080.0f;
        flo32 DimY = 1080.0f / LAYOUT_Y_PIXELS_PER_UNIT;
        flo32 DimX = DimY * AspectRatio;
        
        Camera->Pos = {};
        Camera->Dim = Vec2( DimX, DimY );
    }
}

internal void
ResetCollision( APP_STATE * AppState ) {
    ResetMemory( &AppState->Collision_Memory );
    memset( &AppState->Collision, 0, sizeof( COLLISION_STATE ) );
}

internal void
InitCameraForEditor( EDITOR_STATE * Editor ) {
    CAMERA_STATE * Camera = &Editor->Camera;
    
    flo32 x = ( flo32 )EDITOR__GRID_MAX_WIDTH  * 0.5f * TILE_WIDTH;
    flo32 y = ( flo32 )EDITOR__GRID_MAX_HEIGHT * 0.5f * TILE_HEIGHT;
    
    flo32 aspectRatio = 1920.0f / 1080.0f;
    flo32 DimY = CAMERA_TILE_Y_COUNT * TILE_HEIGHT;
    flo32 DimX = DimY * aspectRatio;
    
    Camera->Pos = Vec2( x, y );
    Camera->Dim = Vec2( DimX, DimY );
}

internal void
ResetEditor( EDITOR_STATE * Editor, APP_STATE * AppState ) {
    Editor->Mode = EditorMode_Default;
    
    InitCameraForEditor( Editor );
    
    Reset( &Editor->SaveOpen );
    
    ResetCollision( AppState );
    
    uint8 * addrA = ( uint8 * )Editor;
    uint8 * addrB = ( uint8 * )&Editor->Reset_Reset;
    uint32 Reset_size = sizeof( EDITOR_STATE ) - ( uint32 )( addrB - addrA );
    memset( addrB, 0, Reset_size );
    
    { // Init Level Bound
        flo32 x = ( flo32 )EDITOR__GRID_MAX_WIDTH  * 0.5f * TILE_WIDTH;
        flo32 y = ( flo32 )EDITOR__GRID_MAX_HEIGHT * 0.5f * TILE_HEIGHT;
        vec2 Center = Vec2( x, y );
        vec2 Dim    = Vec2( LEVEL_DEFAULT_WIDTH, LEVEL_DEFAULT_HEIGHT );
        
        LEVEL_STATS * Stat = &Editor->Stat;
        Stat->Bound       = RectCD( Center, Dim );
        Stat->EnterLeftY  = Stat->Bound.Bottom + TILE_HEIGHT * 4.0f;
        Stat->EnterRightY = Stat->Bound.Bottom + TILE_HEIGHT * 4.0f;
    }
    
    Editor->DrawGrid = true;
}

internal UINT32_QUAD
GetGridBound( EDITOR_STATE * Editor ) {
    uint32 xMin = EDITOR__GRID_MAX_WIDTH;
    uint32 xMax = 0;
    uint32 yMin = EDITOR__GRID_MAX_HEIGHT;
    uint32 yMax = 0;
    
    uint32 nCell = 0;
    
    for( uint32 y = 0; y < EDITOR__GRID_MAX_HEIGHT; y++ ) {
        for( uint32 x = 0; x < EDITOR__GRID_MAX_WIDTH; x++ ) {
            if( Editor->Grid[ y ][ x ] > CellType_Empty ) {
                xMin = MinValue( x, xMin );
                xMax = MaxValue( x, xMax );
                yMin = MinValue( y, yMin );
                yMax = MaxValue( y, yMax );
                nCell++;
            }
        }
    }
    
    if( nCell == 0 ) {
        xMin = xMax = 0;
        yMin = yMax = 0;
    }
    
    UINT32_QUAD Result = {};
    Result.min.x = xMin;
    Result.min.y = yMin;
    Result.max.x = xMax;
    Result.max.y = yMax;
    return Result;
}

internal UINT32_PAIR
GetDimOfCellQuadIndex( UINT32_QUAD Bound ) {
    Assert( Bound.max.x >= Bound.min.x );
    Assert( Bound.max.y >= Bound.min.y );
    
    UINT32_PAIR Result = {};
    Result.x = ( Bound.max.x - Bound.min.x ) + 1;
    Result.y = ( Bound.max.y - Bound.min.y ) + 1;
    return Result;
}

internal UINT32_QUAD
GetCellQuadIndexFromRect( rect R ) {
    UINT32_QUAD Result = {};
    Result.min = GetCellIndexFromPos( GetBL( R ) );
    Result.max = GetCellIndexFromPos( GetTR( R ) );
    Result.max.x -= 1;
    Result.max.y -= 1;
    return Result;
}

internal UINT32_QUAD
getExtrema( UINT32_PAIR A, UINT32_PAIR B ) {
    UINT32_QUAD Result = {};
    Result.min.x = MinValue( A.x, B.x );
    Result.max.x = MaxValue( A.x, B.x );
    Result.min.y = MinValue( A.y, B.y );
    Result.max.y = MaxValue( A.y, B.y );
    return Result;
}

//----------
// CAMERA
//----------

internal void
UpdateCamera( CAMERA_STATE * Camera, KEYBOARD_STATE * Keyboard, flo32 dT ) {
    if( WasPressed ( Keyboard, KeyCode_num4 ) ) { Camera->Move_Left  = true;  }
    if( WasReleased( Keyboard, KeyCode_num4 ) ) { Camera->Move_Left  = false; }
    if( WasPressed ( Keyboard, KeyCode_num6 ) ) { Camera->Move_Right = true;  }
    if( WasReleased( Keyboard, KeyCode_num6 ) ) { Camera->Move_Right = false; }
    if( WasPressed ( Keyboard, KeyCode_num9 ) ) { Camera->Move_In    = true;  }
    if( WasReleased( Keyboard, KeyCode_num9 ) ) { Camera->Move_In    = false; }
    if( WasPressed ( Keyboard, KeyCode_num7 ) ) { Camera->Move_Out   = true;  }
    if( WasReleased( Keyboard, KeyCode_num7 ) ) { Camera->Move_Out   = false; }
    if( WasPressed ( Keyboard, KeyCode_num8 ) ) { Camera->Move_Up    = true;  }
    if( WasReleased( Keyboard, KeyCode_num8 ) ) { Camera->Move_Up    = false; }
    if( WasPressed ( Keyboard, KeyCode_num5 ) ) { Camera->Move_Down  = true;  }
    if( WasReleased( Keyboard, KeyCode_num5 ) ) { Camera->Move_Down  = false; }
    flo32 ratio = Camera->Dim.y / ( CAMERA_TILE_Y_COUNT * TILE_HEIGHT );
    flo32 speed = 9.0f * ratio;
    Camera->Scale_Ratio = ratio;
    
    if( Camera->Move_Left ) {
        Camera->Pos.x -= ( speed * dT );
    }
    if( Camera->Move_Right ) {
        Camera->Pos.x += ( speed * dT );
    }
    if( Camera->Move_Down ) {
        Camera->Pos.y -= ( speed * dT );
    }
    if( Camera->Move_Up ) {
        Camera->Pos.y += ( speed * dT );
    }
    if( Camera->Move_In ) {
        Camera->Dim *= 0.99f;
    }
    if( Camera->Move_Out ) {
        Camera->Dim *= 1.01f;
    }
    
    DISPLAY_VALUE( vec2, Camera->Pos );
}

//----------
// GRID TOOLS
//----------

#define GRID_FLAG__EDGE_EXISTS_LEFT    ( 0x01 )
#define GRID_FLAG__EDGE_EXISTS_BOTTOM  ( 0x02 )
#define GRID_FLAG__EDGE_EXISTS_RIGHT   ( 0x04 )
#define GRID_FLAG__EDGE_EXISTS_TOP     ( 0x08 )

internal boo32
hasNEmptyCellsDown( uint8 * border, uint32 xCell, uint32 yCell, UINT32_PAIR Cell, uint32 nMax ) {
    // NOTE: Count the number of empty Cells below this Cell, including this Cell.
    
    boo32 Result = true;
    if( Cell.y < nMax ) {
        Result = false;
    } else {
        uint32 yMin = Cell.y - nMax;
        for( uint32 iY = Cell.y; iY > yMin; iY-- ) {
            uint32 iCell = iY * xCell + Cell.x;
            if( border[ iCell ] & GRID_FLAG__EDGE_EXISTS_TOP ) {
                Result = false;
            }
        }
    }
    return Result;
}

internal uint32
hasNEmptyCellsUp( uint8 * border, uint32 xCell, uint32 yCell, UINT32_PAIR Cell, uint32 nMax ) {
    // NOTE: Count the number of empty Cells above this Cell, NOT including this Cell.
    
    boo32 Result = true;
    if( Cell.y >= ( yCell - nMax ) ) {
        Result = false;
    } else {
        uint32 yMax = Cell.y + nMax;
        for( uint32 iY = Cell.y + 1; iY <= yMax; iY++ ) {
            uint32 iCell = iY * xCell + Cell.x;
            if( border[ iCell ] & GRID_FLAG__EDGE_EXISTS_BOTTOM ) {
                Result = false;
            }
        }
    }
    return Result;
}

//----------
// KEYBOARD INPUT
//----------

internal void
UpdateKeyboardInputForFileSaveOpen( INPUT_STRING * i, KEYBOARD_STATE * Keyboard, FILE_SAVE_OPEN * State ) {
    for( uint32 iKeyEvent = 0; iKeyEvent < Keyboard->nEvents; iKeyEvent++ ) {
        KEY_STATE Event = Keyboard->Event[ iKeyEvent ];
        if( Event.IsDown ) {
            if( ( Event.KeyCode >= KeyCode_a ) && ( Event.KeyCode <= KeyCode_z ) ) {
                char code = ( char )( Event.KeyCode - KeyCode_a );
                if( Event.flags == KEYBOARD_FLAGS__SHIFT ) {
                    addChar( i, 'A' + code );
                } else if( Event.flags == 0 ) {
                    addChar( i, 'a' + code );
                }
                State->overwrite = false;
            } else if( ( Event.KeyCode >= KeyCode_0 ) && ( Event.KeyCode <= KeyCode_9 ) ) {
                char code = ( char )( Event.KeyCode - KeyCode_0 );
                addChar( i, '0' + code );
                State->overwrite = false;
            } else if( ( Event.KeyCode == KeyCode_dash ) && ( Event.flags == KEYBOARD_FLAGS__SHIFT ) ) {
                addChar( i, '_' );
                State->overwrite = false;
            } else if( Event.KeyCode == KeyCode_Backspace ) {
                deleteChar_Backspace( i );
                State->overwrite = false;
            }
        }
    }
}

//----------
// LEVEL BITMAP
//----------

internal UINT32_QUAD
GetPixelBoundOfEntityForLevelBitmap( rect EntityBound, rect LevelBound, uint32 xPixel, uint32 yPixel ) {
    vec2 LevelDim = GetDim( LevelBound );
    
    UINT32_QUAD Result = {};
    if( DoesRectIntersectRectInc( EntityBound, LevelBound ) ) {
        flo32 tMinX = Clamp01( ( ( EntityBound.Left   - LevelBound.Left   ) / LevelDim.x ) );
        flo32 tMinY = Clamp01( ( ( EntityBound.Bottom - LevelBound.Bottom ) / LevelDim.y ) );
        flo32 tMaxX = Clamp01( ( ( EntityBound.Right  - LevelBound.Left   ) / LevelDim.x ) );
        flo32 tMaxY = Clamp01( ( ( EntityBound.Top    - LevelBound.Bottom ) / LevelDim.y ) );
        
        Result.min.x = ( uint32 )( tMinX * ( flo32 )xPixel );
        Result.min.y = ( uint32 )( tMinY * ( flo32 )yPixel );
        Result.max.x = ( uint32 )( tMaxX * ( flo32 )xPixel );
        Result.max.y = ( uint32 )( tMaxY * ( flo32 )yPixel );
    }
    
    return Result;
}

internal void
OutputRectToLevelBitmap( UINT32_QUAD Bound, uint32 * Pixel, uint32 xPixel, uint32 yPixel, vec4 V4Color ) {
    uint32 Color = ToU32Color( V4Color );
    for( uint32 Y = Bound.min.y; Y < Bound.max.y; Y++ ) {
        for( uint32 X = Bound.min.x; X < Bound.max.x; X++ ) {
            uint32 iPixel = Y * xPixel + X;
            Pixel[ iPixel ] = Color;
        }
    }
}

internal void
OutputRectOutlineToLevelBitmap( UINT32_QUAD Bound, uint32 * Pixel, uint32 xPixel, uint32 yPixel, vec4 V4Color ) {
    uint32 Color = ToU32Color( V4Color );
    for( uint32 Y = Bound.min.y; Y < Bound.max.y; Y++ ) {
        for( uint32 X = Bound.min.x; X < Bound.max.x; X++ ) {
            if( !( ( X > Bound.min.x ) && ( Y > Bound.min.y ) && ( X < ( Bound.max.x - 1 ) ) && ( Y < ( Bound.max.y - 1 ) ) ) ) {
                uint32 iPixel = Y * xPixel + X;
                Pixel[ iPixel ] = Color;
            }
        }
    }
}

internal void
OutputCircleToLevelBitmap( UINT32_QUAD Bound, uint32 * Pixel, uint32 xPixel, uint32 yPixel, vec4 V4Color ) {
    vec2  Min      = Vec2( ( flo32 )Bound.min.x, ( flo32 )Bound.min.y );
    vec2  Max      = Vec2( ( flo32 )Bound.max.x, ( flo32 )Bound.max.y );
    flo32 Dim      = MinValue( Max.x - Min.x, Max.y - Min.y );
    vec2  Center   = ( Min + Max ) * 0.5f;
    flo32 Radius   = Dim * 0.5f;
    flo32 RadiusSq = Radius * Radius;
    
    uint32 Color = ToU32Color( V4Color );
    for( uint32 Y = Bound.min.y; Y < Bound.max.y; Y++ ) {
        for( uint32 X = Bound.min.x; X < Bound.max.x; X++ ) {
            vec2  P      = Vec2( ( flo32 )X, ( flo32 )Y ) + Vec2( 0.5f, 0.5f );
            flo32 DistSq = GetLengthSq( P - Center );
            if( DistSq <= RadiusSq ) {
                uint32 iPixel = Y * xPixel + X;
                Pixel[ iPixel ] = Color;
            }
        }
    }
}