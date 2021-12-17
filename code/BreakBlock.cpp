

internal BREAK_BLOCK
BreakBlockC( rect Bound ) {
    BREAK_BLOCK BreakBlock = {};
    BreakBlock.Position = GetCenter( Bound );
    BreakBlock.Dim      = GetDim( Bound );
    return BreakBlock;
}

internal void
AddBreakBlock( APP_STATE * AppState, rect Bound ) {
    BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
    
    if( BreakBlockS->nBreakBlock < BREAK_BLOCK_MAX_COUNT ) {
        BREAK_BLOCK BreakBlock = BreakBlockC( Bound );
        BreakBlockS->BreakBlock[ BreakBlockS->nBreakBlock++ ] = BreakBlock;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new BREAK_BLOCK, but the state is full! Max Count = %u", BREAK_BLOCK_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
AddBreakBlock( APP_STATE * AppState, UINT32_PAIR Cell, uint32 CellWidth, uint32 CellHeight ) {
    vec2 P   = GetCellRectBLFromIndex( Cell );
    vec2 Dim = TILE_DIM * Vec2( ( flo32 )CellWidth, ( flo32 )CellHeight );
    rect R   = RectBLD( P, Dim );
    
    AddBreakBlock( AppState, R );
}

internal boo32
IsPosOnBreakBlock( APP_STATE * AppState, vec2 Pos, flo32 OffsetX ) {
    BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
    
    boo32 Result = false;
    
    flo32 MarginY = TILE_HEIGHT * 0.1f;
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
        
        rect R = RectCD( BreakBlock.Position, BreakBlock.Dim );
        R.Left  -= OffsetX;
        R.Right += OffsetX;
        R.Bottom = R.Top - MarginY;
        R.Top   += MarginY;
        
        if( IsInBound( Pos, R ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal void
UpdateBreakBlock( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE      * Player      = &AppState->Player;
    BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
    
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        BREAK_BLOCK * BreakBlock = BreakBlockS->BreakBlock + iBreakBlock;
        
        flo32 Friction = 4.0f;
        
        rect CollBound = RectCD( Vec2( 0.0f, 0.0f ), BreakBlock->Dim );
        
        vec2 BreakBlockP = BreakBlock->Position + Vec2( 0.0f, -BreakBlock->Dim.y * 0.5f );
        CONVEYOR_RESULT ConveyorHor = IsPosOnConveyorHor( AppState, BreakBlockP, CollBound );
        
        vec2 Accel = {};
        Accel.x = -BreakBlock->Velocity.x * Friction;
        if( ConveyorHor.IsOnConveyor ) {
            Accel.x += ( ConveyorHor.ConveyorAccel.x * Friction );
        }
        Accel.y = -BREAK_BLOCK_GRAVITY;
        
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + BreakBlock->Velocity * dT;
        BreakBlock->Velocity += Accel * dT;
        
        vec2 PrevPos = BreakBlock->Position;
        
        COLLISION_RESULT Coll = CollisionResult( BreakBlock->Position, BreakBlock->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionLeft ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectCollisionRight( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectCollisionTop  ( &BestIntersect, AppState, Coll, CollBound );
            
            DoesBreakBlockIntersectBreakBlock( &BestIntersect, AppState, Coll, CollBound, iBreakBlock );
            // TODO: Push Block?
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        Coll.Pos += Coll.dPos;
        
        BreakBlock->Position = Coll.Pos;
        BreakBlock->Velocity = Coll.Vel;
        
        BreakBlock->Velocity.y = MaxValue( BreakBlock->Velocity.y, -18.0f );
        
        vec2 NewPos = BreakBlock->Position;
        vec2 ChangePos = NewPos - PrevPos;
        
        if( ChangePos.x != 0.0f ) {
            rect PlayerDim = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM );
            PlayerDim.Bottom -= ( TILE_HEIGHT * 0.1f );
            
            rect PushBound = MinkSub( RectCD( BreakBlock->Position, BreakBlock->Dim ), PlayerDim );
            if( IsInBound( Player->Position, PushBound ) ) {
                Player->Position.x += ChangePos.x;
            }
        }
        
        if( ( !Player->NoDamage ) && ( BreakBlock->Velocity.y < -2.0f ) ) {
            rect DamageBound = RectCD( BreakBlock->Position, BreakBlock->Dim );
            DamageBound.Top     = DamageBound.Bottom;
            DamageBound.Bottom -= ( PLAYER_HEIGHT - TILE_HEIGHT * 0.2f );
            
            if( IsInBound( Player->Position, DamageBound ) ) {
                DamagePlayer( AppState, 2 );
            }
        }
    }
    
    uint32 NewCount = 0;
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
        
        vec2 Dim = Vec2( BreakBlock.Dim.x - TILE_WIDTH * 0.8f, BreakBlock.Dim.y );
        rect SpikesBound = RectCD( Vec2( 0.0f, 0.0f ), Dim );
        
        boo32 DoKill = IsPosInSpikes( AppState, BreakBlock.Position, SpikesBound );
        if( !DoKill ) {
            BreakBlockS->BreakBlock[ NewCount++ ] = BreakBlock;
        }
    }
    BreakBlockS->nBreakBlock = NewCount;
}

internal void
DrawBreakBlock( RENDER_PASS * Pass, BREAK_BLOCK BreakBlock ) {
    rect R = RectCD( BreakBlock.Position, BreakBlock.Dim );
    
    DrawRect( Pass, R, ToColor( 50, 50, 50 ) );
    DrawRectOutline( Pass, R, COLOR_BLACK );
}

internal void
DrawBreakBlock( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    BREAK_BLOCK_STATE * BreakBlockS = Draw->BreakBlockS;
    
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
        DrawBreakBlock( Pass, BreakBlock );
    }
}

internal void
DrawBreakBlockDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    BREAK_BLOCK_STATE * BreakBlockS = Draw->BreakBlockS;
    
    flo32 MarginX = TILE_WIDTH * 0.1f;
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
        rect Bound = RectCD( BreakBlock.Position, BreakBlock.Dim );
        
        DrawRectOutline( Pass, Bound, COLOR_YELLOW );
    }
}

internal void
FinalizeBreakBlock( APP_STATE * AppState ) {
    BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
}

internal void
Editor_DrawBreakBlock( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__BREAK_BLOCK_STATE * BreakBlockS = &Editor->BreakBlockS;
    
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        EDITOR__BREAK_BLOCK Src        = BreakBlockS->BreakBlock[ iBreakBlock ];
        BREAK_BLOCK         BreakBlock = BreakBlockC( Src.Bound );
        DrawBreakBlock( Pass, BreakBlock );
    }
}

