
internal void
addTerrain( APP_STATE * AppState, uint32 x, uint32 y ) {
    TERRAIN_STATE * State = &AppState->Terrain;
    
    TERRAIN Terrain = {};
    
    vec2 P = Vec2( ( flo32 )x, ( flo32 )y ) * TILE_DIM;
    Terrain.Bound = RectBLD( P, TILE_DIM );
    
    Assert( State->nTerrain < TERRAIN_MAX_COUNT );
    State->Terrain[ State->nTerrain++ ] = Terrain;
}

internal void
GenTerrainCollision( APP_STATE * AppState, UINT32_PAIR * Terrain, uint32 nTerrain, MEMORY * TempMemory ) {
    ResetMemory( &AppState->Collision_Memory );
    memset( &AppState->Collision, 0, sizeof( COLLISION_STATE ) );
    
    // get Grid Bound
    uint32 xMin = EDITOR__GRID_MAX_WIDTH;
    uint32 xMax = 0;
    uint32 yMin = EDITOR__GRID_MAX_HEIGHT;
    uint32 yMax = 0;
    for( uint32 iTerrain = 0; iTerrain < nTerrain; iTerrain++ ) {
        UINT32_PAIR t = Terrain[ iTerrain ];
        xMin = MinValue( xMin, t.x );
        xMax = MaxValue( xMax, t.x );
        yMin = MinValue( yMin, t.y );
        yMax = MaxValue( yMax, t.y );
    }
    
    uint32 nEdge_Left   = 0;
    uint32 nEdge_Bottom = 0;
    uint32 nEdge_Right  = 0;
    uint32 nEdge_Top    = 0;
    uint32 maxEdge      = nTerrain * 4;
    
    vec4 * Edge_Left   = _pushArray_clear( TempMemory, vec4, maxEdge );
    vec4 * Edge_Bottom = _pushArray_clear( TempMemory, vec4, maxEdge );
    vec4 * Edge_Right  = _pushArray_clear( TempMemory, vec4, maxEdge );
    vec4 * Edge_Top    = _pushArray_clear( TempMemory, vec4, maxEdge );
    
    // init Grid
    uint32 xCell = ( xMax - xMin + 1 );
    uint32 yCell = ( yMax - yMin + 1 );
    uint32 nCell = xCell * yCell;
    uint8 * border = _pushArray_clear( TempMemory, uint8, nCell );
    for( uint32 iTerrain = 0; iTerrain < nTerrain; iTerrain++ ) {
        UINT32_PAIR t = Terrain[ iTerrain ];
        
        uint32 x = ( t.x - xMin );
        uint32 y = ( t.y - yMin );
        uint32 Cell = y * xCell + x;
        border[ Cell ] = 0xFF;
    }
    
    uint8 wasNotVisited_Left   = 0x10;
    uint8 wasNotVisited_Bottom = 0x20;
    uint8 wasNotVisited_Right  = 0x40;
    uint8 wasNotVisited_Top    = 0x80;
    uint8 EdgeExists_Left      = 0x01;
    uint8 EdgeExists_Bottom    = 0x02;
    uint8 EdgeExists_Right     = 0x04;
    uint8 EdgeExists_Top       = 0x08;
    
    uint8 flag_Left   = ~( wasNotVisited_Left   | EdgeExists_Left   );
    uint8 flag_Bottom = ~( wasNotVisited_Bottom | EdgeExists_Bottom );
    uint8 flag_Right  = ~( wasNotVisited_Right  | EdgeExists_Right  );
    uint8 flag_Top    = ~( wasNotVisited_Top    | EdgeExists_Top    );
    
    // clear invalid Edges
    for( uint32 iTerrain = 0; iTerrain < nTerrain; iTerrain++ ) {
        UINT32_PAIR t = Terrain[ iTerrain ];
        
        uint32 x = ( t.x - xMin );
        uint32 y = ( t.y - yMin );
        uint32 Cell = y * xCell + x;
        
        uint8 * bA = border + Cell;
        
        if( x > 0 ) {
            uint8 * bB = border + Cell - 1;
            if( *bB ) {
                bA[ 0 ] &= flag_Left;
                bB[ 0 ] &= flag_Right;
            }
        }
        if( x < ( xCell - 2 ) ) {
            uint8 * bB = border + Cell + 1;
            if( *bB ) {
                bA[ 0 ] &= flag_Right;
                bB[ 0 ] &= flag_Left;
            }
        }
        if( y > 0 ) {
            uint8 * bB = border + Cell - xCell;
            if( *bB ) {
                bA[ 0 ] &= flag_Bottom;
                bB[ 0 ] &= flag_Top;
            }
        }
        if( y < ( yCell - 2 ) ) {
            uint8 * bB = border + Cell + xCell;
            if( *bB ) {
                bA[ 0 ] &= flag_Top;
                bB[ 0 ] &= flag_Bottom;
            }
        }
    }
    
    // gen Collision geometry
    for( uint32 iCell = 0; iCell < nCell; iCell++ ) {
        uint8 * Cell = border + iCell;
        if( Cell[ 0 ] & 0xF0 ) {
            uint32 atCell = iCell;
            uint8 * CellA = Cell;
            
            Assert( CellA[ 0 ] & wasNotVisited_Left   );
            Assert( CellA[ 0 ] & wasNotVisited_Bottom );
            
            int32 x = ( atCell % xCell ) + xMin;
            int32 y = ( atCell / xCell ) + yMin;
            
            uint32 EdgeID = 0;
            vec2 P = Vec2( ( flo32 )x, ( flo32 )y ) * TILE_DIM;
            vec2 R = P;
            R.y += TILE_HEIGHT;
            CellA[ 0 ] &= ( ~wasNotVisited_Left );
            
            
            boo32 findIsland = true;
            while( findIsland ) {
                switch( EdgeID ) {
                    case 0: { // Left
                        boo32 foundNext = false;
                        if( CellA[ 0 ] & wasNotVisited_Top ) {
                            foundNext = true;
                            
                            Edge_Left[ nEdge_Left++ ] = Vec4( P, R );
                            
                            P    = R;
                            R.x += TILE_WIDTH;
                            
                            CellA[ 0 ] &= ( ~wasNotVisited_Top );
                            
                            EdgeID = 3;
                        }
                        if( !foundNext ) {
                            CellA += xCell;
                            if( CellA[ 0 ] & wasNotVisited_Left ) {
                                foundNext = true;
                                
                                R.y += TILE_HEIGHT;
                                
                                CellA[ 0 ] &= ( ~wasNotVisited_Left );
                            }
                        }
                        if( !foundNext ) {
                            CellA -= 1;
                            if( CellA[ 0 ] & wasNotVisited_Bottom ) {
                                foundNext = true;
                                
                                Edge_Left[ nEdge_Left++ ] = Vec4( P, R );
                                
                                P    = R;
                                R.x -= TILE_WIDTH;
                                
                                CellA[ 0 ] &= ( ~wasNotVisited_Bottom );
                                
                                EdgeID = 1;
                            }
                        }
                        if( !foundNext ) {
                            InvalidCodePath;
                        }
                    } break;
                    
                    case 1: { // Bottom
                        boo32 foundNext = false;
                        if( CellA[ 0 ] & wasNotVisited_Left ) {
                            foundNext = true;
                            
                            Edge_Bottom[ nEdge_Bottom++ ] = Vec4( P, R );
                            
                            P    = R;
                            R.y += TILE_HEIGHT;
                            
                            CellA[ 0 ] &= ( ~wasNotVisited_Left );
                            
                            EdgeID = 0;
                        }
                        if( !foundNext ) {
                            CellA -= 1;
                            if( CellA[ 0 ] & wasNotVisited_Bottom ) {
                                foundNext = true;
                                
                                R.x -= TILE_WIDTH;
                                
                                CellA[ 0 ] &= ( ~wasNotVisited_Bottom );
                            }
                        }
                        if( !foundNext ) {
                            CellA -= xCell;
                            if( CellA[ 0 ] & wasNotVisited_Right ) {
                                foundNext = true;
                                
                                Edge_Bottom[ nEdge_Bottom++ ] = Vec4( P, R );
                                
                                P    = R;
                                R.y -= TILE_HEIGHT;
                                
                                CellA[ 0 ] &= ( ~wasNotVisited_Right );
                                
                                EdgeID = 2;
                            }
                        }
                        if( !foundNext ) {
                            Edge_Bottom[ nEdge_Bottom++ ] = Vec4( P, R );
                            findIsland = false;
                        }
                    } break;
                    
                    case 2: { // Right
                        boo32 foundNext = false;
                        if( CellA[ 0 ] & wasNotVisited_Bottom ) {
                            foundNext = true;
                            
                            Edge_Right[ nEdge_Right++ ] = Vec4( P, R );
                            
                            P    = R;
                            R.x -= TILE_WIDTH;
                            
                            CellA[ 0 ] &= ( ~wasNotVisited_Bottom );
                            
                            EdgeID = 1;
                        }
                        if( !foundNext ) {
                            CellA -= xCell;
                            if( CellA[ 0 ] & wasNotVisited_Right ) {
                                foundNext = true;
                                
                                R.y -= TILE_HEIGHT;
                                
                                CellA[ 0 ] &= ( ~wasNotVisited_Right );
                            }
                        }
                        if( !foundNext ) {
                            CellA += 1;
                            if( CellA[ 0 ] & wasNotVisited_Top ) {
                                foundNext = true;
                                
                                Edge_Right[ nEdge_Right++ ] = Vec4( P, R );
                                
                                P    = R;
                                R.x += TILE_WIDTH;
                                
                                CellA[ 0 ] &= ( ~wasNotVisited_Top );
                                
                                EdgeID = 3;
                            }
                        }
                        if( !foundNext ) {
                            InvalidCodePath;
                        }
                    } break;
                    
                    case 3: { // Top
                        boo32 foundNext = false;
                        if( CellA[ 0 ] & wasNotVisited_Right ) {
                            foundNext = true;
                            
                            Edge_Top[ nEdge_Top++ ] = Vec4( P, R );
                            
                            P    = R;
                            R.y -= TILE_HEIGHT;
                            
                            CellA[ 0 ] &= ( ~wasNotVisited_Right );
                            
                            EdgeID = 2;
                        }
                        if( !foundNext ) {
                            CellA += 1;
                            if( CellA[ 0 ] & wasNotVisited_Top ) {
                                foundNext = true;
                                
                                R.x += TILE_WIDTH;
                                
                                CellA[ 0 ] &= ( ~wasNotVisited_Top );
                            }
                        }
                        if( !foundNext ) {
                            CellA += xCell;
                            if( CellA[ 0 ] & wasNotVisited_Left ) {
                                foundNext = true;
                                
                                Edge_Top[ nEdge_Top++ ] = Vec4( P, R );
                                
                                P    = R;
                                R.y += TILE_HEIGHT;
                                
                                CellA[ 0 ] &= ( ~wasNotVisited_Left );
                                
                                EdgeID = 0;
                            }
                        }
                        if( !foundNext ) {
                            InvalidCodePath;
                        }
                    } break;
                }
            }
        }
    }
    
    uint32 at = 0;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    Collision->Type[ 0 ] = UInt32Pair( at, nEdge_Left   );  at += nEdge_Left;
    Collision->Type[ 1 ] = UInt32Pair( at, nEdge_Bottom );  at += nEdge_Bottom;
    Collision->Type[ 2 ] = UInt32Pair( at, nEdge_Right  );  at += nEdge_Right;
    Collision->Type[ 3 ] = UInt32Pair( at, nEdge_Top    );  at += nEdge_Top;
    
    Collision->Edge  = _pushArray_clear( &AppState->Collision_Memory, vec4, at );
    Collision->nEdge = at;
    
    at = 0;
    memcpy( Collision->Edge + at, Edge_Left,   sizeof( vec4 ) * nEdge_Left   ); at += nEdge_Left;
    memcpy( Collision->Edge + at, Edge_Bottom, sizeof( vec4 ) * nEdge_Bottom ); at += nEdge_Bottom;
    memcpy( Collision->Edge + at, Edge_Right,  sizeof( vec4 ) * nEdge_Right  ); at += nEdge_Right;
    memcpy( Collision->Edge + at, Edge_Top,    sizeof( vec4 ) * nEdge_Top    ); at += nEdge_Top;
    
    LEVEL_STATS Stat = AppState->Editor.Stat;
    { // Jump Bounds
        UINT32_PAIR Type = Collision->Type[ 3 ];
        
        flo32 OffsetUp   = TILE_HEIGHT * 0.1f;
        flo32 OffsetDown = TILE_HEIGHT * 0.025f;
        
        if( Type.n > 0 ) {
            Collision->nJumpBound = Type.n;
            Collision->JumpBound  = _pushArray( &AppState->Collision_Memory, rect, Type.n );
            
            vec4 * Edge = Collision->Edge + Type.m;
            for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                vec4 e = Edge[ iEdge ];
                
                vec2 P = e.xy + Vec2( -( PLAYER_HALF_WIDTH + 0.125f ), -OffsetDown );
                vec2 Q = e.zw + Vec2(  ( PLAYER_HALF_WIDTH + 0.125f ),  OffsetUp   );
                rect R = RectMM( P, Q );
                
                Collision->JumpBound[ iEdge ] = R;
            }
        }
    }
    
    { // wall slide Bounds
        // TODO: don't extend the wall Jump down if there is a floor below it
        flo32 WallOffset = TILE_WIDTH * 0.5f;
        { // Left
            UINT32_PAIR Type = Collision->Type[ 0 ];
            if( Type.n > 0 ) {
                Collision->nWallSlideLeft = Type.n;
                Collision->WallSlideLeft  = _pushArray( &AppState->Collision_Memory, rect, Type.n );
                
                vec4 * Edge = Collision->Edge + Type.m;
                for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                    vec4 e = Edge[ iEdge ];
                    
                    flo32 length = GetLength( e.zw - e.xy );
                    if( length >= WALL_JUMP_MIN_LENGTH ) {
                        flo32 extension = 0.0f;
                        if( length >= WALL_JUMP_MIN_LENGTH_FOR_EXTENSION ) {
                            extension = WALL_JUMP_NEG_Y_EXTENSION;
                        }
                        vec2 P = e.xy + Vec2( -WallOffset, extension );
                        vec2 Q = e.zw;
                        rect R = RectMM( P, Q );
                        
                        Collision->WallSlideLeft[ iEdge ] = R;
                    }
                }
            }
        }
        
        { // Right
            UINT32_PAIR Type = Collision->Type[ 2 ];
            if( Type.n > 0 ) {
                Collision->nWallSlideRight = Type.n;
                Collision->WallSlideRight  = _pushArray( &AppState->Collision_Memory, rect, Type.n );
                
                vec4 * Edge = Collision->Edge + Type.m;
                for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                    vec4 e = Edge[ iEdge ];
                    
                    flo32 length = GetLength( e.zw - e.xy );
                    if( length >= WALL_JUMP_MIN_LENGTH ) {
                        flo32 extension = 0.0f;
                        if( length >= WALL_JUMP_MIN_LENGTH_FOR_EXTENSION ) {
                            extension = WALL_JUMP_NEG_Y_EXTENSION;
                        }
                        vec2 P = e.zw + Vec2( 0.0f, extension );
                        vec2 Q = e.xy + Vec2( WallOffset, 0.0f );
                        rect R = RectMM( P, Q );
                        
                        Collision->WallSlideRight[ iEdge ] = R;
                    }
                }
            }
        }
    }
    
    _popArray( TempMemory, vec4, maxEdge * 4 );
    _popArray( TempMemory, uint8, nCell );
}

