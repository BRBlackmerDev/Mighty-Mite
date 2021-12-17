

internal PUSH_BLOCK
PushBlockC( rect Bound ) {
    PUSH_BLOCK PushBlock = {};
    PushBlock.Position   = GetCenter( Bound );
    PushBlock.Dim        = GetDim( Bound );
    return PushBlock;
}

internal void
AddPushBlock( APP_STATE * AppState, rect Bound ) {
    PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
    
    if( PushBlockS->nPushBlock < PUSH_BLOCK_MAX_COUNT ) {
        PUSH_BLOCK PushBlock = PushBlockC( Bound );
        PushBlockS->PushBlock[ PushBlockS->nPushBlock++ ] = PushBlock;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new PUSH_BLOCK, but the state is full! Max Count = %u", PUSH_BLOCK_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
AddPushBlock( APP_STATE * AppState, UINT32_PAIR Cell, uint32 CellWidth, uint32 CellHeight ) {
    vec2 P   = GetCellRectBLFromIndex( Cell );
    vec2 Dim = TILE_DIM * Vec2( ( flo32 )CellWidth, ( flo32 )CellHeight );
    rect R   = RectBLD( P, Dim );
    
    AddPushBlock( AppState, R );
}

internal boo32
IsPosOnPushBlock( APP_STATE * AppState, vec2 Pos, flo32 OffsetX ) {
    PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
    
    boo32 Result = false;
    
    flo32 MarginY = TILE_HEIGHT * 0.1f;
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
        
        rect R = RectCD( PushBlock.Position, PushBlock.Dim );
        R.Left  -= OffsetX;
        R.Right += OffsetX;
        R.Bottom = R.Top - MarginY;
        R.Top   += MarginY;
        
        if( IsInBound( Pos, R ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal ENTITY_VALID_RESULT
IsInGrabBoundOfPushBlock( APP_STATE * AppState, vec2 Pos ) {
    PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
    
    ENTITY_VALID_RESULT Result = {};
    
    flo32 MarginX = TILE_WIDTH * 0.1f;
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
        if( PushBlock.IsOnGround ) {
            rect Bound = RectCD( PushBlock.Position, PushBlock.Dim );
            
            flo32 Height = MaxValue( PushBlock.Dim.y - TILE_HEIGHT * 2.0f, 0.0f );
            vec2 Dim = Vec2( PLAYER_HALF_WIDTH, Height );
            
            rect Bound_Left = RectBRD( GetBL( Bound ), Dim );
            Bound_Left = AddRadius( Bound_Left, MarginX );
            
            rect Bound_Right = RectBLD( GetBR( Bound ), Dim );
            Bound_Right = AddRadius( Bound_Right, MarginX );
            
            if( ( IsInBound( Pos, Bound_Left ) ) || ( IsInBound( Pos, Bound_Right ) ) ) {
                Result.IsValid = true;
                Result.iEntity = iPushBlock;
            }
        }
    }
    
    return Result;
}

internal void
UpdatePushBlock( APP_STATE * AppState, flo32 dT ) {
    PLAYER_STATE     * Player     = &AppState->Player;
    PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
    COLLISION_STATE  * Collision  = &AppState->Collision;
    
    // TODO: Handle Push Blocks on Conveyors?
    
    // Should cancel grab
    flo32 MaxSpeed = PUSH_BLOCK_MOVE_WALK_SPEED;
    if( Player->Run_IsActive ) {
        MaxSpeed = PUSH_BLOCK_MOVE_RUN_SPEED;
    }
    
    uint32 InitPushBlock = 0;
    if( Player->Grab_IsActive ) {
        InitPushBlock = 1;
    }
    
    for( uint32 iPushBlock = InitPushBlock; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        PUSH_BLOCK * PushBlock = PushBlockS->PushBlock + iPushBlock;
        
        rect CollBound = RectCD( Vec2( 0.0f, 0.0f ), PushBlock->Dim );
        
        flo32 Friction = PUSH_BLOCK_STOP_FRICTION;
        if( ( PushBlock->DirX != 0.0f ) && ( ( PushBlock->DirX * PushBlock->Velocity.x ) > 0.0f ) ) {
            Friction = PUSH_BLOCK_START_FRICTION;
        }
        
        vec2 PushBlockP = PushBlock->Position + Vec2( 0.0f, -PushBlock->Dim.y * 0.5f );
        CONVEYOR_RESULT ConveyorHor = IsPosOnConveyorHor( AppState, PushBlockP, CollBound );
        
        rect IsOnGroundBound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( PushBlock->Dim.x, 0.0f ) );
        PushBlock->IsOnGround = IsPosOnCollisionTop( AppState, PushBlockP, IsOnGroundBound );
        
        vec2 Accel = {};
        Accel.x = ( PushBlock->DirX * MaxSpeed - PushBlock->Velocity.x ) * Friction;
        if( ConveyorHor.IsOnConveyor ) {
            Accel.x += ( ConveyorHor.ConveyorAccel.x * Friction );
        }
        Accel.y = -PUSH_BLOCK_GRAVITY;
        
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + PushBlock->Velocity * dT;
        PushBlock->Velocity += Accel * dT;
        
        vec2 PrevPos = PushBlock->Position;
        
        COLLISION_RESULT Coll = CollisionResult( PushBlock->Position, PushBlock->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionLeft ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectCollisionRight( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectCollisionTop  ( &BestIntersect, AppState, Coll, CollBound );
            
            DoesIntersectBreakBlock( &BestIntersect, AppState, Coll, CollBound );
            
            DoesPushBlockIntersectPushBlock( &BestIntersect, AppState, Coll, CollBound, iPushBlock );
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        Coll.Pos += Coll.dPos;
        
        PushBlock->Position = Coll.Pos;
        PushBlock->Velocity = Coll.Vel;
        
        PushBlock->Velocity.y = MaxValue( PushBlock->Velocity.y, -18.0f );
        
        vec2 NewPos = PushBlock->Position;
        vec2 ChangePos = NewPos - PrevPos;
        
        if( ChangePos.x != 0.0f ) {
            rect PlayerDim = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM );
            PlayerDim.Bottom -= ( TILE_HEIGHT * 0.1f );
            
            rect PushBound = MinkSub( RectCD( PushBlock->Position, PushBlock->Dim ), PlayerDim );
            if( IsInBound( Player->Position, PushBound ) ) {
                Player->Position.x += ChangePos.x;
            }
        }
    }
    
    if( Player->Grab_IsActive ) {
        PUSH_BLOCK * PushBlock = PushBlockS->PushBlock + 0;
        
        rect CollBound = RectCD( Vec2( 0.0f, 0.0f ), PushBlock->Dim );
        
        flo32 Friction = PUSH_BLOCK_STOP_FRICTION;
        if( ( PushBlock->DirX != 0.0f ) && ( ( PushBlock->DirX * PushBlock->Velocity.x ) > 0.0f ) ) {
            Friction = PUSH_BLOCK_START_FRICTION;
        }
        
        vec2 PushBlockP = PushBlock->Position + Vec2( 0.0f, -PushBlock->Dim.y * 0.5f );
        CONVEYOR_RESULT ConveyorHor = IsPosOnConveyorHor( AppState, PushBlockP, CollBound );
        
        rect IsOnGroundBound = RectBCD( Vec2( 0.0f, 0.0f ), Vec2( PushBlock->Dim.x, 0.0f ) );
        PushBlock->IsOnGround = IsPosOnCollisionTop( AppState, PushBlockP, IsOnGroundBound );
        
        vec2 Accel = {};
        Accel.x = ( PushBlock->DirX * MaxSpeed - PushBlock->Velocity.x ) * Friction;
        if( ConveyorHor.IsOnConveyor ) {
            Accel.x += ( ConveyorHor.ConveyorAccel.x * Friction );
        }
        Accel.y = -PUSH_BLOCK_GRAVITY;
        
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + PushBlock->Velocity * dT;
        PushBlock->Velocity += Accel * dT;
        
        rect CollBoundPlayer = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM );
        CollBoundPlayer = AddOffset( CollBoundPlayer, Player->Position - PushBlock->Position );
        
        COLLISION_RESULT Coll = CollisionResult( PushBlock->Position, PushBlock->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionLeft     ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectCollisionRight    ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectCollisionTop      ( &BestIntersect, AppState, Coll, CollBound );
            DoesPushBlockIntersectPushBlock( &BestIntersect, AppState, Coll, CollBound, 0 );
            DoesIntersectBreakBlock        ( &BestIntersect, AppState, Coll, CollBound );
            
            DoesIntersectCollisionLeft     ( &BestIntersect, AppState, Coll, CollBoundPlayer );
            DoesIntersectCollisionRight    ( &BestIntersect, AppState, Coll, CollBoundPlayer );
            DoesIntersectCollisionTop      ( &BestIntersect, AppState, Coll, CollBoundPlayer );
            DoesPushBlockIntersectPushBlock( &BestIntersect, AppState, Coll, CollBoundPlayer, 0 );
            DoesIntersectBreakBlock        ( &BestIntersect, AppState, Coll, CollBoundPlayer );
            
            Coll = FinalizeCollision( BestIntersect, Coll );
        }
        Coll.Pos += Coll.dPos;
        
        PushBlock->Position = Coll.Pos;
        PushBlock->Velocity = Coll.Vel;
        
        flo32 MarginX = COLLISION_EPSILON_OFFSET * 5.0f;
        rect  Bound   = RectCD( PushBlock->Position, PushBlock->Dim );
        flo32 X       = 0.0f;
        
        if( Player->Position.x < PushBlock->Position.x ) {
            X = Bound.Left  - ( PLAYER_HALF_WIDTH + MarginX );
        } else {
            X = Bound.Right + ( PLAYER_HALF_WIDTH + MarginX );
        }
        Player->Position.x = X;
        PushBlock->DirX = 0.0f;
        //rect CollBoundIncPlayer = CollBound;
        
        //if( ( Player->Grab_IsActive ) && ( Player->Grab_iPushBlock == iPushBlock ) ) {
        //if( Player->Position.x < PushBlock->Position.x ) {
        //CollBoundIncPlayer.Left  -= ( PLAYER_WIDTH + MarginX * 2.0f );
        //} else {
        //CollBoundIncPlayer.Right += ( PLAYER_WIDTH + MarginX * 2.0f );
        //}
        
        // pass result to player if being pushed/pulled
    }
}

internal void
DrawPushBlock( RENDER_PASS * Pass, PUSH_BLOCK PushBlock ) {
    rect R = RectCD( PushBlock.Position, PushBlock.Dim );
    rect S = AddRadius( R, -TILE_WIDTH * 0.1f );
    
    DrawRect( Pass, R, ToColor( 50, 50, 70  ) );
    DrawRect( Pass, S, ToColor( 80, 80, 100 ) );
}

internal void
DrawPushBlock( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    PUSH_BLOCK_STATE * PushBlockS = Draw->PushBlockS;
    
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
        DrawPushBlock( Pass, PushBlock );
    }
}

internal void
DrawPushBlockDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    PUSH_BLOCK_STATE * PushBlockS = Draw->PushBlockS;
    
    flo32 MarginX = TILE_WIDTH * 0.1f;
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
        rect Bound = RectCD( PushBlock.Position, PushBlock.Dim );
        
        flo32 Height = MaxValue( PushBlock.Dim.y - TILE_HEIGHT * 2.0f, 0.0f );
        vec2 Dim = Vec2( PLAYER_HALF_WIDTH, Height );
        
        rect Bound_Left = RectBRD( GetBL( Bound ), Dim );
        Bound_Left = AddRadius( Bound_Left, MarginX );
        
        rect Bound_Right = RectBLD( GetBR( Bound ), Dim );
        Bound_Right = AddRadius( Bound_Right, MarginX );
        
        DrawRectOutline( Pass, Bound_Left, COLOR_YELLOW );
        DrawRectOutline( Pass, Bound_Right, COLOR_YELLOW );
    }
    
    // Draw can grab
}

