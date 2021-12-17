
internal void
Stomper02_Event01( APP_STATE * AppState, uint8 * ptr, uint32 iEvent ) {
    //ACTIVE//
    
    LEVEL_STATS Stat = AppState->Stat;
    
    if( !AppState->Level_IsInitialized ) {
        AppState->Level_IsInitialized = true;
        
        AddStomper( AppState, GetPOfLevel( Vec2(  7.0f, 5.0f ), Stat.Bound ) );
        AddStomper( AppState, GetPOfLevel( Vec2( 13.0f, 5.0f ), Stat.Bound ) );
        AddStomper( AppState, GetPOfLevel( Vec2( 19.0f, 5.0f ), Stat.Bound ) );
        AddStomper( AppState, GetPOfLevel( Vec2( 25.0f, 5.0f ), Stat.Bound ) );
    }
}