
internal SPOTTER
SpotterC( vec2 Pos ) {
    SPOTTER Spotter = {};
    Spotter.Mode     = SpotterMode_Idle;
    Spotter.Position = Pos;
    
    return Spotter;
}

internal void
AddSpotter( APP_STATE * AppState, vec2 Pos ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    
    if( SpotterS->nSpotter < SPOTTER_MAX_COUNT ) {
        SPOTTER Spotter = SpotterC( Pos );
        SpotterS->Spotter[ SpotterS->nSpotter++ ] = Spotter;
    } else {
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Attempted to create new SPOTTER, but the state is full! Max Count = %u", SPOTTER_MAX_COUNT );
        CONSOLE_STRING( str );
    }
}

internal void
RemoveSpotter( APP_STATE * AppState, uint32 iRemove ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    
    Assert( SpotterS->nSpotter > 0 );
    SpotterS->Spotter[ iRemove ] = SpotterS->Spotter[ --SpotterS->nSpotter ];
}

internal ENTITY_VALID_RESULT
IsPosOnSpotter( APP_STATE * AppState, vec2 Pos ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    
    ENTITY_VALID_RESULT Result = {};
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
        
        rect R = RectCD( Spotter.Position, SPOTTER_DIM );
        R.Left  -= ( TILE_WIDTH * 0.4f );
        R.Right += ( TILE_WIDTH * 0.4f );
        R.Bottom = R.Top - SPOTTER_RADIUS - ( TILE_HEIGHT * 0.5f );
        R.Top   += ( TILE_HEIGHT * 0.3f );
        
        if( IsInBound( Pos, R ) ) {
            Result.IsValid = true;
            Result.iEntity = iSpotter;
        }
    }
    
    return Result;
}

