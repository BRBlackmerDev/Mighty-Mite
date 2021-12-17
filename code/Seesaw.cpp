
internal SEESAW
SeesawC( rect CellBound ) {
    flo32 MidX = ( CellBound.Left + CellBound.Right ) * 0.5f;
    
    SEESAW Seesaw = {};
    Seesaw.BlockA = Rect( CellBound.Left, CellBound.Bottom, MidX,            CellBound.Top );
    Seesaw.BlockB = Rect( MidX,           CellBound.Bottom, CellBound.Right, CellBound.Top );
    return Seesaw;
}

internal void
AddSeesaw( APP_STATE * AppState, UINT32_PAIR Cell ) {
    SEESAW_STATE * SeesawS = &AppState->SeesawS;
    
    if( SeesawS->nSeesaw < SEESAW_MAX_COUNT ) {
        vec2 P = GetCellRectBLFromIndex( Cell );
        rect R = RectBLD( P, SEESAW_DIM );
        
        SEESAW Seesaw = SeesawC( R );
        SeesawS->Seesaw[ SeesawS->nSeesaw++ ] = Seesaw;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new SEESAW, but the state is full! Max Count = %u", SEESAW_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
UpdateSeesaw( APP_STATE * AppState, flo32 dT ) {
}

internal void
DrawSeesaw( RENDER_PASS * Pass, SEESAW Seesaw ) {
    flo32 BlockUp_Radius   = TILE_WIDTH  * 0.1f;
    flo32 BlockDown_Height = TILE_HEIGHT * 0.25f;
    flo32 OffsetX          = TILE_WIDTH  * 0.1f;
    
    vec2 ArrowPos = {};
    rect BlockC   = {};
    
    rect BlockA = Seesaw.BlockA;
    rect BlockB = Seesaw.BlockB;
    
    BlockA = AddDim( BlockA, Vec2( -OffsetX, 0.0f ) );
    BlockB = AddDim( BlockB, Vec2( -OffsetX, 0.0f ) );
    
    if( Seesaw.BlockAIsUp ) {
        BlockC        = AddRadius( BlockA, -BlockUp_Radius );
        BlockC.Bottom = BlockA.Bottom;
        
        BlockB.Top = BlockB.Bottom + BlockDown_Height;
        
        ArrowPos = GetCenter( BlockA );
    } else {
        BlockC        = AddRadius( BlockB, -BlockUp_Radius );
        BlockC.Bottom = BlockB.Bottom;
        
        BlockA.Top = BlockA.Bottom + BlockDown_Height;
        
        ArrowPos = GetCenter( BlockB );
    }
    
    vec4 ColorA = ToColor( 50,  50,  60 );
    vec4 ColorB = ToColor( 100, 100, 120 );
    
    DrawRect( Pass, BlockA, ColorA );
    DrawRect( Pass, BlockB, ColorA );
    DrawRect( Pass, BlockC, ColorB );
    
    vec2 Scale = Vec2( 0.2f, 0.25f ) * TILE_DIM;
    DrawModel( Pass, ModelID_Triangle, TextureID_WhiteTexture, ArrowPos, Scale, PI, COLOR_RED );
}

internal void
DrawSeesaw( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    SEESAW_STATE * SeesawS = Draw->SeesawS;
    
    for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
        SEESAW Seesaw = SeesawS->Seesaw[ iSeesaw ];
        DrawSeesaw( Pass, Seesaw );
    }
}

internal void
DrawSeesawDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    SEESAW_STATE * SeesawS = Draw->SeesawS;
    
    //for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
    //SEESAW Seesaw = SeesawS->Seesaw[ iSeesaw ];
    //DrawRectOutline( Pass, Seesaw.Bound_Mvt, COLOR_RED );
    //}
}

internal void
Editor_DrawSeesaw( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__SEESAW_STATE * SeesawS = &Editor->SeesawS;
    
    for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
        EDITOR__SEESAW Src    = SeesawS->Seesaw[ iSeesaw ];
        SEESAW         Seesaw = SeesawC( Src.Bound );
        DrawSeesaw( Pass, Seesaw );
    }
}

