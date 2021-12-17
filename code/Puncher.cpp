
internal BARBELL BarbellC( vec2 Pos, vec2 Vel = {}, int32 Skip_iPuncher = -1, boo32 CanHurtPlayer = false );
internal void    AddBarbell( APP_STATE * AppState, BARBELL Barbell );
internal void    DrawBarbell( RENDER_PASS * Pass, BARBELL Barbell );

internal PUNCHER
PuncherC( vec2 Pos ) {
    PUNCHER Puncher = {};
    Puncher.Position = Pos;
    Puncher.Health   = PUNCHER_HEALTH;
    
    Puncher.Barbell_CanThrow = ( ( rand() % 2 ) == 0 );
    
    return Puncher;
}

internal PUNCHER
PuncherStunnedC( vec2 Pos, vec2 Vel, uint32 Health = PUNCHER_HEALTH, boo32 CanStunOtherPunchers = false ) {
    PUNCHER Puncher = {};
    Puncher.Position  = Pos;
    Puncher.Velocity  = Vel;
    Puncher.Health    = Health;
    Puncher.IsStunned = true;
    Puncher.CanStunOtherPunchers = CanStunOtherPunchers;
    
    return Puncher;
}

internal void
AddPuncher( APP_STATE * AppState, PUNCHER Puncher ) {
    PUNCHER_STATE * PuncherS = &AppState->PuncherS;
    
    if( PuncherS->nPuncher < PUNCHER_MAX_COUNT ) {
        PuncherS->Puncher[ PuncherS->nPuncher++ ] = Puncher;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new PUNCHER, but the state is full! Max Count = %u", PUNCHER_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemovePuncher( APP_STATE * AppState, uint32 iRemove ) {
    PUNCHER_STATE * PuncherS = &AppState->PuncherS;
    
    Assert( PuncherS->nPuncher > 0 );
    PuncherS->Puncher[ iRemove ] = PuncherS->Puncher[ --PuncherS->nPuncher ];
}

internal rect
GetPuncherAlertBound( PUNCHER Puncher ) {
    rect Bound = RectBCD( Puncher.Position, PUNCHER_ALERT_DIM );
    Bound.Top    += TILE_HEIGHT * 2.0f;
    Bound.Bottom -= TILE_HEIGHT * 2.0f;
    Bound = AddRadius( Bound, TILE_WIDTH * 0.1f );
    
    return Bound;
}

internal rect
GetPuncherShouldPunchBound( PUNCHER Puncher ) {
    rect Bound = {};
    if( Puncher.DrawMode == PuncherDrawMode_MoveLeft ) {
        Bound = RectBRD( Puncher.Position, PUNCHER_SHOULD_PUNCH_DIM );
    } else {
        Bound = RectBLD( Puncher.Position, PUNCHER_SHOULD_PUNCH_DIM );
    }
    Bound = AddRadius( Bound, TILE_WIDTH * 0.1f );
    
    return Bound;
}

internal rect
GetPuncherPunchBoundToDamagePlayer( PUNCHER Puncher ) {
    rect R = {};
    if( Puncher.Punch_FaceLeft ) {
        R = RectBRD( Puncher.Position, PUNCHER_PUNCH_COLLISION_DIM );
    } else {
        R = RectBLD( Puncher.Position, PUNCHER_PUNCH_COLLISION_DIM );
    }
    
    R = AddRadius( R, TILE_WIDTH * 0.1f );
    return R;
}

internal boo32
IsTouchingPuncher( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    PUNCHER_STATE * PuncherS = &AppState->PuncherS;
    
    boo32 Result = {};
    
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
        
        if( !Puncher.IsStunned ) {
            vec2 Dim   = PUNCHER_DIM;
            rect Bound = RectBCD( Puncher.Position, Dim );
            Bound = MinkSub( Bound, CollBound );
            
            if( IsInBound( Pos, Bound ) ) {
                Result = true;
            }
        }
    }
    
    return Result;
}