internal boo32
IsTouchingSpotter( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    
    boo32 Result = {};
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
        
        vec2 Dim   = SPOTTER_DIM;
        rect Bound = RectCD( Spotter.Position, Dim );
        Bound = MinkSub( Bound, CollBound );
        
        if( IsInBound( Pos, Bound ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal ENTITY_VALID_RESULT
IsInSpotterStunBound( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    
    ENTITY_VALID_RESULT Result = {};
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
        
        vec2 Dim   = SPOTTER_DIM;
        rect Bound = RectBCD( Spotter.Position, Dim );
        // TODO: Tune this!!
        Bound = MinkSub( Bound, CollBound );
        
        if( IsInBound( Pos, Bound ) ) {
            Result.IsValid = true;
            Result.iEntity = iSpotter;
        }
    }
    
    return Result;
}

internal ENTITY_VALID_RESULT
IsInSpotterStunBound( APP_STATE * AppState, vec2 Pos, flo32 Radius ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    
    flo32 Radius0  = ( SPOTTER_RADIUS - ( TILE_WIDTH * 0.1f ) ) + ( Radius - ( TILE_WIDTH * 0.1f ) );
    flo32 RadiusSq = ( Radius0 * Radius0 );
    
    ENTITY_VALID_RESULT Result = {};
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
        
        flo32 DistSq = GetLengthSq( Spotter.Position - Pos );
        if( DistSq < RadiusSq ) {
            Result.IsValid = true;
            Result.iEntity = iSpotter;
        }
    }
    
    return Result;
}

internal void
StunSpotter( APP_STATE * AppState, uint32 iSpotter, vec2 Dir ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    SPOTTER       * Spotter  = SpotterS->Spotter + iSpotter;
    Spotter->Mode   = SpotterMode_IsStunned;
    Spotter->IsStunned_Timer = 0.0f;
    
    Spotter->Velocity = 12.0f * Dir;
}

internal void
StunSpotterX( APP_STATE * AppState, uint32 iSpotter, flo32 DirX ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    SPOTTER       * Spotter  = SpotterS->Spotter + iSpotter;
    Spotter->Mode   = SpotterMode_IsStunned;
    Spotter->IsStunned_Timer = 0.0f;
    
    Spotter->Velocity.x = 14.0f * DirX;
}

internal void
StunSpotterY( APP_STATE * AppState, uint32 iSpotter ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    SPOTTER       * Spotter  = SpotterS->Spotter + iSpotter;
    Spotter->Mode   = SpotterMode_IsStunned;
    Spotter->IsStunned_Timer = 0.0f;
    
    Spotter->Velocity.y = -( PLAYER_JUMP_SPEED * 0.2f );
}

internal void
UpdateSpotter( APP_STATE * AppState, flo32 dT ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    SPOTTER_STATE   * SpotterS  = &AppState->SpotterS;
    PLAYER_STATE    * Player    = &AppState->Player;
    
    vec2 PlayerP = Player->Position + Vec2( 0.0f, TILE_HEIGHT * 1.0f );
    Player->Stamina_IsBeingDrained = false;
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        SPOTTER * Spotter = SpotterS->Spotter + iSpotter;
        
        rect See_Bound   = RectCD( Spotter->Position, SPOTTER_SEE_DIM );
        rect Drain_Bound = RectCD( Spotter->Position, SPOTTER_DRAIN_DIM );
        
        switch( Spotter->Mode ) {
            case SpotterMode_Idle: {
                if( ( Player->Stamina_IsEnabled ) && ( IsInBound( PlayerP, See_Bound ) ) ) {
                    Spotter->Mode = SpotterMode_IsChasing;
                }
            } break;
            
            case SpotterMode_IsChasing: {
                if( Player->Stamina_IsEnabled ) {
                    if( IsInBound( PlayerP, Drain_Bound ) ) {
                        Spotter->Mode        = SpotterMode_IsDraining;
                        Spotter->Pupil_Timer = 0.0f;
                    }
                } else {
                    Spotter->Mode = SpotterMode_Idle;
                }
            } break;
            
            case SpotterMode_IsDraining: {
                if( Player->Stamina_IsEnabled ) {
                    if( !IsInBound( PlayerP, Drain_Bound ) ) {
                        Spotter->Mode = SpotterMode_IsChasing;
                    }
                } else {
                    Spotter->Mode = SpotterMode_Idle;
                }
            } break;
            
            case SpotterMode_IsStunned: {
                Spotter->IsStunned_Timer += dT;
            } break;
        }
        
        flo32 Speed = SPOTTER_MOVE_SPEED;
        vec2 Dir = {};
        switch( Spotter->Mode ) {
            case SpotterMode_Idle: {
                
            } break;
            
            case SpotterMode_IsChasing: {
                Dir = GetNormal( PlayerP - Spotter->Position );
            } break;
            
            case SpotterMode_IsDraining: {
                Player->Stamina_IsBeingDrained = true;
            } break;
            
            case SpotterMode_IsStunned: {
                Dir.y = 1.0f;
                Speed = SPOTTER_STUNNED_SPEED;
            } break;
        }
        
        vec2 Accel = {};
        Accel = ( Dir * Speed - Spotter->Velocity ) * SPOTTER_FRICTION;
        
        LEVEL_STATS Stat = AppState->Stat;
        if( Spotter->Position.x <= Stat.Bound.Left ) {
            Accel.x += ( 1.0f * SPOTTER_FRICTION );
        }
        if( Spotter->Position.x >= Stat.Bound.Right ) {
            Accel.x -= ( 1.0f * SPOTTER_FRICTION );
        }
        
        vec2 Pos  = Spotter->Position;
        vec2 dPos = Accel * ( dT * dT * 0.5f ) + Spotter->Velocity * dT;
        Spotter->Velocity += Accel * dT;
        Spotter->Position += dPos;
        
        vec2 DirToPlayer = GetNormal( PlayerP - Spotter->Position );
        flo32 Pupil_Radius   = SPOTTER_RADIUS * 0.4f;
        vec2  Pupil_Position = Spotter->Position;
        
        switch( Spotter->Mode )  {
            case SpotterMode_Idle: {
                flo32 MaxRadius  = SPOTTER_RADIUS * 0.4f;
                flo32 dRadius = 1.0f / ( 15.0f / 60.0f );
                
                Spotter->Pupil_Radius   = MinValue( Spotter->Pupil_Radius + dRadius * dT, MaxRadius );
                Spotter->Pupil_Position = Spotter->Position;
            } break;
            
            case SpotterMode_IsChasing: {
                flo32 MaxRadius  = SPOTTER_RADIUS * 0.5f;
                flo32 dRadius = 1.0f / ( 15.0f / 60.0f );
                
                Spotter->Pupil_Radius = MinValue( Spotter->Pupil_Radius + dRadius * dT, MaxRadius );
                Spotter->Pupil_Position = Spotter->Position + DirToPlayer * ( TILE_WIDTH * 0.25f );
            } break;
            
            case SpotterMode_IsDraining: {
                flo32 Cycle_TargetTime = ( 15.0f / 60.0f );
                flo32 t = Clamp01( Spotter->Pupil_Timer / Cycle_TargetTime );
                flo32 MinRadius  = lerp( SPOTTER_RADIUS * 0.2f, t, SPOTTER_RADIUS * 0.1f );
                flo32 dRadius = 1.0f / ( 15.0f / 60.0f );
                
                Spotter->Pupil_Radius = MaxValue( Spotter->Pupil_Radius - dRadius * dT, MinRadius );
                Spotter->Pupil_Position = Spotter->Position + DirToPlayer * ( TILE_WIDTH * 0.35f );
                
                Spotter->Pupil_Timer += dT;
                if( Spotter->Pupil_Timer >= Cycle_TargetTime ) {
                    Spotter->Pupil_Timer = 0.0f;
                }
            } break;
            
            case SpotterMode_IsStunned: {
                Spotter->Pupil_Radius = SPOTTER_RADIUS * 0.1f;
                Spotter->Pupil_Position = Spotter->Position + Vec2( 0.0f, SPOTTER_RADIUS * 0.9f );
            } break;
        }
        
#if 0        
        COLLISION_RESULT Coll = CollisionResult( Pos, Spotter->Velocity, dPos );
        for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < 4 ); Iter++ ) {
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            DoesIntersectCollisionAll    ( &BestIntersect, AppState, Coll, SPOTTER_RADIUS );
            DoesIntersectScaffold        ( &BestIntersect, AppState, Coll, SPOTTER_RADIUS );
            DoesIntersectCollapsePlatform( &BestIntersect, AppState, Coll, SPOTTER_RADIUS );
            
            DoesIntersectPushBlock ( &BestIntersect, AppState, Coll, SPOTTER_RADIUS );
            DoesIntersectBreakBlock( &BestIntersect, AppState, Coll, SPOTTER_RADIUS );
            
            DoesIntersectLevelBound( &BestIntersect, AppState, Coll );
            
            Coll = FinalizeCollision( BestIntersect, Coll, Reflect );
        }
        Coll.Pos += Coll.dPos;
        
        Spotter->Position = Coll.Pos;
        Spotter->Velocity = Coll.Vel;