internal void
FinalizeSeesaw( APP_STATE * AppState ) {
    SEESAW_STATE * SeesawS = &AppState->SeesawS;
    
    for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
        SEESAW * Seesaw = SeesawS->Seesaw + iSeesaw;
        
        rect LaunchBound = Seesaw->BlockA;
        rect DropBound   = Seesaw->BlockB;
        if( Seesaw->BlockAIsUp ) {
            _swap( rect, LaunchBound, DropBound );
        }
        
        boo32 DoLaunch = false;
        { // Check DoLaunch
            { // PLAYER
                PLAYER_STATE * Player = &AppState->Player;
                if( IsInBound( Player->Position, DropBound ) ) {
                    DoLaunch = true;
                    Player->Position.y = DropBound.Bottom;
                }
            }
            
            { // BOULDER
                BOULDER_STATE * BoulderS = &AppState->BoulderS;
                for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
                    BOULDER * Boulder = BoulderS->Boulder + iBoulder;
                    
                    vec2 P = Boulder->Position;
                    P.y -= BOULDER_RADIUS;
                    
                    if( IsInBound( P, DropBound ) ) {
                        DoLaunch = true;
                        Boulder->Position.y = DropBound.Bottom + BOULDER_RADIUS;
                    }
                }
            }
            
            { // STOMPER
                STOMPER_STATE * StomperS = &AppState->StomperS;
                for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
                    STOMPER * Stomper = StomperS->Stomper + iStomper;
                    
                    vec2 P = Stomper->Position;
                    if( IsInBound( P, DropBound ) ) {
                        DoLaunch = true;
                        Stomper->Position.y = DropBound.Bottom;
                    }
                }
            }
        }
        
        if( DoLaunch ) {
            { // PLAYER
                PLAYER_STATE * Player = &AppState->Player;
                if( IsInBound( Player->Position, LaunchBound ) ) {
                    Player->Velocity.y = SEESAW_LAUNCH_SPEED;
                    Player->Position.y = LaunchBound.Top;
                }
            }
            
            { // BOULDER
                BOULDER_STATE * BoulderS = &AppState->BoulderS;
                for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
                    BOULDER * Boulder = BoulderS->Boulder + iBoulder;
                    
                    vec2 P = Boulder->Position;
                    P.y -= BOULDER_RADIUS;
                    
                    if( IsInBound( P, LaunchBound ) ) {
                        Boulder->Velocity.y = SEESAW_LAUNCH_SPEED;
                        Boulder->Position.y = LaunchBound.Top + BOULDER_RADIUS;
                    }
                }
            }
            
            { // STOMPER
                STOMPER_STATE * StomperS = &AppState->StomperS;
                for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
                    STOMPER * Stomper = StomperS->Stomper + iStomper;
                    
                    vec2 P = Stomper->Position;
                    if( IsInBound( P, LaunchBound ) ) {
                        Stomper->Mode       = StomperMode_Idle;
                        Stomper->Timer      = 0.0f;
                        Stomper->Velocity.y = SEESAW_LAUNCH_SPEED;
                        Stomper->Position.y = LaunchBound.Top;
                    }
                }
            }
            
            Seesaw->BlockAIsUp = !Seesaw->BlockAIsUp;
        }
    }
}

#define VERSION__SEESAW  ( 1 )
#define FILETAG__SEESAW  ( "SEESAW__" )
internal void
SaveSeesaw( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SEESAW;
    uint32 Version = VERSION__SEESAW;
    EDITOR__SEESAW_STATE * SeesawS = &Editor->SeesawS;
    
    if( SeesawS->nSeesaw > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, SeesawS->nSeesaw );
        for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
            EDITOR__SEESAW Seesaw = SeesawS->Seesaw[ iSeesaw ];
            _writem( output, EDITOR__SEESAW, Seesaw );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__SEESAW
ReadSeesaw( uint32 Version, uint8 ** Ptr ) {
    EDITOR__SEESAW Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect Bound = _read( ptr, rect );
            
            EDITOR__SEESAW Seesaw = {};
            Seesaw.Bound = Bound;
            
            Result = Seesaw;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadSeesaw( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SEESAW;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        SEESAW_STATE * SeesawS = &AppState->SeesawS;
        
        SeesawS->nSeesaw = _read( ptr, uint32 );
        for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
            EDITOR__SEESAW Src = ReadSeesaw( Version, &ptr );
            
            SEESAW Seesaw = SeesawC( Src.Bound );
            SeesawS->Seesaw[ iSeesaw ] = Seesaw;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadSeesaw( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SEESAW;
    EDITOR_STATE           * Editor    = &AppState->Editor;
    EDITOR__SEESAW_STATE * SeesawS = &Editor->SeesawS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        SeesawS->nSeesaw = _read( ptr, uint32 );
        for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
            EDITOR__SEESAW Src = ReadSeesaw( Version, &ptr );
            SeesawS->Seesaw[ iSeesaw ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputSeesawToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS             Stat      = Editor->Stat;
    EDITOR__SEESAW_STATE * SeesawS = &Editor->SeesawS;
    
    for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
        EDITOR__SEESAW Seesaw = SeesawS->Seesaw[ iSeesaw ];
        
        rect  Bound = Seesaw.Bound;
        flo32 MidX  = ( Bound.Left + Bound.Right ) * 0.5f;
        flo32 LoY   = ( Bound.Bottom + TILE_HEIGHT * 0.25f );
        
        rect BoundA  = Bound;
        BoundA.Right = MidX;
        rect BoundB  = Bound;
        BoundB.Left  = MidX;
        BoundB.Top   = LoY;
        
        vec4 ColorA = ToColor( 100, 100, 120 );
        vec4 ColorB = ToColor(  50,  50,  60 );
        
        UINT32_QUAD PixelBoundA = GetPixelBoundOfEntityForLevelBitmap( BoundA, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBoundA, Pixel, xPixel, yPixel, ColorA );
        UINT32_QUAD PixelBoundB = GetPixelBoundOfEntityForLevelBitmap( BoundB, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBoundB, Pixel, xPixel, yPixel, ColorB );
    }
}
