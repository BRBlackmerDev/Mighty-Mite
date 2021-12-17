
internal void
ResetEventMemory( APP_STATE * AppState ) {
    memset( &AppState->Event, 0, sizeof( EVENT_STATE ) );
    ResetMemory( &AppState->Event_Memory );
}

internal void
ResetPlayerMemory( APP_STATE * AppState ) {
    memset( &AppState->Player,     0, sizeof( PLAYER_STATE       ) );
}

internal void
ResetLevelMemory( APP_STATE * AppState ) {
    ResetCollision( AppState );
    
    uint8 * AddrA = ( uint8 * )&AppState->Reset_LevelReset_Start;
    uint8 * AddrB = ( uint8 * )&AppState->Reset_LevelReset_End;
    uint32 Reset_Size = ( uint32 )( AddrB - AddrA );
    memset( AddrA, 0, Reset_Size );
    
    AppState->Level_IsInitialized = false;
}

internal void
SaveGame( APP_STATE * AppState, MEMORY * TempMemory ) {
    // TODO: Save Game!!
}

internal void
InitCameraForLevel( APP_STATE * AppState ) {
    CAMERA_STATE * Camera = &AppState->Camera;
    
    flo32 AspectRatio = 1920.0f / 1080.0f;
    flo32 DimY      = CAMERA_TILE_Y_COUNT * TILE_HEIGHT;
    flo32 DimX      = DimY * AspectRatio;
    Camera->Dim     = Vec2( DimX, DimY );
    Camera->HalfDim = Camera->Dim * 0.5f;
}

internal vec2
GetCheckpointPOfCurrentCheckpoint( APP_STATE * AppState ) {
    LEVEL_STATE      * LevelS      = &AppState->LevelS;
    CHECKPOINT_STATE * CheckpointS = &AppState->CheckpointS;
    
    vec2 Result = {};
    
    if( CheckpointS->nCheckpoint > 0 ) {
        CHECKPOINT Checkpoint = CheckpointS->Checkpoint[ LevelS->Checkpoint_iCheckpoint ];
        Result = GetBC( Checkpoint.Bound );
    } else {
        LEVEL_STATS Stat = AppState->Stat;
        
        flo32 X = Stat.Bound.Left + TILE_WIDTH * 0.25f;
        flo32 Y = Stat.EnterLeftY;
        Result = Vec2( X, Y );
    }
    return Result;
}

internal char *
GetFileNameOfCurrentCheckpoint( APP_STATE * AppState ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LEVEL       * Level  = LevelS->Level + LevelS->Checkpoint_iLevel;
    
    char * Result = Level->FileNameOfLevel;
    return Result;
}

internal char *
GetFileNameOfCurrentLevel( APP_STATE * AppState ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
    
    char * Result = Level->FileNameOfLevel;
    return Result;
}

