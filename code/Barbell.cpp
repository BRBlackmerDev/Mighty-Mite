
internal BARBELL
BarbellC( vec2 Pos, vec2 Vel, int32 Skip_iPuncher, boo32 CanHurtPlayer ) {
    BARBELL Barbell = {};
    Barbell.Position = Pos;
    Barbell.Velocity = Vel;
    Barbell.Skip_iPuncher = Skip_iPuncher;
    Barbell.CanHurtPlayer = CanHurtPlayer;
    
    return Barbell;
}

internal void
AddBarbell( APP_STATE * AppState, BARBELL Barbell ) {
    BARBELL_STATE * BarbellS = &AppState->BarbellS;
    
    if( BarbellS->nBarbell < BARBELL_MAX_COUNT ) {
        BarbellS->Barbell[ BarbellS->nBarbell++ ] = Barbell;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new BARBELL, but the state is full! Max Count = %u", BARBELL_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemoveBarbell( APP_STATE * AppState, uint32 iRemove ) {
    BARBELL_STATE * BarbellS = &AppState->BarbellS;
    
    Assert( BarbellS->nBarbell > 0 );
    BarbellS->Barbell[ iRemove ] = BarbellS->Barbell[ --BarbellS->nBarbell ];
}

internal ENTITY_VALID_RESULT
IsPosInBarbellPickUpBound( APP_STATE * AppState, vec2 Pos, rect ByBound ) {
    BARBELL_STATE * BarbellS = &AppState->BarbellS;
    
    ENTITY_VALID_RESULT Result = {};
    
    flo32 MaxDistSq = FLT_MAX;
    for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
        BARBELL Barbell = BarbellS->Barbell[ iBarbell ];
        
        rect R = MinkSub( RectCD( Barbell.Position, BARBELL_DIM * 2.0f ), ByBound );
        
        if( IsInBound( Pos, R ) ) {
            flo32 DistSq = GetLengthSq( Pos - Barbell.Position );
            if( DistSq < MaxDistSq ) {
                MaxDistSq = DistSq;
                
                Result.IsValid = true;
                Result.iEntity = iBarbell;
            }
        }
    }
    
    return Result;
}