internal ENTITY_VALID_RESULT
IsInPuncherStunBound( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    PUNCHER_STATE * PuncherS = &AppState->PuncherS;
    
    ENTITY_VALID_RESULT Result = {};
    
    int32 iStanding = -1;
    int32 iStunned  = -1;
    flo32 MaxDistSq_Stunned  = FLT_MAX;
    flo32 MaxDistSq_Standing = FLT_MAX;
    
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
        
        if( ( Puncher.Health > 0 ) && ( Puncher.CanBeStunned_Timer >= ( 10.0f / 60.0f ) ) ) {
            vec2 Dim = PUNCHER_DIM;
            if( Puncher.IsStunned ) {
                Dim = Vec2( PUNCHER_WIDTH, PUNCHER_WIDTH );
            }
            
            rect Bound = MinkSub( RectBCD( Puncher.Position, Dim ), CollBound );
            
            if( IsInBound( Pos, Bound ) ) {
                flo32 DistSq = GetLengthSq( Pos - Puncher.Position );
                if( ( Puncher.IsStunned ) && ( DistSq < MaxDistSq_Stunned ) ) {
                    iStunned          = iPuncher;
                    MaxDistSq_Stunned = DistSq;
                } else if( ( !Puncher.IsStunned ) && ( DistSq < MaxDistSq_Standing ) ) {
                    iStanding          = iPuncher;
                    MaxDistSq_Standing = DistSq;
                }
            }
        }
    }
    
    if( iStanding > -1 ) {
        Result.IsValid = true;
        Result.iEntity = iStanding;
    } else if( iStunned > -1 ) {
        Result.IsValid = true;
        Result.iEntity = iStunned;
    }
    
    return Result;
}

internal ENTITY_VALID_RESULT
IsInPuncherPickUpBound( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    PUNCHER_STATE * PuncherS = &AppState->PuncherS;
    
    ENTITY_VALID_RESULT Result = {};
    
    flo32 MaxDistSq = FLT_MAX;
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
        
        if( ( Puncher.IsStunned ) && ( Puncher.Health > 0 ) ) {
            vec2 Dim   = Vec2( PUNCHER_WIDTH + ( TILE_WIDTH * 0.2f ), PUNCHER_WIDTH );
            rect Bound = RectBCD( Puncher.Position, Dim );
            Bound = MinkSub( Bound, CollBound );
            
            if( IsInBound( Pos, Bound ) ) {
                flo32 DistSq = GetLengthSq( Pos - Puncher.Position );
                if( DistSq < MaxDistSq ) {
                    Result.IsValid = true;
                    Result.iEntity = iPuncher;
                    
                    MaxDistSq = DistSq;
                }
            }
        }
    }
    
    return Result;
}

internal void
StunPuncher( APP_STATE * AppState, uint32 iPuncher, flo32 DirX ) {
    PUNCHER_STATE * PuncherS = &AppState->PuncherS;
    PUNCHER       * Puncher  = PuncherS->Puncher + iPuncher;
    
    if( Puncher->Barbell_CanThrow ) {
        Puncher->Barbell_CanThrow = false;
        
        flo32 Barbell_DirX = 1.0f;
        if( Puncher->DrawMode == PuncherDrawMode_MoveRight ) {
            Barbell_DirX = -1.0f;
        }
        
        vec2 BarbellP = Puncher->Position + Vec2( PUNCHER_BARBELL_OFFSET_X * Barbell_DirX, PUNCHER_BARBELL_OFFSET_Y );
        AddBarbell( AppState, BarbellC( BarbellP ) );
    }
    
    Puncher->IsStunned        = true;
    Puncher->IsStunned_Timer  = 0.0f;
    Puncher->Punch_IsCharging = false;
    Puncher->Punch_Timer      = 0.0f;
    Puncher->Punch_DoPunch    = false;
    Puncher->Punch_DoDraw     = false;
    
    Puncher->CanBeStunned_Timer = 0.0f;
    
    vec2 V = Vec2( 14.0f * DirX, 6.0f );
    Puncher->Velocity = V;
    
    uint32 Damage = MinValue( Puncher->Health, ( uint32 )1 );
    Puncher->Health -= Damage;
}

