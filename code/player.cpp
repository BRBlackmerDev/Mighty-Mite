
#define GAME_MAX_STAMINA_XP_FOR_LEVEL  flo32 MaxStaminaXPForLevel[] = {\
70.0f,   250.0f,  500.0f,  1000.0f,  2000.0f,  4000.0f, }
#define GAME_MAX_STAMINA_FOR_LEVEL     flo32 MaxStaminaForLevel[]   = {\
15.0f,    25.0f,   50.0f,   100.0f,   150.0f,   200.0f, }

internal void
SetMaxStamina( GAME_STATE * Game ) {
    GAME_MAX_STAMINA_XP_FOR_LEVEL;
    GAME_MAX_STAMINA_FOR_LEVEL;
    
    Game->MaxStamina   = MaxStaminaForLevel  [ Game->Stamina_Level ];
    Game->MaxStaminaXP = MaxStaminaXPForLevel[ Game->Stamina_Level ];
}

internal void
UpdateStaminaXP( GAME_STATE * Game, PLAYER_STATE * Player ) {
    GAME_MAX_STAMINA_XP_FOR_LEVEL;
    GAME_MAX_STAMINA_FOR_LEVEL;
    uint32 MaxStaminaLevel = _ArrayCount( MaxStaminaXPForLevel );
    if( ( Game->Stamina_Level < MaxStaminaLevel ) && ( Game->StaminaXP >= Game->MaxStaminaXP ) ) {
        Game->StaminaXP -= Game->MaxStaminaXP;
        Game->Stamina_Level++;
        
        SetMaxStamina( Game );
        
        Player->Stamina = Game->MaxStamina;
    }
}

internal vec2
GetPlayerAttackP( PLAYER_STATE Player ) {
    flo32 Height = PLAYER_HEIGHT * 0.75f;
    vec2  Result = Player.Position + Vec2( 0.0f, Height );
    return Result;
}

internal vec2
GetPlayerUseP( PLAYER_STATE Player ) {
    flo32 Height = PLAYER_HEIGHT * 0.65f;
    vec2  Result = Player.Position + Vec2( 0.0f, Height );
    return Result;
}

internal vec2
GetPlayerCameraP( PLAYER_STATE Player ) {
    vec2  Result = Player.Position + Vec2( 0.0f, PLAYER_HEIGHT * 0.6f );
    return Result;
}

internal rect
GetPlayerGrenadeBound( PLAYER_STATE Player ) {
    rect Result = {};
    Result = RectBCD( Player.Position, Vec2( PLAYER_WIDTH, PLAYER_HEIGHT ) );
    Result = AddRadius( Result, TILE_WIDTH * 4.0f );
    return Result;
}

internal void
RandomlyAdjustFireDirection( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    Player->Fire_doHitScan = true;
    
    flo32 radians = atan2f( Player->Aim_Dir.y, Player->Aim_Dir.x );
    
    flo32 range = 0.0625f / PI;
    radians += RandomF32InRange( -range, range );
    vec2 N = Vec2( cosf( radians ), sinf( radians ) );
    
    vec2 offset = GetPerp( N ) * RandomF32InRange( -0.0625f, 0.0625f );
    
    flo32 t = 0.85f;
    vec2 P = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT * 0.75f ) + offset;
    
    Player->Fire_pos = P + offset;
    Player->Fire_dir = N;
    
    { // muzzle flash
        flo32 Dim = RandomF32InRange( 0.085f, 0.095f );
        
        PARTICLE p = {};
        p.Position   = P + Player->Fire_dir * t;
        p.Dim        = Vec2( Dim, Dim );
        p.Radians    = RandomF32InRange( 0.0f, PI * 0.25f );
        p.TargetTime = ( 1.0f / 60.0f );
        p.Color      = COLOR_WHITE;
        
        PARTICLE_STATE * Particle = &AppState->ParticleS;
        Particle->Particle[ Particle->nParticle++ ] = p;
    }
}

internal void
IsPlayerOnConveyor( APP_STATE * AppState, PLAYER_STATE * Player ) {
    CONVEYOR_STATE * ConveyorS = &AppState->ConveyorS;
    
    Player->IsOnConveyorHor      = false;
    Player->IsOnConveyorVer      = false;
    Player->IsOnConveyorVerLeft  = false;
    Player->IsOnConveyorVerRight = false;
    vec2 PlayerP = Player->Position;
    
    for( uint32 iConveyor = 0; iConveyor < ConveyorS->Type[ 3 ].n; iConveyor++ ) {
        CONVEYOR Conveyor = ConveyorS->Conveyor[ ConveyorS->Type[ 3 ].m + iConveyor ];
        if( IsInBound( PlayerP, Conveyor.Bound_Mvt ) ) {
            Player->IsOnConveyorHor = true;
            Player->ConveyorAccel   = Conveyor.Accel;
        }
    }
    
    for( uint32 iConveyor = 0; iConveyor < ConveyorS->Type[ 0 ].n; iConveyor++ ) {
        CONVEYOR Conveyor = ConveyorS->Conveyor[ ConveyorS->Type[ 0 ].m + iConveyor ];
        if( IsInBound( PlayerP, Conveyor.Bound_Mvt ) ) {
            Player->IsOnConveyorVer     = true;
            Player->IsOnConveyorVerLeft = true;
            Player->ConveyorAccel       = Conveyor.Accel;
        }
    }
    
    for( uint32 iConveyor = 0; iConveyor < ConveyorS->Type[ 2 ].n; iConveyor++ ) {
        CONVEYOR Conveyor = ConveyorS->Conveyor[ ConveyorS->Type[ 2 ].m + iConveyor ];
        if( IsInBound( PlayerP, Conveyor.Bound_Mvt ) ) {
            Player->IsOnConveyorVer      = true;
            Player->IsOnConveyorVerRight = true;
            Player->ConveyorAccel        = Conveyor.Accel;
        }
    }
}

internal SCAFFOLD_DOWN_RESULT
IsPlayerOnScaffoldDown( APP_STATE * AppState ) {
    SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    vec2 PlayerP = Player->Position;
    
    SCAFFOLD_DOWN_RESULT Result = {};
    Result.IsOnScaffoldDown = false;
    
    if( !Player->Scaffold_SkipCollision ) {
        for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
            SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
            
            if( IsInBound( PlayerP, Scaffold.Bound_JumpDown ) ) {
                Result.IsOnScaffoldDown = true;
                Result.iScaffold        = iScaffold;
            }
        }
    }
    
    return Result;
}

internal boo32
IsPlayerOnScaffoldUp( APP_STATE * AppState ) {
    SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    vec2 PlayerP = Player->Position;
    
    boo32 Result = false;
    
    if( !Player->Scaffold_SkipCollision ) {
        for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
            SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
            
            if( IsInBound( PlayerP, Scaffold.Bound_JumpUp ) ) {
                Result = true;
            }
        }
    }
    
    return Result;
}

