
internal void
InitLayoutPanel( APP_STATE * AppState ) {
    LAYOUT_STATE * Layout = &AppState->Layout;
    PANEL        * Panel  = &Layout->Panel;
    
    { // set Bound
        rect R = AppState->App_Bound;
        R.Left = R.Right - 320.0f;
        
        Panel->Bound = R;
    }
}

internal void
UpdateAndDrawLayoutPanel( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI ) {
    LAYOUT_STATE * Layout = &AppState->Layout;
    PANEL        * Panel  = &Layout->Panel;
    
    flo32 Margin = 4.0f;
    Panel->Inner = AddRadius( Panel->Bound, -Margin );
    Panel->AtPos = GetTL( Panel->Inner );
    
    DrawRect( Pass_UI, Panel->Bound, COLOR_WHITE );
    
    // SCROLLING
    vec2  Dim     = Vec2( GetWidth( Panel->Inner ) - 4.0f, 24.0f );
    flo32 MarginY = 2.0f;
    flo32 List_Height  = ( flo32 )Layout->nLevel * Dim.y + ( flo32 )( Layout->nLevel - 1 ) * MarginY;
    flo32 Panel_Height = GetHeight( Panel->Inner );
    
    if( List_Height > Panel_Height ) {
        flo32 Scroll_Height = List_Height - Panel_Height;
        Panel->AtPos.y += Scroll_Height * Panel->ScrollT;
        
        uint32 nEntriesPerScroll = 1;
        flo32 ScrollT = ( flo32 )( ( Dim.y + MarginY ) * ( flo32 )nEntriesPerScroll ) / Scroll_Height;
        if( Mouse->WheelClicks != 0 ) {
            Panel->ScrollT = Clamp01( Panel->ScrollT - ( ( flo32 )Mouse->WheelClicks * ScrollT ) );
        }
        
        flo32 Bar_Height = Clamp01( Panel_Height / List_Height ) * Panel_Height;
        flo32 Bar_Y      = Panel->ScrollT * ( Panel_Height - Bar_Height );
        rect  Bar_Bound  = RectTRD( GetTR( Panel->Inner ) + Vec2( 0.0f, -Bar_Y ), Vec2( 2.0f, Bar_Height ) );
        DrawRect( Pass_UI, Bar_Bound, COLOR_BLACK );
    }
    
#if 0
    INPUT_STRING * Filter = &Layout->Filter_String;
    { // Filter string
        vec2 dim = Vec2( GetWidth( Panel->Inner ), 28.0f );
        rect R   = rectTLD( Panel->AtPos, dim );
        
        DrawRect( Pass_UI, R, COLOR_GRAY( 0.95f ) );
        DrawRectOutline( Pass_UI, R, COLOR_BLACK );
        
        if( Filter->nChar > 0 ) {
            DrawString( Pass_UI, AppState->font, Filter->string, Panel->AtPos + Vec2( 4.0f, -4.0f ), textAlign_TopLeft, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
        }
        
        Panel->AtPos.y -= ( dim.y + Margin );
    }
#endif
    
    { // File List
        for( uint32 iLevel = 0; iLevel < Layout->nLevel; iLevel++ ) {
            LAYOUT_LEVEL * Level = Layout->Level + iLevel;
            //if( FindSubstringNoCase( Level->name, Filter->string ) ) {
            rect R = RectTLD( Panel->AtPos, Dim );
            if( Level->Show ) {
                DrawRect( Pass_UI, R, COLOR_GRAY( 0.8f ) );
            }
            DrawString( Pass_UI, AppState->Font, Level->FileName, Panel->AtPos + Vec2( 6.0f, -4.0f ), TextAlign_TopLeft, COLOR_BLACK );
            
            if( IsInBound( Mouse->Position, R ) ) {
                DrawRectOutline( Pass_UI, R, COLOR_BLACK );
                if( WasPressed( Mouse, MouseButton_Left ) ) {
                    CAMERA_STATE * Camera = &Layout->Camera;
                    if( Level->Show ) {
                        Camera->Pos = Level->Pos;
                    } else {
                        Level->Show = true;
                        Level->Pos  = Camera->Pos;
                    }
                }
            }
            
            Panel->AtPos.y -= ( Dim.y + 2.0f );
            //}
        }
    }
}