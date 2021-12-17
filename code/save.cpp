
//----------
// SAVE LEVEL
//----------

internal void
GetFileList( STRING_BUFFER * List, char * FileTag ) {
    char WorkingDir[ 512 + 1 ] = {};
    GetCurrentDirectory( 512, WorkingDir );
    SetCurrentDirectory( LEVEL_SAVE_DIRECTORY );
    
    uint32 Length = ( uint32 )strlen( FileTag );
    Reset( List );
    
    WIN32_FIND_DATA File_Data   = {};
    HANDLE          File_Handle = FindFirstFile( "*", &File_Data ); // .
    FindNextFile( File_Handle, &File_Data ); // ..
    while( FindNextFile( File_Handle, &File_Data ) ) {
        char * ptr = File_Data.cFileName;
        
        uint32 len = ( uint32 )strlen( ptr );
        if( len > Length ) {
            while( *ptr ) { ptr++; }   ptr -= Length;
            
            if( MatchSegment( FileTag, ptr, Length ) ) {
                ptr--;
                ptr[ 0 ] = 0;
                
                char * Status = AddString( List, File_Data.cFileName );
                if( !Status ) {
                    GlobalVar_DebugSystem.ErrorOccurred = true;
                    char str[ 512 ] = {};
                    sprintf( str, "ERROR! FILE LIST ran out of space in STRING_BUFFER for storing FileNames! Max Count = %u", List->nStrMax );
                    CONSOLE_STRING( str );
                }
            }
        }
    }
    
    SetCurrentDirectory( WorkingDir );
}

internal UINT32_PAIR
UploadBMPFromFile( PLATFORM * Platform, TEXTURE_ID TextureID, char * SaveDir, char * FileName ) {
    TEXTURE_DATA Texture = BMP_ReadFile( Platform, SaveDir, FileName );
    UploadTexture( &Platform->Renderer, TextureID, Texture.Width, Texture.Height, Texture.TexelData );
    
    UINT32_PAIR Result = UInt32Pair( Texture.Width, Texture.Height );
    return Result;
}

