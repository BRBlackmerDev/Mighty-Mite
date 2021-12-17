
internal void
InitDebugPanel( APP_STATE * AppState ) {
    PANEL * Panel = &AppState->Debug;
    
    {
        rect R = AppState->App_Bound;
        R.Left = R.Right - 320.0f;
        
        Panel->Bound = R;
    }
}

#if 0
internal void
DEBUG_addButton( PANEL * Panel, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, DRAW_STATE * Draw, char * label, RENDER_FUNC * func ) {
    vec2 button_Dim   = Vec2( 44.0f, 28.0f );
    vec2 Inner_margin = Vec2( 4.0f, 4.0f );
    
    rect Bound = RectTLD( Panel->AtPos, button_Dim );
    
    Panel->AtPos.x += ( button_Dim.x + Inner_margin.x );
    if( ( Panel->AtPos.x + button_Dim.x + Inner_margin.x ) >= Panel->Inner.Right ) {
        Panel->AtPos.x  = Panel->Inner.Left;
        Panel->AtPos.y -= ( button_Dim.y + Inner_margin.y );
    }
    
    if( ( WasPressed( Mouse, MouseButton_Left ) ) && ( IsInBound( Mouse->Position, Bound ) ) ) {
        Panel->IsEnabled[ Panel->nButton ] = !Panel->IsEnabled[ Panel->nButton ];
    }
    
    if( Panel->IsEnabled[ Panel->nButton ] ) {
        func( Pass_Game, Draw );
    }
    
    vec4 Color = COLOR_GRAY( 0.8f );
    if( Panel->IsEnabled[ Panel->nButton ] ) {
        Color = Vec4( 0.8f, 0.4f, 0.4f, 1.0f );
    }
    DrawRect( Pass_UI, Bound, Color );
    DrawRectOutline( Pass_UI, Bound, COLOR_BLACK );
    DrawString( Pass_UI, AppState->Font, label, GetCenter( Bound ), TextAlign_Center, Vec2( 1.0f, 1.0f ), COLOR_BLACK );
    
    Panel->nButton++;
}
#endif

internal void
UpdateAndDrawDebugPanel( APP_STATE * AppState, MOUSE_STATE * Mouse, RENDER_PASS * Pass_Game, RENDER_PASS * Pass_UI, DRAW_STATE * Draw ) {
    PANEL * Panel = &AppState->Debug;
    if( Panel->Show ) {
        Panel->Inner = AddRadius( Panel->Bound, -4.0f );
        Panel->AtPos = GetTL( Panel->Inner );
        Panel->advanceY = 0.0f;
        
        DrawRect( Pass_UI, Panel->Bound, COLOR_WHITE );
        
        addLabel( Panel, Pass_UI, AppState->Font, "TERRAIN", PANEL_LABEL_Y );
        wrapPanel( Panel );
        
        boo32 * IsActive = AppState->Debug_IsActive;
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "COLLSION", DrawCollisionEdges, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "JUMP", DrawJumpBound, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "WALLJUMP", DrawWallSlideBound, IsActive++ );
        wrapPanel( Panel );
        
        wrapPanel( Panel );
        //AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "GRND", DrawGrenadeDebug,  IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "CHECKPNT", DrawCheckpointDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "CONVEYOR", DrawConveyorDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "SCAFFOLD", DrawScaffoldDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "COLLAPSE", DrawCollapsePlatformDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "SPIKES", DrawSpikesDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "PUSHBLCK", DrawPushBlockDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "EXERBALL", DrawExerciseBallDebug, IsActive++ );
        
        wrapPanel( Panel );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "RUNNER", DrawRunnerDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "PUNCHER", DrawPuncherDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "SPOTTER", DrawSpotterDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "STMP DMG", DrawStomperDebug_DamagePlayer, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "STMPPNCH", DrawStomperDebug_PunchedByPlayer, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "CAMPER", DrawCamperDebug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "MINBOSS1", DrawExerciseMiniBoss01Debug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "MINBOSS2", DrawExerciseMiniBoss02Debug, IsActive++ );
        AddDebugButton( Panel, Mouse, Pass_Game, Pass_UI, AppState, Draw, "EXERBOSS", DrawExerciseBossDebug, IsActive++ );
        
        Assert( ( IsActive - AppState->Debug_IsActive ) <= DEBUG_MAX_COUNT );
    }
}