internal void
AddLevelLink( APP_STATE * AppState, int32 iBaseLevel, int32 iNextLevel ) {
    Assert( iBaseLevel != iNextLevel );
    Assert( iBaseLevel > -1 );
    Assert( iNextLevel > -1 );
    
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LEVEL * BaseLevel = LevelS->Level + iBaseLevel;
    LEVEL * NextLevel = LevelS->Level + iNextLevel;
    
    LEVEL_BUILDER * Builder = &AppState->LevelBuilder;
    
    switch( Builder->Mode ) {
        case LevelBuilderMode_AddToLeft: {
            Assert( BaseLevel->iLinkLeft  == -1 );
            Assert( NextLevel->iLinkRight == -1 );
            
            BaseLevel->iLinkLeft           = iNextLevel;
            BaseLevel->iLinkLeft_IsActive  = true;
            
            NextLevel->iLinkRight          = iBaseLevel;
            NextLevel->iLinkRight_IsActive = true;
        } break;
        
        case LevelBuilderMode_AddToRight: {
            Assert( BaseLevel->iLinkRight == -1 );
            Assert( NextLevel->iLinkLeft  == -1 );
            
            BaseLevel->iLinkRight          = iNextLevel;
            BaseLevel->iLinkRight_IsActive = true;
            
            NextLevel->iLinkLeft           = iBaseLevel;
            NextLevel->iLinkLeft_IsActive  = true;
            
        } break;
        
        case LevelBuilderMode_AddToBottom: {
            Assert( BaseLevel->iLinkBottom == -1 );
            Assert( NextLevel->iLinkTop    == -1 );
            
            BaseLevel->iLinkBottom          = iNextLevel;
            BaseLevel->iLinkBottom_IsActive = true;
            
            NextLevel->iLinkTop             = iBaseLevel;
            NextLevel->iLinkTop_IsActive    = true;
            
        } break;
        
        case LevelBuilderMode_AddToTop: {
            Assert( BaseLevel->iLinkTop    == -1 );
            Assert( NextLevel->iLinkBottom == -1 );
            
            BaseLevel->iLinkTop             = iNextLevel;
            BaseLevel->iLinkTop_IsActive    = true;
            
            NextLevel->iLinkBottom          = iBaseLevel;
            NextLevel->iLinkBottom_IsActive = true;
        } break;
    }
}

#if 0
internal void
AddLevelLink( APP_STATE * AppState, char * FileNameOfLevelA, char * FileNameOfLevelB ) {
    Assert( !MatchString( FileNameOfLevelA, FileNameOfLevelB ) );
    
    LEVEL_STATE * LevelS = &AppState->LevelS;
    
    int32 iLinkA = -1;
    int32 iLinkB = -1;
    for( uint32 iLevel = 0; iLevel < LevelS->nLevel; iLevel++ ) {
        LEVEL * Level = LevelS->Level + iLevel;
        
        if( MatchString( Level->FileNameOfLevel, FileNameOfLevelA ) ) {
            iLinkA = iLevel;
        }
        if( MatchString( Level->FileNameOfLevel, FileNameOfLevelB ) ) {
            iLinkB = iLevel;
        }
    }
    
    AddLevelLink( AppState, iLinkA, iLinkB );
}
#endif

internal int32
GetLevelIndexOfFileName( APP_STATE * AppState, char * FileNameOfLevel ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    
    int32 Result = -1;
    for( uint32 iLevel = 0; iLevel < LevelS->nLevel; iLevel++ ) {
        LEVEL * Level = LevelS->Level + iLevel;
        
        if( MatchString( Level->FileNameOfLevel, FileNameOfLevel ) ) {
            Result = iLevel;
        }
    }
    
    return Result;
}

