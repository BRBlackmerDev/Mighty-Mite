
//----------
// CLOSEST POINT
//----------

// P : point
// E : Edge
// L : line
// R : ray

internal vec2
ClosestPointToEdge( vec2 C, vec2 A, vec2 B ) {
    vec2 AB = B - A;
    vec2 AC = C - A;
    
    flo32 t      = Clamp01( dot( AC, AB ) / dot( AB, AB ) );
    vec2  Result = A + AB * t;
    return Result;
}

internal boo32
IsInCapsule( vec2 Pos, CAPSULE2 Cap ) {
    boo32 Result = false;
    
    vec2  P = ClosestPointToEdge( Pos, Cap.P, Cap.Q );
    flo32 DistSq   = GetLengthSq( Pos - P );
    flo32 RadiusSq = Cap.Radius * Cap.Radius;
    if( DistSq <= RadiusSq ) {
        Result = true;
    }
    return Result;
}

internal vec2
ClosestPointToRectPerimeter( vec2 Pos, rect Bound ) {
    flo32 MaxDist = FLT_MAX;
    vec2  Result  = {};
    
    vec2 Point[  ] = {
        GetBL( Bound ),
        GetTL( Bound ),
        GetTR( Bound ),
        GetBR( Bound ),
        GetBL( Bound ),
    };
    
    for( uint32 iEdge = 0; iEdge < 4; iEdge++ ) {
        vec2  P    = ClosestPointToEdge( Pos, Point[ iEdge ], Point[ iEdge + 1 ] );
        flo32 Dist = GetLength( P - Pos );
        if( Dist < MaxDist ) {
            MaxDist = Dist;
            Result  = P;
        }
    }
    return Result;
}

internal boo32
DoIntersectEdgeEdge( vec2 A, vec2 B, vec2 C, vec2 D ) {
    boo32 Result = false;
    
    vec2 AB = B - A;
    vec2 CD = D - C;
    vec2 AC = C - A;
    
    vec2 N = GetNormal( GetPerp( CD ) );
    
    flo32 Denom = dot( N, AB );
    if( Denom != 0.0f ) {
        flo32 t = dot( N, C - A ) / Denom;
        
        if( ( t >= 0.0f ) && ( t <= 1.0f ) ) {
            vec2  P = A + AB * t;
            flo32 s = dot( P - C, CD ) / dot( CD, CD );
            
            if( ( s >= 0.0f ) && ( s <= 1.0f ) ) {
                Result = true;
            }
        }
    }
    return Result;
}

internal boo32
DoIntersectEdgeRect( vec2 A, vec2 B, rect R ) {
    vec2 BL = GetBL( R );
    vec2 TL = GetTL( R );
    vec2 TR = GetTR( R );
    vec2 BR = GetBR( R );
    
    boo32 Result = ( DoIntersectEdgeEdge( A, B, BL, TL ) )
        || ( DoIntersectEdgeEdge( A, B, TL, TR ) )
        || ( DoIntersectEdgeEdge( A, B, TR, BR ) )
        || ( DoIntersectEdgeEdge( A, B, BR, BL ) );
    
    return Result;
}

//----------
// COLLISION
//----------

struct COLLISION_RESULT {
    boo32 DoNextIter;
    boo32 HadCollision;
    vec2  Pos;
    vec2  dPos;
    vec2  Vel;
};

internal COLLISION_RESULT
CollisionResult( vec2 Pos, vec2 Vel, vec2 dPos ) {
    COLLISION_RESULT Result = {};
    Result.DoNextIter = true;
    Result.Pos  = Pos;
    Result.dPos = dPos;
    Result.Vel  = Vel;
    
    //if( GetLength( dPos ) < COLLISION_EPSILON_MIN_VALID_LENGTH ) {
    //Result.DoNextIter = false;
    //Result.dPos       = {};
    //}
    
    return Result;
}

internal rect
MinkSub( rect A, rect B ) {
    rect Result = {};
    Result.Left   = A.Left - B.Right;
    Result.Right  = A.Right - B.Left;
    Result.Bottom = A.Bottom - B.Top;
    Result.Top    = A.Top    - B.Bottom;
    return Result;
}

internal rect
MinkSubX( rect A, rect B ) {
    rect Result = A;
    Result.Left   = A.Left - B.Right;
    Result.Right  = A.Right - B.Left;
    return Result;
}

internal rect
MinkSubY( rect A, rect B ) {
    rect Result = A;
    Result.Bottom = A.Bottom - B.Top;
    Result.Top    = A.Top    - B.Bottom;
    return Result;
}

internal void
DoesPlayerIntersectLevelBound( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
    
    LEVEL_STATS Stat = AppState->Stat;
    
    
    if( ( !Level->iLinkLeft_IsActive ) || ( Level->iLinkLeft == -1 ) ) {
        vec2 P = GetTL( Stat.Bound );
        vec2 Q = GetBL( Stat.Bound );
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), P, Q );
    }
    if( ( !Level->iLinkRight_IsActive ) || ( Level->iLinkRight == -1 ) ) {
        vec2 P = GetBR( Stat.Bound );
        vec2 Q = GetTR( Stat.Bound );
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), P, Q );
    }
}