internal void
DrawTerrain( RENDER_PASS * Pass, APP_STATE * AppState ) {
    TERRAIN_STATE * State = &AppState->Terrain;
    
    for( uint32 iTerrain = 0; iTerrain < State->nTerrain; iTerrain++ ) {
        TERRAIN Terrain = State->Terrain[ iTerrain ];
        
        DrawRect       ( Pass, Terrain.Bound, TERRAIN_COLOR );
        DrawRectOutline( Pass, Terrain.Bound, TERRAIN_OUTLINE_COLOR );
    }
}

#define VERSION__TERRAIN  ( 1 )
#define FILETAG__TERRAIN  ( "TERRAIN_" )
internal void
SaveTerrain( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__TERRAIN;
    uint32 Version = VERSION__TERRAIN;
    uint32 nTerrain = 0;
    for( uint32 iRow = 0; iRow < EDITOR__GRID_MAX_HEIGHT; iRow++ ) {
        for( uint32 iCol = 0; iCol < EDITOR__GRID_MAX_WIDTH; iCol++ ) {
            GRID_CELL_TYPE Type = ( GRID_CELL_TYPE )Editor->Grid[ iRow ][ iCol ];
            if( Type == CellType_Terrain ) {
                nTerrain++;
            }
        }
    }
    
    if( nTerrain > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, EDITOR__GRID_MAX_WIDTH  );
        _writem( output, uint32, EDITOR__GRID_MAX_HEIGHT );
        
        _writem( output, uint32, nTerrain );
        for( uint32 iRow = 0; iRow < EDITOR__GRID_MAX_HEIGHT; iRow++ ) {
            for( uint32 iCol = 0; iCol < EDITOR__GRID_MAX_WIDTH; iCol++ ) {
                GRID_CELL_TYPE Type = ( GRID_CELL_TYPE )Editor->Grid[ iRow ][ iCol ];
                if( Type == CellType_Terrain ) {
                    _writem( output, uint32, iCol );
                    _writem( output, uint32, iRow );
                }
            }
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal void
EDITOR_LoadTerrain( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__TERRAIN;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        switch( Version ) {
            case 1: {
                uint32 Grid_Width  = _read( ptr, uint32 );
                uint32 Grid_Height = _read( ptr, uint32 );
                Assert( Grid_Width  == EDITOR__GRID_MAX_WIDTH  );
                Assert( Grid_Height == EDITOR__GRID_MAX_HEIGHT );
                
                uint32 nTerrainPair = _read( ptr, uint32 );
                for( uint32 iTerrainPair = 0; iTerrainPair < nTerrainPair; iTerrainPair++ ) {
                    UINT32_PAIR TerrainPair = _read( ptr, UINT32_PAIR );
                    
                    Editor->Grid[ TerrainPair.y ][ TerrainPair.x ] = CellType_Terrain;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
GAME_LoadTerrain( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__TERRAIN;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        switch( Version ) {
            case 1: {
                uint32 Grid_Width  = _read( ptr, uint32 );
                uint32 Grid_Height = _read( ptr, uint32 );
                Assert( Grid_Width  == EDITOR__GRID_MAX_WIDTH  );
                Assert( Grid_Height == EDITOR__GRID_MAX_HEIGHT );
                
                uint32       nTerrainPair = _read( ptr, uint32 );
                UINT32_PAIR * TerrainPair = ( UINT32_PAIR * )ptr;
                
                for( uint32 iTerrain = 0; iTerrain < nTerrainPair; iTerrain++ ) {
                    UINT32_PAIR t = TerrainPair[ iTerrain ];
                    addTerrain( AppState, t.x, t.y );
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
Editor_MoveTerrain( EDITOR_STATE * Editor, MEMORY * TempMemory, rect Src_Bound, rect Dest_Bound ) {
    UINT32_QUAD Src  = GetCellQuadIndexFromRect( Src_Bound  );
    UINT32_QUAD Dest = GetCellQuadIndexFromRect( Dest_Bound );
    
    uint32 nCell = EDITOR__GRID_MAX_HEIGHT * EDITOR__GRID_MAX_WIDTH;
    uint8 * Cell = _pushArray( TempMemory, uint8, nCell );
    
    for( uint32 iY = Src.min.y; iY <= Src.max.y; iY++ ) {
        for( uint32 iX = Src.min.x; iX <= Src.max.x; iX++ ) {
            uint32 iCell = iY * EDITOR__GRID_MAX_HEIGHT + iX;
            Cell[ iCell ] = Editor->Grid[ iY ][ iX ];
            
            Editor->Grid[ iY ][ iX ] = CellType_Empty;
        }
    }
    
    for( uint32 iY = Src.min.y, dY = Dest.min.y; iY <= Src.max.y; iY++, dY++ ) {
        for( uint32 iX = Src.min.x, dX = Dest.min.x; iX <= Src.max.x; iX++, dX++ ) {
            uint32 iCell = iY * EDITOR__GRID_MAX_HEIGHT + iX;
            Editor->Grid[ dY ][ dX ] = Cell[ iCell ];
        }
    }
    
    _popArray( TempMemory, uint8, nCell );
}

internal void
CopyTerrain( EDITOR_STATE * Editor, MEMORY * TempMemory, rect Src_Bound, rect Dest_Bound ) {
    UINT32_QUAD Src  = GetCellQuadIndexFromRect( Src_Bound  );
    UINT32_QUAD Dest = GetCellQuadIndexFromRect( Dest_Bound );
    
    uint32 nCell = EDITOR__GRID_MAX_HEIGHT * EDITOR__GRID_MAX_WIDTH;
    uint8 * Cell = _pushArray( TempMemory, uint8, nCell );
    
    for( uint32 iY = Src.min.y; iY <= Src.max.y; iY++ ) {
        for( uint32 iX = Src.min.x; iX <= Src.max.x; iX++ ) {
            uint32 iCell = iY * EDITOR__GRID_MAX_HEIGHT + iX;
            Cell[ iCell ] = Editor->Grid[ iY ][ iX ];
        }
    }
    
    for( uint32 iY = Src.min.y, dY = Dest.min.y; iY <= Src.max.y; iY++, dY++ ) {
        for( uint32 iX = Src.min.x, dX = Dest.min.x; iX <= Src.max.x; iX++, dX++ ) {
            uint32 iCell = iY * EDITOR__GRID_MAX_HEIGHT + iX;
            Editor->Grid[ dY ][ dX ] = Cell[ iCell ];
        }
    }
    
    _popArray( TempMemory, uint8, nCell );
}

internal void
Editor_MirrorTerrain( EDITOR_STATE * Editor, MEMORY * TempMemory, rect Src_Bound ) {
    UINT32_QUAD Src  = GetCellQuadIndexFromRect( Src_Bound  );
    
    uint32 nCell = EDITOR__GRID_MAX_HEIGHT * EDITOR__GRID_MAX_WIDTH;
    uint8 * Cell = _pushArray( TempMemory, uint8, nCell );
    
    for( uint32 iY = Src.min.y; iY <= Src.max.y; iY++ ) {
        for( uint32 iX = Src.min.x; iX <= Src.max.x; iX++ ) {
            uint32 iCell = iY * EDITOR__GRID_MAX_HEIGHT + iX;
            Cell[ iCell ] = Editor->Grid[ iY ][ iX ];
            
            Editor->Grid[ iY ][ iX ] = CellType_Empty;
        }
    }
    
    for( uint32 iY = Src.min.y; iY <= Src.max.y; iY++ ) {
        for( uint32 iX = Src.min.x, dX = Src.max.x; iX <= Src.max.x; iX++, dX-- ) {
            uint32 iCell = iY * EDITOR__GRID_MAX_HEIGHT + iX;
            Editor->Grid[ iY ][ dX ] = Cell[ iCell ];
        }
    }
    
    _popArray( TempMemory, uint8, nCell );
}

internal void
OutputTerrainToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS Stat = Editor->Stat;
    
    UINT32_PAIR MinCell = GetCellIndexFromPos( GetBL( Stat.Bound ) );
    UINT32_PAIR MaxCell = GetCellIndexFromPos( GetTR( Stat.Bound ) );
    MaxCell.x -= 1;
    MaxCell.y -= 1;
    
    for( uint32 Y = MinCell.y; Y <= MaxCell.y; Y++ ) {
        for( uint32 X = MinCell.x; X <= MaxCell.x; X++ ) {
            if( Editor->Grid[ Y ][ X ] == CellType_Terrain ) {
                rect R = GetCellRectFromIndex( X, Y );
                
                UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( R, Stat.Bound, xPixel, yPixel );
                OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, TERRAIN_COLOR );
            }
        }
    }
}