internal void
Editor_MoveBreakBlock( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__BREAK_BLOCK_STATE * BreakBlockS = &Editor->BreakBlockS;
    
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        EDITOR__BREAK_BLOCK * BreakBlock = BreakBlockS->BreakBlock + iBreakBlock;
        
        if( DoesRectIntersectRectExc( SrcBound, BreakBlock->Bound ) ) {
            BreakBlock->Bound = AddOffset( BreakBlock->Bound, Offset );
        }
    }
}

internal void
Editor_MirrorBreakBlock( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__BREAK_BLOCK_STATE * BreakBlockS = &Editor->BreakBlockS;
    
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        EDITOR__BREAK_BLOCK * BreakBlock = BreakBlockS->BreakBlock + iBreakBlock;
        
        vec2 Center = GetCenter( BreakBlock->Bound );
        vec2 Dim    = GetDim   ( BreakBlock->Bound );
        
        if( IsInBound( Center, SrcBound ) ) {
            Center.x = ( SrcBound.Right ) - ( Center.x - SrcBound.Left );
            BreakBlock->Bound = RectCD( Center, Dim );
        }
    }
}

#define VERSION__BREAK_BLOCK  ( 1 )
#define FILETAG__BREAK_BLOCK  ( "BREAKBLK" )
internal void
SaveBreakBlock( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__BREAK_BLOCK;
    uint32 Version = VERSION__BREAK_BLOCK;
    EDITOR__BREAK_BLOCK_STATE * BreakBlockS = &Editor->BreakBlockS;
    
    if( BreakBlockS->nBreakBlock > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, BreakBlockS->nBreakBlock );
        for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
            EDITOR__BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
            _writem( output, EDITOR__BREAK_BLOCK, BreakBlock );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__BREAK_BLOCK
ReadBreakBlock( uint32 Version, uint8 ** Ptr ) {
    EDITOR__BREAK_BLOCK Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect Bound = _read( ptr, rect );
            
            EDITOR__BREAK_BLOCK BreakBlock = {};
            BreakBlock.Bound = Bound;
            
            Result = BreakBlock;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadBreakBlock( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__BREAK_BLOCK;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
        
        BreakBlockS->nBreakBlock = _read( ptr, uint32 );
        for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
            EDITOR__BREAK_BLOCK Src = ReadBreakBlock( Version, &ptr );
            
            BREAK_BLOCK BreakBlock = BreakBlockC( Src.Bound );
            BreakBlockS->BreakBlock[ iBreakBlock ] = BreakBlock;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadBreakBlock( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__BREAK_BLOCK;
    EDITOR_STATE           * Editor    = &AppState->Editor;
    EDITOR__BREAK_BLOCK_STATE * BreakBlockS = &Editor->BreakBlockS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        BreakBlockS->nBreakBlock = _read( ptr, uint32 );
        for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
            EDITOR__BREAK_BLOCK Src = ReadBreakBlock( Version, &ptr );
            BreakBlockS->BreakBlock[ iBreakBlock ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputBreakBlockToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS                 Stat       = Editor->Stat;
    EDITOR__BREAK_BLOCK_STATE * BreakBlockS = &Editor->BreakBlockS;
    
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        EDITOR__BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
        
        vec4 Color = ToColor( 50, 50, 50 );
        rect Bound = BreakBlock.Bound;
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}