internal void
DoesIntersectLevelBound( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound = {} ) {
    LEVEL_STATE * LevelS = &AppState->LevelS;
    LEVEL       * Level  = LevelS->Level + LevelS->iCurrentLevel;
    
    LEVEL_STATS Stat = AppState->Stat;
    rect Bound  = MinkSub( Stat.Bound, CollBound );
    vec2 Margin = Vec2( TILE_WIDTH * 0.1f, 0.0f );
    { // Left
        vec2 P = GetTL( Bound ) - Margin;
        vec2 Q = GetBL( Bound ) - Margin;
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), P, Q );
    }
    { // Right
        vec2 P = GetBR( Bound ) + Margin;
        vec2 Q = GetTR( Bound ) + Margin;
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), P, Q );
    }
}

internal void
DoesIntersectCollisionLeft( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Left ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        
        vec2 P = Edge.P + Vec2( -CollBound.Right, -CollBound.Top );
        vec2 Q = Edge.Q + Vec2( -CollBound.Right, -CollBound.Bottom );
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), P, Q );
    }
}

internal void
DoesIntersectCollisionBottom( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Bottom ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        
        vec2 P = Edge.P + Vec2( -CollBound.Left,  -CollBound.Top );
        vec2 Q = Edge.Q + Vec2( -CollBound.Right, -CollBound.Top );
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), P, Q );
    }
}

internal void
DoesIntersectCollisionRight( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Right ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        
        vec2 P = Edge.P + Vec2( -CollBound.Left, -CollBound.Bottom );
        vec2 Q = Edge.Q + Vec2( -CollBound.Left, -CollBound.Top );
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), P, Q );
    }
}

internal void
DoesIntersectCollisionTop( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Top ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        
        vec2 P = Edge.P + Vec2( -CollBound.Right, -CollBound.Bottom );
        vec2 Q = Edge.Q + Vec2( -CollBound.Left,  -CollBound.Bottom );
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), P, Q );
    }
}

internal boo32
IsPosOnCollisionTop( APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    boo32 Result = false;
    
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Top ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        
        vec2 P = Edge.P + Vec2( 0.0f, -TILE_HEIGHT * 0.1f );
        vec2 Q = Edge.Q + Vec2( 0.0f,  TILE_HEIGHT * 0.1f );
        rect Bound = MinkSub( RectMM( P, Q ), CollBound );
        if( IsInBound( Pos, Bound ) ) {
            Result = true;
        }
    }
    
    return Result;
}

internal void
DoesIntersectCollisionAll( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    DoesIntersectCollisionLeft  ( BestIntersect, AppState, Coll, CollBound );
    DoesIntersectCollisionBottom( BestIntersect, AppState, Coll, CollBound );
    DoesIntersectCollisionRight ( BestIntersect, AppState, Coll, CollBound );
    DoesIntersectCollisionTop   ( BestIntersect, AppState, Coll, CollBound );
}

internal boo32
DoesEdgeIntersectCollisionLeft( APP_STATE * AppState, vec2 P, vec2 Q ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    UINT32_PAIR Type = Collision->Type[ CollisionType_Left ];
    
    boo32 Result = false;
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        if( DoIntersectEdgeEdge( Edge.P, Edge.Q, P, Q ) ) {
            Result = true;
        }
    }
    return Result;
}

internal boo32
DoesEdgeIntersectCollisionBottom( APP_STATE * AppState, vec2 P, vec2 Q ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    UINT32_PAIR Type = Collision->Type[ CollisionType_Bottom ];
    
    boo32 Result = false;
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        if( DoIntersectEdgeEdge( Edge.P, Edge.Q, P, Q ) ) {
            Result = true;
        }
    }
    return Result;
}

internal boo32
DoesEdgeIntersectCollisionRight( APP_STATE * AppState, vec2 P, vec2 Q ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    UINT32_PAIR Type = Collision->Type[ CollisionType_Right ];
    
    boo32 Result = false;
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        if( DoIntersectEdgeEdge( Edge.P, Edge.Q, P, Q ) ) {
            Result = true;
        }
    }
    return Result;
}

internal boo32
DoesEdgeIntersectCollisionTop( APP_STATE * AppState, vec2 P, vec2 Q ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    UINT32_PAIR Type = Collision->Type[ CollisionType_Top ];
    
    boo32 Result = false;
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        if( DoIntersectEdgeEdge( Edge.P, Edge.Q, P, Q ) ) {
            Result = true;
        }
    }
    return Result;
}

internal boo32
DoesEdgeIntersectCollisionAll( APP_STATE * AppState, vec2 P, vec2 Q ) {
    boo32 Result = DoesEdgeIntersectCollisionLeft  ( AppState, P, Q )
        || DoesEdgeIntersectCollisionBottom( AppState, P, Q )
        || DoesEdgeIntersectCollisionRight ( AppState, P, Q )
        || DoesEdgeIntersectCollisionTop   ( AppState, P, Q );
    
    return Result;
}

internal void
DoesIntersectCollisionLeft( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Left ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesRayIntersectCapsule( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), Capsule2( Edge.P, Edge.Q, Radius ) );
    }
}

internal void
DoesIntersectCollisionBottom( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Bottom ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesRayIntersectCapsule( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), Capsule2( Edge.P, Edge.Q, Radius ) );
    }
}