internal vec2
GetRepel( vec2 PosA, vec2 PosB, flo32 MaxStrength, flo32 MaxDist ) {
    flo32 Repel_Strength = MaxStrength * MaxDist * MaxDist;
    
    vec2  Vector   = PosB - PosA;
    flo32 Strength = MinValue( Repel_Strength / GetLengthSq( Vector ), MaxStrength );
    vec2  Result   = GetNormal( Vector ) * Strength;
    
    flo32 MaxDistSq = MaxDist * MaxDist;
    if( GetLengthSq( Vector ) >= MaxDistSq ) {
        Result = {};
    }
    
    return Result;
}

internal void
UpdatePuncher( APP_STATE * AppState, flo32 dT ) {
    PUNCHER_STATE * PuncherS = &AppState->PuncherS;
    PLAYER_STATE  * Player   = &AppState->Player;
    
    vec2 PlayerP = Player->Position;
    
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        PUNCHER * Puncher = PuncherS->Puncher + iPuncher;
        
        vec2 Dir = {};
        
        Puncher->Punch_CooldownTimer = MaxValue( Puncher->Punch_CooldownTimer - dT, 0.0f );
        Puncher->CanBeStunned_Timer += dT;
        
        rect  IsOnGroundBound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( PUNCHER_WIDTH, 0.0f ) );
        boo32 IsOnGround      = IsPosOnCollisionTop( AppState, Puncher->Position, IsOnGroundBound );
        if( IsOnGround ) {
            Puncher->CanStunOtherPunchers = false;
        }
        
        if( Puncher->IsStunned ) {
            Puncher->IsStunned_Timer += dT;
            
            flo32 Speed = GetLength( Puncher->Velocity );
            if( ( Puncher->CanStunOtherPunchers ) && ( !IsOnGround ) && ( Speed > 10.0f ) ) {
                rect StunBound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( PUNCHER_WIDTH, PUNCHER_WIDTH ) );
                for( uint32 iPuncherA = 0; iPuncherA < PuncherS->nPuncher; iPuncherA++ ) {
                    if( iPuncher != iPuncherA ) {
                        PUNCHER * PuncherA = PuncherS->Puncher + iPuncherA;
                        
                        if( PuncherA->Health > 0 ) {
                            vec2 Dim = PUNCHER_DIM;
                            if( PuncherA->IsStunned ) {
                                Dim = Vec2( PUNCHER_WIDTH, PUNCHER_WIDTH );
                            }
                            
                            rect Bound = MinkSub( RectBCD( PuncherA->Position, Dim ), StunBound );
                            if( IsInBound( Puncher->Position, Bound ) ) {
                                flo32 DirX = Puncher->Velocity.x / fabsf( Puncher->Velocity.x );
                                StunPuncher( AppState, iPuncherA, DirX );
                                Puncher->CanStunOtherPunchers = false;
                            }
                        }
                    }
                }
            }
        } else {
            if( Puncher->Punch_IsCharging ) {
                Puncher->Punch_Timer += dT;
                if( Puncher->Punch_Timer >= PUNCHER_PUNCH_TARGET_TIME ) {
                    vec2 Offset = PUNCHER_PUNCH_ART_OFFSET;
                    if( Puncher->DrawMode == PuncherDrawMode_MoveLeft ) {
                        Offset.x = -Offset.x;
                    }
                    
                    vec2 PunchP = Puncher->Position + Offset;
                    Puncher->Punch_CooldownTimer = PUNCHER_PUNCH_COOLDOWN_TARGET_TIME;
                    Puncher->Punch_DoPunch    = true;
                    Puncher->Punch_DoDraw     = true;
                    Puncher->Punch_ArtTimer   = 0.0f;
                    Puncher->Punch_ArtPos     = PunchP;
                    Puncher->Punch_IsCharging = false;
                    Puncher->Punch_FaceLeft   = ( Puncher->DrawMode == PuncherDrawMode_MoveLeft );
                    
                    rect PunchR = GetPuncherPunchBoundToDamagePlayer( *Puncher );
                    if( IsInBound( PlayerP, PunchR ) ) {
                        DamagePlayer( AppState, 1 );
                    }
                }
            } else {
                rect Alert_Bound = GetPuncherAlertBound( *Puncher );
                rect Punch_Bound = GetPuncherShouldPunchBound( *Puncher );
                
                if( IsInBound( PlayerP, Alert_Bound ) ) {
                    Puncher->IsAggressive = true;
                }
                
                if( Puncher->IsAggressive ) {
                    for( uint32 iPuncherA = 0; iPuncherA < PuncherS->nPuncher; iPuncherA++ ) {
                        PUNCHER * PuncherA = PuncherS->Puncher + iPuncherA;
                        rect Alert_BoundA = GetPuncherAlertBound( *PuncherA );
                        if( IsInBound( PuncherA->Position, Alert_BoundA ) ) {
                            PuncherA->IsAggressive = true;
                        }
                    }
                    
                    vec2  SeePlayerP   = Player->Position  + Vec2( 0.0f, TILE_HEIGHT * 1.0f );
                    vec2  SeePuncherP  = Puncher->Position + Vec2( 0.0f, TILE_HEIGHT * 1.5f );
                    
                    boo32 DoIntersectCollision = DoesEdgeIntersectCollisionAll( AppState, SeePuncherP, SeePlayerP );
                    boo32 DoIntersectPuncher = DoesEdgeIntersectPuncher( AppState, SeePuncherP, SeePlayerP, iPuncher );
                    flo32 DistToPlayer = GetLength( SeePuncherP - SeePlayerP );
                    
                    boo32 CanSeePlayer = ( !DoIntersectCollision )
                        && ( !DoIntersectPuncher )
                        && ( DistToPlayer < ( TILE_WIDTH * 10.0f ) );
                    Puncher->CanSeePlayer = CanSeePlayer;
                    
                    if( CanSeePlayer ) {
                        Puncher->Barbell_ThrowTimer += dT;
                    } else {
                        Puncher->Barbell_ThrowTimer  = 0.0f;
                    }
                    
                    if( PlayerP.x < Puncher->Position.x ) {
                        Dir.x = -1.0f;
                        Puncher->DrawMode = PuncherDrawMode_MoveLeft;
                    } else {
                        Dir.x =  1.0f;
                        Puncher->DrawMode = PuncherDrawMode_MoveRight;
                    }
                    
                    {
                        rect CollBound = RectBCD( Vec2( 0.0f, 0.0f ), PUNCHER_DIM );
                        //RAY2_INTERSECT BestIntersectHor = Ray2IntersectInit();
                        RAY2_INTERSECT BestIntersectVer = Ray2IntersectInit();
                        
                        vec2 Center = GetCenter( RectBCD( Puncher->Position, PUNCHER_DIM ) );
                        
                        //COLLISION_RESULT CollHor = CollisionResult( Center, Puncher->Velocity, Vec2( Dir.x * 0.25f, 0.0f ) );
                        COLLISION_RESULT CollVer = CollisionResult( Center + Vec2( Dir.x * 1.4f, 0.0f ), Puncher->Velocity, Vec2( 0.0f, -1.0f ) );
                        
                        //DoesIntersectCollisionAll( &BestIntersectHor, AppState, CollHor, CollBound );
                        DoesIntersectCollisionAll( &BestIntersectVer, AppState, CollVer, CollBound );
                        //DoesIntersectBreakBlock  ( &BestIntersectHor, AppState, CollHor, CollBound );
                        //DoesIntersectBreakBlock  ( &BestIntersectVer, AppState, CollVer, CollBound );
                        
                        //boo32 HorIsValid = ( BestIntersectHor.IsValid ) && ( BestIntersectHor.t <= 1.0f );
                        boo32 VerIsValid = ( BestIntersectVer.IsValid ) && ( BestIntersectVer.t <= 1.0f );
                        
                        //if( ( HorIsValid ) || ( !VerIsValid ) ) {
                        if( !VerIsValid ) {
                            Dir.x = 0.0f;
                        }
                    }
                } else {
                    Puncher->DrawMode = PuncherDrawMode_Idle;
                    Puncher->Barbell_ThrowTimer = 0.0f;
                }
                
                boo32 CanPunch = ( Puncher->Punch_CooldownTimer <= 0.0f )
                    && ( !Puncher->Barbell_CanThrow );
                if( ( CanPunch ) && ( IsInBound( PlayerP, Punch_Bound ) ) ) {
                    Puncher->Punch_IsCharging = true;
                    Puncher->Punch_Timer      = 0.0f;
                }
            }
            
            if( Puncher->Punch_DoDraw ) {
                flo32 StepX = ( TILE_WIDTH * 0.4f );
                if( Puncher->Punch_FaceLeft ) {
                    StepX = -StepX;
                }
                
                Puncher->Punch_ArtTimer += dT;
                Puncher->Punch_ArtPos.x += StepX;
            }
            
#if 0            
            if( Player->Punch_DoPunch ) {
                rect R = Player->Punch_Bound;
                rect S = RectBCD( Vec2( 0.0f, 0.0f ), PUNCHER_DIM );
                R.Left   -= S.Right;
                R.Bottom -= S.Top;
                R.Right  -= S.Left;
                R.Top    -= S.Bottom;
                
                if( IsInBound( Puncher->Position, R ) ) {
                    uint32 DoDamage = 1;
                    if( Player->Punch_ArtCharge >= 1.0f ) {
                        DoDamage = 2;
                    }
                    uint32 Damage = MinValue( Puncher->Health, DoDamage );
                    Puncher->Health -= Damage;
                    
                    flo32 Speed = 14.0f;
                    if( ( Player->Punch_ArtCharge >= 1.0f ) || ( Puncher->Health == 0 ) ) {
                        Speed = 22.0f;
                    }
                    vec2 V = Vec2( 1.0f, 0.25f );
                    if( Player->Punch_FaceLeft ) {
                        V.x = -1.0f;
                    }
                    Puncher->Velocity += ( V * Speed );
                }
            }
#endif
        }
        
        flo32 Friction = PUNCHER_MOVE_FRICTION;
        if( ( !IsOnGround ) && ( Puncher->IsStunned ) ) {
            Friction = PUNCHER_STUNNED_FRICTION;
        }
        
        vec2 Repel_Velocity = Puncher->Repel;
        for( uint32 iPuncherA = iPuncher + 1; iPuncherA < PuncherS->nPuncher; iPuncherA++ ) {
            PUNCHER * PuncherA = PuncherS->Puncher + iPuncherA;
            if( PuncherA->Health > 0 ) {
                vec2 Repel = GetRepel( Puncher->Position, PuncherA->Position, PUNCHER_REPEL_MAX_STRENGTH, PUNCHER_REPEL_MAX_DISTANCE );
                Repel_Velocity.x  -= Repel.x;
                PuncherA->Repel.x += Repel.x;
            }
        }
        
        vec2 Accel = {};
        Accel.x = ( Dir.x * PUNCHER_MOVE_SPEED - Puncher->Velocity.x + Repel_Velocity.x ) * Friction;
        Accel.y = -PUNCHER_GRAVITY;
        
        
        vec2 Pos  = Puncher->Position;
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + Puncher->Velocity * dT;
        Puncher->Velocity += Accel * dT;
        
        rect CollBound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( PUNCHER_WIDTH, PUNCHER_WIDTH ) );
        
        COLLISION_RESULT Coll = CollisionResult( Pos, Puncher->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectPushBlock   ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectBreakBlock  ( &BestIntersect, AppState, Coll, CollBound );
            //DoesIntersectScaffold    ( &BestIntersect, AppState, Coll, CollBound );
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        Coll.Pos += Coll.dPos;
        
        Puncher->Position = Coll.Pos;
        Puncher->Velocity = Coll.Vel;
        Puncher->Repel = {};
        
        if( ( Puncher->Barbell_CanThrow ) && ( Puncher->Barbell_ThrowTimer >= PUNCHER_BARBELL_THROW_TARGET_TIME ) ) {
            Puncher->Barbell_CanThrow = false;
            
            flo32 Barbell_DirX = 1.0f;
            if( Puncher->DrawMode == PuncherDrawMode_MoveRight ) {
                Barbell_DirX = -1.0f;
            }
            
            vec2 BarbellP = Puncher->Position + Vec2( PUNCHER_BARBELL_OFFSET_X * Barbell_DirX, PUNCHER_BARBELL_OFFSET_Y );
            AddBarbell( AppState, BarbellC( BarbellP, Vec2( 8.0f * -Barbell_DirX, 14.0f ), iPuncher, true ) );
        }
    }
}