internal void
UpdateBarbell( APP_STATE * AppState, flo32 dT ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    BARBELL_STATE   * BarbellS  = &AppState->BarbellS;
    PLAYER_STATE    * Player    = &AppState->Player;
    
    for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
        BARBELL * Barbell = BarbellS->Barbell + iBarbell;
        
        flo32 Friction = BARBELL_AIR_FRICTION;
        
        vec2  BarbellGroundPos = Barbell->Position + Vec2( 0.0f, -BARBELL_HALF_HEIGHT );
        
        boo32 IsOnGround = false;
        { // IsOnGround : TERRAIN
            for( uint32 iJumpBound = 0; iJumpBound < Collision->nJumpBound; iJumpBound++ ) {
                rect R = Collision->JumpBound[ iJumpBound ];
                
                if( IsInBound( BarbellGroundPos, R ) ) {
                    IsOnGround = true;
                }
            }
        }
        
#if 0        
        boo32 IsOnScaffold         = IsBarbellOnScaffold( AppState, BarbellGroundPos );
        boo32 IsOnCollapsePlatform = IsPosOnCollapsePlatform( AppState, BarbellGroundPos );
        boo32 DoGroundFriction = ( IsOnGround ) || ( IsOnScaffold ) || ( IsOnCollapsePlatform );
        if( DoGroundFriction ) {
            Friction = BARBELL_GROUND_FRICTION;
        }
#endif
        
        flo32 Reflect = 0.25f;
        if( IsOnGround ) {
            Friction = BARBELL_GROUND_FRICTION;
        }
        
        vec2 Accel = {};
        Accel.x = -Barbell->Velocity.x * Friction;
        Accel.y = -BARBELL_GRAVITY;
        
        LEVEL_STATS Stat = AppState->Stat;
        if( Barbell->Position.x <= Stat.Bound.Left ) {
            Accel.x += ( 1.0f * Friction );
        }
        if( Barbell->Position.x >= Stat.Bound.Right ) {
            Accel.x -= ( 1.0f * Friction );
        }
        
        rect CollBound = RectCD( Vec2( 0.0f, 0.0f ), BARBELL_DIM );
        
        CONVEYOR_RESULT ConveyorH_Result = IsOnConveyorH( AppState, BarbellGroundPos, CollBound );
        if( ConveyorH_Result.IsOnConveyor ) {
            Accel.x += ( ConveyorH_Result.ConveyorAccel.x * Friction );
        }
        
        vec2 Pos  = Barbell->Position;
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + Barbell->Velocity * dT;
        Barbell->Velocity += Accel * dT;
        flo32 Speed = GetLength( Barbell->Velocity );
        
        COLLISION_RESULT Coll = CollisionResult( Pos, Barbell->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll    ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectScaffold        ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectCollapsePlatform( &BestIntersect, AppState, Coll, CollBound );
            
            DoesIntersectPushBlock ( &BestIntersect, AppState, Coll, CollBound );
            DoesIntersectBreakBlock( &BestIntersect, AppState, Coll, CollBound );
            
            DoesIntersectLevelBound( &BestIntersect, AppState, Coll );
            
            Coll = FinalizeCollision( BestIntersect, Coll, Reflect );
        }
        Coll.Pos += Coll.dPos;
        
        if( ( Coll.HadCollision ) && ( Speed > 10.0f ) ) {
            AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_Barbell_Bounce01 + RandomU32InRange( 0, 1 ) );
            PlayAudio( AppState, Sound_Select, 0.5f );
        }
        
        Barbell->Position = Coll.Pos;
        Barbell->Velocity = Coll.Vel;
        
        if( Speed > 10.0f ) {
            // TODO: Probably check for some airborne velocity threshold
            ENTITY_VALID_RESULT IsInStunBound = IsInPuncherStunBound( AppState, Barbell->Position, CollBound );
            ENTITY_VALID_RESULT Spotter_IsInStunBound = IsInSpotterStunBound( AppState, Barbell->Position, CollBound );
            if( ( IsInStunBound.IsValid ) && ( ( int32 )IsInStunBound.iEntity != Barbell->Skip_iPuncher ) ) {
                PUNCHER_STATE * PuncherS = &AppState->PuncherS;
                PUNCHER       * Puncher  = PuncherS->Puncher + IsInStunBound.iEntity;
                
                flo32 DirX = 0.0f;
                if( Barbell->Velocity.x != 0.0f ) {
                    DirX = Barbell->Velocity.x / fabsf( Barbell->Velocity.x );
                } else if( Puncher->Velocity.x != 0.0f ) {
                    DirX = Puncher->Velocity.x / fabsf( Puncher->Velocity.x );
                }
                StunPuncher( AppState, IsInStunBound.iEntity, DirX );
                
                AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_Barbell_Hit01 + RandomU32InRange( 0, 1 ) );
                PlayAudio( AppState, Sound_Select, 0.5f );
                
                Barbell->DoKill = true;
            } else if( Spotter_IsInStunBound.IsValid ) {
                StunSpotter( AppState, Spotter_IsInStunBound.iEntity, GetNormal( Barbell->Velocity ) );
                
                AUDIO_ID Sound_Select = ( AUDIO_ID )( AudioID_Sound_Barbell_Hit01 + RandomU32InRange( 0, 1 ) );
                PlayAudio( AppState, Sound_Select, 0.5f );
                
                Barbell->DoKill = true;
            } else if( Barbell->CanHurtPlayer ) {
                rect PlayerBound  = RectBCD( Vec2( 0.0f, 0.0f ), PLAYER_DIM * Vec2( 0.6f, 0.7f ) );
                rect BarbellBound = MinkSub( RectCD( Barbell->Position, BARBELL_DIM ), PlayerBound );
                if( IsInBound( Player->Position, BarbellBound ) ) {
                    Barbell->CanHurtPlayer = false;
                    DamagePlayer( AppState, 1 );
                }
            }
        }
    }
}

internal void
DrawBarbell( RENDER_PASS * Pass, BARBELL Barbell ) {
    if( !Barbell.DoKill ) {
        flo32 Bar_Width  = TILE_WIDTH  * 0.1f;
        vec2  Bar_Dim    = Vec2( Bar_Width, BARBELL_HEIGHT );
        
        flo32 End_Height = TILE_HEIGHT * 0.25f;
        vec2  End_Dim    = Vec2( BARBELL_WIDTH, End_Height );
        
        flo32 Radians = Barbell.Radians;
        flo32 Dist    = BARBELL_HALF_HEIGHT - ( End_Height * 0.5f );
        vec2  Offset  = ToDirection2D( Radians + PI * 0.5f ) * Dist;
        
        vec2  PosA = Barbell.Position - Offset;
        vec2  PosB = Barbell.Position + Offset;
        
        orect Bar_Bound  = ORectCD( Barbell.Position, Bar_Dim, Radians );
        orect End_BoundA = ORectCD( PosA, End_Dim, Radians );
        orect End_BoundB = ORectCD( PosB, End_Dim, Radians );
        
        vec4 ColorA = ToColor( 50, 40, 40 );
        vec4 ColorB = ToColor( 70, 60, 60 );
        
        DrawORect( Pass, Bar_Bound, ColorA );
        DrawORect( Pass, End_BoundA, ColorB );
        DrawORect( Pass, End_BoundB, ColorB );
    }
}

