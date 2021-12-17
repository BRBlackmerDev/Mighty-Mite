
internal SCAFFOLD
ScaffoldC( rect CellBound ) {
    rect R = Rect( CellBound.Left, CellBound.Top - SCAFFOLD_ART_THICKNESS, CellBound.Right, CellBound.Top );
    vec2 P = GetTL( R );
    vec2 Q = GetTR( R );
    
    flo32 OffsetBottom = TILE_HEIGHT * 0.5f;
    flo32 OffsetTop    = TILE_HEIGHT * 0.1f;
    
    rect S = {};
    S.Left   = P.x - PLAYER_HALF_WIDTH;
    S.Bottom = P.y - OffsetTop;
    S.Right  = Q.x + PLAYER_HALF_WIDTH;
    S.Top    = P.y + OffsetTop;
    
    rect T = {};
    T.Left   = P.x + PLAYER_HALF_WIDTH;
    T.Bottom = P.y - OffsetBottom;
    T.Right  = Q.x - PLAYER_HALF_WIDTH;
    T.Top    = P.y + OffsetTop;
    
    SCAFFOLD Scaffold = {};
    Scaffold.Bound_Art      = R;
    Scaffold.Bound_JumpUp   = S;
    Scaffold.Bound_JumpDown = T;
    Scaffold.CollisionEdge  = Vec4( P, Q );
    
    return Scaffold;
}

internal void
AddScaffold( APP_STATE * AppState, uint32 xCellA, uint32 xCellB, uint32 yCell ) {
    SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
    
    Assert( ScaffoldS->nScaffold < SCAFFOLD_MAX_COUNT );
    
    if( xCellA > xCellB ) {
        _swap( uint32, xCellA, xCellB );
    }
    
    rect BoundA = GetCellRectFromIndex( xCellA, yCell );
    rect BoundB = GetCellRectFromIndex( xCellB, yCell );
    rect R      = Rect( BoundA.Left, BoundA.Top, BoundB.Right, BoundA.Bottom );
    
    SCAFFOLD Scaffold = ScaffoldC( R );
    ScaffoldS->Scaffold[ ScaffoldS->nScaffold++ ] = Scaffold;
}

internal void
UpdateScaffold( APP_STATE * AppState, flo32 dT ) {
}

internal void
DrawScaffold( RENDER_PASS * Pass, APP_STATE * AppState ) {
    SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
    
    for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
        SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
        DrawRect( Pass, Scaffold.Bound_Art, COLOR_GRAY( 0.25f ) );
    }
}

internal void
DrawScaffoldDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
    
    for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
        SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
        DrawRectOutline( Pass, Scaffold.Bound_JumpUp,   COLOR_RED );
        DrawRectOutline( Pass, Scaffold.Bound_JumpDown, COLOR_CYAN );
    }
}

internal void
Editor_DrawScaffold( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__SCAFFOLD_STATE * ScaffoldS = &Editor->ScaffoldS;
    
    for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
        EDITOR__SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
        
        rect R = Scaffold.Bound;
        rect S = Rect( R.Left, R.Top - SCAFFOLD_ART_THICKNESS, R.Right, R.Top );
        
        DrawRect       ( Pass, S, COLOR_GRAY( 0.25f ) );
        DrawRectOutline( Pass, R, COLOR_GRAY( 0.5f  ) );
    }
}

internal void
FinalizeScaffold( APP_STATE * AppState ) {
    SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
    PLAYER_STATE   * Player    = &AppState->Player;
    
    vec2 PlayerP = Player->Position;
    
    if( Player->Scaffold_SkipCollision ) {
        SCAFFOLD Scaffold = ScaffoldS->Scaffold[ Player->Scaffold_iScaffold ];
        if( !IsInBound( PlayerP, Scaffold.Bound_JumpDown ) ) {
            Player->Scaffold_SkipCollision = false;
        }
    }
}


#define VERSION__SCAFFOLD  ( 1 )
#define FILETAG__SCAFFOLD  ( "SCAFFOLD" )
internal void
SaveScaffold( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SCAFFOLD;
    uint32 Version = VERSION__SCAFFOLD;
    EDITOR__SCAFFOLD_STATE * ScaffoldS = &Editor->ScaffoldS;
    
    if( ScaffoldS->nScaffold > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, ScaffoldS->nScaffold );
        for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
            EDITOR__SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
            _writem( output, EDITOR__SCAFFOLD, Scaffold );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__SCAFFOLD
ReadScaffold( uint32 Version, uint8 ** Ptr ) {
    EDITOR__SCAFFOLD Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect Bound = _read( ptr, rect );
            
            EDITOR__SCAFFOLD Scaffold = {};
            Scaffold.Bound = Bound;
            
            Result = Scaffold;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadScaffold( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SCAFFOLD;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
        
        ScaffoldS->nScaffold = _read( ptr, uint32 );
        for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
            EDITOR__SCAFFOLD Src = ReadScaffold( Version, &ptr );
            
            SCAFFOLD Scaffold = ScaffoldC( Src.Bound );
            ScaffoldS->Scaffold[ iScaffold ] = Scaffold;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadScaffold( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SCAFFOLD;
    EDITOR_STATE           * Editor    = &AppState->Editor;
    EDITOR__SCAFFOLD_STATE * ScaffoldS = &Editor->ScaffoldS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        ScaffoldS->nScaffold = _read( ptr, uint32 );
        for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
            EDITOR__SCAFFOLD Src = ReadScaffold( Version, &ptr );
            ScaffoldS->Scaffold[ iScaffold ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputScaffoldToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS               Stat      = Editor->Stat;
    EDITOR__SCAFFOLD_STATE * ScaffoldS = &Editor->ScaffoldS;
    
    for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
        EDITOR__SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
        
        vec4 Color = COLOR_GRAY( 0.25f );
        rect Bound = Scaffold.Bound;
        Bound.Bottom += TILE_HEIGHT * 0.5f;
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}