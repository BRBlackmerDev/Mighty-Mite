
internal void
AddStomper( APP_STATE * AppState, vec2 Pos ) {
    STOMPER_STATE * StomperS = &AppState->StomperS;
    
    if( StomperS->nStomper < STOMPER_MAX_COUNT ) {
        STOMPER Stomper = {};
        Stomper.Mode     = StomperMode_Idle;
        Stomper.Position = Pos;
        Stomper.Velocity = {};
        StomperS->Stomper[ StomperS->nStomper++ ] = Stomper;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new STOMPER, but the state is full! Max Count = %u", STOMPER_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemoveStomper( APP_STATE * AppState, uint32 iRemove ) {
    STOMPER_STATE * StomperS = &AppState->StomperS;
    
    Assert( StomperS->nStomper > 0 );
    StomperS->Stomper[ iRemove ] = StomperS->Stomper[ --StomperS->nStomper ];
}

internal rect
GetPlayerBoundToBeDamagedByStomper( vec2 PlayerP ) {
    flo32 xMargin       = STOMPER_HALF_WIDTH - ( TILE_WIDTH  * 0.2f  );
    flo32 yMarginBottom = STOMPER_HEIGHT     - ( TILE_HEIGHT * 0.25f );
    flo32 yMarginTop    = TILE_HEIGHT * 0.2f;
    
    rect Bound = RectBCD( PlayerP, PLAYER_DIM );
    Bound.Left   -= xMargin;
    Bound.Right  += xMargin;
    Bound.Bottom -= yMarginBottom;
    Bound.Top    -= yMarginTop;
    
    return Bound;
}

internal rect
GetPlayerPunchBound( vec2 PlayerP, flo32 Charge, boo32 FaceLeft ) {
    vec2 Dim = PLAYER_PUNCH_COLLISION_DIM_LO;
    if( Charge >= 1.0f ) {
        Dim = PLAYER_PUNCH_COLLISION_DIM_HI;
    }
    
    rect Bound = {};
    if( FaceLeft ) {
        Bound = RectBRD( PlayerP, Dim );
    } else {
        Bound = RectBLD( PlayerP, Dim );
    }
    Bound = AddRadius( Bound, TILE_WIDTH * 0.1f );
    
    return Bound;
}

internal rect
GetPlayerBoundToPunchStomper( vec2 PlayerP, flo32 Charge, boo32 FaceLeft ) {
    flo32 xMargin       = STOMPER_HALF_WIDTH - ( TILE_WIDTH  * 0.2f  );
    flo32 yMarginBottom = STOMPER_HEIGHT     - ( TILE_HEIGHT * 0.25f );
    flo32 yMarginTop    = TILE_HEIGHT * 0.2f;
    
    vec2 Dim = PLAYER_PUNCH_COLLISION_DIM_LO;
    if( Charge >= 1.0f ) {
        Dim = PLAYER_PUNCH_COLLISION_DIM_HI;
    }
    
    rect Bound = {};
    if( FaceLeft ) {
        Bound = RectBRD( PlayerP, Dim );
    } else {
        Bound = RectBLD( PlayerP, Dim );
    }
    Bound = AddRadius( Bound, TILE_WIDTH * 0.1f );
    
    Bound.Left   -= xMargin;
    Bound.Right  += xMargin;
    Bound.Bottom -= yMarginBottom;
    Bound.Top    -= yMarginTop;
    
    return Bound;
}

internal void
UpdateStomper( APP_STATE * AppState, flo32 dT ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    STOMPER_STATE   * StomperS  = &AppState->StomperS;
    PLAYER_STATE    * Player    = &AppState->Player;
    
    for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
        STOMPER * Stomper = StomperS->Stomper + iStomper;
        
        Stomper->Timer += dT;
        
        boo32 IsOnGround = false;
        for( uint32 iJumpBound = 0; iJumpBound < Collision->nJumpBound; iJumpBound++ ) {
            rect R = Collision->JumpBound[ iJumpBound ];
            
            if( IsInBound( Stomper->Position, R ) ) {
                IsOnGround = true;
            }
        }
        
        vec2  Dir      = {};
        flo32 MaxSpeed = 0.0f;
        flo32 Friction = ( IsOnGround ) ? STOMPER_GROUND_FRICTION : STOMPER_AIR_FRICTION;
        
        switch( Stomper->Mode ) {
            case StomperMode_Idle: {
                Dir.y = -1.0f;
                if( ( IsOnGround ) && ( Stomper->Timer >= STOMPER_IDLE_TARGET_TIME ) ) {
                    Stomper->Timer = 0.0f;
                    Stomper->Mode  = StomperMode_Move;
                    Stomper->Move_xDir = ( Player->Position.x <= Stomper->Position.x ) ? -1.0f : 1.0f;
                    Stomper->CanHop    = true;
                    Stomper->iHop      = 0;
                    Stomper->nHop      = 3;
                }
            } break;
            
            case StomperMode_Move: {
                if( ( IsOnGround ) && ( Stomper->CanHop ) ) {
                    Stomper->CanHop = false;
                    
                    flo32 DistToPlayerX = fabsf( Player->Position.x - Stomper->Position.x );
                    if( DistToPlayerX <= STOMPER_ATTACK_MAX_DIST ) {
                        Stomper->Timer = 0.0f;
                        Stomper->Mode  = StomperMode_Jump;
                        
                        vec2 Dest = Vec2( Player->Position.x, Stomper->Position.y + STOMPER_STOMP_HEIGHT );
                        Stomper->JumpVel  = ( Dest - Stomper->Position ) / STOMPER_JUMP_TARGET_TIME;
                    } else {
                        if( Stomper->iHop < Stomper->nHop ) {
                            Stomper->Velocity = STOMPER_MOVE_SPEED * Vec2( Stomper->Move_xDir, 1.0f );
                            Stomper->iHop++;
                        } else {
                            Stomper->Mode = StomperMode_Idle;
                        }
                    }
                }
                
                Dir.y = -1.0f;
                if( ( IsOnGround ) && ( Stomper->Timer >= STOMPER_MOVE_TARGET_TIME ) ) {
                    Stomper->Timer  = 0.0f;
                    Stomper->CanHop = true;
                }
            } break;
            
            case StomperMode_Jump: {
                Friction = 0.0f;
                Stomper->Velocity = Stomper->JumpVel;
                if( Stomper->Timer >= STOMPER_JUMP_TARGET_TIME ) {
                    Stomper->Timer = 0.0f;
                    Stomper->Mode  = StomperMode_Delay;
                }
            } break;
            
            case StomperMode_Delay: {
                Friction = 0.0f;
                Stomper->Velocity = {};
                if( Stomper->Timer >= STOMPER_DELAY_TARGET_TIME ) {
                    Stomper->Timer = 0.0f;
                    Stomper->Mode  = StomperMode_Stomp;
                }
            } break;
            
            case StomperMode_Stomp: {
                Stomper->Velocity = Vec2( 0.0f, -STOMPER_STOMP_SPEED );
            } break;
        }
        
        vec2 Accel = {};
        Accel.x = ( Dir.x * MaxSpeed - Stomper->Velocity.x ) * Friction;
        Accel.y = Dir.y * STOMPER_GRAVITY;
        
        vec2 Pos  = Stomper->Position;
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + Stomper->Velocity * dT;
        Stomper->Velocity += Accel * dT;
        
        COLLISION_RESULT Coll = CollisionResult( Pos, Stomper->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll( &BestIntersect, AppState, Coll, STOMPER_COLLISION_OFFSET );
            //DoesIntersectScaffold    ( &BestIntersect, AppState, Coll, STOMPER_RADIUS );
            DoesIntersectSeesaw      ( &BestIntersect, AppState, Coll );
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        Coll.Pos += Coll.dPos;
        
        Stomper->Position = Coll.Pos;
        Stomper->Velocity = Coll.Vel;
        
        if( ( Stomper->Mode == StomperMode_Stomp ) && ( Coll.HadCollision ) ) {
            Stomper->Timer = 0.0f;
            Stomper->Mode  = StomperMode_Idle;
        }
    }
}

internal void
DrawStomper( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    STOMPER_STATE * StomperS = Draw->StomperS;
    
    for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
        STOMPER Stomper = StomperS->Stomper[ iStomper ];
        
        rect R = RectBCD( Stomper.Position, STOMPER_DIM );
        rect S = RectBCD( Stomper.Position, STOMPER_DIM * Vec2( 1.7f, 0.4f ) );
        DrawRect( Pass, S, ToColor( 0, 80,  80 ) );
        DrawRect( Pass, R, ToColor( 0, 150, 150 ) );
    }
}