internal void
DoesIntersectCollisionRight( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Right ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesRayIntersectCapsule( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), Capsule2( Edge.P, Edge.Q, Radius ) );
    }
}

internal void
DoesIntersectCollisionTop( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Top ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesRayIntersectCapsule( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), Capsule2( Edge.P, Edge.Q, Radius ) );
    }
}

internal void
DoesIntersectCollisionAll( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, flo32 Radius ) {
    DoesIntersectCollisionLeft  ( BestIntersect, AppState, Coll, Radius );
    DoesIntersectCollisionBottom( BestIntersect, AppState, Coll, Radius );
    DoesIntersectCollisionRight ( BestIntersect, AppState, Coll, Radius );
    DoesIntersectCollisionTop   ( BestIntersect, AppState, Coll, Radius );
}

internal void
DoesPosPenetrateRect( PENETRATE2D * BestPenetrate, vec2 Pos, rect Bound, RECT_SIDE Side = RectSide_All ) {
    if( IsInBound( Pos, Bound ) ) {
        vec2  P    = {};
        flo32 Dist = 0.0f;
        
        switch( Side ) {
            case RectSide_All: {
                P    = ClosestPointToRectPerimeter( Pos, Bound );
                Dist = GetLength( P - Pos );
            } break;
            
            case RectSide_Left: {
                Dist = Pos.x - Bound.Left;
                P    = Vec2( Bound.Left, Pos.y );
            } break;
            
            case RectSide_Bottom: {
                Dist = Pos.y - Bound.Bottom;
                P    = Vec2( Pos.x, Bound.Bottom );
            } break;
            
            case RectSide_Right: {
                Dist = Bound.Right - Pos.x;
                P    = Vec2( Bound.Right, Pos.y );
            } break;
            
            case RectSide_Top: {
                Dist = Bound.Top - Pos.y;
                P    = Vec2( Pos.x, Bound.Top );
            } break;
        }
        
        if( Dist < BestPenetrate->Dist ) {
            BestPenetrate->IsValid = true;
            BestPenetrate->Dist = Dist;
            BestPenetrate->P    = P;
        }
    }
}

internal void
DoesPenetrateCollisionLeft( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Left ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesPosPenetrateRect( BestPenetrate, Pos, MinkSub( RectExtrema( Edge.P, Edge.Q ), CollBound ), RectSide_Left );
    }
}

internal void
DoesPenetrateCollisionBottom( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Bottom ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge  = Collision->Edge[ Type.m + iEdge ];
        DoesPosPenetrateRect( BestPenetrate, Pos, MinkSub( RectExtrema( Edge.P, Edge.Q ), CollBound ), RectSide_Bottom );
    }
}

internal void
DoesPenetrateCollisionRight( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Right ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesPosPenetrateRect( BestPenetrate, Pos, MinkSub( RectExtrema( Edge.P, Edge.Q ), CollBound ), RectSide_Right );
    }
}

internal void
DoesPenetrateCollisionTop( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Top ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesPosPenetrateRect( BestPenetrate, Pos, MinkSub( RectExtrema( Edge.P, Edge.Q ), CollBound ), RectSide_Top );
    }
}

internal void
DoesPenetrateCollisionAll( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, rect CollBound ) {
    DoesPenetrateCollisionLeft  ( BestPenetrate, AppState, Pos, CollBound );
    DoesPenetrateCollisionBottom( BestPenetrate, AppState, Pos, CollBound );
    DoesPenetrateCollisionRight ( BestPenetrate, AppState, Pos, CollBound );
    DoesPenetrateCollisionTop   ( BestPenetrate, AppState, Pos, CollBound );
}

internal void
DoesPosPenetrateCapsule( PENETRATE2D * BestPenetrate, vec2 Pos, CAPSULE2 Cap, CAPSULE_SIDE Side = CapsuleSide_All ) {
    if( IsInCapsule( Pos, Cap ) ) {
        flo32 AddDist = 0.0f;
        switch( Side ) {
            case CapsuleSide_Left: {
                if( Pos.x > Cap.P.x ) {
                    AddDist = fabsf( Pos.x - Cap.P.x );
                    Pos.x   = Cap.P.x - AddDist;
                }
            } break;
            
            case CapsuleSide_Bottom: {
                if( Pos.y > Cap.P.y ) {
                    AddDist = fabsf( Pos.y - Cap.P.y );
                    Pos.y   = Cap.P.y - AddDist;
                }
            } break;
            
            case CapsuleSide_Right: {
                if( Pos.x < Cap.P.x ) {
                    AddDist = fabsf( Pos.x - Cap.P.x );
                    Pos.x   = Cap.P.x + AddDist;
                }
            } break;
            
            case CapsuleSide_Top: {
                if( Pos.y < Cap.P.y ) {
                    AddDist = fabsf( Pos.y - Cap.P.y );
                    Pos.y   = Cap.P.y + AddDist;
                }
            } break;
        }
        
        vec2 P = ClosestPointToEdge( Pos, Cap.P, Cap.Q );
        vec2 V = Pos - P;
        flo32 Dist = GetLength( V ) + AddDist;
        
        if( Dist < BestPenetrate->Dist ) {
            BestPenetrate->IsValid = true;
            BestPenetrate->Dist    = Dist;
            BestPenetrate->P       = P + GetNormal( V ) * Cap.Radius;
        }
    }
}