internal boo32
IsPlayerOnSpikes( APP_STATE * AppState ) {
    SPIKES_STATE * SpikesS = &AppState->SpikesS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    vec2 PlayerP = Player->Position;
    
    boo32 Result = false;
    
    for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
        SPIKES Spikes = SpikesS->Spikes[ iSpikes ];
        
        if( IsInBound( PlayerP, Spikes.Bound_Dmg ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal ENTITY_VALID_RESULT
IsPosByBoulder( APP_STATE * AppState, vec2 Pos ) {
    BOULDER_STATE * BoulderS = &AppState->BoulderS;
    
    ENTITY_VALID_RESULT Result = {};
    
    flo32 MaxDistSq = FLT_MAX;
    for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
        BOULDER Boulder = BoulderS->Boulder[ iBoulder ];
        
        rect R = RectCD( Boulder.Position, BOULDER_DIM );
        if( IsInBound( Pos, R ) ) {
            flo32 DistSq = GetLengthSq( Pos - Boulder.Position );
            if( DistSq < MaxDistSq ) {
                MaxDistSq = DistSq;
                
                Result.IsValid = true;
                Result.iEntity = iBoulder;
            }
        }
    }
    
    return Result;
}

internal boo32
IsRunnerTouchingPlayer( APP_STATE * AppState ) {
    RUNNER_STATE * RunnerS = &AppState->RunnerS;
    PLAYER_STATE * Player  = &AppState->Player;
    
    rect PlayerBound = GetPlayerBoundToBeDamagedByRunner( Player->Position );
    
    boo32 Result = false;
    for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
        RUNNER Runner = RunnerS->Runner[ iRunner ];
        if( IsInBound( Runner.Position, PlayerBound ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal boo32
IsStomperTouchingPlayer( APP_STATE * AppState ) {
    STOMPER_STATE * StomperS = &AppState->StomperS;
    PLAYER_STATE  * Player  = &AppState->Player;
    
    rect PlayerBound = GetPlayerBoundToBeDamagedByStomper( Player->Position );
    
    boo32 Result = false;
    for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
        STOMPER Stomper = StomperS->Stomper[ iStomper ];
        if( IsInBound( Stomper.Position, PlayerBound ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal void
UpdatePlayerInput( APP_STATE * AppState, CONTROLLER_STATE * Control, flo32 dT ) {
    GAME_STATE   * Game   = &AppState->Game;
    PLAYER_STATE * Player = &AppState->Player;
    Player->dPos = {};
    Player->Use_WasPressed   = false;
    Player->Use_WasReleased  = false;
    Player->Throw_WasPressed = false;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    
    CONTROLLER_BUTTON_ID  ID_Jump  = ControllerButton_Shoulder_Left;
    //CONTROLLER_BUTTON_ID  ID_Dash  = ControllerButton_Shoulder_Right;
    CONTROLLER_BUTTON_ID  ID_Dash  = ControllerButton_X;
    CONTROLLER_BUTTON_ID  ID_Run   = ControllerButton_X;
    CONTROLLER_BUTTON_ID  id_use   = ControllerButton_Y;
    CONTROLLER_THUMBSTICK ID_Move  = Control->Left;
    CONTROLLER_THUMBSTICK id_aim   = Control->Right;
    CONTROLLER_BUTTON_ID  ID_Drop  = ControllerButton_A;
    CONTROLLER_BUTTON_ID  ID_Throw  = ControllerButton_Trigger_Right;
    CONTROLLER_BUTTON_ID  ID_PickUp = ControllerButton_Trigger_Right;
    CONTROLLER_BUTTON_ID  ID_Punch  = ControllerButton_Shoulder_Right;
    CONTROLLER_BUTTON_ID  ID_Grab   = ControllerButton_Trigger_Right;
    CONTROLLER_BUTTON_ID  ID_Charge = ControllerButton_A;
    CONTROLLER_BUTTON_ID  ID_StaminaCharge = ControllerButton_A;
    
    if( Player->IsDead ) {
        flo32 Friction = PLAYER_DEAD_FRICTION;
        
        vec2 Accel = {};
        Accel.x = -Player->Velocity.x * Friction;
        Accel.y = -PLAYER_GRAVITY;
        
        Player->dPos      = Accel * ( dT * dT * 0.5f ) + Player->Velocity * dT;
        Player->Velocity += Accel * dT;
        
        if( WasPressed( Control, id_use ) ) {
            AppState->LoadCheckpoint_IsActive = true;
        }
        if( WasReleased( Control, id_use ) ) {
            AppState->LoadCheckpoint_IsActive = false;
        }
    } else {
        flo32 Move_NormalX = 0.0f;
        flo32 Move_NormalY = 0.0f;
        
        vec2 dir = {};
        { // move Left/Right
            if( ID_Move.xy.x != 0 ) {
                flo32 Accel = 0.0f;
                if( ID_Move.xy.x < -ID_Move.dead ) {
                    Accel = -1.0f;
                }
                if( ID_Move.xy.x >  ID_Move.dead ) {
                    Accel =  1.0f;
                }
                dir.x = Accel;
            }
        }
        
        { // up/down
            if( ID_Move.xy.y != 0 ) {
                if( ID_Move.xy.y < -ID_Move.dead ) {
                    Move_NormalY = ( flo32 )ID_Move.xy.y / ( flo32 )SHRT_MAX;
                    
                    dir.y = -1.0f;
                    if( ID_Move.xy.y > SHRT_MIN ) {
                        flo32 numer = ( flo32 )( ID_Move.xy.y + ID_Move.dead );
                        flo32 Denom = ( flo32 )( SHRT_MAX - ID_Move.dead );
                        dir.y = Clamp( numer / Denom, -1.0f, 0.0f );
                    }
                }
                if( ID_Move.xy.y >  ID_Move.dead ) {
                    dir.y =  1.0f;
                }
            }
        }
        
        Player->Control_xDir = dir.x;
        
        if( dir.x != 0.0f ) {
            Player->Face_DirX = dir.x;
        }
        
        boo32 IsOnGround    = false;
        boo32 IsOnWallLeft  = false;
        boo32 IsOnWallRight = false;
        boo32 IsFalling     = ( Player->Velocity.y < 0.0f );
        boo32 IsMovingLeft  = ( dir.x < 0.0f );
        boo32 IsMovingRight = ( dir.x > 0.0f );
        boo32 IsMovingDown  = ( dir.y < 0.0f );
        boo32 IsMovingUp    = ( dir.y > 0.0f );
        //boo32 IsAttemptingDrop = ( Move_NormalY < -0.75f );
        //DISPLAY_VALUE( boo32, IsAttemptingDrop );
        //DISPLAY_VALUE( flo32, Move_NormalY );
        
        { // IsOnGround
            for( uint32 iJumpBound = 0; iJumpBound < Collision->nJumpBound; iJumpBound++ ) {
                rect R = Collision->JumpBound[ iJumpBound ];
                
                if( IsInBound( Player->Position, R ) ) {
                    IsOnGround = true;
                }
            }
            
            boo32 IsOnScaffoldUp = IsPlayerOnScaffoldUp( AppState );
            if( IsOnScaffoldUp ) {
                IsOnGround = true;
            }
            
            boo32 IsOnPushBlock = IsPosOnPushBlock( AppState, Player->Position, PLAYER_HALF_WIDTH );
            if( IsOnPushBlock ) {
                IsOnGround = true;
            }
            
            boo32 IsOnBreakBlock = IsPosOnBreakBlock( AppState, Player->Position, PLAYER_HALF_WIDTH );
            if( IsOnBreakBlock ) {
                IsOnGround = true;
            }
            
            boo32 IsOnCollapsePlatform = IsPosOnCollapsePlatform( AppState, Player->Position );
            if( IsOnCollapsePlatform ) {
                IsOnGround = true;
            }
        }
        
        { // IsOnWallLeft
            for( uint32 iBound = 0; iBound < Collision->nWallSlideLeft; iBound++ ) {
                rect R = Collision->WallSlideLeft[ iBound ];
                
                if( IsInBound( Player->Position, R ) ) {
                    IsOnWallLeft = true;
                }
            }
        }
        
        { // IsOnWallRight
            for( uint32 iBound = 0; iBound < Collision->nWallSlideRight; iBound++ ) {
                rect R = Collision->WallSlideRight[ iBound ];
                
                if( IsInBound( Player->Position, R ) ) {
                    IsOnWallRight = true;
                }
            }
        }
        
        ENTITY_VALID_RESULT IsByPushBlock = IsInGrabBoundOfPushBlock( AppState, Player->Position );
        
        boo32 CanGrab = ( Game->Grab_IsFeatured )
            && ( Player->Carry_ID == PlayerCarryID_None )
            && ( !Player->Grab_IsActive )
            && ( !Player->GrabExerciseMiniBoss03_IsActive )
            && ( !Player->Punch_IsCharging )
            && ( Player->Stamina_IsEnabled )
            && ( Player->Stamina > 0.0f )
            && ( IsOnGround )
            && ( IsByPushBlock.IsValid );
        //&& ( !Player->Dash_IsActive )
        
        boo32 IsByExerciseMiniBoss03 = IsInGrabBoundOfExerciseMiniBoss03( AppState, Player->Position );
        boo32 CanGrabExerciseMiniBoss03 = ( Game->Grab_IsFeatured )
            && ( Player->Carry_ID == PlayerCarryID_None )
            && ( !Player->Grab_IsActive )
            && ( !Player->GrabExerciseMiniBoss03_IsActive )
            && ( !Player->Punch_IsCharging )
            && ( Player->Stamina_IsEnabled )
            && ( Player->Stamina > 0.0f )
            && ( IsOnGround )
            && ( IsByExerciseMiniBoss03 );
        
        Player->Grab_IsDown = IsDown( Control, ID_Grab );
        if( CanGrab ) {
            PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
            PUSH_BLOCK         PushBlock  = PushBlockS->PushBlock[ IsByPushBlock.iEntity ];
            flo32 dX = PushBlock.Position.x - Player->Position.x;
            
            Player->CanGrab      = true;
            Player->Grab_ArtDirX = dX / fabsf( dX );
        } else if( CanGrabExerciseMiniBoss03 ) {
            Player->CanGrab      = true;
            Player->Grab_ArtDirX = 1.0f;
        } else {
            Player->CanGrab = false;
        }
        
        
        
#if 0        
        boo32 CanPunch = ( Player->Punch_IsFeatured )
            && ( !Player->CarryBoulder_IsActive ) // TODO: Should I allow a punch if carrying, but cancel carry?
            && ( !Player->CarryExerciseBall_IsActive ) // TODO: Should I allow a punch if carrying, but cancel carry?
            && ( !Player->Grab_IsActive )  // TODO: Should I allow a punch if grabbing, but cancel grab?
            && (  Player->Stamina_IsEnabled )
            && (  Player->Stamina >= 0.0f );
#endif
        
        rect PlayerBound = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM );
        CONVEYOR_RESULT ConveyorHor = IsOnConveyorH( AppState, Player->Position, PlayerBound );
        
        Player->Boulder_IsBy = IsPosByBoulder( AppState, Player->Position );
        Player->ExerciseBall_IsBy = IsPosByExerciseBall( AppState, Player->Position, RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM ) );
        
        rect PickUpBound = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM );
        // TODO: Tune this!!
        ENTITY_VALID_RESULT IsByStunnedPuncher = IsInPuncherPickUpBound( AppState, Player->Position, PickUpBound );
        ENTITY_VALID_RESULT IsByStunnedPopper = IsInPopperPickUpBound( AppState, Player->Position, PickUpBound );
        ENTITY_VALID_RESULT IsByCamper = IsInCamperPickUpBound( AppState, Player->Position, PickUpBound );
        ENTITY_VALID_RESULT IsByBarbell = IsPosInBarbellPickUpBound( AppState, Player->Position, PickUpBound );
        
        Player->Carry_IsBy_ID = PlayerCarryID_None;
        if( Player->ExerciseBall_IsBy.IsValid ) {
            Player->Carry_IsBy_ID      = PlayerCarryID_ExerciseBall;
            Player->Carry_IsBy_iEntity = Player->ExerciseBall_IsBy.iEntity;
        }
        if( Player->Boulder_IsBy.IsValid ) {
            Player->Carry_IsBy_ID      = PlayerCarryID_Boulder;
            Player->Carry_IsBy_iEntity = Player->Boulder_IsBy.iEntity;
        }
        if( IsByStunnedPuncher.IsValid ) {
            Player->Carry_IsBy_ID      = PlayerCarryID_Puncher;
            Player->Carry_IsBy_iEntity = IsByStunnedPuncher.iEntity;
        }
        if( IsByStunnedPopper.IsValid ) {
            Player->Carry_IsBy_ID      = PlayerCarryID_Popper;
            Player->Carry_IsBy_iEntity = IsByStunnedPopper.iEntity;
        }
        if( IsByCamper.IsValid ) {
            Player->Carry_IsBy_ID      = PlayerCarryID_Camper;
            Player->Carry_IsBy_iEntity = IsByCamper.iEntity;
        }
        if( IsByBarbell.IsValid ) {
            Player->Carry_IsBy_ID      = PlayerCarryID_Barbell;
            Player->Carry_IsBy_iEntity = IsByBarbell.iEntity;
        }
        
        boo32 CanPickUp = ( Game->Carry_IsFeatured )
            && ( Player->Carry_ID      == PlayerCarryID_None )
            && ( Player->Carry_IsBy_ID != PlayerCarryID_None )
            && ( !Player->Grab_IsActive )
            && ( !Player->GrabExerciseMiniBoss03_IsActive );
        //&& ( !Player->Jump_IsActive )
        //&& (  IsOnGround )
        //&& (  Player->Stamina_IsEnabled )
        //&& (  Player->Stamina >= PLAYER_STAMINA_PICKUP_COST );
        //&& ( !Player->Dash_IsActive )
        
        //&& (  Player->Stamina_IsEnabled )
        //&& (  Player->Stamina >= PLAYER_STAMINA_PICKUP_COST );
        //&& ( !Player->Jump_IsActive )
        //&& (  IsOnGround )
        //&& ( !Player->Dash_IsActive )
        
        Player->CanPickUp = CanPickUp;
        if( dir.x != 0.0f ) {
            Player->PickUp_DirX = dir.x;
        }
        
        ENTITY_VALID_RESULT IsOnExerciseBall = IsPosOnExerciseBall( AppState, Player->Position );
        ENTITY_VALID_RESULT IsOnSpotter      = IsPosOnSpotter     ( AppState, Player->Position );
        
        boo32 CanThrow = ( Game->Carry_IsFeatured )
            && ( Player->Carry_ID != PlayerCarryID_None );
        
        if( ( !Player->Dash_IsActive ) && ( IsOnGround ) ) {
            Player->Dash_DidTouchGround = true;
        }
        
        Player->Dash_IsEnabled = false;
        if( ( Player->Dash_DidTouchGround ) && ( Player->Dash_CooldownTimer <= 0.0f ) ) {
            Player->Dash_IsEnabled = true;
        }
        
        boo32 CanDash = ( Game->Dash_IsFeatured )
            && ( Player->Dash_IsEnabled )
            && ( Player->Carry_ID == PlayerCarryID_None )
            && ( !Player->Grab_IsActive )
            && ( Player->Stamina_IsEnabled )
            && ( Player->Stamina > 0.0f );
        
        boo32 CanRun = ( Game->Run_IsFeatured )
            && (   ( Player->Carry_ID == PlayerCarryID_None         )
                || ( Player->Carry_ID == PlayerCarryID_Camper       )
                || ( Player->Carry_ID == PlayerCarryID_ExerciseBall ) )
            && ( !Player->Dash_IsActive )
            && ( Player->Stamina_IsEnabled );
        
        
        boo32 CanWallJumpToLeft = ( Game->WallJump_IsFeatured )
            && ( IsOnWallLeft )
            && ( !IsOnGround );
        
        boo32 CanWallJumpToRight = ( Game->WallJump_IsFeatured )
            && ( IsOnWallRight )
            && ( !IsOnGround );
        
        boo32 CanWallSlideOnLeftWall = ( Game->WallSlide_IsFeatured )
            && ( IsOnWallLeft  )
            && ( IsFalling )
            && ( !IsOnGround )
            && ( IsMovingRight );
        //&& ( !Player->IsOnConveyorVer )
        
        boo32 CanWallSlideOnRightWall = ( Game->WallSlide_IsFeatured )
            && ( IsOnWallRight )
            && ( IsFalling )
            && ( !IsOnGround )
            && ( IsMovingLeft );
        //&& ( !Player->IsOnConveyorVer )
        
        boo32 CanJump = ( ( IsOnGround ) || ( IsOnExerciseBall.IsValid ) || ( IsOnSpotter.IsValid ) )
            && ( !Player->Grab_IsActive )
            && ( !Player->GrabExerciseMiniBoss03_IsActive )
            && ( Player->Stamina_IsEnabled )
            && ( Player->Stamina >= 0.0f );
        
        SCAFFOLD_DOWN_RESULT ScaffoldDown = IsPlayerOnScaffoldDown( AppState );
        boo32 CanDropThroughScaffold = ( ScaffoldDown.IsOnScaffoldDown );
        
        if( WasPressed( Control, id_use ) ) {
            Player->Use_WasPressed = true;
            Player->Use_IsActive   = true;
        }
        if( WasReleased( Control, id_use ) ) {
            Player->Use_WasReleased = true;
            Player->Use_IsActive    = false;
        }
        
        { // aim dir
            vec2 V = Vec2( ( flo32 )id_aim.xy.x, ( flo32 )id_aim.xy.y );
            
            boo32 prev = Player->Fire_IsActive;
            if( ( GetLength( V ) > ( flo32 )id_aim.dead ) ) {
                Player->Aim_Dir       = GetNormal( V );
                Player->Fire_IsActive = true;
            } else {
                Player->Aim_Dir       = Vec2( 0.0f, 0.0f );
                Player->Fire_IsActive = false;
            }
            
            //if( ( Player->Fire_IsActive != prev ) && ( Player->Fire_IsActive ) ) {
            //Player->Fire_Timer = 0.0f;
            //RandomlyAdjustFireDirection( AppState );
            //}
        }
        
        if( ( CanPickUp ) && ( WasPressed( Control, ID_PickUp ) ) ) {
            switch( Player->Carry_IsBy_ID ) {
                case PlayerCarryID_None: {
                    InvalidCodePath;
                } break;
                
                case PlayerCarryID_Camper: {
                    if( Player->Stamina_IsEnabled ) {
                        Player->Stamina  = MaxValue( Player->Stamina - PLAYER_STAMINA_PICKUP_COST * 0.5f, 0.0f );
                        Game->StaminaXP += PLAYER_STAMINA_XP_PICKUP_XP * 0.5f;
                        RemoveCamper( AppState, Player->Carry_IsBy_iEntity );
                        
                        Player->Carry_ID = PlayerCarryID_Camper;
                        
                        CAMPER_STATE * CamperS = &AppState->CamperS;
                        CAMPER         Camper  = CamperS->Camper[ Player->Carry_IsBy_iEntity ];
                        Player->Camper_IsCarrying = Camper;
                        
                        PlayAudio( AppState, AudioID_Sound_Camper_PickUp, 0.5f );
                        
                        CanGrab = false;
                    }
                } break;
                
                case PlayerCarryID_Boulder: {
                    if( Player->Stamina_IsEnabled ) {
                        Player->Stamina  = MaxValue( Player->Stamina - PLAYER_STAMINA_PICKUP_COST, 0.0f );
                        Game->StaminaXP += PLAYER_STAMINA_XP_PICKUP_XP;
                        RemoveBoulder( AppState, Player->Carry_IsBy_iEntity );
                        
                        Player->Carry_ID = PlayerCarryID_Boulder;
                        
                        CanRun  = false;
                        CanGrab = false;
                    }
                } break;
                
                case PlayerCarryID_ExerciseBall: {
                    RemoveExerciseBall( AppState, Player->Carry_IsBy_iEntity );
                    Player->Carry_ID = PlayerCarryID_ExerciseBall;
                    
                    CanGrab = false;
                } break;
                
                case PlayerCarryID_Puncher: {
                    if( Player->Stamina_IsEnabled ) {
                        Player->Stamina  = MaxValue( Player->Stamina - PLAYER_STAMINA_PICKUP_COST, 0.0f );
                        Game->StaminaXP += PLAYER_STAMINA_XP_PICKUP_XP;
                        
                        PUNCHER_STATE * PuncherS = &AppState->PuncherS;
                        PUNCHER         Puncher  = PuncherS->Puncher[ Player->Carry_IsBy_iEntity ];
                        Player->Carry_PuncherHealth = Puncher.Health;
                        
                        RemovePuncher( AppState, Player->Carry_IsBy_iEntity );
                        Player->Carry_ID = PlayerCarryID_Puncher;
                        
                        CanRun  = false;
                        CanGrab = false;
                    }
                } break;
                
                case PlayerCarryID_Popper: {
                    if( Player->Stamina_IsEnabled ) {
                        Player->Stamina    = MaxValue( Player->Stamina - PLAYER_STAMINA_PICKUP_COST * 0.5f, 0.0f );
                        Game->StaminaXP += PLAYER_STAMINA_XP_PICKUP_XP * 0.5f;
                        
                        RemovePopper( AppState, Player->Carry_IsBy_iEntity );
                        Player->Carry_ID = PlayerCarryID_Popper;
                        
                        CanRun  = false;
                        CanGrab = false;
                    }
                } break;
                
                case PlayerCarryID_Barbell: {
                    if( Player->Stamina_IsEnabled ) {
                        Player->Stamina    = MaxValue( Player->Stamina - PLAYER_STAMINA_PICKUP_COST * 0.5f, 0.0f );
                        Game->StaminaXP += PLAYER_STAMINA_XP_PICKUP_XP * 0.5f;
                        
                        RemoveBarbell( AppState, Player->Carry_IsBy_iEntity );
                        Player->Carry_ID = PlayerCarryID_Barbell;
                        
                        CanRun  = false;
                        CanGrab = false;
                    }
                } break;
            }
            
        }
        
        if( ( CanThrow ) && ( WasPressed( Control, ID_Throw ) ) ) {
            switch( Player->Carry_ID ) {
                case PlayerCarryID_None: {
                    InvalidCodePath;
                } break;
                
                case PlayerCarryID_Camper: {
                    vec2 DropP = Player->CarryP + Vec2( -Player->Face_DirX * TILE_WIDTH * 0.15f, -TILE_HEIGHT * 0.8f );
                    
                    Player->Camper_IsCarrying.Position = DropP;
                    AddCamper( AppState, Player->Camper_IsCarrying );
                    
                    PlayAudio( AppState, AudioID_Sound_Camper_Drop, 0.5f );
                    
                    Player->Carry_ID = PlayerCarryID_None;
                } break;
                
                case PlayerCarryID_Boulder: {
                    if( ( Player->Stamina_IsEnabled ) && ( Player->Stamina >= 0.0f ) ) {
                        Player->Stamina    = MaxValue( Player->Stamina - PLAYER_STAMINA_THROW_COST, 0.0f );
                        Game->StaminaXP += PLAYER_STAMINA_XP_THROW_XP;
                        
                        AddBoulder( AppState, Player->CarryP, Player->Aim_Dir * BOULDER_THROW_SPEED );
                        Player->Carry_ID = PlayerCarryID_None;
                    }
                } break;
                
                case PlayerCarryID_ExerciseBall: {
                    AddExerciseBall( AppState, Player->CarryP, Player->Aim_Dir * EXERCISE_BALL_THROW_SPEED );
                    Player->Carry_ID = PlayerCarryID_None;
                } break;
                
                case PlayerCarryID_Puncher: {
                    if( ( Player->Stamina_IsEnabled ) && ( Player->Stamina >= 0.0f ) ) {
                        Player->Stamina    = MaxValue( Player->Stamina - PLAYER_STAMINA_THROW_COST, 0.0f );
                        Game->StaminaXP += PLAYER_STAMINA_XP_THROW_XP;
                        
                        uint32 Damage = MinValue( Player->Carry_PuncherHealth, ( uint32 )1 );
                        Player->Carry_PuncherHealth -= Damage;
                        
                        AddPuncher( AppState, PuncherStunnedC( Player->CarryP, Player->Aim_Dir * PUNCHER_THROW_SPEED, Player->Carry_PuncherHealth, true ) );
                        Player->Carry_ID = PlayerCarryID_None;
                    }
                } break;
                
                case PlayerCarryID_Popper: {
                    if( ( Player->Stamina_IsEnabled ) && ( Player->Stamina >= 0.0f ) ) {
                        Player->Stamina    = MaxValue( Player->Stamina - PLAYER_STAMINA_THROW_COST * 0.5f, 0.0f );
                        Game->StaminaXP += PLAYER_STAMINA_XP_THROW_XP * 0.5f;
                        vec2 PopperP = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT );
                        
                        AddPopper( AppState, PopperThrownC( Player->CarryP, Player->Aim_Dir * POPPER_THROW_SPEED ) );
                        Player->Carry_ID = PlayerCarryID_None;
                    }
                } break;
                
                case PlayerCarryID_Barbell: {
                    if( ( Player->Stamina_IsEnabled ) && ( Player->Stamina >= 0.0f ) ) {
                        Player->Stamina    = MaxValue( Player->Stamina - PLAYER_STAMINA_THROW_COST * 0.5f, 0.0f );
                        Game->StaminaXP += PLAYER_STAMINA_XP_THROW_XP * 0.5f;
                        vec2 BarbellP = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT );
                        
                        AddBarbell( AppState, BarbellC( Player->CarryP, Player->Aim_Dir * BARBELL_THROW_SPEED ) );
                        Player->Carry_ID = PlayerCarryID_None;
                    }
                } break;
            }
        }
        if( ( Player->Dash_IsActive ) && ( Player->ExerciseBall_IsBy.IsValid ) ) {
            EXERCISE_BALL_STATE * ExerciseBallS = &AppState->ExerciseBallS;
            EXERCISE_BALL       * ExerciseBall  = ExerciseBallS->ExerciseBall + Player->ExerciseBall_IsBy.iEntity;
            ExerciseBall->nBounce = 0;
            
            flo32 DirX = ( Player->Velocity.x / fabsf( Player->Velocity.x ) );
            vec2 V = Vec2( 14.0f, 6.0f );
            V.x *= DirX;
            
            ExerciseBall->Velocity.x  = V.x;
            ExerciseBall->Velocity.y += V.y;
            
            Player->Dash_IsActive = false;
            Player->Velocity.x = 0.0f;
            
            AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_ExerciseBall_Bounce01 + RandomU32InRange( 0, 1 ) );
            PlayAudio( AppState, Sound_Select, 0.25f );
        }
        if( ( CanGrab ) && ( Player->Grab_IsDown ) ) {
            Player->Grab_IsActive   = true;
            //Player->Grab_iPushBlock = PushBlockResult.iPushBlock;
            
            PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
            _swap( PUSH_BLOCK, PushBlockS->PushBlock[ 0 ], PushBlockS->PushBlock[ IsByPushBlock.iEntity ] );
            Player->Grab_DirX       = 0.0f;
        }
        if( ( CanGrabExerciseMiniBoss03 ) && ( Player->Grab_IsDown ) ) {
            Player->GrabExerciseMiniBoss03_IsActive = true;
            Player->Grab_DirX                       = 0.0f;
        }
        if( Player->Grab_IsActive ) {
            if( ( !IsOnGround ) || ( WasReleased( Control, ID_Grab ) ) ) {
                PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
                PUSH_BLOCK       * PushBlock  = PushBlockS->PushBlock + 0;
                Player->Velocity.x = PushBlock->Velocity.x;
                PushBlock->DirX    = 0.0f;
                
                Player->Grab_IsActive = false;
            }
        }
        if( Player->GrabExerciseMiniBoss03_IsActive ) {
            if( WasReleased( Control, ID_Grab ) ) {
                Player->GrabExerciseMiniBoss03_IsActive = false;
            }
        }
        
        boo32 Run_IsDown    = IsDown( Control, ID_Run );
        boo32 Charge_IsDown = IsDown( Control, ID_Charge );
        if( Charge_IsDown ) {
            Run_IsDown = false;
        }
        
        Player->Run_IsActive    = false;
        if( ( CanRun ) && ( Run_IsDown ) ) {
            Player->Run_IsActive = true;
        }
        
#if 0
        boo32 CanRunCharge = ( Player->Carry_ID == PlayerCarryID_None )
            && ( !Player->Grab_IsActive )
            && ( !Player->GrabExerciseMiniBoss03_IsActive )
            && ( ( dir.x * Player->Velocity.x ) > 0.0f );
        
        Player->Charge_IsActive = false;
        if( ( CanRunCharge ) && ( Charge_IsDown ) ) {
            Player->Charge_IsActive = true;
            
            flo32 Rate = 1.0f / ( 20.0f / 60.0f );
            Player->Run_Charge = MinValue( Player->Run_Charge + ( Rate * dT ), 1.0f );
            
        } else {
            Player->Run_Charge = 0.0f;
        }
#endif
        
        //if( WasPressed( Control, ID_Throw ) ) {
        //Player->Throw_WasPressed = true;
        //}
        Player->GrenadeThrow_CooldownTimer -= dT;
        
        boo32 Dash_DoDoubleTap = false;
        if( WasPressed( Control, ID_Run ) ) {
            if( Player->Run_DoDoubleTap ) {
                if( Player->Run_DoubleTapTimer <= RUN_DOUBLE_TAP_TARGET_TIME ) {
                    Dash_DoDoubleTap = true;
                    Player->Run_DoDoubleTap = false;
                }
            } else {
                Player->Run_DoDoubleTap = true;
                Player->Run_DoubleTapTimer = 0.0f;
            }
        }
        
        if( Player->Run_DoDoubleTap ) {
            Player->Run_DoubleTapTimer += dT;
            if( Player->Run_DoubleTapTimer > RUN_DOUBLE_TAP_TARGET_TIME ) {
                Player->Run_DoDoubleTap = false;
            }
        }
        
        DISPLAY_VALUE( flo32, Player->Run_DoubleTapTimer );
        DISPLAY_VALUE( boo32, Player->Run_DoDoubleTap );
        
        if( ( CanDash ) && ( Dash_DoDoubleTap ) ) {
            Player->Dash_IsEnabled      = false;
            Player->Dash_IsActive       = true;
            Player->Dash_Timer          = 0.0f;
            Player->Dash_CooldownTimer  = PLAYER_DASH_COOLDOWN_TARGET_TIME;
            Player->Dash_DidTouchGround = false;
            
            Player->Dash_Direction.x = Player->Face_DirX;
            Player->Dash_Direction.y = 0.0f;
            
            Player->Jump_IsActive = false;
            Player->Jump_DoDampen = false;
            
            Player->Stamina  = MaxValue( Player->Stamina - PLAYER_STAMINA_DASH_COST, 0.0f );
            Game->StaminaXP += PLAYER_STAMINA_XP_DASH_XP;
            
            PlayAudio( AppState, AudioID_Sound_Player_Dash, 0.25f );
        }
        
        if( Player->Dash_IsActive ) {
            CanWallSlideOnLeftWall  = false;
            CanWallSlideOnRightWall = false;
            CanWallJumpToLeft       = false;
            CanWallJumpToRight      = false;
        }
        
        if( WasPressed( Control, ID_Jump ) ) {
            AppState->LastJump = Player->Position;
            
            if( CanJump ) {
                flo32 MaxSpeed = PLAYER_JUMP_SPEED;
                
                boo32 DoShortJump = ( Player->Carry_ID == PlayerCarryID_Boulder )
                    || ( Player->Carry_ID == PlayerCarryID_Puncher );
                if( DoShortJump ) {
                    MaxSpeed = PLAYER_CARRY_JUMP_SPEED;
                }
                
                Player->Velocity.y = MaxSpeed;
                
                Player->Jump_Timer    = 0.0f;
                Player->Jump_IsActive = true;
                Player->Jump_DoDampen = false;
                
                CanDropThroughScaffold = false;
                
                Player->Stamina    = MaxValue( Player->Stamina - PLAYER_STAMINA_JUMP_COST, 0.0f );
                Game->StaminaXP += PLAYER_STAMINA_XP_JUMP_XP;
                
                if( ( IsOnSpotter.IsValid ) && ( !IsOnGround ) ) {
                    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
                    SPOTTER       * Spotter  = SpotterS->Spotter + IsOnSpotter.iEntity;
                    Spotter->Velocity.y = -( PLAYER_JUMP_SPEED * 0.8f );
                    
                    StunSpotterY( AppState, IsOnSpotter.iEntity );
                    
                    //AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_Spotter_Bounce01 + RandomU32InRange( 0, 1 ) );
                    //PlayAudio( AppState, Sound_Select, 0.25f );
                } else if( IsOnExerciseBall.IsValid ) {
                    EXERCISE_BALL_STATE * ExerciseBallS = &AppState->ExerciseBallS;
                    EXERCISE_BALL       * ExerciseBall  = ExerciseBallS->ExerciseBall + IsOnExerciseBall.iEntity;
                    ExerciseBall->Velocity.y = -( PLAYER_JUMP_SPEED * 0.8f );
                    ExerciseBall->nBounce    = 0;
                    
                    AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_ExerciseBall_Bounce01 + RandomU32InRange( 0, 1 ) );
                    PlayAudio( AppState, Sound_Select, 0.25f );
                }
                
                Player->Dash_IsActive = false;
            }
            if( CanWallJumpToLeft ) {
                Player->Velocity.x = -PLAYER_WALL_JUMP_SPEED_X;
                Player->Velocity.y =  PLAYER_WALL_JUMP_SPEED_Y;
                
                CanWallSlideOnLeftWall  = false;
                CanWallSlideOnRightWall = false;
                
                Player->Jump_DoDampen = false;
            }
            if( CanWallJumpToRight ) {
                Player->Velocity.x =  PLAYER_WALL_JUMP_SPEED_X;
                Player->Velocity.y =  PLAYER_WALL_JUMP_SPEED_Y;
                
                CanWallSlideOnLeftWall  = false;
                CanWallSlideOnRightWall = false;
                
                Player->Jump_DoDampen = false;
            }
        }
        
        if( ( CanDropThroughScaffold ) && ( WasPressed( Control, ID_Drop ) ) ) {
            Player->Scaffold_SkipCollision = true;
            Player->Scaffold_iScaffold     = ScaffoldDown.iScaffold;
        }
        
        
#if 0        
        boo32 DoPunch = false;
        if( ( WasPressed( Control, ID_Punch  ) ) && ( CanPunch ) ) {
            Player->Punch_IsCharging = true;
            Player->Punch_Charge     = 0.0f;
            Player->Stamina    = MaxValue( Player->Stamina - PLAYER_STAMINA_PUNCH_COST, 0.0f );
            Game->StaminaXP += PLAYER_STAMINA_XP_PUNCH_XP;
        }
        if( WasReleased( Control, ID_Punch ) ) {
            if( Player->Punch_IsCharging ) {
                DoPunch = true;
            }
            
            Player->Punch_IsCharging = false;
        }
#endif
        
        if( WasReleased( Control, ID_Jump ) ) {
            if( Player->Jump_IsActive ) {
                Player->Jump_DoDampen = true;
            }
            Player->Jump_IsActive = false;
        }
        
        if( Player->Jump_IsActive ) {
            Player->Jump_Timer += dT;
            if( Player->Jump_Timer > PLAYER_JUMP_TARGET_TIME ) {
                Player->Jump_IsActive = false;
                Player->Jump_DoDampen = false;
            }
        }
        
        if( ( Player->Jump_DoDampen ) && ( IsFalling ) ) {
            Player->Jump_DoDampen = false;
        }
        if( Player->IsOnConveyorVer ) {
            Player->Jump_DoDampen = false;
        }
        
        boo32 WallSlide_IsActive = ( CanWallSlideOnLeftWall ) || ( CanWallSlideOnRightWall );
        if( WallSlide_IsActive ) {
            Player->WallSlide_ParticleTimer += dT;
            if( Player->WallSlide_ParticleTimer >= PLAYER_WALL_SLIDE_PARTICLE_CYCLE_TARGET_TIME ) {
                Player->WallSlide_ParticleTimer = 0.0f;
                addWallSlide( &AppState->ParticleS, Player->Position, CanWallSlideOnLeftWall );
            }
        } else {
            Player->WallSlide_ParticleTimer = 0.0f;
        }
        
        if( Player->Dash_IsActive ) {
            // TODO: Is this the best way to organize Dash_IsActive? Does this make the code too complicated to change later, if needed?
            Player->Velocity    = Player->Dash_Direction * PLAYER_DASH_SPEED;
            Player->dPos        = Player->Velocity * dT;
            Player->Dash_Timer += dT;
        } else if( Player->Grab_IsActive ) {
            PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
            PUSH_BLOCK       * PushBlock  = PushBlockS->PushBlock + 0;
            PushBlock->DirX    = dir.x;
            Player->Grab_DirX  = dir.x;
            Player->Velocity.x = PushBlock->Velocity.x;
        } else if( Player->GrabExerciseMiniBoss03_IsActive ) {
            if( dir.x > 0.0f ) {
                EXERCISE_MINIBOSS03 * Boss = &AppState->ExerciseMiniBoss03;
                Player->Grab_DirX  = 1.0f;
                Player->Velocity.x = Boss->Velocity.x;
                
                Boss->Push_DirX = 1.0f;
            }
        } else {
            flo32 MaxSpeed = PLAYER_WALK_SPEED;
            flo32 Friction = PLAYER_WALK_FRICTION;
            
            if( !IsOnGround ) {
                Friction = PLAYER_AIR_FRICTION;
            }
            
            boo32 DoCarryWalkSpeed = ( Player->Carry_ID == PlayerCarryID_Boulder )
                || ( Player->Carry_ID == PlayerCarryID_Puncher );
            if( DoCarryWalkSpeed ) {
                MaxSpeed = PLAYER_CARRY_SPEED;
                Friction = PLAYER_CARRY_FRICTION;
            }
            if( Player->Run_IsActive ) {
                MaxSpeed = PLAYER_RUN_SPEED;
                Friction = PLAYER_RUN_FRICTION;
            }
            
            if( Player->DoBossBlowback ) {
                Friction = 3.0f;
                
                Player->BossBlowback_Timer += dT;
                if( Player->BossBlowback_Timer >= ( 36.0f / 60.0f ) ) {
                    Player->DoBossBlowback = false;
                }
            }
            vec2 Accel = {};
            Accel.x = ( dir.x * MaxSpeed - Player->Velocity.x ) * Friction;
            
            if( ConveyorHor.IsOnConveyor ) {
                Accel.x += ( ConveyorHor.ConveyorAccel.x * CONVEYOR_HORIZONTAL_FRICTION );
            }
            
            if( ( WallSlide_IsActive ) && ( IsFalling ) ) {
                Accel.y = ( -1.0f * PLAYER_WALL_SLIDE_SPEED - Player->Velocity.y ) * PLAYER_WALL_SLIDE_FRICTION;
            } else {
                Accel.y = -PLAYER_GRAVITY;
                if( Player->Jump_DoDampen ) {
                    Accel.y *= 4.0f;
                }
            }
            
            if( Player->IsOnConveyorVer ) {
                Accel.y += ( Player->ConveyorAccel.y - Player->Velocity.y ) * CONVEYOR_VERTICAL_FRICTION;
            }
            
            Player->dPos      = Accel * ( dT * dT * 0.5f ) + Player->Velocity * dT;
            Player->Velocity += Accel * dT;
        }
        
        Player->Draw_ID = DrawID_Default;
        
        if( !Player->Fire_IsActive ) {
            if( IsMovingLeft ) {
                Player->Draw_FaceLeft = true;
            }
            if( IsMovingRight ) {
                Player->Draw_FaceLeft = false;
            }
        }
        
        if( ( CanWallSlideOnLeftWall  ) || ( Player->IsOnConveyorVerLeft  ) ) {
            Player->Draw_ID       = DrawID_WallSlide;
            Player->Draw_FaceLeft = true;
        }
        if( ( CanWallSlideOnRightWall ) || ( Player->IsOnConveyorVerRight ) ) {
            Player->Draw_ID       = DrawID_WallSlide;
            Player->Draw_FaceLeft = false;
        }
        
        // STAMINA
        if( Game->Stamina_IsFeatured ) {
            if( ( !Player->Stamina_IsEnabled ) && ( WasPressed( Control, ID_StaminaCharge ) ) ) {
                Player->Stamina += 0.25f;
            }
            
            if( Player->Run_IsActive ) {
                Player->Stamina   -= ( PLAYER_STAMINA_RUN_RATE    * dT );
                Game->StaminaXP += ( PLAYER_STAMINA_XP_RUN_RATE * dT );
                Player->Stamina_CooldownTimer = 0.0f;
            }
            if( Player->Jump_IsActive ) {
                Player->Stamina   -= ( PLAYER_STAMINA_JUMP_RATE    * dT );
                Game->StaminaXP += ( PLAYER_STAMINA_XP_JUMP_RATE * dT );
                Player->Stamina_CooldownTimer = 0.0f;
            }
            if( 
               ( Player->Carry_ID == PlayerCarryID_Boulder )
               || ( Player->Carry_ID == PlayerCarryID_Puncher ) ) {
                Player->Stamina -= ( PLAYER_STAMINA_CARRY_RATE    * dT );
                Game->StaminaXP += ( PLAYER_STAMINA_XP_CARRY_RATE * dT );
                Player->Stamina_CooldownTimer = 0.0f;
            }
            if( ( Player->Carry_ID == PlayerCarryID_Popper )
               || ( Player->Carry_ID == PlayerCarryID_Camper )
               || ( Player->Carry_ID == PlayerCarryID_Barbell ) ) {
                Player->Stamina -= ( PLAYER_STAMINA_CARRY_RATE    * 0.5f * dT );
                Game->StaminaXP += ( PLAYER_STAMINA_XP_CARRY_RATE * 0.5f * dT );
                Player->Stamina_CooldownTimer = 0.0f;
            }
            if( Player->Punch_IsCharging ) {
                Player->Punch_Charge += ( dT / PLAYER_PUNCH_CHARGE_TARGET_TIME );
                Player->Punch_Charge  = MinValue( Player->Punch_Charge, 1.0f );
                if( Player->Punch_Charge < 1.0f ) {
                    Player->Stamina      -= ( PLAYER_STAMINA_PUNCH_RATE * dT );
                    Game->StaminaXP    += ( PLAYER_STAMINA_XP_PUNCH_RATE * dT );
                }
                Player->Stamina_CooldownTimer = 0.0f;
            }
            if( ( Player->Grab_IsActive ) && ( Player->Grab_DirX != 0.0f ) ) {
                Player->Stamina -= ( PLAYER_STAMINA_GRAB_RATE    * dT );
                Game->StaminaXP += ( PLAYER_STAMINA_XP_GRAB_RATE * dT );
                Player->Stamina_CooldownTimer = 0.0f;
            }
            if( ( Player->GrabExerciseMiniBoss03_IsActive ) && ( Player->Grab_DirX != 0.0f ) ) {
                Player->Stamina -= ( PLAYER_STAMINA_GRAB_RATE    * dT );
                Game->StaminaXP += ( PLAYER_STAMINA_XP_GRAB_RATE * dT );
                Player->Stamina_CooldownTimer = 0.0f;
            }
            if( Player->Stamina_IsBeingDrained ) {
                Player->Stamina -= ( SPOTTER_STAMINA_DRAIN_RATE * dT );
                Player->Stamina_CooldownTimer = 0.0f;
            }
            Player->Stamina = MaxValue( Player->Stamina, 0.0f );
            
            if( Player->Stamina == 0.0f ) {
                Player->Stamina_IsEnabled = false;
                Game->StaminaXP += PLAYER_STAMINA_XP_NO_STAMINA_XP_BOOST;
                
                Player->Run_IsActive    = false;
                if( Player->Jump_IsActive ) {
                    Player->Jump_DoDampen = true;
                }
                Player->Jump_IsActive = false;
                
                switch( Player->Carry_ID ) {
                    case PlayerCarryID_Camper: {
                        vec2 DropP = Player->CarryP + Vec2( -Player->Face_DirX * TILE_WIDTH * 0.15f, -TILE_HEIGHT * 0.8f );
                        Player->Camper_IsCarrying.Position = DropP;
                        AddCamper( AppState, Player->Camper_IsCarrying );
                        
                        PlayAudio( AppState, AudioID_Sound_Camper_Drop, 0.5f );
                    } break;
                    
                    case PlayerCarryID_Boulder: {
                        AddBoulder( AppState, Player->CarryP );
                    } break;
                    
                    case PlayerCarryID_ExerciseBall: {
                        AddExerciseBall( AppState, Player->CarryP );
                    } break;
                    
                    case PlayerCarryID_Puncher: {
                        AddPuncher( AppState, PuncherStunnedC( Player->CarryP, Vec2( 0.0f, 0.0f ), Player->Carry_PuncherHealth ) );
                    } break;
                    
                    case PlayerCarryID_Popper: {
                        AddPopper( AppState, PopperThrownC( Player->CarryP, Vec2( 0.0f, 0.0f ) ) );
                    } break;
                    
                    case PlayerCarryID_Barbell: {
                        AddBarbell( AppState, BarbellC( Player->CarryP, Vec2( 0.0f, 0.0f ) ) );
                    } break;
                }
                Player->Carry_ID = PlayerCarryID_None;
                
#if 0                
                if( Player->Punch_IsCharging ) {
                    Player->Punch_IsCharging = false;
                    DoPunch = true;
                }
#endif
                
                if( Player->Grab_IsActive ) {
                    PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
                    PUSH_BLOCK       * PushBlock  = PushBlockS->PushBlock + 0;
                    Player->Velocity.x = PushBlock->Velocity.x;
                    PushBlock->DirX    = 0.0f;
                }
                Player->Grab_IsActive = false;
                Player->Grab_DirX     = 0.0f;
                Player->GrabExerciseMiniBoss03_IsActive = false;
            }
            
            
#if 0            
            if( DoPunch ) {
                // TODO: Check Charge
                vec2 Offset = PLAYER_PUNCH_ART_OFFSET;
                if( Player->Draw_FaceLeft ) {
                    Offset.x = -Offset.x;
                }
                
                vec2 PunchP = Player->Position + Offset;
                Player->Punch_DoDraw    = true;
                Player->Punch_ArtTimer  = 0.0f;
                Player->Punch_ArtCharge = Player->Punch_Charge;
                Player->Punch_ArtPos    = PunchP;
                Player->Punch_FaceLeft  = Player->Draw_FaceLeft;
                
                Player->Punch_DoPunch   = true;
                Player->Punch_Bound     = GetPlayerPunchBound( Player->Position, Player->Punch_Charge, Player->Punch_FaceLeft );
                
                { // Damage Entities
                    { // STOMPER
                        STOMPER_STATE * StomperS = &AppState->StomperS;
                        
                        vec2 Vel = PLAYER_PUNCH_STOMPER_SPEED_LO;
                        if( Player->Punch_Charge >= 1.0f ) {
                            Vel = PLAYER_PUNCH_STOMPER_SPEED_HI;
                        }
                        if( Player->Punch_FaceLeft ) {
                            Vel.x = -Vel.x;
                        }
                        
                        rect StomperR = GetPlayerBoundToPunchStomper( Player->Position, Player->Punch_Charge, Player->Punch_FaceLeft );
                        for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
                            STOMPER * Stomper = StomperS->Stomper + iStomper;
                            if( IsInBound( Stomper->Position, StomperR ) ) {
                                Stomper->Mode     = StomperMode_Idle;
                                Stomper->Timer    = 0.0f;
                                Stomper->Velocity = Vel;
                            }
                        }
                    }
                }
                
                Player->Punch_Charge = 0.0f;
            }
#endif
            
            { // Save Camper
                // TODO: Drop camper into safe zone??
                
                boo32 DoSaveCamper = ( Player->Carry_ID == PlayerCarryID_Camper )
                    && ( IsOnGround )
                    && ( !IsInBound( Player->Position, Player->Camper_IsCarrying.HazardBound ) );
                
                Player->DoSaveCamper = DoSaveCamper;
            }
        }
    }
    
    DISPLAY_VALUE( vec2,  Player->Position );
    DISPLAY_VALUE( vec2,  Player->dPos     );
    DISPLAY_VALUE( vec2,  Player->Velocity );
}

