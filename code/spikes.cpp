
internal SPIKES
ToSpikes( UINT32_PAIR Cell, SPIKES_TYPE Type ) {
    rect R = GetCellRectFromIndex( Cell );
    
    flo32 Tab      = TILE_WIDTH  * 0.2f;
    flo32 HOffsetX = TILE_WIDTH  * 0.15f;
    flo32 HOffsetY = TILE_HEIGHT * 0.1f;
    flo32 VOffsetX = PLAYER_HALF_WIDTH + TILE_WIDTH * 0.5f - TILE_WIDTH * 0.1f;
    flo32 VOffsetYBottom = TILE_HEIGHT * 0.3f; // TODO: Tune this!!
    flo32 VOffsetYTop    = 0.0f;
    
    SPIKES Spikes = {};
    
    switch( Type ) {
        case SpikesType_PointLeft: {
            Spikes.Art_Origin  = GetBR( R );
            Spikes.Art_Radians = PI * 0.5f;
            
            rect Bound   = {};
            Bound.Left   = R.Right - VOffsetX;
            Bound.Bottom = R.Bottom - PLAYER_HEIGHT + VOffsetYBottom;
            Bound.Right  = R.Right;
            Bound.Top    = R.Top - Tab;
            Spikes.Bound_Dmg = Bound;
        } break;
        
        case SpikesType_PointDown: {
            Spikes.Art_Origin  = GetTR( R );
            Spikes.Art_Radians = PI * 1.0f;
            
            rect Bound   = {};
            Bound.Left   = R.Left - HOffsetX;
            Bound.Bottom = R.Top - ( PLAYER_HEIGHT + TILE_HEIGHT * 0.5f );
            Bound.Right  = R.Right + HOffsetX;
            Bound.Top    = R.Top;
            Spikes.Bound_Dmg = Bound;
        } break;
        
        case SpikesType_PointRight: {
            Spikes.Art_Origin  = GetTL( R );
            Spikes.Art_Radians = PI * 1.5f;
            
            rect Bound   = {};
            Bound.Left   = R.Left;
            Bound.Bottom = R.Bottom - PLAYER_HEIGHT + VOffsetYBottom;
            Bound.Right  = R.Left + VOffsetX;
            Bound.Top    = R.Top - Tab;
            Spikes.Bound_Dmg = Bound;
        } break;
        
        case SpikesType_PointUp: {
            Spikes.Art_Origin  = GetBL( R );
            Spikes.Art_Radians = 0.0f;
            
            rect Bound   = {};
            Bound.Left   = R.Left - HOffsetX;
            Bound.Bottom = R.Bottom;
            Bound.Right  = R.Right + HOffsetX;
            Bound.Top    = R.Bottom + ( TILE_HEIGHT * 0.5f ) - HOffsetY;
            Spikes.Bound_Dmg = Bound;
        } break;
    }
    
    return Spikes;
}

