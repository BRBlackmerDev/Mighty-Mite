
internal void
UpdateCheckpoint( APP_STATE * AppState, flo32 dT ) {
}

internal void
DrawCheckpoint( RENDER_PASS * Pass_Game, APP_STATE * AppState ) {
    LEVEL_STATE      * LevelS      = &AppState->LevelS;
    CHECKPOINT_STATE * CheckpointS = &AppState->CheckpointS;
    
    for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
        CHECKPOINT Checkpoint = CheckpointS->Checkpoint[ iCheckpoint ];
        
        vec4 Color = ToColor( 40, 40, 0 );
        
        rect A = Checkpoint.Bound;
        A.Top = A.Bottom + TILE_HEIGHT * 0.1f;
        
        rect B = Checkpoint.Bound;
        B.Left  += TILE_WIDTH * 0.45f;
        B.Right -= TILE_WIDTH * 0.45f;
        
        DrawRect( Pass_Game, A, Color );
        DrawRect( Pass_Game, B, Color );
        
        if( ( LevelS->iCurrentLevel == LevelS->Checkpoint_iLevel ) && ( iCheckpoint == LevelS->Checkpoint_iCheckpoint ) ) {
            rect C = Checkpoint.Bound;
            C.Bottom = C.Top - TILE_HEIGHT * 0.7f;
            C.Left  += TILE_WIDTH * 0.55f;
            C.Right += TILE_WIDTH * 0.45f;
            
            DrawRect( Pass_Game, C, ToColor( 255, 255, 0 ) );
        }
        
    }
}

internal void
DrawCheckpointDebug( RENDER_PASS * Pass_Game, APP_STATE * AppState, DRAW_STATE * Draw ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    CHECKPOINT_STATE        * CheckpointS      = &AppState->CheckpointS;
    
    for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
        CHECKPOINT Checkpoint = CheckpointS->Checkpoint[ iCheckpoint ];
        
        char Num[ 8 ] = {};
        sprintf( Num, "%u", iCheckpoint );
        DrawString( Pass_Game, AppState->Font, Num, GetTC( Checkpoint.Bound ), TextAlign_BottomCenter, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
    }
}

internal void
FinalizeCheckpoint( APP_STATE * AppState ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    CHECKPOINT_STATE        * CheckpointS      = &AppState->CheckpointS;
    
    PLAYER_STATE * Player  = &AppState->Player;
    vec2           PlayerP = Player->Position;
    
    for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
        CHECKPOINT Checkpoint = CheckpointS->Checkpoint[ iCheckpoint ];
        if( IsInBound( PlayerP, Checkpoint.Bound ) ) {
            if( ( LevelS->Checkpoint_iLevel != LevelS->iCurrentLevel ) || ( LevelS->Checkpoint_iCheckpoint != iCheckpoint ) ) {
                LevelS->Checkpoint_iLevel      = LevelS->iCurrentLevel;
                LevelS->Checkpoint_iCheckpoint = iCheckpoint;
                
                PlayAudio( AppState, AudioID_Sound_Checkpoint, 0.25f );
            }
        }
    }
}

internal void
Editor_MoveCheckpoint( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__CHECKPOINT_STATE * CheckpointS = &Editor->CheckpointS;
    
    for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
        EDITOR__CHECKPOINT * Checkpoint = CheckpointS->Checkpoint + iCheckpoint;
        
        if( DoesRectIntersectRectExc( SrcBound, Checkpoint->Bound ) ) {
            Checkpoint->Bound = AddOffset( Checkpoint->Bound, Offset );
        }
    }
}

internal void
Editor_MirrorCheckpoint( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__CHECKPOINT_STATE * CheckpointS = &Editor->CheckpointS;
    
    for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
        EDITOR__CHECKPOINT * Checkpoint = CheckpointS->Checkpoint + iCheckpoint;
        
        vec2 Center = GetCenter( Checkpoint->Bound );
        vec2 Dim    = GetDim   ( Checkpoint->Bound );
        
        if( IsInBound( Center, SrcBound ) ) {
            Center.x = ( SrcBound.Right ) - ( Center.x - SrcBound.Left );
            Checkpoint->Bound = RectCD( Center, Dim );
        }
    }
}

#define VERSION__CHECKPOINT  ( 1 )
#define FILETAG__CHECKPOINT  ( "CHECKPNT" )
internal void
SaveCheckpoint( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CHECKPOINT;
    uint32 Version = VERSION__CHECKPOINT;
    EDITOR__CHECKPOINT_STATE * CheckpointS = &Editor->CheckpointS;
    
    if( CheckpointS->nCheckpoint > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, CheckpointS->nCheckpoint );
        for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
            EDITOR__CHECKPOINT Checkpoint = CheckpointS->Checkpoint[ iCheckpoint ];
            _writem( output, EDITOR__CHECKPOINT, Checkpoint );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__CHECKPOINT
ReadCheckpoint( uint32 Version, uint8 ** Ptr ) {
    EDITOR__CHECKPOINT Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect Bound = _read( ptr, rect  );
            
            EDITOR__CHECKPOINT Checkpoint = {};
            Checkpoint.Bound     = Bound;
            
            Result = Checkpoint;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadCheckpoint( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CHECKPOINT;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        CHECKPOINT_STATE * CheckpointS = &AppState->CheckpointS;
        CheckpointS->nCheckpoint = _read( ptr, uint32 );
        
        for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
            EDITOR__CHECKPOINT Src = ReadCheckpoint( Version, &ptr );
            
            CHECKPOINT Checkpoint = {};
            Checkpoint.Bound = Src.Bound;
            
            CheckpointS->Checkpoint[ iCheckpoint ] = Checkpoint;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadCheckpoint( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CHECKPOINT;
    EDITOR_STATE             * Editor      = &AppState->Editor;
    EDITOR__CHECKPOINT_STATE * CheckpointS = &Editor->CheckpointS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        CheckpointS->nCheckpoint = _read( ptr, uint32 );
        for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
            EDITOR__CHECKPOINT Src = ReadCheckpoint( Version, &ptr );
            CheckpointS->Checkpoint[ iCheckpoint ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}