internal void
AddLevel( APP_STATE * AppState, char * FileNameOfLevel, char * FileNameOfPrevLevel = 0 ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    
    if( LevelS->nLevel < LEVEL_MAX_COUNT ) {
        LEVEL Level = {};
        strcpy( Level.FileNameOfLevel, FileNameOfLevel );
        Level.iLinkLeft   = -1;
        Level.iLinkRight  = -1;
        Level.iLinkBottom = -1;
        Level.iLinkTop    = -1;
        LevelS->Level[ LevelS->nLevel++ ] = Level;
        
        LEVEL_BUILDER * Builder = &AppState->LevelBuilder;
        
        if( FileNameOfPrevLevel ) {
            Builder->PrevLevel = GetLevelIndexOfFileName( AppState, FileNameOfPrevLevel );
        }
        
        if( Builder->PrevLevel > -1 ) {
            AddLevelLink( AppState, Builder->PrevLevel, LevelS->nLevel - 1 );
        }
        
        Builder->PrevLevel = LevelS->nLevel - 1;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new LEVEL in StartGame(), but the state is full! Max Count = %u", LEVEL_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RevivePlayer( APP_STATE * AppState ) {
    ResetPlayerMemory( AppState );
    
    PLAYER_STATE * Player = &AppState->Player;
    GAME_STATE   * Game   = &AppState->Game;
    Player->Position = GetCheckpointPOfCurrentCheckpoint( AppState );
    Player->Health   = Game->MaxHealth;
    Player->Stamina  = Game->MaxStamina;
    Player->Stamina_IsEnabled = true;
    Player->Face_DirX = 1.0f;
}

internal void
InitGame( APP_STATE * AppState ) {
    GAME_STATE * Game = &AppState->Game;
    Game->MaxHealth = PLAYER_HEALTH;
    
    //Game->Stamina_Level = 0;
    //SetMaxStamina( Game );
    
    Game->Stamina_IsFeatured   = true;
    Game->StaminaXP_IsFeatured = true;
    Game->Run_IsFeatured       = true;
    Game->Carry_IsFeatured     = true;
    Game->Punch_IsFeatured     = false;
    Game->Grab_IsFeatured      = true;
    Game->Dash_IsFeatured      = true;
    Game->WallJump_IsFeatured  = false;
    Game->WallSlide_IsFeatured = false;
    
    Game->Stamina_Level = 1;
    SetMaxStamina( Game );
}

internal void
LoadCheckpoint( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    ResetLevelMemory ( AppState );
    // Reset events? probably not, i think i'm just adding additional events to handle cases of death (is there a better way?)
    
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LevelS->iCurrentLevel = LevelS->Checkpoint_iLevel;
    
    char * CurrentLevel = GetFileNameOfCurrentCheckpoint( AppState );
    GAME_LoadLevel( Platform, AppState, TempMemory, LEVEL_SAVE_DIRECTORY, CurrentLevel );
    
    RevivePlayer( AppState );
    InitCameraForLevel( AppState );
}

internal void
ToNextLevel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    AppState->DoLevelTransition = false;
    
    ResetLevelMemory( AppState );
    // Reset events? probably not, i think i'm just adding additional events to handle cases of death (is there a better way?)
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LevelS->iCurrentLevel = AppState->LevelTransition_iLevel;
    
    char * CurrentLevel = GetFileNameOfCurrentLevel( AppState );
    GAME_LoadLevel( Platform, AppState, TempMemory, LEVEL_SAVE_DIRECTORY, CurrentLevel );
    
    LEVEL_STATS Stat = AppState->Stat;
    PLAYER_STATE * Player = &AppState->Player;
    
    vec2  EnterPos = Player->Position;
    flo32 MarginX  = TILE_WIDTH  * 0.25f;
    
    switch( AppState->LevelTransition_Mode ) {
        case LevelTransitionMode_ExitLeft: {
            EnterPos.x  = Stat.Bound.Right - MarginX;
            EnterPos.y += ( Stat.EnterRightY - AppState->LevelTransition_Exit );
        } break;
        
        case LevelTransitionMode_ExitRight: {
            EnterPos.x  = Stat.Bound.Left + MarginX;
            EnterPos.y += ( Stat.EnterLeftY - AppState->LevelTransition_Exit );
        } break;
        
        case LevelTransitionMode_ExitBottom: {
            EnterPos.x += ( Stat.EnterTopX - AppState->LevelTransition_Exit );
            EnterPos.y  = Stat.Bound.Top - ( TILE_HEIGHT * 0.35f );
        } break;
        
        case LevelTransitionMode_ExitTop: {
            EnterPos.x += ( Stat.EnterBottomX - AppState->LevelTransition_Exit );
            EnterPos.y  = Stat.Bound.Bottom;
            Player->Velocity.y    = 12.0f;
            Player->Jump_IsActive = false;
            Player->Jump_DoDampen = false;
        } break;
    }
    
    Player->Position = EnterPos;
    
    InitCameraForLevel( AppState );
}

