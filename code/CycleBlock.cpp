
internal CYCLE_BLOCK
CycleBlockC( vec2 PosA, vec2 PosB, vec2 Dim ) {
    CYCLE_BLOCK CycleBlock = {};
    CycleBlock.Position   = PosA;
    CycleBlock.Dim        = Dim;
    CycleBlock.MoveVector = ( PosB - PosA ) / CYCLE_BLOCK_MOVE_TARGET_TIME;
    CycleBlock.PosA       = PosA;
    CycleBlock.PosB       = PosB;
    return CycleBlock;
}

internal CYCLE_BLOCK
CycleBlockC( rect Bound, vec2 Dest ) {
    vec2 PosA = GetCenter( Bound );
    vec2 PosB = Dest;
    
    CYCLE_BLOCK CycleBlock = {};
    CycleBlock.Position   = PosA;
    CycleBlock.Dim        = GetDim( Bound );
    CycleBlock.MoveVector = ( PosB - PosA ) / CYCLE_BLOCK_MOVE_TARGET_TIME;
    CycleBlock.PosA       = PosA;
    CycleBlock.PosB       = PosB;
    return CycleBlock;
}

internal void
AddCycleBlock( APP_STATE * AppState, vec2 PosA, vec2 PosB, vec2 Dim ) {
    CYCLE_BLOCK_STATE * CycleBlockS = &AppState->CycleBlockS;
    
    if( CycleBlockS->nCycleBlock < CYCLE_BLOCK_MAX_COUNT ) {
        CYCLE_BLOCK CycleBlock = CycleBlockC( PosA, PosB, Dim );
        CycleBlockS->CycleBlock[ CycleBlockS->nCycleBlock++ ] = CycleBlock;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new CYCLE_BLOCK, but the state is full! Max Count = %u", CYCLE_BLOCK_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
AddCycleBlock( APP_STATE * AppState, UINT32_PAIR Cell, uint32 CellWidth, uint32 CellHeight, int32 CellMoveY ) {
    vec2 P   = GetCellRectBLFromIndex( Cell );
    vec2 Dim = TILE_DIM * Vec2( ( flo32 )CellWidth, ( flo32 )CellHeight );
    rect R   = RectBLD( P, Dim );
    vec2 V   = Vec2( 0.0f, 1.0f ) * ( flo32 )CellMoveY;
    
    vec2 A = GetCenter( R );
    vec2 B = A + V;
    
    AddCycleBlock( AppState, A, B, Dim );
}

internal boo32
IsPosInCycleBlock( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    CYCLE_BLOCK_STATE * CycleBlockS = &AppState->CycleBlockS;
    
    boo32 Result = false;
    
    for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
        CYCLE_BLOCK CycleBlock = CycleBlockS->CycleBlock[ iCycleBlock ];
        
        rect R = MinkSub( RectCD( CycleBlock.Position, CycleBlock.Dim ), CollBound );
        if( IsInBound( Pos, R ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal void
UpdateCycleBlock( APP_STATE * AppState, flo32 dT ) {
    CYCLE_BLOCK_STATE * CycleBlockS = &AppState->CycleBlockS;
    
    for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
        CYCLE_BLOCK * CycleBlock = CycleBlockS->CycleBlock + iCycleBlock;
        
        flo32 TargetTimeA = CYCLE_BLOCK_MOVE_TARGET_TIME;
        flo32 TargetTimeB = TargetTimeA + CYCLE_BLOCK_HOLD_TARGET_TIME;
        
        CycleBlock->Timer += dT;
        if( CycleBlock->Timer <= TargetTimeA ) {
            vec2 dPos = CycleBlock->MoveVector * dT;
            CycleBlock->Position += dPos;
        }
    }
}

internal void
DrawCycleBlock( RENDER_PASS * Pass, CYCLE_BLOCK CycleBlock ) {
    rect R = RectCD( CycleBlock.Position, CycleBlock.Dim );
    rect S = AddRadius( R, -TILE_WIDTH * 0.1f );
    
    DrawRect( Pass, R, ToColor( 80, 0, 0 ) );
    DrawRect( Pass, S, ToColor( 30, 30, 30 ) );
}

internal void
DrawCycleBlock( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    CYCLE_BLOCK_STATE * CycleBlockS = Draw->CycleBlockS;
    
    for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
        CYCLE_BLOCK CycleBlock = CycleBlockS->CycleBlock[ iCycleBlock ];
        DrawCycleBlock( Pass, CycleBlock );
    }
}

internal void
DrawCycleBlockDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    CYCLE_BLOCK_STATE * CycleBlockS = Draw->CycleBlockS;
    
    //for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
    //CYCLE_BLOCK CycleBlock = CycleBlockS->CycleBlock[ iCycleBlock ];
    //DrawRectOutline( Pass, CycleBlock.Bound_Mvt, COLOR_RED );
    //}
}

internal void
FinalizeCycleBlock( APP_STATE * AppState ) {
    CYCLE_BLOCK_STATE * CycleBlockS = &AppState->CycleBlockS;
    
    for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
        CYCLE_BLOCK * CycleBlock = CycleBlockS->CycleBlock + iCycleBlock;
        
        flo32 TargetTimeA = CYCLE_BLOCK_MOVE_TARGET_TIME;
        flo32 TargetTimeB = TargetTimeA + CYCLE_BLOCK_HOLD_TARGET_TIME;
        
        if( CycleBlock->Timer >= TargetTimeB ) {
            CycleBlock->MoveVector = -CycleBlock->MoveVector;
            CycleBlock->Timer      = 0.0f;
        }
    }
}

internal void
Editor_MoveCycleBlock( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__CYCLE_BLOCK_STATE * CycleBlockS = &Editor->CycleBlockS;
    
    for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
        EDITOR__CYCLE_BLOCK * CycleBlock = CycleBlockS->CycleBlock + iCycleBlock;
        
        if( DoesRectIntersectRectExc( SrcBound, CycleBlock->Bound ) ) {
            CycleBlock->Bound  = AddOffset( CycleBlock->Bound, Offset );
            CycleBlock->Dest  += Offset;
        }
    }
}