internal boo32
IsInSpikesKillBound( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    SPIKES_STATE * SpikesS = &AppState->SpikesS;
    
    boo32 Result = false;
    
    for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
        SPIKES Spikes = SpikesS->Spikes[ iSpikes ];
        
        // TODO: I will probably need to fix this! Bound_Dmg does not cover the full spikes art, and there is currently no clean way to get that bound. This solution is most likely temporary.
        rect Bound = MinkSub( Spikes.Bound_Dmg, CollBound );
        
        if( IsInBound( Pos, Bound ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal boo32
IsPosInSpikes( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    SPIKES_STATE * SpikesS = &AppState->SpikesS;
    
    boo32 Result = false;
    
    for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
        SPIKES Spikes = SpikesS->Spikes[ iSpikes ];
        
        // TODO: I will probably need to fix this! Bound_Dmg does not cover the full spikes art, and there is currently no clean way to get that bound. This solution is most likely temporary.
        rect Bound = MinkSub( Spikes.Bound_Dmg, CollBound );
        
        if( IsInBound( Pos, Bound ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal void
UpdateSpikes( APP_STATE * AppState, flo32 dT ) {
}

internal void
DrawSpikes( RENDER_PASS * Pass, APP_STATE * AppState ) {
    SPIKES_STATE * SpikesS = &AppState->SpikesS;
    
    for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
        SPIKES Spikes = SpikesS->Spikes[ iSpikes ];
        DrawModel( Pass, ModelID_Spikes, TextureID_WhiteTexture, Spikes.Art_Origin, TILE_DIM, Spikes.Art_Radians, COLOR_GRAY( 0.5f ) );
    }
}

internal void
DrawSpikesDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    SPIKES_STATE * SpikesS = &AppState->SpikesS;
    
    for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
        SPIKES Spikes = SpikesS->Spikes[ iSpikes ];
        DrawRectOutline( Pass, Spikes.Bound_Dmg, COLOR_YELLOW );
    }
}

internal void
FinalizeSpikes( APP_STATE * AppState ) {
    
}

internal void
Editor_DrawSpikes( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__SPIKES_STATE * SpikesS = &Editor->SpikesS;
    
    for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
        EDITOR__SPIKES Spikes = SpikesS->Spikes[ iSpikes ];
        
        vec2  Origin  = GetBL( Spikes.Bound );
        flo32 Radians = 0.0f;
        
        switch( Spikes.Type ) {
            case SpikesType_PointLeft: {
                Origin  = GetBR( Spikes.Bound );
                Radians = PI * 0.5f;
            } break;
            
            case SpikesType_PointDown: {
                Origin  = GetTR( Spikes.Bound );
                Radians = PI * 1.0f;
            } break;
            
            case SpikesType_PointRight: {
                Origin  = GetTL( Spikes.Bound );
                Radians = PI * 1.5f;
            } break;
        }
        
        DrawModel( Pass, ModelID_Spikes, TextureID_WhiteTexture, Origin, TILE_DIM, Radians, COLOR_GRAY( 0.5f ) );
    }
}

internal void
Editor_MoveSpikes( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__SPIKES_STATE * SpikesS = &Editor->SpikesS;
    
    for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
        EDITOR__SPIKES * Spikes = SpikesS->Spikes + iSpikes;
        
        if( DoesRectIntersectRectExc( SrcBound, Spikes->Bound ) ) {
            Spikes->Bound = AddOffset( Spikes->Bound, Offset );
        }
    }
}

internal void
Editor_MirrorSpikes( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__SPIKES_STATE * SpikesS = &Editor->SpikesS;
    
    for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
        EDITOR__SPIKES * Spikes = SpikesS->Spikes + iSpikes;
        
        vec2 Center = GetCenter( Spikes->Bound );
        vec2 Dim    = GetDim   ( Spikes->Bound );
        
        if( IsInBound( Center, SrcBound ) ) {
            Center.x = ( SrcBound.Right ) - ( Center.x - SrcBound.Left );
            Spikes->Bound = RectCD( Center, Dim );
        }
    }
}

#define VERSION__SPIKES  ( 1 )
#define FILETAG__SPIKES  ( "SPIKES__" )
internal void
SaveSpikes( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SPIKES;
    uint32 Version = VERSION__SPIKES;
    EDITOR__SPIKES_STATE * SpikesS = &Editor->SpikesS;
    
    if( SpikesS->nSpikes > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, SpikesS->nSpikes );
        for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
            EDITOR__SPIKES Spikes = SpikesS->Spikes[ iSpikes ];
            _writem( output, EDITOR__SPIKES, Spikes );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__SPIKES
ReadSpikes( uint32 Version, uint8 ** Ptr ) {
    EDITOR__SPIKES Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            SPIKES_TYPE Type  = _read( ptr, SPIKES_TYPE );
            rect        Bound = _read( ptr, rect );
            
            EDITOR__SPIKES Spikes = {};
            Spikes.Type  = Type;
            Spikes.Bound = Bound;
            
            Result = Spikes;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadSpikes( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SPIKES;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        SPIKES_STATE * SpikesS = &AppState->SpikesS;
        
        SpikesS->nSpikes = _read( ptr, uint32 );
        for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
            EDITOR__SPIKES Src  = ReadSpikes( Version, &ptr );
            UINT32_PAIR    Cell = GetCellIndexFromPos( GetBL( Src.Bound ) );
            
            SPIKES Spikes = ToSpikes( Cell, Src.Type );
            SpikesS->Spikes[ iSpikes ] = Spikes;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadSpikes( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SPIKES;
    EDITOR_STATE         * Editor  = &AppState->Editor;
    EDITOR__SPIKES_STATE * SpikesS = &Editor->SpikesS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        SpikesS->nSpikes = _read( ptr, uint32 );
        for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
            EDITOR__SPIKES Src = ReadSpikes( Version, &ptr );
            SpikesS->Spikes[ iSpikes ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}