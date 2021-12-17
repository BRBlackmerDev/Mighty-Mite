
internal EXERCISE_BALL
ExerciseBallC( vec2 Pos, vec2 Vel = {} ) {
    EXERCISE_BALL ExerciseBall = {};
    ExerciseBall.Position = Pos;
    ExerciseBall.Velocity = Vel;
    
    return ExerciseBall;
}

internal void
AddExerciseBall( APP_STATE * AppState, vec2 Pos, vec2 Vel = {} ) {
    EXERCISE_BALL_STATE * ExerciseBallS = &AppState->ExerciseBallS;
    
    if( ExerciseBallS->nExerciseBall < EXERCISE_BALL_MAX_COUNT ) {
        EXERCISE_BALL ExerciseBall = ExerciseBallC( Pos, Vel );
        ExerciseBallS->ExerciseBall[ ExerciseBallS->nExerciseBall++ ] = ExerciseBall;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new EXERCISE_BALL, but the state is full! Max Count = %u", EXERCISE_BALL_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemoveExerciseBall( APP_STATE * AppState, uint32 iRemove ) {
    EXERCISE_BALL_STATE * ExerciseBallS = &AppState->ExerciseBallS;
    
    Assert( ExerciseBallS->nExerciseBall > 0 );
    ExerciseBallS->ExerciseBall[ iRemove ] = ExerciseBallS->ExerciseBall[ --ExerciseBallS->nExerciseBall ];
}

internal ENTITY_VALID_RESULT
IsPosOnExerciseBall( APP_STATE * AppState, vec2 Pos ) {
    EXERCISE_BALL_STATE * ExerciseBallS = &AppState->ExerciseBallS;
    
    ENTITY_VALID_RESULT Result = {};
    
    for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
        EXERCISE_BALL ExerciseBall = ExerciseBallS->ExerciseBall[ iExerciseBall ];
        
        rect R = RectCD( ExerciseBall.Position, EXERCISE_BALL_DIM );
        R.Left  -= ( TILE_WIDTH * 0.4f );
        R.Right += ( TILE_WIDTH * 0.4f );
        R.Bottom = R.Top - EXERCISE_BALL_RADIUS - ( TILE_HEIGHT * 0.5f );
        R.Top   += ( TILE_HEIGHT * 0.3f );
        
        if( IsInBound( Pos, R ) ) {
            Result.IsValid = true;
            Result.iEntity = iExerciseBall;
        }
    }
    
    return Result;
}

internal ENTITY_VALID_RESULT
IsPosByExerciseBall( APP_STATE * AppState, vec2 Pos, rect ByBound ) {
    EXERCISE_BALL_STATE * ExerciseBallS = &AppState->ExerciseBallS;
    
    ENTITY_VALID_RESULT Result = {};
    
    flo32 MaxDistSq = FLT_MAX;
    for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
        EXERCISE_BALL ExerciseBall = ExerciseBallS->ExerciseBall[ iExerciseBall ];
        
        rect R = RectCD( ExerciseBall.Position, EXERCISE_BALL_DIM );
        R.Left   -= ByBound.Right;
        R.Right  -= ByBound.Left;
        R.Bottom -= ByBound.Top;
        R.Top    -= ByBound.Bottom;
        
        if( IsInBound( Pos, R ) ) {
            flo32 DistSq = GetLengthSq( Pos - ExerciseBall.Position );
            if( DistSq < MaxDistSq ) {
                MaxDistSq = DistSq;
                
                Result.IsValid = true;
                Result.iEntity = iExerciseBall;
            }
        }
    }
    
    return Result;
}