internal void
DamagePlayer( APP_STATE * AppState, uint32 Damage ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    uint32 dmg = MinValue( Player->Health, Damage );
    Player->Health -= dmg;
    
    flo32 maxTime = PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME * 3.0f;
    maxTime = PLAYER_NO_DAMAGE_TARGET_TIME - ( 4.0f / 60.0f );
    Player->ShowDamage_Timer = maxTime - 0.0001f;
    
    Player->NoDamage       = true;
    Player->NoDamage_Timer = 0.0f;
    
    boo32 IsFacingLeft = Player->Draw_FaceLeft;
    vec2  BoostVel     = Vec2( ( IsFacingLeft ? 1.0f : -1.0f ), 1.0f ) * PLAYER_DAMAGE_BOOST_SPEED;
    
    Player->Velocity = BoostVel;
    
    Player->Punch_IsCharging = false;
    
    Player->Dash_IsActive = false;
    
    switch( Player->Carry_ID ) {
        case PlayerCarryID_Camper: {
            vec2 DropP = Player->CarryP + Vec2( -Player->Face_DirX * TILE_WIDTH * 0.15f, -TILE_HEIGHT * 0.8f );
            Player->Camper_IsCarrying.Position = DropP;
            AddCamper( AppState, Player->Camper_IsCarrying );
            
            PlayAudio( AppState, AudioID_Sound_Camper_Drop, 0.5f );
        } break;
        
        case PlayerCarryID_Boulder: {
            AddBoulder( AppState, Player->CarryP );
        } break;
        
        case PlayerCarryID_ExerciseBall: {
            AddExerciseBall( AppState, Player->CarryP );
        } break;
        
        case PlayerCarryID_Puncher: {
            AddPuncher( AppState, PuncherStunnedC( Player->CarryP, Vec2( 0.0f, 0.0f ), Player->Carry_PuncherHealth ) );
        } break;
        
        case PlayerCarryID_Popper: {
            AddPopper( AppState, PopperThrownC( Player->CarryP, Vec2( 0.0f, 0.0f ) ) );
        } break;
        
        case PlayerCarryID_Barbell: {
            AddBarbell( AppState, BarbellC( Player->CarryP, Vec2( 0.0f, 0.0f ) ) );
        } break;
    }
    Player->Carry_ID = PlayerCarryID_None;
    
    // TODO: Drop some items if carried
    // TODO: Stop grab
    //if( Player->CarryBoulder_IsActive ) {
    //vec2 BoulderP = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT + BOULDER_RADIUS );
    //AddBoulder( AppState, BoulderP );
    //}
    //Player->CarryBoulder_IsActive   = false;
}

