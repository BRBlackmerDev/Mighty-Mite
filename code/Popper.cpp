
internal POPPER
PopperC( vec2 Pos, vec2 Vel = {} ) {
    POPPER Popper = {};
    Popper.Position = Pos;
    Popper.Velocity = Vel;
    Popper.DirX     = -1.0f;
    
    return Popper;
}

internal POPPER
PopperThrownC( vec2 Pos, vec2 Vel ) {
    POPPER Popper = {};
    Popper.Position  = Pos;
    Popper.Velocity  = Vel;
    Popper.Mode      = PopperMode_IsThrown;
    Popper.Charge    = 1.0f;
    
    return Popper;
}

internal void
AddPopper( APP_STATE * AppState, POPPER Popper ) {
    POPPER_STATE * PopperS = &AppState->PopperS;
    
    if( PopperS->nPopper < POPPER_MAX_COUNT ) {
        PopperS->Popper[ PopperS->nPopper++ ] = Popper;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new POPPER, but the state is full! Max Count = %u", POPPER_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemovePopper( APP_STATE * AppState, uint32 iRemove ) {
    POPPER_STATE * PopperS = &AppState->PopperS;
    
    Assert( PopperS->nPopper > 0 );
    PopperS->Popper[ iRemove ] = PopperS->Popper[ --PopperS->nPopper ];
}

internal ENTITY_VALID_RESULT
IsInPopperStunBound( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    POPPER_STATE * PopperS = &AppState->PopperS;
    
    ENTITY_VALID_RESULT Result = {};
    
    for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
        POPPER Popper = PopperS->Popper[ iPopper ];
        
        //if( Popper.Mode != PopperMode_IsStunned ) {
        vec2 Dim   = POPPER_DIM;
        rect Bound = RectBCD( Popper.Position, Dim );
        Bound = MinkSub( Bound, CollBound );
        
        if( IsInBound( Pos, Bound ) ) {
            Result.IsValid = true;
            Result.iEntity = iPopper;
        }
        //}
    }
    
    return Result;
}

internal ENTITY_VALID_RESULT
IsInPopperPickUpBound( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    POPPER_STATE * PopperS = &AppState->PopperS;
    
    ENTITY_VALID_RESULT Result = {};
    
    for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
        POPPER Popper = PopperS->Popper[ iPopper ];
        
        if( Popper.Mode == PopperMode_IsStunned ) {
            vec2 Dim   = Vec2( POPPER_WIDTH + ( TILE_WIDTH * 0.2f ), POPPER_WIDTH );
            rect Bound = RectBCD( Popper.Position, Dim );
            Bound = MinkSub( Bound, CollBound );
            
            if( IsInBound( Pos, Bound ) ) {
                Result.IsValid = true;
                Result.iEntity = iPopper;
            }
        }
    }
    
    return Result;
}

internal void
StunPopper( APP_STATE * AppState, uint32 iPopper, flo32 DirX ) {
    POPPER_STATE * PopperS = &AppState->PopperS;
    POPPER       * Popper  = PopperS->Popper + iPopper;
    Popper->Mode   = PopperMode_IsStunned;
    Popper->Timer  = 0.0f;
    Popper->Charge = 0.0f;
    
    vec2 V = Vec2( 14.0f * DirX, 6.0f );
    Popper->Velocity = V;
}

