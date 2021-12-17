
internal void
AddRunner( APP_STATE * AppState, vec2 Pos, vec2 Vel ) {
    RUNNER_STATE * RunnerS = &AppState->RunnerS;
    
    if( RunnerS->nRunner < RUNNER_MAX_COUNT ) {
        RUNNER Runner = {};
        Runner.Position = Pos;
        Runner.Velocity = Vel;
        Runner.Color    = Vec4( RandomF32(), RandomF32(), RandomF32(), 1.0f );
        
        RunnerS->Runner[ RunnerS->nRunner++ ] = Runner;
    } else {
        //GlobalVar_DebugSystem.ErrorOccurred = true;
        //char str[ 512 ] = {};
        //sprintf( str, "ERROR! Attempted to create new RUNNER, but the state is full! Max Count = %u", RUNNER_MAX_COUNT );
        //CONSOLE_STRING( str );
    }
}


#if 0
internal RUNNER
ToRunner( EDITOR__RUNNER Src ) {
    flo32 Tab      = TILE_WIDTH * 0.1f;
    flo32 HOffsetX = 0.0f;
    flo32 VOffsetX = PLAYER_HALF_WIDTH + Tab;
    flo32 VOffsetYBottom = 0.0f; // TODO: Tune this!!
    flo32 VOffsetYTop    = 0.0f;
    
    rect R = Src.Bound;
    RUNNER Runner = {};
    
    switch( Src.Type ) {
        case EditorRunnerType_VerticalOnLeftWall: {
            Runner.Bound_Art = Rect( R.Left, R.Bottom, R.Left + RUNNER_ART_THICKNESS, R.Top );
            Runner.Bound_Mvt = Rect( R.Left - VOffsetX, R.Bottom - VOffsetYBottom, R.Left + Tab, R.Top + VOffsetYTop );
            Runner.Accel = Vec2( 0.0f, RUNNER_VERTICAL_SPEED );
        } break;
        
        case EditorRunnerType_VerticalOnRightWall: {
            Runner.Bound_Art = Rect( R.Right - RUNNER_ART_THICKNESS, R.Bottom, R.Right, R.Top );
            Runner.Bound_Mvt = Rect( R.Right - Tab, R.Bottom - VOffsetYBottom, R.Right + VOffsetX, R.Top + VOffsetYTop );
            Runner.Accel = Vec2( 0.0f, RUNNER_VERTICAL_SPEED );
        } break;
        
        case EditorRunnerType_HorizontalMoveLeft:
        case EditorRunnerType_HorizontalMoveRight: {
            Runner.Bound_Art = Rect( R.Left, R.Top - RUNNER_ART_THICKNESS, R.Right, R.Top );
            Runner.Bound_Mvt = Rect( R.Left - HOffsetX, R.Top - Tab, R.Right + HOffsetX, R.Top + Tab );
            
            flo32 Accel = RUNNER_HORIZONTAL_SPEED;
            if( Src.Type == EditorRunnerType_HorizontalMoveLeft ) {
                Accel = -RUNNER_HORIZONTAL_SPEED;
            }
            
            Runner.Accel = Vec2( Accel, 0.0f );
        } break;
    }
    
    return Runner;
}
// TODO: NOTE: Temporary functions until RUNNERS are added to the editor
#endif

internal rect
GetRunnerBound( vec2 P ) {
    rect Result = RectBCD( P, RUNNER_DIM );
    return Result;
}

