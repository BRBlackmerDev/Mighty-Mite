
internal void
InitExerciseMiniBoss01( APP_STATE * AppState ) {
    EXERCISE_MINIBOSS01 * Boss = &AppState->ExerciseMiniBoss01;
    Boss->IsActive = true;
    
    LEVEL_STATS Stat = AppState->Stat;
    
    Boss->Position = GetT( Stat.Bound, Vec2( 0.5f, 0.8f ) );
    Boss->Velocity = {};
    
    Boss->nDest = 2;
    Boss->iDest = 0;
    Boss->Dest[ 0 ] = GetT( Stat.Bound, Vec2( 0.3333f, 0.7f ) );
    Boss->Dest[ 1 ] = GetT( Stat.Bound, Vec2( 0.6667f, 0.7f ) );
    
    Boss->Timer = 0.0f;
    
    Boss->Health = 2;
}

internal void
UpdateExerciseMiniBoss01( APP_STATE * AppState, flo32 dT ) {
    EXERCISE_MINIBOSS01 * Boss   = &AppState->ExerciseMiniBoss01;
    PLAYER_STATE        * Player = &AppState->Player;
    
    if( Boss->IsActive ) {
        Boss->Timer += dT;
        
        switch( Boss->Mode ) {
            case ExerciseMiniBoss01Mode_Move: {
                vec2 Dest = Boss->Dest[ Boss->iDest ];
                if( Boss->Position != Dest ) {
                    flo32 MaxSpeed = EXERCISE_MINIBOSS01_MOVE_SPEED;
                    flo32 Friction = EXERCISE_MINIBOSS01_MOVE_FRICTION;
                    
                    vec2 Dir   = GetNormal( Dest - Boss->Position );
                    vec2 Accel = ( Dir * MaxSpeed - Boss->Velocity ) * Friction;
                    vec2 dPos  = Accel * ( dT * dT * 0.5f ) + Boss->Velocity * dT;
                    
                    Boss->Position += dPos;
                    Boss->Velocity += Accel * dT;
                }
            } break;
            
            case ExerciseMiniBoss01Mode_MoveToStomp: {
                Boss->Position += Boss->Velocity * dT;
            } break;
            
            case ExerciseMiniBoss01Mode_HoldBeforeStomp: {
                
            } break;
            
            case ExerciseMiniBoss01Mode_Stomp: {
                vec2 dPos = Boss->Velocity * dT;
                
                COLLISION_RESULT Coll = CollisionResult( Boss->Position, Boss->Velocity, dPos );
                for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
                    RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                    DoesIntersectCollisionTop( &BestIntersect, AppState, Coll, EXERCISE_MINIBOSS01_COLLISION_OFFSET );
                    
                    Coll = FinalizeCollision( BestIntersect, Coll );
                }
                Coll.Pos += Coll.dPos;
                
                Boss->Position = Coll.Pos;
                
                if( Coll.HadCollision ) {
                    Boss->Mode     = ExerciseMiniBoss01Mode_PostStomp;
                    Boss->Timer    = 0.0f;
                    Boss->Velocity = {};
                }
            } break;
            
            case ExerciseMiniBoss01Mode_PostStomp: {
                
            } break;
            
            case ExerciseMiniBoss01Mode_IsDying: {
                flo32 MaxSpeed = EXERCISE_MINIBOSS01_MOVE_SPEED;
                flo32 Friction = EXERCISE_MINIBOSS01_MOVE_FRICTION;
                
                vec2 Dir   = Vec2( 0.0f, 1.0f );
                vec2 Accel = ( Dir * MaxSpeed - Boss->Velocity ) * Friction;
                vec2 dPos  = Accel * ( dT * dT * 0.5f ) + Boss->Velocity * dT;
                
                Boss->Position += dPos;
                Boss->Velocity += Accel * dT;
            } break;
        }
        
        boo32 CanBeAttacked = ( Boss->Mode == ExerciseMiniBoss01Mode_Move )
            || ( Boss->Mode == ExerciseMiniBoss01Mode_MoveToStomp )
            || ( Boss->Mode == ExerciseMiniBoss01Mode_HoldBeforeStomp )
            || ( Boss->Mode == ExerciseMiniBoss01Mode_PostStomp );
        
        if( CanBeAttacked ) {
            if( Player->Punch_DoPunch ) {
                rect R = Player->Punch_Bound;
                R = AddDim( R, EXERCISE_MINIBOSS01_DIM );
                
                if( IsInBound( Boss->Position, R ) ) {
                    vec2 Dir = Vec2( 1.0f, 0.0f );
                    if( Player->Punch_FaceLeft ) {
                        Dir.x = -1.0f;
                    }
                    Boss->Velocity += ( Dir * 10.0f );
                    
                    Boss->Timer   = 0.0f;
                    Boss->Health -= 1;
                    if( Boss->Health > 0 ) {
                        Boss->Mode  = ExerciseMiniBoss01Mode_Move;
                        Boss->iDest = ( Boss->iDest + 1 ) % Boss->nDest;
                    } else {
                        Boss->Mode  = ExerciseMiniBoss01Mode_IsDying;
                    }
                }
            }
        }
        
        boo32 CanDamagePlayer = ( Boss->Mode != ExerciseMiniBoss01Mode_IsDead )
            && ( Boss->Mode != ExerciseMiniBoss01Mode_IsDying )
            && ( !Player->NoDamage );
        
        if( CanDamagePlayer ) {
            rect R = RectBCD( Player->Position, PLAYER_DIM );
            R = AddDim( R, EXERCISE_MINIBOSS01_DAMAGE_PLAYER_HALF_DIM );
            
            if( IsInBound( Boss->Position, R ) ) {
                //DamagePlayer( AppState, 2 );
            }
        }
    }
}

