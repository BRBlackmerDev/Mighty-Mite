
internal HEDGEHOG
HedgehogC( vec2 Pos, vec2 Vel = {} ) {
    HEDGEHOG Hedgehog = {};
    Hedgehog.Position = Pos;
    Hedgehog.Velocity = Vel;
    
    return Hedgehog;
}

internal void
AddHedgehog( APP_STATE * AppState, HEDGEHOG Hedgehog ) {
    HEDGEHOG_STATE * HedgehogS = &AppState->HedgehogS;
    
    if( HedgehogS->nHedgehog < HEDGEHOG_MAX_COUNT ) {
        HedgehogS->Hedgehog[ HedgehogS->nHedgehog++ ] = Hedgehog;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new HEDGEHOG, but the state is full! Max Count = %u", HEDGEHOG_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemoveHedgehog( APP_STATE * AppState, uint32 iRemove ) {
    HEDGEHOG_STATE * HedgehogS = &AppState->HedgehogS;
    
    Assert( HedgehogS->nHedgehog > 0 );
    HedgehogS->Hedgehog[ iRemove ] = HedgehogS->Hedgehog[ --HedgehogS->nHedgehog ];
}

internal boo32
IsPosInHedgehog( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    HEDGEHOG_STATE * HedgehogS = &AppState->HedgehogS;
    
    boo32 Result = false;
    
    for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
        HEDGEHOG Hedgehog = HedgehogS->Hedgehog[ iHedgehog ];
        rect Bound = MinkSub( RectBCD( Hedgehog.Position, HEDGEHOG_DIM ), CollBound );
        
        if( IsInBound( Pos, Bound ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal void
UpdateHedgehog( APP_STATE * AppState, flo32 dT ) {
    HEDGEHOG_STATE * HedgehogS = &AppState->HedgehogS;
    
    for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
        HEDGEHOG * Hedgehog = HedgehogS->Hedgehog + iHedgehog;
        
        vec2 Dir = {};
        
        rect  CollBound  = RectBCD( Vec2( 0.0f, 0.0f ), HEDGEHOG_DIM );
        rect  IsOnGroundBound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( HEDGEHOG_WIDTH, 0.0f ) );
        boo32 IsOnGround = ( IsPosOnCollisionTop( AppState, Hedgehog->Position, IsOnGroundBound ) );
        
        flo32 Friction = HEDGEHOG_GROUND_FRICTION;
        if( !IsOnGround ) {
            Friction = HEDGEHOG_AIR_FRICTION;
        }
        
        vec2 Accel = {};
        Accel.x = -Hedgehog->Velocity.x * Friction;
        Accel.y = -HEDGEHOG_GRAVITY;
        
        CONVEYOR_RESULT ConveyorHor = IsPosOnConveyorHor( AppState, Hedgehog->Position, CollBound );
        if( ConveyorHor.IsOnConveyor ) {
            Accel.x += ( ConveyorHor.ConveyorAccel.x * Friction );
        }
        
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + Hedgehog->Velocity * dT;
        Hedgehog->Velocity += Accel * dT;
        
        COLLISION_RESULT Coll = CollisionResult( Hedgehog->Position, Hedgehog->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll( &BestIntersect, AppState, Coll, CollBound );
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        Coll.Pos += Coll.dPos;
        
        Hedgehog->Position = Coll.Pos;
        Hedgehog->Velocity = Coll.Vel;
    }
}

internal void
DrawHedgehog( RENDER_PASS * Pass, HEDGEHOG Hedgehog ) {
    vec2 Pos = Hedgehog.Position + Vec2( 0.0f, HEDGEHOG_HALF_HEIGHT );
    vec2 Dim = Vec2( HEDGEHOG_WIDTH + TILE_WIDTH * 0.4f, TILE_HEIGHT * 0.2f );
    
    DrawORect( Pass, ORectCD( Pos, Dim, PI *  9.0f / 24.0f ), ToColor(  70, 15, 15 ) );
    DrawORect( Pass, ORectCD( Pos, Dim, PI * 17.0f / 24.0f ), ToColor(  85, 25, 25 ) );
    DrawORect( Pass, ORectCD( Pos, Dim, PI *  1.0f / 24.0f ), ToColor( 100, 30, 30 ) );
}

internal void
DrawHedgehog( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    HEDGEHOG_STATE * HedgehogS = Draw->HedgehogS;
    
    for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
        HEDGEHOG Hedgehog = HedgehogS->Hedgehog[ iHedgehog ];
        DrawHedgehog( Pass, Hedgehog );
    }
}

internal void
DrawHedgehogDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    HEDGEHOG_STATE * HedgehogS = Draw->HedgehogS;
    
    for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
    }
}

