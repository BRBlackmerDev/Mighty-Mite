
internal MONEY
MoneyC( MONEY_TYPE Type, vec2 Pos, vec2 Vel ) {
    MONEY Money = {};
    Money.Type       = Type;
    Money.HasGravity = true;
    Money.Position   = Pos;
    Money.Velocity   = Vel;
    
    return Money;
}

internal MONEY
MoneyC( MONEY_TYPE Type, vec2 Pos ) {
    MONEY Money = {};
    Money.Type       = Type;
    Money.HasGravity = false;
    Money.Position   = Pos;
    
    return Money;
}

internal void
AddMoney( APP_STATE * AppState, MONEY Money ) {
    MONEY_STATE * MoneyS = &AppState->MoneyS;
    
    if( MoneyS->nMoney < MONEY_MAX_COUNT ) {
        MoneyS->Money[ MoneyS->nMoney++ ] = Money;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new MONEY, but the state is full! Max Count = %u", MONEY_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemoveMoney( APP_STATE * AppState, uint32 iRemove ) {
    MONEY_STATE * MoneyS = &AppState->MoneyS;
    
    Assert( MoneyS->nMoney > 0 );
    MoneyS->Money[ iRemove ] = MoneyS->Money[ --MoneyS->nMoney ];
}

internal ENTITY_VALID_RESULT
IsPosInMoneyPickUpBound( APP_STATE * AppState, vec2 Pos, rect ByBound ) {
    MONEY_STATE * MoneyS = &AppState->MoneyS;
    
    ENTITY_VALID_RESULT Result = {};
    
    MONEY_DIMS;
    
    flo32 MaxDistSq = FLT_MAX;
    for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
        MONEY Money = MoneyS->Money[ iMoney ];
        
        rect R = MinkSub( RectBCD( Money.Position, MoneyDims[ Money.Type ] ), ByBound );
        
        if( IsInBound( Pos, R ) ) {
            flo32 DistSq = GetLengthSq( Pos - Money.Position );
            if( DistSq < MaxDistSq ) {
                MaxDistSq = DistSq;
                
                Result.IsValid = true;
                Result.iEntity = iMoney;
            }
        }
    }
    
    return Result;
}

internal void
UpdateMoney( APP_STATE * AppState, flo32 dT ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    MONEY_STATE     * MoneyS    = &AppState->MoneyS;
    PLAYER_STATE    * Player    = &AppState->Player;
    
    MONEY_DIMS;
    
    for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
        MONEY * Money = MoneyS->Money + iMoney;
        
        if( Money->HasGravity ) {
            flo32 Friction = MONEY_FRICTION;
            
            vec2  MoneyGroundPos = Money->Position;
            
            vec2 Accel = {};
            Accel.x = -Money->Velocity.x * Friction;
            Accel.y = -MONEY_GRAVITY;
            
            LEVEL_STATS Stat = AppState->Stat;
            if( Money->Position.x <= Stat.Bound.Left ) {
                Accel.x += ( 1.0f * Friction );
            }
            if( Money->Position.x >= Stat.Bound.Right ) {
                Accel.x -= ( 1.0f * Friction );
            }
            
            rect CollBound = RectBCD( Vec2( 0.0f, 0.0f ), MoneyDims[ Money->Type ] );
            
            CONVEYOR_RESULT ConveyorH_Result = IsOnConveyorH( AppState, MoneyGroundPos, CollBound );
            if( ConveyorH_Result.IsOnConveyor ) {
                Accel.x += ( ConveyorH_Result.ConveyorAccel.x * Friction );
            }
            
            vec2 Pos  = Money->Position;
            vec2 dPos = Accel * ( dT * dT * 0.5f ) + Money->Velocity * dT;
            Money->Velocity += Accel * dT;
            
            COLLISION_RESULT Coll = CollisionResult( Pos, Money->Velocity, dPos );
            for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
                RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
                DoesIntersectCollisionAll    ( &BestIntersect, AppState, Coll, CollBound );
                DoesIntersectScaffold        ( &BestIntersect, AppState, Coll, CollBound );
                DoesIntersectCollapsePlatform( &BestIntersect, AppState, Coll, CollBound );
                DoesIntersectLevelBound( &BestIntersect, AppState, Coll );
                
                Coll = FinalizeCollision( BestIntersect, Coll );
            }
            Coll.Pos += Coll.dPos;
            
            Money->Position = Coll.Pos;
            Money->Velocity = Coll.Vel;
        }
    }
}

