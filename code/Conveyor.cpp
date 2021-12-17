

// TODO: NOTE: Temporary functions until CONVEYORS are added to the editor
internal void
InitConveyorType( APP_STATE * AppState, CONVEYOR_TYPE Type ) {
    CONVEYOR_STATE * ConveyorS = &AppState->ConveyorS;
    ConveyorS->ActiveType = Type;
    ConveyorS->Type[ ConveyorS->ActiveType ].m = ConveyorS->nConveyor;
}

internal void
AddConveyor( APP_STATE * AppState, vec2 P, vec2 Q, flo32 Accel ) {
    CONVEYOR_STATE * ConveyorS = &AppState->ConveyorS;
    
    Assert( ConveyorS->nConveyor < CONVEYOR_MAX_COUNT );
    
    flo32 Tab      = TILE_WIDTH * 0.1f;
    flo32 HOffsetX = 0.0f;
    flo32 VOffsetX = PLAYER_HALF_WIDTH + Tab;
    flo32 VOffsetYBottom = 0.0f; // TODO: Tune this!!
    flo32 VOffsetYTop    = 0.0f;
    
    CONVEYOR Conveyor = {};
    switch( ConveyorS->ActiveType ) {
        case ConveyorType_OnLeft: {
            Assert( P.x == Q.x );
            
            flo32 Bottom = MinValue( P.y, Q.y );
            flo32 Top    = MaxValue( P.y, Q.y );
            
            Conveyor.Bound_Art = Rect( P.x, Bottom, P.x + CONVEYOR_ART_THICKNESS, Top );
            Conveyor.Bound_Mvt = Rect( P.x - VOffsetX, Bottom - VOffsetYBottom, P.x + Tab, Top + VOffsetYTop );
            Conveyor.Accel = Vec2( 0.0f, Accel );
        } break;
        
        case ConveyorType_OnBottom: {
            InvalidCodePath;
        } break;
        
        case ConveyorType_OnRight: {
            Assert( P.x == Q.x );
            
            flo32 Bottom = MinValue( P.y, Q.y );
            flo32 Top    = MaxValue( P.y, Q.y );
            
            Conveyor.Bound_Art = Rect( P.x - CONVEYOR_ART_THICKNESS, Bottom, P.x, Top );
            Conveyor.Bound_Mvt = Rect( P.x - Tab, Bottom - VOffsetYBottom, P.x + VOffsetX, Top + VOffsetYTop );
            Conveyor.Accel = Vec2( 0.0f, Accel );
        } break;
        
        case ConveyorType_OnTop: {
            Assert( P.y == Q.y );
            
            flo32 Left  = MinValue( P.x, Q.x );
            flo32 Right = MaxValue( P.x, Q.x );
            
            Conveyor.Bound_Art = Rect( Left, P.y - CONVEYOR_ART_THICKNESS, Right, P.y );
            Conveyor.Bound_Mvt = Rect( Left - HOffsetX, P.y - Tab, Right + HOffsetX, P.y + Tab );
            Conveyor.Accel = Vec2( Accel, 0.0f );
        } break;
    }
    
    ConveyorS->Conveyor[ ConveyorS->nConveyor++ ] = Conveyor;
    ConveyorS->Type[ ConveyorS->ActiveType ].n++;
}

internal CONVEYOR
ToConveyor( EDITOR__CONVEYOR Src ) {
    CONVEYOR Conveyor = {};
    
    flo32 Tab = TILE_WIDTH * 0.1f;
    
    rect R = Src.Bound;
    switch( Src.Type ) {
        case EditorConveyorType_VerticalOnLeftWall: {
            Conveyor.Bound_Art = Rect( R.Left, R.Bottom, R.Left + CONVEYOR_ART_THICKNESS, R.Top );
            Conveyor.Bound_Mvt = Rect( R.Left - Tab, R.Bottom, R.Left + Tab, R.Top );
            Conveyor.Accel = Vec2( 0.0f, CONVEYOR_VERTICAL_SPEED );
        } break;
        
        case EditorConveyorType_VerticalOnRightWall: {
            Conveyor.Bound_Art = Rect( R.Right - CONVEYOR_ART_THICKNESS, R.Bottom, R.Right, R.Top );
            Conveyor.Bound_Mvt = Rect( R.Right - Tab, R.Bottom, R.Right + Tab, R.Top );
            Conveyor.Accel = Vec2( 0.0f, CONVEYOR_VERTICAL_SPEED );
        } break;
        
        case EditorConveyorType_HorizontalMoveLeft:
        case EditorConveyorType_HorizontalMoveRight: {
            Conveyor.Bound_Art = Rect( R.Left, R.Top - CONVEYOR_ART_THICKNESS, R.Right, R.Top );
            Conveyor.Bound_Mvt = Rect( R.Left, R.Top - Tab, R.Right, R.Top + Tab );
            
            flo32 Accel = CONVEYOR_HORIZONTAL_SPEED;
            if( Src.Type == EditorConveyorType_HorizontalMoveLeft ) {
                Accel = -CONVEYOR_HORIZONTAL_SPEED;
            }
            
            Conveyor.Accel = Vec2( Accel, 0.0f );
        } break;
    }
    
    return Conveyor;
}
// TODO: NOTE: Temporary functions until CONVEYORS are added to the editor

