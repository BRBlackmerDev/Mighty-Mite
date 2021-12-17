
//----------
// vec2 check functions
//----------

internal boo32
isDegenerate( vec2 V ) {
    flo32 dotVV  = dot( V, V );
    boo32 Result = ( dotVV <= DOT__DEGENERATE_EPSILON );
    return Result;
}

internal boo32
isOrthogonalTo( vec2 U, vec2 V ) {
    flo32 dotUV = dot( U, V );
    dotUV = fabsf( dotUV );
    boo32 Result = ( dotUV <= DOT__ORTHOGONAL_EPSILON );
    return Result;
}

internal boo32
isCollinearTo( vec2 U, vec2 V ) {
    flo32 dotUV = dot( U, V );
    boo32 Result = ( fabsf( dotUV ) >= ( 1.0f - DOT__COLLINEAR_EPSILON ) );
    return Result;
}

internal boo32
isOppositeTo( vec2 U, vec2 V ) {
    flo32 dotUV = dot( U, V );
    boo32 Result = ( dotUV < -DOT__ORTHOGONAL_EPSILON );
    return Result;
}

//----------
// LINE2 functions
//----------

internal LINE2
Line2( vec2 Origin, vec2 Vector ) {
    LINE2 Result = {};
    Result.Origin = Origin;
    Result.Vector = Vector;
    return Result;
}

internal LINE2_INTERSECT
DoesIntersect( LINE2 A, LINE2 B ) {
    LINE2_INTERSECT Result = {};
    
    vec2 N = GetNormal( GetPerp( B.Vector ) );
    
    flo32 Denom = dot( N, A.Vector );
    if( Denom != 0.0f ) {
        Result.IsValid = true;
        Result.tA      = dot( N, B.Origin - A.Origin ) / Denom;
        Result.tB      = 0.0f; // TODO: Solve for this!
        Result.P       = A.Origin + A.Vector * Result.tA;
    }
    
    return Result;
}

//----------
// ray2 functions
//----------

internal RAY2
Ray2( vec2 Origin, vec2 Vector ) {
    RAY2 Result = {};
    Result.Origin = Origin;
    Result.Vector = Vector;
    return Result;
}

internal RAY2_INTERSECT
Ray2IntersectInit() {
    RAY2_INTERSECT Result = {};
    Result.t = FLT_MAX;
    return Result;
}

internal void
DoesRayIntersectEdge( RAY2_INTERSECT * BestIntersect, RAY2 Ray, vec2 P, vec2 Q ) {
    RAY2_INTERSECT Result = {};
    
    vec2 V = Q - P;
    vec2 W = P - Ray.Origin;
    Result.N = GetNormal( GetPerp( V ) );
    
    Result.Denom = dot( Result.N, Ray.Vector );
    if( Result.Denom <= -DOT__COLLINEAR_EPSILON ) {
        Result.tMin = ( -RAY_INTERSECT__VALID_NEG_DIST_EPSILON / GetLength( Ray.Vector ) );
        Result.t = dot( Result.N, W ) / Result.Denom;
        if( Result.t >= Result.tMin ) {
            Result.P = Ray.Origin + Ray.Vector * Result.t;
            Result.s = dot( Result.P - P, V ) / dot( V, V );
            
            if( ( Result.s >= 0.0f ) && ( Result.s <= 1.0f ) ) {
                Result.IsValid = true;
                
                if( Result.t < BestIntersect->t ) {
                    *BestIntersect = Result;
                }
            }
        }
    }
}

internal RAY2_INTERSECT
DoesIntersect( RAY2 ray, vec2 P, vec2 Q ) {
    RAY2_INTERSECT Result = {};
    
    vec2 V = Q - P;
    vec2 W = P - ray.Origin;
    Result.N = GetNormal( GetPerp( V ) );
    
    Result.Denom = dot( Result.N, ray.Vector );
    if( Result.Denom <= -DOT__COLLINEAR_EPSILON ) {
        Result.tMin = ( -RAY_INTERSECT__VALID_NEG_DIST_EPSILON / GetLength( ray.Vector ) );
        Result.t = dot( Result.N, W ) / Result.Denom;
        if( Result.t >= Result.tMin ) {
            Result.P = ray.Origin + ray.Vector * Result.t;
            Result.s = dot( Result.P - P, V ) / dot( V, V );
            
            if( ( Result.s >= 0.0f ) && ( Result.s <= 1.0f ) ) {
                Result.IsValid = true;
            }
        }
    }
    return Result;
}

internal void
DoesRayIntersectRect( RAY2_INTERSECT * BestIntersect, RAY2 Ray, rect Bound ) {
    vec2 Point[ 4 + 1 ] = {};
    Point[ 0 ] = Vec2( Bound.Left,  Bound.Bottom );
    Point[ 1 ] = Vec2( Bound.Left,  Bound.Top    );
    Point[ 2 ] = Vec2( Bound.Right, Bound.Top    );
    Point[ 3 ] = Vec2( Bound.Right, Bound.Bottom );
    Point[ 4 ] = Vec2( Bound.Left,  Bound.Bottom );
    
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec2 P = Point[ iter     ];
        vec2 Q = Point[ iter + 1 ];
        DoesRayIntersectEdge( BestIntersect, Ray, P, Q );
    }
}