internal void
KillPlayer( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    Player->IsDead       = true;
    Player->IsDead_Timer = 0.0f;
    
    Player->Jump_IsActive           = false;
    Player->Jump_DoDampen           = false;
    Player->Fire_IsActive           = false;
    Player->Fire_doHitScan          = false;
    Player->Use_IsActive            = false;
    
    { // SPOTTER
        SPOTTER_STATE * SpotterS = &AppState->SpotterS;
        for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
            SPOTTER * Spotter = SpotterS->Spotter + iSpotter;
            
            if( ( Spotter->Mode == SpotterMode_IsChasing ) || ( Spotter->Mode == SpotterMode_IsDraining ) ) {
                Spotter->Mode = SpotterMode_Idle;
            }
        }
    }
}

internal void
UpdatePlayer( APP_STATE * AppState, flo32 dT ) {
    GAME_STATE   * Game   = &AppState->Game;
    PLAYER_STATE * Player = &AppState->Player;
    
    if( Player->IsDead ) {
        Player->IsDead_Timer += dT;
    } else {
        if( Player->ShowDamage_Timer > 0.0f ) {
            Player->ShowDamage_Timer = MaxValue( Player->ShowDamage_Timer - dT, 0.0f );
        }
        
        //if( Player->Fire_IsActive ) {
        //Player->Fire_Timer += dT;
        //if( Player->Fire_Timer >= PLAYER_BULLET_TARGET_TIME ) {
        //Player->Fire_Timer = 0.0f;
        //RandomlyAdjustFireDirection( AppState );
        //}
        //}
        
        if( Player->NoDamage ) {
            Player->NoDamage_Timer += dT;
        }
        
        Player->Dash_CooldownTimer = MaxValue( Player->Dash_CooldownTimer - dT, 0.0f );
        
        if( Game->Stamina_IsFeatured ) { // Update Stamina
            UpdateStaminaXP( Game, Player );
            
            Player->Stamina_CooldownTimer += dT;
            
            flo32 Rate = 0.0f;
            if( Player->Stamina_IsEnabled ) {
                boo32 DoCharge = ( Player->Stamina_CooldownTimer >= PLAYER_STAMINA_COOLDOWN_TARGET_TIME )
                    && ( !Player->Run_IsActive )
                    && ( !Player->Dash_IsActive )
                    && ( ( Player->Carry_ID == PlayerCarryID_None )
                        || ( Player->Carry_ID == PlayerCarryID_ExerciseBall ) )
                    && ( !Player->Punch_IsCharging )
                    && ( !Player->Jump_IsActive )
                    && ( !Player->Dash_IsActive )
                    && ( !( ( Player->Grab_IsActive ) && ( Player->Grab_DirX != 0.0f ) ) )
                    && ( !( ( Player->GrabExerciseMiniBoss03_IsActive ) && ( Player->Grab_DirX != 0.0f ) ) )
                    && ( !( Player->Stamina_IsBeingDrained ) );
                
                if( DoCharge ) {
                    Rate = Game->MaxStamina / PLAYER_STAMINA_CHARGE_FAST_TARGET_TIME;
                }
            } else {
                Rate = Game->MaxStamina / PLAYER_STAMINA_CHARGE_SLOW_TARGET_TIME;
                
                if( Game->StaminaXP_IsFeatured ) {
                    Game->StaminaXP += ( 1.0f * dT );
                }
            }
            
            Player->Stamina += ( Rate * dT );
            Player->Stamina  = MinValue( Player->Stamina, Game->MaxStamina );
            
            if( Player->Stamina == Game->MaxStamina ) {
                Player->Stamina_IsEnabled = true;
            }
            
            if( Player->Punch_DoDraw ) {
                flo32 StepX = ( TILE_WIDTH * 0.2f );
                if( Player->Punch_ArtCharge >= 1.0f ) {
                    StepX = ( TILE_WIDTH * 0.5f );
                }
                if( Player->Punch_FaceLeft ) {
                    StepX = -StepX;
                }
                
                Player->Punch_ArtTimer += dT;
                Player->Punch_ArtPos.x += StepX;
            }
        }
    }
    
    vec2 Player_Dim = Vec2( PLAYER_WIDTH, PLAYER_HEIGHT );
    
    rect CollBound = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM );
    
    boo32 DoRunCharge   = false;
    vec2  DoRunCharge_N = {};
    
    COLLISION_RESULT Coll = CollisionResult( Player->Position, Player->Velocity, Player->dPos );
    for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < COLLISION_MAX_ITERATION ); Iter++ ) {
        RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
        
        DoesPlayerIntersectLevelBound( &BestIntersect, AppState, Coll );
        DoesIntersectCollisionAll( &BestIntersect, AppState, Coll, CollBound );
        
        if( !Player->Scaffold_SkipCollision ) {
            DoesIntersectScaffold ( &BestIntersect, AppState, Coll, CollBound );
        }
        
        DoesIntersectSeesaw    ( &BestIntersect, AppState, Coll );
        DoesIntersectPushBlock ( &BestIntersect, AppState, Coll, CollBound );
        DoesIntersectBreakBlock( &BestIntersect, AppState, Coll, CollBound );
        
        DoesIntersectCollapsePlatform( &BestIntersect, AppState, Coll, CollBound );
        
        DoesIntersectExerciseMiniBoss03( &BestIntersect, AppState, Coll, CollBound );
        
        Coll = FinalizeCollision( BestIntersect, Coll );
        
        // TODO: Fix this! This should probably only check against break blocks and not against other entities, esp. entities that the player can push.
        if( ( Coll.HadCollision ) && ( BestIntersect.N.y == 0.0f ) && ( Player->Dash_IsActive ) ) {
            DoRunCharge   = true;
            DoRunCharge_N = BestIntersect.N;
            
            PlayAudio( AppState, AudioID_Sound_Player_Bash, 0.5f );
        }
    }
    Coll.Pos += Coll.dPos;
    
    Player->Position = Coll.Pos;
    Player->Velocity = Coll.Vel;
    
    Player->Velocity.y = MaxValue( Player->Velocity.y, PLAYER_FALL_MAX_SPEED );
    
    if( DoRunCharge ) {
        BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
        
        boo32 HadCollision = false;
        
        uint32 NewCount = 0;
        
        flo32 MarginX = TILE_WIDTH * 0.1f;
        rect  RunChargeBound = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM + Vec2( MarginX * 2.0f, 0.0f ) );
        RunChargeBound.Bottom += ( TILE_HEIGHT * 0.25f );
        RunChargeBound.Top    -= ( TILE_HEIGHT * 0.1f  );
        for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
            BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
            
            rect Bound = MinkSub( RectCD( BreakBlock.Position, BreakBlock.Dim ), RunChargeBound );
            
            // TODO: Tune Bound and Selection
            if( !IsInBound( Player->Position, Bound ) ) {
                BreakBlockS->BreakBlock[ NewCount++ ] = BreakBlock;
            } else {
                HadCollision = true;
            }
        }
        BreakBlockS->nBreakBlock = NewCount;
        
        if( HadCollision ) {
            Player->Dash_IsActive = false;
            Player->Velocity.x = DoRunCharge_N.x * 16.0f;
            Player->Velocity.y = 8.0f;
            PlayAudio( AppState, AudioID_Sound_BreakBlock, 0.5f );
        }
    }
    
    if( ( Player->Dash_IsActive ) && ( Coll.HadCollision ) ) {
        Player->Dash_IsActive = false;
        Player->Velocity.x = -Player->Dash_Direction.x * 14.0f;
        Player->Velocity.y = 6.0f;
    }
    
    boo32 PlayerIsOnSpikes = IsPlayerOnSpikes( AppState );
    if( ( PlayerIsOnSpikes ) && ( !Player->NoDamage ) ) {
        Player->Dash_IsActive = false;
        DamagePlayer( AppState, 1 );
    }
    
    boo32 PlayerIsTouchingRunner = IsRunnerTouchingPlayer( AppState );
    if( ( PlayerIsTouchingRunner ) && ( !Player->NoDamage ) ) {
        Player->Dash_IsActive = false;
        DamagePlayer( AppState, 1 );
    }
    
    boo32 PlayerIsTouchingStomper = IsStomperTouchingPlayer( AppState );
    if( ( PlayerIsTouchingStomper ) && ( !Player->NoDamage ) ) {
        DamagePlayer( AppState, 1 );
    }
    
    rect CycleBlockBound = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM - Vec2( TILE_WIDTH * 0.2f, TILE_HEIGHT * 0.2f ) );
    boo32 IsTouchingCycleBlock = IsPosInCycleBlock( AppState, Player->Position, CycleBlockBound );
    if( ( IsTouchingCycleBlock ) && ( !Player->NoDamage ) ) {
        DamagePlayer( AppState, 1 );
    }
    
    rect PlayerBound = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM );
    boo32 IsTouchingHedgehog = IsPosInHedgehog( AppState, Player->Position, PlayerBound );
    if( ( IsTouchingHedgehog ) && ( !Player->NoDamage ) ) {
        DamagePlayer( AppState, 1 );
    }
    
    if( Player->Dash_IsActive ) {
        vec2 Dim   = PLAYER_DIM;
        rect Bound = RectBCD( Vec2( 0.0f, 0.0f ), Dim );
        flo32 DirX = Player->Velocity.x / fabsf( Player->Velocity.x );
        
        { // PUNCHER
            ENTITY_VALID_RESULT IsInStunBound = IsInPuncherStunBound( AppState, Player->Position, Bound );
            if( IsInStunBound.IsValid ) {
                StunPuncher( AppState, IsInStunBound.iEntity, DirX );
                
                Player->Dash_IsActive = false;
                Player->Velocity.x = -DirX * 16.0f;
                Player->Velocity.y = 8.0f;
                
                Player->Dash_DidTouchGround = true;
                Player->Dash_CooldownTimer  = PLAYER_DASH_COOLDOWN_TARGET_TIME;
            }
        }
        
        { // POPPER
            ENTITY_VALID_RESULT IsInStunBound = IsInPopperStunBound( AppState, Player->Position, Bound );
            if( IsInStunBound.IsValid ) {
                StunPopper( AppState, IsInStunBound.iEntity, DirX );
            }
        }
        
        { // SPOTTER
            ENTITY_VALID_RESULT IsInStunBound = IsInSpotterStunBound( AppState, Player->Position, Bound );
            if( IsInStunBound.IsValid ) {
                // TODO: I probably want to change this to include DirY.
                StunSpotterX( AppState, IsInStunBound.iEntity, DirX );
                
                Player->Dash_IsActive = false;
                Player->Velocity.x = -DirX * 16.0f;
                Player->Velocity.y = 8.0f;
                
                Player->Dash_DidTouchGround = true;
                Player->Dash_CooldownTimer  = PLAYER_DASH_COOLDOWN_TARGET_TIME;
            }
        }
    }
    
    if( Player->Carry_ID != PlayerCarryID_None ) {
        boo32 DoRectCollision = false;
        flo32 Radius          = 0.0f;
        rect  Bound           = {};
        vec2  CarryP = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT );
        vec2  Offset = {};
        
        switch( Player->Carry_ID ) {
            case PlayerCarryID_ExerciseBall:
            case PlayerCarryID_Boulder: {
                DoRectCollision = false;
                
                if( Player->Carry_ID == PlayerCarryID_ExerciseBall ) {
                    Radius = EXERCISE_BALL_RADIUS;
                }
                if( Player->Carry_ID == PlayerCarryID_Boulder ) {
                    Radius = BOULDER_RADIUS;
                }
                Offset = Vec2( 0.0f, Radius );
            } break;
            
            case PlayerCarryID_Camper:
            case PlayerCarryID_Popper:
            case PlayerCarryID_Puncher: {
                DoRectCollision = true;
                
                if( Player->Carry_ID == PlayerCarryID_Camper ) {
                    Bound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( CAMPER_WIDTH, TILE_HEIGHT * 0.2f ) );
                }
                if( Player->Carry_ID == PlayerCarryID_Puncher ) {
                    Bound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( PUNCHER_WIDTH, PUNCHER_WIDTH ) );
                }
                if( Player->Carry_ID == PlayerCarryID_Popper ) {
                    Bound = RectBCD( Vec2( 0.0f, 0.0f ), POPPER_DIM );
                }
            } break;
        }
        
        PENETRATE2D BestIn = {};
        BestIn.Dist = FLT_MAX;
        
        if( DoRectCollision ) {
            DoesPenetrateCollisionAll( &BestIn, AppState, CarryP, Bound  );
        } else {
            DoesPenetrateCollisionAll( &BestIn, AppState, CarryP, Vec2( 0.0f, -Radius ), Radius );
        }
        
        if( BestIn.IsValid ) {
            CarryP = BestIn.P;
        }
        Player->CarryP = CarryP + Offset;
    }
}