internal void
UpdateDebugLevelNavigation( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    CONTROLLER_STATE * Controller = &Platform->Controller;
    
    CONTROLLER_BUTTON_ID  ID_NextCheckpoint = ControllerButton_dPad_Right;
    CONTROLLER_BUTTON_ID  ID_PrevCheckpoint = ControllerButton_dPad_Left;
    CONTROLLER_BUTTON_ID  ID_NextLevel      = ControllerButton_dPad_Up;
    CONTROLLER_BUTTON_ID  ID_PrevLevel      = ControllerButton_dPad_Down;
    
    LEVEL_STATE      * LevelS      = &AppState->LevelS;
    CHECKPOINT_STATE * CheckpointS = &AppState->CheckpointS;
    
    DISPLAY_VALUE( uint32, LevelS->Checkpoint_iLevel );
    DISPLAY_VALUE( uint32, LevelS->iCurrentLevel );
    
    boo32 DoLoadCheckpoint = false;
    if( ( WasPressed( Controller, ID_NextCheckpoint ) ) && ( CheckpointS->nCheckpoint > 0 ) ) {
        DoLoadCheckpoint = true;
        
        LevelS->Checkpoint_iLevel      = LevelS->iCurrentLevel;
        LevelS->Checkpoint_iCheckpoint = ( uint32 )UInt32Wrap( LevelS->Checkpoint_iCheckpoint, 1, CheckpointS->nCheckpoint );
    } else if( ( WasPressed( Controller, ID_PrevCheckpoint ) ) && ( CheckpointS->nCheckpoint > 0 ) ) {
        DoLoadCheckpoint = true;
        
        LevelS->Checkpoint_iLevel      = LevelS->iCurrentLevel;
        LevelS->Checkpoint_iCheckpoint = ( uint32 )UInt32Wrap( LevelS->Checkpoint_iCheckpoint, -1, CheckpointS->nCheckpoint );
    } else if( WasPressed( Controller, ID_NextLevel ) ) {
        DoLoadCheckpoint = true;
        
        LevelS->Checkpoint_iLevel      = UInt32Wrap( LevelS->Checkpoint_iLevel, 1, LevelS->nLevel );
        LevelS->Checkpoint_iCheckpoint = 0;
    } else if( WasPressed( Controller, ID_PrevLevel ) ) {
        DoLoadCheckpoint = true;
        
        LevelS->Checkpoint_iLevel      = UInt32Wrap( LevelS->Checkpoint_iLevel, -1, LevelS->nLevel );
        LevelS->Checkpoint_iCheckpoint = 0;
    }
    
    if( DoLoadCheckpoint ) {
        LoadCheckpoint( Platform, AppState, TempMemory );
    }
}

