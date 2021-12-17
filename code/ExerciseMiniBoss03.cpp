
internal void
InitExerciseMiniBoss03( APP_STATE * AppState ) {
    EXERCISE_MINIBOSS03 * Boss = &AppState->ExerciseMiniBoss03;
    Boss->IsActive = true;
    
    LEVEL_STATS Stat = AppState->Stat;
    
    Boss->Position = GetPOfLevel( Vec2( 28.0f, 2.0f ), Stat.Bound );
    Boss->Velocity = {};
    
    Boss->Mode        = ExerciseMiniBoss03Mode_MoveToSweep;
    Boss->Destination = GetPOfLevel( Vec2( 26.0f, 2.0f ), Stat.Bound );
    
    Boss->Timer  = 0.0f;
    Boss->Health    = 20;
    Boss->MaxHealth = 20;
    
    Boss->Mode = ExerciseMiniBoss03Mode_Spawn;
    Boss->Position = GetPOfLevel( Vec2( 28.0f, 2.5f ), Stat.Bound );
    
    Boss->Mode          = ExerciseMiniBoss03Mode_Squash;
    Boss->Push_MaxSpeed = 7.0f;
    Boss->Push_EndTimer = 0.0f;
}

internal boo32
IsInMiniBoss03ChargeBound( EXERCISE_MINIBOSS03 * Boss, vec2 Pos, rect CollBound ) {
    boo32 Result = {};
    
    vec2 Dim   = EXERCISE_MINIBOSS03_DIM;
    rect Bound = RectBCD( Boss->Position, Dim );
    if( IsInBound( Pos, Bound ) ) {
        Result = true;
    }
    return Result;
}

internal boo32
IsTouchingExerciseMiniBoss03( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    EXERCISE_MINIBOSS03 * Boss = &AppState->ExerciseMiniBoss03;
    
    boo32 Result = {};
    if( Boss->IsActive ) {
        vec2 Dim   = Boss->Dim;
        rect Bound = RectBCD( Boss->Position, Dim );
        if( IsInBound( Pos, Bound ) ) {
            Result = true;
        }
    }
    return Result;
}

internal boo32
IsInGrabBoundOfExerciseMiniBoss03( APP_STATE * AppState, vec2 Pos ) {
    boo32 Result = false;
    
    EXERCISE_MINIBOSS03 * Boss = &AppState->ExerciseMiniBoss03;
    if( Boss->IsActive ) {
        boo32 CanGrab = ( Boss->Mode == ExerciseMiniBoss03Mode_Squash );
        
        flo32 Margin = TILE_WIDTH * 0.1f;
        if( CanGrab ) {
            rect Bound = RectBCD( Boss->Position, Boss->Dim );
            Bound.Right  = Bound.Left;
            Bound.Left  -= PLAYER_HALF_WIDTH;
            Bound = AddRadius( Bound, Margin );
            
            if( IsInBound( Pos, Bound ) ) {
                Result = true;
            }
        }
    }
    
    return Result;
}