internal void
DoesPenetrateCollisionLeft( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, vec2 Offset, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Left ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesPosPenetrateCapsule( BestPenetrate, Pos, Capsule2( Edge.P + Offset, Edge.Q + Offset, Radius ), CapsuleSide_Left );
    }
}

internal void
DoesPenetrateCollisionBottom( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, vec2 Offset, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Bottom ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesPosPenetrateCapsule( BestPenetrate, Pos, Capsule2( Edge.P + Offset, Edge.Q + Offset, Radius ), CapsuleSide_Bottom );
    }
}

internal void
DoesPenetrateCollisionRight( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, vec2 Offset, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Right ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesPosPenetrateCapsule( BestPenetrate, Pos, Capsule2( Edge.P + Offset, Edge.Q + Offset, Radius ), CapsuleSide_Right );
    }
}

internal void
DoesPenetrateCollisionTop( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, vec2 Offset, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    UINT32_PAIR Type = Collision->Type[ CollisionType_Top ];
    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
        vec4 Edge = Collision->Edge[ Type.m + iEdge ];
        DoesPosPenetrateCapsule( BestPenetrate, Pos, Capsule2( Edge.P + Offset, Edge.Q + Offset, Radius ), CapsuleSide_Top );
    }
}

internal void
DoesPenetrateCollisionAll( PENETRATE2D * BestPenetrate, APP_STATE * AppState, vec2 Pos, vec2 Offset, flo32 Radius ) {
    DoesPenetrateCollisionLeft  ( BestPenetrate, AppState, Pos, Offset, Radius );
    DoesPenetrateCollisionBottom( BestPenetrate, AppState, Pos, Offset, Radius );
    DoesPenetrateCollisionRight ( BestPenetrate, AppState, Pos, Offset, Radius );
    DoesPenetrateCollisionTop   ( BestPenetrate, AppState, Pos, Offset, Radius );
}

internal COLLISION_RESULT
FinalizeCollision( RAY2_INTERSECT BestIntersect, COLLISION_RESULT Coll ) {
    Coll.DoNextIter = true;
    
    if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
        Coll.Vel  -= dot( BestIntersect.N, Coll.Vel ) * BestIntersect.N;
        Coll.Pos   = BestIntersect.P + BestIntersect.N * COLLISION_EPSILON_OFFSET;
        Coll.dPos -= dot( BestIntersect.N, Coll.dPos ) * BestIntersect.N;
        Coll.dPos *= ( 1.0f - BestIntersect.t );
        if( GetLength( Coll.dPos ) <= COLLISION_EPSILON_MIN_VALID_LENGTH ) {
            Coll.dPos = {};
            Coll.DoNextIter = false;
        }
        
        Coll.HadCollision = true;
    } else {
        Coll.DoNextIter = false;
    }
    
    return Coll;
}

internal COLLISION_RESULT
FinalizeCollision( RAY2_INTERSECT BestIntersect, COLLISION_RESULT Coll, flo32 Reflect ) {
    Coll.DoNextIter = true;
    
    if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
        flo32 ReflectValue = 1.0f + Reflect;
        Coll.Vel  -= ( ReflectValue * dot( BestIntersect.N, Coll.Vel ) ) * BestIntersect.N;
        Coll.Pos   = BestIntersect.P + BestIntersect.N * COLLISION_EPSILON_OFFSET;
        Coll.dPos -= ( ReflectValue * dot( BestIntersect.N, Coll.dPos ) ) * BestIntersect.N;
        Coll.dPos *= ( 1.0f - BestIntersect.t );
        if( GetLength( Coll.dPos ) <= COLLISION_EPSILON_MIN_VALID_LENGTH ) {
            Coll.dPos = {};
            Coll.DoNextIter = false;
        }
        
        Coll.HadCollision = true;
    } else {
        Coll.DoNextIter = false;
    }
    
    return Coll;
}

internal COLLISION_RESULT
UpdateGrenadeCollision( APP_STATE * AppState, vec2 Pos, vec2 Vel, vec2 dPos, flo32 Radius ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    COLLISION_RESULT Coll = CollisionResult( Pos, Vel, dPos );
    for( uint32 Iter = 0; ( Coll.DoNextIter ) && ( Iter < COLLISION_MAX_ITERATION ); Iter++ ) {
        RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
        
        DoesIntersectCollisionAll( &BestIntersect, AppState, Coll, Radius );
        
        Coll = FinalizeCollision( BestIntersect, Coll );
    }
    Coll.Pos += Coll.dPos;
    
    return Coll;
    
    
#if 0    
    COLLISION_RESULT Result = {};
    Result.Position = P;
    Result.Velocity = V;
    
    flo32 length    = GetLength( dP );
    flo32 minLength = 0.0001f;
    if( length > minLength ) {
        for( uint32 iter = 0; iter < 4; iter++ ) {
            RAY2 ray = Ray2( P, dP );
            RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
            
            { // TERRAIN COLLISION
                for( uint32 iType = 0; iType < 4; iType++ ) {
                    UINT32_PAIR Type = Collision->Type[ iType ];
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        RAY2_INTERSECT Intersect = DoesIntersect( ray, Capsule2( e.P, e.Q, Radius ) );
                        if( ( Intersect.IsValid ) && ( Intersect.t < BestIntersect.t ) ) {
                            BestIntersect = Intersect;
                        }
                    }
                }
            }
            
            if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
                V -= dot( BestIntersect.N, V ) * BestIntersect.N;
                
                P   = BestIntersect.P + BestIntersect.N * COLLISION_EPSILON_OFFSET;
                dP -= dot( BestIntersect.N, dP ) * BestIntersect.N;
                dP *= ( 1.0f - BestIntersect.t );
                length = GetLength( dP );
                if( length <= minLength ) {
                    dP = {};
                    iter = 4;
                }
            } else {
                iter = 4;
            }
        }
        
        Result.Position = P + dP;
        Result.Velocity = V;
    }
    
    return Result;