internal void
UpdatePopper( APP_STATE * AppState, flo32 dT ) {
    POPPER_STATE * PopperS = &AppState->PopperS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
        POPPER * Popper = PopperS->Popper + iPopper;
        
        Popper->Timer += dT;
        
        vec2 Dir = {};
        
        if( Popper->Mode == PopperMode_Move ) {
            vec2 PanicDim   = Vec2( TILE_WIDTH * 6.0f, TILE_HEIGHT * 3.5f );
            rect PanicBound = RectBCD( Popper->Position, PanicDim );
            
            boo32 DoPanic = ( IsInBound( Player->Position, PanicBound ) );
            
            { // PUNCHER
                PUNCHER_STATE * PuncherS = &AppState->PuncherS;
                for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
                    PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
                    if( IsInBound( Puncher.Position, PanicBound ) ) {
                        DoPanic = true;
                    }
                }
            }
            
            if( DoPanic ) {
                flo32 Rate = 1.0f / 2.0f;
                Popper->Charge += Rate * dT;
                
                if( Popper->Charge >= 1.0f ) {
                    Popper->Mode        = PopperMode_IsPopping;
                    Popper->Velocity.y  = 8.0f;
                    Popper->Timer       = 0.0f;
                }
            } else {
                flo32 Rate = 1.0f / 1.0f;
                Popper->Charge = MaxValue( Popper->Charge - Rate * dT, 0.0f );
                
                if( Popper->Charge == 0.0f ) {
                    Dir.x = Popper->DirX;
                    
                    rect CollBound = RectBCD( Vec2( 0.0f, 0.0f ), POPPER_DIM );
                    RAY2_INTERSECT BestIntersectHor = Ray2IntersectInit();
                    RAY2_INTERSECT BestIntersectVer = Ray2IntersectInit();
                    
                    vec2 Center = GetCenter( RectBCD( Popper->Position, POPPER_DIM ) );
                    
                    COLLISION_RESULT CollHor = CollisionResult( Center, Popper->Velocity, Vec2( Dir.x * 0.25f, 0.0f ) );
                    COLLISION_RESULT CollVer = CollisionResult( Center + Vec2( Dir.x * 1.0f, 0.0f ), Popper->Velocity, Vec2( 0.0f, -1.0f ) );
                    
                    DoesIntersectCollisionAll( &BestIntersectHor, AppState, CollHor, CollBound );
                    DoesIntersectCollisionAll( &BestIntersectVer, AppState, CollVer, CollBound );
                    DoesIntersectBreakBlock  ( &BestIntersectHor, AppState, CollHor, CollBound );
                    DoesIntersectBreakBlock  ( &BestIntersectVer, AppState, CollVer, CollBound );
                    
                    boo32 HorIsValid = ( BestIntersectHor.IsValid ) && ( BestIntersectHor.t <= 1.0f );
                    boo32 VerIsValid = ( BestIntersectVer.IsValid ) && ( BestIntersectVer.t <= 1.0f );
                    
                    if( ( HorIsValid ) || ( !VerIsValid ) ) {
                        Popper->DirX = -Popper->DirX;
                        Dir.x = Popper->DirX;
                    }
                    
                    // TODO: change direction
                    //  if had forward collision
                    //  if no floor ahead
                }
            }
        }
        
        boo32 DoPop = false;
        
        flo32 PopTargetTime = ( 20.0f / 60.0f );
        if( ( Popper->Mode == PopperMode_IsPopping ) && ( Popper->Timer >= PopTargetTime ) ) {
            DoPop = true;
        }
        
        flo32 Friction = POPPER_MOVE_FRICTION;
        if( Popper->Mode == PopperMode_IsThrown ) {
            Friction = POPPER_AIR_FRICTION;
        }
        
        rect CollBound = RectBCD( Vec2( 0.0f, 0.0f ), POPPER_DIM );
        CONVEYOR_RESULT ConveyorHor = IsPosOnConveyorHor( AppState, Popper->Position, CollBound );
        
        vec2 Accel = {};
        Accel.x = ( Dir.x * POPPER_MOVE_SPEED - Popper->Velocity.x ) * Friction;
        if( ConveyorHor.IsOnConveyor ) {
            Accel.x += ( ConveyorHor.ConveyorAccel.x * Friction );
        }
        
        Accel.y = -POPPER_GRAVITY;
        
        vec2 Pos  = Popper->Position;
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + Popper->Velocity * dT;
        Popper->Velocity += Accel * dT;
        
        COLLISION_RESULT Coll = CollisionResult( Pos, Popper->Velocity, dPos );
        if( Popper->Mode == PopperMode_IsThrown ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll    ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectBreakBlock      ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectPushBlock       ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectScaffold        ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectCollapsePlatform( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectLevelBound      ( &BestIntersect, AppState, Coll, CollBound );
            Coll = FinalizeCollision( BestIntersect, Coll );
            Coll.Pos += Coll.dPos;
            
            if( Coll.HadCollision ) {
                DoPop = true;
            } else {
                DoPop = ( IsTouchingPuncher( AppState, Popper->Position, CollBound ) )
                    || ( IsTouchingSpotter( AppState, Popper->Position, CollBound ) )
                    || ( IsTouchingExerciseMiniBoss02( AppState, Popper->Position, CollBound ) )
                    || ( IsTouchingExerciseMiniBoss03( AppState, Popper->Position, CollBound ) );
            }
        } else {
            for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
                RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                DoesIntersectCollisionAll( &BestIntersect, AppState, Coll, CollBound );
                //DoesIntersectScaffold    ( &BestIntersect, AppState, Coll, CollBound );
                
                Coll = FinalizeCollision( BestIntersect, Coll );
            }
            Coll.Pos += Coll.dPos;
        }
        
        Popper->Position = Coll.Pos;
        Popper->Velocity = Coll.Vel;
        
        if( Popper->Mode != PopperMode_IsThrown ) {
            rect SpikesBound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( TILE_WIDTH * 0.2f, POPPER_HEIGHT ) );
            boo32 IsTouchingSpikes = IsPosInSpikes( AppState, Popper->Position, SpikesBound );
            if( IsTouchingSpikes ) {
                DoPop = true;
            }
            
            BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
            for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
                BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
                if( BreakBlock.Velocity.y < -2.0f ) {
                    rect DamageBound = RectCD( BreakBlock.Position, BreakBlock.Dim );
                    DamageBound.Top     = DamageBound.Bottom;
                    DamageBound.Bottom -= ( POPPER_HEIGHT - TILE_HEIGHT * 0.2f );
                    
                    if( IsInBound( Popper->Position, DamageBound ) ) {
                        DoPop = true;
                    }
                }
            }
        }
        
        if( DoPop ) {
            Popper->Mode = PopperMode_IsDead;
            vec2 DamageDim   = Vec2( TILE_WIDTH * 5.0f, TILE_HEIGHT * 5.0f );
            rect DamageBound = RectCD( Popper->Position, DamageDim );
            
            AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_Popper_Explode01 + RandomU32InRange( 0, 1 ) );
            PlayAudio( AppState, Sound_Select, 0.25f );
            
            { // Player
                if( IsInBound( Player->Position, DamageBound ) ) {
                    DamagePlayer( AppState, 1 );
                }
            }
            
            { // Exercise Mini Boss 02
                EXERCISE_MINIBOSS02 * Boss = &AppState->ExerciseMiniBoss02;
                if( Boss->IsActive ) {
                    rect R = MinkSub( DamageBound, RectBCD( Vec2( 0.0f, 0.0f ), EXERCISE_MINIBOSS02_DIM ) );
                    if( IsInBound( Boss->Position, R ) ) {
                        flo32 DirX = Popper->Velocity.x / fabsf( Popper->Velocity.x );
                        Boss->DoStun  = true;
                        Boss->DoStunX = DirX;
                    }
                }
            }
            
            { // Exercise Mini Boss 03
                EXERCISE_MINIBOSS03 * Boss = &AppState->ExerciseMiniBoss03;
                if( Boss->IsActive ) {
                    rect R = MinkSub( DamageBound, RectBCD( Vec2( 0.0f, 0.0f ), Boss->Dim ) );
                    if( IsInBound( Boss->Position, R ) ) {
                        Boss->DoPop = true;
                    }
                }
            }
            
            { // Puncher
                PUNCHER_STATE * PuncherS = &AppState->PuncherS;
                for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
                    PUNCHER * Puncher = PuncherS->Puncher + iPuncher;
                    
                    rect R = MinkSub( DamageBound, RectBCD( Vec2( 0.0f, 0.0f ), PUNCHER_DIM ) );
                    if( IsInBound( Puncher->Position, R ) ) {
                        flo32 DirX = Popper->Velocity.x / fabsf( Popper->Velocity.x );
                        Puncher->IsStunned       = true;
                        Puncher->IsStunned_Timer = 0.0f;
                        Puncher->Velocity        = Vec2( DirX * 14.0f, 6.0f );
                    }
                }
            }
            
            { // BreakBlock
                BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
                uint32 NewCount = 0;
                for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
                    BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
                    
                    rect R = MinkSub( DamageBound, RectBCD( Vec2( 0.0f, 0.0f ), BreakBlock.Dim ) );
                    if( !IsInBound( BreakBlock.Position, R ) ) {
                        BreakBlockS->BreakBlock[ NewCount++ ] = BreakBlock;
                    }
                }
                BreakBlockS->nBreakBlock = NewCount;
            }
            
            { // Spotter
                SPOTTER_STATE * SpotterS = &AppState->SpotterS;
                uint32 NewCount = 0;
                for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
                    SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
                    
                    // TODO: Tune this!
                    rect R = MinkSub( DamageBound, RectCD( Vec2( 0.0f, 0.0f ), SPOTTER_DIM ) );
                    if( !IsInBound( Spotter.Position, R ) ) {
                        SpotterS->Spotter[ NewCount++ ] = Spotter;
                    } else {
                        AddSpotterKill( AppState, Spotter.Position );
                    }
                }
                SpotterS->nSpotter = NewCount;
            }
            
            { // Popper
                for( uint32 iPopperA = 0; iPopperA < PopperS->nPopper; iPopperA++ ) {
                    if( iPopperA != iPopper ) {
                        POPPER * PopperA = PopperS->Popper + iPopperA;
                        if( IsInBound( PopperA->Position, DamageBound ) ) {
                            PopperA->Mode        = PopperMode_IsPopping;
                            PopperA->Velocity.y  = 8.0f;
                            PopperA->Timer       = 0.0f;
                        }
                    }
                }
            }
        }
    }
}