internal void
DrawPuncher( RENDER_PASS * Pass, PUNCHER Puncher ) {
    rect R = RectBCD( Puncher.Position, PUNCHER_DIM );
    
    if( !Puncher.IsStunned ) {
        switch( Puncher.DrawMode ) {
            case PuncherDrawMode_Idle: {
                vec2 Arm_Pos = Puncher.Position + Vec2( 0.0f, TILE_HEIGHT * 0.4f );
                rect Arm_Bound = RectBCD( Arm_Pos, PUNCHER_ARM_DIM );
                DrawRect( Pass, Arm_Bound, ToColor( 180, 90, 45 ) );
            } break;
        }
    }
    
    if( Puncher.IsStunned ) {
        flo32 t = Clamp01( Puncher.IsStunned_Timer / 0.3f );
        
        vec2 P = Puncher.Position + Vec2( 0.0f, PUNCHER_HEIGHT * 0.5f );
        vec2 Q = Puncher.Position + Vec2( 0.0f, PUNCHER_WIDTH * 0.5f );
        
        flo32 Radians = lerp( 0.0f, t, PI * 0.5f );
        vec2  Pos     = lerp( P, t, Q );
        
        if( Puncher.DrawMode == PuncherDrawMode_MoveLeft ) {
            Radians = -Radians;
        }
        
        vec2  Dim   = PUNCHER_DIM;
        orect Bound = ORectCD( Pos, Dim, Radians );
        
        vec4  Color = ToColor( 120, 50, 25 );
        if( Puncher.Health == 0 ) {
            Color = ToColor( 30, 15, 5 );
            flo32 TargetTime = ( 60.0f / 60.0f );
            flo32 Alpha = 1.0f - Clamp01( ( Puncher.IsStunned_Timer - TargetTime ) / ( PUNCHER_STUNNED_TARGET_TIME - TargetTime ) );
            Color.a    = Alpha;
            Color.rgb *= Alpha;
        }
        
        DrawORect( Pass, Bound, Color );
        DrawORectOutline( Pass, Bound, COLOR_BLACK );
    } else {
        DrawRect( Pass, R, ToColor( 120, 50, 25 ) );
        DrawRectOutline( Pass, R, COLOR_BLACK );
    }
    
    if( !Puncher.IsStunned ) {
        if( Puncher.Punch_IsCharging ) {
            vec2 Offset = PUNCHER_PUNCH_ART_OFFSET;
            if( Puncher.DrawMode == PuncherDrawMode_MoveRight ) {
                Offset.x = -Offset.x;
            }
            
            flo32 Radians       = RandomF32() * 2.0f * PI;
            flo32 t             = Clamp01( Puncher.Punch_Timer / PUNCHER_PUNCH_TARGET_TIME );
            vec2  VibrateOffset = ToDirection2D( Radians ) * t * PUNCHER_PUNCH_ART_VIBRATE_OFFSET;
            
            vec2 PunchP = Puncher.Position + Offset + VibrateOffset;
            rect PunchR = RectCD( PunchP, PUNCHER_FIST_DIM );
            
            DrawRect( Pass, PunchR, ToColor( 180, 90, 45 ) );
        } else {
            switch( Puncher.DrawMode ) {
                case PuncherDrawMode_MoveLeft: {
                    vec2 Arm_Pos   = Puncher.Position + Vec2( -TILE_WIDTH * 0.1f, TILE_HEIGHT * 0.4f );
                    rect Arm_Bound = RectBLD( Arm_Pos, PUNCHER_ARM_SIDE_DIM );
                    DrawRect( Pass, Arm_Bound, ToColor( 180, 90, 45 ) );
                } break;
                
                case PuncherDrawMode_MoveRight: {
                    vec2 Arm_Pos   = Puncher.Position + Vec2( TILE_WIDTH * 0.1f, TILE_HEIGHT * 0.4f );
                    rect Arm_Bound = RectBRD( Arm_Pos, PUNCHER_ARM_SIDE_DIM );
                    DrawRect( Pass, Arm_Bound, ToColor( 180, 90, 45 ) );
                } break;
            }
            
            if( Puncher.Barbell_CanThrow ) {
                flo32 Barbell_DirX = 1.0f;
                if( Puncher.DrawMode == PuncherDrawMode_MoveRight ) {
                    Barbell_DirX = -1.0f;
                }
                
                vec2 BarbellP = Puncher.Position + Vec2( PUNCHER_BARBELL_OFFSET_X * Barbell_DirX, PUNCHER_BARBELL_OFFSET_Y );
                if( Puncher.DrawMode == PuncherDrawMode_Idle ) {
                    BarbellP.x += TILE_WIDTH * 0.35f;
                }
                
                BARBELL Barbell = BarbellC( BarbellP );
                Barbell.Radians = PI * 0.5f;
                DrawBarbell( Pass, Barbell );
            }
        }
    }
    
    if( ( !Puncher.IsStunned ) && ( Puncher.Punch_DoDraw ) ) {
        vec2 PunchP = Puncher.Punch_ArtPos;
        rect PunchR = RectCD( PunchP, PUNCHER_FIST_DIM );
        
        vec2 Dim = PUNCHER_PUNCH_COLLISION_DIM;
        Dim.x    = TILE_WIDTH * 0.65f;
        
        rect S = {};
        if( Puncher.Punch_FaceLeft ) {
            S = RectRCD( GetCenter( PunchR ), Dim );
        } else {
            S = RectLCD( GetCenter( PunchR ), Dim );
        }
        
        DrawRect( Pass, S, COLOR_WHITE );
        DrawRect( Pass, PunchR, ToColor( 180, 90, 45 ) );
    }
}