internal void
DrawPlayer( RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, APP_STATE * AppState, DRAW_STATE * Draw ) {
    GAME_STATE   * Game   = Draw->Game;
    PLAYER_STATE * Player = Draw->Player;
    
    if( Player->IsDead ) {
        flo32 t = Clamp01( Player->IsDead_Timer / PLAYER_DEAD_TARGET_TIME );
        
        vec2 P = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT * 0.5f );
        vec2 Q = Player->Position + Vec2( 0.0f, PLAYER_WIDTH * 0.5f );
        
        flo32 radians = lerp( 0.0f, t, PI * 0.5f );
        vec2  pos     = lerp( P, t, Q );
        
        vec2  Dim = Vec2( PLAYER_WIDTH, PLAYER_HEIGHT );
        orect R   = ORectCD( pos, Dim, radians );
        
        vec4  Color = ToColor( 0, 150, 0 );
        
        DrawORect( Pass_Game, R, Color );
        
        // TODO: Add Boulder if carrying!!
    } else {
        flo32 a = 1.0f;
        { // Draw Player
            if( Player->Carry_ID == PlayerCarryID_Camper ) {
                vec2 CarryP = Player->CarryP;
                CarryP.y -= TILE_HEIGHT * 0.8f;
                CarryP.x -= ( Player->Face_DirX * TILE_WIDTH * 0.15f );
                Player->Camper_IsCarrying.Position = CarryP;
                DrawCamper( Pass_Game, Player->Camper_IsCarrying );
            }
            
            vec4 Color = ToColor( 0, 150, 0 );
            
            if( Player->Dash_IsActive ) {
                Color = ToColor( 150, 0, 0 );
            } else if( !Player->Dash_IsEnabled ) {
                Color = ToColor( 0, 75, 0 );
            }
            if( !Player->Stamina_IsEnabled ) {
                Color = ToColor( 20, 100, 20 );
            }
            
            { // Show Damage
                if( Player->ShowDamage_Timer > 0.0f ) {
                    flo32 HalfTime = PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME * 0.5f;
                    uint32 cycle = ( uint32 )( Player->ShowDamage_Timer / HalfTime );
                    if( ( cycle % 2 ) == 1 ) {
                        a = 0.15f;
                    }
                }
            }
            
            Color *= a;
            
            switch( Player->Draw_ID ) {
                case DrawID_WallSlide: {
                    vec2 pos   = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT * 0.5f );
                    vec2 scale = Vec2( PLAYER_WIDTH, PLAYER_HEIGHT ) * 0.5f;
                    if( Player->Draw_FaceLeft ) {
                        scale.x = -scale.x;
                    }
                    
                    DrawModel( Pass_Game, ModelID_Player_WallSlide, TextureID_WhiteTexture, pos, scale, Color );
                } break;
                
                case DrawID_Default: {
                    vec2 Scale = Vec2( 1.0f, 1.0f );
                    if( !Player->Stamina_IsEnabled ) {
                        flo32 Radians = Clamp01( Player->Stamina / Game->MaxStamina ) * PI * 8.0f;
                        flo32 t       = ( sinf( -Radians ) * 0.5f + 0.5f );
                        Scale.x = lerp( 1.15f, t, 1.05f );
                        Scale.y = lerp( 0.87f, t, 0.97f );
                    }
                    rect R = RectBCD( Player->Position, PLAYER_DIM * Scale );
                    DrawRect( Pass_Game, R, Color );
                } break;
            }
            
            // TODO: Clean up this code, so only one indicator is drawn at any given time. Currently, Save Indicator is overdrawing the Use Indicator.
#if 0
            { // Draw use
                if( ( Player->Use_IsActive ) && ( Player->Use_t > 0.0f ) ) {
                    vec2 P   = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT + ( TILE_HEIGHT * 0.25f ) );
                    vec2 Dim = Vec2( PLAYER_WIDTH * 0.9f, TILE_HEIGHT * 0.75f );
                    
                    rect R = RectBCD( P, Dim );
                    DrawRect       ( Pass_Game, R, COLOR_GRAY( 0.05f  ) );
                    DrawRectOutline( Pass_Game, R, COLOR_GRAY( 0.15f  ) );
                    
                    rect S = AddRadius( R, -TILE_WIDTH * 0.125f );
                    S.Right = lerp( S.Left, Player->Use_t, S.Right );
                    DrawRect( Pass_Game, S, COLOR_YELLOW );
                }
                
                vec2 PlayerP = GetPlayerUseP( *Player );
                DrawPoint( Pass_Game, PlayerP, TILE_DIM * 0.5f, COLOR_BLUE );
            }
