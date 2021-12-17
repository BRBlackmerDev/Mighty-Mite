
internal void
APP_UpdateAndRender( APP_STATE * AppState, PLATFORM * Platform ) {
    RENDERER * Renderer = &Platform->Renderer;
    RENDER_PASS * Pass_Game = &Renderer->Pass_Game;
    RENDER_PASS * Pass_UI   = &Renderer->Pass_UI;
    
	MOUSE_STATE      * Mouse      = &Platform->Mouse;
	KEYBOARD_STATE   * Keyboard   = &Platform->Keyboard;
    CONTROLLER_STATE * Controller = &Platform->Controller;
	
	MEMORY * PermMemory = &Platform->PermMemory;
	MEMORY * TempMemory = &Platform->TempMemory;
	
    AUDIO_SYSTEM * AudioSystem = &Platform->AudioSystem;
	//ASSET_SYSTEM * assetSystem = &Platform->assetSystem;
    
    flo32 dT = Platform->TargetSec;
    AppState->dT = dT;
    
    switch( AppState->Mode ) {
        case AppMode_Game: {
            CAMERA_STATE * Camera = &AppState->Camera;
            
            vec2 WorldPos = GetWorldPos( Camera, AppState->App_Dim, Mouse->Position );
            DISPLAY_VALUE( vec2, WorldPos );
            
            if( !AppState->IsInitialized ) {
                AppState->IsInitialized    = true;
                
                InitDebugPanel( AppState );
                StartGame( Platform, AppState, TempMemory );
                
                AppState->AudioList[ AudioID_Sound_Barbell_Bounce01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Barbell_Bounce01" );
                AppState->AudioList[ AudioID_Sound_Barbell_Bounce02 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Barbell_Bounce02" );
                AppState->AudioList[ AudioID_Sound_Barbell_Hit01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Barbell_Hit01" );
                AppState->AudioList[ AudioID_Sound_Barbell_Hit02 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Barbell_Hit02" );
                AppState->AudioList[ AudioID_Sound_Boulder ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Boulder" );
                AppState->AudioList[ AudioID_Sound_BreakBlock ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_BreakBlock" );
                AppState->AudioList[ AudioID_Sound_Camper_Drop ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Camper_Drop" );
                AppState->AudioList[ AudioID_Sound_Camper_PickUp ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Camper_PickUp" );
                AppState->AudioList[ AudioID_Sound_Camper_Save ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Camper_Save" );
                AppState->AudioList[ AudioID_Sound_Checkpoint ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Checkpoint" );
                AppState->AudioList[ AudioID_Sound_ExerciseBall_Bounce01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_ExerciseBall_Bounce01" );
                AppState->AudioList[ AudioID_Sound_ExerciseBall_Bounce02 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_ExerciseBall_Bounce02" );
                AppState->AudioList[ AudioID_Sound_ExerciseBall_Hit01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_ExerciseBall_Hit01" );
                AppState->AudioList[ AudioID_Sound_ExerciseBall_Hit02 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_ExerciseBall_Hit02" );
                AppState->AudioList[ AudioID_Sound_ExerciseBall_Hit03 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_ExerciseBall_Hit03" );
                AppState->AudioList[ AudioID_Sound_ExerciseBall_Hit04 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_ExerciseBall_Hit04" );
                AppState->AudioList[ AudioID_Sound_ExerciseBall_Hit05 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_ExerciseBall_Hit05" );
                AppState->AudioList[ AudioID_Sound_ExerciseBall_Hit06 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_ExerciseBall_Hit06" );
                AppState->AudioList[ AudioID_Sound_ExerciseBall_Hit07 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_ExerciseBall_Hit07" );
                AppState->AudioList[ AudioID_Sound_Money ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Money" );
                AppState->AudioList[ AudioID_Sound_Player_Bash ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Player_Bash" );
                AppState->AudioList[ AudioID_Sound_Player_Dash ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Player_Dash" );
                AppState->AudioList[ AudioID_Sound_Popper_Explode01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Popper_Explode01" );
                AppState->AudioList[ AudioID_Sound_Popper_Explode02 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Sound_Popper_Explode02" );
                
                AppState->AudioList[ AudioID_Music_Exercise_Boss01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Music_Exercise_Boss01" );
                AppState->AudioList[ AudioID_Music_Exercise_Stage01 ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "Music_Exercise_Stage01" );
                //AppState->AudioList[ AudioID_Sound_Pop ] = WAV_ReadFile( Platform, AUDIO_SAVE_DIRECTORY, "sound_pop01" );
                
                AppState->AudioSystem = AudioSystem;
                AudioSystem->MainVolume = 1.0f;
                
                //PlayAudio( AppState, AudioID_Music_Exercise_Stage01, 0.15f, 1.0f, true );
            }
            
            if( WasPressed( Mouse, MouseButton_Right ) ) {
                //PlayAudio( AudioSystem, AppState->AudioList, AudioID_Sound_Pop, 0.5f );
            }
            
            TEXT_BOX_STATE * TextBoxS = &AppState->TextBoxS;
            
            REPLAY_STATE * Replay = &AppState->Replay;
            PANEL        * Panel  = &AppState->Debug;
            
            if( AppState->TextTrigger_IsActive ) {
                PLAYER_STATE * Player = &AppState->Player;
                if( IsInBound( Player->Position, AppState->TextTrigger_Bound ) ) {
                    AppState->TextTrigger_IsActive = false;
                    
                    ShowTextBox( AppState, 0 );
                }
                
                DrawRectOutline( Pass_Game, AppState->TextTrigger_Bound, COLOR_GREEN );
            }
            
            PLAYER_STATE * Player = &AppState->Player;
            if( Player->DoSaveCamper ) {
                Player->DoSaveCamper = false;
                Player->Carry_ID     = PlayerCarryID_None;
                
                PlayAudio( AppState, AudioID_Sound_Camper_Save, 0.5f );
            }
            
            if( TextBoxS->Show ) {
                UpdateTextBox( AppState, dT );
            } else if( !Replay->Show ) {
                UpdateEvents( Platform, AppState );
                
                UpdatePlayerInput( AppState, Controller, dT );
                UpdatePlayer( AppState, dT );
                UpdatePlayerHitScan( AppState );
                
                CaptureReplay( AppState );
                
                UpdateCamper          ( AppState, dT );
                UpdateRunner          ( AppState, dT );
                UpdatePuncher         ( AppState, dT );
                UpdatePopper          ( AppState, dT );
                UpdateSpotter         ( AppState, dT );
                UpdateHedgehog        ( AppState, dT );
                UpdateBoulder         ( AppState, dT );
                UpdateExerciseBall    ( AppState, dT );
                UpdateBarbell         ( AppState, dT );
                UpdateMoney           ( AppState, dT );
                UpdateStomper         ( AppState, dT );
                UpdateCycleBlock      ( AppState, dT );
                UpdatePushBlock       ( AppState, dT );
                UpdateBreakBlock      ( AppState, dT );
                UpdateCollapsePlatform( AppState, dT );
                
                UpdateExerciseMiniBoss01( AppState, dT );
                UpdateExerciseMiniBoss02( AppState, dT );
                UpdateExerciseMiniBoss03( AppState, dT );
                UpdateExerciseBoss      ( AppState, dT );
                
                UpdateConveyor  ( AppState, dT );
                //UpdateGrenade   ( AppState, dT );
                UpdateParticle  ( AppState, dT );
                UpdateCheckpoint( AppState, dT );
                
                { // update Camera
                    rect Bound = AppState->Camera_Bound;
                    
                    if( Panel->Show ) {
                        // TODO: check if Camera_Dim is larger than Terrain_Dim
                        flo32 Width = GetWidth( Panel->Bound ) / AppState->App_Dim.x * Camera->Dim.x;
                        
                        Bound.Right += Width;
                    }
                    
                    vec2 PlayerP = GetPlayerCameraP( *Player );
                    vec2 CameraP = ClampToBound( PlayerP, Bound );
                    
                    Camera->Pos = CameraP;
                }
                
            }
            
            DRAW_STATE  _Draw = {};
            DRAW_STATE * Draw = &_Draw;
            Draw->Debug_iEntity = AppState->Debug_iEntity;
            
            if( Replay->Show ) {
                if( !Replay->Show_holdFrame ) {
                    flo32 t = Clamp01( Mouse->Position.x / AppState->App_Dim.x );
                    Replay->Show_atFrame = ( ( uint32 )( t * ( flo32 )REPLAY_FRAME_MAX_COUNT ) + Replay->atFrame ) % REPLAY_FRAME_MAX_COUNT;
                    DISPLAY_VALUE( uint32, Replay->Show_atFrame );
                }
                
                if( WasPressed( Mouse, MouseButton_Right ) ) {
                    Replay->Show_holdFrame = !Replay->Show_holdFrame;
                }
                
                REPLAY_FRAME * Frame = Replay->Frame + Replay->Show_atFrame;
                Draw->Game              = &Frame->Game;
                Draw->Player            = &Frame->Player;
                Draw->CamperS           = &Frame->CamperS;
                Draw->Grenade           = &Frame->Grenade;
                Draw->RunnerS           = &Frame->RunnerS;
                Draw->PuncherS          = &Frame->PuncherS;
                Draw->PopperS           = &Frame->PopperS;
                Draw->SpotterS          = &Frame->SpotterS;
                Draw->HedgehogS         = &Frame->HedgehogS;
                Draw->SeesawS           = &Frame->SeesawS;
                Draw->BoulderS          = &Frame->BoulderS;
                Draw->ExerciseBallS     = &Frame->ExerciseBallS;
                Draw->BarbellS          = &Frame->BarbellS;
                Draw->MoneyS            = &Frame->MoneyS;
                Draw->StomperS          = &Frame->StomperS;
                Draw->CycleBlockS       = &Frame->CycleBlockS;
                Draw->PushBlockS        = &Frame->PushBlockS;
                Draw->BreakBlockS       = &Frame->BreakBlockS;
                Draw->CollapsePlatformS = &Frame->CollapsePlatformS;
                
                Draw->ExerciseMiniBoss01 = &Frame->ExerciseMiniBoss01;
                Draw->ExerciseMiniBoss02 = &Frame->ExerciseMiniBoss02;
                Draw->ExerciseMiniBoss03 = &Frame->ExerciseMiniBoss03;
                Draw->ExerciseBoss       = &Frame->ExerciseBoss;
            } else {
                Draw->Game              = &AppState->Game;
                Draw->Player            = &AppState->Player;
                Draw->CamperS           = &AppState->CamperS;
                Draw->Grenade           = &AppState->Grenade;
                Draw->RunnerS           = &AppState->RunnerS;
                Draw->PuncherS          = &AppState->PuncherS;
                Draw->SpotterS          = &AppState->SpotterS;
                Draw->PopperS           = &AppState->PopperS;
                Draw->HedgehogS         = &AppState->HedgehogS;
                Draw->SeesawS           = &AppState->SeesawS;
                Draw->BoulderS          = &AppState->BoulderS;
                Draw->ExerciseBallS     = &AppState->ExerciseBallS;
                Draw->BarbellS          = &AppState->BarbellS;
                Draw->MoneyS            = &AppState->MoneyS;
                Draw->StomperS          = &AppState->StomperS;
                Draw->CycleBlockS       = &AppState->CycleBlockS;
                Draw->PushBlockS        = &AppState->PushBlockS;
                Draw->BreakBlockS       = &AppState->BreakBlockS;
                Draw->CollapsePlatformS = &AppState->CollapsePlatformS;
                
                Draw->ExerciseMiniBoss01 = &AppState->ExerciseMiniBoss01;
                Draw->ExerciseMiniBoss02 = &AppState->ExerciseMiniBoss02;
                Draw->ExerciseMiniBoss03 = &AppState->ExerciseMiniBoss03;
                Draw->ExerciseBoss       = &AppState->ExerciseBoss;
            }
            
            if( WasPressed( Keyboard, KeyCode_e, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                Replay->Show = !Replay->Show;
                Replay->Show_holdFrame = false;
            }
            if( WasPressed( Controller, ControllerButton_Start ) ) {
                if( Replay->Show ) {
                    Replay->Show = false;
                    Panel->Show  = false;
                } else {
                    Replay->Show = true;
                    Panel->Show  = true;
                }
            }
            
            DrawTerrain         ( Pass_Game, AppState );
            
            DrawExerciseMiniBoss01( Pass_Game, AppState, Draw );
            DrawExerciseMiniBoss02( Pass_Game, Pass_UI, AppState, Draw );
            DrawExerciseMiniBoss03( Pass_Game, Pass_UI, AppState, Draw );
            DrawExerciseBoss      ( Pass_Game, Pass_UI, AppState, Draw );
            
            DrawScaffold        ( Pass_Game, AppState );
            DrawConveyor        ( Pass_Game, AppState );
            DrawCheckpoint      ( Pass_Game, AppState );
            DrawSpikes          ( Pass_Game, AppState );
            DrawSeesaw          ( Pass_Game, AppState, Draw );
            DrawPushBlock       ( Pass_Game, AppState, Draw );
            DrawBreakBlock      ( Pass_Game, AppState, Draw );
            DrawCycleBlock      ( Pass_Game, AppState, Draw );
            DrawCollapsePlatform( Pass_Game, AppState, Draw );
            DrawStomper         ( Pass_Game, AppState, Draw );
            DrawRunner          ( Pass_Game, AppState, Draw );
            DrawPuncher         ( Pass_Game, AppState, Draw );
            DrawPopper          ( Pass_Game, AppState, Draw );
            DrawSpotter         ( Pass_Game, AppState, Draw );
            DrawHedgehog        ( Pass_Game, AppState, Draw );
            
            DrawBoulder     ( Pass_Game, AppState, Draw );
            DrawExerciseBall( Pass_Game, AppState, Draw );
            DrawBarbell     ( Pass_Game, AppState, Draw );
            DrawMoney       ( Pass_Game, AppState, Draw );
            DrawCamper      ( Pass_Game, AppState, Draw );
            DrawPlayer      ( Pass_Game, Pass_UI, AppState, Draw );
            //DrawGrenade   ( Pass_Game, AppState, Draw );
            DrawParticle    ( Pass_Game, AppState );
            
            LEVEL_STATS Stat = AppState->Stat;
            DrawRectOutline( Pass_Game, Stat.Bound, COLOR_RED );
            
            DrawTextBox( Pass_UI, AppState );
            if( ( !TextBoxS->Show ) && ( !Replay->Show ) ) {
                FinalizePlayer          ( AppState );
                FinalizeCamper          ( AppState );
                //FinalizeGrenade         ( AppState );
                FinalizeScaffold        ( AppState );
                FinalizeRunner          ( AppState );
                FinalizePuncher         ( AppState );
                FinalizePopper          ( AppState );
                FinalizeSpotter         ( AppState );
                FinalizeHedgehog        ( AppState );
                FinalizeStomper         ( AppState );
                FinalizeCycleBlock      ( AppState );
                FinalizePushBlock       ( AppState );
                FinalizeBreakBlock      ( AppState );
                FinalizeBoulder         ( AppState );
                FinalizeExerciseBall    ( AppState );
                FinalizeBarbell         ( AppState );
                FinalizeMoney           ( AppState );
                FinalizeCollapsePlatform( AppState );
                
                FinalizeExerciseMiniBoss01( AppState );
                FinalizeExerciseMiniBoss02( AppState );
                FinalizeExerciseMiniBoss03( AppState );
                FinalizeExerciseBoss      ( AppState );
                
                FinalizeCheckpoint( AppState );
                FinalizeSeesaw    ( AppState );
                FinalizeParticle  ( AppState );
                FinalizeLevelTransition( AppState );
                UpdateDebugLevelNavigation( Platform, AppState, TempMemory );
                
#if 0                
                if( AppState->DoSaveGame ) {
                    AppState->DoSaveGame = false;
                    SaveGame( AppState, TempMemory );
                    
                    AppState->DoRecentSave = true;
                }
                if( AppState->DoRecentSave ) {
                    flo32 TargetTime = ( 1.0f / 60.0f );
                    if( AppState->RecentSave_Timer < TargetTime ) {
                        DrawRect( Pass_UI, AppState->App_Bound, COLOR_WHITE );
                    }
                    
                    AppState->RecentSave_Timer += dT;
                    if( AppState->RecentSave_Timer >= 3.0f ) {
                        AppState->DoRecentSave       = false;
                        AppState->RecentSave_Timer = 0.0f;
                    }
                    
                    vec2 P = GetT( AppState->App_Bound, Vec2( 0.5f, 0.1f ) );
                    DrawString( Pass_UI, AppState->Font, "Game Saved.", P, TextAlign_Center, Vec2( 2.0f, 2.0f ), COLOR_WHITE );
                }
#endif
                
                { // LOAD FROM CHECKPOINT
                    if( ( Player->IsDead ) && ( Player->IsDead_Timer >= 1.0f ) ) {
                        vec2 P = GetT( AppState->App_Bound, Vec2( 0.45f, 0.1f ) );
                        
                        vec2 Dim = Vec2( 36.0f, 36.0f );
                        rect R   = RectRCD( P + Vec2( -10.0f, 0.0f ), Dim );
                        DrawRect       ( Pass_UI, R, COLOR_GRAY( 0.02f ) );
                        DrawRectOutline( Pass_UI, R, COLOR_GRAY( 0.1f  ) );
                        
                        if( AppState->LoadCheckpoint_IsActive ) {
                            AppState->LoadCheckpoint_Timer += dT;
                            
                            flo32 t = Clamp01( AppState->LoadCheckpoint_Timer / LOAD_CHECKPOINT_TARGET_TIME );
                            rect  S = R;
                            S.Top = lerp( S.Bottom, t, S.Top );
                            
                            DrawRect( Pass_UI, S, COLOR_YELLOW );
                        } else {
                            AppState->LoadCheckpoint_Timer = 0.0f;
                        }
                        
                        DrawString( Pass_UI, AppState->Font, ": Load Checkpoint", P, TextAlign_CenterLeft, Vec2( 2.0f, 2.0f ), COLOR_WHITE );
                        
                        if( AppState->LoadCheckpoint_Timer >= LOAD_CHECKPOINT_TARGET_TIME ) {
                            AppState->DoLoadCheckpoint = true;
                            
                            AppState->LoadCheckpoint_Timer    = 0.0f;
                            AppState->LoadCheckpoint_IsActive = false;
                        }
                    }
                }
                if( WasPressed( Keyboard, KeyCode_c, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                    AppState->DoLoadCheckpoint = true;
                }
                if( AppState->DoLoadCheckpoint ) {
                    AppState->DoLoadCheckpoint = false;
                    LoadCheckpoint( Platform, AppState, TempMemory );
                }
                
                if( AppState->DoLevelTransition ) {
                    ToNextLevel( Platform, AppState, TempMemory );
                }
            }
            
            FinalizeTextBox( Controller, AppState );
            
            if( WasPressed( Keyboard, KeyCode_r, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                StartGame( Platform, AppState, TempMemory );
            }
            if( WasPressed( Controller, ControllerButton_Back ) ) {
                StartGame( Platform, AppState, TempMemory );
            }
            if( WasPressed( Keyboard, KeyCode_l, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                AppState->Mode = AppMode_Editor;
            }
            
            UpdateAndDrawDebugPanel( AppState, Mouse, Pass_Game, Pass_UI, Draw );
            if( WasPressed( Keyboard, KeyCode_d, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                Panel->Show = !Panel->Show;
            }
            
            DrawPoint( Pass_Game, AppState->LastJump, TILE_DIM * 0.05f, COLOR_YELLOW );
        } break;
        
        case AppMode_Editor: {
            EDITOR_STATE   * Editor   = &AppState->Editor;
            CAMERA_STATE   * Camera   = &Editor->Camera;
            FILE_SAVE_OPEN * SaveOpen = &Editor->SaveOpen;
            PANEL          * Panel    = &Editor->Panel;
            
            if( !Editor->IsInitialized ) {
                Editor->IsInitialized = true;
                
                SaveOpen->FileList  = StringBuffer( PermMemory, 4096, _KB( 256 ) );
                Editor->Event_Names = StringBuffer( PermMemory, 1024, _KB(  32 ) );
                
                InitEditorPanel( AppState );
                ResetEditor( Editor, AppState );
            }
            
            UpdateCamera( Camera, Keyboard, dT );
            vec2 MouseP = GetWorldPos( Camera, AppState->App_Dim, Mouse->Position );
            DISPLAY_VALUE( vec2, MouseP );
            
            { // Draw cursor Cell
                UINT32_PAIR Cell = GetCell( Camera, AppState->App_Dim, Mouse->Position );
                DISPLAY_VALUE( uint32, Cell.x );
                DISPLAY_VALUE( uint32, Cell.y );
                
                rect R = GetCellRectFromIndex( Cell.x, Cell.y );
                DrawRectOutline( Pass_Game, R, COLOR_YELLOW );
            }
            
            { // Draw Grid Bound
                vec2 Grid_Dim = Vec2( ( flo32 )EDITOR__GRID_MAX_WIDTH, ( flo32 )EDITOR__GRID_MAX_HEIGHT ) * TILE_DIM;
                rect Grid_Bound = RectBLD( Vec2( 0.0f, 0.0f ), Grid_Dim );
                DrawRectOutline( Pass_Game, Grid_Bound, COLOR_RED );
            }
            
            rect view_Bound  = AppState->App_Bound;
            if( Panel->Show ) {
                view_Bound.Right = Editor->Panel.Bound.Left;
            }
            
            DISPLAY_VALUE( rect, Editor->Stat.Bound );
            
            if( Editor->DrawGrid ) {
                DrawEditorGrid( Pass_Game, AppState );
            }
            
            boo32 CanSaveOpen = true;
            if( IsInBound( Mouse->Position, view_Bound ) ) {
                boo32 Mouse_IsActive = IsDown( Mouse, MouseButton_Left  );
                boo32 Erase_IsActive = IsDown( Mouse, MouseButton_Right );
                
                if( ( Mouse_IsActive ) || ( Erase_IsActive ) ) {
                    CanSaveOpen = false;
                    SaveOpen->UnsavedChanges = true;
                }
                
                switch( Editor->Mode ) {
                    case EditorMode_Default: {
                        
                    } break;
                    
                    case EditorMode_MoveEntity: {
                        // set bound : No Hover, Left Click -> Drag Box
                        // move      : Hover, Left Click -> Drag
                        
                        LEVEL_STATS * Stat = &Editor->Stat;
                        
                        if( WasPressed( Mouse, MouseButton_Left ) ) {
                            if( ( IsInBound( MouseP, Editor->Move_SrcBound ) ) ) {
                                Editor->Move_DoMouse  = true;
                                Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                            } else {
                                Editor->Mouse_DoMouse = true;
                                Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                            }
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            Editor->Mouse_TempBound = Bound;
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                Editor->Move_SrcBound = Bound;
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        if( Editor->Move_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            int32 dX = ( int32 )iCell.x - ( int32 )Editor->Mouse_iCell.x;
                            int32 dY = ( int32 )iCell.y - ( int32 )Editor->Mouse_iCell.y;
                            
                            vec2 Offset = Vec2( ( flo32 )dX, ( flo32 )dY ) * TILE_DIM;
                            rect Bound  = AddOffset( Editor->Move_SrcBound, Offset );
                            
                            Editor->Move_DestBound = Bound;
                            
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Move_DoMouse = false;
                                
                                Editor_MoveTerrain( Editor, TempMemory, Editor->Move_SrcBound, Editor->Move_DestBound );
                                Editor_MoveCycleBlock      ( Editor, Editor->Move_SrcBound, Offset );
                                Editor_MoveBreakBlock      ( Editor, Editor->Move_SrcBound, Offset );
                                Editor_MovePushBlock       ( Editor, Editor->Move_SrcBound, Offset );
                                Editor_MoveCheckpoint      ( Editor, Editor->Move_SrcBound, Offset );
                                Editor_MoveSpikes          ( Editor, Editor->Move_SrcBound, Offset );
                                Editor_MoveConveyor        ( Editor, Editor->Move_SrcBound, Offset );
                                Editor_MoveCamper          ( Editor, Editor->Move_SrcBound, Offset );
                                Editor_MoveCollapsePlatform( Editor, Editor->Move_SrcBound, Offset );
                                Editor_MoveMoney           ( Editor, Editor->Move_SrcBound, Offset );
                                Editor_MovePuncher         ( Editor, Editor->Move_SrcBound, Offset );
                                
                                Editor->Move_SrcBound = Editor->Move_DestBound;
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                    } break;
                    
                    case EditorMode_CopyEntity: {
                        // set bound  : Left Click -> Drag Box
                        // paste copy : Right Click
                        
                        LEVEL_STATS * Stat = &Editor->Stat;
                        
                        if( WasPressed( Mouse, MouseButton_Left ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            Editor->Mouse_TempBound = Bound;
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                Editor->Copy_SrcBound = Bound;
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        
                        UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                        
                        int32 dX = ( int32 )iCell.x - ( int32 )Editor->Mouse_iCell.x;
                        int32 dY = ( int32 )iCell.y - ( int32 )Editor->Mouse_iCell.y;
                        
                        vec2 Offset = Vec2( ( flo32 )dX, ( flo32 )dY ) * TILE_DIM;
                        rect Bound  = AddOffset( Editor->Copy_SrcBound, Offset );
                        Editor->Copy_DestBound = Bound;
                        
                        if( WasPressed( Mouse, MouseButton_Right ) ) {
                            CopyTerrain( Editor, TempMemory, Editor->Copy_SrcBound, Editor->Copy_DestBound );
                            
                            Editor->Mouse_iCell   = iCell;
                            Editor->Copy_SrcBound = Bound;
                        }
                    } break;
                    
                    case EditorMode_MirrorEntity: {
                        // set bound  : Left Click -> Drag Box
                        // mirror entities : Right Click
                        
                        LEVEL_STATS * Stat = &Editor->Stat;
                        
                        if( WasPressed( Mouse, MouseButton_Left ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            Editor->Mouse_TempBound = Bound;
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                Editor->Mirror_SrcBound = Bound;
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        
                        if( WasPressed( Mouse, MouseButton_Right ) ) {
                            Editor_MirrorTerrain         ( Editor, TempMemory, Editor->Mirror_SrcBound );
                            Editor_MirrorCycleBlock      ( Editor, Editor->Mirror_SrcBound );
                            Editor_MirrorBreakBlock      ( Editor, Editor->Mirror_SrcBound );
                            Editor_MirrorPushBlock       ( Editor, Editor->Mirror_SrcBound );
                            Editor_MirrorCheckpoint      ( Editor, Editor->Mirror_SrcBound );
                            Editor_MirrorSpikes          ( Editor, Editor->Mirror_SrcBound );
                            Editor_MirrorConveyor        ( Editor, Editor->Mirror_SrcBound );
                            Editor_MirrorCamper          ( Editor, Editor->Mirror_SrcBound );
                            Editor_MirrorCollapsePlatform( Editor, Editor->Mirror_SrcBound );
                            Editor_MirrorMoney           ( Editor, Editor->Mirror_SrcBound );
                            Editor_MirrorPuncher         ( Editor, Editor->Mirror_SrcBound );
                        }
                    } break;
                    
                    case EditorMode_LevelBound: {
                        // set : Left Click -> Drag Box
                        // set left entrance  : Hover + L_Key
                        // set right entrance : Hover + R_Key
                        
                        LEVEL_STATS * Stat = &Editor->Stat;
                        
                        if( WasPressed( Mouse, MouseButton_Left ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        if( WasPressed( Keyboard, KeyCode_l ) ) {
                            rect R = GetCellRectFromPos( MouseP );
                            Stat->EnterLeftY = R.Bottom;
                        }
                        if( WasPressed( Keyboard, KeyCode_r ) ) {
                            rect R = GetCellRectFromPos( MouseP );
                            Stat->EnterRightY = R.Bottom;
                        }
                        if( WasPressed( Keyboard, KeyCode_b ) ) {
                            rect R = GetCellRectFromPos( MouseP );
                            Stat->EnterBottomX = R.Left;
                        }
                        if( WasPressed( Keyboard, KeyCode_t ) ) {
                            rect R = GetCellRectFromPos( MouseP );
                            Stat->EnterTopX = R.Left;
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                Stat->Bound = Bound;
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                    } break;
                    
                    case EditorMode_Terrain: {
                        boo32 DrawGuide = false;
                        if( Editor->Terrain_DoPlace ) {
                            DrawGuide = true;
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                UINT32_PAIR iCell = GetCellIndexFromPos( MouseP ) ;
                                UINT32_QUAD Bound = getExtrema( iCell, Editor->Mouse_iCell );
                                
                                for( uint32 y = Bound.min.y; y <= Bound.max.y; y++ ) {
                                    for( uint32 x = Bound.min.x; x <= Bound.max.x; x++ ) {
                                        Editor->Grid[ y ][ x ] = CellType_Terrain;
                                    }
                                }
                                Editor->Terrain_DoPlace = false;
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Terrain_DoPlace = false;
                            }
                        } else if( Editor->Terrain_DoErase ) {
                            DrawGuide = true;
                            if( WasReleased( Mouse, MouseButton_Right ) ) {
                                UINT32_PAIR iCell = GetCellIndexFromPos( MouseP ) ;
                                UINT32_QUAD Bound = getExtrema( iCell, Editor->Mouse_iCell );
                                
                                for( uint32 y = Bound.min.y; y <= Bound.max.y; y++ ) {
                                    for( uint32 x = Bound.min.x; x <= Bound.max.x; x++ ) {
                                        Editor->Grid[ y ][ x ] = CellType_Empty;
                                    }
                                }
                                Editor->Terrain_DoErase = false;
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Terrain_DoErase = false;
                            }
                        } else {
                            if( WasPressed( Mouse, MouseButton_Left ) ) {
                                Editor->Terrain_DoPlace = true;
                                Editor->Mouse_iCell     = GetCellIndexFromPos( MouseP );
                            } else if( WasPressed( Mouse, MouseButton_Right ) ) {
                                Editor->Terrain_DoErase = true;
                                Editor->Mouse_iCell     = GetCellIndexFromPos( MouseP );
                            }
                        }
                        
                        if( DrawGuide ) {
                            UINT32_PAIR iCellA = Editor->Mouse_iCell;
                            UINT32_PAIR iCellB = GetCellIndexFromPos( MouseP );
                            uint32 x = abs( ( int32 )iCellA.x - ( int32 )iCellB.x ) + 1;
                            uint32 y = abs( ( int32 )iCellA.y - ( int32 )iCellB.y ) + 1;
                            
                            UINT32_QUAD Bound = getExtrema( iCellA, iCellB );
                            rect        R     = GetBound( Bound );
                            DrawRectOutline( Pass_Game, R, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            vec2 P = GetCellTL( iCellB );
                            char str[ 128 ] = {};
                            sprintf( str, "%u,%u", x, y );
                            DrawString( Pass_Game, AppState->Font, str, P, TextAlign_BottomLeft, WORLD_DEBUG_TEXT_SCALE * Camera->Scale_Ratio, COLOR_YELLOW );
                        }
                    } break;
                    
                    case EditorMode_Spikes: {
                        // add new     : Left Click + No Hover
                        // change type : Hover + Mouse Wheel
                        // delete      : Hover + Delete
                        
                        EDITOR__SPIKES_STATE * SpikesS = &Editor->SpikesS;
                        
                        int32 iHover = -1;
                        for( uint32 iSpikes = 0; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
                            EDITOR__SPIKES Spikes = SpikesS->Spikes[ iSpikes ];
                            if( IsInBound( MouseP, Spikes.Bound ) ) {
                                iHover = iSpikes;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( SpikesS->nSpikes < SPIKES_MAX_COUNT ) {
                                EDITOR__SPIKES Spikes = {};
                                Spikes.Type  = SpikesS->ActiveType;
                                Spikes.Bound = GetCellRectFromPos( MouseP );
                                
                                SpikesS->Spikes[ SpikesS->nSpikes++ ] = Spikes;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new SPIKES, but the state is full! Max Count = %u", SPIKES_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            SpikesS->nSpikes--;
                            for( uint32 iSpikes = iHover; iSpikes < SpikesS->nSpikes; iSpikes++ ) {
                                SpikesS->Spikes[ iSpikes ] = SpikesS->Spikes[ iSpikes + 1 ];
                            }
                        }
                        if( ( iHover > -1 ) && ( Mouse->WheelClicks != 0 ) ) {
                            EDITOR__SPIKES * Spikes = SpikesS->Spikes + iHover;
                            Spikes->Type =  ( SPIKES_TYPE )UInt32Wrap( Spikes->Type, Mouse->WheelClicks, SpikesType_Count );
                            
                            SpikesS->ActiveType = Spikes->Type;
                        }
                    } break;
                    
                    case EditorMode_Conveyor: {
                        // add new : Left Click + No Hover -> Drag Box
                        // delete  : Hover + Delete
                        
                        EDITOR__CONVEYOR_STATE * ConveyorS = &Editor->ConveyorS;
                        
                        int32 iHover = -1;
                        for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
                            EDITOR__CONVEYOR Conveyor = ConveyorS->Conveyor[ iConveyor ];
                            if( IsInBound( MouseP, Conveyor.Bound ) ) {
                                iHover = iConveyor;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            ConveyorS->nConveyor--;
                            for( uint32 iConveyor = iHover; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
                                ConveyorS->Conveyor[ iConveyor ] = ConveyorS->Conveyor[ iConveyor + 1 ];
                            }
                        }
                        if( ( iHover > -1 ) && ( Mouse->WheelClicks != 0 ) ) {
                            EDITOR__CONVEYOR * Conveyor = ConveyorS->Conveyor + iHover;
                            Conveyor->Type = ( EDITOR__CONVEYOR_TYPE )( ( Conveyor->Type + 1 ) % EditorConveyorType_Count );
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                
                                if( ConveyorS->nConveyor < CONVEYOR_MAX_COUNT ) {
                                    EDITOR__CONVEYOR Conveyor = {};
                                    Conveyor.Bound = Bound;
                                    Conveyor.Type  = EditorConveyorType_HorizontalMoveLeft;
                                    
                                    ConveyorS->Conveyor[ ConveyorS->nConveyor++ ] = Conveyor;
                                } else {
                                    GlobalVar_DebugSystem.ErrorOccurred = true;
                                    char str[ 512 ] = {};
                                    sprintf( str, "ERROR! Attempted to create new CONVEYOR, but the state is full! Max Count = %u", CONVEYOR_MAX_COUNT );
                                    CONSOLE_STRING( str );
                                }
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        DISPLAY_VALUE( uint32, ConveyorS->nConveyor );
                    } break;
                    
                    case EditorMode_Scaffold: {
                        // add new : Left Click + No Hover -> Drag Box
                        // delete  : Hover + Delete
                        
                        EDITOR__SCAFFOLD_STATE * ScaffoldS = &Editor->ScaffoldS;
                        
                        int32 iHover = -1;
                        for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
                            EDITOR__SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
                            if( IsInBound( MouseP, Scaffold.Bound ) ) {
                                iHover = iScaffold;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            ScaffoldS->nScaffold--;
                            for( uint32 iScaffold = iHover; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
                                ScaffoldS->Scaffold[ iScaffold ] = ScaffoldS->Scaffold[ iScaffold + 1 ];
                            }
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                
                                if( ScaffoldS->nScaffold < SCAFFOLD_MAX_COUNT ) {
                                    EDITOR__SCAFFOLD Scaffold = {};
                                    Scaffold.Bound = Bound;
                                    
                                    ScaffoldS->Scaffold[ ScaffoldS->nScaffold++ ] = Scaffold;
                                } else {
                                    GlobalVar_DebugSystem.ErrorOccurred = true;
                                    char str[ 512 ] = {};
                                    sprintf( str, "ERROR! Attempted to create new SCAFFOLD, but the state is full! Max Count = %u", SCAFFOLD_MAX_COUNT );
                                    CONSOLE_STRING( str );
                                }
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        DISPLAY_VALUE( uint32, ScaffoldS->nScaffold );
                    } break;
                    
                    case EditorMode_Seesaw: {
                        // add new  : Left click + no hover
                        // delete   : hover + delete
                        // change order : hover + MouseWheel
                        EDITOR__SEESAW_STATE * SeesawS = &Editor->SeesawS;
                        
                        int32 iHover = -1;
                        for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
                            EDITOR__SEESAW Seesaw = SeesawS->Seesaw[ iSeesaw ];
                            if( IsInBound( MouseP, Seesaw.Bound ) ) {
                                iHover = iSeesaw;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( SeesawS->nSeesaw < SEESAW_MAX_COUNT ) {
                                rect R = GetCellRectFromPos( MouseP );
                                R.Right += ( TILE_WIDTH * 3.0f );
                                
                                EDITOR__SEESAW Seesaw = {};
                                Seesaw.Bound = R;
                                
                                SeesawS->Seesaw[ SeesawS->nSeesaw++ ] = Seesaw;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new SEESAW, but the state is full! Max Count = %u", SEESAW_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            SeesawS->nSeesaw--;
                            for( uint32 iSeesaw = iHover; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
                                SeesawS->Seesaw[ iSeesaw ] = SeesawS->Seesaw[ iSeesaw + 1 ];
                            }
                        }
                    } break;
                    
                    case EditorMode_CycleBlock: {
                        // add new  : No Hover, Left Click -> Drag Box
                        // set dest : Hover, Up/Down to Inc/Dec Dest by 1 Tile
                        // delete   : Hover + Delete
                        
                        EDITOR__CYCLE_BLOCK_STATE * CycleBlockS = &Editor->CycleBlockS;
                        
                        int32 iHover = -1;
                        for( uint32 iCycleBlock = 0; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
                            EDITOR__CYCLE_BLOCK CycleBlock = CycleBlockS->CycleBlock[ iCycleBlock ];
                            if( IsInBound( MouseP, CycleBlock.Bound ) ) {
                                iHover = iCycleBlock;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            CycleBlockS->nCycleBlock--;
                            for( uint32 iCycleBlock = iHover; iCycleBlock < CycleBlockS->nCycleBlock; iCycleBlock++ ) {
                                CycleBlockS->CycleBlock[ iCycleBlock ] = CycleBlockS->CycleBlock[ iCycleBlock + 1 ];
                            }
                        }
                        if( iHover > -1 ) {
                            EDITOR__CYCLE_BLOCK * CycleBlock = CycleBlockS->CycleBlock + iHover;
                            
                            if( WasPressed( Keyboard, KeyCode_Up ) ) {
                                CycleBlock->Dest.y += ( TILE_HEIGHT * 1.0f );
                            }
                            if( WasPressed( Keyboard, KeyCode_Down ) ) {
                                CycleBlock->Dest.y -= ( TILE_HEIGHT * 1.0f );
                            }
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                
                                if( CycleBlockS->nCycleBlock < CYCLE_BLOCK_MAX_COUNT ) {
                                    EDITOR__CYCLE_BLOCK CycleBlock = {};
                                    CycleBlock.Bound = Bound;
                                    CycleBlock.Dest  = GetCenter( Bound );
                                    
                                    CycleBlockS->CycleBlock[ CycleBlockS->nCycleBlock++ ] = CycleBlock;
                                } else {
                                    GlobalVar_DebugSystem.ErrorOccurred = true;
                                    char str[ 512 ] = {};
                                    sprintf( str, "ERROR! Attempted to create new CYCLE_BLOCK, but the state is full! Max Count = %u", CYCLE_BLOCK_MAX_COUNT );
                                    CONSOLE_STRING( str );
                                }
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        DISPLAY_VALUE( uint32, CycleBlockS->nCycleBlock );
                    } break;
                    
                    case EditorMode_PushBlock: {
                        // add new  : No Hover, Left Click -> Drag Box
                        // delete   : Hover + Delete
                        
                        EDITOR__PUSH_BLOCK_STATE * PushBlockS = &Editor->PushBlockS;
                        
                        int32 iHover = -1;
                        for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
                            EDITOR__PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
                            if( IsInBound( MouseP, PushBlock.Bound ) ) {
                                iHover = iPushBlock;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            PushBlockS->nPushBlock--;
                            for( uint32 iPushBlock = iHover; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
                                PushBlockS->PushBlock[ iPushBlock ] = PushBlockS->PushBlock[ iPushBlock + 1 ];
                            }
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                
                                if( PushBlockS->nPushBlock < PUSH_BLOCK_MAX_COUNT ) {
                                    EDITOR__PUSH_BLOCK PushBlock = {};
                                    PushBlock.Bound = Bound;
                                    
                                    PushBlockS->PushBlock[ PushBlockS->nPushBlock++ ] = PushBlock;
                                } else {
                                    GlobalVar_DebugSystem.ErrorOccurred = true;
                                    char str[ 512 ] = {};
                                    sprintf( str, "ERROR! Attempted to create new PUSH_BLOCK, but the state is full! Max Count = %u", PUSH_BLOCK_MAX_COUNT );
                                    CONSOLE_STRING( str );
                                }
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        DISPLAY_VALUE( uint32, PushBlockS->nPushBlock );
                    } break;
                    
                    case EditorMode_BreakBlock: {
                        // add new  : No Hover, Left Click -> Drag Box
                        // delete   : Hover + Delete
                        
                        EDITOR__BREAK_BLOCK_STATE * BreakBlockS = &Editor->BreakBlockS;
                        
                        int32 iHover = -1;
                        for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
                            EDITOR__BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
                            if( IsInBound( MouseP, BreakBlock.Bound ) ) {
                                iHover = iBreakBlock;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            BreakBlockS->nBreakBlock--;
                            for( uint32 iBreakBlock = iHover; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
                                BreakBlockS->BreakBlock[ iBreakBlock ] = BreakBlockS->BreakBlock[ iBreakBlock + 1 ];
                            }
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                
                                if( BreakBlockS->nBreakBlock < BREAK_BLOCK_MAX_COUNT ) {
                                    EDITOR__BREAK_BLOCK BreakBlock = {};
                                    BreakBlock.Bound = Bound;
                                    
                                    BreakBlockS->BreakBlock[ BreakBlockS->nBreakBlock++ ] = BreakBlock;
                                } else {
                                    GlobalVar_DebugSystem.ErrorOccurred = true;
                                    char str[ 512 ] = {};
                                    sprintf( str, "ERROR! Attempted to create new BREAK_BLOCK, but the state is full! Max Count = %u", BREAK_BLOCK_MAX_COUNT );
                                    CONSOLE_STRING( str );
                                }
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        DISPLAY_VALUE( uint32, BreakBlockS->nBreakBlock );
                    } break;
                    
                    case EditorMode_Boulder: {
                        // add new  : No Hover, Left Click
                        // delete   : Hover + Delete
                        
                        EDITOR__BOULDER_STATE * BoulderS = &Editor->BoulderS;
                        
                        int32 iHover = -1;
                        for( uint32 iBoulder = 0; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
                            EDITOR__BOULDER Boulder = BoulderS->Boulder[ iBoulder ];
                            rect R = RectCD( Boulder.Position, BOULDER_DIM );
                            if( IsInBound( MouseP, R ) ) {
                                iHover = iBoulder;
                            }
                        }
                        
                        flo32 Scale    = TILE_WIDTH * 0.5f;
                        vec2  ScalePos = Vec2(
                                              ( flo32 )( ( uint32 )( ( MouseP.x / Scale ) + 0.5f ) ) * Scale, ( flo32 )( ( uint32 )( ( MouseP.y / Scale ) + 0.5f ) ) * Scale );
                        DrawPoint( Pass_Game, ScalePos, TILE_DIM * 0.1f, COLOR_YELLOW );
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( BoulderS->nBoulder < BOULDER_MAX_COUNT ) {
                                
                                EDITOR__BOULDER Boulder = {};
                                Boulder.Position = ScalePos;
                                
                                BoulderS->Boulder[ BoulderS->nBoulder++ ] = Boulder;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new BOULDER, but the state is full! Max Count = %u", BOULDER_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            BoulderS->nBoulder--;
                            for( uint32 iBoulder = iHover; iBoulder < BoulderS->nBoulder; iBoulder++ ) {
                                BoulderS->Boulder[ iBoulder ] = BoulderS->Boulder[ iBoulder + 1 ];
                            }
                        }
                    } break;
                    
                    case EditorMode_ExerciseBall: {
                        // add new  : No Hover, Left Click
                        // delete   : Hover + Delete
                        
                        EDITOR__EXERCISE_BALL_STATE * ExerciseBallS = &Editor->ExerciseBallS;
                        
                        int32 iHover = -1;
                        for( uint32 iExerciseBall = 0; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
                            EDITOR__EXERCISE_BALL ExerciseBall = ExerciseBallS->ExerciseBall[ iExerciseBall ];
                            rect R = RectCD( ExerciseBall.Position, EXERCISE_BALL_DIM );
                            if( IsInBound( MouseP, R ) ) {
                                iHover = iExerciseBall;
                            }
                        }
                        
                        flo32 Scale    = TILE_WIDTH * 0.5f;
                        vec2  ScalePos = Vec2(
                                              ( flo32 )( ( uint32 )( ( MouseP.x / Scale ) + 0.5f ) ) * Scale, ( flo32 )( ( uint32 )( ( MouseP.y / Scale ) + 0.5f ) ) * Scale );
                        DrawPoint( Pass_Game, ScalePos, TILE_DIM * 0.1f, COLOR_YELLOW );
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( ExerciseBallS->nExerciseBall < EXERCISE_BALL_MAX_COUNT ) {
                                
                                EDITOR__EXERCISE_BALL ExerciseBall = {};
                                ExerciseBall.Position = ScalePos;
                                
                                ExerciseBallS->ExerciseBall[ ExerciseBallS->nExerciseBall++ ] = ExerciseBall;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new EXERCISE_BALL, but the state is full! Max Count = %u", EXERCISE_BALL_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            ExerciseBallS->nExerciseBall--;
                            for( uint32 iExerciseBall = iHover; iExerciseBall < ExerciseBallS->nExerciseBall; iExerciseBall++ ) {
                                ExerciseBallS->ExerciseBall[ iExerciseBall ] = ExerciseBallS->ExerciseBall[ iExerciseBall + 1 ];
                            }
                        }
                    } break;
                    
                    case EditorMode_Money: {
                        // add new  : No Hover, Left Click
                        // delete   : Hover + Delete
                        
                        EDITOR__MONEY_STATE * MoneyS = &Editor->MoneyS;
                        
                        MONEY_DIMS;
                        
                        int32 iHover = -1;
                        for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
                            EDITOR__MONEY Money = MoneyS->Money[ iMoney ];
                            rect R = RectBCD( Money.Position, MoneyDims[ MoneyType_100 ]  );
                            if( IsInBound( MouseP, R ) ) {
                                iHover = iMoney;
                            }
                        }
                        
                        flo32 Scale    = TILE_WIDTH * 0.5f;
                        vec2  ScalePos = Vec2(
                                              ( flo32 )( ( uint32 )( ( MouseP.x / Scale ) + 0.5f ) ) * Scale, ( flo32 )( ( uint32 )( ( MouseP.y / Scale ) + 0.5f ) ) * Scale );
                        DrawPoint( Pass_Game, ScalePos, TILE_DIM * 0.1f, COLOR_YELLOW );
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( MoneyS->nMoney < MONEY_MAX_COUNT ) {
                                
                                EDITOR__MONEY Money = {};
                                Money.Type     = MoneyS->ActiveType;
                                Money.Position = ScalePos;
                                
                                MoneyS->Money[ MoneyS->nMoney++ ] = Money;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new MONEY, but the state is full! Max Count = %u", MONEY_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            MoneyS->nMoney--;
                            for( uint32 iMoney = iHover; iMoney < MoneyS->nMoney; iMoney++ ) {
                                MoneyS->Money[ iMoney ] = MoneyS->Money[ iMoney + 1 ];
                            }
                        }
                        if( ( iHover > -1 ) && ( Mouse->WheelClicks != 0 ) ) {
                            EDITOR__MONEY * Money = MoneyS->Money + iHover;
                            Money->Type =  ( MONEY_TYPE )UInt32Wrap( Money->Type, Mouse->WheelClicks, MoneyType_Count );
                            
                            MoneyS->ActiveType = Money->Type;
                        }
                        
                        DISPLAY_VALUE( uint32, MoneyS->nMoney );
                    } break;
                    
                    case EditorMode_CollapsePlatform: {
                        // add new  : No Hover, Left Click -> Drag Box
                        // delete   : Hover + Delete
                        
                        EDITOR__COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &Editor->CollapsePlatformS;
                        
                        int32 iHover = -1;
                        for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
                            EDITOR__COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ];
                            if( IsInBound( MouseP, CollapsePlatform.Bound ) ) {
                                iHover = iCollapsePlatform;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            CollapsePlatformS->nCollapsePlatform--;
                            for( uint32 iCollapsePlatform = iHover; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
                                CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ] = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform + 1 ];
                            }
                        }
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                
                                if( CollapsePlatformS->nCollapsePlatform < COLLAPSE_PLATFORM_MAX_COUNT ) {
                                    EDITOR__COLLAPSE_PLATFORM CollapsePlatform = {};
                                    CollapsePlatform.Bound = Bound;
                                    
                                    CollapsePlatformS->CollapsePlatform[ CollapsePlatformS->nCollapsePlatform++ ] = CollapsePlatform;
                                } else {
                                    GlobalVar_DebugSystem.ErrorOccurred = true;
                                    char str[ 512 ] = {};
                                    sprintf( str, "ERROR! Attempted to create new COLLAPSE_PLATFORM, but the state is full! Max Count = %u", COLLAPSE_PLATFORM_MAX_COUNT );
                                    CONSOLE_STRING( str );
                                }
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        DISPLAY_VALUE( uint32, CollapsePlatformS->nCollapsePlatform );
                    } break;
                    
                    case EditorMode_Puncher: {
                        // add new     : Left Click + No Hover
                        // delete      : Hover + Delete
                        
                        EDITOR__PUNCHER_STATE * PuncherS = &Editor->PuncherS;
                        
                        int32 iHover = -1;
                        for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
                            EDITOR__PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
                            rect Bound = RectBCD( Puncher.Position, PUNCHER_DIM );
                            if( IsInBound( MouseP, Bound ) ) {
                                iHover = iPuncher;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( PuncherS->nPuncher < PUNCHER_MAX_COUNT ) {
                                vec2 Pos = GetCellRectBCFromPos( MouseP );
                                
                                EDITOR__PUNCHER Puncher = {};
                                Puncher.Position = Pos;
                                
                                PuncherS->Puncher[ PuncherS->nPuncher++ ] = Puncher;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new PUNCHER, but the state is full! Max Count = %u", PUNCHER_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            PuncherS->nPuncher--;
                            for( uint32 iPuncher = iHover; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
                                PuncherS->Puncher[ iPuncher ] = PuncherS->Puncher[ iPuncher + 1 ];
                            }
                        }
                    } break;
                    
                    case EditorMode_Barbell: {
                        // add new     : Left Click + No Hover
                        // delete      : Hover + Delete
                        
                        EDITOR__BARBELL_STATE * BarbellS = &Editor->BarbellS;
                        
                        int32 iHover = -1;
                        for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
                            EDITOR__BARBELL Barbell = BarbellS->Barbell[ iBarbell ];
                            rect Bound = RectBCD( Barbell.Position, BARBELL_DIM );
                            if( IsInBound( MouseP, Bound ) ) {
                                iHover = iBarbell;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( BarbellS->nBarbell < BARBELL_MAX_COUNT ) {
                                vec2 Pos = GetCellRectBCFromPos( MouseP );
                                Pos.y += BARBELL_HALF_HEIGHT;
                                
                                EDITOR__BARBELL Barbell = {};
                                Barbell.Position = Pos;
                                
                                BarbellS->Barbell[ BarbellS->nBarbell++ ] = Barbell;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new BARBELL, but the state is full! Max Count = %u", BARBELL_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            BarbellS->nBarbell--;
                            for( uint32 iBarbell = iHover; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
                                BarbellS->Barbell[ iBarbell ] = BarbellS->Barbell[ iBarbell + 1 ];
                            }
                        }
                    } break;
                    
                    case EditorMode_Popper: {
                        // add new     : Left Click + No Hover
                        // delete      : Hover + Delete
                        
                        EDITOR__POPPER_STATE * PopperS = &Editor->PopperS;
                        
                        int32 iHover = -1;
                        for( uint32 iPopper = 0; iPopper < PopperS->nPopper; iPopper++ ) {
                            EDITOR__POPPER Popper = PopperS->Popper[ iPopper ];
                            rect Bound = RectBCD( Popper.Position, POPPER_DIM );
                            if( IsInBound( MouseP, Bound ) ) {
                                iHover = iPopper;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( PopperS->nPopper < POPPER_MAX_COUNT ) {
                                vec2 Pos = GetCellRectBCFromPos( MouseP );
                                
                                EDITOR__POPPER Popper = {};
                                Popper.Position = Pos;
                                
                                PopperS->Popper[ PopperS->nPopper++ ] = Popper;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new POPPER, but the state is full! Max Count = %u", POPPER_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            PopperS->nPopper--;
                            for( uint32 iPopper = iHover; iPopper < PopperS->nPopper; iPopper++ ) {
                                PopperS->Popper[ iPopper ] = PopperS->Popper[ iPopper + 1 ];
                            }
                        }
                    } break;
                    
                    case EditorMode_Spotter: {
                        // add new     : Left Click + No Hover
                        // delete      : Hover + Delete
                        
                        EDITOR__SPOTTER_STATE * SpotterS = &Editor->SpotterS;
                        
                        int32 iHover = -1;
                        for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
                            EDITOR__SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
                            rect Bound = RectCD( Spotter.Position, SPOTTER_DIM );
                            if( IsInBound( MouseP, Bound ) ) {
                                iHover = iSpotter;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( SpotterS->nSpotter < SPOTTER_MAX_COUNT ) {
                                vec2 Pos = GetCellRectCFromPos( MouseP );
                                
                                EDITOR__SPOTTER Spotter = {};
                                Spotter.Position = Pos;
                                
                                SpotterS->Spotter[ SpotterS->nSpotter++ ] = Spotter;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new SPOTTER, but the state is full! Max Count = %u", SPOTTER_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            SpotterS->nSpotter--;
                            for( uint32 iSpotter = iHover; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
                                SpotterS->Spotter[ iSpotter ] = SpotterS->Spotter[ iSpotter + 1 ];
                            }
                        }
                    } break;
                    
                    case EditorMode_Camper: {
                        // enable  : set rescue bound or position
                        // disable : hover rescue bound or position + delete
                        // rescue bound : left click + drag
                        // position     : Left Click + No Hover
                        // only one camper per level
                        
                        EDITOR__CAMPER_STATE * CamperS = &Editor->CamperS;
                        EDITOR__CAMPER       * Camper  = CamperS->Camper;
                        
                        int32 iHover = -1;
                        if( CamperS->nCamper > 0 ) {
                            rect BoundA = RectBCD( Camper->Position, CAMPER_DIM );
                            rect BoundB = Camper->HazardBound;
                            if( ( IsInBound( MouseP, BoundA ) ) || ( IsInBound( MouseP, BoundB ) ) ){
                                iHover = 0;
                            }
                        }
                        
                        flo32 Scale    = TILE_WIDTH * 0.5f;
                        vec2  ScalePos = Vec2(
                                              ( flo32 )( ( uint32 )( ( MouseP.x / Scale ) + 0.5f ) ) * Scale, ( flo32 )( ( uint32 )( ( MouseP.y / Scale ) + 0.5f ) ) * Scale );
                        DrawPoint( Pass_Game, ScalePos, TILE_DIM * 0.1f, COLOR_YELLOW );
                        
                        if( WasPressed( Mouse, MouseButton_Right ) ) {
                            CamperS->nCamper = 1;
                            Camper->Position = ScalePos;
                        }
                        if( WasPressed( Mouse, MouseButton_Left ) ) {
                            Editor->Mouse_DoMouse = true;
                            Editor->Mouse_iCell   = GetCellIndexFromPos( MouseP );
                        }
                        
                        if( Editor->Mouse_DoMouse ) {
                            UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                            
                            rect R = GetCellRectFromIndex( Editor->Mouse_iCell );
                            rect S = GetCellRectFromIndex( iCell );
                            rect Bound = RectExtrema( R, S );
                            DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                            
                            if( WasReleased( Mouse, MouseButton_Left ) ) {
                                Editor->Mouse_DoMouse = false;
                                
                                CamperS->nCamper = 1;
                                Camper->HazardBound = Bound;
                            }
                            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                                Editor->Mouse_DoMouse = false;
                            }
                        }
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            CamperS->nCamper = 0;
                        }
                    } break;
                    
                    case EditorMode_Checkpoint: {
                        // add new  : Left click + no hover
                        // delete   : hover + delete
                        // change order : hover + MouseWheel
                        EDITOR__CHECKPOINT_STATE * CheckpointS = &Editor->CheckpointS;
                        
                        int32 iHover = -1;
                        for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
                            EDITOR__CHECKPOINT Checkpoint = CheckpointS->Checkpoint[ iCheckpoint ];
                            if( IsInBound( MouseP, Checkpoint.Bound ) ) {
                                iHover = iCheckpoint;
                            }
                        }
                        
                        if( ( iHover == -1 ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
                            if( CheckpointS->nCheckpoint < CHECKPOINT_MAX_COUNT ) {
                                rect R = GetCellRectFromPos( MouseP );
                                R.Top += TILE_HEIGHT;
                                
                                EDITOR__CHECKPOINT Checkpoint = {};
                                Checkpoint.Bound = R;
                                
                                CheckpointS->Checkpoint[ CheckpointS->nCheckpoint++ ] = Checkpoint;
                            } else {
                                GlobalVar_DebugSystem.ErrorOccurred = true;
                                char str[ 512 ] = {};
                                sprintf( str, "ERROR! Attempted to create new CHECKPOINT, but the state is full! Max Count = %u", CHECKPOINT_MAX_COUNT );
                                CONSOLE_STRING( str );
                            }
                        }
                        
                        if( ( iHover > -1 ) && ( WasPressed( Keyboard, KeyCode_delete ) ) ) {
                            CheckpointS->nCheckpoint--;
                            for( uint32 iCheckpoint = iHover; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
                                CheckpointS->Checkpoint[ iCheckpoint ] = CheckpointS->Checkpoint[ iCheckpoint + 1 ];
                            }
                        }
                        
                        if( ( iHover > -1 ) && ( Mouse->WheelClicks != 0 ) ) {
                            if( ( Mouse->WheelClicks < 0 ) && ( iHover > 0 ) ) {
                                _swap( EDITOR__CHECKPOINT, CheckpointS->Checkpoint[ iHover ], CheckpointS->Checkpoint[ iHover - 1 ] );
                            }
                            if( ( Mouse->WheelClicks > 0 ) && ( iHover < ( ( int32 )CheckpointS->nCheckpoint - 1 ) ) ) {
                                _swap( EDITOR__CHECKPOINT, CheckpointS->Checkpoint[ iHover ], CheckpointS->Checkpoint[ iHover + 1 ] );
                            }
                        }
                    } break;
                    
                    case EditorMode_Event: {
                        switch( Editor->Event_Type ) {
                            case dataType_point: {
                                if( WasPressed( Mouse, MouseButton_Left ) ) {
                                    ( ( vec2 * )Editor->Event_data )[ 0 ] = MouseP;
                                }
                            } break;
                            
                            case dataType_rect: {
                                if( WasPressed( Mouse, MouseButton_Left ) ) {
                                    Editor->Event_DoMouse = true;
                                    Editor->Event_iCell   = GetCellIndexFromPos( MouseP );
                                }
                                if( Editor->Event_DoMouse ) {
                                    UINT32_PAIR iCell = GetCellIndexFromPos( MouseP );
                                    
                                    rect R = GetCellRectFromIndex( Editor->Event_iCell );
                                    rect S = GetCellRectFromIndex( iCell );
                                    rect Bound = RectExtrema( R, S );
                                    DrawRectOutline( Pass_Game, Bound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                                    
                                    if( WasReleased( Mouse, MouseButton_Left ) ) {
                                        Editor->Event_DoMouse = false;
                                        
                                        ( ( rect * )Editor->Event_data )[ 0 ] = Bound;
                                    }
                                }
                            } break;
                        }
                    } break;
                }
                
                if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                    Editor->Mode = EditorMode_Default;
                }
            }
            
            { // Draw
                for( uint32 iRow = 0; iRow < EDITOR__GRID_MAX_HEIGHT; iRow++ ) {
                    for( uint32 iCol = 0; iCol < EDITOR__GRID_MAX_WIDTH; iCol++ ) {
                        GRID_CELL_TYPE Type = ( GRID_CELL_TYPE )Editor->Grid[ iRow ][ iCol ];
                        switch( Type ) {
                            case CellType_Empty: {
                                // do nothing
                            } break;
                            
                            case CellType_Terrain: {
                                // TODO: group Cells into larger rects for better rendering
                                rect Bound = GetCellRectFromIndex( iCol, iRow );
                                DrawRect       ( Pass_Game, Bound, TERRAIN_COLOR );
                                DrawRectOutline( Pass_Game, Bound, TERRAIN_OUTLINE_COLOR );
                            } break;
                        }
                    }
                }
                
                LEVEL_STATS Stat = Editor->Stat;
                DrawRectOutline( Pass_Game, Stat.Bound, COLOR_RED );
                DrawPoint( Pass_Game, Vec2( Stat.Bound.Left,   Stat.EnterLeftY   ), Vec2( 0.1f, 0.1f ), COLOR_GREEN );
                DrawPoint( Pass_Game, Vec2( Stat.Bound.Right,  Stat.EnterRightY  ), Vec2( 0.1f, 0.1f ), COLOR_GREEN );
                DrawPoint( Pass_Game, Vec2( Stat.EnterBottomX, Stat.Bound.Bottom ), Vec2( 0.1f, 0.1f ), COLOR_GREEN );
                DrawPoint( Pass_Game, Vec2( Stat.EnterTopX,    Stat.Bound.Top    ), Vec2( 0.1f, 0.1f ), COLOR_GREEN );
                
                Editor_DrawCamper          ( Pass_Game, Editor );
                Editor_DrawSpikes          ( Pass_Game, Editor );
                Editor_DrawConveyor        ( Pass_Game, Editor );
                Editor_DrawScaffold        ( Pass_Game, Editor );
                Editor_DrawSeesaw          ( Pass_Game, Editor );
                Editor_DrawPushBlock       ( Pass_Game, Editor );
                Editor_DrawBreakBlock      ( Pass_Game, Editor );
                Editor_DrawCycleBlock      ( Pass_Game, Editor );
                Editor_DrawBoulder         ( Pass_Game, Editor );
                Editor_DrawExerciseBall    ( Pass_Game, Editor );
                Editor_DrawCollapsePlatform( Pass_Game, Editor );
                Editor_DrawPuncher         ( Pass_Game, Editor );
                Editor_DrawBarbell         ( Pass_Game, Editor );
                Editor_DrawMoney           ( Pass_Game, Editor );
                Editor_DrawPopper          ( Pass_Game, Editor );
                Editor_DrawSpotter         ( Pass_Game, Editor );
                
                { // DRAW CHECKPOINT
                    EDITOR__CHECKPOINT_STATE * CheckpointS  = &Editor->CheckpointS;
                    for( uint32 iCheckpoint = 0; iCheckpoint < CheckpointS->nCheckpoint; iCheckpoint++ ) {
                        EDITOR__CHECKPOINT Checkpoint = CheckpointS->Checkpoint[ iCheckpoint ];
                        
                        DrawRectOutline( Pass_Game, Checkpoint.Bound, COLOR_RED );
                        
                        char Num[ 8 ] = {};
                        sprintf( Num, "%u", iCheckpoint );
                        DrawString( Pass_Game, AppState->Font, Num, GetCenter( Checkpoint.Bound ), TextAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
                    }
                }
                
                { // DRAW MOVE BOUND
                    if( Editor->Mode == EditorMode_MoveEntity ) {
                        if( Editor->Mouse_DoMouse ) {
                            DrawRectOutline( Pass_Game, Editor->Mouse_TempBound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                        }
                        DrawRectOutline( Pass_Game, Editor->Move_SrcBound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                        
                        if( Editor->Move_DoMouse ) {
                            DrawRectOutline( Pass_Game, Editor->Move_DestBound, COLOR_YELLOW );
                        }
                    }
                }
                
                { // DRAW COPY BOUND
                    if( Editor->Mode == EditorMode_CopyEntity ) {
                        if( Editor->Mouse_DoMouse ) {
                            DrawRectOutline( Pass_Game, Editor->Mouse_TempBound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                        }
                        DrawRectOutline( Pass_Game, Editor->Copy_SrcBound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                        DrawRectOutline( Pass_Game, Editor->Copy_DestBound, COLOR_YELLOW );
                    }
                }
                
                { // DRAW MIRROR BOUND
                    if( Editor->Mode == EditorMode_MirrorEntity ) {
                        if( Editor->Mouse_DoMouse ) {
                            DrawRectOutline( Pass_Game, Editor->Mouse_TempBound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                        }
                        DrawRectOutline( Pass_Game, Editor->Mirror_SrcBound, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
                    }
                }
            }
            
            UpdateAndDrawEditorPanel( Platform, AppState, TempMemory, Mouse, Pass_Game, Pass_UI );
            
            boo32 doOpen   = WasPressed( Keyboard, KeyCode_o, KEYBOARD_FLAGS__CONTROL );
            boo32 doSave   = WasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL );
            boo32 doSaveAs = WasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT );
            boo32 doNew    = WasPressed( Keyboard, KeyCode_n, KEYBOARD_FLAGS__CONTROL );
            
            if( SaveOpen->current[ 0 ] != 0 ) {
                char str[ 128 ] = {};
                sprintf( str, "FILE: %s", SaveOpen->current );
                DISPLAY_STRING( str );
            } else {
                DISPLAY_STRING( "NO FILENAME!!", COLOR_RED );
            }
            if( SaveOpen->UnsavedChanges ) {
                DISPLAY_STRING( "UNSAVED CHANGES!!", COLOR_RED );
            }
            if( SaveOpen->recentSave ) {
                char str[ 128 ] = {};
                sprintf( str, "File Saved!! %s", SaveOpen->current );
                DISPLAY_STRING( str, COLOR_YELLOW );
                
                SaveOpen->Timer += dT;
                if( SaveOpen->Timer >= 2.0f ) {
                    SaveOpen->Timer      = 0.0f;
                    SaveOpen->recentSave = false;
                }
            }
            
            if( CanSaveOpen ) {
                INPUT_STRING * i = &SaveOpen->input_string;
                if( doSave ) {
                    if( SaveOpen->current[ 0 ] != 0 ) {
                        SaveLevel( Platform, AppState, TempMemory );
                    } else {
                        AppState->Mode = AppMode_SaveLevel;
                        AppState->Mode_IsInitialized = false;
                        SaveOpen->overwrite = false;
                        SaveOpen->Select_iSelect = 0;
                        SaveOpen->Select_iFile   = -1;
                        InitInputString( i );
                    }
                } else if( doSaveAs ) {
                    AppState->Mode = AppMode_SaveLevel;
                    AppState->Mode_IsInitialized = false;
                    SaveOpen->overwrite   = false;
                    SaveOpen->Select_iSelect = 0;
                    SaveOpen->Select_iFile   = -1;
                    InitInputString( i );
                } else if( doOpen ) {
                    AppState->Mode               = AppMode_OpenLevel;
                    AppState->Mode_IsInitialized = false;
                    SaveOpen->Select_iSelect = 0;
                    SaveOpen->Select_iFile   = -1;
                    InitInputString( i );
                }
            }
            
            if( doNew ) {
                if( !SaveOpen->UnsavedChanges ) {
                    SaveOpen->ConfirmNew = true;
                }
                
                if( SaveOpen->ConfirmNew ) {
                    ResetEditor( Editor, AppState );
                } else if( SaveOpen->UnsavedChanges ) {
                    SaveOpen->ConfirmNew = true;
                    
                }
            }
            if( SaveOpen->ConfirmNew ) {
                DISPLAY_STRING( "Careful! File contains unsaved changes!!" );
            }
            
            if( WasPressed( Keyboard, KeyCode_l, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) {
                AppState->Mode = AppMode_Game;
            }
            
            if( WasPressed( Keyboard, KeyCode_h, KEYBOARD_FLAGS__CONTROL ) ) {
                Panel->Show = !Panel->Show;
            }
            if( WasPressed( Keyboard, KeyCode_c, KEYBOARD_FLAGS__CONTROL ) ) {
                InitCameraForEditor( Editor );
            }
            
            EDITOR__CHECKPOINT_STATE * CheckpointS = &Editor->CheckpointS;
            DISPLAY_VALUE( uint32, CheckpointS->nCheckpoint );
        } break;
        
        case AppMode_Layout: {
            LAYOUT_STATE   * Layout   = &AppState->Layout;
            CAMERA_STATE   * Camera   = &Layout->Camera;
            FILE_SAVE_OPEN * SaveOpen = &Layout->SaveOpen;
            //INPUT_STRING   * Filter   = &Layout->Filter_String;
            
            if( !Layout->IsInitialized ) {
                Layout->IsInitialized = true;
                
                SaveOpen->FileList      = StringBuffer( PermMemory, 32, 2048 );
                //Layout->Filter_FileList = StringBuffer( PermMemory, LAYOUT__LEVEL_MAX_COUNT, _KB( 256 ) );
                
                InitLayoutPanel( AppState );
                //InitInputString( Filter, 32 );
                
                LAYOUT_LoadLevels( Platform, AppState, TempMemory );
                
                ResetLayout( Layout, AppState );
            }
            
            //UpdateKeyboardInputForLayoutFilter( Filter, Keyboard );
            UpdateAndDrawLayoutPanel( AppState, Mouse, Pass_Game, Pass_UI );
            
            UpdateCamera( Camera, Keyboard, dT );
            vec2 MouseP = GetWorldPos( Camera, AppState->App_Dim, Mouse->Position );
            
            if( Layout->DoMove ) {
                vec2 dPos = MouseP - Layout->DoMove_bPos;
                
                LAYOUT_LEVEL * Level = Layout->Level + Layout->DoMove_iLevel;
                Level->Pos += dPos;
                
                Layout->DoMove_bPos = MouseP;
                
                if( WasReleased( Mouse, MouseButton_Left ) ) {
                    Layout->DoMove = false;
                }
            }
            
            boo32 CanDoHover = ( !Layout->DoMove );
            
            int32 iHover = -1;
            if( CanDoHover ) {
                for( uint32 iLevel = 0; iLevel < Layout->nLevel; iLevel++ ) {
                    LAYOUT_LEVEL Level = Layout->Level[ iLevel ];
                    if( Level.Show ) {
                        rect R = RectCD( Level.Pos, Level.Dim );
                        if( IsInBound( MouseP, R ) ) {
                            iHover = iLevel;
                        }
                    }
                }
            }
            
            if( iHover > -1 ) {
                if( WasPressed( Mouse, MouseButton_Left ) ) {
                    Layout->DoMove        = true;
                    Layout->DoMove_iLevel = iHover;
                    Layout->DoMove_bPos   = MouseP;
                }
                if( WasPressed( Keyboard, KeyCode_delete ) ) {
                    LAYOUT_LEVEL * Level = Layout->Level + iHover;
                    
                    Level->Show = false;
                    Level->Pos  = {};
                }
            }
            
            { // Draw
                { // Level Bitmaps
                    for( uint32 iLevel = 0; iLevel < Layout->nLevel; iLevel++ ) {
                        LAYOUT_LEVEL Level = Layout->Level[ iLevel ];
                        if( Level.Show ) {
                            rect R = RectCD( Level.Pos, Level.Dim );
                            
                            TEXTURE_ID TextureID = ( TEXTURE_ID )( TextureID_Level_Bitmap + iLevel );
                            DrawRect( Pass_Game, TextureID, R, COLOR_WHITE );
                            
                            vec4 Color = Vec4( 0.1f, 0.1f, 0.0f, 1.0f );
                            if( ( ( int32 )iLevel == iHover ) || ( ( int32 )iLevel == Layout->DoMove_iLevel ) ) {
                                Color = COLOR_YELLOW;
                            }
                            
                            DrawRectOutline( Pass_Game, R, Color );
                            
                            vec2 P = GetTL( R );
                            DrawString( Pass_Game, AppState->Font, Level.FileName, P, TextAlign_TopLeft, WORLD_DEBUG_TEXT_SCALE * Camera->Scale_Ratio, COLOR_WHITE );
                        }
                    }
                }
            }
            
            
            boo32 CanSaveOpen = ( !Layout->DoMove );
            boo32 doOpen   = WasPressed( Keyboard, KeyCode_o, KEYBOARD_FLAGS__CONTROL );
            boo32 doSave   = WasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL );
            boo32 doSaveAs = WasPressed( Keyboard, KeyCode_s, KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT );
            boo32 doNew    = WasPressed( Keyboard, KeyCode_n, KEYBOARD_FLAGS__CONTROL );
            
            if( SaveOpen->current[ 0 ] != 0 ) {
                char str[ 128 ] = {};
                sprintf( str, "FILE: %s", SaveOpen->current );
                DISPLAY_STRING( str );
            } else {
                DISPLAY_STRING( "NO FILENAME!!", COLOR_RED );
            }
            if( SaveOpen->UnsavedChanges ) {
                DISPLAY_STRING( "UNSAVED CHANGES!!", COLOR_RED );
            }
            if( SaveOpen->recentSave ) {
                char str[ 128 ] = {};
                sprintf( str, "File Saved!! %s", SaveOpen->current );
                DISPLAY_STRING( str, COLOR_YELLOW );
                
                SaveOpen->Timer += dT;
                if( SaveOpen->Timer >= 2.0f ) {
                    SaveOpen->Timer      = 0.0f;
                    SaveOpen->recentSave = false;
                }
            }
            
            if( CanSaveOpen ) {
                INPUT_STRING * i = &SaveOpen->input_string;
                if( doSave ) {
                    if( SaveOpen->current[ 0 ] != 0 ) {
                        SaveLayout( Platform, AppState, TempMemory );
                    } else {
                        AppState->Mode = AppMode_SaveLayout;
                        AppState->Mode_IsInitialized = false;
                        SaveOpen->overwrite      = false;
                        SaveOpen->Select_iSelect = 0;
                        SaveOpen->Select_iFile   = -1;
                        InitInputString( i );
                    }
                } else if( doSaveAs ) {
                    AppState->Mode = AppMode_SaveLayout;
                    AppState->Mode_IsInitialized = false;
                    SaveOpen->overwrite   = false;
                    SaveOpen->Select_iSelect = 0;
                    SaveOpen->Select_iFile   = -1;
                    InitInputString( i );
                } else if( doOpen ) {
                    AppState->Mode = AppMode_OpenLayout;
                    AppState->Mode_IsInitialized = false;
                    SaveOpen->Select_iSelect = 0;
                    SaveOpen->Select_iFile   = -1;
                    InitInputString( i );
                }
            }
            
            if( doNew ) {
                if( !SaveOpen->UnsavedChanges ) {
                    SaveOpen->ConfirmNew = true;
                }
                
                if( SaveOpen->ConfirmNew ) {
                    ResetLayout( Layout, AppState );
                } else if( SaveOpen->UnsavedChanges ) {
                    SaveOpen->ConfirmNew = true;
                    
                }
            }
            if( SaveOpen->ConfirmNew ) {
                DISPLAY_STRING( "Careful! File contains unsaved changes!!" );
            }
        } break;
        
        case AppMode_SaveLevel: {
            EDITOR_STATE * Editor = &AppState->Editor;
            FILE_SAVE_OPEN * SaveOpen = &Editor->SaveOpen;
            
            if( !AppState->Mode_IsInitialized ) {
                AppState->Mode_IsInitialized = true;
                GetFileList( &SaveOpen->FileList, "dat" );
            }
            
            INPUT_STRING * i = &SaveOpen->input_string;
            UpdateKeyboardInputForFileSaveOpen( i, Keyboard, SaveOpen );
            DrawAndUpdateFileListForSaveOpen( Pass_UI, Keyboard, AppState, SaveOpen );
            
            INPUT_STRING FileSelect_String = {};
            if( SaveOpen->Select_iFile > -1 ) {
                FileSelect_String.maxChar = INPUT_STRING__CHAR_MAX_COUNT;
                
                STRING_BUFFER * FileList = &SaveOpen->FileList;
                char          * FileName = FileList->str[ SaveOpen->Select_iFile ];
                strcpy( FileSelect_String.string, FileName );
                
                FileSelect_String.nChar = ( uint32 )strlen( FileSelect_String.string );
                
                i = &FileSelect_String;
            }
            
            if( ( WasPressed( Keyboard, KeyCode_Enter ) ) && ( i->nChar > 0 ) ) {
                if( SaveOpen->overwrite ) {
                    strcpy( SaveOpen->current, i->string );
                    SaveLevel( Platform, AppState, TempMemory );
                } else {
                    boo32 FileExists = Platform->DoesFileExist( LEVEL_SAVE_DIRECTORY, i->string, "dat" );
                    if( FileExists ) {
                        SaveOpen->overwrite = true;
                    } else {
                        strcpy( SaveOpen->current, i->string );
                        SaveLevel( Platform, AppState, TempMemory );
                    }
                }
            }
            
            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = AppMode_Editor;
            }
        } break;
        
        case AppMode_OpenLevel: {
            EDITOR_STATE * Editor = &AppState->Editor;
            FILE_SAVE_OPEN * SaveOpen = &Editor->SaveOpen;
            
            if( !AppState->Mode_IsInitialized ) {
                AppState->Mode_IsInitialized = true;
                GetFileList( &SaveOpen->FileList, "dat" );
            }
            
            INPUT_STRING * i = &SaveOpen->input_string;
            UpdateKeyboardInputForFileSaveOpen( i, Keyboard, SaveOpen );
            DrawAndUpdateFileListForSaveOpen( Pass_UI, Keyboard, AppState, SaveOpen );
            
            INPUT_STRING FileSelect_String = {};
            if( SaveOpen->Select_iFile > -1 ) {
                FileSelect_String.maxChar = INPUT_STRING__CHAR_MAX_COUNT;
                
                STRING_BUFFER * FileList = &SaveOpen->FileList;
                char          * FileName = FileList->str[ SaveOpen->Select_iFile ];
                strcpy( FileSelect_String.string, FileName );
                
                FileSelect_String.nChar = ( uint32 )strlen( FileSelect_String.string );
                
                i = &FileSelect_String;
            }
            
            if( ( WasPressed( Keyboard, KeyCode_Enter ) ) && ( i->nChar > 0 ) ) {
                boo32 FileExists = Platform->DoesFileExist( LEVEL_SAVE_DIRECTORY, i->string, "dat" );
                if( FileExists ) {
                    AppState->Mode = AppMode_Editor;
                    EDITOR_LoadLevel( Platform, AppState, TempMemory, LEVEL_SAVE_DIRECTORY, i->string );
                }
            }
            
            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = AppMode_Editor;
            }
            
        } break;
        
        case AppMode_SaveLayout: {
            LAYOUT_STATE   * Layout   = &AppState->Layout;
            FILE_SAVE_OPEN * SaveOpen = &Layout->SaveOpen;
            
            if( !AppState->Mode_IsInitialized ) {
                AppState->Mode_IsInitialized = true;
                GetFileList( &SaveOpen->FileList, FILETAG__LAYOUT );
            }
            
            INPUT_STRING * i = &SaveOpen->input_string;
            UpdateKeyboardInputForFileSaveOpen( i, Keyboard, SaveOpen );
            DrawAndUpdateFileListForSaveOpen( Pass_UI, Keyboard, AppState, SaveOpen );
            
            INPUT_STRING FileSelect_String = {};
            if( SaveOpen->Select_iFile > -1 ) {
                FileSelect_String.maxChar = INPUT_STRING__CHAR_MAX_COUNT;
                
                STRING_BUFFER * FileList = &SaveOpen->FileList;
                char          * FileName = FileList->str[ SaveOpen->Select_iFile ];
                strcpy( FileSelect_String.string, FileName );
                
                FileSelect_String.nChar = ( uint32 )strlen( FileSelect_String.string );
                
                i = &FileSelect_String;
            }
            
            if( ( WasPressed( Keyboard, KeyCode_Enter ) ) && ( i->nChar > 0 ) ) {
                if( SaveOpen->overwrite ) {
                    strcpy( SaveOpen->current, i->string );
                    SaveLayout( Platform, AppState, TempMemory );
                } else {
                    boo32 FileExists = Platform->DoesFileExist( LEVEL_SAVE_DIRECTORY, i->string, FILETAG__LAYOUT );
                    if( FileExists ) {
                        SaveOpen->overwrite = true;
                    } else {
                        strcpy( SaveOpen->current, i->string );
                        SaveLayout( Platform, AppState, TempMemory );
                    }
                }
            }
            
            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = AppMode_Layout;
            }
        } break;
        
        case AppMode_OpenLayout: {
            LAYOUT_STATE   * Layout   = &AppState->Layout;
            FILE_SAVE_OPEN * SaveOpen = &Layout->SaveOpen;
            
            if( !AppState->Mode_IsInitialized ) {
                AppState->Mode_IsInitialized = true;
                GetFileList( &SaveOpen->FileList, FILETAG__LAYOUT );
            }
            
            INPUT_STRING * i = &SaveOpen->input_string;
            UpdateKeyboardInputForFileSaveOpen( i, Keyboard, SaveOpen );
            DrawAndUpdateFileListForSaveOpen( Pass_UI, Keyboard, AppState, SaveOpen );
            
            INPUT_STRING FileSelect_String = {};
            if( SaveOpen->Select_iFile > -1 ) {
                FileSelect_String.maxChar = INPUT_STRING__CHAR_MAX_COUNT;
                
                STRING_BUFFER * FileList = &SaveOpen->FileList;
                char          * FileName = FileList->str[ SaveOpen->Select_iFile ];
                strcpy( FileSelect_String.string, FileName );
                
                FileSelect_String.nChar = ( uint32 )strlen( FileSelect_String.string );
                
                i = &FileSelect_String;
            }
            
            if( ( WasPressed( Keyboard, KeyCode_Enter ) ) && ( i->nChar > 0 ) ) {
                boo32 FileExists = Platform->DoesFileExist( LEVEL_SAVE_DIRECTORY, i->string, FILETAG__LAYOUT );
                if( FileExists ) {
                    AppState->Mode = AppMode_Layout;
                    LAYOUT_LoadLayout( Platform, Layout, TempMemory, LEVEL_SAVE_DIRECTORY, i->string );
                }
            }
            
            if( WasPressed( Keyboard, KeyCode_Escape ) ) {
                AppState->Mode = AppMode_Layout;
            }
            
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    DISPLAY_VALUE( uint64, TempMemory->used );
    Assert( TempMemory->used < TempMemory->size );
    
    EVENT_STATE * Event = &AppState->Event;
    DISPLAY_VALUE( uint32, Event->nEvent );
}