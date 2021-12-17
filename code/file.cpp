
internal void
Reset( FILE_SAVE_OPEN * State ) {
    State->overwrite      = false;
    State->UnsavedChanges = false;
    State->recentSave     = false;
    State->Timer          = 0.0f;
    State->ConfirmNew     = false;
    memset( State->current, 0, FILE_SAVE_OPEN__FILENAME_MAX_CHAR );
}

internal char *
FindSubstring( char * StringA, char * StringB ) {
    int32 LengthA = ( int32 )strlen( StringA );
    int32 LengthB = ( int32 )strlen( StringB );
    
    char * Result = 0;
    
    int32 Range = LengthA - LengthB + 1;
    
    boo32 FoundMatch = false;
    for( int32 iChar = 0; ( !FoundMatch ) && ( iChar < Range ); iChar++ ) {
        boo32 FoundSegment = MatchSegment( StringA + iChar, StringB, LengthB );
        if( FoundSegment ) {
            FoundMatch = true;
            Result     = StringA + iChar;
        }
    }
    return Result;
}

internal char *
FindSubstringNoCase( char * StringA, char * StringB ) {
    int32 LengthA = ( int32 )strlen( StringA );
    int32 LengthB = ( int32 )strlen( StringB );
    
    char * Result = 0;
    
    int32 Range = LengthA - LengthB + 1;
    
    boo32 FoundMatch = false;
    for( int32 iChar = 0; ( !FoundMatch ) && ( iChar < Range ); iChar++ ) {
        boo32 FoundSegment = MatchSegmentNoCase( StringA + iChar, StringB, LengthB );
        if( FoundSegment ) {
            FoundMatch = true;
            Result     = StringA + iChar;
        }
    }
    return Result;
}

internal void
DrawAndUpdateFileListForSaveOpen( RENDER_PASS * Pass, KEYBOARD_STATE * Keyboard, APP_STATE * AppState, FILE_SAVE_OPEN * SaveOpen ) {
    INPUT_STRING * i = &SaveOpen->input_string;
    
    vec2 AtPos = GetTL( AppState->App_Bound ) + Vec2( 4.0f, -4.0f );
    vec2 Dim   = Vec2( 512.0f, 18.0f );
    rect Bound = RectTLD( AtPos, Dim );
    vec4 Color = COLOR_GRAY( 0.1f );
    if( SaveOpen->overwrite ) {
        Color = Vec4( 0.6f, 0.0f, 0.0f, 1.0f );
    }
    DrawRect( Pass, Bound, Color );
    
    AtPos += Vec2( 2.0f, -2.0f );
    DrawString( Pass, AppState->Font, "FILENAME:", AtPos, TextAlign_TopLeft, COLOR_WHITE );
    vec2 Select_AtPos = AtPos + Vec2( 100.0f, 0.0f );
    
    DrawString( Pass, AppState->Font, i->string, AtPos + Vec2( 100.0f, 0.0f ), TextAlign_TopLeft, COLOR_WHITE );
    
    uint32 nDisplay = 0;
    
    STRING_BUFFER * FileList = &SaveOpen->FileList;
    AtPos += Vec2( 8.0f, -28.0f );
    vec2 bPos = AtPos;
    for( uint32 iFile = 0; iFile < FileList->nStr; iFile++ ) {
        char * FileName = FileList->str[ iFile ];
        if( FindSubstringNoCase( FileName, i->string ) ) {
            DrawString( Pass, AppState->Font, FileList->str[ iFile ], AtPos, TextAlign_TopLeft, COLOR_WHITE );
            AtPos.y -= 20.0f;
            
            nDisplay++;
        }
    }
    
    SaveOpen->Select_iFile = -1;
    if( nDisplay > 0 ) {
        SaveOpen->Select_iSelect = MinValue( SaveOpen->Select_iSelect, nDisplay - 1 );
        uint32 iSelect = 0;
        
        if( WasPressed( Keyboard, KeyCode_Up ) ) {
            SaveOpen->Select_iSelect = UInt32Wrap( SaveOpen->Select_iSelect, -1, nDisplay );
        }
        if( WasPressed( Keyboard, KeyCode_Down ) ) {
            SaveOpen->Select_iSelect = UInt32Wrap( SaveOpen->Select_iSelect,  1, nDisplay );
        }
        
        AtPos = bPos;
        for( uint32 iFile = 0; iFile < FileList->nStr; iFile++ ) {
            char * FileName = FileList->str[ iFile ];
            if( FindSubstringNoCase( FileName, i->string ) ) {
                if( SaveOpen->Select_iSelect == iSelect ) {
                    vec2 RectDim   = Vec2( 512.0f, 18.0f );
                    rect RectBound = RectTLD( AtPos, RectDim );
                    DrawRectOutline( Pass, RectBound, COLOR_GRAY( 0.5f ) );
                    
                    SaveOpen->Select_iFile = iFile;
                }
                
                AtPos.y -= 20.0f;
                iSelect++;
            }
        }
    }
    
    DISPLAY_VALUE( uint32, nDisplay );
    DISPLAY_VALUE( uint32, SaveOpen->Select_iSelect );
    DISPLAY_VALUE( int32, SaveOpen->Select_iFile );
}