internal void
DrawStomperDebug_DamagePlayer( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    STOMPER_STATE * StomperS = Draw->StomperS;
    
    for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
        STOMPER Stomper = StomperS->Stomper[ iStomper ];
        DrawPoint( Pass, Stomper.Position, TILE_DIM * 0.05f, COLOR_RED );
    }
    
    PLAYER_STATE * Player = Draw->Player;
    rect PlayerBound = GetPlayerBoundToBeDamagedByStomper( Player->Position );
    DrawRectOutline( Pass, PlayerBound, COLOR_YELLOW );
}

internal void
DrawStomperDebug_PunchedByPlayer( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    STOMPER_STATE * StomperS = Draw->StomperS;
    PLAYER_STATE  * Player   = Draw->Player;
    
    for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
        STOMPER Stomper = StomperS->Stomper[ iStomper ];
        DrawPoint( Pass, Stomper.Position, TILE_DIM * 0.05f, COLOR_RED );
    }
    
    rect Bound = GetPlayerBoundToPunchStomper( Player->Position, Player->Punch_Charge, Player->Draw_FaceLeft );
    DrawRectOutline( Pass, Bound, COLOR_YELLOW );
}


#if 0
internal void
Editor_DrawStomper( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__STOMPER_STATE * StomperS = &Editor->StomperS;
    
    flo32 OffsetX      = TILE_WIDTH * 0.2f;
    flo32 ArrowOffsetH = TILE_WIDTH * 0.1f;
    flo32 ArrowOffsetV = TILE_WIDTH * 0.25f;
    
    for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
        EDITOR__STOMPER Stomper = StomperS->Stomper[ iStomper ];
        DrawRect( Pass, Stomper.Bound, COLOR_GRAY( 0.5f ) );
        
        vec2 P = {};
        vec2 N = {};
        vec2 V = {};
        
        switch( Stomper.Type ) {
            case EditorStomperType_VerticalOnLeftWall: {
                P = GetLC( Stomper.Bound ) + Vec2( OffsetX, 0.0f );
                N = Vec2( 0.0f, 1.0f ) * ArrowOffsetV;
                V = Vec2( 1.0f, 0.0f ) * ArrowOffsetH;
            } break;
            
            case EditorStomperType_VerticalOnRightWall: {
                P = GetRC( Stomper.Bound ) + Vec2( -OffsetX, 0.0f );
                N = Vec2( 0.0f, 1.0f ) * ArrowOffsetV;
                V = Vec2( 1.0f, 0.0f ) * ArrowOffsetH;
            } break;
            
            case EditorStomperType_HorizontalMoveLeft: {
                P = GetCenter( Stomper.Bound );
                N = Vec2( -1.0f,  0.0f ) * ArrowOffsetV;
                V = Vec2(  0.0f, -1.0f ) * ArrowOffsetH;
            } break;
            
            case EditorStomperType_HorizontalMoveRight: {
                P = GetCenter( Stomper.Bound );
                N = Vec2(  1.0f,  0.0f ) * ArrowOffsetV;
                V = Vec2(  0.0f,  1.0f ) * ArrowOffsetH;
            } break;
        }
        
        vec2 A = P + N;
        vec2 B = P - N + V;
        vec2 C = P - N - V;
        
        DrawLine( Pass, A, B, COLOR_RED );
        DrawLine( Pass, B, C, COLOR_RED );
        DrawLine( Pass, C, A, COLOR_RED );
    }
}
#endif


