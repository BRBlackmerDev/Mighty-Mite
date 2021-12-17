
internal COLLAPSE_PLATFORM
CollapsePlatformC( rect Bound ) {
    flo32 MarginX = PLAYER_HALF_WIDTH;
    flo32 MarginY = TILE_HEIGHT * 0.1f;
    
    rect  Collapse_Bound = Rect( Bound.Left - MarginX, Bound.Top - MarginY, Bound.Right + MarginX, Bound.Top + MarginY );
    
    COLLAPSE_PLATFORM CollapsePlatform = {};
    CollapsePlatform.Bound              = Bound;
    CollapsePlatform.Collision_IsActive = true;
    CollapsePlatform.Collapse_Bound     = Collapse_Bound;
    CollapsePlatform.Art_Radians        = RandomF32InRange( -2.0f, 2.0f ) * PI / 180.0f;
    return CollapsePlatform;
}

internal void
AddCollapsePlatform( APP_STATE * AppState, rect Bound ) {
    COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &AppState->CollapsePlatformS;
    
    if( CollapsePlatformS->nCollapsePlatform < COLLAPSE_PLATFORM_MAX_COUNT ) {
        COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformC( Bound );
        CollapsePlatformS->CollapsePlatform[ CollapsePlatformS->nCollapsePlatform++ ] = CollapsePlatform;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new COLLAPSE_PLATFORM, but the state is full! Max Count = %u", COLLAPSE_PLATFORM_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
AddCollapsePlatform( APP_STATE * AppState, UINT32_PAIR Cell, uint32 CellWidth, uint32 CellHeight ) {
    vec2 P   = GetCellRectBLFromIndex( Cell );
    vec2 Dim = TILE_DIM * Vec2( ( flo32 )CellWidth, ( flo32 )CellHeight );
    rect R   = RectBLD( P, Dim );
    
    AddCollapsePlatform( AppState, R );
}

internal boo32
IsPosOnCollapsePlatform( APP_STATE * AppState, vec2 Pos ) {
    COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &AppState->CollapsePlatformS;
    
    boo32 Result = false;
    
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ];
        
        if( ( CollapsePlatform.Collision_IsActive ) && ( IsInBound( Pos, CollapsePlatform.Collapse_Bound ) ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal void
UpdateCollapsePlatform( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE            * Player            = &AppState->Player;
    COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &AppState->CollapsePlatformS;
    
    vec2 PlayerP = Player->Position;
    
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        COLLAPSE_PLATFORM * CollapsePlatform = CollapsePlatformS->CollapsePlatform + iCollapsePlatform;
        
        if( ( CollapsePlatform->Collision_IsActive ) && ( IsInBound( PlayerP, CollapsePlatform->Collapse_Bound ) ) ) {
            CollapsePlatform->Collapse_IsActive = true;
        }
        if( CollapsePlatform->Collapse_IsActive ) {
            CollapsePlatform->Collapse_Timer += dT;
        }
        if( !CollapsePlatform->Collision_IsActive ) {
            CollapsePlatform->Collapse_Timer += dT;
        }
    }
}

internal void
DrawCollapsePlatform( RENDER_PASS * Pass, COLLAPSE_PLATFORM CollapsePlatform ) {
    vec2 P   = GetCenter( CollapsePlatform.Bound );
    vec2 Dim = GetDim( CollapsePlatform.Bound );
    
    flo32 Radians = CollapsePlatform.Art_Radians;
    if( ( CollapsePlatform.Collapse_IsActive ) && ( CollapsePlatform.Collapse_Timer < ( 12.0f / 60.0f ) ) ) {
        Radians += ( RandomF32InRange( -3.0f, 3.0f ) * PI / 180.0f );
    }
    
    vec4 OutlineColor = COLOR_GRAY( 0.5f );
    
    orect Bound = ORectCD( P, Dim, Radians );
    if( CollapsePlatform.Collision_IsActive ) {
        OutlineColor = ToColor( 50, 50, 10 );
        DrawORect( Pass, Bound, ToColor( 80, 70, 20 ) );
    }
    DrawORectOutline( Pass, Bound, OutlineColor );
}

internal void
DrawCollapsePlatform( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    COLLAPSE_PLATFORM_STATE * CollapsePlatformS = Draw->CollapsePlatformS;
    
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ];
        DrawCollapsePlatform( Pass, CollapsePlatform );
    }
}

internal void
DrawCollapsePlatformDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    COLLAPSE_PLATFORM_STATE * CollapsePlatformS = Draw->CollapsePlatformS;
    
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ];
        
        if( CollapsePlatform.Collision_IsActive ) {
            vec4 Color = COLOR_YELLOW;
            if( CollapsePlatform.Collapse_IsActive ) {
                Color = COLOR_RED;
            }
            DrawRectOutline( Pass, CollapsePlatform.Collapse_Bound, Color );
        }
    }
}

