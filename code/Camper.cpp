
internal CAMPER
CamperC( vec2 Pos, rect Bound = {} ) {
    CAMPER Camper = {};
    Camper.Position    = Pos;
    Camper.HazardBound = Bound;
    
    return Camper;
}

internal void
AddCamper( APP_STATE * AppState, CAMPER Camper ) {
    CAMPER_STATE * CamperS = &AppState->CamperS;
    
    if( CamperS->nCamper < CAMPER_MAX_COUNT ) {
        CamperS->Camper[ CamperS->nCamper++ ] = Camper;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new CAMPER, but the state is full! Max Count = %u", CAMPER_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemoveCamper( APP_STATE * AppState, uint32 iRemove ) {
    CAMPER_STATE * CamperS = &AppState->CamperS;
    
    Assert( CamperS->nCamper > 0 );
    CamperS->Camper[ iRemove ] = CamperS->Camper[ --CamperS->nCamper ];
}

internal ENTITY_VALID_RESULT
IsInCamperPickUpBound( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    CAMPER_STATE * CamperS = &AppState->CamperS;
    
    ENTITY_VALID_RESULT Result = {};
    
    for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
        CAMPER Camper = CamperS->Camper[ iCamper ];
        
        vec2 Dim   = Vec2( CAMPER_WIDTH + ( TILE_WIDTH * 0.2f ), CAMPER_WIDTH );
        rect Bound = MinkSub( RectBCD( Camper.Position, Dim ), CollBound );
        
        if( IsInBound( Pos, Bound ) ) {
            Result.IsValid = true;
            Result.iEntity = iCamper;
        }
    }
    
    return Result;
}

internal void
UpdateCamper( APP_STATE * AppState, flo32 dT ) {
    CAMPER_STATE * CamperS = &AppState->CamperS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
        CAMPER * Camper = CamperS->Camper + iCamper;
        
        flo32 Friction  = CAMPER_FRICTION;
        rect  CollBound = RectBCD( Vec2( 0.0f, 0.0f ), CAMPER_DIM );
        
        CONVEYOR_RESULT ConveyorHor = IsPosOnConveyorHor( AppState, Camper->Position, CollBound );
        
        vec2 Accel = {};
        Accel.x = -Camper->Velocity.x * Friction;
        if( ConveyorHor.IsOnConveyor ) {
            Accel.x += ( ConveyorHor.ConveyorAccel.x * Friction );
        }
        
        Accel.y = -CAMPER_GRAVITY;
        
        vec2 Pos  = Camper->Position;
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + Camper->Velocity * dT;
        Camper->Velocity += Accel * dT;
        
        COLLISION_RESULT Coll = CollisionResult( Pos, Camper->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectCollapsePlatform( &BestIntersect, AppState, Coll, CollBound );
            //DoesIntersectScaffold    ( &BestIntersect, AppState, Coll, CollBound );
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        
        Coll.Pos += Coll.dPos;
        
        Camper->Position = Coll.Pos;
        Camper->Velocity = Coll.Vel;
    }
}

internal void
DrawCamper( RENDER_PASS * Pass, CAMPER Camper ) {
    rect R = RectBCD( Camper.Position, CAMPER_DIM );
    
    vec4 Color = COLOR_YELLOW;
    
    DrawRect( Pass, R, Color );
}

internal void
DrawCamper( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    CAMPER_STATE * CamperS = Draw->CamperS;
    
    for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
        CAMPER Camper = CamperS->Camper[ iCamper ];
        DrawCamper( Pass, Camper );
    }
}

internal void
DrawCamperDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    CAMPER_STATE * CamperS = Draw->CamperS;
    
    for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
        CAMPER Camper = CamperS->Camper[ iCamper ];
        DrawRectOutline( Pass, Camper.HazardBound, COLOR_GREEN );
    }
}

internal void
FinalizeCamper( APP_STATE * AppState ) {
    CAMPER_STATE * CamperS = &AppState->CamperS;
    
    rect CollBound = RectBCD( Vec2( 0.0f, 0.0f ), CAMPER_DIM - Vec2( TILE_WIDTH * 0.2f, TILE_HEIGHT * 0.2f ) );
    uint32 NewCount = 0;
    for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
        CAMPER Camper = CamperS->Camper[ iCamper ];
        
        boo32 DoKill = ( IsPosInCycleBlock( AppState, Camper.Position, CollBound ) ) || ( IsPosInSpikes( AppState, Camper.Position, CollBound ) );
        
        if( !DoKill ) {
            CamperS->Camper[ NewCount++ ] = Camper;
        }
    }
    CamperS->nCamper = NewCount;
}

