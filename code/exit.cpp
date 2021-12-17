
internal void
updateExit( APP_STATE * AppState ) {
    EXIT_STATE   * State  = &AppState->Exit;
    PLAYER_STATE * Player = &AppState->Player;
    
    vec2 PlayerP = Player->Position;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        EXIT Exit = State->Entity[ iEntity ];
        if( Exit.Type == ExitTypeS_Grate_Duck ) {
            if( ( IsInBound( PlayerP, Exit.Bound ) ) && ( Player->crouch_IsActive ) ) {
                Player->Draw_EnterIndicator = true;
            }
        }
    }
}

internal void
DrawExit( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    EXIT_STATE * State = Draw->Exit;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        EXIT Exit = State->Entity[ iEntity ];
        //DrawRectOutline( Pass, Exit.Bound, COLOR_GREEN );
        
        TEXTURE_ID TextureID = {};
        orect      R         = toORect( Exit.Bound );
        
        flo32 OffsetX = TILE_WIDTH * 1.5f;
        boo32 DoXFlip = false;
        boo32 DoYFlip = false;
        
        flo32 WalkT_OffsetX = TILE_WIDTH * 2.0f;
        
        flo32 GrateDuck_OffsetX = TILE_WIDTH  * 0.25f;
        flo32 GrateDuck_OffsetY = TILE_HEIGHT * 0.25f;
        flo32 GrateDrop_OffsetX = -TILE_HEIGHT * 0.25f;
        
        switch( Exit.Type ) {
            case ExitTypeS_Walk_EnterLeft: {
                TextureID = TextureID_ExitTypeS_Walk_EnterLeft;
                R = addLeft( R, OffsetX );
            } break;
            
            case ExitTypeS_Walk_EnterRight: {
                TextureID = TextureID_ExitTypeS_Walk_EnterLeft;
                R         = addRight( R, OffsetX );
                DoXFlip   = true;
            } break;
            
            case ExitTypeT_Walk_EnterLeft: {
                TextureID = TextureID_ExitTypeS_Walk_EnterLeft;
                R = addLeft( R, OffsetX );
            } break;
            
            case ExitTypeT_Walk_EnterUp: {
                TextureID = TextureID_ExitTypeS_Walk_EnterLeft;
                R.xAxis = Vec2(  0.0f, 1.0f );
                R.yAxis = Vec2( -1.0f, 0.0f );
                R.HalfDim = Vec2( R.HalfDim.y, R.HalfDim.x );
                R = addRight( R, WalkT_OffsetX );
            } break;
            
            case ExitTypeT_Walk_EnterRight: {
                TextureID = TextureID_ExitTypeS_Walk_EnterLeft;
                R         = addRight( R, OffsetX );
                DoXFlip   = true;
            } break;
            
            case ExitTypeT_Walk_EnterDown: {
                TextureID = TextureID_ExitTypeS_Walk_EnterLeft;
                R.xAxis = Vec2( 0.0f, -1.0f );
                R.yAxis = Vec2( 1.0f,  0.0f );
                R.HalfDim = Vec2( R.HalfDim.y, R.HalfDim.x );
                R = addRight( R, WalkT_OffsetX );
            } break;
            
            case ExitTypeS_Grate_Drop: {
                TextureID = TextureID_ExitTypeS_Walk_EnterLeft;
                R.xAxis = Vec2( 0.0f, 1.0f );
                R.yAxis = Vec2( 1.0f, 0.0f );
                R.HalfDim = Vec2( R.HalfDim.y, R.HalfDim.x );
                R = addLeft( R, GrateDrop_OffsetX );
            } break;
            
            case ExitTypeS_Grate_Duck: {
                TextureID = TextureID_ExitTypeS_Grate_Duck;
                R = addLeft ( R, GrateDuck_OffsetX );
                R = addRight( R, GrateDuck_OffsetX );
                R = addTop  ( R, GrateDuck_OffsetY );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        if( DoXFlip ) { R.xAxis = -R.xAxis; }
        if( DoYFlip ) { R.yAxis = -R.yAxis; }
        
        DrawORect( Pass, TextureID, R, COLOR_WHITE );
    }
}

internal WORLD__EXIT
getWorldExit( APP_STATE * AppState, uint32 iExit ) {
    WORLD_STATE * World = &AppState->World;
    ROOM        * Room  = World->Room + AppState->iRoom;
    
    WORLD__EXIT Result = Room->Exit[ iExit ];
    return Result;
}

internal boo32
ExitIsValid( WORLD__EXIT Exit ) {
    boo32 Result = ( Exit.toRoom_name[ 0 ] != 0 );
    return Result;
}

internal vec2
getEnterP( EXIT Exit, flo32 Height ) {
    flo32 offset = TILE_WIDTH * 4.0f;
    
    vec2 P = {};
    switch( Exit.Type ) {
        case ExitTypeS_Walk_EnterLeft: {
            P    = GetBL( Exit.Bound );
            P.x -= offset;
        } break;
        
        case ExitTypeS_Walk_EnterRight: {
            P    = GetBR( Exit.Bound );
            P.x += offset;
        } break;
        
        case ExitTypeT_Walk_EnterLeft: {
            P    = GetLC( Exit.Bound );
            P.x -= offset;
        } break;
        
        case ExitTypeT_Walk_EnterUp: {
            P    = GetTC( Exit.Bound );
            P.y += offset;
        } break;
        
        case ExitTypeT_Walk_EnterRight: {
            P    = GetRC( Exit.Bound );
            P.x += offset;
        } break;
        
        case ExitTypeT_Walk_EnterDown: {
            P    = GetBC( Exit.Bound );
            P.y -= offset;
        } break;
        
        case ExitTypeS_Grate_Drop: {
            P    = GetBC( Exit.Bound );
            P.y -= ( Height + TILE_HEIGHT * 1.0f );
        } break;
        
        case ExitTypeS_Grate_Duck: {
            P    = GetBC( Exit.Bound );
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    return P;
}

internal void
DrawExitDebug( RENDER_PASS * Pass, DRAW_STATE * Draw ) {
    EXIT_STATE * State = Draw->Exit;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        EXIT Exit = State->Entity[ iEntity ];
        
        vec2 P = GetCenter( Exit.Bound );
        vec4 Color = COLOR_GREEN;
        
        WORLD__EXIT e = getWorldExit( Draw->AppState, iEntity );
        if( ExitIsValid( e ) ) {
            char str[ 64 ] = {};
            sprintf( str, "%s, %u", e.toRoom_name, e.toRoom_iExit );
            DrawString( Pass, AppState->Font, str, P, TextAlign_Center, WORLD_DEBUG_TEXT_SCALE, COLOR_WHITE );
        } else {
            Color = COLOR_RED;
        }
        
        DrawRectOutline( Pass, Exit.Bound, Color );
    }
}

internal void
FinalizeExit( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    EXIT_STATE   * State  = &AppState->Exit;
    
    vec2 PlayerP = Player->Position;
    for( uint32 iEntity = 0; iEntity < State->nEntity; iEntity++ ) {
        EXIT Exit = State->Entity[ iEntity ];
        switch( Exit.Type ) {
            case ExitTypeS_Walk_EnterLeft  :
            case ExitTypeS_Walk_EnterRight :
            case ExitTypeT_Walk_EnterLeft  :
            case ExitTypeT_Walk_EnterUp    :
            case ExitTypeT_Walk_EnterRight :
            case ExitTypeT_Walk_EnterDown  : {
                if( IsInBound( PlayerP, Exit.Bound ) ) {
                    WORLD__EXIT e = getWorldExit( AppState, iEntity );
                    if( ExitIsValid( e ) ) {
                        AppState->ChangeRoom = true;
                        strcpy( AppState->ChangeRoom_Name, e.toRoom_name );
                        AppState->ChangeRoom_iExit       = e.toRoom_iExit;
                        AppState->ChangeRoom_iJumperEdge = e.toRoom_iJumperEdge;
                    }
                }
            } break;
            
            case ExitTypeS_Grate_Duck: {
                if( ( IsInBound( PlayerP, Exit.Bound ) ) && ( Player->crouch_IsActive ) && ( Player->Use_WasPressed ) ) {
                    WORLD__EXIT e = getWorldExit( AppState, iEntity );
                    if( ExitIsValid( e ) ) {
                        AppState->ChangeRoom = true;
                        strcpy( AppState->ChangeRoom_Name, e.toRoom_name );
                        AppState->ChangeRoom_iExit       = e.toRoom_iExit;
                        AppState->ChangeRoom_iJumperEdge = e.toRoom_iJumperEdge;
                    }
                }
            } break;
        }
    }
}