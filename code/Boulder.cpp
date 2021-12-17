
internal BOULDER
BoulderC( vec2 Pos, vec2 Vel = {} ) {
    BOULDER Boulder = {};
    Boulder.Position = Pos;
    Boulder.Velocity = Vel;
    return Boulder;
}

internal void
AddBoulder( APP_STATE * AppState, vec2 Pos, vec2 Vel = Vec2( 0.0f, 0.0f ) ) {
    BOULDER_STATE * BoulderS = &AppState->BoulderS;
    
    if( BoulderS->nBoulder < BOULDER_MAX_COUNT ) {
        BOULDER Boulder = BoulderC( Pos, Vel );
        BoulderS->Boulder[ BoulderS->nBoulder++ ] = Boulder;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new BOULDER, but the state is full! Max Count = %u", BOULDER_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemoveBoulder( APP_STATE * AppState, uint32 iRemove ) {
    BOULDER_STATE * BoulderS = &AppState->BoulderS;
    
    Assert( BoulderS->nBoulder > 0 );
    BoulderS->Boulder[ iRemove ] = BoulderS->Boulder[ --BoulderS->nBoulder ];
}

internal CONVEYOR_RESULT
IsOnConveyorH( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    CONVEYOR_STATE * ConveyorS = &AppState->ConveyorS;
    
    CONVEYOR_RESULT Result = {};
    
    UINT32_PAIR Type = ConveyorS->Type[ ConveyorType_OnTop ];
    for( uint32 iConveyor = 0; iConveyor < Type.n; iConveyor++ ) {
        CONVEYOR Conveyor = ConveyorS->Conveyor[ Type.m + iConveyor ];
        rect R = MinkSub( Conveyor.Bound_Mvt, CollBound );
        if( IsInBound( Pos, R ) ) {
            Result.IsOnConveyor  = true;
            Result.ConveyorAccel = Conveyor.Accel;
        }
    }
    
    return Result;
}

internal boo32
IsBoulderOnScaffold( APP_STATE * AppState, vec2 BoulderP ) {
    SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
    
    boo32 Result = false;
    for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
        SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
        
        vec2 Offset = Vec2( 0.0f, TILE_HEIGHT * 0.1f );
        vec2 P = Scaffold.CollisionEdge.P - Offset;
        vec2 Q = Scaffold.CollisionEdge.Q + Offset;
        rect R = RectMM( P, Q );
        if( IsInBound( BoulderP, R ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal void
UpdateBoulder( APP_STATE * AppState, flo32 dT ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    BOULDER_STATE   * BoulderS  = &AppState->BoulderS;
    
    for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
        BOULDER * Boulder = BoulderS->Boulder + iBoulder;
        
        flo32 Friction = BOULDER_AIR_FRICTION;
        boo32 IsOnGround = false;
        vec2  BoulderGroundPos = Boulder->Position + Vec2( 0.0f, -BOULDER_RADIUS );
        { // IsOnGround : TERRAIN
            for( uint32 iJumpBound = 0; iJumpBound < Collision->nJumpBound; iJumpBound++ ) {
                rect R = Collision->JumpBound[ iJumpBound ];
                
                if( IsInBound( BoulderGroundPos, R ) ) {
                    IsOnGround = true;
                }
            }
        }
        
        boo32 IsOnScaffold         = IsBoulderOnScaffold( AppState, BoulderGroundPos );
        boo32 IsOnCollapsePlatform = IsPosOnCollapsePlatform( AppState, BoulderGroundPos );
        boo32 DoGroundFriction = ( IsOnGround ) || ( IsOnScaffold ) || ( IsOnCollapsePlatform );
        if( DoGroundFriction ) {
            Friction = BOULDER_GROUND_FRICTION;
        }
        
        vec2 Accel = {};
        Accel.x = -Boulder->Velocity.x * Friction;
        Accel.y = -BOULDER_GRAVITY;
        
        LEVEL_STATS Stat = AppState->Stat;
        if( Boulder->Position.x <= Stat.Bound.Left ) {
            Accel.x += ( 1.0f * Friction );
        }
        if( Boulder->Position.x >= Stat.Bound.Right ) {
            Accel.x -= ( 1.0f * Friction );
        }
        
        rect Conveyor_CollBound = {};
        CONVEYOR_RESULT ConveyorH_Result = IsOnConveyorH( AppState, BoulderGroundPos, Conveyor_CollBound );
        if( ConveyorH_Result.IsOnConveyor ) {
            Accel.x += ( ConveyorH_Result.ConveyorAccel.x * Friction );
        }
        
        vec2 Pos  = Boulder->Position;
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + Boulder->Velocity * dT;
        Boulder->Velocity += Accel * dT;
        flo32 Speed = GetLength( Boulder->Velocity );
        
        COLLISION_RESULT Coll = CollisionResult( Pos, Boulder->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll( &BestIntersect, AppState, Coll, BOULDER_RADIUS );
            DoesIntersectScaffold    ( &BestIntersect, AppState, Coll, BOULDER_RADIUS );
            DoesIntersectSeesaw      ( &BestIntersect, AppState, Coll );
            DoesIntersectCollapsePlatform( &BestIntersect, AppState, Coll, BOULDER_RADIUS );
            
            DoesIntersectPushBlock ( &BestIntersect, AppState, Coll, BOULDER_RADIUS );
            DoesIntersectBreakBlock( &BestIntersect, AppState, Coll, BOULDER_RADIUS );
            
            DoesIntersectLevelBound( &BestIntersect, AppState, Coll );
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        Coll.Pos += Coll.dPos;
        
        if( ( Coll.HadCollision ) && ( Speed > 8.0f ) ) {
            PlayAudio( AppState, AudioID_Sound_Boulder, 0.5f );
        }
        
        Boulder->Position = Coll.Pos;
        Boulder->Velocity = Coll.Vel;
        
        if( !IsOnGround ) {
            // TODO: Probably check for some airborne velocity threshold
            rect Bound = RectCD( Vec2( 0.0f, 0.0f ), BOULDER_DIM );
            ENTITY_VALID_RESULT IsInStunBound = IsInPuncherStunBound( AppState, Boulder->Position, Bound );
            if( IsInStunBound.IsValid ) {
                flo32 DirX = Boulder->Velocity.x / fabsf( Boulder->Velocity.x );
                StunPuncher( AppState, IsInStunBound.iEntity, DirX );
            }
            
            ENTITY_VALID_RESULT Spotter_IsInStunBound = IsInSpotterStunBound( AppState, Boulder->Position, BOULDER_RADIUS );
            if( Spotter_IsInStunBound.IsValid ) {
                SPOTTER_STATE * SpotterS = &AppState->SpotterS;
                SPOTTER       * Spotter  = SpotterS->Spotter + Spotter_IsInStunBound.iEntity;
                
                flo32 Radius = ( SPOTTER_RADIUS - ( TILE_WIDTH * 0.1f ) ) + ( BOULDER_RADIUS - ( TILE_WIDTH * 0.1f ) ) + COLLISION_EPSILON_OFFSET;
                
                vec2 Dir  = GetNormal( Boulder->Velocity );
                vec2 Norm = GetNormal( Boulder->Position - Spotter->Position );
                StunSpotter( AppState, Spotter_IsInStunBound.iEntity, GetNormal( Boulder->Velocity ) );
                Spotter->Position = Boulder->Position - Norm * Radius;
                
                Boulder->Velocity -= ( 1.1f * dot( Boulder->Velocity, Norm ) ) * Norm;
                
                //AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_Boulder_Hit01 + RandomU32InRange( 0, 6 ) );
                //PlayAudio( AppState, Sound_Select, 0.35f );
            }
        }
    }
}

internal void
DrawBoulder( RENDER_PASS * Pass, vec2 BoulderP ) {
    DrawCircle( Pass, BoulderP, BOULDER_RADIUS, COLOR_GRAY( 0.15f ) );
}

internal void
DrawBoulder( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    BOULDER_STATE * BoulderS = Draw->BoulderS;
    
    for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
        BOULDER Boulder = BoulderS->Boulder[ iBoulder ];
        DrawBoulder( Pass, Boulder.Position );
    }
}

internal void
DrawBoulderDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    BOULDER_STATE * BoulderS = Draw->BoulderS;
    
    //for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
    //BOULDER Boulder = BoulderS->Boulder[ iBoulder ];
    //DrawRectOutline( Pass, Boulder.Bound_Mvt, COLOR_RED );
    //}
}

internal void
FinalizeBoulder( APP_STATE * AppState ) {
    BOULDER_STATE * BoulderS = &AppState->BoulderS;
    
    uint32 NewCount = 0;
    for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
        BOULDER Boulder = BoulderS->Boulder[ iBoulder ];
        rect Bound = RectCRad( Boulder.Position, BOULDER_RADIUS );
        
        boo32 IsActive = ( Boulder.Position.y > AppState->DeathPlaneY );
        
        if( IsActive ) {
            BoulderS->Boulder[ NewCount++ ] = Boulder;
        }
    }
    BoulderS->nBoulder = NewCount;
}

internal void
Editor_DrawBoulder( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__BOULDER_STATE * BoulderS = &Editor->BoulderS;
    
    for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
        EDITOR__BOULDER Src     = BoulderS->Boulder[ iBoulder ];
        BOULDER         Boulder = BoulderC( Src.Position );
        DrawBoulder( Pass, Boulder.Position );
    }
}