internal RAY2_INTERSECT
DoesIntersect( RAY2 ray, orect Bound ) {
    RAY2_INTERSECT Result = Ray2IntersectInit();
    
    vec2 xAxis = Bound.xAxis * Bound.HalfDim.x;
    vec2 yAxis = Bound.yAxis * Bound.HalfDim.y;
    
    vec2 point[ 4 + 1 ] = {};
    point[ 0 ] = Bound.Center - xAxis - yAxis;
    point[ 1 ] = Bound.Center - xAxis + yAxis;
    point[ 2 ] = Bound.Center + xAxis + yAxis;
    point[ 3 ] = Bound.Center + xAxis - yAxis;
    point[ 4 ] = point[ 0 ];
    
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec2 P = point[ iter     ];
        vec2 Q = point[ iter + 1 ];
        RAY2_INTERSECT Intersect = DoesIntersect( ray, P, Q );
        if( ( Intersect.IsValid ) && ( Intersect.t < Result.t ) ) {
            Result = Intersect;
        }
    }
    
    return Result;
}

internal RAY2_INTERSECT
DoesIntersect( RAY2 ray, circ C ) {
    RAY2_INTERSECT Result = {};
    
    vec2 U = GetNormal( ray.Vector );
    vec2 V = ray.Origin - C.Center;
    
    flo32 dotUV = dot( U, V );
    
    flo32 discriminant = ( dotUV * dotUV ) + ( C.Radius * C.Radius ) - dot( V, V );
    Result.Denom = discriminant;
    if( discriminant >= 0.0f ) {
        flo32 tMin = -RAY_INTERSECT__VALID_NEG_DIST_EPSILON;
        Result.tMin = tMin;
        flo32 t = -dotUV - sqrtf( discriminant );
        Result.t = t / GetLength( ray.Vector );
        //NOTE: only checks if the near side of the sphere is valid
        //NOTE: t = -dotUV + sqrtf( disc ) would check the far side of the sphere
        if( t >= tMin ) {
            Result.IsValid = true;
            Result.P       = ray.Origin + ray.Vector * Result.t;
            Result.N       = GetNormal( Result.P - C.Center );
        }
    }
    
    return Result;
}

internal void
DoesRayIntersectCapsule( RAY2_INTERSECT * BestIntersect, RAY2 Ray, CAPSULE2 Cap ) {
    vec2 xAxis = GetNormal( Cap.Q - Cap.P );
    vec2 yAxis = GetPerp( xAxis );
    yAxis *= Cap.Radius;
    
    RAY2_INTERSECT Intersect[ 2 ] = {};
    Intersect[ 0 ] = DoesIntersect( Ray, Cap.P + yAxis, Cap.Q + yAxis );
    Intersect[ 1 ] = DoesIntersect( Ray, Circ( Cap.P, Cap.Radius ) );
    
    // TODO: NOTE: This eliminates duplicate and unnecessary calculation. Probably temporary. This is only possible because Collision and obstacles are very strict about consistent direction and closure.
    //Intersect[ 0 ] = DoesIntersect( Ray, LineSeg( Cap.P + yAxis, Cap.Q + yAxis ) );
    //Intersect[ 1 ] = DoesIntersect( Ray, LineSeg( Cap.Q - yAxis, Cap.P - yAxis ) );
    //Intersect[ 2 ] = DoesIntersect( Ray, Circle( Cap.P, Cap.Radius ) );
    //Intersect[ 3 ] = DoesIntersect( Ray, Circle( Cap.Q, Cap.Radius ) );
    
    for( uint32 Iter = 0; Iter < 2; Iter++ ) {
        if( ( Intersect[ Iter ].IsValid ) && ( Intersect[ Iter ].t < BestIntersect->t ) ) {
            *BestIntersect = Intersect[ Iter ];
        }
    }
}

internal void
DoesRayIntersectRoundedRect( RAY2_INTERSECT * BestIntersect, RAY2 Ray, rect Bound, flo32 Radius ) {
    vec2 Point[ 4 + 1 ] = {};
    Point[ 0 ] = Vec2( Bound.Left,  Bound.Bottom );
    Point[ 1 ] = Vec2( Bound.Left,  Bound.Top    );
    Point[ 2 ] = Vec2( Bound.Right, Bound.Top    );
    Point[ 3 ] = Vec2( Bound.Right, Bound.Bottom );
    Point[ 4 ] = Vec2( Bound.Left,  Bound.Bottom );
    
    for( uint32 Iter = 0; Iter < 4; Iter++ ) {
        vec2 P = Point[ Iter     ];
        vec2 Q = Point[ Iter + 1 ];
        DoesRayIntersectCapsule( BestIntersect, Ray, Capsule2( P, Q, Radius ) );
    }
}