internal void
StartGame( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    ResetEventMemory ( AppState );
    ResetPlayerMemory( AppState );
    ResetLevelMemory ( AppState );
    memset( &AppState->LevelS, 0, sizeof( LEVEL_STATE ) );
    
    AppState->Mode = AppMode_Game;
    
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LevelS->nLevel = 0;
    
    MEMORY * Event_Memory = &AppState->Event_Memory;
    
    LEVEL_BUILDER * Builder = &AppState->LevelBuilder;
    Builder->PrevLevel = -1;
    
    // TODO: Init room layout here!!
    //AddLevel( AppState, "vert01" );
    //Builder->Mode = LevelBuilderMode_AddToTop;
    //AddLevel( AppState, "vert02" );
    
    
    //Builder->Mode = LevelBuilderMode_AddToRight;
    //AddLevel( AppState, "Exercise_MiniBoss03" );
    //AddEvent( AppState, EventC( Event_ExerciseMiniBoss03, Event_Memory ) );
    
    //AddLevel( AppState, "Camper05" );
    //AddLevel( AppState, "Camper04" );
    //AddLevel( AppState, "Camper03" );
    //AddLevel( AppState, "Camper02" );
    //AddLevel( AppState, "Camper01" );
    
#if 0 // Portfolio Demo
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "Exercise_Intro01" );
    AddEvent( AppState, EventC( Event_Exercise_Intro01, Event_Memory ) );
    
    AddLevel( AppState, "Exercise_Intro02" );
    AddEvent( AppState, EventC( Event_Exercise_Intro02, Event_Memory ) );
    AddLevel( AppState, "Exercise_Intro04" );
    AddLevel( AppState, "Exercise_LockerEntrance" );
    AddLevel( AppState, "Exercise_LockerRoom" );
    AddEvent( AppState, EventC( Event_Exercise_LockerRoom, Event_Memory ) );
    AddLevel( AppState, "Exercise_TrainingRoom01" );
    Builder->Mode = LevelBuilderMode_AddToTop;
    AddLevel( AppState, "Exercise_TrainingRoom01_Attic" );
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "ExerciseBall_JumpIntro01", "Exercise_TrainingRoom01" );
    AddLevel( AppState, "Exercise_Intro03" );
    AddLevel( AppState, "Puncher01a" );
    AddLevel( AppState, "ExerciseBall_Puncher01" );
    AddLevel( AppState, "ExerciseBall_Puncher02" );
    Builder->Mode = LevelBuilderMode_AddToBottom;
    AddLevel( AppState, "Exercise_BasementCave01" );
    Builder->Mode = LevelBuilderMode_AddToLeft;
    AddLevel( AppState, "Camper01" );
    
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "PushBlock01", "ExerciseBall_Puncher02" );
    AddLevel( AppState, "Puncher01b" );
    AddLevel( AppState, "Puncher01c" );
    AddLevel( AppState, "CycleBlock_Terrain01" );
    AddLevel( AppState, "CycleBlock_Conveyor01" );
    AddLevel( AppState, "Puncher01h" );
    AddLevel( AppState, "Puncher01e" );
    Builder->Mode = LevelBuilderMode_AddToTop;
    AddLevel( AppState, "Camper02" );
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "Puncher01g", "Puncher01e" );
    
    AddLevel( AppState, "BreakBlock01" );
    AddLevel( AppState, "CollapsePlatform01" );
    AddLevel( AppState, "Conveyor02" );
    AddLevel( AppState, "Popper01" );
    Builder->Mode = LevelBuilderMode_AddToTop;
    AddLevel( AppState, "Camper03" );
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "Popper02", "Popper01" );
    AddLevel( AppState, "Exercise_MiniBoss02" );
    AddEvent( AppState, EventC( Event_ExerciseMiniBoss02, Event_Memory ) );
    
    AddLevel( AppState, "PushBlock03" );
    AddLevel( AppState, "CollapsePlatform02" );
    AddLevel( AppState, "BreakBlock_Puncher01" );
    AddLevel( AppState, "CycleBlock_Terrain02" );
    AddLevel( AppState, "CollapsePlatform03" );
    
    AddLevel( AppState, "Boulder_Seesaw01" );
    AddLevel( AppState, "Conveyor_Boulder01" );
    AddLevel( AppState, "Boulder_ScreenCarry01" );
    AddLevel( AppState, "Boulder_ScreenCarry02" );
    Builder->Mode = LevelBuilderMode_AddToTop;
    AddLevel( AppState, "Camper04" );
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "Conveyor_Boulder02", "Boulder_ScreenCarry02" );
    
    AddLevel( AppState, "CollapsePlatform_Conveyor01" );
    AddLevel( AppState, "CollapsePlatform_CycleBlock01" );
    AddLevel( AppState, "CollapsePlatform_Scaffold01" );
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "Camper05" );
    Builder->Mode = LevelBuilderMode_AddToTop;
    AddLevel( AppState, "CollapsePlatform_Spikes01", "CollapsePlatform_Scaffold01" );
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "Checkpoint01" );
    AddLevel( AppState, "Exercise_MiniBoss03" );
    AddEvent( AppState, EventC( Event_ExerciseMiniBoss03, Event_Memory ) );
    AddLevel( AppState, "CollapsePlatform_PushBlock01" );
#endif
    