#endif
            
            if( Player->Carry_ID != PlayerCarryID_None ) {
                flo32 Gravity = 0.0f;
                flo32 Speed   = 0.0f;
                
                vec2 CarryP = Player->CarryP;
                switch( Player->Carry_ID ) {
                    case PlayerCarryID_Boulder: {
                        DrawBoulder( Pass_Game, CarryP );
                        
                        Gravity = -BOULDER_GRAVITY;
                        Speed   =  BOULDER_THROW_SPEED;
                    } break;
                    
                    case PlayerCarryID_ExerciseBall: {
                        DrawExerciseBall( Pass_Game, ExerciseBallC( CarryP ) );
                        
                        Gravity = -EXERCISE_BALL_GRAVITY;
                        Speed   =  EXERCISE_BALL_THROW_SPEED;
                    } break;
                    
                    case PlayerCarryID_Puncher: {
                        PUNCHER Puncher = {};
                        Puncher.Position        = CarryP;
                        Puncher.IsStunned       = true;
                        Puncher.IsStunned_Timer = ( 18.0f / 60.0f );
                        Puncher.Health          = 1;
                        DrawPuncher( Pass_Game, Puncher );
                        
                        Gravity = -PUNCHER_GRAVITY;
                        Speed   =  PUNCHER_THROW_SPEED;
                    } break;
                    
                    case PlayerCarryID_Popper: {
                        POPPER Popper = {};
                        Popper.Position = CarryP;
                        Popper.Mode     = PopperMode_IsThrown;
                        Popper.Charge   = 1.0f;
                        DrawPopper( Pass_Game, Popper );
                        
                        Gravity = -POPPER_GRAVITY;
                        Speed   =  POPPER_THROW_SPEED;
                    } break;
                    
                    case PlayerCarryID_Barbell: {
                        BARBELL Barbell = {};
                        Barbell.Position = CarryP;
                        DrawBarbell( Pass_Game, Barbell );
                        
                        Gravity = -BARBELL_GRAVITY;
                        Speed   =  BARBELL_THROW_SPEED;
                    } break;
                }
                
                flo32 dT = ( 1.0f / 60.0f );
                
                uint32 nArcP = 48;
                vec2 Accel = Vec2( 0.0f, Gravity );
                vec2 AtVel = Player->Aim_Dir * Speed;
                vec2 AtPos = CarryP;
                
                for( uint32 iArcP = 0; iArcP < nArcP; iArcP++ ) {
                    vec2 dPos  = Accel * ( dT * dT * 0.5f ) + AtVel * dT;
                    vec2 ToPos = AtPos + dPos;
                    
                    DrawLine( Pass_Game, AtPos, ToPos, COLOR_RED );
                    
                    AtPos  = ToPos;
                    AtVel += Accel * dT;
                }
            }
            
            if( Player->Punch_IsCharging ) {
                vec2 Offset = PLAYER_PUNCH_ART_OFFSET;
                if( !Player->Draw_FaceLeft ) {
                    Offset.x = -Offset.x;
                }
                
                flo32 Radians       = RandomF32() * 2.0f * PI;
                flo32 t             = Player->Punch_Charge * PLAYER_PUNCH_ART_VIBRATE_OFFSET;
                vec2  VibrateOffset = ToDirection2D( Radians ) * t;
                
                vec2 PunchP = Player->Position + Offset + VibrateOffset;
                rect PunchR = RectCD( PunchP, TILE_DIM * 0.5f );
                
                if( Player->Punch_Charge >= 1.0f ) {
                    rect ChargeR = AddRadius( PunchR, TILE_WIDTH * 0.1f );
                    DrawRect( Pass_Game, ChargeR, ToColor( 200, 225, 200 ) );
                }
                
                DrawRect( Pass_Game, PunchR, ToColor( 100, 200, 100 ) );
            }
            if( Player->Punch_DoDraw ) {
                vec2 PunchP = Player->Punch_ArtPos;
                rect PunchR = RectCD( PunchP, TILE_DIM * 0.4f );
                
                vec2 Dim = PLAYER_PUNCH_COLLISION_DIM_LO;
                Dim.x    = TILE_WIDTH * 0.5f;
                
                if( Player->Punch_ArtCharge >= 1.0f ) {
                    Dim   = PLAYER_PUNCH_COLLISION_DIM_HI;
                    Dim.x = TILE_WIDTH * 1.0f;
                }
                
                rect S = {};
                if( Player->Punch_FaceLeft ) {
                    S = RectRCD( GetCenter( PunchR ), Dim );
                } else {
                    S = RectLCD( GetCenter( PunchR ), Dim );
                }
                
                DrawRect( Pass_Game, S, COLOR_WHITE );
                DrawRect( Pass_Game, PunchR, ToColor( 150, 220, 150 ) );
            }
            
            if( ( Player->Grab_IsActive ) || ( Player->GrabExerciseMiniBoss03_IsActive ) ) {
                vec2 Offset = TILE_DIM * Vec2( 0.5f * Player->Grab_ArtDirX, 0.8f );
                
                vec2 GrabP = Player->Position + Offset;
                rect GrabR = RectCD( GrabP, TILE_DIM * 0.5f );
                
                DrawRect( Pass_Game, GrabR, ToColor( 100, 200, 100 ) );
            } else if( ( Player->CanGrab ) || ( Player->CanPickUp ) ) {
                flo32 DirX = 0.0f;
                if( Player->CanGrab ) {
                    DirX = Player->Grab_ArtDirX;
                }
                if( Player->CanPickUp ) {
                    DirX = Player->PickUp_DirX;
                }
                vec2 Offset = TILE_DIM * Vec2( 0.35f * DirX, 0.6f );
                
                vec2 GrabP = Player->Position + Offset;
                rect GrabR = RectCD( GrabP, TILE_DIM * Vec2( 0.3f, 0.4f ) );
                
                DrawRect( Pass_Game, GrabR, ToColor( 100, 200, 100 ) );
            }
            
        }
        
