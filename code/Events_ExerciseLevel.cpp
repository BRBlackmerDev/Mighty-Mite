
internal void
Event_Exercise_Intro01( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    GAME_STATE   * Game   = &AppState->Game;
    PLAYER_STATE * Player = &AppState->Player;
    
    if( !AppState->Level_IsInitialized ) {
        AppState->Level_IsInitialized = true;
        AddTextBox( AppState, TextBoxSpeakerID_None, "SCHOOL ATHLETIC FACILITIES" );
        
        //Game->StaminaXP_IsFeatured = false;
    }
    
    LEVEL_STATS Stat = AppState->Stat;
    
    vec2 P = GetPOfLevel( Vec2( 20.0f, 5.0f ), Stat.Bound );
    vec2 Q = GetPOfLevel( Vec2( 23.0f, 7.0f ), Stat.Bound );
    rect R = RectMM( P, Q );
    
    CONTROLLER_STATE * Control = &Platform->Controller;
    vec2 PlayerP = Player->Position + Vec2( 0.0f, TILE_HEIGHT * 1.0f );
    
    if( ( WasPressed( Control, ControllerButton_Y ) ) && ( IsInBound( PlayerP, R ) ) ) {
        ShowTextBox( AppState, 0 );
    }
    
    RENDER_PASS * Pass_Game = &Platform->Renderer.Pass_Game;
    DrawRectOutline( Pass_Game, R, COLOR_GREEN );
}

internal void
Event_Exercise_Intro02( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    if( !AppState->Level_IsInitialized ) {
        AppState->Level_IsInitialized = true;
        AddTextBox( AppState, TextBoxSpeakerID_None, "Wait... Stamina? Was that green bar always there?" );
    }
    
    LEVEL_STATS Stat = AppState->Stat;
    
    vec2 P = GetPOfLevel( Vec2(  8.0f,  5.0f ), Stat.Bound );
    vec2 Q = GetPOfLevel( Vec2( 14.0f, 13.0f ), Stat.Bound );
    rect R = RectMM( P, Q );
    
    CONTROLLER_STATE * Control = &Platform->Controller;
    PLAYER_STATE     * Player  = &AppState->Player;
    vec2 PlayerP = Player->Position + Vec2( 0.0f, TILE_HEIGHT * 1.0f );
    
    boo32 * IsInit  = _addr( ptr, boo32 );
    if( ( !( *IsInit ) ) && ( IsInBound( PlayerP, R ) ) ) {
        *IsInit  = true;
        ShowTextBox( AppState, 0 );
    }
    
    RENDER_PASS * Pass_Game = &Platform->Renderer.Pass_Game;
    DrawRectOutline( Pass_Game, R, COLOR_GREEN );
}

internal void
Event_Exercise_LockerRoom( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    if( !AppState->Level_IsInitialized ) {
        AppState->Level_IsInitialized = true;
        AddTextBox( AppState, TextBoxSpeakerID_Coach, "Hold it, son! You'll never last out there with such low stamina!" );
        AddTextBox( AppState, TextBoxSpeakerID_Coach, "There! Stamina XP! Just fill up that bar and you'll upgrade your stamina!", 2 );
        AddTextBox( AppState, TextBoxSpeakerID_Coach, "Soon you'll have tons of energy to run, and jump, and throw things- whatever you need!", 3 );
        AddTextBox( AppState, TextBoxSpeakerID_Coach, "If you wanna use the room next door to train, feel free, but you can always train out in the field!" );
    }
    
    LEVEL_STATS    Stat    = AppState->Stat;
    GAME_STATE   * Game    = &AppState->Game;
    PLAYER_STATE * Player  = &AppState->Player;
    vec2           PlayerP = Player->Position + Vec2( 0.0f, TILE_HEIGHT * 1.0f );
    
    uint32 * iTextBox = _addr( ptr, uint32 );
    
    if( *iTextBox == 1 ) {
        Game->StaminaXP_IsFeatured = true;
        ShowTextBox( AppState, 1 );
        ( *iTextBox )++;
    }
    
    if( !Game->StaminaXP_IsFeatured ) {
        vec2 P = GetPOfLevel( Vec2( 20.0f, 4.0f ), Stat.Bound );
        vec2 Q = GetPOfLevel( Vec2( 28.0f, 9.0f ), Stat.Bound );
        rect R = RectMM( P, Q );
        
        RENDER_PASS * Pass_Game = &Platform->Renderer.Pass_Game;
        DrawRectOutline( Pass_Game, R, COLOR_GREEN );
        
        if( IsInBound( PlayerP, R ) ) {
            ShowTextBox( AppState, 0 );
            ( *iTextBox )++;
        }
    }
}

