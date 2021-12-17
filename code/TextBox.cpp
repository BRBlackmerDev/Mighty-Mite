
internal void
AddTextBox( APP_STATE * AppState, TEXT_BOX_SPEAKER_ID SpeakerID, char * Text, int32 NextText = -1 ) {
    TEXT_BOX_STATE * TextBoxS = &AppState->TextBoxS;
    
    if( TextBoxS->nTextBox < TEXT_BOX_MAX_COUNT ) {
        TEXT_BOX TextBox = {};
        TextBox.SpeakerID = SpeakerID;
        strcpy( TextBox.Text, Text );
        TextBox.NextText  = NextText;
        
        TextBoxS->TextBox[ TextBoxS->nTextBox++ ] = TextBox;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new TEXT_BOX, but the state is full! Max Count = %u", TEXT_BOX_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
ShowTextBox( APP_STATE * AppState, uint32 iTextBox ) {
    TEXT_BOX_STATE * TextBoxS = &AppState->TextBoxS;
    TextBoxS->Show          = true;
    TextBoxS->Show_iTextBox = iTextBox;
}

internal void
UpdateTextBox( APP_STATE * AppState, flo32 dT ) {
    
}

internal void
DrawTextBox( RENDER_PASS * Pass, APP_STATE * AppState ) {
    TEXT_BOX_STATE * TextBoxS = &AppState->TextBoxS;
    
    if( TextBoxS->Show ) {
        TEXT_BOX TextBox = TextBoxS->TextBox[ TextBoxS->Show_iTextBox ];
        
        vec2 Pos = GetT( AppState->App_Bound, Vec2( 0.5f, 0.05f ) );
        vec2 Dim = AppState->App_Dim * Vec2( 0.6f, 0.15f );
        
        rect R = RectBCD( Pos, Dim );
        
        DrawRect( Pass, R, COLOR_GRAY( 0.02f ) );
        DrawRectOutline( Pass, R, COLOR_GRAY( 0.2f ) );
        
        // Draw Speaker
        DrawString( Pass, AppState->Font, TextBox.Text, GetTL( R ), TextAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_WHITE );
    }
}

internal void
FinalizeTextBox( CONTROLLER_STATE * Control, APP_STATE * AppState ) {
    TEXT_BOX_STATE * TextBoxS = &AppState->TextBoxS;
    if( TextBoxS->Show ) {
        CONTROLLER_BUTTON_ID ID_Next = ControllerButton_A;
        CONTROLLER_BUTTON_ID ID_Skip = ControllerButton_Back;
        if( WasPressed( Control, ID_Next ) ) {
            TEXT_BOX TextBox = TextBoxS->TextBox[ TextBoxS->Show_iTextBox ];
            
            TextBoxS->Show_iTextBox = TextBox.NextText;
            if( TextBoxS->Show_iTextBox == -1 ) {
                TextBoxS->Show = false;
            }
        }
    }
}