#if 0
        // Aim_Dir
        vec2 Dim   = Vec2( 0.95f, 0.1f );
        vec4 Color = ToColor( 87, 63, 7 );
        
        Color *= a;
        
        if( Player->Fire_IsActive ) {
            vec2 P = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT * 0.75f );
            vec2 V = Player->Aim_Dir;
            orect R = orectXD( P - V * 0.1f, V, Dim );
            DrawORect( Pass_Game, R, Color );
        } else {
            flo32 radians = -PI * 1.0f / 8.0f;
            
            vec2  P = Player->Position + Vec2( 0.0f, PLAYER_HEIGHT * 0.6f );
            vec2  V = ToDirection2D( radians );
            if( Player->Draw_FaceLeft ) {
                V.x = -V.x;
            }
            
            orect R = orectCXD( P + V * 0.2f, V, Dim );
            DrawORect( Pass_Game, R, Color );
        }
        
        //if( Player->Fire_IsActive ) {
        //vec2 P = Player->Fire_pos;
        //vec2 N = Player->Fire_dir;
        
        //vec2 A = P + N * Player->Fire_lo;
        //vec2 B = P + N * Player->Fire_hi;
        //DrawLine( Pass_Game, A, B, COLOR_WHITE );
        //}
#endif
    }
    
    vec2 bPos = GetTL( AppState->App_Bound ) + Vec2( 72.0f, -48.0f );
    { // PLAYER HUD
        flo32 Bar_Height  = 14.0f;
        flo32 Bar_MarginX = 16.0f;
        
        flo32 HealthUnit_Width  = 14.0f;
        flo32 HealthUnit_Margin = 8.0f;
        vec2  HealthUnit_Dim    = Vec2( HealthUnit_Width, Bar_Height );
        
        flo32 StaminaUnit_Width = 20.0f;
        flo32 StaminaXPUnit_BaseWidth  = 100.0f;
        flo32 StaminaXPUnit_LevelWidth =  20.0f;
        
        flo32 HealthBar_Width = ( HealthUnit_Width * ( flo32 )Game->MaxHealth );
        HealthBar_Width += ( HealthUnit_Margin * ( flo32 )( Game->MaxHealth - 1 ) );
        
        flo32 StaminaBar_Widths[] {
            200.0f,  250.0f,  300.0f,  350.0f,  400.0f,  450.0f, 
        };
        flo32 StaminaXPBar_Widths[] = {
            120.0f,  160.0f,  200.0f,  240.0f,  280.0f,  320.0f,
        };
        
        flo32 StaminaBar_Width   = StaminaBar_Widths  [ Game->Stamina_Level ];
        flo32 StaminaXPBar_Width = StaminaXPBar_Widths[ Game->Stamina_Level ];
        
        vec2 HealthBar_Dim    = Vec2( HealthBar_Width,  Bar_Height );
        vec2 StaminaBar_Dim   = Vec2( StaminaBar_Width, Bar_Height );
        vec2 StaminaXPBar_Dim = Vec2( StaminaXPBar_Width, Bar_Height );
        
        flo32 Panel_Width = MaxValue( HealthBar_Width, StaminaBar_Width );
        Panel_Width += ( Bar_MarginX * 2.0f );
        
        flo32 Label_OffsetY   = 20.0f;
        flo32 Element_Height  = Bar_Height + Label_OffsetY;
        flo32 Element_MarginY = 8.0f;
        
        uint32 nBar = 2;
        if( Game->StaminaXP_IsFeatured ) {
            nBar = 3;
        }
        flo32 Panel_Height = ( Element_Height * ( flo32 )nBar );
        Panel_Height += ( Element_MarginY * ( flo32 )( nBar + 1 ) );
        
        vec2 Panel_Dim = Vec2( Panel_Width, Panel_Height );
        
        vec2 AtPos = bPos;
        vec2 MoneyPos = {};
        { // Panel
            rect R = RectTLD( AtPos, Panel_Dim );
            DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.02f ) );
            DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
            
            MoneyPos = GetTR( R );
            
            AtPos.y -= Element_MarginY;
        }
        
        { // Health Bar
            DrawString( Pass_UI, AppState->Font, "HP:", AtPos + Vec2( 8.0f, 0.0f ), TextAlign_TopLeft, Vec2( 1.1f, 1.1f ), COLOR_WHITE );
            
            vec2 At = AtPos + Vec2( Bar_MarginX, -Element_Height );
            
            uint32 nFill  = Player->Health;
            uint32 nEmpty = Game->MaxHealth - Player->Health;
            
            for( uint32 iUnit = 0; iUnit < nFill; iUnit++ ) {
                rect R = RectBLD( At, HealthUnit_Dim );
                DrawRect( Pass_UI, R, COLOR_RED );
                
                At.x += ( HealthUnit_Width + HealthUnit_Margin );
            }
            
            for( uint32 iUnit = 0; iUnit < nEmpty; iUnit++ ) {
                rect R = RectBLD( At, HealthUnit_Dim );
                DrawRect( Pass_UI, R, ToColor( 50, 20, 20 ) );
                
                At.x += ( HealthUnit_Width + HealthUnit_Margin );
            }
            
            AtPos.y -= Element_Height;
            AtPos.y -= Element_MarginY;
        }
        
        if( Game->Stamina_IsFeatured ) { 
            { // Stamina Bar
                DrawString( Pass_UI, AppState->Font, "STAMINA:", AtPos + Vec2( 8.0f, 0.0f ), TextAlign_TopLeft, Vec2( 1.1f, 1.1f ), COLOR_WHITE );
                
                vec2 At = AtPos + Vec2( Bar_MarginX, -Element_Height );
                
                rect StaminaBar_Bound = RectBLD( At, StaminaBar_Dim );
                DrawRect( Pass_UI, StaminaBar_Bound, COLOR_BLACK );
                
                vec4 Outline_Color = COLOR_GRAY( 0.1f );
                DrawRectOutline( Pass_UI, StaminaBar_Bound, Outline_Color );
                
                flo32 t             = Clamp01( Player->Stamina / Game->MaxStamina );
                vec2  Stamina_Dim   = Vec2( StaminaBar_Dim.x * t, StaminaBar_Dim.y );
                rect  Stamina_Bound = RectBLD( At, Stamina_Dim );
                
                vec4 Color = COLOR_GREEN;
                if( !Player->Stamina_IsEnabled ) {
                    Color = ToColor( 0, 75, 0 );
                }
                DrawRect( Pass_UI, Stamina_Bound, Color );
                AtPos.y -= Element_Height;
                AtPos.y -= Element_MarginY;
            }
            
            if( Game->StaminaXP_IsFeatured ) { // Stamina XP Bar
                DrawString( Pass_UI, AppState->Font, "STAMINA XP:", AtPos + Vec2( 8.0f, 0.0f ), TextAlign_TopLeft, Vec2( 1.1f, 1.1f ), COLOR_WHITE );
                
                vec2 At = AtPos + Vec2( Bar_MarginX, -Element_Height );
                
                rect StaminaXPBar_Bound = RectBLD( At, StaminaXPBar_Dim );
                DrawRect( Pass_UI, StaminaXPBar_Bound, COLOR_BLACK );
                
                vec4 Outline_Color = COLOR_GRAY( 0.1f );
                DrawRectOutline( Pass_UI, StaminaXPBar_Bound, Outline_Color );
                
                flo32 t             = Clamp01( Game->StaminaXP / Game->MaxStaminaXP );
                vec2  Stamina_Dim   = Vec2( StaminaXPBar_Dim.x * t, StaminaXPBar_Dim.y );
                rect  Stamina_Bound = RectBLD( At, Stamina_Dim );
                
                vec4 Color = COLOR_BLACK;
                Color.r = lerp( 0.4f, t, 1.0f );
                Color.g = lerp( 0.4f, t, 1.0f );
                Color.b = lerp( 0.4f, t, 0.0f );
                
                DrawRect( Pass_UI, Stamina_Bound, Color );
            }
        }
        
        { // Money
            MoneyPos.x += 6.0f;
            
            vec2 Dim = Vec2( 100.0f, 40.0f );
            rect R   = RectTLD( MoneyPos, Dim );
            
            DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.02f ) );
            DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
            
            char str[ 12 ] = {};
            sprintf( str, "$%.02f", Player->Money );
            DrawString( Pass_UI, AppState->Font, str, MoneyPos + Vec2( 10.0f, -6.0f ), TextAlign_TopLeft, Vec2( 1.5f, 1.5f ), COLOR_WHITE );
        }
        