internal void
DrawPuncher( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    PUNCHER_STATE * PuncherS = Draw->PuncherS;
    
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
        DrawPuncher( Pass, Puncher );
    }
}

internal rect
GetPlayerBoundToBeDamagedByPuncher( vec2 PlayerP ) {
    flo32 xMargin       = PUNCHER_HALF_WIDTH - ( TILE_WIDTH  * 0.2f  );
    flo32 yMarginBottom = PUNCHER_HEIGHT     - ( TILE_HEIGHT * 0.25f );
    flo32 yMarginTop    = TILE_HEIGHT * 0.5f;
    
    rect Bound = RectBCD( PlayerP, PLAYER_DIM );
    Bound.Left   -= xMargin;
    Bound.Right  += xMargin;
    Bound.Bottom -= yMarginBottom;
    Bound.Top    -= yMarginTop;
    
    return Bound;
}

internal void
DrawPuncherDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    PUNCHER_STATE * PuncherS = Draw->PuncherS;
    PLAYER_STATE  * Player   = Draw->Player;
    
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
        
        rect Alert_Bound = GetPuncherAlertBound( Puncher );
        DrawRectOutline( Pass, Alert_Bound, COLOR_YELLOW );
        
        rect Punch_Bound = GetPuncherShouldPunchBound( Puncher );
        DrawRectOutline( Pass, Punch_Bound, COLOR_YELLOW );
        
        if( Puncher.Punch_DoPunch ) {
            rect Bound = GetPuncherPunchBoundToDamagePlayer( Puncher );
            DrawRectOutline( Pass, Bound, COLOR_RED );
        }
        
        vec4 Color = COLOR_RED;
        if( Puncher.CanSeePlayer ) {
            Color = COLOR_GREEN;
        }
        
        DrawLine( Pass, Player->Position, Puncher.Position, Color );
    }
}