#endif
    
}

internal void
DoesIntersectScaffold( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
    for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
        SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
        
        vec2 P = Scaffold.CollisionEdge.P + Vec2( -CollBound.Right, -CollBound.Bottom );
        vec2 Q = Scaffold.CollisionEdge.Q + Vec2( -CollBound.Left,  -CollBound.Bottom );
        
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), P, Q );
    }
}

internal void
DoesIntersectScaffold( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, flo32 Radius ) {
    SCAFFOLD_STATE * ScaffoldS = &AppState->ScaffoldS;
    for( uint32 iScaffold = 0; iScaffold < ScaffoldS->nScaffold; iScaffold++ ) {
        SCAFFOLD Scaffold = ScaffoldS->Scaffold[ iScaffold ];
        
        vec2 P = Scaffold.CollisionEdge.P;
        vec2 Q = Scaffold.CollisionEdge.Q;
        
        DoesRayIntersectCapsule( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), Capsule2( P, Q, Radius ) );
    }
}

internal void
DoesIntersectSeesaw( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll ) {
    SEESAW_STATE * SeesawS = &AppState->SeesawS;
    for( uint32 iSeesaw = 0; iSeesaw < SeesawS->nSeesaw; iSeesaw++ ) {
        SEESAW Seesaw = SeesawS->Seesaw[ iSeesaw ];
        
        rect R = ( Seesaw.BlockAIsUp ) ? Seesaw.BlockA : Seesaw.BlockB;
        
        vec4 EdgeA = Vec4( GetBL( R ), GetTL( R ) );
        vec4 EdgeB = Vec4( GetTR( R ), GetBR( R ) );
        
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), EdgeA.P, EdgeA.Q );
        DoesRayIntersectEdge( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), EdgeB.P, EdgeB.Q );
    }
}

internal void
DoesIntersectPushBlock( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
        
        rect R = MinkSub( RectCD( PushBlock.Position, PushBlock.Dim ), CollBound );
        DoesRayIntersectRect( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), R );
    }
}

internal void
DoesIntersectPushBlock( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, flo32 Radius ) {
    PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
        
        rect R = RectCD( PushBlock.Position, PushBlock.Dim );
        DoesRayIntersectRoundedRect( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), R, Radius );
    }
}

internal void
DoesIntersectBreakBlock( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
        
        rect R = MinkSub( RectCD( BreakBlock.Position, BreakBlock.Dim ), CollBound );
        DoesRayIntersectRect( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), R );
    }
}

internal void
DoesIntersectBreakBlock( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, flo32 Radius ) {
    BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
        
        rect R = RectCD( BreakBlock.Position, BreakBlock.Dim );
        DoesRayIntersectRoundedRect( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), R, Radius );
    }
}

internal void
DoesIntersectCollapsePlatform( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &AppState->CollapsePlatformS;
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ];
        
        if( CollapsePlatform.Collision_IsActive ) {
            rect R = CollapsePlatform.Bound;
            R.Left   -= CollBound.Right;
            R.Right  -= CollBound.Left;
            R.Bottom -= CollBound.Top;
            R.Top    -= CollBound.Bottom;
            
            DoesRayIntersectRect( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), R );
        }
    }
}

internal void
DoesIntersectCollapsePlatform( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, flo32 Radius ) {
    COLLAPSE_PLATFORM_STATE * CollapsePlatformS = &AppState->CollapsePlatformS;
    for( uint32 iCollapsePlatform = 0; iCollapsePlatform < CollapsePlatformS->nCollapsePlatform; iCollapsePlatform++ ) {
        COLLAPSE_PLATFORM CollapsePlatform = CollapsePlatformS->CollapsePlatform[ iCollapsePlatform ];
        
        if( CollapsePlatform.Collision_IsActive ) {
            DoesRayIntersectRoundedRect( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), CollapsePlatform.Bound, Radius );
        }
    }
}