internal CONVEYOR_RESULT
IsPosOnConveyorHor( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    CONVEYOR_STATE * ConveyorS = &AppState->ConveyorS;
    
    CONVEYOR_RESULT Result = {};
    
    UINT32_PAIR Type = ConveyorS->Type[ ConveyorType_OnTop ];
    for( uint32 iConveyor = 0; iConveyor < Type.n; iConveyor++ ) {
        CONVEYOR Conveyor = ConveyorS->Conveyor[ Type.m + iConveyor ];
        
        rect R = MinkSubX( Conveyor.Bound_Mvt, CollBound );
        if( IsInBound( Pos, R ) ) {
            Result.IsOnConveyor  = true;
            Result.ConveyorAccel = Conveyor.Accel;
        }
    }
    
    return Result;
}

internal void
UpdateConveyor( APP_STATE * AppState, flo32 dT ) {
    
}

internal void
DrawConveyor( RENDER_PASS * Pass, APP_STATE * AppState ) {
    CONVEYOR_STATE * ConveyorS = &AppState->ConveyorS;
    
    for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
        CONVEYOR Conveyor = ConveyorS->Conveyor[ iConveyor ];
        DrawRect( Pass, Conveyor.Bound_Art, COLOR_GRAY( 0.1f ) );
        DrawRect( Pass, AddRadius( Conveyor.Bound_Art, -TILE_WIDTH * 0.1f ), COLOR_GRAY( 0.2f ) );
        
        vec2  P       = GetCenter( Conveyor.Bound_Art );
        vec2  Scale   = Vec2( 0.2f, 0.35f ) * TILE_DIM;
        flo32 Radians = 0.0f;
        if( Conveyor.Accel.x < 0.0f ) { Radians =  PI * 0.5f; }
        if( Conveyor.Accel.x > 0.0f ) { Radians = -PI * 0.5f; }
        
        DrawModel( Pass, ModelID_Triangle, TextureID_WhiteTexture, P, Scale, Radians, COLOR_RED );
    }
}

internal void
DrawConveyorDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    CONVEYOR_STATE * ConveyorS = &AppState->ConveyorS;
    
    for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
        CONVEYOR Conveyor = ConveyorS->Conveyor[ iConveyor ];
        DrawRectOutline( Pass, Conveyor.Bound_Mvt, COLOR_RED );
        
        // TODO: Draw Accel!!
    }
}