internal void
DrawPopper( RENDER_PASS * Pass, POPPER Popper ) {
    rect R = RectBCD( Popper.Position, POPPER_DIM );
    
    vec4 ColorA = ToColor( 100, 100, 20 );
    if( Popper.Mode == PopperMode_IsStunned ) {
        ColorA = ToColor( 50, 50, 10 );
    }
    vec4 ColorB = ToColor( 200, 200, 40 );
    
    if( Popper.Mode == PopperMode_IsDead ) {
        vec2 DamageDim   = Vec2( TILE_WIDTH * 5.0f, TILE_HEIGHT * 5.0f );
        rect DamageBound = RectCD( Popper.Position, DamageDim );
        DrawRect( Pass, DamageBound, COLOR_YELLOW );
    }
    
    DrawRect( Pass, R, ColorA );
    if( Popper.Charge > 0.0f ) {
        rect S = R;
        S.Top = lerp( S.Bottom, Popper.Charge, S.Top );
        
        DrawRect( Pass, S, ColorB );
    }
}

internal void
DrawPopper( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    POPPER_STATE * PopperS = Draw->PopperS;
    
    for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
        POPPER Popper = PopperS->Popper[ iPopper ];
        DrawPopper( Pass, Popper );
    }
}

internal void
DrawPopperDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    POPPER_STATE * PopperS = Draw->PopperS;
    
    for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
        POPPER Popper = PopperS->Popper[ iPopper ];
    }
}