internal void
UpdateExerciseMiniBoss03( APP_STATE * AppState, flo32 dT ) {
    EXERCISE_MINIBOSS03 * Boss   = &AppState->ExerciseMiniBoss03;
    PLAYER_STATE        * Player = &AppState->Player;
    
    if( Boss->IsActive ) {
        if( Boss->Mode == ExerciseMiniBoss03Mode_IsDying ) {
            Boss->DoBlast     = false;
            Boss->DoStomp     = false;
            Boss->DoStun      = false;
            Boss->DoStagger   = false;
            Boss->IsStaggered = false;
        }
        
        if( Boss->DoPop ) {
            Boss->DoPop = false;
            
            uint32 Damage = MinValue( Boss->Health, ( uint32 )1 );
            Boss->Health -= Damage;
            
            Boss->IsStaggered       = true;
            Boss->IsStaggered_Timer = 0.0f;
            Boss->nStagger++;
        }
        
        if( Boss->IsStaggered ) {
            Boss->IsStaggered_Timer += dT;
        } else {
            Boss->Timer += dT;
        }
        
        if( Boss->nStagger >= 1 ) {
            Boss->nStagger    = 0;
            Boss->IsStaggered = false;
            Boss->Mode        = ExerciseMiniBoss03Mode_IsStunned;
            Boss->Timer       = 0.0f;
            
            LEVEL_STATS Stat = AppState->Stat;
            Boss->Velocity.y  = 8.0f;
            Boss->Position.y += ( TILE_HEIGHT * 6.0f );
        }
        
        flo32 DestMargin   = TILE_WIDTH * 0.25f;
        flo32 DestMarginSq = DestMargin * DestMargin;
        flo32 DistToDestSq = GetLengthSq( Boss->Destination - Boss->Position );
        boo32 IsAtDest     = ( DistToDestSq <= DestMarginSq );
        
        LEVEL_STATS Stat = AppState->Stat;
        flo32 MidX = ( Stat.Bound.Left + Stat.Bound.Right ) * 0.5f;
        
        vec2  Dir      = {};
        flo32 MaxSpeed  = EXERCISE_MINIBOSS03_MOVE_SPEED;
        flo32 Friction  = EXERCISE_MINIBOSS03_MOVE_FRICTION;
        flo32 PushSpeed = 0.0f;
        
        boo32 DoBlast   = false;
        boo32 DoSpikes  = false;
        boo32 DoStomp   = false;
        boo32 DoSweep   = false;
        flo32 SweepDirX = 0.0f;
        
        switch( Boss->Mode ) {
            case ExerciseMiniBoss03Mode_Spawn: {
                Boss->Dim = EXERCISE_MINIBOSS03_SPAWN_DIM;
            } break;
            
            case ExerciseMiniBoss03Mode_Squash: {
                Boss->Dim = EXERCISE_MINIBOSS03_SQUASH_DIM;
                
                flo32 TargetTimeA = EXERCISE_MINIBOSS03_SQUASH_WARMUP_TARGET_TIME;
                flo32 TargetTimeB = TargetTimeA + ( 60.0f / 60.0f );
                
                if( Boss->Timer < TargetTimeA ) {
                    Dir.y = -1.0f;
                }
                if( Boss->Timer >= TargetTimeA ) {
                    Dir.x = -1.0f;
                    
                    if( ( Player->GrabExerciseMiniBoss03_IsActive ) && ( Boss->Push_DirX != 0.0f ) ) {
                        flo32 Rate = 1.0f / 1.0f;
                        PushSpeed = EXERCISE_MINIBOSS03_PUSH_WALK_SPEED;
                        if( Player->Run_IsActive ) {
                            PushSpeed = EXERCISE_MINIBOSS03_PUSH_RUN_SPEED;
                            Rate      = 1.0f / 1.0f;
                        }
                        
                        Boss->Push_MaxSpeed = MaxValue( Boss->Push_MaxSpeed - Rate * dT, 0.0f );
                    }
                    
                    MaxSpeed = Boss->Push_MaxSpeed;
                    Friction = EXERCISE_MINIBOSS03_PUSH_FRICTION;
                }
                
                boo32 DoEndTimer = ( ( Boss->Timer >= TargetTimeB ) && ( Boss->Velocity.x > -0.5f ) )
                    || ( Boss->Position.x < ( Stat.Bound.Left + TILE_WIDTH * 3.0f + Boss->Dim.x * 0.5f ) );
                
                if( DoEndTimer ) {
                    Boss->Push_EndTimer += dT;
                } else {
                    Boss->Push_EndTimer  = 0.0f;
                }
            } break;
            
            case ExerciseMiniBoss03Mode_Move:
            case ExerciseMiniBoss03Mode_MoveToSweep: {
                if( !IsAtDest ) {
                    flo32 dX = Boss->Destination.x - Boss->Position.x;
                    if( dX != 0.0f ) {
                        Dir.x = dX / fabsf( dX );
                    }
                }
            } break;
            
            case ExerciseMiniBoss03Mode_Sweep: {
                flo32 NX    = Boss->Destination.x - MidX;
                flo32 DirX  = NX / fabsf( NX );
                
                if( Boss->Timer >= EXERCISE_MINIBOSS03_SWEEP_WARMUP_TARGET_TIME ) {
                    flo32 MarginX = TILE_WIDTH * 10.0f;
                    flo32 StopX = MidX + DirX * MarginX;
                    
                    boo32 DoSlowDown = 
                        ( ( DirX > 0.0f ) && ( Boss->Position.x >= StopX ) ) || ( ( DirX < 0.0f ) && ( Boss->Position.x <= StopX ) );
                    
                    if( DoSlowDown ) {
                        Friction = 4.0f;
                    } else {
                        MaxSpeed = EXERCISE_MINIBOSS03_SWEEP_SPEED;
                        Friction = EXERCISE_MINIBOSS03_SWEEP_FRICTION;
                        Dir.x = DirX;
                    }
                }
                
                DoSweep   = true;
                SweepDirX = DirX;
            } break;
            
            case ExerciseMiniBoss03Mode_Stomp: {
                flo32 TargetTimeA = EXERCISE_MINIBOSS03_STOMP_WARMUP_TARGET_TIME;
                flo32 TargetTimeB = TargetTimeA + EXERCISE_MINIBOSS03_STOMP_TARGET_TIME;
                flo32 TargetTimeC = TargetTimeB + EXERCISE_MINIBOSS03_STOMP_COOLDOWN_TARGET_TIME;
                
                if( Boss->Timer >= TargetTimeA ) {
                    if( Boss->DoStomp ) {
                        Boss->DoStomp = false;
                        
                        flo32 TargetTime     = EXERCISE_MINIBOSS03_STOMP_TARGET_TIME;
                        flo32 HalfTargetTime = TargetTime * 0.5f;
                        
                        flo32 MarginX = TILE_WIDTH * 1.5f;
                        flo32 DestXA = Player->Position.x;
                        flo32 DestXB = Player->Position.x + Player->Velocity.x * TargetTime;
                        flo32 DestX  = lerp( DestXA, 0.25f, DestXB );
                        DestX = Clamp( DestX, Stat.Bound.Left + MarginX, Stat.Bound.Right - MarginX );
                        
                        flo32 MaxDist = TILE_WIDTH * 16.0f;
                        flo32 DistX   = DestX - Boss->Position.x;
                        DistX = Clamp( DistX, -MaxDist, MaxDist );
                        
                        flo32 VelY = EXERCISE_MINIBOSS03_GRAVITY * HalfTargetTime;
                        flo32 VelX = DistX / TargetTime;
                        
                        Boss->Mode     = ExerciseMiniBoss03Mode_Stomp;
                        Boss->Velocity = Vec2( VelX, VelY );
                        Boss->nStomp++;
                    }
                    if( Boss->Timer < TargetTimeB ) {
                        Friction = 0.0f;
                    }
                    if( Boss->Timer >= TargetTimeB ) {
                        Friction = 8.0f;
                    }
                }
                
                flo32 TargetTimeD = lerp( TargetTimeA, 0.5f, TargetTimeB );
                if( Boss->Timer >= TargetTimeD ) {
                    DoStomp = true;
                }
            } break;
            
            case ExerciseMiniBoss03Mode_Spikes: {
                flo32 TargetTimeA = EXERCISE_MINIBOSS03_SPIKES_WARMUP_TARGET_TIME;
                flo32 TargetTimeB = TargetTimeA + EXERCISE_MINIBOSS03_SPIKES_TARGET_TIME;
                flo32 TargetTimeC = TargetTimeB + EXERCISE_MINIBOSS03_SPIKES_COOLDOWN_TARGET_TIME;
                
                if( ( Boss->Timer >= TargetTimeA ) && ( Boss->Timer < TargetTimeB ) ) {
                    DoSpikes = true;
                }
            } break;
            
            case ExerciseMiniBoss03Mode_IsStunned: {
                Boss->Dim = EXERCISE_MINIBOSS03_STUNNED_DIM;
                Dir.y = -1.0f;
            } break;
            
            case ExerciseMiniBoss03Mode_IsDying: {
                flo32 TargetTimeA = ( 140.0f / 60.0f );
                flo32 TargetTimeB = TargetTimeA + ( 120.0f / 60.0f );
                if( Boss->Timer >= TargetTimeA ) {
                    Dir.x    = 1.0f;
                    MaxSpeed = 28.0f;
                    Friction = 8.0f;
                }
            } break;
        }
        
        vec2 Accel = {};
        Accel.x = ( ( Dir.x * MaxSpeed ) + ( Boss->Push_DirX * PushSpeed ) - Boss->Velocity.x ) * Friction;
        Accel.y = Dir.y * EXERCISE_MINIBOSS03_GRAVITY;
        
        Boss->Velocity += Accel * dT;
        vec2 dPos  = Accel * ( dT * dT * 0.5f ) + Boss->Velocity * dT;
        
        boo32 DoLevelIntersect = ( Boss->Mode != ExerciseMiniBoss03Mode_IsDying );
        
        vec2 PrevPos = Boss->Position;
        
        // TODO: Tune this!!
        rect LevelCollBound = RectCD( Vec2( 0.0f, 0.0f ), Vec2( EXERCISE_MINIBOSS03_WIDTH - TILE_WIDTH * 5.0f, 0.0f ) );
        // TODO: Tune this!!
        rect             CollBound = RectBCD( Vec2( 0.0f, 0.0f ), Boss->Dim );
        CollBound.Left -= PLAYER_WIDTH;
        
        COLLISION_RESULT Coll      = CollisionResult( Boss->Position, Boss->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll( &BestIntersect, AppState, Coll, CollBound );
            
            if( DoLevelIntersect ) {
                DoesIntersectLevelBound( &BestIntersect, AppState, Coll, LevelCollBound );
            }
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        Coll.Pos += Coll.dPos;
        
        Boss->Position = Coll.Pos;
        Boss->Velocity = Coll.Vel;
        
        boo32 CanPushPlayer = ( Boss->Mode == ExerciseMiniBoss03Mode_Squash );
        if( CanPushPlayer ) {
            vec2 NewPos    = Boss->Position;
            vec2 ChangePos = NewPos - PrevPos;
            
            if( ChangePos.x != 0.0f ) {
                rect PlayerDim = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM );
                rect BossBound = MinkSub( RectBCD( Boss->Position, Boss->Dim ), PlayerDim );
                if( IsInBound( Player->Position, BossBound ) ) {
                    Player->Position.x += ChangePos.x;
                }
            }
        }
        
        boo32 CanBeCharged = ( Boss->Mode == ExerciseMiniBoss03Mode_IsStunned );
        
        boo32 CancelDamageBound = false;
        if( ( CanBeCharged ) && ( Player->Dash_IsActive ) ) {
            vec2 ChargeDim   = EXERCISE_MINIBOSS03_STUNNED_DIM;
            rect ChargeBound = RectBCD( Boss->Position, ChargeDim );
            ChargeBound = MinkSub( ChargeBound, RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM ) );
            
            if( IsInBound( Player->Position, ChargeBound ) ) {
                Player->Dash_IsActive = false;
                Player->Velocity   = Vec2( -40.0f, 8.0f );
                Player->DoBossBlowback     = true;
                Player->BossBlowback_Timer = 0.0f;
                Player->Jump_IsActive = false;
                Player->Jump_DoDampen = false;
                
                uint32 Damage = MinValue( Boss->Health, ( uint32 )1 );
                Boss->Health -= Damage;
                
                Boss->Velocity += Vec2( 8.0f, 4.0f );
            }
        }
        
        if( Boss->DoStagger ) {
            Boss->DoStagger = false;
            
            CancelDamageBound = true;
            flo32 DirX = Player->Velocity.x / fabsf( Player->Velocity.x );
            
            Player->Dash_IsActive = false;
            Player->Velocity   = Vec2( 32.0f * -DirX, 6.0f );
            
            if( Boss->Mode == ExerciseMiniBoss03Mode_IsStunned ) {
                if( Boss->Timer < EXERCISE_MINIBOSS03_STUNNED_DOWN_TARGET_TIME ) {
                    Boss->Velocity += Vec2( 16.0f * DirX, 8.0f );
                    
                    flo32 IncTime = ( 60.0f / 60.0f );
                    Boss->Timer = MinValue( Boss->Timer + IncTime, EXERCISE_MINIBOSS03_STUNNED_DOWN_TARGET_TIME );
                } else {
                    Boss->Velocity += Vec2( 8.0f * DirX, 4.0f );
                }
            } else {
                Boss->nStagger++;
                
                if( Boss->nStagger < 3 ) {
                    Boss->IsStaggered       = true;
                    Boss->IsStaggered_Timer = 0.0f;
                    
                    flo32 BoostX = 16.0f;
                    if( Friction == 0.0f ) {
                        BoostX = 1.0f;
                    }
                    Boss->Velocity += Vec2( BoostX * DirX, 8.0f );
                } else if( Boss->nStagger >= 3 ) {
                    Boss->nStagger    = 0;
                    Boss->IsStaggered = false;
                    Boss->Mode        = ExerciseMiniBoss03Mode_IsStunned;
                    Boss->Timer       = 0.0f;
                    Boss->Velocity   += Vec2( 24.0f * DirX, 8.0f );
                    Boss->DoBlast     = true;
                }
            }
            Boss->Velocity.y = MinValue( Boss->Velocity.y, 32.0f );
        }
        
        if( Boss->DoStun ) {
            Boss->DoStun = false;
            Boss->nStagger    = 0;
            Boss->IsStaggered = false;
            Boss->Mode        = ExerciseMiniBoss03Mode_IsStunned;
            Boss->Timer       = 0.0f;
            Boss->Velocity   += Vec2( 24.0f * Boss->DoStunX, 8.0f );
            Boss->DoBlast     = true;
            
            uint32 Damage = MinValue( Boss->Health, ( uint32 )2 );
            Boss->Health -= Damage;
        }
        
        boo32 CanDamagePlayer = ( Boss->Mode != ExerciseMiniBoss03Mode_IsStunned )
            && ( Boss->Mode != ExerciseMiniBoss03Mode_IsDying )
            && ( !Boss->IsStaggered )
            && ( !Player->NoDamage )
            && ( !CancelDamageBound );
        
        if( CanDamagePlayer ) {
            vec2 DamageDim = EXERCISE_MINIBOSS03_DIM;
            DamageDim.x -= ( TILE_WIDTH  * 0.2f );
            DamageDim.y -= ( TILE_HEIGHT * 0.1f );
            rect DamageBound = RectBCD( Boss->Position, DamageDim );
            
            vec2 SpikesDim = GetDim( DamageBound );
            rect PlayerBound = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM );
            
            DamageBound = MinkSub( DamageBound, PlayerBound );
            boo32 DoDamage = IsInBound( Player->Position, DamageBound );
            
            if( DoSpikes ) {
                vec2 SpikesDimA   = SpikesDim + Vec2( ( EXERCISE_MINIBOSS03_SPIKES_HEIGHT - TILE_WIDTH * 0.4f ) * 2.0f, -TILE_HEIGHT * 0.25f );
                vec2 SpikesDimB   = SpikesDim + Vec2( -TILE_HEIGHT * 0.25f * 2.0f, EXERCISE_MINIBOSS03_SPIKES_HEIGHT - TILE_HEIGHT * 0.25f );
                
                rect SpikesBoundA = MinkSub( RectBCD( Boss->Position, SpikesDimA ), PlayerBound );
                rect SpikesBoundB = MinkSub( RectBCD( Boss->Position, SpikesDimB ), PlayerBound );
                
                DoDamage =
                    ( IsInBound( Player->Position, SpikesBoundA ) ) || ( IsInBound( Player->Position, SpikesBoundB ) );
            }
            if( DoStomp ) {
                vec2 SpikesDimA = SpikesDim + Vec2( -TILE_WIDTH * 0.25f * 2.0f, 0.0f );
                SpikesDimA.y = EXERCISE_MINIBOSS03_SPIKES_HEIGHT - TILE_HEIGHT * 0.25f;
                
                rect SpikesBoundA = MinkSub( RectBCD( Boss->Position, SpikesDim  ), PlayerBound );
                rect SpikesBoundB = MinkSub( RectTCD( Boss->Position, SpikesDimA ), PlayerBound );
                
                DoDamage = 
                    ( IsInBound( Player->Position, SpikesBoundA ) ) || ( IsInBound( Player->Position, SpikesBoundB ) );
            }
            if( DoSweep ) {
                rect BossBound  = RectBCD( Boss->Position, EXERCISE_MINIBOSS03_DIM );
                vec2 SpikesDimA = Vec2( EXERCISE_MINIBOSS03_SPIKES_HEIGHT - TILE_WIDTH * 0.4, SpikesDim.y - TILE_HEIGHT * 0.25f );
                
                rect SpikesBoundA = {};
                if( SweepDirX < 0.0f ) {
                    SpikesBoundA = MinkSub( RectBRD( GetBL( BossBound ), SpikesDimA ), PlayerBound );
                } else {
                    SpikesBoundA = MinkSub( RectBLD( GetBR( BossBound ), SpikesDimA ), PlayerBound );
                }
                
                DoDamage = ( IsInBound( Player->Position, SpikesBoundA ) );
            }
            
            if( DoDamage ) {
                DamagePlayer( AppState, 1 );
            }
        }
        
        if( DoBlast ) {
            flo32 DX   = Player->Position.x - Boss->Position.x;
            flo32 DirX = DX / fabsf( DX );
            Player->Velocity = Vec2( 40.0f * DirX, 10.0f );
        }
        
        if( ( Boss->Mode != ExerciseMiniBoss03Mode_IsDying ) && ( Boss->Health == 0 ) ) {
            Boss->Mode        = ExerciseMiniBoss03Mode_IsDying;
            Boss->Timer       = 0.0f;
            Boss->DoBlast     = false;
            Boss->DoStomp     = false;
            Boss->DoStun      = false;
            Boss->DoStagger   = false;
            Boss->IsStaggered = false;
        }
        
        if( Player->GrabExerciseMiniBoss03_IsActive ) {
            flo32 MarginX = COLLISION_EPSILON_OFFSET * 5.0f;
            rect  Bound   = RectCD( Boss->Position, Boss->Dim );
            flo32 X       = Bound.Left - ( PLAYER_HALF_WIDTH + MarginX );
            
            Player->Position.x = X;
            Boss->Push_DirX    = 0.0f;
        }
    }
}