internal void
FinalizePushBlock( APP_STATE * AppState ) {
    PLAYER_STATE     * Player     = &AppState->Player;
    PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
    
    if( Player->Grab_IsActive ) {
        PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ 0 ];
        
        rect  R = RectCD( PushBlock.Position, PushBlock.Dim );
        vec2  HalfDim    = PushBlock.Dim * 0.5f;
        vec2  PushBlockP = GetBC( R );
        flo32 MarginY    = TILE_HEIGHT * 0.1f;
        
        boo32 IsOnGround = false;
        
        COLLISION_STATE * Collision = &AppState->Collision;
        UINT32_PAIR Type = Collision->Type[ CollisionType_Top ];
        for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
            vec4 Edge = Collision->Edge[ iEdge + Type.m ];
            
            rect Bound = Rect( Edge.P.x - HalfDim.x, Edge.P.y - MarginY, Edge.Q.x + HalfDim.x, Edge.Q.y + MarginY );
            if( IsInBound( PushBlockP, Bound ) ) {
                IsOnGround = true;
            }
        }
        
        // TODO: or block is dead
        if( !IsOnGround ) {
            Player->Velocity.x    = PushBlock.Velocity.x;
            Player->Grab_IsActive = false;
            Player->Grab_DirX     = 0.0f;
        }
    }
    
    // TODO: Remove dead blocks
}