internal void
FinalizePopper( APP_STATE * AppState ) {
    POPPER_STATE * PopperS = &AppState->PopperS;
    
    uint32 NewCount = 0;
    for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
        POPPER * Popper = PopperS->Popper + iPopper;
        
        if( ( Popper->Mode == PopperMode_IsStunned ) && ( Popper->Timer >= POPPER_STUNNED_TARGET_TIME ) ) {
            Popper->Mode   = PopperMode_Move;
            Popper->Charge = 0.0f;
        }
        
        if( Popper->Mode != PopperMode_IsDead ) {
            PopperS->Popper[ NewCount++ ] = *Popper;
        }
    }
    PopperS->nPopper = NewCount;
}

internal void
Editor_DrawPopper( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__POPPER_STATE * PopperS = &Editor->PopperS;
    
    for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
        EDITOR__POPPER Src     = PopperS->Popper[ iPopper ];
        POPPER         Popper = PopperC( Src.Position );
        DrawPopper( Pass, Popper );
    }
}


#define VERSION__POPPER  ( 1 )
#define FILETAG__POPPER  ( "POPPER__" )
internal void
SavePopper( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir,
           char * FileName ) {
    char * FileTag = FILETAG__POPPER;
    uint32 Version = VERSION__POPPER;
    EDITOR__POPPER_STATE * PopperS = &Editor->PopperS;
    
    if( PopperS->nPopper > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, PopperS->nPopper );
        for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
            EDITOR__POPPER Popper = PopperS->Popper[ iPopper ];
            _writem( output, EDITOR__POPPER, Popper );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__POPPER
ReadPopper( uint32 Version, uint8 ** Ptr ) {
    EDITOR__POPPER Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            vec2 Position = _read( ptr, vec2 );
            
            EDITOR__POPPER Popper = {};
            Popper.Position = Position;
            
            Result = Popper;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadPopper( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__POPPER;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        POPPER_STATE * PopperS = &AppState->PopperS;
        
        PopperS->nPopper = _read( ptr, uint32 );
        for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
            EDITOR__POPPER Src = ReadPopper( Version, &ptr );
            
            POPPER Popper = PopperC( Src.Position );
            PopperS->Popper[ iPopper ] = Popper;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadPopper( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__POPPER;
    EDITOR_STATE             * Editor      = &AppState->Editor;
    EDITOR__POPPER_STATE * PopperS = &Editor->PopperS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        PopperS->nPopper = _read( ptr, uint32 );
        for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
            EDITOR__POPPER Src = ReadPopper( Version, &ptr );
            PopperS->Popper[ iPopper ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputPopperToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS              Stat      = Editor->Stat;
    EDITOR__POPPER_STATE * PopperS = &Editor->PopperS;
    
    for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
        EDITOR__POPPER Popper = PopperS->Popper[ iPopper ];
        
        vec4 Color = ToColor( 100, 100, 20 );
        rect Bound = RectBCD( Popper.Position, POPPER_DIM );
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}