internal void
DrawExerciseMiniBoss03( RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, APP_STATE * AppState, DRAW_STATE * Draw ) {
    EXERCISE_MINIBOSS03 * Boss = Draw->ExerciseMiniBoss03;
    
    if( Boss->IsActive ) {
        { // BOSS
            vec4 Color = COLOR_RED;
            if( Boss->IsStaggered ) {
                flo32  CycleTime = ( 4.0f / 60.0f );
                uint32 Cycle     = ( uint32 )( Boss->IsStaggered_Timer / CycleTime );
                if( ( Cycle % 2 ) == 0 ) {
                    Color = ToColor( 255, 50, 50 );
                } else {
                    Color = ToColor( 200,  0,  0 );
                }
            }
            
            vec2 Dim   = Boss->Dim;
            rect Bound = RectBCD( Boss->Position, Dim );
            DrawRect( Pass_Game, Bound, Color );
        }
        
#if 0        
        boo32 DoBlast  = false;
        flo32 DoBlastT = 0.0f;
        { // BOSS
            vec4 Color = ToColor( 120, 20, 20 );
            if( Boss->Mode == ExerciseMiniBoss03Mode_IsStunned ) {
                if( Boss->Timer < EXERCISE_MINIBOSS03_STUNNED_DOWN_TARGET_TIME ) {
                    Color = ToColor( 60, 20, 20 );
                } else {
                    flo32 TargetTime = ( 4.0f / 60.0f );
                    flo32 Time = Boss->Timer - EXERCISE_MINIBOSS03_STUNNED_DOWN_TARGET_TIME;
                    if( Time < TargetTime ) {
                        DoBlast  = true;
                        DoBlastT = Clamp01( Time / TargetTime );
                    }
                }
            }
            
            { // BLAST
                if( DoBlast ) {
                    vec2  Dim   = EXERCISE_MINIBOSS03_DIM;
                    flo32 Scale = lerp( 1.2f, DoBlastT, 1.5f );
                    rect  BlastBound = RectBCD( Boss->Position, Dim * Scale );
                    DrawRect( Pass_Game, BlastBound, COLOR_WHITE );
                }
            }
            
            vec2 Pos = Boss->Position;
            if( Boss->Mode == ExerciseMiniBoss03Mode_IsDying ) {
                Pos += RandomDirection2D() * ( TILE_WIDTH * 0.1f );
            }
            
            if( Boss->Mode == ExerciseMiniBoss03Mode_IsDying ) {
                flo32 Radians = Boss->Timer * PI * 1.0f;
                flo32 Alpha = 1.0f - ( ( cosf( Radians ) + 1.0f ) * 0.25f );
                Color *= Alpha;
            }
            
            vec2 Dim = EXERCISE_MINIBOSS03_DIM;
            
            boo32 IsInSpikesMode = false;
            if( Boss->Mode == ExerciseMiniBoss03Mode_Spikes ) {
                flo32 TargetTimeA = EXERCISE_MINIBOSS03_SPIKES_WARMUP_TARGET_TIME;
                flo32 TargetTimeB = TargetTimeA + EXERCISE_MINIBOSS03_SPIKES_TARGET_TIME;
                flo32 TargetTimeC = TargetTimeB + EXERCISE_MINIBOSS03_SPIKES_COOLDOWN_TARGET_TIME;
                
                if( Boss->Timer < TargetTimeA ) {
                    flo32 t      = Clamp01( Boss->Timer / TargetTimeA );
                    flo32 ScaleX = lerp( 1.0f, t, 0.9f  );
                    flo32 ScaleY = lerp( 1.0f, t, 0.95f );
                    Dim *= Vec2( ScaleX, ScaleY );
                }
                if( ( Boss->Timer >= TargetTimeA ) && ( Boss->Timer < TargetTimeB ) ) {
                    IsInSpikesMode = true;
                }
            }
            
            boo32 IsInSweepMode_Left  = false;
            boo32 IsInSweepMode_Right = false;
            if( Boss->Mode == ExerciseMiniBoss03Mode_Sweep ) {
                LEVEL_STATS Stat = AppState->Stat;
                flo32 MidX = ( Stat.Bound.Left + Stat.Bound.Right ) * 0.5f;
                if( Boss->Destination.x < MidX ) {
                    IsInSweepMode_Left  = true;
                } else {
                    IsInSweepMode_Right = true;
                }
            }
            
            boo32 IsInStompMode = false;
            if( Boss->Mode == ExerciseMiniBoss03Mode_Stomp ) {
                flo32 TargetTimeA = EXERCISE_MINIBOSS03_STOMP_WARMUP_TARGET_TIME;
                flo32 TargetTimeB = TargetTimeA + EXERCISE_MINIBOSS03_STOMP_TARGET_TIME;
                flo32 TargetTimeC = TargetTimeB + EXERCISE_MINIBOSS03_STOMP_COOLDOWN_TARGET_TIME;
                
                if( Boss->Timer < TargetTimeA ) {
                    flo32 t      = Clamp01( Boss->Timer / TargetTimeA );
                    flo32 ScaleX = lerp( 1.0f, t, 1.05f  );
                    flo32 ScaleY = lerp( 1.0f, t, 0.85f );
                    Dim *= Vec2( ScaleX, ScaleY );
                }
                
                flo32 TargetTimeD = lerp( TargetTimeA, 0.5f, TargetTimeB );
                if( Boss->Timer >= TargetTimeD ) {
                    IsInStompMode = true;
                }
            }
            
            boo32 DoSpikes_Left   = ( IsInSpikesMode ) || ( IsInSweepMode_Left  );
            boo32 DoSpikes_Right  = ( IsInSpikesMode ) || ( IsInSweepMode_Right );
            boo32 DoSpikes_Top    = ( IsInSpikesMode );
            boo32 DoSpikes_Bottom = ( IsInStompMode );
            
            boo32 DoSpikes = ( DoSpikes_Left ) || ( DoSpikes_Right ) || ( DoSpikes_Top ) || ( DoSpikes_Bottom );
            
            if( DoSpikes ) {
                vec2 HalfDim = Dim * 0.5f;
                
                uint32 nSpikesX = 4;
                uint32 nSpikesY = 3;
                
                flo32 Spike_WidthX = Dim.x / ( flo32 )nSpikesX;
                flo32 Spike_WidthY = Dim.y / ( flo32 )nSpikesY;
                flo32 Spike_Height = TILE_HEIGHT * 2.0f;
                vec2  ScaleX = Vec2( Spike_WidthX / 2.0f, Spike_Height / 2.0f );
                vec2  ScaleY = Vec2( Spike_WidthY / 2.0f, Spike_Height / 2.0f );
                
                flo32 SpikeXOffset = Dim.x / ( ( flo32 )nSpikesX * 2.0f );
                flo32 SpikeYOffset = Dim.y / ( ( flo32 )nSpikesY * 2.0f );
                
                flo32 Left   = Boss->Position.x - HalfDim.x - 1.0f * ScaleY.y;
                flo32 Right  = Boss->Position.x + HalfDim.x + 1.0f * ScaleY.y;
                flo32 Top    = Boss->Position.y + Dim.y     + 1.0f * ScaleX.y;
                flo32 Bottom = Boss->Position.y             - 1.0f * ScaleX.y;
                
                if( DoSpikes_Left ) {
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Left, Boss->Position.y + SpikeYOffset * 1.0f ), ScaleY, PI * 0.5f, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Left, Boss->Position.y + SpikeYOffset * 3.0f ), ScaleY, PI * 0.5f, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Left, Boss->Position.y + SpikeYOffset * 5.0f ), ScaleY, PI * 0.5f, Color );
                }
                
                if( DoSpikes_Right ) {
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Right, Boss->Position.y + SpikeYOffset * 1.0f ), ScaleY, -PI * 0.5f, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Right, Boss->Position.y + SpikeYOffset * 3.0f ), ScaleY, -PI * 0.5f, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Right, Boss->Position.y + SpikeYOffset * 5.0f ), ScaleY, -PI * 0.5f, Color );
                }
                
                if( DoSpikes_Top ) {
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Boss->Position.x - SpikeXOffset * 3.0f, Top ), ScaleX, 0.0f, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Boss->Position.x - SpikeXOffset * 1.0f, Top ), ScaleX, 0.0f, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Boss->Position.x + SpikeXOffset * 1.0f, Top ), ScaleX, 0.0f, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Boss->Position.x + SpikeXOffset * 3.0f, Top ), ScaleX, 0.0f, Color );
                }
                
                if( DoSpikes_Bottom ) {
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Boss->Position.x - SpikeXOffset * 3.0f, Bottom ), ScaleX, PI, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Boss->Position.x - SpikeXOffset * 1.0f, Bottom ), ScaleX, PI, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Boss->Position.x + SpikeXOffset * 1.0f, Bottom ), ScaleX, PI, Color );
                    DrawModel( Pass_Game, ModelID_Triangle, TextureID_WhiteTexture, Vec2( Boss->Position.x + SpikeXOffset * 3.0f, Bottom ), ScaleX, PI, Color );
                }
            }
            
            rect R = RectBCD( Pos, Dim );
            DrawRect( Pass_Game, R, Color );
        }