internal void
FinalizeHedgehog( APP_STATE * AppState ) {
    HEDGEHOG_STATE * HedgehogS = &AppState->HedgehogS;
    
    uint32 NewCount = 0;
    for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
        HEDGEHOG * Hedgehog = HedgehogS->Hedgehog + iHedgehog;
        
        rect Bound = RectBCD( Vec2( 0.0f, 0.0f ), HEDGEHOG_DIM );
        boo32 IsOnSpikes = IsInSpikesKillBound( AppState, Hedgehog->Position, Bound );
        if( !IsOnSpikes ) {
            HedgehogS->Hedgehog[ NewCount++ ] = *Hedgehog;
        }
    }
    HedgehogS->nHedgehog = NewCount;
}

#if 0
internal void
Editor_DrawHedgehog( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__HEDGEHOG_STATE * HedgehogS = &Editor->HedgehogS;
    
    for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
        EDITOR__HEDGEHOG Src     = HedgehogS->Hedgehog[ iHedgehog ];
        HEDGEHOG         Hedgehog = HedgehogC( Src.Position );
        DrawHedgehog( Pass, Hedgehog );
    }
}


#define VERSION__HEDGEHOG  ( 1 )
#define FILETAG__HEDGEHOG  ( "HEDGEHOG_" )
internal void
SaveHedgehog( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir,
             char * FileName ) {
    char * FileTag = FILETAG__HEDGEHOG;
    uint32 Version = VERSION__HEDGEHOG;
    EDITOR__HEDGEHOG_STATE * HedgehogS = &Editor->HedgehogS;
    
    if( HedgehogS->nHedgehog > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, HedgehogS->nHedgehog );
        for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
            EDITOR__HEDGEHOG Hedgehog = HedgehogS->Hedgehog[ iHedgehog ];
            _writem( output, EDITOR__HEDGEHOG, Hedgehog );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__HEDGEHOG
ReadHedgehog( uint32 Version, uint8 ** Ptr ) {
    EDITOR__HEDGEHOG Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            vec2 Position = _read( ptr, vec2 );
            
            EDITOR__HEDGEHOG Hedgehog = {};
            Hedgehog.Position = Position;
            
            Result = Hedgehog;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadHedgehog( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__HEDGEHOG;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        HEDGEHOG_STATE * HedgehogS = &AppState->HedgehogS;
        
        HedgehogS->nHedgehog = _read( ptr, uint32 );
        for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
            EDITOR__HEDGEHOG Src = ReadHedgehog( Version, &ptr );
            
            HEDGEHOG Hedgehog = HedgehogC( Src.Position );
            HedgehogS->Hedgehog[ iHedgehog ] = Hedgehog;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadHedgehog( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__HEDGEHOG;
    EDITOR_STATE             * Editor      = &AppState->Editor;
    EDITOR__HEDGEHOG_STATE * HedgehogS = &Editor->HedgehogS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        HedgehogS->nHedgehog = _read( ptr, uint32 );
        for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
            EDITOR__HEDGEHOG Src = ReadHedgehog( Version, &ptr );
            HedgehogS->Hedgehog[ iHedgehog ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputHedgehogToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS              Stat      = Editor->Stat;
    EDITOR__HEDGEHOG_STATE * HedgehogS = &Editor->HedgehogS;
    
    for( uint32 iHedgehog = 0; iHedgehog < HedgehogS->nHedgehog; iHedgehog++ ) {
        EDITOR__HEDGEHOG Hedgehog = HedgehogS->Hedgehog[ iHedgehog ];
        
        vec4 Color = ToColor( 180, 90, 45 );
        rect Bound = RectBCD( Hedgehog.Position, HEDGEHOG_DIM );
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}
#endif