internal void
UpdateExerciseBall( APP_STATE * AppState, flo32 dT ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    EXERCISE_BALL_STATE   * ExerciseBallS  = &AppState->ExerciseBallS;
    
    for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
        EXERCISE_BALL * ExerciseBall = ExerciseBallS->ExerciseBall + iExerciseBall;
        
        flo32 Friction = EXERCISE_BALL_AIR_FRICTION;
        
        boo32 IsOnGround = false;
        vec2  ExerciseBallGroundPos = ExerciseBall->Position + Vec2( 0.0f, -EXERCISE_BALL_RADIUS );
        { // IsOnGround : TERRAIN
            for( uint32 iJumpBound = 0; iJumpBound < Collision->nJumpBound; iJumpBound++ ) {
                rect R = Collision->JumpBound[ iJumpBound ];
                
                if( IsInBound( ExerciseBallGroundPos, R ) ) {
                    IsOnGround = true;
                }
            }
        }
#if 0        
        
        boo32 IsOnScaffold         = IsExerciseBallOnScaffold( AppState, ExerciseBallGroundPos );
        boo32 IsOnCollapsePlatform = IsPosOnCollapsePlatform( AppState, ExerciseBallGroundPos );
        boo32 DoGroundFriction = ( IsOnGround ) || ( IsOnScaffold ) || ( IsOnCollapsePlatform );
        if( DoGroundFriction ) {
            Friction = EXERCISE_BALL_GROUND_FRICTION;
        }
#endif
        
        flo32 Reflect = 0.0f;
        if( ExerciseBall->nBounce == 0 ) {
            Reflect  = 0.95f;
            Friction = EXERCISE_BALL_AIR_FRICTION * 1.0f;
        }
        if( ExerciseBall->nBounce == 1 ) {
            Reflect  = 0.80f;
            Friction = EXERCISE_BALL_AIR_FRICTION * 2.0f;
        }
        if( ExerciseBall->nBounce == 2 ) {
            Reflect  = 0.70f;
            Friction = EXERCISE_BALL_AIR_FRICTION * 4.0f;
        }
        if( ( ExerciseBall->nBounce >= 3 ) && ( ExerciseBall->nBounce <= 8 ) ) {
            Reflect  = 0.60f;
            Friction = EXERCISE_BALL_AIR_FRICTION * 6.0f;
        }
        if( ( IsOnGround ) && ( ExerciseBall->nBounce > 8 ) ) {
            Friction = EXERCISE_BALL_GROUND_FRICTION;
        }
        
        vec2 Accel = {};
        Accel.x = -ExerciseBall->Velocity.x * Friction;
        Accel.y = -EXERCISE_BALL_GRAVITY;
        
        LEVEL_STATS Stat = AppState->Stat;
        if( ExerciseBall->Position.x <= Stat.Bound.Left ) {
            Accel.x += ( 1.0f * Friction );
        }
        if( ExerciseBall->Position.x >= Stat.Bound.Right ) {
            Accel.x -= ( 1.0f * Friction );
        }
        
        
#if 0        
        CONVEYOR_RESULT ConveyorH_Result = IsOnConveyorH( AppState, ExerciseBallGroundPos );
        if( ConveyorH_Result.IsOnConveyor ) {
            Accel.x += ( ConveyorH_Result.ConveyorAccel.x * Friction );
        }
#endif
        
        vec2 Pos  = ExerciseBall->Position;
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + ExerciseBall->Velocity * dT;
        ExerciseBall->Velocity += Accel * dT;
        flo32 Speed = GetLength( ExerciseBall->Velocity );
        
        COLLISION_RESULT Coll = CollisionResult( Pos, ExerciseBall->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll    ( &BestIntersect, AppState, Coll, EXERCISE_BALL_RADIUS );
            DoesIntersectScaffold        ( &BestIntersect, AppState, Coll, EXERCISE_BALL_RADIUS );
            DoesIntersectCollapsePlatform( &BestIntersect, AppState, Coll, EXERCISE_BALL_RADIUS );
            
            DoesIntersectPushBlock ( &BestIntersect, AppState, Coll, EXERCISE_BALL_RADIUS );
            DoesIntersectBreakBlock( &BestIntersect, AppState, Coll, EXERCISE_BALL_RADIUS );
            
            DoesIntersectLevelBound( &BestIntersect, AppState, Coll );
            
            Coll = FinalizeCollision( BestIntersect, Coll, Reflect );
        }
        Coll.Pos += Coll.dPos;
        
        ExerciseBall->Position = Coll.Pos;
        ExerciseBall->Velocity = Coll.Vel;
        if( Coll.HadCollision ) {
            ExerciseBall->nBounce++;
            
            if( Speed > 10.0f ) {
                AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_ExerciseBall_Bounce01 + RandomU32InRange( 0, 1 ) );
                PlayAudio( AppState, Sound_Select, 0.25f );
            }
        }
        
        if( Speed > 10.0f ) {
            // TODO: Probably check for some airborne velocity threshold
            rect Bound = RectCD( Vec2( 0.0f, 0.0f ), EXERCISE_BALL_DIM );
            ENTITY_VALID_RESULT IsInStunBound = IsInPuncherStunBound( AppState, ExerciseBall->Position, Bound );
            if( IsInStunBound.IsValid ) {
                flo32 DirX = ExerciseBall->Velocity.x / fabsf( ExerciseBall->Velocity.x );
                StunPuncher( AppState, IsInStunBound.iEntity, DirX );
                
                ExerciseBall->Velocity.x = -ExerciseBall->Velocity.x;
                ExerciseBall->nBounce++;
                
                AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_ExerciseBall_Hit01 + RandomU32InRange( 0, 6 ) );
                PlayAudio( AppState, Sound_Select, 0.35f );
            }
            
            // TODO: Tune this!
            ENTITY_VALID_RESULT Spotter_IsInStunBound = IsInSpotterStunBound( AppState, ExerciseBall->Position, EXERCISE_BALL_RADIUS );
            if( Spotter_IsInStunBound.IsValid ) {
                SPOTTER_STATE * SpotterS = &AppState->SpotterS;
                SPOTTER       * Spotter  = SpotterS->Spotter + Spotter_IsInStunBound.iEntity;
                
                flo32 Radius = ( SPOTTER_RADIUS - ( TILE_WIDTH * 0.1f ) ) + ( EXERCISE_BALL_RADIUS - ( TILE_WIDTH * 0.1f ) ) + COLLISION_EPSILON_OFFSET;
                
                vec2 Dir  = GetNormal( ExerciseBall->Velocity );
                vec2 Norm = GetNormal( ExerciseBall->Position - Spotter->Position );
                StunSpotter( AppState, Spotter_IsInStunBound.iEntity, GetNormal( ExerciseBall->Velocity ) );
                Spotter->Position = ExerciseBall->Position - Norm * Radius;
                
                ExerciseBall->Velocity -= ( 2.0f * dot( ExerciseBall->Velocity, Norm ) ) * Norm;
                ExerciseBall->nBounce++;
                
                AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_ExerciseBall_Hit01 + RandomU32InRange( 0, 6 ) );
                PlayAudio( AppState, Sound_Select, 0.35f );
            }
        }
    }
}