internal void
UpdateRunner( APP_STATE * AppState, flo32 dT ) {
    RUNNER_STATE * RunnerS = &AppState->RunnerS;
    
    for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
        RUNNER * Runner = RunnerS->Runner + iRunner;
        
        vec2 Accel = {};
        Accel.y = -RUNNER_GRAVITY;
        
        vec2 Pos  = Runner->Position;
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + Runner->Velocity * dT;
        Runner->Velocity += Accel * dT;
        
        COLLISION_RESULT Coll = CollisionResult( Pos, Runner->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionTop( &BestIntersect, AppState, Coll, RUNNER_COLLISION_OFFSET );
            DoesIntersectScaffold    ( &BestIntersect, AppState, Coll, RUNNER_COLLISION_OFFSET );
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        Coll.Pos += Coll.dPos;
        
        Runner->Position = Coll.Pos;
        Runner->Velocity = Coll.Vel;
        if( Coll.HadCollision ) {
            Runner->Velocity.y = 5.5f;
        }
    }
    
    RunnerS->Spawn_Timer      += dT;
    RunnerS->Stationary_Timer += dT;
    
    CAMERA_STATE Camera     = AppState->Camera;
    UINT32_PAIR  CameraCell = GetCellIndexFromPos( Camera.Pos );
    if( CameraCell.x != RunnerS->Spawn_CameraXCell ) {
        RunnerS->PlayerIsMovingRight = ( CameraCell.x > RunnerS->Spawn_CameraXCell );
        RunnerS->Stationary_Timer    = 0.0f;
        
        RunnerS->Spawn_Timer += 0.5f;
        RunnerS->Spawn_CameraXCell = CameraCell.x;
    }
    
    if( RunnerS->Stationary_Timer >= 2.0f ) {
        RunnerS->PlayerIsMovingRight = false;
    }
    
    if( RunnerS->Spawn_Timer >= RunnerS->Spawn_TargetTime ) {
        RunnerS->Spawn_TargetTime = RandomF32InRange( 1.0f, 4.0f );
        RunnerS->Spawn_Timer      = 0.0f;
        
        flo32 xLeft  = Camera.Pos.x - ( Camera.HalfDim.x + RUNNER_SPAWN_CAMERA_OFFSET );
        flo32 xRight = Camera.Pos.x + ( Camera.HalfDim.x + RUNNER_SPAWN_CAMERA_OFFSET );
        
        uint32 nLane = 3;
        flo32  LaneY[ 3 ] = { 492.5f, 495.5f, 498.5f };
        uint32 LaneSelect = rand() % nLane;
        
        flo32 X    = 0.0f;
        flo32 VelX = 0.0f;
        
        uint32 Select = ( rand() % 100 ) + 1;
        if( RunnerS->PlayerIsMovingRight ) {
            if( Select >= 100 ) {
                X    = xRight;
                VelX = -4.0f;
            } else if( Select >= 90 ) {
                X    = xLeft;
                VelX = 7.0f;
            } else if( Select >= 45 ) {
                X    = xRight;
                VelX = 4.0f;
            } else {
                X    = xRight;
                VelX = 2.0f;
            }
        } else {
            if( Select >= 100 ) {
                X    = xRight;
                VelX = -4.0f;
            } else if( Select >= 90 ) {
                X    = xLeft;
                VelX = 7.0f;
            } else if( Select >= 45 ) {
                X    = xLeft;
                VelX = 4.0f;
            } else {
                X    = xLeft;
                VelX = 2.0f;
            }
        }
        
        flo32  Y        = LaneY[ LaneSelect ];
        vec2   SpawnP   = Vec2( X, Y );
        vec2   SpawnVel = Vec2( VelX, 0.0f );
        
        AddRunner( AppState, SpawnP, SpawnVel );
    }
    
    LEVEL_STATS Stat = AppState->Stat;
    RunnerS->Race_StartX  = Stat.Bound.Left  + ( TILE_WIDTH * 8.0f );
    RunnerS->Race_FinishX = Stat.Bound.Right - ( TILE_WIDTH * 8.0f );
    
    PLAYER_STATE * Player = &AppState->Player;
    if( Player->Position.x < RunnerS->Race_FinishX ) {
        RunnerS->Race_Timer += dT;
        
        char Str[ 128 ] = {};
        sprintf( Str, "Time:  %.02f sec", RunnerS->Race_Timer );
        //DISPLAY_STRING( Str );
    } else {
        flo32 Speed = ( RunnerS->Race_FinishX - RunnerS->Race_StartX ) / RunnerS->Race_Timer;
        
        char Str[ 128 ] = {};
        sprintf( Str, "Speed: %.02f TPS", Speed );
        //DISPLAY_STRING( Str );
        
        sprintf( Str, "Time:  %.02f sec", RunnerS->Race_Timer );
        //DISPLAY_STRING( Str );
        //DISPLAY_STRING( "FINISHED!!" );
    }
}

internal void
DrawRunner( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    RUNNER_STATE * RunnerS = Draw->RunnerS;
    
    for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
        RUNNER Runner = RunnerS->Runner[ iRunner ];
        rect   R = GetRunnerBound( Runner.Position );
        DrawRect( Pass, R, Runner.Color );
    }
    