internal boo32
DoesEdgeIntersectPuncher( APP_STATE * AppState, vec2 P, vec2 Q, uint32 Skip_iPuncher ) {
    PUNCHER_STATE * PuncherS = &AppState->PuncherS;
    
    boo32 Result = false;
    for( uint32 iPuncher = 0; iPuncher < PuncherS->nPuncher; iPuncher++ ) {
        PUNCHER Puncher = PuncherS->Puncher[ iPuncher ];
        if( iPuncher != Skip_iPuncher ) {
            rect R = RectBCD( Puncher.Position, PUNCHER_DIM );
            if( DoIntersectEdgeRect( P, Q, R ) ) {
                Result = true;
            }
        }
    }
    return Result;
}

internal void
DoesPushBlockIntersectPushBlock( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound, uint32 bPushBlock ) {
    PUSH_BLOCK_STATE * PushBlockS = &AppState->PushBlockS;
    for( uint32 iPushBlock = 0; iPushBlock < PushBlockS->nPushBlock; iPushBlock++ ) {
        if( iPushBlock != bPushBlock ) {
            PUSH_BLOCK PushBlock = PushBlockS->PushBlock[ iPushBlock ];
            
            rect R = RectCD( PushBlock.Position, PushBlock.Dim );
            R.Left   -= CollBound.Right;
            R.Bottom -= CollBound.Top;
            R.Right  -= CollBound.Left;
            R.Top    -= CollBound.Bottom;
            
            DoesRayIntersectRect( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), R );
        }
    }
}

internal void
DoesBreakBlockIntersectBreakBlock( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound, uint32 bBreakBlock ) {
    BREAK_BLOCK_STATE * BreakBlockS = &AppState->BreakBlockS;
    for( uint32 iBreakBlock = 0; iBreakBlock < BreakBlockS->nBreakBlock; iBreakBlock++ ) {
        if( iBreakBlock != bBreakBlock ) {
            BREAK_BLOCK BreakBlock = BreakBlockS->BreakBlock[ iBreakBlock ];
            
            rect R = MinkSub( RectCD( BreakBlock.Position, BreakBlock.Dim ), CollBound );
            DoesRayIntersectRect( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), R );
        }
    }
}

internal void
DoesIntersectExerciseMiniBoss03( RAY2_INTERSECT * BestIntersect, APP_STATE * AppState, COLLISION_RESULT Coll, rect CollBound ) {
    EXERCISE_MINIBOSS03 * Boss = &AppState->ExerciseMiniBoss03;
    if( Boss->IsActive ) {
        // TODO: What should we do about collision with other boss modes?
        boo32 CanIntersect = ( Boss->Mode == ExerciseMiniBoss03Mode_Squash );
        if( CanIntersect ) {
            rect R = MinkSub( RectBCD( Boss->Position, Boss->Dim ), CollBound );
            DoesRayIntersectRect( BestIntersect, Ray2( Coll.Pos, Coll.dPos ), R );
        }
    }
}

//----------
// convert screen and World coordinates
//----------

internal vec2
toScreenCoord( mat4 Camera_transform, vec3 point ) {
    vec4 Position = Camera_transform * Vec4( point, 1.0f );
    vec2 Result   = ( ( Vec2( Position.x, Position.y ) / Position.w ) * 0.5f + Vec2( 0.5f, 0.5f ) );
    return Result;
}

internal vec3
screenCoordToWorld( vec2 pos, vec2 Dim, mat4 Camera_transform, vec3 Camera_Position ) {
    flo32 tX = ( pos.x / Dim.x ) * 2.0f - 1.0f;
    flo32 tY = ( pos.y / Dim.y ) * 2.0f - 1.0f;
    
    mat4 inverse_transform = mat4_inverse( Camera_transform );
    vec4 nearPoint         = inverse_transform * Vec4( tX, tY, -1.0f, 1.0f );
    nearPoint.xyz    /= nearPoint.w;
    
    vec3 Result = GetNormal( nearPoint.xyz - Camera_Position );
    return Result;
}

//----------
// Color functions
//----------

internal uint32
ToU32Color( uint8 r, uint8 g, uint8 b, uint8 a = 255 ) {
    uint32 Result = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b << 0 );
    return Result;
}

internal uint32
ToU32Color( vec4 V4Color ) {
    uint8 r = ( uint8 )( V4Color.r * 255 );
    uint8 g = ( uint8 )( V4Color.g * 255 );
    uint8 b = ( uint8 )( V4Color.b * 255 );
    uint8 a = ( uint8 )( V4Color.a * 255 );
    uint32 Result = ToU32Color( r, g, b, a );
    return Result;
}

internal vec4
ToColor( uint8 r, uint8 g, uint8 b ) {
    vec4 Result = {};
    Result.r = ( flo32 )r / 255.0f;
    Result.g = ( flo32 )g / 255.0f;
    Result.b = ( flo32 )b / 255.0f;
    Result.a = 1.0f;
    return Result;
}

//----------
// UINT32_PAIR functions
//----------

internal UINT32_PAIR
UInt32Pair( uint32 x, uint32 y ) {
    UINT32_PAIR Result = { x, y };
    return Result;
}

internal boo32
operator==( UINT32_PAIR a, UINT32_PAIR b ) {
    boo32 Result = ( a.x == b.x ) && ( a.y == b.y );
    return Result;
}

internal boo32
operator!=( UINT32_PAIR a, UINT32_PAIR b ) {
    boo32 Result = !( a == b );
    return Result;
}

//----------
// UINT32_TRI functions
//----------