#define VERSION__BOULDER  ( 1 )
#define FILETAG__BOULDER  ( "BOULDER_" )
internal void
SaveBoulder( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__BOULDER;
    uint32 Version = VERSION__BOULDER;
    EDITOR__BOULDER_STATE * BoulderS = &Editor->BoulderS;
    
    if( BoulderS->nBoulder > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, BoulderS->nBoulder );
        for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
            EDITOR__BOULDER Boulder = BoulderS->Boulder[ iBoulder ];
            _writem( output, EDITOR__BOULDER, Boulder );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__BOULDER
ReadBoulder( uint32 Version, uint8 ** Ptr ) {
    EDITOR__BOULDER Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            vec2 Pos = _read( ptr, vec2 );
            
            EDITOR__BOULDER Boulder = {};
            Boulder.Position = Pos;
            
            Result = Boulder;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadBoulder( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__BOULDER;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        BOULDER_STATE * BoulderS = &AppState->BoulderS;
        
        BoulderS->nBoulder = _read( ptr, uint32 );
        for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
            EDITOR__BOULDER Src = ReadBoulder( Version, &ptr );
            
            BOULDER Boulder = BoulderC( Src.Position );
            BoulderS->Boulder[ iBoulder ] = Boulder;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadBoulder( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__BOULDER;
    EDITOR_STATE          * Editor   = &AppState->Editor;
    EDITOR__BOULDER_STATE * BoulderS = &Editor->BoulderS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        BoulderS->nBoulder = _read( ptr, uint32 );
        for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
            EDITOR__BOULDER Src = ReadBoulder( Version, &ptr );
            BoulderS->Boulder[ iBoulder ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputBoulderToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS                 Stat       = Editor->Stat;
    EDITOR__BOULDER_STATE * BoulderS = &Editor->BoulderS;
    
    for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
        EDITOR__BOULDER Boulder = BoulderS->Boulder[ iBoulder ];
        
        vec4 Color = COLOR_GRAY( 0.15f );
        rect Bound = RectCD( Boulder.Position, BOULDER_DIM );
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputCircleToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}