internal void
Event_ExerciseMiniBoss02( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    LEVEL_STATS Stat = AppState->Stat;
    
    PLAYER_STATE        * Player = &AppState->Player;
    EXERCISE_MINIBOSS02 * Boss   = &AppState->ExerciseMiniBoss02;
    if( !Boss->IsActive ) {
        InitExerciseMiniBoss02( AppState );
        
        LEVEL_STATE * LevelS = &AppState->LevelS;
        LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
        Level->iLinkLeft_IsActive  = false;
        Level->iLinkRight_IsActive = false;
    }
    
    flo32 * TimerA = _addr( ptr, flo32 );
    flo32 * TimerB = _addr( ptr, flo32 );
    
    if( Boss->IsActive ) {
        POPPER_STATE * PopperS = &AppState->PopperS;
        if( Boss->Mode == ExerciseMiniBoss02Mode_IsDying ) {
            Player->NoDamage = true;
            
            if( PopperS->nPopper > 0 ) {
                *TimerB += Platform->TargetSec;
                
                // TODO: Make this TargetTime faster.
                if( *TimerB >= 1.0f ) {
                    *TimerB = 0.0f;
                    
                    POPPER * Popper = PopperS->Popper + 0;
                    Popper->Mode        = PopperMode_IsPopping;
                    Popper->Velocity.y  = 8.0f;
                    Popper->Timer       = 0.0f;
                }
            }
        } else {
            if( PopperS->nPopper < 2 ) {
                *TimerA += Platform->TargetSec;
                
                if( *TimerA >= 6.0f ) {
                    *TimerA = 0.0f;
                    
                    flo32 X = RandomF32InRange( 2.0f, 30.0f );
                    AddPopper( AppState, PopperC( GetPOfLevel( Vec2( X, 20.0f ), Stat.Bound ) ) );
                }
            }
        }
        
        if( Boss->Mode == ExerciseMiniBoss02Mode_EndEncounter ) {
            Boss->IsActive = false;
            
            LEVEL_STATE * LevelS = &AppState->LevelS;
            LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
            Level->iLinkLeft_IsActive  = true;
            Level->iLinkRight_IsActive = true;
            
            Level->Event[ iEvent ].IsActive = false;
            
            // TODO: Kill any poppers without killing player
            
            Player->NoDamage_Timer = PLAYER_NO_DAMAGE_TARGET_TIME;
        }
    }
}

internal void
Event_ExerciseMiniBoss03( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    LEVEL_STATS Stat = AppState->Stat;
    
    PLAYER_STATE        * Player = &AppState->Player;
    EXERCISE_MINIBOSS03 * Boss   = &AppState->ExerciseMiniBoss03;
    if( !Boss->IsActive ) {
        InitExerciseMiniBoss03( AppState );
        
        LEVEL_STATE * LevelS = &AppState->LevelS;
        LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
        Level->iLinkLeft_IsActive  = false;
        Level->iLinkRight_IsActive = false;
    }
    
    flo32 * SpawnTimerA = _addr( ptr, flo32 );
    flo32 * SpawnTimerB = _addr( ptr, flo32 );
    flo32 * TimerB = _addr( ptr, flo32 );
    
    if( Boss->IsActive ) {
        POPPER_STATE * PopperS = &AppState->PopperS;
        if( Boss->Mode == ExerciseMiniBoss03Mode_IsDying ) {
            Player->NoDamage = true;
            
            if( PopperS->nPopper > 0 ) {
                *TimerB += Platform->TargetSec;
                
                // TODO: Make this TargetTime faster.
                if( *TimerB >= 1.0f ) {
                    *TimerB = 0.0f;
                    
                    POPPER * Popper = PopperS->Popper + 0;
                    Popper->Mode        = PopperMode_IsPopping;
                    Popper->Velocity.y  = 8.0f;
                    Popper->Timer       = 0.0f;
                }
            }
        } else if( Boss->Mode == ExerciseMiniBoss03Mode_Spawn ) {
            *SpawnTimerA += Platform->TargetSec;
            if( *SpawnTimerA >= 3.4f ) {
                *SpawnTimerA = 0.0f;
                vec2 SpawnPos = GetPOfLevel( Vec2( 26.0f, 6.0f ), Stat.Bound );
                vec2 SpawnVel = Vec2( -10.0f, 0.0f );
                //AddPopper( AppState, PopperC( SpawnPos, SpawnVel ) );
                AddHedgehog( AppState, HedgehogC( SpawnPos, SpawnVel ) );
            }
            
            //*SpawnTimerB += Platform->TargetSec;
            if( *SpawnTimerB >= 10.0f ) {
                *SpawnTimerB = 0.0f;
                
                uint32 nSpawn = 1;
                flo32  SpawnX = 20.0f;
                flo32  SpawnY = 30.0f;
                
                for( uint32 iSpawn = 0; iSpawn < nSpawn; iSpawn++ ) {
                    vec2 Pos   = GetPOfLevel( Vec2( SpawnX, SpawnY ), Stat.Bound );
                    vec2 Dim   = TILE_DIM * Vec2( 3.0f, 3.0f );
                    rect Bound = RectBLD( Pos, Dim );
                    
                    //AddBreakBlock( AppState, Bound );
                    AddPushBlock( AppState, Bound );
                    
                    SpawnX += Dim.x;
                }
            }
        }
        
        if( Boss->Mode == ExerciseMiniBoss03Mode_EndEncounter ) {
            Boss->IsActive = false;
            
            LEVEL_STATE * LevelS = &AppState->LevelS;
            LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
            Level->iLinkLeft_IsActive  = true;
            Level->iLinkRight_IsActive = true;
            
            Level->Event[ iEvent ].IsActive = false;
            
            // TODO: Kill any poppers without killing player
            
            Player->NoDamage_Timer = PLAYER_NO_DAMAGE_TARGET_TIME;
        }
    }
}

internal void
Event_ExerciseBoss( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    LEVEL_STATS Stat = AppState->Stat;
    
    EXERCISE_BOSS * Boss = &AppState->ExerciseBoss;
    if( !Boss->IsActive ) {
        InitExerciseBoss( AppState );
        
        LEVEL_STATE * LevelS = &AppState->LevelS;
        LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
        Level->iLinkLeft_IsActive  = false;
        Level->iLinkRight_IsActive = false;
    }
    
    if( ( Boss->IsActive ) && ( Boss->Mode == ExerciseBossMode_IsDead ) ) {
        Boss->IsActive = false;
        
        LEVEL_STATE * LevelS = &AppState->LevelS;
        LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
        Level->iLinkLeft_IsActive  = true;
        Level->iLinkRight_IsActive = true;
        
        Level->Event[ iEvent ].IsActive = false;
    }
}