#if 0        
        { // grenade indicator
            AtPos.x += 12.0f;
            vec2 Dim = Vec2( 42.0f, 42.0f );
            
            rect R = RectTLD( AtPos, Dim );
            DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.02f ) );
            DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
            
            flo32 TargetTime = PLAYER_GRENADE_COOLDOWN_TARGET_TIME * 0.75f;
            flo32 t = 1.0f;
            if( Player->GrenadeThrow_CooldownTimer < 0.0f ) {
                t = 1.0f - Clamp01( Player->GrenadeThrow_CooldownTimer / -0.5f );
            } else if( Player->GrenadeThrow_CooldownTimer > TargetTime ) {
                flo32 Denom = PLAYER_GRENADE_COOLDOWN_TARGET_TIME - TargetTime;
                t = 1.0f - Clamp01( ( Player->GrenadeThrow_CooldownTimer - TargetTime ) / ( Denom ) );
            }
            vec4 background_Color_lo = COLOR_GRAY( 0.1f  );
            vec4 background_Color_hi = COLOR_GRAY( 0.35f );
            vec4 background_Color = lerp( background_Color_lo, t, background_Color_hi );
            
            flo32 tA = Clamp01( Player->GrenadeThrow_CooldownTimer / PLAYER_GRENADE_COOLDOWN_TARGET_TIME );
            rect S = R;
            S.Top = lerp( S.Bottom, 1.0f - tA, S.Top );
            DrawRect( Pass_UI, S, background_Color );
            
            vec4 Color = GRENADE_COLOR;
            if( Player->GrenadeThrow_CooldownTimer > 0.0f ) {
                Color = COLOR_GRAY( 0.2f );
            }
            
            flo32 Radius = 10.0f;
            DrawCircle       ( Pass_UI, GetCenter( R ), Radius, Color );
            DrawCircleOutline( Pass_UI, GetCenter( R ), Radius, COLOR_BLACK );
        }
#endif
    }
    
    if( !Game->StaminaXP_IsFeatured ) {
        Game->StaminaXP = 0.0f;
    }
}

internal void
FinalizePlayer( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    Player->Punch_DoPunch = false;
    if( ( Player->Punch_DoDraw ) && ( Player->Punch_ArtTimer >= PLAYER_PUNCH_ART_TARGET_TIME ) ) {
        Player->Punch_DoDraw = false;
    }
    
    Player->Use_t      = 0.0f;
    
    if( !Player->IsDead ) {
        if( ( Player->NoDamage ) && ( Player->NoDamage_Timer >= PLAYER_NO_DAMAGE_TARGET_TIME ) ) {
            Player->NoDamage = false;
        }
        
        if( ( Player->Dash_IsActive ) && ( Player->Dash_Timer >= PLAYER_DASH_TARGET_TIME ) ) {
            Player->Dash_IsActive = false;
        }
    }
    
    if( !Player->IsDead ) {
        if( Player->Health == 0 ) {
            KillPlayer( AppState );
        }
        if( Player->Position.y < AppState->DeathPlaneY ) {
            KillPlayer( AppState );
        }
    }
}