#endif
        
        { // HEALTH BAR
            vec2 App_Dim = AppState->App_Dim;
            vec2 Pos = GetT( AppState->App_Bound, Vec2( 0.95f, 0.95f ) );
            vec2 Dim = Vec2( App_Dim.x * 0.5f, App_Dim.y * 0.05f );
            rect R   = RectTRD( Pos, Dim );
            
            vec2 Label_Pos = GetTL( R ) + Vec2( Dim.x * 0.03f, -Dim.y * 0.06f );
            DrawString( Pass_UI, AppState->Font, "BOSS", Label_Pos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
            
            vec2 Bar_Dim = Vec2( Dim.x * 0.98f, Dim.y * 0.5f );
            vec2 Bar_Pos = GetBC( R ) + Vec2( 0.0f, Dim.y * 0.1f );
            
            flo32 t = ( flo32 )Boss->Health / ( flo32 )Boss->MaxHealth;
            rect  S = RectBCD( Bar_Pos, Bar_Dim );
            S.Right = lerp( S.Left, t, S.Right );
            
            DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.03f ) );
            DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
            
            DrawRect       ( Pass_UI, S, COLOR_RED );
        }
    }
}

internal void
DrawExerciseMiniBoss03Debug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    EXERCISE_MINIBOSS03 * Boss   = Draw->ExerciseMiniBoss03;
    PLAYER_STATE        * Player = Draw->Player;
    
    if( Boss->IsActive ) {
        boo32 DoSpikes = false;
        if( Boss->Mode == ExerciseMiniBoss03Mode_Spikes ) {
            flo32 TargetTimeA = EXERCISE_MINIBOSS03_SPIKES_WARMUP_TARGET_TIME;
            flo32 TargetTimeB = TargetTimeA + EXERCISE_MINIBOSS03_SPIKES_TARGET_TIME;
            flo32 TargetTimeC = TargetTimeB + EXERCISE_MINIBOSS03_SPIKES_COOLDOWN_TARGET_TIME;
            
            if( ( Boss->Timer >= TargetTimeA ) && ( Boss->Timer < TargetTimeB ) ) {
                DoSpikes = true;
            }
        }
        
        boo32 DoStomp = false;
        if( Boss->Mode == ExerciseMiniBoss03Mode_Stomp ) {
            flo32 TargetTimeA = EXERCISE_MINIBOSS03_STOMP_WARMUP_TARGET_TIME;
            flo32 TargetTimeB = TargetTimeA + EXERCISE_MINIBOSS03_STOMP_TARGET_TIME;
            flo32 TargetTimeC = TargetTimeB + EXERCISE_MINIBOSS03_STOMP_COOLDOWN_TARGET_TIME;
            
            flo32 TargetTimeD = lerp( TargetTimeA, 0.5f, TargetTimeB );
            if( Boss->Timer >= TargetTimeD ) {
                DoStomp = true;
            }
        }
        
        boo32 CanBeCharged = ( Boss->Mode == ExerciseMiniBoss03Mode_Move )
            || ( Boss->Mode == ExerciseMiniBoss03Mode_MoveToSweep )
            || ( Boss->Mode == ExerciseMiniBoss03Mode_Stomp )
            || ( Boss->Mode == ExerciseMiniBoss03Mode_IsStunned )
            || ( ( Boss->Mode == ExerciseMiniBoss03Mode_Spikes ) && ( !DoSpikes ) );
        
        if( ( CanBeCharged ) && ( Player->Dash_IsActive ) ) {
            vec2 ChargeDim   = EXERCISE_MINIBOSS03_DIM;
            rect ChargeBound = RectBCD( Boss->Position, ChargeDim );
            if( Player->Velocity.x > 0.0f ) {
                ChargeBound.Right -= TILE_WIDTH * 0.4f;
            } else {
                ChargeBound.Left  += TILE_WIDTH * 0.4f;
            }
            DrawRectOutline( Pass, ChargeBound, COLOR_MAGENTA );
        }
        
        boo32 CanDamagePlayer = ( Boss->Mode != ExerciseMiniBoss03Mode_IsStunned )
            && ( Boss->Mode != ExerciseMiniBoss03Mode_IsDying )
            && ( !Boss->IsStaggered )
            && ( !Player->NoDamage );
        
        if( CanDamagePlayer ) {
            vec2 DamageDim = EXERCISE_MINIBOSS03_DIM;
            DamageDim.x -= ( TILE_WIDTH  * 0.2f );
            DamageDim.y -= ( TILE_HEIGHT * 0.1f );
            rect DamageBound = RectBCD( Boss->Position, DamageDim );
            
            vec2 SpikesDim = GetDim( DamageBound );
            
            if( DoSpikes ) {
                vec2 SpikesDimA   = SpikesDim + Vec2( ( EXERCISE_MINIBOSS03_SPIKES_HEIGHT - TILE_WIDTH * 0.4f ) * 2.0f, -TILE_HEIGHT * 0.25f );
                vec2 SpikesDimB   = SpikesDim + Vec2( -TILE_HEIGHT * 0.25f * 2.0f, EXERCISE_MINIBOSS03_SPIKES_HEIGHT - TILE_HEIGHT * 0.25f );
                
                rect SpikesBoundA = RectBCD( Boss->Position, SpikesDimA );
                rect SpikesBoundB = RectBCD( Boss->Position, SpikesDimB );
                
                DrawRectOutline( Pass, SpikesBoundA, COLOR_YELLOW );
                DrawRectOutline( Pass, SpikesBoundB, COLOR_YELLOW );
            } else if( DoStomp ) {
                vec2 SpikesDimA = SpikesDim + Vec2( -TILE_WIDTH * 0.25f * 2.0f, 0.0f );
                SpikesDimA.y = EXERCISE_MINIBOSS03_SPIKES_HEIGHT - TILE_HEIGHT * 0.25f;
                
                rect SpikesBoundA = RectBCD( Boss->Position, SpikesDim  );
                rect SpikesBoundB = RectTCD( Boss->Position, SpikesDimA );
                
                DrawRectOutline( Pass, SpikesBoundA, COLOR_YELLOW );
                DrawRectOutline( Pass, SpikesBoundB, COLOR_YELLOW );
            } else {
                DrawRectOutline( Pass, DamageBound, COLOR_YELLOW );
            }
        }
    }
    
    DISPLAY_VALUE( uint32, Boss->nStagger );
    DISPLAY_VALUE( boo32, Boss->IsStaggered );
    DISPLAY_VALUE( flo32, Boss->IsStaggered_Timer );
    DISPLAY_VALUE( uint32, Boss->nStomp );
    DISPLAY_VALUE( uint32, Boss->Health );
    DISPLAY_VALUE( flo32, Boss->Timer );
    DISPLAY_VALUE( vec2,  Boss->Velocity );
    DISPLAY_VALUE( vec2,  Boss->Destination );
    DISPLAY_VALUE( vec2,  Boss->Position );
    DISPLAY_VALUE( uint32, ( uint32 )Boss->Mode );
    DISPLAY_VALUE( boo32, Boss->IsActive );
}