internal void
Editor_DrawPushBlock( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__PUSH_BLOCK_STATE * PushBlockS = &Editor->PushBlockS;
    
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        EDITOR__PUSH_BLOCK Src       = PushBlockS->PushBlock[ iPushBlock ];
        PUSH_BLOCK         PushBlock = PushBlockC( Src.Bound );
        DrawPushBlock( Pass, PushBlock );
    }
}

internal void
Editor_MovePushBlock( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__PUSH_BLOCK_STATE * PushBlockS = &Editor->PushBlockS;
    
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        EDITOR__PUSH_BLOCK * PushBlock = PushBlockS->PushBlock + iPushBlock;
        
        if( DoesRectIntersectRectExc( SrcBound, PushBlock->Bound ) ) {
            PushBlock->Bound = AddOffset( PushBlock->Bound, Offset );
        }
    }
}

internal void
Editor_MirrorPushBlock( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__PUSH_BLOCK_STATE * PushBlockS = &Editor->PushBlockS;
    
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        EDITOR__PUSH_BLOCK * PushBlock = PushBlockS->PushBlock + iPushBlock;
        
        vec2 Center = GetCenter( PushBlock->Bound );
        vec2 Dim    = GetDim   ( PushBlock->Bound );
        
        if( IsInBound( Center, SrcBound ) ) {
            Center.x = ( SrcBound.Right ) - ( Center.x - SrcBound.Left );
            PushBlock->Bound = RectCD( Center, Dim );
        }
    }
}