#if 0    
    CAMERA_STATE Camera = AppState->Camera;
    
    uint32 nSegment = 16;
    flo32  Dist     = RunnerS->Race_FinishX - RunnerS->Race_StartX;
    flo32  DistPerSegment = Dist / ( flo32 )nSegment;
    
    flo32 atX = RunnerS->Race_StartX;
    flo32 atY = Camera.Pos.y;
    for( uint32 iSegment = 0; iSegment < ( nSegment + 1 ); iSegment++ ) {
        vec2 P = Vec2( atX, atY - Camera.Dim.y );
        vec2 Q = Vec2( atX, atY + Camera.Dim.y );
        
        if( iSegment == nSegment ) {
            rect R = Rect( P.x, P.y, P.x + TILE_WIDTH * 0.1f, Q.y );
            DrawRect( Pass, R, COLOR_RED );
        } else {
            DrawLine( Pass, P, Q, COLOR_RED );
        }
        
        atX += DistPerSegment;
    }
#endif
    
}

internal rect
GetPlayerBoundToBeDamagedByRunner( vec2 PlayerP ) {
    flo32 xMargin       = RUNNER_HALF_WIDTH - ( TILE_WIDTH  * 0.2f  );
    flo32 yMarginBottom = RUNNER_HEIGHT     - ( TILE_HEIGHT * 0.25f );
    flo32 yMarginTop    = TILE_HEIGHT * 0.5f;
    
    rect Bound = RectBCD( PlayerP, PLAYER_DIM );
    Bound.Left   -= xMargin;
    Bound.Right  += xMargin;
    Bound.Bottom -= yMarginBottom;
    Bound.Top    -= yMarginTop;
    
    return Bound;
}

internal void
DrawRunnerDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    RUNNER_STATE * RunnerS = Draw->RunnerS;
    
    for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
        RUNNER Runner = RunnerS->Runner[ iRunner ];
        DrawPoint( Pass, Runner.Position, TILE_DIM * 0.05f, COLOR_RED );
    }
    
    PLAYER_STATE * Player = Draw->Player;
    rect PlayerBound = GetPlayerBoundToBeDamagedByRunner( Player->Position );
    DrawRectOutline( Pass, PlayerBound, COLOR_YELLOW );
}