internal void
DrawMoney( RENDER_PASS * Pass, MONEY Money ) {
    MONEY_DIMS;
    MONEY_COLORS;
    
    switch( Money.Type ) {
        case MoneyType_1: {
            vec2  Dim    = MoneyDims  [ Money.Type ];
            flo32 Radius = Dim.x * 0.5f;
            vec2  Pos    = Money.Position + Vec2( 0.0f, Radius );
            vec4  Color  = MoneyColors[ Money.Type ];
            
            DrawCircle( Pass, Pos, Radius, Color );
        } break;
        
        case MoneyType_5: {
            vec2  Dim    = MoneyDims  [ Money.Type ];
            flo32 Radius = Dim.x * 0.5f;
            vec2  Pos    = Money.Position + Vec2( 0.0f, Radius );
            vec4  Color  = MoneyColors[ Money.Type ];
            
            DrawHexagon( Pass, Pos, Radius, Color );
        } break;
        
        case MoneyType_10: {
            vec2  Dim    = MoneyDims  [ Money.Type ];
            flo32 Radius = Dim.x * 0.5f;
            vec2  Pos    = Money.Position + Vec2( 0.0f, Radius );
            vec4  Color  = MoneyColors[ Money.Type ];
            
            DrawOctagon( Pass, Pos, Radius, Color );
        } break;
        
        case MoneyType_25: {
            vec2  Dim    = MoneyDims  [ Money.Type ];
            flo32 Radius = Dim.x * 0.5f;
            vec2  Pos    = Money.Position + Vec2( 0.0f, Radius );
            vec4  Color  = MoneyColors[ Money.Type ];
            
            DrawCircle( Pass, Pos, Radius, Color );
        } break;
        
        case MoneyType_100: {
            vec2  Dim    = MoneyDims  [ Money.Type ];
            rect  Bound  = RectBCD( Money.Position, Dim );
            vec4  Color  = MoneyColors[ Money.Type ];
            
            DrawRect( Pass, Bound, Color );
        } break;
    }
}

internal void
DrawMoney( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    MONEY_STATE * MoneyS = Draw->MoneyS;
    
    for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
        MONEY Money = MoneyS->Money[ iMoney ];
        DrawMoney( Pass, Money );
    }
}

internal void
DrawMoneyDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    MONEY_STATE * MoneyS = Draw->MoneyS;
    
    MONEY_DIMS;
    
    for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
        MONEY Money = MoneyS->Money[ iMoney ];
        
        rect R = RectBCD( Money.Position, MoneyDims[ Money.Type ] );
        DrawRectOutline( Pass, R, COLOR_RED );
    }
}

internal void
PlayerPickUpMoney( APP_STATE * AppState, MONEY Money ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    flo32 Value = 0.0f;
    switch( Money.Type ) {
        case MoneyType_1: {
            Value = 0.01f;
        } break;
        
        case MoneyType_5: {
            Value = 0.05f;
        } break;
        
        case MoneyType_10: {
            Value = 0.10f;
        } break;
        
        case MoneyType_25: {
            Value = 0.25f;
        } break;
        
        case MoneyType_100: {
            Value = 1.0f;
        } break;
    }
    
    Player->Money += Value;
}

internal void
FinalizeMoney( APP_STATE * AppState ) {
    MONEY_STATE  * MoneyS = &AppState->MoneyS;
    PLAYER_STATE * Player = &AppState->Player;
    
    MONEY_DIMS;
    
    vec2 PlayerP = Player->Position;
    
    boo32 DoPickUp = false;
    
    uint32 NewCount = 0;
    for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
        MONEY Money = MoneyS->Money[ iMoney ];
        
        vec2 Dim = MoneyDims[ Money.Type ];
        rect MoneyBound = MinkSub( RectBCD( Money.Position, Dim ), RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM ) );
        
        if( IsInBound( PlayerP, MoneyBound ) ) {
            PlayerPickUpMoney( AppState, Money );
            DoPickUp = true;
        } else {
            MoneyS->Money[ NewCount++ ] = Money;
        }
        
        // TODO: Do pick up!!
    }
    MoneyS->nMoney = NewCount;
    
    if( DoPickUp ) {
        PlayAudio( AppState, AudioID_Sound_Money, 0.25f );
    }
}

