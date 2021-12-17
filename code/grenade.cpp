
internal vec2
getGroundP( GRENADE Grenade ) {
    vec2 Result = Grenade.Position + Vec2( 0.0f, -GRENADE_RADIUS );
    return Result;
}

internal void
UpdateGrenade( APP_STATE * AppState, flo32 dT ) {
    GRENADE_STATE   * State     = &AppState->Grenade;
    COLLISION_STATE * Collision = &AppState->Collision;
    PLAYER_STATE    * Player    = &AppState->Player;
    
    if( ( Player->Throw_WasPressed ) && ( Player->GrenadeThrow_CooldownTimer <= 0.0f ) ) {
        Player->Throw_WasPressed           = false;
        Player->GrenadeThrow_CooldownTimer = PLAYER_GRENADE_COOLDOWN_TARGET_TIME;
        vec2 PlayerP = GetPlayerAttackP( *Player );
        
        flo32 Speed = GetLength( Player->Velocity ) * 0.5f;
        vec2  Dir   = GetNormal( Player->Velocity );
        if( GetLengthSq( Player->Aim_Dir ) > 0.0f ) {
            Dir     = Player->Aim_Dir;
            flo32 t = Clamp01( ( Dir.y + 1.0f ) / 2.0f );
            Speed   = lerp( GRENADE_THROW_SPEED_LO, t, GRENADE_THROW_SPEED_HI );;
        }
        
        GRENADE Grenade = {};
        Grenade.Position = PlayerP;
        Grenade.Velocity = Dir * Speed;
        
        if( State->nGrenade < GRENADE_MAX_COUNT ) {
            State->Grenade[ State->nGrenade++ ] = Grenade;
        } else {
            char str[ 512 ] = {};
            sprintf( str, "ERROR! Attempted to spawn GRENADE, but GRENADE_STATE was full! MaxCount = %u", GRENADE_MAX_COUNT );
            CONSOLE_STRING( str );
            GlobalVar_DebugSystem.ErrorOccurred = true;
        }
    }
    
    for( uint32 iGrenade = 0; iGrenade < State->nGrenade; iGrenade++ ) {
        GRENADE * Grenade = State->Grenade + iGrenade;
        
        flo32 Friction = GRENADE_AIR_FRICTION;
        boo32 IsOnGround = false;
        { // IsOnGround : TERRAIN
            for( uint32 iBound = 0; iBound < Collision->nJumpBound; iBound++ ) {
                rect R = Collision->JumpBound[ iBound ];
                
                vec2 GrenadeP = getGroundP( *Grenade );
                if( IsInBound( GrenadeP, R ) ) {
                    IsOnGround = true;
                }
            }
        }
        if( IsOnGround ) {
            Friction = GRENADE_GROUND_FRICTION;
        }
        
        vec2 Accel = {};
        Accel.x = -Grenade->Velocity.x * Friction;
        Accel.y = -GRENADE_GRAVITY;
        
        vec2 dP = Accel * ( dT * dT * 0.5f ) + Grenade->Velocity * dT;
        Grenade->Velocity += Accel * dT;
        
        COLLISION_RESULT Coll = UpdateGrenadeCollision( AppState, Grenade->Position, Grenade->Velocity, dP, GRENADE_RADIUS );
        
        Grenade->Position = Coll.Pos;
        Grenade->Velocity = Coll.Vel;
        
        Grenade->Timer += dT;
        if( Grenade->Timer >= GRENADE_DETONATE_TARGET_TIME ) {
            // TODO: different Damages at different Radius?
            addGrenadeDetonate( &AppState->ParticleS, Grenade->Position );
            
            { // PLAYER
                rect R = GetPlayerGrenadeBound( *Player );
                if( IsInBound( Grenade->Position, R ) ) {
                    DamagePlayer( AppState, GRENADE_DAMAGE_TO_PLAYER );
                }
            }
        }
    }
}

internal void
DrawGrenade( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    GRENADE_STATE * State = Draw->Grenade;
    for( uint32 iGrenade = 0; iGrenade < State->nGrenade; iGrenade++ ) {
        GRENADE Grenade = State->Grenade[ iGrenade ];
        
        vec4 Color = GRENADE_COLOR;
        
        flo32 CycleTimeA  = ( 6.0f / 60.0f );
        flo32 CycleTimeB  = ( 4.0f / 60.0f );
        flo32 TargetTimeB = GRENADE_DETONATE_TARGET_TIME - ( CycleTimeB * 3.0f );
        flo32 TargetTimeA = TargetTimeB - ( CycleTimeA * 2.0f );
        if( Grenade.Timer >= TargetTimeB ) {
            uint32 Cycle = ( uint32 )( ( Grenade.Timer - TargetTimeB ) / CycleTimeB );
            if( ( Cycle % 2 ) == 0 ) {
                Color = ToColor( 220, 0, 0 );
            }
        } else if( Grenade.Timer >= TargetTimeA ) {
            uint32 Cycle = ( uint32 )( ( Grenade.Timer - TargetTimeA ) / CycleTimeA );
            if( ( Cycle % 2 ) == 0 ) {
                Color = ToColor( 220, 0, 0 );
            }
        }
        DrawCircle       ( Pass, Grenade.Position, GRENADE_RADIUS, Color );
        DrawCircleOutline( Pass, Grenade.Position, GRENADE_RADIUS, COLOR_BLACK );
    }
}

internal void
DrawGrenadeDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    { // GRENADE
        GRENADE_STATE * State = Draw->Grenade;
        for( uint32 iGrenade = 0; iGrenade < State->nGrenade; iGrenade++ ) {
            GRENADE Grenade = State->Grenade[ iGrenade ];
            
            if( Grenade.Timer >= GRENADE_DETONATE_TARGET_TIME ) {
                DrawCircleOutline( Pass, Grenade.Position, GRENADE_DAMAGE_RADIUS, COLOR_YELLOW );
            }
        }
    }
    
    { // PLAYER
        PLAYER_STATE * Player = Draw->Player;
        rect R = GetPlayerGrenadeBound( *Player );
        DrawRectOutline( Pass, R, COLOR_YELLOW );
    }
}

internal void
FinalizeGrenade( APP_STATE * AppState ) {
    GRENADE_STATE * State = &AppState->Grenade;
    
    uint32 nGrenade = 0;
    for( uint32 iGrenade = 0; iGrenade < State->nGrenade; iGrenade++ ) {
        GRENADE Grenade = State->Grenade[ iGrenade ];
        if( Grenade.Timer < GRENADE_DETONATE_TARGET_TIME ) {
            State->Grenade[ nGrenade++ ] = Grenade;
        }
    }
    State->nGrenade = nGrenade;
}