internal void
FinalizePuncher( APP_STATE * AppState ) {
    PUNCHER_STATE * PuncherS = &AppState->PuncherS;
    
    uint32 NewCount = 0;
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        PUNCHER * Puncher = PuncherS->Puncher + iPuncher;
        
        boo32 IsDead = false;
        if( Puncher->Health > 0 ) {
            if( Puncher->IsStunned_Timer >= PUNCHER_STUNNED_TARGET_TIME ) {
                Puncher->IsStunned = false;
            }
        } else {
            if( Puncher->IsStunned_Timer >= PUNCHER_DYING_TARGET_TIME ) {
                IsDead = true;
            }
        }
        
        Puncher->Punch_DoPunch = false;
        if( ( Puncher->Punch_DoDraw ) && ( Puncher->Punch_ArtTimer >= PLAYER_PUNCH_ART_TARGET_TIME ) ) {
            Puncher->Punch_DoDraw = false;
        }
        
        rect Bound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( PUNCHER_WIDTH, PUNCHER_WIDTH ) );
        boo32 IsOnSpikes = IsInSpikesKillBound( AppState, Puncher->Position, Bound );
        
        if( ( !IsDead ) && ( !IsOnSpikes ) ) {
            PuncherS->Puncher[ NewCount++ ] = *Puncher;
        }
    }
    PuncherS->nPuncher = NewCount;
}

