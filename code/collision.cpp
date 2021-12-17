
internal boo32
IsPosInJumpBound( APP_STATE * AppState, vec2 Pos, flo32 OffsetX ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    boo32 Result = false;
    for( uint32 iJumpBound = 0; iJumpBound < Collision->nJumpBound; iJumpBound++ ) {
        rect R = Collision->JumpBound[ iJumpBound ];
        R.Left  -= OffsetX;
        R.Right += OffsetX;
        if( IsInBound( Pos, R ) ) {
            Result = true;
        }
    }
    return Result;
}

internal void
DrawCollisionEdges( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    for( uint32 iEdge = 0; iEdge < Collision->nEdge; iEdge++ ) {
        vec4 Edge = Collision->Edge[ iEdge ];
        DrawLine( Pass, Edge.xy, Edge.zw, COLOR_YELLOW );
    }
}

internal void
DrawJumpBound( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    for( uint32 iBound = 0; iBound < Collision->nJumpBound; iBound++ ) {
        rect R = Collision->JumpBound[ iBound ];
        DrawRectOutline( Pass, R, COLOR_RED );
    }
}

internal void
DrawWallSlideBound( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    for( uint32 iBound = 0; iBound < Collision->nWallSlideLeft; iBound++ ) {
        rect R = Collision->WallSlideLeft[ iBound ];
        DrawRectOutline( Pass, R, COLOR_BLUE );
    }
    
    for( uint32 iBound = 0; iBound < Collision->nWallSlideRight; iBound++ ) {
        rect R = Collision->WallSlideRight[ iBound ];
        DrawRectOutline( Pass, R, COLOR_BLUE );
    }
}

#define VERSION__COLLISION_TERRAIN  ( 1 )
#define FILETAG__COLLISION_TERRAIN  ( "COLLTERR" )
internal void
SaveCollisionTerrain( PLATFORM * Platform, COLLISION_STATE * Collision, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__COLLISION_TERRAIN;
    uint32 Version = VERSION__COLLISION_TERRAIN;
    if( Collision->nEdge > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        for( uint32 iType = 0; iType < 4; iType++ ) {
            _writem( output, UINT32_PAIR, Collision->Type[ iType ] );
        }
        
        _writem( output, uint32, Collision->nEdge );
        _writeb( output, Collision->Edge, sizeof( vec4 ) * Collision->nEdge );
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

#define VERSION__COLLISION_JUMP     ( 1 )
#define FILETAG__COLLISION_JUMP     ( "COLLJUMP" )
internal void
SaveCollisionJump( PLATFORM * Platform, COLLISION_STATE * Collision, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__COLLISION_JUMP;
    uint32 Version = VERSION__COLLISION_JUMP;
    uint32 nEntity = Collision->nJumpBound;
    uint8 * Entity = ( uint8 * )Collision->JumpBound;
    uint32    size = sizeof( rect ) * nEntity;
    
    if( nEntity > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, nEntity );
        _writeb( output, Entity, size );
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

#define VERSION__WALL_SLIDE     ( 1 )
#define FILETAG__WALL_SLIDE     ( "WALLSLID" )
internal void
SaveWallSlide( PLATFORM * Platform, COLLISION_STATE * Collision, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__WALL_SLIDE;
    uint32 Version = VERSION__WALL_SLIDE;
    
    uint32 nEntityA = Collision->nWallSlideLeft;
    uint8 * EntityA = ( uint8 * )Collision->WallSlideLeft;
    uint32    sizeA = sizeof( rect ) * nEntityA;
    uint32 nEntityB = Collision->nWallSlideRight;
    uint8 * EntityB = ( uint8 * )Collision->WallSlideRight;
    uint32    sizeB = sizeof( rect ) * nEntityB;
    
    if( ( nEntityA > 0 ) || ( nEntityB > 0 ) ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, nEntityA );
        _writem( output, uint32, nEntityB );
        _writeb( output, EntityA, sizeA );
        _writeb( output, EntityB, sizeB );
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal void
GAME_LoadCollisionTerrain( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__COLLISION_TERRAIN;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        switch( Version ) {
            case 1: {
                COLLISION_STATE * Collision = &AppState->Collision;
                for( uint32 iType = 0; iType < 4; iType++ ) {
                    Collision->Type[ iType ] = _read( ptr, UINT32_PAIR );
                }
                Collision->nEdge = _read( ptr, uint32 );
                Collision->Edge  = _copya( &AppState->Collision_Memory, ptr, vec4, Collision->nEdge );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
GAME_LoadCollisionJump( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__COLLISION_JUMP;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        switch( Version ) {
            case 1: {
                COLLISION_STATE * Collision = &AppState->Collision;
                Collision->nJumpBound = _read( ptr, uint32 );
                Collision->JumpBound  = _copya( &AppState->Collision_Memory, ptr, rect, Collision->nJumpBound );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
GAME_LoadWallSlide( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__WALL_SLIDE;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        switch( Version ) {
            case 1: {
                COLLISION_STATE * Collision = &AppState->Collision;
                Collision->nWallSlideLeft  = _read( ptr, uint32 );
                Collision->nWallSlideRight = _read( ptr, uint32 );
                Collision->WallSlideLeft = _copya( &AppState->Collision_Memory, ptr, rect, Collision->nWallSlideLeft );
                Collision->WallSlideRight = _copya( &AppState->Collision_Memory, ptr, rect, Collision->nWallSlideRight );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, File.size );
    }
}