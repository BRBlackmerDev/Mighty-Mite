
#define VERSION__LAYOUT  ( 1 )
#define FILETAG__LAYOUT  ( "LAYOUT__" )
internal void
SaveLayout( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    LAYOUT_STATE   * Layout = &AppState->Layout;
    FILE_SAVE_OPEN * Save   = &Layout->SaveOpen;
    
    AppState->Mode = AppMode_Layout;
    
    char * FileTag  = FILETAG__LAYOUT;
    uint32 Version  = VERSION__LAYOUT;
    char * SaveDir  = LEVEL_SAVE_DIRECTORY;
    char * FileName = Save->current;
    
    if( Layout->nLevel > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        uint32 nShow = 0;
        for( uint32 iLevel = 0; iLevel < Layout->nLevel; iLevel++ ) {
            LAYOUT_LEVEL Level = Layout->Level[ iLevel ];
            if( Level.Show ) {
                nShow++;
            }
        }
        
        _writem( output, uint32, nShow );
        for( uint32 iLevel = 0; iLevel < Layout->nLevel; iLevel++ ) {
            LAYOUT_LEVEL Level = Layout->Level[ iLevel ];
            if( Level.Show ) {
                _writem( output, LAYOUT_LEVEL, Level );
            }
        }
        
        boo32 IsValid = OutputFile( Platform, output, SaveDir, FileName, FileTag );
        if( IsValid ) {
            Save->UnsavedChanges = false;
            Save->recentSave     = true;
        } else {
            char str[ 512 ] = {};
            sprintf( str, "ERROR! Error occurred when attempting to save File: %s.%s", Save->current, FILETAG__LAYOUT );
            CONSOLE_STRING( str );
            GlobalVar_DebugSystem.ErrorOccurred = true;
        }
        
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal LAYOUT_LEVEL
ReadLayoutLevel( uint32 Version, uint8 ** Ptr ) {
    LAYOUT_LEVEL Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            uint32 nChar = _ArrayCount( Result.FileName );
            uint32 nDiff = nChar % 4;
            if( nDiff != 0 ) {
                nDiff  = 4 - nDiff;
                nChar += nDiff;
            }
            
            boo32  Show     = _read( ptr, boo32 );
            char * FileName = _addra( ptr, char, nChar );
            vec2   Pos      = _read( ptr, vec2 );
            vec2   Dim      = _read( ptr, vec2 );
            
            LAYOUT_LEVEL Level = {};
            Level.Show = Show;
            strcpy( Level.FileName, FileName );
            Level.Pos  = Pos;
            Level.Dim  = Dim;
            
            Result = Level;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
LAYOUT_LoadLayout( PLATFORM * Platform, LAYOUT_STATE * Layout, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__LAYOUT;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        uint32 nShow = _read( ptr, uint32 );
        for( uint32 iShow = 0; iShow < nShow; iShow++ ) {
            LAYOUT_LEVEL Src = ReadLayoutLevel( Version, &ptr );
            
            int32 Index = -1;
            for( uint32 iLevel = 0; ( Index == -1 ) && ( iLevel < Layout->nLevel ); iLevel++ ) {
                LAYOUT_LEVEL Level = Layout->Level[ iLevel ];
                if( MatchString( Src.FileName, Level.FileName ) ) {
                    Index = iLevel;
                }
            }
            
            if( Index > -1 ) {
                LAYOUT_LEVEL * Level = Layout->Level + Index;
                Level->Show = true;
                Level->Pos  = Src.Pos;
            }
        }
        
        _popSize( TempMemory, File.size );
        
        FILE_SAVE_OPEN * Open = &Layout->SaveOpen;
        strcpy( Open->current, FileName );
    }
}