internal void
Editor_MirrorCycleBlock( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__CYCLE_BLOCK_STATE * CycleBlockS = &Editor->CycleBlockS;
    
    for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
        EDITOR__CYCLE_BLOCK * CycleBlock = CycleBlockS->CycleBlock + iCycleBlock;
        
        vec2 Center = GetCenter( CycleBlock->Bound );
        vec2 Dim    = GetDim   ( CycleBlock->Bound );
        vec2 Dest   = CycleBlock->Dest;
        
        if( IsInBound( Center, SrcBound ) ) {
            Center.x = ( SrcBound.Right ) - ( Center.x - SrcBound.Left );
            Dest.x   = ( SrcBound.Right ) - ( Dest.x   - SrcBound.Left );
            CycleBlock->Bound = RectCD( Center, Dim );
            CycleBlock->Dest  = Dest;
        }
    }
}

internal void
Editor_DrawCycleBlock( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__CYCLE_BLOCK_STATE * CycleBlockS = &Editor->CycleBlockS;
    
    for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
        EDITOR__CYCLE_BLOCK Src        = CycleBlockS->CycleBlock[ iCycleBlock ];
        CYCLE_BLOCK         CycleBlock = CycleBlockC( Src.Bound, Src.Dest );
        DrawCycleBlock( Pass, CycleBlock );
        
        rect S = RectCD( CycleBlock.PosB, CycleBlock.Dim );
        DrawLine( Pass, CycleBlock.PosA, CycleBlock.PosB, COLOR_CYAN );
        DrawRectOutline( Pass, S, COLOR_CYAN );
    }
}

#define VERSION__CYCLE_BLOCK  ( 1 )
#define FILETAG__CYCLE_BLOCK  ( "CYCBLOCK" )
internal void
SaveCycleBlock( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CYCLE_BLOCK;
    uint32 Version = VERSION__CYCLE_BLOCK;
    EDITOR__CYCLE_BLOCK_STATE * CycleBlockS = &Editor->CycleBlockS;
    
    if( CycleBlockS->nCycleBlock > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, CycleBlockS->nCycleBlock );
        for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
            EDITOR__CYCLE_BLOCK CycleBlock = CycleBlockS->CycleBlock[ iCycleBlock ];
            _writem( output, EDITOR__CYCLE_BLOCK, CycleBlock );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__CYCLE_BLOCK
ReadCycleBlock( uint32 Version, uint8 ** Ptr ) {
    EDITOR__CYCLE_BLOCK Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect Bound = _read( ptr, rect );
            vec2 Dest  = _read( ptr, vec2 );
            
            EDITOR__CYCLE_BLOCK CycleBlock = {};
            CycleBlock.Bound = Bound;
            CycleBlock.Dest  = Dest;
            
            Result = CycleBlock;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadCycleBlock( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CYCLE_BLOCK;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        CYCLE_BLOCK_STATE * CycleBlockS = &AppState->CycleBlockS;
        
        CycleBlockS->nCycleBlock = _read( ptr, uint32 );
        for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
            EDITOR__CYCLE_BLOCK Src = ReadCycleBlock( Version, &ptr );
            
            CYCLE_BLOCK CycleBlock = CycleBlockC( Src.Bound, Src.Dest );
            CycleBlockS->CycleBlock[ iCycleBlock ] = CycleBlock;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadCycleBlock( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CYCLE_BLOCK;
    EDITOR_STATE           * Editor    = &AppState->Editor;
    EDITOR__CYCLE_BLOCK_STATE * CycleBlockS = &Editor->CycleBlockS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        CycleBlockS->nCycleBlock = _read( ptr, uint32 );
        for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
            EDITOR__CYCLE_BLOCK Src = ReadCycleBlock( Version, &ptr );
            CycleBlockS->CycleBlock[ iCycleBlock ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputCycleBlockToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS                  Stat        = Editor->Stat;
    EDITOR__CYCLE_BLOCK_STATE * CycleBlockS = &Editor->CycleBlockS;
    
    for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
        EDITOR__CYCLE_BLOCK CycleBlock = CycleBlockS->CycleBlock[ iCycleBlock ];
        
        vec4 Color = ToColor( 50, 50, 50 );
        rect Bound = CycleBlock.Bound;
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap       ( PixelBound, Pixel, xPixel, yPixel, ToColor( 30, 30, 30 ) );
        OutputRectOutlineToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, ToColor( 80,  0,  0 ) );
    }
}
