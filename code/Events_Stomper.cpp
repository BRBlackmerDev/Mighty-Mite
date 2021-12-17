
internal void
CycleBlockSet_BoulderInit_Event01( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    PLAYER_STATE  * Player   = &AppState->Player;
    BOULDER_STATE * BoulderS = &AppState->BoulderS;
    LEVEL_STATS      Stat     = AppState->Stat;
    
    if( !AppState->Level_IsInitialized ) {
        AppState->Level_IsInitialized = true;
    }
    
    if( ( Player->Carry_ID != PlayerCarryID_Boulder ) && ( BoulderS->nBoulder == 0 ) ) {
        AddBoulder( AppState, GetPOfLevel( Vec2( 28.0f, 16.0f ), Stat.Bound ) );
    }
}

internal void
Stomper01_Event01( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    LEVEL_STATS Stat = AppState->Stat;
    
    if( !AppState->Level_IsInitialized ) {
        AppState->Level_IsInitialized = true;
        
        AddStomper( AppState, GetPOfLevel( Vec2( 22.0f, 5.0f ), Stat.Bound ) );
    }
    
    STOMPER_STATE * StomperS = &AppState->StomperS;
    if( StomperS->nStomper < 1 ) {
        AddStomper( AppState, GetPOfLevel( Vec2( 34.0f, 14.0f ), Stat.Bound ) );
    }
}

internal void
Stomper02_Event01( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    LEVEL_STATS Stat = AppState->Stat;
    
    if( !AppState->Level_IsInitialized ) {
        AppState->Level_IsInitialized = true;
        
        AddStomper( AppState, GetPOfLevel( Vec2(  3.0f, 5.0f ), Stat.Bound ) );
        AddStomper( AppState, GetPOfLevel( Vec2(  7.0f, 5.0f ), Stat.Bound ) );
        AddStomper( AppState, GetPOfLevel( Vec2( 11.0f, 5.0f ), Stat.Bound ) );
        AddStomper( AppState, GetPOfLevel( Vec2( 15.0f, 5.0f ), Stat.Bound ) );
        AddStomper( AppState, GetPOfLevel( Vec2( 19.0f, 5.0f ), Stat.Bound ) );
        AddStomper( AppState, GetPOfLevel( Vec2( 23.0f, 5.0f ), Stat.Bound ) );
        AddBoulder( AppState, GetPOfLevel( Vec2( 27.0f, 5.0f ), Stat.Bound ) );
    }
}