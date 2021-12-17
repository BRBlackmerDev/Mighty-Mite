
internal void
InitExerciseBoss( APP_STATE * AppState ) {
    EXERCISE_BOSS * Boss = &AppState->ExerciseBoss;
    Boss->IsActive = true;
    
    LEVEL_STATS Stat = AppState->Stat;
    
    Boss->Mode     = ExerciseBossMode_Run;
    Boss->Position = GetPOfLevel( Vec2( 30.0f, 4.0f ), Stat.Bound );
    Boss->Velocity = {};
    Boss->RunLeft  = true;
    Boss->Jump_TargetTime = 1.0f;
    
    Boss->Timer = 0.0f;
    Boss->Health    = EXERCISE_BOSS_HEALTH;
    Boss->MaxHealth = EXERCISE_BOSS_HEALTH;
}

internal rect
GetExerciseBossBoundToDamagePlayer( EXERCISE_BOSS * Boss ) {
    flo32 OffsetX = PLAYER_HALF_WIDTH - ( TILE_WIDTH * 0.2f );
    
    rect R = RectBCD( Boss->Position, EXERCISE_BOSS_DIM );
    R.Left   -= OffsetX;
    R.Right  += OffsetX;
    R.Bottom += ( TILE_HEIGHT * 0.5f - PLAYER_HEIGHT );
    R.Top    -= TILE_HEIGHT * 0.5f;
    return R;
}

internal void
UpdateExerciseBoss( APP_STATE * AppState, flo32 dT ) {
    EXERCISE_BOSS * Boss   = &AppState->ExerciseBoss;
    PLAYER_STATE  * Player = &AppState->Player;
    
    if( Boss->IsActive ) {
        Boss->Timer += dT;
        
        switch( Boss->Mode ) {
            case ExerciseBossMode_Run: {
                flo32 MaxSpeed = EXERCISE_BOSS_RUN_SPEED;
                flo32 Friction = EXERCISE_BOSS_RUN_FRICTION;
                
                boo32 IsOnGround = IsPosInJumpBound( AppState, Boss->Position, EXERCISE_BOSS_HALF_WIDTH );
                
                boo32 CanJump = ( IsOnGround );
                if( CanJump ) {
                    Boss->Jump_Timer += dT;
                }
                
                if( ( CanJump ) && ( Boss->Jump_Timer >= Boss->Jump_TargetTime ) ) {
                    Boss->Jump_Timer = 0.0f;
                    Boss->Velocity.y = EXERCISE_BOSS_JUMP_SPEED;
                }
                
                vec2 Dir = Vec2( 1.0f, 0.0f );
                if( Boss->RunLeft ) {
                    Dir.x = -1.0f;
                }
                
                vec2 Accel = {};
                Accel.x = ( Dir.x * MaxSpeed - Boss->Velocity.x ) * Friction;
                Accel.y = ( -EXERCISE_BOSS_GRAVITY );
                
                vec2 dPos = Accel * ( dT * dT * 0.5f ) + Boss->Velocity * dT;
                Boss->Velocity += Accel * dT;
                
                COLLISION_RESULT Coll = CollisionResult( Boss->Position, Boss->Velocity, dPos );
                for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesIntersectCollisionTop( &BestIntersect, AppState, Coll, EXERCISE_BOSS_COLLISION_OFFSET );
                    
                    Coll = FinalizeCollision( BestIntersect, Coll );
                }
                Coll.Pos += Coll.dPos;
                
                Boss->Position = Coll.Pos;
                Boss->Velocity = Coll.Vel;
            } break;
            
            
#if 0            
            case ExerciseBossMode_Move: {
                vec2 Dest = Boss->Dest[ Boss->iDest ];
                if( Boss->Position != Dest ) {
                    flo32 MaxSpeed = EXERCISE_BOSS_MOVE_SPEED;
                    flo32 Friction = EXERCISE_BOSS_MOVE_FRICTION;
                    
                    vec2 Dir   = GetNormal( Dest - Boss->Position );
                    vec2 Accel = ( Dir * MaxSpeed - Boss->Velocity ) * Friction;
                    vec2 dPos  = Accel * ( dT * dT * 0.5f ) + Boss->Velocity * dT;
                    
                    Boss->Position += dPos;
                    Boss->Velocity += Accel * dT;
                }
            } break;
            
            case ExerciseBossMode_MoveToStomp: {
                Boss->Position += Boss->Velocity * dT;
            } break;
            
            case ExerciseBossMode_HoldBeforeStomp: {
                
            } break;
            
            case ExerciseBossMode_Stomp: {
                vec2 dPos = Boss->Velocity * dT;
                
                COLLISION_RESULT Coll = CollisionResult( Boss->Position, Boss->Velocity, dPos );
                for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesIntersectCollisionTop( &BestIntersect, AppState, Coll, EXERCISE_BOSS_COLLISION_OFFSET );
                    
                    Coll = FinalizeCollision( BestIntersect, Coll );
                }
                Coll.Pos += Coll.dPos;
                
                Boss->Position = Coll.Pos;
                
                if( Coll.HadCollision ) {
                    Boss->Mode     = ExerciseBossMode_PostStomp;
                    Boss->Timer    = 0.0f;
                    Boss->Velocity = {};
                }
            } break;
            
            case ExerciseBossMode_PostStomp: {
                
            } break;
            
            case ExerciseBossMode_IsDying: {
                flo32 MaxSpeed = EXERCISE_BOSS_MOVE_SPEED;
                flo32 Friction = EXERCISE_BOSS_MOVE_FRICTION;
                
                vec2 Dir   = Vec2( 0.0f, 1.0f );
                vec2 Accel = ( Dir * MaxSpeed - Boss->Velocity ) * Friction;
                vec2 dPos  = Accel * ( dT * dT * 0.5f ) + Boss->Velocity * dT;
                
                Boss->Position += dPos;
                Boss->Velocity += Accel * dT;
            } break;
#endif
            
        }
        
        boo32 CanBeAttacked = ( Boss->Mode == ExerciseBossMode_Run );
        
        if( CanBeAttacked ) {
            if( Player->Punch_DoPunch ) {
                rect R = Player->Punch_Bound;
                R = AddDim( R, EXERCISE_BOSS_HALF_DIM );
                
                if( IsInBound( Boss->Position, R ) ) {
                    vec2 Dir = Vec2( 1.0f, 0.0f );
                    if( Player->Punch_FaceLeft ) {
                        Dir.x = -1.0f;
                    }
                    Boss->Velocity += ( Dir * 10.0f );
                    
                    Boss->Timer   = 0.0f;
                    Boss->Health -= 1;
                    if( Boss->Health > 0 ) {
                        //Boss->Mode  = ExerciseBossMode_Move;
                        //Boss->iDest = ( Boss->iDest + 1 ) % Boss->nDest;
                    } else {
                        Boss->Mode  = ExerciseBossMode_IsDying;
                    }
                }
            }
        }
        
        boo32 CanDamagePlayer = ( Boss->Mode != ExerciseBossMode_IsDead )
            && ( Boss->Mode != ExerciseBossMode_IsDying )
            && ( !Player->NoDamage );
        
        if( CanDamagePlayer ) {
            rect R = GetExerciseBossBoundToDamagePlayer( Boss );
            if( IsInBound( Player->Position, R ) ) {
                DamagePlayer( AppState, 1 );
            }
        }
    }
}