internal void
FinalizeStomper( APP_STATE * AppState ) {
    STOMPER_STATE * StomperS = &AppState->StomperS;
}


#if 0
#define VERSION__STOMPER  ( 1 )
#define FILETAG__STOMPER  ( "STOMPER__" )
internal void
SaveStomper( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__STOMPER;
    uint32 Version = VERSION__STOMPER;
    EDITOR__STOMPER_STATE * StomperS = &Editor->StomperS;
    
    if( StomperS->nStomper > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        uint32 nType[ StomperType_Count ] = {};
        for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
            EDITOR__STOMPER Stomper = StomperS->Stomper[ iStomper ];
            switch( Stomper.Type ) {
                case EditorStomperType_VerticalOnLeftWall: {
                    nType[ StomperType_OnLeft  ]++;
                } break;
                
                case EditorStomperType_VerticalOnRightWall: {
                    nType[ StomperType_OnRight ]++;
                } break;
                
                case EditorStomperType_HorizontalMoveLeft:
                case EditorStomperType_HorizontalMoveRight: {
                    nType[ StomperType_OnTop ]++;
                } break;
            }
        }
        
        uint32 atIndex = 0;
        for( uint32 iType = 0; iType < StomperType_Count; iType++ ) {
            _writem( output, uint32, atIndex );
            _writem( output, uint32, nType[ iType ] );
            
            atIndex += nType[ iType ];
        }
        
        _writem( output, uint32, StomperS->nStomper );
        for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
            EDITOR__STOMPER Stomper = StomperS->Stomper[ iStomper ];
            if( Stomper.Type == EditorStomperType_VerticalOnLeftWall ) {
                _writem( output, EDITOR__STOMPER, Stomper );
            }
        }
        for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
            EDITOR__STOMPER Stomper = StomperS->Stomper[ iStomper ];
            if( Stomper.Type == EditorStomperType_VerticalOnRightWall ) {
                _writem( output, EDITOR__STOMPER, Stomper );
            }
        }
        for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
            EDITOR__STOMPER Stomper = StomperS->Stomper[ iStomper ];
            if( ( Stomper.Type == EditorStomperType_HorizontalMoveLeft ) || ( Stomper.Type == EditorStomperType_HorizontalMoveRight ) ) {
                _writem( output, EDITOR__STOMPER, Stomper );
            }
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__STOMPER
ReadStomper( uint32 Version, uint8 ** Ptr ) {
    EDITOR__STOMPER Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            EDITOR__STOMPER_TYPE Type  = _read( ptr, EDITOR__STOMPER_TYPE );
            rect                  Bound = _read( ptr, rect );
            
            EDITOR__STOMPER Stomper = {};
            Stomper.Type  = Type;
            Stomper.Bound = Bound;
            
            Result = Stomper;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadStomper( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__STOMPER;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        STOMPER_STATE * StomperS = &AppState->StomperS;
        
        for( uint32 iType = 0; iType < StomperType_Count; iType++ ) {
            StomperS->Type[ iType ] = _read( ptr, UINT32_PAIR );
        }
        
        StomperS->nStomper = _read( ptr, uint32 );
        for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
            EDITOR__STOMPER Src = ReadStomper( Version, &ptr );
            
            STOMPER Stomper = ToStomper( Src );
            StomperS->Stomper[ iStomper ] = Stomper;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadStomper( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__STOMPER;
    EDITOR_STATE           * Editor    = &AppState->Editor;
    EDITOR__STOMPER_STATE * StomperS = &Editor->StomperS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        for( uint32 iType = 0; iType < StomperType_Count; iType++ ) {
            UINT32_PAIR Ignore = _read( ptr, UINT32_PAIR );
        }
        
        StomperS->nStomper = _read( ptr, uint32 );
        for( uint32 iStomper = 0; iStomper < StomperS->nStomper; iStomper++ ) {
            EDITOR__STOMPER Src = ReadStomper( Version, &ptr );
            StomperS->Stomper[ iStomper ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}
#endif
