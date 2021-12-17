
internal EVENT
EventC( EVENT_FUNC * Func, MEMORY * Event_Memory ) {
    EVENT Result = {};
    Result.IsActive = true;
    Result.Func     = Func;
    Result.Data     = ( uint8 * )_pushSize( Event_Memory, 512 );
    return Result;
}

internal void
AddEvent( APP_STATE * AppState, EVENT Event ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    
    Assert( LevelS->nLevel > 0 );
    LEVEL * Level  = LevelS->Level + ( LevelS->nLevel - 1 );
    
    if( Level->nEvent < LEVEL__EVENT_MAX_COUNT ) {
        Level->Event[ Level->nEvent++ ] = Event;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to add new EVENT to Level %s, but the state is full! Max Count = %u", Level->FileNameOfLevel, LEVEL__EVENT_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
UpdateEvents( PLATFORM * Platform, APP_STATE * AppState ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
    
    for( uint32 iEvent = 0; iEvent < Level->nEvent; iEvent++ ) {
        EVENT Event = Level->Event[ iEvent ];
        if( Event.IsActive ) {
            Event.Func( Platform, AppState, Event.Data, iEvent );
        }
    }
}