internal void
FinalizeCollapsePlatform( APP_STATE * AppState ) {
    COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &AppState->CollapsePlatformS;
    
    flo32 TargetTime = COLLAPSE_PLATFORM_COLLAPSE_TARGET_TIME;
    
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        COLLAPSE_PLATFORM * CollapsePlatform = CollapsePlatformS->CollapsePlatform + iCollapsePlatform;
        
        if( CollapsePlatform->Collision_IsActive ) {
            if( CollapsePlatform->Collapse_Timer >= COLLAPSE_PLATFORM_COLLAPSE_TARGET_TIME ) {
                CollapsePlatform->Collision_IsActive = false;
                CollapsePlatform->Collapse_Timer     = 0.0f;
                CollapsePlatform->Collapse_IsActive  = false;
            }
        } else {
            if( CollapsePlatform->Collapse_Timer >= COLLAPSE_PLATFORM_DOWN_TARGET_TIME ) {
                CollapsePlatform->Collision_IsActive = true;
                CollapsePlatform->Collapse_Timer     = 0.0f;
                CollapsePlatform->Collapse_IsActive  = false;
            }
        }
        
        // TODO: else add to particle system
    }
}

internal void
Editor_DrawCollapsePlatform( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &Editor->CollapsePlatformS;
    
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        EDITOR__COLLAPSE_PLATFORM Src = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ];
        COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformC( Src.Bound );
        CollapsePlatform.Art_Radians = ( 2.0f * PI / 180.0f );
        DrawCollapsePlatform( Pass, CollapsePlatform );
    }
}

internal void
Editor_MoveCollapsePlatform( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &Editor->CollapsePlatformS;
    
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        EDITOR__COLLAPSE_PLATFORM * CollapsePlatform = CollapsePlatformS->CollapsePlatform + iCollapsePlatform;
        
        if( DoesRectIntersectRectExc( SrcBound, CollapsePlatform->Bound ) ) {
            CollapsePlatform->Bound = AddOffset( CollapsePlatform->Bound, Offset );
        }
    }
}

internal void
Editor_MirrorCollapsePlatform( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &Editor->CollapsePlatformS;
    
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        EDITOR__COLLAPSE_PLATFORM * CollapsePlatform = CollapsePlatformS->CollapsePlatform + iCollapsePlatform;
        
        vec2 Center = GetCenter( CollapsePlatform->Bound );
        vec2 Dim    = GetDim   ( CollapsePlatform->Bound );
        
        if( IsInBound( Center, SrcBound ) ) {
            Center.x = ( SrcBound.Right ) - ( Center.x - SrcBound.Left );
            CollapsePlatform->Bound = RectCD( Center, Dim );
        }
    }
}

#define VERSION__COLLAPSE_PLATFORM  ( 1 )
#define FILETAG__COLLAPSE_PLATFORM  ( "COLLAPSE" )
internal void
SaveCollapsePlatform( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__COLLAPSE_PLATFORM;
    uint32 Version = VERSION__COLLAPSE_PLATFORM;
    EDITOR__COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &Editor->CollapsePlatformS;
    
    if( CollapsePlatformS->nCollapsePlatform > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, CollapsePlatformS->nCollapsePlatform );
        for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
            EDITOR__COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ];
            _writem( output, EDITOR__COLLAPSE_PLATFORM, CollapsePlatform );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__COLLAPSE_PLATFORM
ReadCollapsePlatform( uint32 Version, uint8 ** Ptr ) {
    EDITOR__COLLAPSE_PLATFORM Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect Bound = _read( ptr, rect );
            
            EDITOR__COLLAPSE_PLATFORM CollapsePlatform = {};
            CollapsePlatform.Bound = Bound;
            
            Result = CollapsePlatform;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadCollapsePlatform( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__COLLAPSE_PLATFORM;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &AppState->CollapsePlatformS;
        
        CollapsePlatformS->nCollapsePlatform = _read( ptr, uint32 );
        for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
            EDITOR__COLLAPSE_PLATFORM Src = ReadCollapsePlatform( Version, &ptr );
            
            COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformC( Src.Bound );
            CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ] = CollapsePlatform;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadCollapsePlatform( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__COLLAPSE_PLATFORM;
    EDITOR_STATE           * Editor    = &AppState->Editor;
    EDITOR__COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &Editor->CollapsePlatformS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        CollapsePlatformS->nCollapsePlatform = _read( ptr, uint32 );
        for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
            EDITOR__COLLAPSE_PLATFORM Src = ReadCollapsePlatform( Version, &ptr );
            CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputCollapsePlatformToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS                 Stat       = Editor->Stat;
    EDITOR__COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &Editor->CollapsePlatformS;
    
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        EDITOR__COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ];
        
        vec4 Color = ToColor( 80, 70, 20 );
        rect Bound = CollapsePlatform.Bound;
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}