internal UINT32_TRI
UInt32Tri( uint32 x, uint32 y, uint32 z ) {
    UINT32_TRI Result = { x, y, z };
    return Result;
}

internal boo32
operator==( UINT32_TRI a, UINT32_TRI b ) {
    boo32 Result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z );
    return Result;
}

internal boo32
operator!=( UINT32_TRI a, UINT32_TRI b ) {
    boo32 Result = !( a == b );
    return Result;
}

//----------
// INT32_PAIR functions
//----------

internal INT32_PAIR
Int32Pair( int32 x, int32 y ) {
    INT32_PAIR Result = { x, y };
    return Result;
}

//----------
// INT32_TRI functions
//----------

internal INT32_TRI
Int32Tri( int32 x, int32 y, int32 z ) {
    INT32_TRI Result = { x, y, z };
    return Result;
}

internal boo32
operator==( INT32_TRI a, INT32_TRI b ) {
    boo32 Result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z );
    return Result;
}

internal boo32
operator!=( INT32_TRI a, INT32_TRI b ) {
    boo32 Result = !( a == b );
    return Result;
}

internal UINT32_TRI
ToUInt32Tri( INT32_TRI Tri ) {
    Assert( Tri.x > -1 );
    Assert( Tri.y > -1 );
    Assert( Tri.z > -1 );
    
    UINT32_TRI Result = { ( uint32 )Tri.x, ( uint32 )Tri.y, ( uint32 )Tri.z };
    return Result;
}

//----------
// GEN TEXTURE FUNCTIONS
//----------

internal void
GenTexture_ExitTypeS_Walk_EnterLeft( DIRECTX_12_RENDERER * Renderer, MEMORY * TempMemory, TEXTURE_ID TextureID ) {
    uint32 Width  = 256;
    uint32 Height = 256;
    uint32 nTexel = Width * Height;
    uint32 * Data = _pushArray_clear( TempMemory, uint32, nTexel );
    
    uint8 hi = 40;
    uint8 lo = 0;
    
    for( uint32 iTexel = 0; iTexel < nTexel; iTexel++ ) {
        uint32 iRow = ( iTexel / Width );
        uint32 iCol = ( iTexel % Width );
        
        flo32 t = ( flo32 )iCol / ( flo32 )Width;
        uint8 c = ( uint8 )lerp( ( flo32 )lo, t, ( flo32 )hi );
        
        uint32 Color = ToU32Color( c, c, c );
        Data[ iTexel ] = Color;
    }
    
    UploadTexture( Renderer, TextureID, Width, Height, Data );
    _popArray( TempMemory, uint32, nTexel );
}

//----------
// gen Model functions
//----------

internal MODEL_DATA
GenModel_PlayerWallSlide( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    VERTEX1 vertex[ 4 ] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        0.25f, -1.0f,  0.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    };
    uint32 nVertex = _ArrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 2,
        3, 2, 1,
    };
    uint32 nIndex = _ArrayCount( index );
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal MODEL_DATA
GenModel_Spikes( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    VERTEX1 vertex[] = {
        0.0f,  0.0f, 0.0f,  0.0f, 0.0f,
        0.5f,  0.0f, 0.0f,  1.0f, 0.0f,
        0.25f, 0.5f, 0.0f,  0.0f, 1.0f,
        0.5f,  0.0f, 0.0f,  0.0f, 0.0f,
        1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
        0.75f, 0.5f, 0.0f,  0.0f, 1.0f,
    };
    uint32 nVertex = _ArrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 2,
        3, 4, 5,
    };
    uint32 nIndex = _ArrayCount( index );
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

//----------
// SAVE/LOAD ENTITY
//----------

struct ENTITY_FILE_HEADER {
    char EntityTag[ 8 ];
    char Version  [ 4 ];
};

internal void
WriteEntityFileHeader( MEMORY * output, char * FileTag, uint32 Version ) {
    writeSegment( output, FileTag );
    
    char str[ 4 + 1 ] = {};
    sprintf( str, "%04u", Version );
    writeSegment( output, str );
}