#if 0
internal void
Editor_DrawRunner( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__RUNNER_STATE * RunnerS = &Editor->RunnerS;
    
    flo32 OffsetX      = TILE_WIDTH * 0.2f;
    flo32 ArrowOffsetH = TILE_WIDTH * 0.1f;
    flo32 ArrowOffsetV = TILE_WIDTH * 0.25f;
    
    for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
        EDITOR__RUNNER Runner = RunnerS->Runner[ iRunner ];
        DrawRect( Pass, Runner.Bound, COLOR_GRAY( 0.5f ) );
        
        vec2 P = {};
        vec2 N = {};
        vec2 V = {};
        
        switch( Runner.Type ) {
            case EditorRunnerType_VerticalOnLeftWall: {
                P = GetLC( Runner.Bound ) + Vec2( OffsetX, 0.0f );
                N = Vec2( 0.0f, 1.0f ) * ArrowOffsetV;
                V = Vec2( 1.0f, 0.0f ) * ArrowOffsetH;
            } break;
            
            case EditorRunnerType_VerticalOnRightWall: {
                P = GetRC( Runner.Bound ) + Vec2( -OffsetX, 0.0f );
                N = Vec2( 0.0f, 1.0f ) * ArrowOffsetV;
                V = Vec2( 1.0f, 0.0f ) * ArrowOffsetH;
            } break;
            
            case EditorRunnerType_HorizontalMoveLeft: {
                P = GetCenter( Runner.Bound );
                N = Vec2( -1.0f,  0.0f ) * ArrowOffsetV;
                V = Vec2(  0.0f, -1.0f ) * ArrowOffsetH;
            } break;
            
            case EditorRunnerType_HorizontalMoveRight: {
                P = GetCenter( Runner.Bound );
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
FinalizeRunner( APP_STATE * AppState ) {
    RUNNER_STATE * RunnerS = &AppState->RunnerS;
    CAMERA_STATE   Camera  =  AppState->Camera;
    
    rect DespawnBound   = {};
    DespawnBound.Left   = Camera.Pos.x - ( Camera.HalfDim.x + RUNNER_DESPAWN_CAMERA_OFFSET );
    DespawnBound.Bottom = AppState->DeathPlaneY;
    DespawnBound.Right  = Camera.Pos.x + ( Camera.HalfDim.x + RUNNER_DESPAWN_CAMERA_OFFSET );
    DespawnBound.Top    = FLT_MAX;
    
    uint32 NewCount = 0;
    for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
        RUNNER Runner = RunnerS->Runner[ iRunner ];
        if( IsInBound( Runner.Position, DespawnBound ) ) {
            RunnerS->Runner[ NewCount++ ] = Runner;
        }
    }
    RunnerS->nRunner = NewCount;
}


#if 0
#define VERSION__RUNNER  ( 1 )
#define FILETAG__RUNNER  ( "RUNNER" )
internal void
SaveRunner( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName, char * FileTag, uint32 Version ) {
    EDITOR__RUNNER_STATE * RunnerS = &Editor->RunnerS;
    
    if( RunnerS->nRunner > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        uint32 nType[ RunnerType_Count ] = {};
        for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
            EDITOR__RUNNER Runner = RunnerS->Runner[ iRunner ];
            switch( Runner.Type ) {
                case EditorRunnerType_VerticalOnLeftWall: {
                    nType[ RunnerType_OnLeft  ]++;
                } break;
                
                case EditorRunnerType_VerticalOnRightWall: {
                    nType[ RunnerType_OnRight ]++;
                } break;
                
                case EditorRunnerType_HorizontalMoveLeft:
                case EditorRunnerType_HorizontalMoveRight: {
                    nType[ RunnerType_OnTop ]++;
                } break;
            }
        }
        
        uint32 atIndex = 0;
        for( uint32 iType = 0; iType < RunnerType_Count; iType++ ) {
            _writem( output, uint32, atIndex );
            _writem( output, uint32, nType[ iType ] );
            
            atIndex += nType[ iType ];
        }
        
        _writem( output, uint32, RunnerS->nRunner );
        for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
            EDITOR__RUNNER Runner = RunnerS->Runner[ iRunner ];
            if( Runner.Type == EditorRunnerType_VerticalOnLeftWall ) {
                _writem( output, EDITOR__RUNNER, Runner );
            }
        }
        for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
            EDITOR__RUNNER Runner = RunnerS->Runner[ iRunner ];
            if( Runner.Type == EditorRunnerType_VerticalOnRightWall ) {
                _writem( output, EDITOR__RUNNER, Runner );
            }
        }
        for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
            EDITOR__RUNNER Runner = RunnerS->Runner[ iRunner ];
            if( ( Runner.Type == EditorRunnerType_HorizontalMoveLeft ) || ( Runner.Type == EditorRunnerType_HorizontalMoveRight ) ) {
                _writem( output, EDITOR__RUNNER, Runner );
            }
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__RUNNER
ReadRunner( uint32 Version, uint8 ** Ptr ) {
    EDITOR__RUNNER Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            EDITOR__RUNNER_TYPE Type  = _read( ptr, EDITOR__RUNNER_TYPE );
            rect                  Bound = _read( ptr, rect );
            
            EDITOR__RUNNER Runner = {};
            Runner.Type  = Type;
            Runner.Bound = Bound;
            
            Result = Runner;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadRunner( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName, char * FileTag ) {
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        RUNNER_STATE * RunnerS = &AppState->RunnerS;
        
        for( uint32 iType = 0; iType < RunnerType_Count; iType++ ) {
            RunnerS->Type[ iType ] = _read( ptr, UINT32_PAIR );
        }
        
        RunnerS->nRunner = _read( ptr, uint32 );
        for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
            EDITOR__RUNNER Src = ReadRunner( Version, &ptr );
            
            RUNNER Runner = ToRunner( Src );
            RunnerS->Runner[ iRunner ] = Runner;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadRunner( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName, char * FileTag ) {
    EDITOR_STATE             * Editor      = &AppState->Editor;
    EDITOR__RUNNER_STATE * RunnerS = &Editor->RunnerS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        for( uint32 iType = 0; iType < RunnerType_Count; iType++ ) {
            UINT32_PAIR Ignore = _read( ptr, UINT32_PAIR );
        }
        
        RunnerS->nRunner = _read( ptr, uint32 );
        for( uint32 iRunner = 0; iRunner < RunnerS->nRunner; iRunner++ ) {
            EDITOR__RUNNER Src = ReadRunner( Version, &ptr );
            RunnerS->Runner[ iRunner ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}
#endif