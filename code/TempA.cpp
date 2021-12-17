
internal void
TempA_Event01( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    LEVEL_STATS Stat = AppState->Stat;
    
    EXERCISE_MINIBOSS01 * Boss = &AppState->ExerciseMiniBoss01;
    if( !Boss->IsActive ) {
        InitExerciseMiniBoss01( AppState );
        
        LEVEL_STATE * LevelS = &AppState->LevelS;
        LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
        Level->iLinkLeft_IsActive  = false;
        Level->iLinkRight_IsActive = false;
        
        AddStomper( AppState, GetPOfLevel( Vec2(  8.0f, 6.0f ), Stat.Bound ) );
        AddStomper( AppState, GetPOfLevel( Vec2( 24.0f, 6.0f ), Stat.Bound ) );
    }
    
    if( ( Boss->IsActive ) && ( Boss->Mode != ExerciseMiniBoss01Mode_IsDead ) ) {
        STOMPER_STATE * StomperS = &AppState->StomperS;
        if( StomperS->nStomper < 2 ) {
            AddStomper( AppState, GetPOfLevel( Vec2( -2.0f, 6.0f ), Stat.Bound ) );
        }
    }
    
    if( ( Boss->IsActive ) && ( Boss->Mode == ExerciseMiniBoss01Mode_IsDead ) ) {
        Boss->IsActive = false;
        
        LEVEL_STATE * LevelS = &AppState->LevelS;
        LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
        Level->iLinkLeft_IsActive  = true;
        Level->iLinkRight_IsActive = true;
        
        Level->Event[ iEvent ].IsActive = false;
    }
}