internal boo32
OutputFile( PLATFORM * Platform, MEMORY * output, char * SaveDir, char * FileName, char * FileTag ) {
    boo32 IsValid = Platform->WriteFile( SaveDir, FileName, FileTag, output->base, ( uint32 )output->used );
    if( !IsValid ) {
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Error occurred when attempting to save File: %s.%s", FileName, FileTag );
        CONSOLE_STRING( str );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
    
    return IsValid;
}

internal uint32
VerifyEntityHeaderAndGetVersion( uint8 ** ptr, char * FileTag ) {
    ENTITY_FILE_HEADER * header = _addr( *ptr, ENTITY_FILE_HEADER );
    Assert( MatchSegment( header->EntityTag, FileTag, 8 ) );
    
    char str[ 4 + 1 ] = {};
    memcpy( str, header->Version, 4 );
    
    uint32 Result = strtoul( str, 0, 0 );
    return Result;
}

#if 0    
COLLISION_RESULT Result = {};
Result.Position = P;
Result.Velocity = V;

vec2 offset = Vec2( Dim.x * 0.5f, Dim.y );

flo32 length    = GetLength( dP );
flo32 minLength = 0.0001f;
if( length > minLength ) {
    for( uint32 iter = 0; iter < 4; iter++ ) {
        RAY2 ray = Ray2( P, dP );
        RAY2_INTERSECT BestIntersect = Ray2IntersectInit();
        
        vec2 offsetA[ 4 ] = {
            -offset,
            Vec2( offset.x, -offset.y ),
            Vec2( offset.x, 0.0f ),
            Vec2( -offset.x, 0.0f ),
        };
        
        vec2 offsetB[ 4 ] = {
            Vec2( -offset.x, 0.0f ),
            -offset,
            Vec2( offset.x, -offset.y ),
            Vec2( offset.x, 0.0f ),
        };
        
        { // TERRAIN COLLISION
            for( uint32 iType = 0; iType < 4; iType++ ) {
                UINT32_PAIR Type = Collision->Type[ iType ];
                
                vec2 oA = offsetA[ iType ];
                vec2 oB = offsetB[ iType ];
                
                vec4 * Edge = Collision->Edge + Type.m;
                for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                    vec4 e = Edge[ iEdge ];
                    
                    vec2 A = e.xy + oA;
                    vec2 B = e.zw + oB;
                    RAY2_INTERSECT Intersect = DoesIntersect( ray, A, B );
                    if( ( Intersect.IsValid ) && ( Intersect.t < BestIntersect.t ) ) {
                        BestIntersect = Intersect;
                    }
                }
            }
        }
        
        { // SCAFFOLD
            if( !Player->Scaffold_SkipCollision ) {
                RAY2_INTERSECT Intersect = DoesIntersectScaffold( AppState, P, dP );
                if( ( Intersect.IsValid ) && ( Intersect.t < BestIntersect.t ) ) {
                    BestIntersect = Intersect;
                }
            }
        }
        
        if( ( BestIntersect.IsValid ) && ( BestIntersect.t <= 1.0f ) ) {
            V -= dot( BestIntersect.N, V ) * BestIntersect.N;
            
            P   = BestIntersect.P + BestIntersect.N * COLLISION_EPSILON_OFFSET;
            dP -= dot( BestIntersect.N, dP ) * BestIntersect.N;
            dP *= ( 1.0f - BestIntersect.t );
            length = GetLength( dP );
            if( length <= minLength ) {
                dP = {};
                iter = 4;
            }
            
            Result.HadCollision = true;
        } else {
            iter = 4;
        }
    }
    
    Result.Position = P + dP;
    Result.Velocity = V;
}
#endif


#if 0
internal COLLISION_RESULT
UpdateCollision( APP_STATE * AppState, vec2 P, vec2 V, vec2 dP, vec2 Dim ) {
    COLLISION_STATE * Collision = &AppState->Collision;
    
    COLLISION_RESULT result = {};
    result.Position = P;
    result.Velocity = V;
    
    vec2 offset = Vec2( Dim.x * 0.5f, Dim.y );
    
    flo32 length    = GetLength( dP );
    flo32 minLength = 0.0001f;
    if( length > minLength ) {
        for( uint32 iter = 0; iter < COLLISION_MAX_ITERATION; iter++ ) {
            RAY2 ray = Ray2( P, dP );
            RAY2_INTERSECT bestIntersect = Ray2IntersectInit();
            
            vec2 offsetA[ 4 ] = {
                -offset,
                Vec2( offset.x, -offset.y ),
                Vec2( offset.x, 0.0f ),
                Vec2( -offset.x, 0.0f ),
            };
            
            vec2 offsetB[ 4 ] = {
                Vec2( -offset.x, 0.0f ),
                -offset,
                Vec2( offset.x, -offset.y ),
                Vec2( offset.x, 0.0f ),
            };
            
            { // TERRAIN COLLISION
                for( uint32 iType = 0; iType < 4; iType++ ) {
                    UINT32_PAIR Type = Collision->Type[ iType ];
                    
                    vec2 oA = offsetA[ iType ];
                    vec2 oB = offsetB[ iType ];
                    
                    vec4 * Edge = Collision->Edge + Type.m;
                    for( uint32 iEdge = 0; iEdge < Type.n; iEdge++ ) {
                        vec4 e = Edge[ iEdge ];
                        
                        vec2 A = e.xy + oA;
                        vec2 B = e.zw + oB;
                        RAY2_INTERSECT intersect = DoesIntersect( ray, A, B );
                        if( ( intersect.IsValid ) && ( intersect.t < bestIntersect.t ) ) {
                            bestIntersect = intersect;
                        }
                    }
                }
            }
            
            if( ( bestIntersect.IsValid ) && ( bestIntersect.t <= 1.0f ) ) {
                V -= dot( bestIntersect.N, V ) * bestIntersect.N;
                
                P   = bestIntersect.P + bestIntersect.N * COLLISION_EPSILON_OFFSET;
                dP -= dot( bestIntersect.N, dP ) * bestIntersect.N;
                dP *= ( 1.0f - bestIntersect.t );
                length = GetLength( dP );
                if( length <= minLength ) {
                    dP = {};
                    iter = 4;
                }
            } else {
                iter = 4;
            }
        }
        
        result.Position = P + dP;
        result.Velocity = V;
    }
    
    return result;
}
#endif