internal void
FinalizeExerciseMiniBoss03( APP_STATE * AppState ) {
    EXERCISE_MINIBOSS03 * Boss   = &AppState->ExerciseMiniBoss03;
    PLAYER_STATE        * Player = &AppState->Player;
    
    flo32 DestMargin   = TILE_WIDTH * 0.25f;
    flo32 DestMarginSq = DestMargin * DestMargin;
    flo32 DistToDestSq = GetLengthSq( Boss->Destination - Boss->Position );
    boo32 IsAtDest     = ( DistToDestSq <= DestMarginSq );
    
    LEVEL_STATS Stat = AppState->Stat;
    flo32       MidX = ( Stat.Bound.Left + Stat.Bound.Right ) * 0.5f;
    
    if( Boss->IsActive ) {
        if( Boss->IsStaggered_Timer >= EXERCISE_MINIBOSS03_STAGGERED_TARGET_TIME ) {
            Boss->IsStaggered = false;
        }
        
        switch( Boss->Mode ) {
            case ExerciseMiniBoss03Mode_Squash: {
                if( Boss->Push_EndTimer >= 3.0f ) {
                    Boss->Mode     = ExerciseMiniBoss03Mode_Spawn;
                    Boss->Position = GetPOfLevel( Vec2( 28.0f, 2.5f ), Stat.Bound );
                    
                    if( Player->GrabExerciseMiniBoss03_IsActive ) {
                        Player->GrabExerciseMiniBoss03_IsActive = false;
                        Player->Grab_DirX = 0.0f;
                    }
                }
            } break;
            
            case ExerciseMiniBoss03Mode_Move: {
                if( IsAtDest ) {
                    flo32 Select = RandomF32();
                    if( Select > 0.25f ) {
                        Boss->Mode     = ExerciseMiniBoss03Mode_Stomp;
                        Boss->MaxStomp = RandomU32InRange( 2, 4 );
                        Boss->nStomp   = 0;
                        Boss->DoStomp  = true;
                        Boss->Timer    = 0.0f;
                    } else {
                        Boss->Mode     = ExerciseMiniBoss03Mode_Spikes;
                        Boss->Timer    = 0.0f;
                    }
                }
            } break;
            
            case ExerciseMiniBoss03Mode_MoveToSweep: {
                if( IsAtDest ) {
                    Boss->Mode  = ExerciseMiniBoss03Mode_Sweep;
                    Boss->Timer = 0.0f;
                    
                    if( Boss->Destination.x < MidX ) {
                        Boss->Destination = GetPOfLevel( Vec2( 34.0f, 2.0f ), Stat.Bound );
                    } else {
                        Boss->Destination = GetPOfLevel( Vec2( -2.0f, 2.0f ), Stat.Bound );
                    }
                }
            } break;
            
            case ExerciseMiniBoss03Mode_Sweep: {
                flo32 MarginX = TILE_WIDTH * 10.0f;
                flo32 NX    = Boss->Destination.x - MidX;
                flo32 DirX  = NX / fabsf( NX );
                flo32 StopX = MidX + DirX * MarginX;
                
                boo32 DoSlowDown = 
                    ( ( DirX > 0.0f ) && ( Boss->Position.x >= StopX ) ) || ( ( DirX < 0.0f ) && ( Boss->Position.x <= StopX ) );
                
                flo32 StopEpsilon = 0.5f;
                flo32 Speed       = GetLength( Boss->Velocity );
                
                if( ( DoSlowDown ) && ( Speed <= StopEpsilon ) ) {
                    flo32 OffsetX = 11.0f;
                    
                    Boss->Mode        = ExerciseMiniBoss03Mode_Move;
                    Boss->Destination = GetPOfLevel( Vec2( 16.0f + DirX * OffsetX, 2.0f ), Stat.Bound );
                }
            } break;
            
            case ExerciseMiniBoss03Mode_Stomp: {
                flo32 TargetTimeA = EXERCISE_MINIBOSS03_STOMP_WARMUP_TARGET_TIME;
                flo32 TargetTimeB = TargetTimeA + EXERCISE_MINIBOSS03_STOMP_TARGET_TIME;
                flo32 TargetTimeC = TargetTimeB + EXERCISE_MINIBOSS03_STOMP_COOLDOWN_TARGET_TIME;
                if( Boss->Timer >= TargetTimeC ) {
                    if( Boss->nStomp >= Boss->MaxStomp ) {
                        flo32 Select = RandomF32();
                        if( Select > 0.25f ) {
                            Boss->Mode  = ExerciseMiniBoss03Mode_Spikes;
                            Boss->Timer = 0.0f;
                        } else {
                            Boss->Mode  = ExerciseMiniBoss03Mode_MoveToSweep;
                            Boss->Timer = 0.0f;
                            
                            if( Player->Position.x < Boss->Position.x ) {
                                Boss->Destination = GetPOfLevel( Vec2( 26.0f, 2.0f ), Stat.Bound );
                            } else {
                                Boss->Destination = GetPOfLevel( Vec2(  6.0f, 2.0f ), Stat.Bound );
                            }
                        }
                    } else {
                        Boss->Timer       = 0.0f;
                        Boss->DoStomp     = true;
                    }
                }
            } break;
            
            case ExerciseMiniBoss03Mode_Spikes: {
                flo32 TargetTimeA = EXERCISE_MINIBOSS03_SPIKES_WARMUP_TARGET_TIME;
                flo32 TargetTimeB = TargetTimeA + EXERCISE_MINIBOSS03_SPIKES_TARGET_TIME;
                flo32 TargetTimeC = TargetTimeB + EXERCISE_MINIBOSS03_SPIKES_COOLDOWN_TARGET_TIME;
                
                if( Boss->Timer >= TargetTimeC ) {
                    flo32 Select = RandomF32();
                    if( Select > 0.25f ) {
                        Boss->Mode     = ExerciseMiniBoss03Mode_Stomp;
                        Boss->MaxStomp = RandomU32InRange( 2, 4 );
                        Boss->nStomp   = 0;
                        Boss->DoStomp  = true;
                        Boss->Timer    = 0.0f;
                    } else {
                        Boss->Mode  = ExerciseMiniBoss03Mode_MoveToSweep;
                        Boss->Timer = 0.0f;
                        
                        if( Player->Position.x < Boss->Position.x ) {
                            Boss->Destination = GetPOfLevel( Vec2( 26.0f, 2.0f ), Stat.Bound );
                        } else {
                            Boss->Destination = GetPOfLevel( Vec2(  6.0f, 2.0f ), Stat.Bound );
                        }
                    }
                }
            } break;
            
            case ExerciseMiniBoss03Mode_IsStunned: {
#if 0
                flo32 TargetTime = EXERCISE_MINIBOSS03_STUNNED_DOWN_TARGET_TIME + EXERCISE_MINIBOSS03_STUNNED_RECOVER_TARGET_TIME;
                
                if( Boss->Timer >= TargetTime ) {
                    flo32 Select = RandomF32();
                    if( Select < 0.5f ) {
                        Boss->Mode  = ExerciseMiniBoss03Mode_MoveToSweep;
                        Boss->Timer = 0.0f;
                        
                        if( Player->Position.x < Boss->Position.x ) {
                            Boss->Destination = GetPOfLevel( Vec2( 26.0f, 2.0f ), Stat.Bound );
                        } else {
                            Boss->Destination = GetPOfLevel( Vec2(  6.0f, 2.0f ), Stat.Bound );
                        }
                    } else if( Select > 0.75f ) {
                        Boss->Mode     = ExerciseMiniBoss03Mode_Stomp;
                        Boss->MaxStomp = RandomU32InRange( 2, 4 );
                        Boss->nStomp   = 0;
                        Boss->DoStomp  = true;
                        Boss->Timer    = 0.0f;
                    } else {
                        Boss->Mode     = ExerciseMiniBoss03Mode_Spikes;
                        Boss->Timer    = 0.0f;
                    }
                }
#endif
            } break;
            
            case ExerciseMiniBoss03Mode_IsDying: {
                flo32 TargetTimeA = ( 140.0f / 60.0f );
                flo32 TargetTimeB = TargetTimeA + ( 120.0f / 60.0f );
                if( Boss->Timer >= TargetTimeB ) {
                    Boss->Mode = ExerciseMiniBoss03Mode_EndEncounter;
                }
            } break;
        }
    }
}