#endif
    }
}

internal void
DrawSpotter( RENDER_PASS * Pass, SPOTTER Spotter ) {
    
    vec4 Color = ToColor( 220, 50, 30 );
    DrawCircle( Pass, Spotter.Position, SPOTTER_RADIUS,        Color );
    DrawCircle( Pass, Spotter.Position, SPOTTER_RADIUS * 0.95f, COLOR_BLACK );
    DrawCircle( Pass, Spotter.Pupil_Position, Spotter.Pupil_Radius, Color );
}

internal void
DrawSpotter( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    SPOTTER_STATE * SpotterS = Draw->SpotterS;
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
        DrawSpotter( Pass, Spotter );
    }
}

internal void
DrawSpotterDebug( RENDER_PASS * Pass, APP_STATE * AppState, DRAW_STATE * Draw ) {
    SPOTTER_STATE * SpotterS = Draw->SpotterS;
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
        
        rect See_Bound   = RectCD( Spotter.Position, SPOTTER_SEE_DIM   );
        rect Drain_Bound = RectCD( Spotter.Position, SPOTTER_DRAIN_DIM );
        
        DrawRectOutline( Pass, See_Bound,   COLOR_GREEN );
        DrawRectOutline( Pass, Drain_Bound, COLOR_RED   );
    }
}

internal void
FinalizeSpotter( APP_STATE * AppState ) {
    SPOTTER_STATE * SpotterS = &AppState->SpotterS;
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        SPOTTER * Spotter = SpotterS->Spotter + iSpotter;
        
        switch( Spotter->Mode )  {
            case SpotterMode_Idle: {
            } break;
            
            case SpotterMode_IsChasing: {
            } break;
            
            case SpotterMode_IsDraining: {
            } break;
            
            case SpotterMode_IsStunned: {
                if( Spotter->IsStunned_Timer >= SPOTTER_STUNNED_TARGET_TIME ) {
                    Spotter->Mode = SpotterMode_Idle;
                }
            } break;
        }
    }
    
    uint32 NewCount = 0;
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
        
        boo32 IsActive = ( Spotter.Mode != SpotterMode_IsDead );
        
        if( IsActive ) {
            SpotterS->Spotter[ NewCount++ ] = Spotter;
        }
    }
    SpotterS->nSpotter = NewCount;
}