#if 1
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "Spotter_PushBlock01" );
    AddLevel( AppState, "Camper_Spotter01" );
    AddLevel( AppState, "Spotter_PushBlock01" );
    AddLevel( AppState, "Exercise_Intro01" );
    AddEvent( AppState, EventC( Event_Exercise_Intro01, Event_Memory ) );
    
    AddLevel( AppState, "Puncher01b" );
    AddLevel( AppState, "Puncher01c" );
    AddLevel( AppState, "Puncher01d" );
    AddLevel( AppState, "Puncher01e" );
    AddLevel( AppState, "Puncher01f" );
    AddLevel( AppState, "Puncher01g" );
    AddLevel( AppState, "Puncher01h" );
    
    AddLevel( AppState, "Exercise_Intro02" );
    AddEvent( AppState, EventC( Event_Exercise_Intro02, Event_Memory ) );
    AddLevel( AppState, "Exercise_Intro04" );
    AddLevel( AppState, "Exercise_LockerEntrance" );
    AddLevel( AppState, "Exercise_LockerRoom" );
    AddEvent( AppState, EventC( Event_Exercise_LockerRoom, Event_Memory ) );
    AddLevel( AppState, "Exercise_TrainingRoom01" );
    Builder->Mode = LevelBuilderMode_AddToTop;
    AddLevel( AppState, "Exercise_TrainingRoom01_Attic" );
    
    Builder->Mode = LevelBuilderMode_AddToRight;
    AddLevel( AppState, "ExerciseBall_JumpIntro01", "Exercise_TrainingRoom01" );
    AddLevel( AppState, "Exercise_Intro03" );
    AddLevel( AppState, "Puncher01a" );
    AddLevel( AppState, "ExerciseBall_Puncher01" );
    AddLevel( AppState, "ExerciseBall_Puncher02" );
    AddLevel( AppState, "PushBlock01" );
    AddLevel( AppState, "PushBlock02" );
    //AddLevel( AppState, "Stomper01" );
    //AddEvent( AppState, EventC( Stomper01_Event01, Event_Memory ) );
    AddLevel( AppState, "ExerciseBall_Puncher03" );
    AddLevel( AppState, "PushBlock03" );
    
    AddLevel( AppState, "Scaffold01a" );
    AddLevel( AppState, "CycleBlock_Terrain01" );
    AddLevel( AppState, "CycleBlock_Scaffold01" );
    AddLevel( AppState, "CycleBlock_Conveyor01" );
    //AddLevel( AppState, "CycleBlockSet_BoulderInit" );
    //AddEvent( AppState, EventC( CycleBlockSet_BoulderInit_Event01, Event_Memory ) );
    
    AddLevel( AppState, "Boulder_Seesaw01" );
    AddLevel( AppState, "Conveyor_Boulder01" );
    AddLevel( AppState, "Boulder_ScreenCarry01" );
    AddLevel( AppState, "Boulder_ScreenCarry02" );
    AddLevel( AppState, "CollapsePlatform01" );
    AddLevel( AppState, "CollapsePlatform_Boulder01" );
    AddLevel( AppState, "Conveyor_Boulder02" );
    
    AddLevel( AppState, "BreakBlock01" );
    AddLevel( AppState, "Conveyor01" );
    AddLevel( AppState, "Conveyor02" );
    AddLevel( AppState, "CollapsePlatform02" );
    AddLevel( AppState, "BreakBlock_Puncher01" );
    AddLevel( AppState, "CycleBlock_Terrain02" );
    AddLevel( AppState, "CollapsePlatform03" );
    
    AddLevel( AppState, "CollapsePlatform_Conveyor01" );
    AddLevel( AppState, "CollapsePlatform_CycleBlock01" );
    AddLevel( AppState, "CollapsePlatform_Scaffold01" );
    AddLevel( AppState, "CollapsePlatform_Spikes01" );
    AddLevel( AppState, "Popper01" );
    AddLevel( AppState, "Popper02" );
    AddLevel( AppState, "Checkpoint01" );
    AddLevel( AppState, "Exercise_MiniBoss02" );
    AddEvent( AppState, EventC( Event_ExerciseMiniBoss02, Event_Memory ) );
    AddLevel( AppState, "CollapsePlatform_PushBlock01" );
