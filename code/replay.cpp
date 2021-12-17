
internal void
CaptureReplay( APP_STATE * AppState ) {
    REPLAY_STATE * Replay = &AppState->Replay;
    
    if( !Replay->Show ) {
        REPLAY_FRAME * Frame = Replay->Frame + Replay->atFrame;
        memcpy( &Frame->Game,          &AppState->Game,          sizeof( GAME_STATE          ) );
        memcpy( &Frame->Player,        &AppState->Player,        sizeof( PLAYER_STATE        ) );
        memcpy( &Frame->CamperS,       &AppState->CamperS,       sizeof( CAMPER_STATE        ) );
        memcpy( &Frame->Grenade,       &AppState->Grenade,       sizeof( GRENADE_STATE       ) );
        memcpy( &Frame->RunnerS,       &AppState->RunnerS,       sizeof( RUNNER_STATE        ) );
        memcpy( &Frame->PuncherS,      &AppState->PuncherS,      sizeof( PUNCHER_STATE       ) );
        memcpy( &Frame->PopperS,       &AppState->PopperS,       sizeof( POPPER_STATE        ) );
        memcpy( &Frame->SpotterS,      &AppState->SpotterS,      sizeof( SPOTTER_STATE       ) );
        memcpy( &Frame->HedgehogS,     &AppState->HedgehogS,     sizeof( HEDGEHOG_STATE      ) );
        memcpy( &Frame->SeesawS,       &AppState->SeesawS,       sizeof( SEESAW_STATE        ) );
        memcpy( &Frame->BoulderS,      &AppState->BoulderS,      sizeof( BOULDER_STATE       ) );
        memcpy( &Frame->ExerciseBallS, &AppState->ExerciseBallS, sizeof( EXERCISE_BALL_STATE ) );
        memcpy( &Frame->BarbellS,      &AppState->BarbellS,      sizeof( BARBELL_STATE       ) );
        memcpy( &Frame->MoneyS,        &AppState->MoneyS,        sizeof( MONEY_STATE         ) );
        memcpy( &Frame->StomperS,      &AppState->StomperS,      sizeof( STOMPER_STATE       ) );
        memcpy( &Frame->CycleBlockS,   &AppState->CycleBlockS,   sizeof( CYCLE_BLOCK_STATE   ) );
        memcpy( &Frame->PushBlockS,    &AppState->PushBlockS,    sizeof( PUSH_BLOCK_STATE    ) );
        memcpy( &Frame->BreakBlockS,   &AppState->BreakBlockS,   sizeof( BREAK_BLOCK_STATE   ) );
        memcpy( &Frame->CollapsePlatformS, &AppState->CollapsePlatformS,  sizeof( COLLAPSE_PLATFORM_STATE ) );
        
        memcpy( &Frame->ExerciseMiniBoss01, &AppState->ExerciseMiniBoss01, sizeof( EXERCISE_MINIBOSS01 ) );
        memcpy( &Frame->ExerciseMiniBoss02, &AppState->ExerciseMiniBoss02, sizeof( EXERCISE_MINIBOSS02 ) );
        memcpy( &Frame->ExerciseMiniBoss03, &AppState->ExerciseMiniBoss03, sizeof( EXERCISE_MINIBOSS03 ) );
        memcpy( &Frame->ExerciseBoss,       &AppState->ExerciseBoss,       sizeof( EXERCISE_BOSS       ) );
        
        Replay->atFrame = ( Replay->atFrame + 1 ) % REPLAY_FRAME_MAX_COUNT;
    }
}