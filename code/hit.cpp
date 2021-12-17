
internal void
UpdatePlayerHitScan( APP_STATE * AppState ) {
    PLAYER_STATE * Player = &AppState->Player;
    
    if( Player->Fire_doHitScan ) {
        Player->Fire_doHitScan = false;
        
        uint32  MaxHit           = 3;
        uint32    nHit           = 1;
        BULLET_HIT Hit[ 12 + 1 ] = {};
        for( uint32 iter = 0; iter < ( 12 + 1 ); iter++ ) {
            Hit[ iter ].bHit = Ray2IntersectInit();
            Hit[ iter ].iHit = -1;
        }
        
        RAY2 ray = Ray2( Player->Fire_pos, Player->Fire_dir );
        
        { // Collision
            COLLISION_STATE * Collision = &AppState->Collision;
            
            for( uint32 iType = 0; iType < 4; iType++ ) {
                UINT32_PAIR t = Collision->Type[ iType ];
                
                vec4 * Edge = Collision->Edge + t.m;
                for( uint32 iEdge = 0; iEdge < t.n; iEdge++ ) {
                    vec4 e = Edge[ iEdge ];
                    
                    RAY2_INTERSECT Intersect = DoesIntersect( ray, e.xy, e.zw );
                    if( ( Intersect.IsValid ) && ( Intersect.t <= Hit[ nHit - 1 ].bHit.t ) ) {
                        int32 at = nHit - 1;
                        while( ( at > 0 ) && ( Hit[ at - 1 ].bHit.t > Intersect.t ) ) {
                            Hit[ at ] = Hit[ at - 1 ];
                            at--;
                        }
                        
                        Hit[ at ] = BulletHit( HitType_Wall, iEdge, Intersect );
                    }
                }
            }
        }
        
        Player->Fire_lo = RandomF32InRange( 3.0f,  5.0f );
        Player->Fire_hi = RandomF32InRange( 9.0f, 12.0f );
        
        if( Hit[ 0 ].bHit.IsValid  ) {
            Player->Fire_lo = MinValue( Player->Fire_lo, Hit[ nHit - 1 ].bHit.t );
            Player->Fire_hi = MinValue( Player->Fire_hi, Hit[ nHit - 1 ].bHit.t );
            
            for( uint32 iHit = 0; iHit < nHit; iHit++ ) {
                BULLET_HIT H = Hit[ iHit ];
                switch( H.Type ) {
                    case HitType_Wall: {
                        addWallHit( &AppState->ParticleS, ray.Vector, H.bHit );
                    } break;
                }
            }
        }
    }
}