internal void
DrawExerciseBoss( RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, APP_STATE * AppState, DRAW_STATE * Draw ) {
    EXERCISE_BOSS * Boss = Draw->ExerciseBoss;
    
    if( ( Boss->IsActive ) && ( Boss->Mode != ExerciseBossMode_IsDead ) ) {
        { // BOSS
            vec4 Color = ToColor( 120, 20, 20 );
            
            vec2 Pos = Boss->Position;
            if( Boss->Mode == ExerciseBossMode_IsDying ) {
                Pos += RandomDirection2D() * ( TILE_WIDTH * 0.1f );
            }
            
            if( Boss->Mode == ExerciseBossMode_IsDying ) {
                flo32 Alpha = 1.0f - Clamp01( Boss->Timer / EXERCISE_BOSS_DEATH_TARGET_TIME );
                Color *= Alpha;
            }
            
            rect R = RectBCD( Pos, EXERCISE_BOSS_DIM );
            DrawRect( Pass_Game, R, Color );
        }
        
        { // HEALTH BAR
            vec2 App_Dim = AppState->App_Dim;
            vec2 Pos = GetT( AppState->App_Bound, Vec2( 0.95f, 0.95f ) );
            vec2 Dim = Vec2( App_Dim.x * 0.5f, App_Dim.y * 0.05f );
            rect R   = RectTRD( Pos, Dim );
            
            vec2 Label_Pos = GetTL( R ) + Vec2( Dim.x * 0.02f, -Dim.y * 0.06f );
            DrawString( Pass_UI, AppState->Font, "BOSS", Label_Pos, TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
            
            vec2 Bar_Dim = Vec2( Dim.x * 0.98f, Dim.y * 0.5f );
            vec2 Bar_Pos = GetBC( R ) + Vec2( 0.0f, Dim.y * 0.1f );
            
            flo32 t = ( flo32 )Boss->Health / ( flo32 )Boss->MaxHealth;
            rect  S = RectBCD( Bar_Pos, Bar_Dim );
            S.Right = lerp( S.Left, t, S.Right );
            
            DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.02f ) );
            DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
            
            DrawRect       ( Pass_UI, S, COLOR_RED );
        }
    }
}