internal void
DrawExerciseBall( RENDER_PASS * Pass, EXERCISE_BALL ExerciseBall ) {
    DrawCircle( Pass, ExerciseBall.Position, EXERCISE_BALL_RADIUS, ToColor( 0, 0, 200 ) );
}

internal void
DrawExerciseBall( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    EXERCISE_BALL_STATE * ExerciseBallS = Draw->ExerciseBallS;
    
    for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
        EXERCISE_BALL ExerciseBall = ExerciseBallS->ExerciseBall[ iExerciseBall ];
        DrawExerciseBall( Pass, ExerciseBall );
    }
}

internal void
DrawExerciseBallDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    EXERCISE_BALL_STATE * ExerciseBallS = Draw->ExerciseBallS;
    
    for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
        EXERCISE_BALL ExerciseBall = ExerciseBallS->ExerciseBall[ iExerciseBall ];
        
        rect R = RectCD( ExerciseBall.Position, EXERCISE_BALL_DIM );
        R.Left  -= ( TILE_WIDTH * 0.4f );
        R.Right += ( TILE_WIDTH * 0.4f );
        R.Bottom = R.Top - EXERCISE_BALL_RADIUS - ( TILE_HEIGHT * 0.5f );
        R.Top   += ( TILE_HEIGHT * 0.3f );
        
        DrawRectOutline( Pass, R, COLOR_RED );
    }
}