#endif
    
#if 0    
    AddLevel( AppState, "test" );
    AddLevel( AppState, "ExerciseBoss" );
    AddEvent( AppState, EventC( Event_ExerciseBoss, Event_Memory ) );
    AddLevel( AppState, "temp" );
    AddLevel( AppState, "Stomper02" );
    AddEvent( AppState, EventC( Stomper02_Event01, Event_Memory ) );
    AddLevel( AppState, "TempB" );
    AddLevel( AppState, "TempA" );
    EVENT Event_TempA_Event01 = {};
    Event_TempA_Event01.IsActive = true;
    Event_TempA_Event01.Func     = TempA_Event01;
    Event_TempA_Event01.Data     = 0; // TODO: FIX THIS!!
    AddEvent( AppState, Event_TempA_Event01 );
#endif
    
    
    //AddLevel( AppState, "temp", 1 );
    //AddLevel( AppState, "showcase03", 1 );
    //AddLevel( AppState, "showcase01", 6 );
    //AddLevel( AppState, "showcase02", 10 );
    
    // Access LEVEL_STATE to init the game to a specific level.
    // LevelS->CurrentLevel      = 0;
    // LevelS->CurrentCheckpoint = 0;
    
    char * CurrentLevel = GetFileNameOfCurrentCheckpoint( AppState );
    GAME_LoadLevel( Platform, AppState, TempMemory, LEVEL_SAVE_DIRECTORY, CurrentLevel );
    
    //AddScaffold( AppState, 506, 513, 514 );
    //AddScaffold( AppState, 506, 513, 517 );
    //AddScaffold( AppState, 517, 522, 514 );
    //AddRunner( AppState, Vec2( 506.5f, 515.5f ) );
    //AddSeesaw( AppState, UInt32Pair( 512, 512 ) );
    //AddBoulder( AppState, Vec2( 500.0f, 512.0f ) );
    //AddBoulder( AppState, Vec2( 532.5f, 520.0f ) );
    //AddStomper( AppState, Vec2( 504.0f, 520.0f ) );
    //AddStomper( AppState, Vec2( 520.0f, 520.0f ) );
    //AddSeesaw( AppState, UInt32Pair( 498, 513 ) );
    //AddSeesaw( AppState, UInt32Pair( 510, 513 ) );
    //AddSeesaw( AppState, UInt32Pair( 522, 513 ) );
    //InitExerciseMiniBoss01( AppState );
    //AddPuncher( AppState, Vec2( 512.0f, 511.0f ) );
    //AddTextBox( AppState, TextBoxSpeakerID_Player, "HI!!" );
    //AddExerciseBall( AppState, Vec2( 500.0f, 512.0f ) );
    
    //AddCycleBlock( AppState, UInt32Pair( 502, 507 ), 3, 4, 4 );
    //AddCycleBlock( AppState, UInt32Pair( 508, 507 ), 3, 4, 4 );
    //AddCycleBlock( AppState, UInt32Pair( 514, 511 ), 3, 4, -4 );
    
    //AddPushBlock( AppState, UInt32Pair( 512, 510 ), 3, 3 );
    //AddCollapsePlatform( AppState, UInt32Pair( 500, 509 ), 4, 1 );
    //AddCollapsePlatform( AppState, UInt32Pair( 504, 509 ), 4, 1 );
    
    //AddPopper( AppState, PopperC( Vec2( 508.0f, 512.0f ) ) );
    //AddCamper( AppState, CamperC( Vec2( 508.0f, 512.0f ) ) );
    //AddBarbell( AppState, BarbellC( Vec2( 508.0f, 512.0f ) ) );
    //AddMoney( AppState, MoneyC( MoneyType_1,   Vec2( 508.0f, 508.0f ), Vec2( 0.0f, 6.0f ) ) );
    //AddMoney( AppState, MoneyC( MoneyType_5,   Vec2( 510.0f, 508.0f ), Vec2( 0.0f, 6.0f ) ) );
    //AddMoney( AppState, MoneyC( MoneyType_10,  Vec2( 512.0f, 508.0f ), Vec2( 0.0f, 6.0f ) ) );
    //AddMoney( AppState, MoneyC( MoneyType_25,  Vec2( 514.0f, 508.0f ), Vec2( 0.0f, 6.0f ) ) );
    //AddMoney( AppState, MoneyC( MoneyType_100, Vec2( 516.0f, 508.0f ), Vec2( 0.0f, 6.0f ) ) );
    
    InitGame( AppState );
    RevivePlayer( AppState );
    InitCameraForLevel( AppState );
    
    //AddSpotter     ( AppState, Vec2( 508.0f, 510.0f ) );
    //AddExerciseBall( AppState, Vec2( 504.0f, 508.0f ) );
    //AddBoulder       ( AppState, Vec2( 504.0f, 508.0f ) );
    //AddPopper        ( AppState, PopperC( Vec2( 504.0f, 508.0f ) ) );
    //AddBarbell     ( AppState, BarbellC( Vec2( 504.0f, 508.0f ) ) );
    
    //AppState->TextTrigger_IsActive = true;
    //AppState->TextTrigger_Bound    = Rect( 520.0f, 508.0f, 528.0f, 518.0f );
}