internal void
LAYOUT_LoadLevel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * FileName ) {
    LAYOUT_STATE * Layout = &AppState->Layout;
    if( Layout->nLevel < LAYOUT__LEVEL_MAX_COUNT ) {
        LEVEL_STATS Stat = LAYOUT_LoadLevelStats( Platform, TempMemory, LEVEL_SAVE_DIRECTORY, FileName );
        
        TEXTURE_ID TextureID = ( TEXTURE_ID )( ( uint32 )TextureID_Level_Bitmap + Layout->nLevel );
        UploadBMPFromFile( Platform, TextureID, LEVEL_SAVE_DIRECTORY, FileName );
        
        LAYOUT_LEVEL * Level = Layout->Level + Layout->nLevel;
        Level->Dim = GetDim( Stat.Bound );
        strcpy( Level->FileName, FileName );
        
        Layout->nLevel++;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to add level to LAYOUT_STATE, but state is full! Max Count = %u", LAYOUT__LEVEL_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
LAYOUT_LoadLevels( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    LAYOUT_STATE * Layout = &AppState->Layout;
    
    char * FileTag = FILETAG__LEVEL_STATS;
    
    char WorkingDir[ 512 + 1 ] = {};
    GetCurrentDirectory( 512, WorkingDir );
    SetCurrentDirectory( LEVEL_SAVE_DIRECTORY );
    
    uint32 Length = ( uint32 )strlen( FileTag );
    Layout->nLevel = 0;
    
    WIN32_FIND_DATA File_Data   = {};
    HANDLE          File_Handle = FindFirstFile( "*", &File_Data ); // .
    FindNextFile( File_Handle, &File_Data ); // ..
    while( FindNextFile( File_Handle, &File_Data ) ) {
        char * ptr = File_Data.cFileName;
        
        uint32 len = ( uint32 )strlen( ptr );
        if( len > Length ) {
            while( *ptr ) { ptr++; }   ptr -= Length;
            
            if( MatchSegment( FileTag, ptr, Length ) ) {
                ptr--;
                ptr[ 0 ] = 0;
                
                LAYOUT_LoadLevel( Platform, AppState, TempMemory, File_Data.cFileName );
            }
        }
    }
    
    SetCurrentDirectory( WorkingDir );
}

#define VERSION__EVENTS_EDIT  ( 1 )
#define FILETAG__EVENTS_EDIT  ( "EVENTSED" )
internal void
SaveEventsForEdit( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__EVENTS_EDIT;
    uint32 Version = VERSION__EVENTS_EDIT;
    
    if( Editor->nEvent > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        DATA_TYPE_LABELS;
        DATA_TYPE_SIZES;
        
        _writem( output, uint32, Editor->nEvent );
        for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
            EDITOR__EVENT Event = Editor->Event[ iEvent ];
            writeString( output, Event.name );
            _writem( output, boo32, Event.IsActive );
            _writem( output, uint32, Event.nData );
            for( uint32 iData = 0; iData < Event.nData; iData++ ) {
                EDITOR_EVENT__DATA_TYPE Type = ( EDITOR_EVENT__DATA_TYPE )Event.data_Type[ iData ];
                writeSegment( output, dataTypeLabel[ Type ] );
                
                writeString( output, Event.data_name[ iData ] );
                
                uint32 size = dataTypeSize[ Type ];
                uint8 * dest = ( uint8 * )_pushSize( output, size );
                memcpy( dest, Event.data_ptr[ iData ], size );
            }
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

#define VERSION__EVENTS_GAME  ( 1 )
#define FILETAG__EVENTS_GAME  ( "EVENTSGM" )
internal void
SaveEventsForGame( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__EVENTS_GAME;
    uint32 Version = VERSION__EVENTS_GAME;
    
    if( Editor->nEvent > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, Editor->nEvent );
        for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
            EDITOR__EVENT Event = Editor->Event[ iEvent ];
            
            _writem( output, boo32, Event.IsActive );
            
            _writem( output, uint32,              Event.data_size );
            _writeb( output, Event.data_ptr[ 0 ], Event.data_size );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal void
SaveEvents( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    SaveEventsForEdit( Platform, Editor, TempMemory, SaveDir, FileName );
    SaveEventsForGame( Platform, Editor, TempMemory, SaveDir, FileName );
}

internal void
SaveBitmap( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    LEVEL_STATS Stat = Editor->Stat;
    vec2       Dim  = GetDim( Stat.Bound );
    
    uint32 nPixelPerCell = 16;
    
    uint32 xCell = ( uint32 )( Dim.x / TILE_WIDTH  );
    uint32 yCell = ( uint32 )( Dim.y / TILE_HEIGHT );
    uint32 nCell = xCell * yCell;
    
    if( ( xCell > 0 ) && ( yCell > 0 ) ) {
        uint32 xPixel = xCell * nPixelPerCell;
        uint32 yPixel = yCell * nPixelPerCell;
        uint32 nPixel = xPixel * yPixel;
        
        uint32   Output_Size = sizeof( BMP_HEADER ) + sizeof( uint32 ) * nPixel;
        MEMORY  _Output      = SubArena( TempMemory, Output_Size );
        MEMORY * Output      = &_Output;
        
        BMP_HEADER * header = _pushType( Output, BMP_HEADER );
        header->FileType     = 0x4d42;
        header->FileSize     = Output_Size;
        header->offsetToData = sizeof( BMP_HEADER );
        header->bitmapInfoHeaderSize = 40;
        header->Width        = xPixel;
        header->Height       = yPixel;
        header->planes       = 1;
        header->bitsPerPixel = 32;
        
        uint32 * Pixel = _pushArray( Output, uint32, nPixel );
        for( uint32 iPixel = 0; iPixel < nPixel; iPixel++ ) {
            Pixel[ iPixel ] = 0xFF000000;
        }
        
        OutputTerrainToLevelBitmap         ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputConveyorToLevelBitmap        ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        //OutputSpikesToLevelBitmap      ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        //OutputCheckpointToLevelBitmap  ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputScaffoldToLevelBitmap        ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputPuncherToLevelBitmap         ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputSeesawToLevelBitmap          ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputCycleBlockToLevelBitmap      ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputPushBlockToLevelBitmap       ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputBoulderToLevelBitmap         ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputExerciseBallToLevelBitmap    ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputBreakBlockToLevelBitmap      ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputPopperToLevelBitmap          ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputSpotterToLevelBitmap         ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputCamperToLevelBitmap          ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputCollapsePlatformToLevelBitmap( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        OutputMoneyToLevelBitmap           ( Editor, nPixelPerCell, Pixel, xPixel, yPixel );
        
        OutputFile( Platform, Output, SaveDir, FileName, "bmp" );
        _popSize( TempMemory, Output_Size );
    }
}

internal void
SaveLevel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    FILE_SAVE_OPEN * save = &Editor->SaveOpen;
    
    AppState->Mode = AppMode_Editor;
    
    { // COLLISION
        uint32 nTerrain = 0;
        for( uint32 y = 0; y < EDITOR__GRID_MAX_HEIGHT; y++ ) {
            for( uint32 x = 0; x < EDITOR__GRID_MAX_WIDTH; x++ ) {
                if( Editor->Grid[ y ][ x ] == CellType_Terrain ) {
                    nTerrain++;
                }
            }
        }
        if( nTerrain > 0 ) {
            UINT32_PAIR * Terrain_pair = _pushArray_clear( TempMemory, UINT32_PAIR, nTerrain );
            
            nTerrain = 0;
            for( uint32 y = 0; y < EDITOR__GRID_MAX_HEIGHT; y++ ) {
                for( uint32 x = 0; x < EDITOR__GRID_MAX_WIDTH; x++ ) {
                    if( Editor->Grid[ y ][ x ] == CellType_Terrain ) {
                        Terrain_pair[ nTerrain++ ] = UInt32Pair( x, y );
                    }
                }
            }
            
            GenTerrainCollision( AppState, Terrain_pair, nTerrain, TempMemory );
            
            _popArray( TempMemory, UINT32_PAIR, nTerrain );
            
            SaveCollisionTerrain( Platform, &AppState->Collision, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
            SaveCollisionJump   ( Platform, &AppState->Collision, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
            SaveWallSlide       ( Platform, &AppState->Collision, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
        }
    }
    
    SaveCamper          ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveTerrain         ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveSpikes          ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveConveyor        ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveScaffold        ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveSeesaw          ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveCycleBlock      ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SavePushBlock       ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveBreakBlock      ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveCollapsePlatform( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SavePuncher         ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveBarbell         ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveMoney           ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SavePopper          ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveSpotter         ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveBoulder         ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveExerciseBall    ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveCheckpoint      ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveEvents          ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveBitmap          ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    SaveLevelStats      ( Platform, Editor, TempMemory, LEVEL_SAVE_DIRECTORY, save->current );
    
    MEMORY  _output = SubArena( TempMemory, 4 );
    MEMORY * output = &_output;
    
    boo32 IsValid = Platform->WriteFile( LEVEL_SAVE_DIRECTORY, save->current, "dat", output->base, ( uint32 )output->size );
    if( IsValid ) {
        save->UnsavedChanges = false;
        save->recentSave     = true;
    } else {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to save File: %s.dat", save->current );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
    
    _popSize( TempMemory, 4 );
}