internal void
FinalizeExerciseBall( APP_STATE * AppState ) {
    EXERCISE_BALL_STATE * ExerciseBallS = &AppState->ExerciseBallS;
    
    uint32 NewCount = 0;
    for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
        EXERCISE_BALL ExerciseBall = ExerciseBallS->ExerciseBall[ iExerciseBall ];
        rect Bound = RectCRad( ExerciseBall.Position, EXERCISE_BALL_RADIUS );
        
        boo32 IsActive = ( ExerciseBall.Position.y > AppState->DeathPlaneY );
        
        if( IsActive ) {
            ExerciseBallS->ExerciseBall[ NewCount++ ] = ExerciseBall;
        }
    }
    ExerciseBallS->nExerciseBall = NewCount;
}

internal void
Editor_DrawExerciseBall( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__EXERCISE_BALL_STATE * ExerciseBallS = &Editor->ExerciseBallS;
    
    for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
        EDITOR__EXERCISE_BALL Src     = ExerciseBallS->ExerciseBall[ iExerciseBall ];
        EXERCISE_BALL         ExerciseBall = ExerciseBallC( Src.Position );
        DrawExerciseBall( Pass, ExerciseBall );
    }
}

#define VERSION__EXERCISE_BALL  ( 1 )
#define FILETAG__EXERCISE_BALL  ( "EXERBALL" )
internal void
SaveExerciseBall( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__EXERCISE_BALL;
    uint32 Version = VERSION__EXERCISE_BALL;
    EDITOR__EXERCISE_BALL_STATE * ExerciseBallS = &Editor->ExerciseBallS;
    
    if( ExerciseBallS->nExerciseBall > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, ExerciseBallS->nExerciseBall );
        for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
            EDITOR__EXERCISE_BALL ExerciseBall = ExerciseBallS->ExerciseBall[ iExerciseBall ];
            _writem( output, EDITOR__EXERCISE_BALL, ExerciseBall );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__EXERCISE_BALL
ReadExerciseBall( uint32 Version, uint8 ** Ptr ) {
    EDITOR__EXERCISE_BALL Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            vec2 Pos = _read( ptr, vec2 );
            
            EDITOR__EXERCISE_BALL ExerciseBall = {};
            ExerciseBall.Position = Pos;
            
            Result = ExerciseBall;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadExerciseBall( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__EXERCISE_BALL;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        EXERCISE_BALL_STATE * ExerciseBallS = &AppState->ExerciseBallS;
        
        ExerciseBallS->nExerciseBall = _read( ptr, uint32 );
        for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
            EDITOR__EXERCISE_BALL Src = ReadExerciseBall( Version, &ptr );
            
            EXERCISE_BALL ExerciseBall = ExerciseBallC( Src.Position );
            ExerciseBallS->ExerciseBall[ iExerciseBall ] = ExerciseBall;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadExerciseBall( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__EXERCISE_BALL;
    EDITOR_STATE          * Editor   = &AppState->Editor;
    EDITOR__EXERCISE_BALL_STATE * ExerciseBallS = &Editor->ExerciseBallS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        ExerciseBallS->nExerciseBall = _read( ptr, uint32 );
        for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
            EDITOR__EXERCISE_BALL Src = ReadExerciseBall( Version, &ptr );
            ExerciseBallS->ExerciseBall[ iExerciseBall ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputExerciseBallToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS                 Stat       = Editor->Stat;
    EDITOR__EXERCISE_BALL_STATE * ExerciseBallS = &Editor->ExerciseBallS;
    
    for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
        EDITOR__EXERCISE_BALL ExerciseBall = ExerciseBallS->ExerciseBall[ iExerciseBall ];
        
        vec4 Color = ToColor( 0, 0, 200 );
        rect Bound = RectCD( ExerciseBall.Position, EXERCISE_BALL_DIM );
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputCircleToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}