internal void
Editor_DrawPuncher( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__PUNCHER_STATE * PuncherS = &Editor->PuncherS;
    
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        EDITOR__PUNCHER Src     = PuncherS->Puncher[ iPuncher ];
        PUNCHER         Puncher = PuncherC( Src.Position );
        DrawPuncher( Pass, Puncher );
    }
}

internal void
Editor_MovePuncher( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__PUNCHER_STATE * PuncherS = &Editor->PuncherS;
    
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        EDITOR__PUNCHER * Puncher = PuncherS->Puncher + iPuncher;
        
        if( IsInBound( Puncher->Position, SrcBound ) ) {
            Puncher->Position += Offset;
        }
    }
}

internal void
Editor_MirrorPuncher( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__PUNCHER_STATE * PuncherS = &Editor->PuncherS;
    
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        EDITOR__PUNCHER * Puncher = PuncherS->Puncher + iPuncher;
        
        if( IsInBound( Puncher->Position, SrcBound ) ) {
            Puncher->Position.x = ( SrcBound.Right ) - ( Puncher->Position.x - SrcBound.Left );
        }
    }
}

#define VERSION__PUNCHER  ( 1 )
#define FILETAG__PUNCHER  ( "PUNCHER_" )
internal void
SavePuncher( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir,
            char * FileName ) {
    char * FileTag = FILETAG__PUNCHER;
    uint32 Version = VERSION__PUNCHER;
    EDITOR__PUNCHER_STATE * PuncherS = &Editor->PuncherS;
    
    if( PuncherS->nPuncher > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, PuncherS->nPuncher );
        for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
            EDITOR__PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
            _writem( output, EDITOR__PUNCHER, Puncher );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__PUNCHER
ReadPuncher( uint32 Version, uint8 ** Ptr ) {
    EDITOR__PUNCHER Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            vec2 Position = _read( ptr, vec2 );
            
            EDITOR__PUNCHER Puncher = {};
            Puncher.Position = Position;
            
            Result = Puncher;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadPuncher( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__PUNCHER;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        PUNCHER_STATE * PuncherS = &AppState->PuncherS;
        
        PuncherS->nPuncher = _read( ptr, uint32 );
        for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
            EDITOR__PUNCHER Src = ReadPuncher( Version, &ptr );
            
            PUNCHER Puncher = PuncherC( Src.Position );
            PuncherS->Puncher[ iPuncher ] = Puncher;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadPuncher( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__PUNCHER;
    EDITOR_STATE             * Editor      = &AppState->Editor;
    EDITOR__PUNCHER_STATE * PuncherS = &Editor->PuncherS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        PuncherS->nPuncher = _read( ptr, uint32 );
        for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
            EDITOR__PUNCHER Src = ReadPuncher( Version, &ptr );
            PuncherS->Puncher[ iPuncher ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputPuncherToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS              Stat      = Editor->Stat;
    EDITOR__PUNCHER_STATE * PuncherS = &Editor->PuncherS;
    
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        EDITOR__PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
        
        vec4 Color = ToColor( 180, 90, 45 );
        rect Bound = RectBCD( Puncher.Position, PUNCHER_DIM );
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}