internal void
DrawExerciseMiniBoss01( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    EXERCISE_MINIBOSS01 * Boss = Draw->ExerciseMiniBoss01;
    
    if( ( Boss->IsActive ) && ( Boss->Mode != ExerciseMiniBoss01Mode_IsDead ) ) {
        vec4 Color = ToColor( 120, 20, 20 );
        
        vec2 Pos = Boss->Position;
        if( Boss->Mode == ExerciseMiniBoss01Mode_IsDying ) {
            Pos += RandomDirection2D() * ( TILE_WIDTH * 0.1f );
        }
        
        if( Boss->Mode == ExerciseMiniBoss01Mode_IsDying ) {
            flo32 Alpha = 1.0f - Clamp01( Boss->Timer / EXERCISE_MINIBOSS01_DEATH_TARGET_TIME );
            Color *= Alpha;
        }
        
        rect R = RectCD( Pos, EXERCISE_MINIBOSS01_DIM );
        DrawRect( Pass, R, Color );
    }
}

internal void
DrawExerciseMiniBoss01Debug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    EXERCISE_MINIBOSS01 * Boss   = Draw->ExerciseMiniBoss01;
    PLAYER_STATE        * Player = Draw->Player;
    
    if( Boss->IsActive ) {
        boo32 CanDamagePlayer = ( Boss->Mode != ExerciseMiniBoss01Mode_IsDying )
            && ( !Player->NoDamage );
        
        if( CanDamagePlayer ) {
            rect R = RectBCD( Player->Position, PLAYER_DIM );
            R = AddDim( R, EXERCISE_MINIBOSS01_DAMAGE_PLAYER_HALF_DIM );
            
            DrawRectOutline( Pass, R, COLOR_YELLOW );
        }
        
        DrawPoint( Pass, Player->Position, TILE_DIM * 0.05f, COLOR_RED );
    }
}

internal void
FinalizeExerciseMiniBoss01( APP_STATE * AppState ) {
    EXERCISE_MINIBOSS01 * Boss   = &AppState->ExerciseMiniBoss01;
    PLAYER_STATE        * Player = &AppState->Player;
    
    if( Boss->IsActive ) {
        switch( Boss->Mode ) {
            case ExerciseMiniBoss01Mode_Move: {
                if( Boss->Timer >= EXERCISE_MINIBOSS01_STOMP_TARGET_TIME ) {
                    Boss->Mode = ExerciseMiniBoss01Mode_MoveToStomp;
                    
                    LEVEL_STATS Stat = AppState->Stat;
                    vec2 P   = GetT( Stat.Bound, Vec2( 0.0f, 0.6f ) );
                    vec2 Pos = Vec2( Player->Position.x, P.y );
                    
                    flo32 TargetTime = 0.25f;
                    Boss->Velocity = ( Pos - Boss->Position ) / EXERCISE_MINIBOSS01_MOVE_TO_STOMP_TARGET_TIME;
                    Boss->Timer    = 0.0f;
                }
            } break;
            
            case ExerciseMiniBoss01Mode_MoveToStomp: {
                if( Boss->Timer >= EXERCISE_MINIBOSS01_MOVE_TO_STOMP_TARGET_TIME ) {
                    Boss->Mode     = ExerciseMiniBoss01Mode_HoldBeforeStomp;
                    Boss->Timer    = 0.0f;
                    Boss->Velocity = {};
                }
            } break;
            
            case ExerciseMiniBoss01Mode_HoldBeforeStomp: {
                if( Boss->Timer >= EXERCISE_MINIBOSS01_HOLD_BEFORE_STOMP_TARGET_TIME ) {
                    Boss->Mode     = ExerciseMiniBoss01Mode_Stomp;
                    Boss->Timer    = 0.0f;
                    Boss->Velocity = Vec2( 0.0f, -1.0f * EXERCISE_MINIBOSS01_STOMP_SPEED );
                }
            } break;
            
            case ExerciseMiniBoss01Mode_Stomp: {
            } break;
            
            case ExerciseMiniBoss01Mode_PostStomp: {
                if( Boss->Timer >= EXERCISE_MINIBOSS01_HOLD_AFTER_STOMP_TARGET_TIME ) {
                    Boss->Mode     = ExerciseMiniBoss01Mode_Move;
                    Boss->Timer    = 0.0f;
                    Boss->Velocity = Vec2( 0.0f, 10.0f );
                }
            } break;
            
            case ExerciseMiniBoss01Mode_IsDying: {
                if( Boss->Timer >= EXERCISE_MINIBOSS01_DEATH_TARGET_TIME ) {
                    Boss->Mode  = ExerciseMiniBoss01Mode_IsDead;
                    Boss->Timer = 0.0f;
                }
            } break;
        }
    }
}