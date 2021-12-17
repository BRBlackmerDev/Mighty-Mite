
internal void
GAME_LoadLevel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    
    GAME_LoadLevelStats      ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadTerrain         ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadCollisionTerrain( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadCollisionJump   ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadWallSlide       ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadSpikes          ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadConveyor        ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadScaffold        ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadSeesaw          ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadCycleBlock      ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadPushBlock       ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadBreakBlock      ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadBoulder         ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadExerciseBall    ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadCollapsePlatform( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadPuncher         ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadBarbell         ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadMoney           ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadPopper          ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadSpotter         ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadCheckpoint      ( Platform, AppState, TempMemory, SaveDir, FileName );
    GAME_LoadCamper          ( Platform, AppState, TempMemory, SaveDir, FileName );
    
    LEVEL_STATS Stat = AppState->Stat;
    AppState->DeathPlaneY = Stat.Bound.Bottom;
    
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LEVEL         Level  = LevelS->Level[ LevelS->iCurrentLevel ];
    
    if( Level.iLinkBottom_IsActive ) {
        AppState->DeathPlaneY -= ( TILE_HEIGHT * 4.0f );
    }
    
    { // set Camera Bound
        rect Bound = Stat.Bound;
        
        vec2 Center   = GetCenter( Bound );
        vec2 DimA     = GetDim( Bound );
        vec2 HalfDimA = DimA * 0.5f;
        
        flo32 aspectRatio = 1920.0f / 1080.0f;
        flo32 y = CAMERA_TILE_Y_COUNT * TILE_HEIGHT;
        flo32 x = y * aspectRatio;
        
        vec2 DimB     = Vec2( x, y );
        vec2 HalfDimB = DimB * 0.5f;
        
        if( DimB.x < DimA.x ) {
            Bound.Left  += HalfDimB.x;
            Bound.Right -= HalfDimB.x;
        } else {
            Bound.Left  = Center.x;
            Bound.Right = Center.x;
        }
        if( DimB.y < DimA.y ) {
            Bound.Bottom += HalfDimB.y;
            Bound.Top    -= HalfDimB.y;
        } else {
            Bound.Bottom = Center.y;
            Bound.Top    = Center.y;
        }
        
        AppState->Camera_Bound = Bound;
    }
    
}

#if 0
internal void
ROOM_LoadEvents( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * FileName, uint32 iRoom ) {
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + iRoom;
    
    EVENT_STATE * Event  = &AppState->Event;
    MEMORY      * memory = &AppState->Event_Memory;
    
    char * SaveDir = LEVEL_SAVE_DIRECTORY;
    char * FileTag = FILETAG__EVENTS_GAME;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        switch( Version ) {
            case 1: {
                // TODO: error checking and verification
                
                uint32 nEvent = _read( ptr, uint32 );
                
                Room->Event = UInt32Pair( Event->nEvent, nEvent );
                Event->nEvent += nEvent;
                
                for( uint32 iEvent = 0; iEvent < nEvent; iEvent++ ) {
                    EVENT * e = Event->Event + ( Room->Event.m + iEvent );
                    e->IsActive = _read( ptr, boo32 );
                    
                    uint32 data_size = _read( ptr, uint32 );
                    uint8 * src  = _addra( ptr, uint8, data_size );
                    uint8 * dest = ( uint8 * )_pushSize( memory, data_size );
                    memcpy( dest, src, data_size );
                    
                    e->data = dest;
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
GAME_LoadEvents( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    WORLD_STATE * World = &AppState->World;
    for( uint32 iRoom = 0; iRoom < World->nRoom; iRoom++ ) {
        ROOM * Room = World->Room + iRoom;
        ROOM_LoadEvents( Platform, AppState, TempMemory, Room->name, iRoom );
    }
}
#endif

//----------
// EDITOR LOAD ENTITY
//----------

internal void
EDITOR_LoadEvents( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__EVENTS_EDIT;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        switch( Version ) {
            case 1: {
                STRING_BUFFER * Event_Names = &Editor->Event_Names;
                
                MEMORY  _memory = Memory( Editor->Event_Memory, EDITOR__EVENT_MEMORY_SIZE );
                MEMORY * memory = &_memory;
                
                Editor->nEvent = _read( ptr, uint32 );
                
                DATA_TYPE_LABELS;
                DATA_TYPE_SIZES;
                
                for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
                    EDITOR__EVENT * Event = Editor->Event + iEvent;
                    
                    char * name = _addrStr( ptr );
                    Event->name = AddString( Event_Names, name );
                    Event->IsActive = _read( ptr, boo32 );
                    Event->nData = _read( ptr, uint32 );
                    Assert( Event->nData < EDITOR_EVENT__DATA_MAX_COUNT );
                    for( uint32 iData = 0; iData < Event->nData; iData++ ) {
                        char * label = _addra( ptr, char, 4 );
                        
                        int32 i = -1;
                        for( uint32 iLabel = 0; iLabel < dataType_Count; iLabel++ ) {
                            if( MatchSegment( label, dataTypeLabel[ iLabel ], 4 ) ) {
                                i = iLabel;
                            }
                        }
                        if( i > -1 ) {
                            uint32 size = dataTypeSize[ i ];
                            
                            char * var = _addrStr( ptr );
                            Event->data_name[ iData ] = AddString( Event_Names, var );
                            Event->data_Type[ iData ] = ( EDITOR_EVENT__DATA_TYPE )i;
                            Event->data_ptr [ iData ] = ( uint8 * )_pushSize( memory, size );
                            
                            uint8 * src  = _addra( ptr, uint8, size );
                            memcpy( Event->data_ptr[ iData ], src, size );
                            
                            Event->data_size += size;
                        } else {
                            InvalidCodePath;
                        }
                    }
                }
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadLevel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    // TODO: How should I handle EDITOR_Loading previous Versions? Should I send an error message? Where should I send it?
    
    EDITOR_STATE * Editor = &AppState->Editor;
    ResetEditor( Editor, AppState );
    
    FILE_SAVE_OPEN * open = &Editor->SaveOpen;
    strcpy( open->current, FileName );
    
    EDITOR_LoadTerrain         ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadLevelStats      ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadEvents          ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadCheckpoint      ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadConveyor        ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadScaffold        ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadSeesaw          ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadCycleBlock      ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadPushBlock       ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadBreakBlock      ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadBoulder         ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadExerciseBall    ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadCollapsePlatform( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadPuncher         ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadBarbell         ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadMoney           ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadPopper          ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadSpotter         ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadSpikes          ( Platform, AppState, TempMemory, SaveDir, FileName );
    EDITOR_LoadCamper          ( Platform, AppState, TempMemory, SaveDir, FileName );
    
    LEVEL_STATS Stat = Editor->Stat;
    
    CAMERA_STATE * Camera = &Editor->Camera;
    Camera->Pos = GetCenter( Stat.Bound );
}