internal void
Editor_DrawMoney( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__MONEY_STATE * MoneyS = &Editor->MoneyS;
    
    for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
        EDITOR__MONEY Src   = MoneyS->Money[ iMoney ];
        MONEY         Money = MoneyC( Src.Type, Src.Position );
        DrawMoney( Pass, Money );
    }
}

internal void
Editor_MoveMoney( EDITOR_STATE * Editor, rect SrcBound, vec2 Offset ) {
    EDITOR__MONEY_STATE * MoneyS = &Editor->MoneyS;
    
    for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
        EDITOR__MONEY * Money = MoneyS->Money + iMoney;
        
        if( IsInBound( Money->Position, SrcBound ) ) {
            Money->Position += Offset;
        }
    }
}

internal void
Editor_MirrorMoney( EDITOR_STATE * Editor, rect SrcBound ) {
    EDITOR__MONEY_STATE * MoneyS = &Editor->MoneyS;
    
    for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
        EDITOR__MONEY * Money = MoneyS->Money + iMoney;
        
        if( IsInBound( Money->Position, SrcBound ) ) {
            Money->Position.x = ( SrcBound.Right ) - ( Money->Position.x - SrcBound.Left );
        }
    }
}

#define VERSION__MONEY  ( 1 )
#define FILETAG__MONEY  ( "MONEY___" )
internal void
SaveMoney( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__MONEY;
    uint32 Version = VERSION__MONEY;
    EDITOR__MONEY_STATE * MoneyS = &Editor->MoneyS;
    
    if( MoneyS->nMoney > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, MoneyS->nMoney );
        for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
            EDITOR__MONEY Money = MoneyS->Money[ iMoney ];
            _writem( output, EDITOR__MONEY, Money );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__MONEY
ReadMoney( uint32 Version, uint8 ** Ptr ) {
    EDITOR__MONEY Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            MONEY_TYPE Type = _read( ptr, MONEY_TYPE );
            vec2       Pos  = _read( ptr, vec2 );
            
            EDITOR__MONEY Money = {};
            Money.Type     = Type;
            Money.Position = Pos;
            
            Result = Money;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadMoney( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__MONEY;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        MONEY_STATE * MoneyS = &AppState->MoneyS;
        
        MoneyS->nMoney = _read( ptr, uint32 );
        for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
            EDITOR__MONEY Src = ReadMoney( Version, &ptr );
            
            MONEY Money = MoneyC( Src.Type, Src.Position );
            MoneyS->Money[ iMoney ] = Money;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadMoney( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__MONEY;
    EDITOR_STATE          * Editor   = &AppState->Editor;
    EDITOR__MONEY_STATE * MoneyS = &Editor->MoneyS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        MoneyS->nMoney = _read( ptr, uint32 );
        for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
            EDITOR__MONEY Src = ReadMoney( Version, &ptr );
            MoneyS->Money[ iMoney ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputMoneyToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS           Stat   = Editor->Stat;
    EDITOR__MONEY_STATE * MoneyS = &Editor->MoneyS;
    
    MONEY_DIMS;
    MONEY_COLORS;
    
    for( uint32 iMoney = 0; iMoney < MoneyS->nMoney; iMoney++ ) {
        EDITOR__MONEY Money = MoneyS->Money[ iMoney ];
        
        switch( Money.Type ) {
            case MoneyType_1: {
                vec4  Color  = MoneyColors[ Money.Type ];
                rect  Bound  = RectBCD( Money.Position, MoneyDims[ Money.Type ] );
                
                UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
                OutputCircleToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
            } break;
            
            case MoneyType_5: {
                vec4  Color  = MoneyColors[ Money.Type ];
                rect  Bound  = RectBCD( Money.Position, MoneyDims[ Money.Type ] );
                
                UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
                OutputCircleToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
            } break;
            
            case MoneyType_10: {
                vec4  Color  = MoneyColors[ Money.Type ];
                rect  Bound  = RectBCD( Money.Position, MoneyDims[ Money.Type ] );
                
                UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
                OutputCircleToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
            } break;
            
            case MoneyType_25: {
                vec4  Color  = MoneyColors[ Money.Type ];
                rect  Bound  = RectBCD( Money.Position, MoneyDims[ Money.Type ] );
                
                UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
                OutputCircleToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
            } break;
            
            case MoneyType_100: {
                vec4  Color  = MoneyColors[ Money.Type ];
                rect  Bound  = RectBCD( Money.Position, MoneyDims[ Money.Type ] );
                
                UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
                OutputRectToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
            } break;
        }
    }
}