#define VERSION__PUSH_BLOCK  ( 1 )
#define FILETAG__PUSH_BLOCK  ( "PUSHBLCK" )
internal void
SavePushBlock( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__PUSH_BLOCK;
    uint32 Version = VERSION__PUSH_BLOCK;
    EDITOR__PUSH_BLOCK_STATE * PushBlockS = &Editor->PushBlockS;
    
    if( PushBlockS->nPushBlock > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, PushBlockS->nPushBlock );
        for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
            EDITOR__PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
            _writem( output, EDITOR__PUSH_BLOCK, PushBlock );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__PUSH_BLOCK
ReadPushBlock( uint32 Version, uint8 ** Ptr ) {
    EDITOR__PUSH_BLOCK Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            rect Bound = _read( ptr, rect );
            
            EDITOR__PUSH_BLOCK PushBlock = {};
            PushBlock.Bound = Bound;
            
            Result = PushBlock;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadPushBlock( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__PUSH_BLOCK;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
        
        PushBlockS->nPushBlock = _read( ptr, uint32 );
        for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
            EDITOR__PUSH_BLOCK Src = ReadPushBlock( Version, &ptr );
            
            PUSH_BLOCK PushBlock = PushBlockC( Src.Bound );
            PushBlockS->PushBlock[ iPushBlock ] = PushBlock;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadPushBlock( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__PUSH_BLOCK;
    EDITOR_STATE           * Editor    = &AppState->Editor;
    EDITOR__PUSH_BLOCK_STATE * PushBlockS = &Editor->PushBlockS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        PushBlockS->nPushBlock = _read( ptr, uint32 );
        for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
            EDITOR__PUSH_BLOCK Src = ReadPushBlock( Version, &ptr );
            PushBlockS->PushBlock[ iPushBlock ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputPushBlockToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS                 Stat       = Editor->Stat;
    EDITOR__PUSH_BLOCK_STATE * PushBlockS = &Editor->PushBlockS;
    
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        EDITOR__PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
        
        vec4 ColorA = ToColor( 80, 80, 100 );
        vec4 ColorB = ToColor( 50, 50, 70 );
        rect Bound  = PushBlock.Bound;
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputRectToLevelBitmap       ( PixelBound, Pixel, xPixel, yPixel, ColorA );
        OutputRectOutlineToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, ColorB );
    }
}