internal void
Editor_DrawConveyor( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__CONVEYOR_STATE * ConveyorS = &Editor->ConveyorS;
    
    flo32 OffsetX      = TILE_WIDTH * 0.2f;
    flo32 ArrowOffsetH = TILE_WIDTH * 0.1f;
    flo32 ArrowOffsetV = TILE_WIDTH * 0.25f;
    
    for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
        EDITOR__CONVEYOR Conveyor = ConveyorS->Conveyor[ iConveyor ];
        DrawRect( Pass, Conveyor.Bound, COLOR_GRAY( 0.5f ) );
        
        vec2 P = {};
        vec2 N = {};
        vec2 V = {};
        
        switch( Conveyor.Type ) {
            case EditorConveyorType_VerticalOnLeftWall: {
                P = GetLC( Conveyor.Bound ) + Vec2( OffsetX, 0.0f );
                N = Vec2( 0.0f, 1.0f ) * ArrowOffsetV;
                V = Vec2( 1.0f, 0.0f ) * ArrowOffsetH;
            } break;
            
            case EditorConveyorType_VerticalOnRightWall: {
                P = GetRC( Conveyor.Bound ) + Vec2( -OffsetX, 0.0f );
                N = Vec2( 0.0f, 1.0f ) * ArrowOffsetV;
                V = Vec2( 1.0f, 0.0f ) * ArrowOffsetH;
            } break;
            
            case EditorConveyorType_HorizontalMoveLeft: {
                P = GetCenter( Conveyor.Bound );
                N = Vec2( -1.0f,  0.0f ) * ArrowOffsetV;
                V = Vec2(  0.0f, -1.0f ) * ArrowOffsetH;
            } break;
            
            case EditorConveyorType_HorizontalMoveRight: {
                P = GetCenter( Conveyor.Bound );
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

internal void
FinalizeConveyor( APP_STATE * AppState ) {
    
}

internal void
Editor_MoveConveyor( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__CONVEYOR_STATE * ConveyorS = &Editor->ConveyorS;
    
    for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
        EDITOR__CONVEYOR * Conveyor = ConveyorS->Conveyor + iConveyor;
        
        if( DoesRectIntersectRectExc( SrcBound, Conveyor->Bound ) ) {
            Conveyor->Bound = AddOffset( Conveyor->Bound, Offset );
        }
    }
}

internal void
Editor_MirrorConveyor( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__CONVEYOR_STATE * ConveyorS = &Editor->ConveyorS;
    
    for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
        EDITOR__CONVEYOR * Conveyor = ConveyorS->Conveyor + iConveyor;
        
        vec2 Center = GetCenter( Conveyor->Bound );
        vec2 Dim    = GetDim   ( Conveyor->Bound );
        
        if( IsInBound( Center, SrcBound ) ) {
            Center.x = ( SrcBound.Right ) - ( Center.x - SrcBound.Left );
            Conveyor->Bound = RectCD( Center, Dim );
        }
    }
}

#define VERSION__CONVEYOR  ( 1 )
#define FILETAG__CONVEYOR  ( "CONVEYOR" )
internal void
SaveConveyor( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CONVEYOR;
    uint32 Version = VERSION__CONVEYOR;
    EDITOR__CONVEYOR_STATE * ConveyorS = &Editor->ConveyorS;
    
    if( ConveyorS->nConveyor > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        uint32 nType[ ConveyorType_Count ] = {};
        for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
            EDITOR__CONVEYOR Conveyor = ConveyorS->Conveyor[ iConveyor ];
            switch( Conveyor.Type ) {
                case EditorConveyorType_VerticalOnLeftWall: {
                    nType[ ConveyorType_OnLeft  ]++;
                } break;
                
                case EditorConveyorType_VerticalOnRightWall: {
                    nType[ ConveyorType_OnRight ]++;
                } break;
                
                case EditorConveyorType_HorizontalMoveLeft:
                case EditorConveyorType_HorizontalMoveRight: {
                    nType[ ConveyorType_OnTop ]++;
                } break;
            }
        }
        
        uint32 atIndex = 0;
        for( uint32 iType = 0; iType < ConveyorType_Count; iType++ ) {
            _writem( output, uint32, atIndex );
            _writem( output, uint32, nType[ iType ] );
            
            atIndex += nType[ iType ];
        }
        
        _writem( output, uint32, ConveyorS->nConveyor );
        for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
            EDITOR__CONVEYOR Conveyor = ConveyorS->Conveyor[ iConveyor ];
            if( Conveyor.Type == EditorConveyorType_VerticalOnLeftWall ) {
                _writem( output, EDITOR__CONVEYOR, Conveyor );
            }
        }
        for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
            EDITOR__CONVEYOR Conveyor = ConveyorS->Conveyor[ iConveyor ];
            if( Conveyor.Type == EditorConveyorType_VerticalOnRightWall ) {
                _writem( output, EDITOR__CONVEYOR, Conveyor );
            }
        }
        for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
            EDITOR__CONVEYOR Conveyor = ConveyorS->Conveyor[ iConveyor ];
            if( ( Conveyor.Type == EditorConveyorType_HorizontalMoveLeft ) || ( Conveyor.Type == EditorConveyorType_HorizontalMoveRight ) ) {
                _writem( output, EDITOR__CONVEYOR, Conveyor );
            }
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__CONVEYOR
ReadConveyor( uint32 Version, uint8 ** Ptr ) {
    EDITOR__CONVEYOR Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            EDITOR__CONVEYOR_TYPE Type  = _read( ptr, EDITOR__CONVEYOR_TYPE );
            rect                  Bound = _read( ptr, rect );
            
            EDITOR__CONVEYOR Conveyor = {};
            Conveyor.Type  = Type;
            Conveyor.Bound = Bound;
            
            Result = Conveyor;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadConveyor( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CONVEYOR;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        CONVEYOR_STATE * ConveyorS = &AppState->ConveyorS;
        
        for( uint32 iType = 0; iType < ConveyorType_Count; iType++ ) {
            ConveyorS->Type[ iType ] = _read( ptr, UINT32_PAIR );
        }
        
        ConveyorS->nConveyor = _read( ptr, uint32 );
        for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
            EDITOR__CONVEYOR Src = ReadConveyor( Version, &ptr );
            
            CONVEYOR Conveyor = ToConveyor( Src );
            ConveyorS->Conveyor[ iConveyor ] = Conveyor;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadConveyor( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__CONVEYOR;
    EDITOR_STATE           * Editor    = &AppState->Editor;
    EDITOR__CONVEYOR_STATE * ConveyorS = &Editor->ConveyorS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        for( uint32 iType = 0; iType < ConveyorType_Count; iType++ ) {
            UINT32_PAIR Ignore = _read( ptr, UINT32_PAIR );
        }
        
        ConveyorS->nConveyor = _read( ptr, uint32 );
        for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
            EDITOR__CONVEYOR Src = ReadConveyor( Version, &ptr );
            ConveyorS->Conveyor[ iConveyor ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputConveyorToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS               Stat      = Editor->Stat;
    EDITOR__CONVEYOR_STATE * ConveyorS = &Editor->ConveyorS;
    
    for( uint32 iConveyor = 0; iConveyor < ConveyorS->nConveyor; iConveyor++ ) {
        EDITOR__CONVEYOR Conveyor = ConveyorS->Conveyor[ iConveyor ];
        
        vec4 Color = COLOR_GRAY( 0.1f );
        rect Bound = Conveyor.Bound;
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
        
        // TODO: Output direction
    }
}