internal void
DrawBarbell( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    BARBELL_STATE * BarbellS = Draw->BarbellS;
    
    for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
        BARBELL Barbell = BarbellS->Barbell[ iBarbell ];
        DrawBarbell( Pass, Barbell );
    }
}

internal void
DrawBarbellDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    BARBELL_STATE * BarbellS = Draw->BarbellS;
    
    for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
        BARBELL Barbell = BarbellS->Barbell[ iBarbell ];
        
        rect R = RectCD( Barbell.Position, BARBELL_DIM );
        DrawRectOutline( Pass, R, COLOR_RED );
    }
}

internal void
FinalizeBarbell( APP_STATE * AppState ) {
    BARBELL_STATE * BarbellS = &AppState->BarbellS;
    
    uint32 NewCount = 0;
    for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
        BARBELL Barbell = BarbellS->Barbell[ iBarbell ];
        
        boo32 IsActive = ( Barbell.Position.y > AppState->DeathPlaneY )
            && ( !Barbell.DoKill );
        
        if( IsActive ) {
            BarbellS->Barbell[ NewCount++ ] = Barbell;
        }
    }
    BarbellS->nBarbell = NewCount;
}

internal void
Editor_DrawBarbell( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__BARBELL_STATE * BarbellS = &Editor->BarbellS;
    
    for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
        EDITOR__BARBELL Src     = BarbellS->Barbell[ iBarbell ];
        BARBELL         Barbell = BarbellC( Src.Position );
        DrawBarbell( Pass, Barbell );
    }
}

#define VERSION__BARBELL  ( 1 )
#define FILETAG__BARBELL  ( "BARBELL_" )
internal void
SaveBarbell( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__BARBELL;
    uint32 Version = VERSION__BARBELL;
    EDITOR__BARBELL_STATE * BarbellS = &Editor->BarbellS;
    
    if( BarbellS->nBarbell > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, BarbellS->nBarbell );
        for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
            EDITOR__BARBELL Barbell = BarbellS->Barbell[ iBarbell ];
            _writem( output, EDITOR__BARBELL, Barbell );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__BARBELL
ReadBarbell( uint32 Version, uint8 ** Ptr ) {
    EDITOR__BARBELL Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            vec2 Pos = _read( ptr, vec2 );
            
            EDITOR__BARBELL Barbell = {};
            Barbell.Position = Pos;
            
            Result = Barbell;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadBarbell( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__BARBELL;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        BARBELL_STATE * BarbellS = &AppState->BarbellS;
        
        BarbellS->nBarbell = _read( ptr, uint32 );
        for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
            EDITOR__BARBELL Src = ReadBarbell( Version, &ptr );
            
            BARBELL Barbell = BarbellC( Src.Position );
            BarbellS->Barbell[ iBarbell ] = Barbell;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadBarbell( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__BARBELL;
    EDITOR_STATE          * Editor   = &AppState->Editor;
    EDITOR__BARBELL_STATE * BarbellS = &Editor->BarbellS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        BarbellS->nBarbell = _read( ptr, uint32 );
        for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
            EDITOR__BARBELL Src = ReadBarbell( Version, &ptr );
            BarbellS->Barbell[ iBarbell ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputBarbellToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS                 Stat       = Editor->Stat;
    EDITOR__BARBELL_STATE * BarbellS = &Editor->BarbellS;
    
    for( uint32 iBarbell = 0; iBarbell < BarbellS->nBarbell; iBarbell++ ) {
        EDITOR__BARBELL Barbell = BarbellS->Barbell[ iBarbell ];
        
        vec4 Color = ToColor( 0, 0, 200 );
        rect Bound = RectCD( Barbell.Position, BARBELL_DIM );
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputCircleToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}