internal void
Editor_DrawCamper( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__CAMPER_STATE * CamperS = &Editor->CamperS;
    
    for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
        EDITOR__CAMPER Src    = CamperS->Camper[ iCamper ];
        CAMPER         Camper = CamperC( Src.Position );
        DrawCamper( Pass, Camper );
        DrawRectOutline( Pass, Src.HazardBound, COLOR_GREEN );
    }
}

internal void
Editor_MoveCamper( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__CAMPER_STATE * CamperS = &Editor->CamperS;
    
    for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
        EDITOR__CAMPER * Camper = CamperS->Camper + iCamper;
        
        if( DoesRectIntersectRectExc( SrcBound, Camper->HazardBound ) ) {
            Camper->HazardBound  = AddOffset( Camper->HazardBound, Offset );
        }
        if( IsInBound( Camper->Position, SrcBound ) ) {
            Camper->Position    += Offset;
        }
    }
}

internal void
Editor_MirrorCamper( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__CAMPER_STATE * CamperS = &Editor->CamperS;
    
    for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
        EDITOR__CAMPER * Camper = CamperS->Camper + iCamper;
        
        vec2 Center = GetCenter( Camper->HazardBound );
        vec2 Dim    = GetDim   ( Camper->HazardBound );
        
        if( IsInBound( Camper->Position, SrcBound ) ) {
            Center.x = ( SrcBound.Right ) - ( Center.x - SrcBound.Left );
            Camper->HazardBound = RectCD( Center, Dim );
            
            Camper->Position.x = ( SrcBound.Right ) - ( Camper->Position.x - SrcBound.Left );
        }
    }
}

#define VERSION__CAMPER  ( 1 )
#define FILETAG__CAMPER  ( "CAMPER__" )
internal void
SaveCamper( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir,
           char * FileName ) {
    char * FileTag = FILETAG__CAMPER;
    uint32 Version = VERSION__CAMPER;
    EDITOR__CAMPER_STATE * CamperS = &Editor->CamperS;
    
    if( CamperS->nCamper > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, CamperS->nCamper );
        for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
            EDITOR__CAMPER Camper = CamperS->Camper[ iCamper ];
            _writem( output, EDITOR__CAMPER, Camper );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__CAMPER
ReadCamper( uint32 Version, uint8 ** Ptr ) {
    EDITOR__CAMPER Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            vec2 Position    = _read( ptr, vec2 );
            rect HazardBound = _read( ptr, rect );
            
            EDITOR__CAMPER Camper = {};
            Camper.Position    = Position;
            Camper.HazardBound = HazardBound;
            
            Result = Camper;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadCamper( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CAMPER;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        CAMPER_STATE * CamperS = &AppState->CamperS;
        
        CamperS->nCamper = _read( ptr, uint32 );
        for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
            EDITOR__CAMPER Src = ReadCamper( Version, &ptr );
            
            CAMPER Camper = CamperC( Src.Position, Src.HazardBound );
            CamperS->Camper[ iCamper ] = Camper;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadCamper( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CAMPER;
    EDITOR_STATE             * Editor      = &AppState->Editor;
    EDITOR__CAMPER_STATE * CamperS = &Editor->CamperS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        CamperS->nCamper = _read( ptr, uint32 );
        for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
            EDITOR__CAMPER Src = ReadCamper( Version, &ptr );
            CamperS->Camper[ iCamper ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputCamperToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS              Stat      = Editor->Stat;
    EDITOR__CAMPER_STATE * CamperS = &Editor->CamperS;
    
    for( uint32 iCamper = 0; iCamper < CamperS->nCamper; iCamper++ ) {
        EDITOR__CAMPER Camper = CamperS->Camper[ iCamper ];
        
        vec4 Color = COLOR_YELLOW;
        rect Bound = RectBCD( Camper.Position, CAMPER_DIM );
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}