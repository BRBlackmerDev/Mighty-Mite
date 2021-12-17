
internal void
InitEditorPanel( APP_STATE * AppState ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Panel;
    
    { // set Bound
        rect R = AppState->App_Bound;
        R.Left = R.Right - 320.0f;
        
        Panel->Bound = R;
    }
}

internal void
EDITOR_addToggleButton( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, char * label, boo32 * value ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Panel;
    
    vec2 button_Dim = Vec2( 44.0f, 28.0f );
    rect Bound = GetBound( Panel, button_Dim );
    
    if( ( WasPressed( Mouse, MouseButton_Left ) ) && ( IsInBound( Mouse->Position, Bound ) ) ) {
        *value = !( *value );
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( *value ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    }
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->Font, label, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}

internal boo32
EDITOR_addPressButton( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, char * label, vec2 Dim ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Panel;
    
    rect  Bound   = GetBound( Panel, Dim );
    boo32 isHover = IsInBound( Mouse->Position, Bound );
    
    boo32 Result = false;
    if( ( WasPressed( Mouse, MouseButton_Left ) ) && ( isHover ) ) {
        Result = true;
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( Result ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    } else if( isHover ) {
        Color = COLOR_GRAY( 0.5f );
    }
    
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->Font, label, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    return Result;
}

internal boo32
EDITOR_addEnumButton( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, char ** label, uint32 nLabel, uint32 * value ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Panel;
    
    boo32 Result = false;
    
    vec2 button_Dim = Vec2( 44.0f, 28.0f );
    rect Bound = GetBound( Panel, button_Dim );
    
    boo32 isHover = IsInBound( Mouse->Position, Bound );
    uint32 m = *value;
    
    if( ( Mouse->WheelClicks != 0 ) && ( isHover ) ) {
        uint32 n = abs( Mouse->WheelClicks );
        if( ( n % nLabel ) != 0 ) {
            m = ( m + ( nLabel * 4 ) + Mouse->WheelClicks ) % nLabel;
            *value = m;
            
            Result = true;
        }
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( Result ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    } else if( isHover ) {
        Color = COLOR_GRAY( 0.5f );
    }
    
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->Font, label[ m ], GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    return Result;
}

internal void
EDITOR_AddModeButton( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, char * label, EDITOR_MODE Mode ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Panel;
    
    vec2 button_Dim = Vec2( 72.0f, 28.0f );
    rect Bound = GetBound( Panel, button_Dim );
    
    if( ( WasPressed( Mouse, MouseButton_Left ) ) && ( IsInBound( Mouse->Position, Bound ) ) ) {
        Editor->Mode = Mode;
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( Editor->Mode == Mode ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    }
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->Font, label, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}

internal void
EDITOR_addU32Counter( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass, char * label, uint32 * value ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Panel;
    
    vec2 counter_Dim = Vec2( 72.0f, 28.0f );
    rect Bound = GetBound( Panel, counter_Dim );
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( IsInBound( Mouse->Position, Bound ) ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
        if( Mouse->WheelClicks != 0 ) {
            if( Mouse->WheelClicks < 0 ) {
                uint32 sub = MinValue( *value, ( uint32 )abs( Mouse->WheelClicks ) );
                *value -= sub;
            } else {
                *value += Mouse->WheelClicks;
            }
        }
    }
    
    DrawRect( Pass, Bound, Color );
    DrawRectOutline( Pass, Bound, COLOR_BLACK );
    
    char str[ 32 ] = {};
    sprintf( str, "%s: %u", label, *value );
    DrawString( Pass, AppState->Font, str, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
}

internal void
EDITOR_addEvent( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, EDITOR__EVENT * Event, uint32 iEvent ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Panel;
    
    flo32 name_Height = 18.0f;
    vec2 Inner_margin = Vec2( 4.0f, 4.0f );
    vec2 data_Dim = Vec2( GetWidth( Panel->Inner ) - ( Inner_margin.x * 2.0f ), 18.0f );
    
    flo32 Height = ( flo32 )Event->nData * ( data_Dim.y + Inner_margin.y ) + name_Height + Inner_margin.y * 2.0f;
    
    vec2 bPos = Panel->AtPos;
    vec2 bDim = Vec2( GetWidth( Panel->Inner ), Height );
    rect B    = RectTLD( bPos, bDim );
    vec4 bColor   = COLOR_GRAY( 0.9f );
    vec4 bOutline = COLOR_BLACK;
    if( ( Editor->Mode == EditorMode_Event ) && ( Editor->Event_iEvent == iEvent ) ) {
        bOutline = COLOR_RED;
    }
    if( Event->IsActive ) {
        bColor = Vec4( 0.0f, 0.4f, 0.0f, 1.0f );
    }
    DrawRect       ( Pass_UI, B, bColor );
    DrawRectOutline( Pass_UI, B, bOutline );
    
    vec2 AtPos = bPos;
    
    DrawString( Pass_UI, AppState->Font, Event->name, bPos + Vec2( Inner_margin.x * 2.0f, -Inner_margin.y ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    if( Event->nData == 0 ) {
        DrawString( Pass_UI, AppState->Font, "*NO DATA*", GetTR( B ) + Vec2( -Inner_margin.x, -Inner_margin.y ), TextAlign_TopRight, Vec2( 1.0f, 1.0f ), COLOR_GRAY( 0.6f ) );
    }
    
    AtPos.y -= ( name_Height + ( Inner_margin.y * 2.0f ) );
    AtPos.x += Inner_margin.x;
    
    Panel->AtPos.y -= ( Height + Inner_margin.y );
    
    DATA_TYPE_LABELS;
    
    for( uint32 iData = 0; iData < Event->nData; iData++ ) {
        rect R = RectTLD( AtPos, data_Dim );
        
        boo32 doHover = IsInBound( Mouse->Position, R );
        if( ( doHover ) && ( WasPressed( Mouse, MouseButton_Left ) ) ) {
            Editor->Mode = EditorMode_Event;
            
            Editor->Event_Type = Event->data_Type[ iData ];
            Editor->Event_data = Event->data_ptr [ iData ];
            Editor->Event_iEvent = iEvent;
            Editor->Event_iData  = iData;
        }
        
        vec4 rColor = COLOR_GRAY( 0.7f );
        if( IsInBound( Mouse->Position, R ) ) {
            rColor = COLOR_GRAY( 0.5f );
        }
        if( ( Editor->Mode == EditorMode_Event ) && ( Editor->Event_iEvent == iEvent ) && ( Editor->Event_iData == iData ) ) {
            rColor = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
        }
        DrawRect       ( Pass_UI, R, rColor   );
        DrawRectOutline( Pass_UI, R, COLOR_BLACK );
        
        char str[ 64 ] = {};
        sprintf( str, "%s : %s", dataTypeLabel[ Event->data_Type[ iData ] ], Event->data_name[ iData ] );
        DrawString( Pass_UI, AppState->Font, str, AtPos + Vec2( Inner_margin.x, -1.0f ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
        
        AtPos.y -= ( data_Dim.y + Inner_margin.y );
        
        switch( Event->data_Type[ iData ] ) {
            case dataType_point: {
                vec2 P = ( ( vec2 * )Event->data_ptr[ iData ] )[ 0 ];
                DrawPoint( Pass_Game, P, TILE_DIM * 0.5f, COLOR_YELLOW );
                
                if( doHover ) {
                    DISPLAY_VALUE( vec2, P );
                }
            } break;
            
            case dataType_rect: {
                rect S = ( ( rect * )Event->data_ptr[ iData ] )[ 0 ];
                DrawRectOutline( Pass_Game, S, COLOR_YELLOW );
                
                if( doHover ) {
                    DISPLAY_VALUE( rect, S );
                }
            } break;
        }
    }
}

internal void
DrawEditorGrid( RENDER_PASS * Pass, APP_STATE * AppState ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    CAMERA_STATE * Camera = &Editor->Camera;
    
    vec2 P = Camera->Pos - Camera->Dim * 0.5f;
    UINT32_PAIR Cell = GetCellIndexFromPos( P );
    
    Cell.x -= ( Cell.x % 4 );
    Cell.y -= ( Cell.y % 4 );
    
    uint32 scale = 4;
    uint32 nX = ( ( uint32 )( Camera->Dim.x / TILE_WIDTH  ) / 4 ) + 1;
    uint32 nY = ( ( uint32 )( Camera->Dim.y / TILE_HEIGHT ) / 4 ) + 1;
    vec2 offset = Vec2( 4.0f, 4.0f ) * TILE_DIM;
    
    vec2 Q = GetWorldPos( Cell.x, Cell.y );
    
    vec2 AtPos = Q;
    for( uint32 iX = 0; iX < nX; iX++ ) {
        vec2 A = AtPos;
        vec2 B = AtPos + Vec2( 0.0f, ( flo32 )nY * offset.y );
        
        DrawLine( Pass, A, B, Vec4( 0.1f, 0.1f, 0.0f, 1.0f ) );
        
        AtPos.x += offset.x;
    }
    
    AtPos = Q;
    for( uint32 iX = 0; iX < nX; iX++ ) {
        vec2 A = AtPos;
        vec2 B = AtPos + Vec2( ( flo32 )nX * offset.x, 0.0f );
        
        DrawLine( Pass, A, B, Vec4( 0.1f, 0.1f, 0.0f, 1.0f ) );
        
        AtPos.y += offset.y;
    }
}

internal void
GenEventsFromCpp( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory ) {
    char * SaveDir = LEVEL_SAVE_DIRECTORY;
    char * FileTag = FILETAG__EVENTS_EDIT;
    uint32 Version = VERSION__EVENTS_EDIT;
    
    EDITOR_STATE * Editor   = &AppState->Editor;
    FILE_SAVE_OPEN SaveOpen = Editor->SaveOpen;
    
    char * FileName = SaveOpen.current;
    
    FILE_DATA File_cpp = Platform->ReadFile( TempMemory, CODE_DIRECTORY, FileName, "cpp" );
    if( File_cpp.contents ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        uint32   nEvent  = 0;
        uint32 * nEvent0 = _pushType( output, uint32 );
        
        FILE_PARSER  _parser = FileParser( File_cpp.contents, File_cpp.size );
        FILE_PARSER * parser = &_parser;
        
        while( hasTextRemaining( parser ) ) {
            STRING token = parseToken( parser );
            
            if( MatchString( token, "internal" ) ) {
                parseToken( parser ); // void
                
                uint32 Nest = 0;
                STRING tokenA = {};
                STRING tokenB = {};
                STRING tokenC = {};
                STRING tokenD = {};
                
                STRING func_name = parseToken( parser );
                Assert( func_name.string[ func_name.nChar - 1 ] == '(' );
                func_name.nChar--;
                
                nEvent++;
                writeString( output, func_name.string, func_name.nChar );
                
                boo32 * IsActive = _pushType( output, boo32 );
                
                uint32   nData  = 0;
                uint32 * nData0 = _pushType( output, uint32 );
                
                int32 iEvent0 = -1;
                for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
                    EDITOR__EVENT e = Editor->Event[ iEvent ];
                    if( MatchString( func_name, e.name ) ) {
                        iEvent0 = iEvent;
                    }
                }
                
                DATA_TYPE_LABELS;
                DATA_TYPE_TOKENS;
                DATA_TYPE_SIZES;
                
                boo32 doParse = true;
                while( doParse ) {
                    tokenA = parseToken( parser );
                    
                    if( MatchString( tokenA, "_read(" ) ) {
                        int32 i = -1;
                        for( uint32 iToken = 0; iToken < dataType_Count; iToken++ ) {
                            if( MatchSegment( tokenD.string, dataTypeToken[ iToken ], tokenD.nChar ) ) {
                                i = iToken;
                            }
                        }
                        
                        writeSegment( output, dataTypeLabel[ i ] );
                        writeString ( output, tokenC.string, tokenC.nChar );
                        uint8 * data = ( uint8 * )_pushSize( output, dataTypeSize[ i ] );
                        
                        int32 iData0 = -1;
                        if( iEvent0 > -1 ) {
                            EDITOR__EVENT e = Editor->Event[ iEvent0 ];
                            
                            for( uint32 iData = 0; iData < e.nData; iData++ ) {
                                if( MatchString( tokenC, e.data_name[ iData ] ) ) {
                                    iData0 = iData;
                                }
                            }
                            
                            if( iData0 > -1 ) {
                                memcpy( data, e.data_ptr[ iData0 ], dataTypeSize[ i ] );
                            }
                        }
                        
                        nData++;
                    } else if( MatchString( tokenA, "//ACTIVE//" ) ) {
                        *IsActive = true;
                    } else if( MatchString( tokenA, "{" ) ) {
                        Nest++;
                    } else if( MatchString( tokenA, "}" ) ) {
                        Assert( Nest > 0 );
                        
                        Nest--;
                        if( Nest == 0 ) {
                            doParse = false;
                        }
                    }
                    
                    tokenD = tokenC;
                    tokenC = tokenB;
                    tokenB = tokenA;
                }
                
                *nData0 = nData;
            }
        }
        
        *nEvent0 = nEvent;
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag ); 
        _popSize( TempMemory, output->size );
        
        _popSize( TempMemory, File_cpp.size );
    }
    
}

internal void
UpdateAndDrawEditorPanel( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI ) {
    EDITOR_STATE * Editor = &AppState->Editor;
    PANEL        * Panel  = &Editor->Panel;
    
    if( Panel->Show ) {
        vec2  button_Dim = Vec2( 40.0f, 16.0f );
        flo32 labelY     = 14.0f;
        flo32 labelPreY  = 8.0f;
        
        Panel->Inner = AddRadius( Panel->Bound, -4.0f );
        Panel->AtPos = GetTL( Panel->Inner );
        Panel->advanceY = 0.0f;
        
        DrawRect( Pass_UI, Panel->Bound, COLOR_WHITE );
        
        EDITOR_addToggleButton( AppState, Mouse, Pass_Game, Pass_UI, "GRID", &Editor->DrawGrid );
        
        wrapPanel( Panel );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "MOVE",     EditorMode_MoveEntity );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "COPY",     EditorMode_CopyEntity );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "MIRROR",   EditorMode_MirrorEntity );
        wrapPanel( Panel );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "LEVLBOND", EditorMode_LevelBound );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "CHECKPNT", EditorMode_Checkpoint );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "TERRAIN",  EditorMode_Terrain );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "CONVEYOR", EditorMode_Conveyor );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "SCAFFOLD", EditorMode_Scaffold );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "SEESAW",   EditorMode_Seesaw );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "SPIKES",   EditorMode_Spikes );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "CYCLEBLK", EditorMode_CycleBlock );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "PUSHBLCK", EditorMode_PushBlock );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "BREAKBLK", EditorMode_BreakBlock );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "BOULDER",  EditorMode_Boulder );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "EXERBALL", EditorMode_ExerciseBall );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "COLLAPSE", EditorMode_CollapsePlatform );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "PUNCHER",  EditorMode_Puncher );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "BARBELL",  EditorMode_Barbell );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "MONEY",    EditorMode_Money   );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "POPPER",   EditorMode_Popper );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "SPOTTER",  EditorMode_Spotter );
        EDITOR_AddModeButton( AppState, Mouse, Pass_Game, Pass_UI, "CAMPER",   EditorMode_Camper );
        
        DRAW_STATE  _Draw = {};
        DRAW_STATE * Draw = &_Draw;
        
        boo32 * IsActive = AppState->Debug_IsActive;
        { // Draw Debug Collision
            wrapPanel( Panel, labelPreY );
            addLabel( Panel, Pass_UI, AppState->Font, "COLLISION", labelY );
            
            wrapPanel( Panel );
            AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "COLLSION", DrawCollisionEdges, IsActive++ );
            AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "JUMP", DrawJumpBound,      IsActive++ );
            AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "WALLJUMP", DrawWallSlideBound, IsActive++ );
            
            Assert( ( IsActive - AppState->Debug_IsActive ) <= DEBUG_MAX_COUNT );
        }
        
        wrapPanel( Panel, labelPreY );
        addLabel( Panel, Pass_UI, AppState->Font, "EVENTS:", labelY );
        
        FILE_SAVE_OPEN SaveOpen = Editor->SaveOpen;
        if( SaveOpen.current[ 0 ] != 0 ) {
            boo32 isPressed = EDITOR_addPressButton( AppState, Mouse, Pass_Game, Pass_UI, "GEN", button_Dim );
            if( isPressed ) {
                GenEventsFromCpp( Platform, AppState, TempMemory );
                
                Reset( &Editor->Event_Names );
                memset( &Editor->Reset_Event_Start, 0, &Editor->Reset_Event_end - &Editor->Reset_Event_Start );
                
                EDITOR_LoadEvents( Platform, AppState, TempMemory, LEVEL_SAVE_DIRECTORY, SaveOpen.current );
            }
        }
        
        wrapPanel( Panel );
        for( uint32 iEvent = 0; iEvent < Editor->nEvent; iEvent++ ) {
            EDITOR__EVENT * Event = Editor->Event + iEvent;
            EDITOR_addEvent( AppState, Mouse, Pass_Game, Pass_UI, Event, iEvent );
        }
    }
}