
#define VERSION__LEVEL_STATS  ( 2 )
#define FILETAG__LEVEL_STATS  ( "ROOMSTAT" )
internal void
SaveLevelStats( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__LEVEL_STATS;
    uint32 Version = VERSION__LEVEL_STATS;
    uint32 catalog_size = 0;
    
    uint32 output_size = sizeof( ENTITY_FILE_HEADER ) + catalog_size;
    output_size += sizeof( LEVEL_STATS );
    
    MEMORY  _output = SubArena( TempMemory, output_size );
    MEMORY * output = &_output;
    
    WriteEntityFileHeader( output, FileTag, Version );
    
    _writem( output, LEVEL_STATS, Editor->Stat );
    
    OutputFile( Platform, output, SaveDir, FileName, FileTag );
    _popSize( TempMemory, output->size );
}

internal LEVEL_STATS
ReadLevelStats( uint32 Version, uint8 ** Ptr ) {
    LEVEL_STATS Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect  Bound       = _read( ptr, rect );
            flo32 EnterLeftY  = _read( ptr, flo32 );
            flo32 EnterRightY = _read( ptr, flo32 );
            
            LEVEL_STATS Stat = {};
            Stat.Bound       = Bound;
            Stat.EnterLeftY  = EnterLeftY;
            Stat.EnterRightY = EnterRightY;
            
            Result = Stat;
        } break;
        
        case 2: {
            rect  Bound        = _read( ptr, rect );
            flo32 EnterLeftY   = _read( ptr, flo32 );
            flo32 EnterRightY  = _read( ptr, flo32 );
            flo32 EnterBottomX = _read( ptr, flo32 );
            flo32 EnterTopX    = _read( ptr, flo32 );
            
            LEVEL_STATS Stat = {};
            Stat.Bound        = Bound;
            Stat.EnterLeftY   = EnterLeftY;
            Stat.EnterRightY  = EnterRightY;
            Stat.EnterBottomX = EnterBottomX;
            Stat.EnterTopX    = EnterTopX;
            
            Result = Stat;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal LEVEL_STATS
LAYOUT_LoadLevelStats( PLATFORM * Platform, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    LEVEL_STATS Result = {};
    
    char * FileTag = FILETAG__LEVEL_STATS;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        LEVEL_STATS Stat = ReadLevelStats( Version, &ptr );
        Result = Stat;
        
        _popSize( TempMemory, File.size );
    }
    
    return Result;
}

internal void
GAME_LoadLevelStats( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__LEVEL_STATS;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        LEVEL_STATS Stat = ReadLevelStats( Version, &ptr );
        AppState->Stat = Stat;
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadLevelStats( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__LEVEL_STATS;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        EDITOR_STATE * Editor = &AppState->Editor;
        
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        LEVEL_STATS Stat = ReadLevelStats( Version, &ptr );
        Editor->Stat = Stat;
        
        _popSize( TempMemory, File.size );
    }
}