internal void
DrawExerciseBossDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    EXERCISE_BOSS * Boss   = Draw->ExerciseBoss;
    PLAYER_STATE  * Player = Draw->Player;
    
    if( Boss->IsActive ) {
        boo32 CanDamagePlayer = ( Boss->Mode != ExerciseBossMode_IsDead )
            && ( Boss->Mode != ExerciseBossMode_IsDying )
            && ( !Player->NoDamage );
        
        if( CanDamagePlayer ) {
            rect R = GetExerciseBossBoundToDamagePlayer( Boss );
            DrawRectOutline( Pass, R, COLOR_YELLOW );
        }
        
        DrawPoint( Pass, Player->Position, TILE_DIM * 0.05f, COLOR_RED );
    }
}

internal void
FinalizeExerciseBoss( APP_STATE * AppState ) {
    EXERCISE_BOSS * Boss   = &AppState->ExerciseBoss;
    PLAYER_STATE  * Player = &AppState->Player;
    
    if( Boss->IsActive ) {
        switch( Boss->Mode ) {
            case ExerciseBossMode_Run: {
                LEVEL_STATS Stat = AppState->Stat;
                
                flo32 MarginX = TILE_WIDTH * 0.5f;
                
                flo32 LeftX  = Stat.Bound.Left  + MarginX;
                flo32 RightX = Stat.Bound.Right - MarginX;
                
                if( Boss->Position.x <= LeftX ) {
                    Boss->RunLeft = false;
                }
                if( Boss->Position.x >= RightX ) {
                    Boss->RunLeft = true;
                }
            } break;
            
            
#if 0            
            case ExerciseBossMode_Move: {
                if( Boss->Timer >= EXERCISE_BOSS_STOMP_TARGET_TIME ) {
                    Boss->Mode = ExerciseBossMode_MoveToStomp;
                    
                    LEVEL_STATS Stat = AppState->Stat;
                    vec2 P   = GetT( Stat.Bound, Vec2( 0.0f, 0.6f ) );
                    vec2 Pos = Vec2( Player->Position.x, P.y );
                    
                    flo32 TargetTime = 0.25f;
                    Boss->Velocity = ( Pos - Boss->Position ) / EXERCISE_BOSS_MOVE_TO_STOMP_TARGET_TIME;
                    Boss->Timer    = 0.0f;
                }
            } break;
            
            case ExerciseBossMode_MoveToStomp: {
                if( Boss->Timer >= EXERCISE_BOSS_MOVE_TO_STOMP_TARGET_TIME ) {
                    Boss->Mode     = ExerciseBossMode_HoldBeforeStomp;
                    Boss->Timer    = 0.0f;
                    Boss->Velocity = {};
                }
            } break;
            
            case ExerciseBossMode_HoldBeforeStomp: {
                if( Boss->Timer >= EXERCISE_BOSS_HOLD_BEFORE_STOMP_TARGET_TIME ) {
                    Boss->Mode     = ExerciseBossMode_Stomp;
                    Boss->Timer    = 0.0f;
                    Boss->Velocity = Vec2( 0.0f, -1.0f * EXERCISE_BOSS_STOMP_SPEED );
                }
            } break;
            
            case ExerciseBossMode_Stomp: {
            } break;
            
            case ExerciseBossMode_PostStomp: {
                if( Boss->Timer >= EXERCISE_BOSS_HOLD_AFTER_STOMP_TARGET_TIME ) {
                    Boss->Mode     = ExerciseBossMode_Move;
                    Boss->Timer    = 0.0f;
                    Boss->Velocity = Vec2( 0.0f, 10.0f );
                }
            } break;
            
            case ExerciseBossMode_IsDying: {
                if( Boss->Timer >= EXERCISE_BOSS_DEATH_TARGET_TIME ) {
                    Boss->Mode  = ExerciseBossMode_IsDead;
                    Boss->Timer = 0.0f;
                }
            } break;
#endif
            
        }
    }
}