internal void
Editor_DrawSpotter( RENDER_PASS * Pass, EDITOR_STATE * Editor ) {
    EDITOR__SPOTTER_STATE * SpotterS = &Editor->SpotterS;
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        EDITOR__SPOTTER Src     = SpotterS->Spotter[ iSpotter ];
        SPOTTER         Spotter = SpotterC( Src.Position );
        DrawSpotter( Pass, Spotter );
    }
}

#define VERSION__SPOTTER  ( 1 )
#define FILETAG__SPOTTER  ( "SPOTTER_" )
internal void
SaveSpotter( PLATFORM * Platform, EDITOR_STATE * Editor, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SPOTTER;
    uint32 Version = VERSION__SPOTTER;
    EDITOR__SPOTTER_STATE * SpotterS = &Editor->SpotterS;
    
    if( SpotterS->nSpotter > 0 ) {
        MEMORY  _output = SubArena( TempMemory );
        MEMORY * output = &_output;
        
        WriteEntityFileHeader( output, FileTag, Version );
        
        _writem( output, uint32, SpotterS->nSpotter );
        for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
            EDITOR__SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
            _writem( output, EDITOR__SPOTTER, Spotter );
        }
        
        OutputFile( Platform, output, SaveDir, FileName, FileTag );
        _popSize( TempMemory, output->size );
    } else if( Platform->DoesFileExist( SaveDir, FileName, FileTag ) ) {
        Platform->DeleteFile( SaveDir, FileName, FileTag );
    }
}

internal EDITOR__SPOTTER
ReadSpotter( uint32 Version, uint8 ** Ptr ) {
    EDITOR__SPOTTER Result = {};
    uint8 * ptr = *Ptr;
    
    switch( Version ) {
        case 1: {
            vec2 Pos = _read( ptr, vec2 );
            
            EDITOR__SPOTTER Spotter = {};
            Spotter.Position = Pos;
            
            Result = Spotter;
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
    
    *Ptr = ptr;
    return Result;
}

internal void
GAME_LoadSpotter( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SPOTTER;
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        SPOTTER_STATE * SpotterS = &AppState->SpotterS;
        
        SpotterS->nSpotter = _read( ptr, uint32 );
        for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
            EDITOR__SPOTTER Src = ReadSpotter( Version, &ptr );
            
            SPOTTER Spotter = SpotterC( Src.Position );
            SpotterS->Spotter[ iSpotter ] = Spotter;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
EDITOR_LoadSpotter( PLATFORM * Platform, APP_STATE * AppState, MEMORY * TempMemory, char * SaveDir, char * FileName ) {
    char * FileTag = FILETAG__SPOTTER;
    EDITOR_STATE          * Editor   = &AppState->Editor;
    EDITOR__SPOTTER_STATE * SpotterS = &Editor->SpotterS;
    
    FILE_DATA File = Platform->ReadFile( TempMemory, SaveDir, FileName, FileTag );
    if( File.contents ) {
        uint8 * ptr = ( uint8 * )File.contents;
        
        uint32 Version = VerifyEntityHeaderAndGetVersion( &ptr, FileTag );
        
        SpotterS->nSpotter = _read( ptr, uint32 );
        for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
            EDITOR__SPOTTER Src = ReadSpotter( Version, &ptr );
            SpotterS->Spotter[ iSpotter ] = Src;
        }
        
        _popSize( TempMemory, File.size );
    }
}

internal void
OutputSpotterToLevelBitmap( EDITOR_STATE * Editor, uint32 nPixelPerCell, uint32 * Pixel, uint32 xPixel, uint32 yPixel ) {
    LEVEL_STATS                 Stat       = Editor->Stat;
    EDITOR__SPOTTER_STATE * SpotterS = &Editor->SpotterS;
    
    for( uint32 iSpotter = 0; iSpotter < SpotterS->nSpotter; iSpotter++ ) {
        EDITOR__SPOTTER Spotter = SpotterS->Spotter[ iSpotter ];
        
        vec4 Color = ToColor( 220, 50, 30 );
        rect Bound = RectCD( Spotter.Position, SPOTTER_DIM );
        
        UINT32_QUAD PixelBound = GetPixelBoundOfEntityForLevelBitmap( Bound, Stat.Bound, xPixel, yPixel );
        OutputCircleToLevelBitmap( PixelBound, Pixel, xPixel, yPixel, Color );
    }
}