internal void
FinalizeLevelTransition( APP_STATE * AppState ) {
    LEVEL_STATS Stat = AppState->Stat;
    
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
    
    PLAYER_STATE * Player = &AppState->Player;
    rect R = Stat.Bound;
    R.Left   -= ( TILE_WIDTH  * 0.1f );
    R.Right  += ( TILE_WIDTH  * 0.1f );
    R.Bottom -= ( TILE_HEIGHT * 1.0f );
    R.Top    -= ( TILE_HEIGHT * 0.25f );
    
    boo32 DoExitLeft   = ( Player->Position.x <= R.Left   ) && ( Level->iLinkLeft   > -1 );
    boo32 DoExitRight  = ( Player->Position.x >= R.Right  ) && ( Level->iLinkRight  > -1 );
    boo32 DoExitBottom = ( Player->Position.y <= R.Bottom ) && ( Level->iLinkBottom > -1 );
    boo32 DoExitTop    = ( Player->Position.y >= R.Top    ) && ( Level->iLinkTop    > -1 );
    
    if( ( DoExitLeft ) || ( DoExitRight ) || ( DoExitBottom ) || ( DoExitTop ) ) {
        uint32 CheckExit = ( DoExitLeft ) + ( DoExitRight ) + ( DoExitBottom ) + ( DoExitTop );
        Assert( CheckExit <= 1 );
        int32 iNext = -1;
        
        AppState->DoLevelTransition = true;
        if( DoExitLeft ) {
            AppState->LevelTransition_Mode = LevelTransitionMode_ExitLeft;
            AppState->LevelTransition_Exit = Stat.EnterLeftY;
            iNext = Level->iLinkLeft;
        } else if( DoExitRight ) {
            AppState->LevelTransition_Mode = LevelTransitionMode_ExitRight;
            AppState->LevelTransition_Exit = Stat.EnterRightY;
            iNext = Level->iLinkRight;
        } else if( DoExitBottom ) {
            AppState->LevelTransition_Mode = LevelTransitionMode_ExitBottom;
            AppState->LevelTransition_Exit = Stat.EnterBottomX;
            iNext = Level->iLinkBottom;
        } else if( DoExitTop ) {
            AppState->LevelTransition_Mode = LevelTransitionMode_ExitTop;
            AppState->LevelTransition_Exit = Stat.EnterTopX;
            iNext = Level->iLinkTop;
        }
        
        Assert( iNext > -1 );
        AppState->LevelTransition_iLevel = iNext;
    }
}