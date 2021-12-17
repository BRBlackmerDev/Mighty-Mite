

//----------
// DRAW
//----------

internal void
DrawModel( RENDER_OBJECT_LIST * objectList, MODEL_ID ModelID, TEXTURE_ID TextureID, mat4 transform, vec4 modColor ) {
    if( objectList->nObjects < objectList->maxObjects ) {
        RENDER_OBJECT object = {};
        object.ModelID   = ModelID;
        object.TextureID = TextureID;
        object.transform = transform;
        object.modColor  = modColor;
        
        objectList->object[ objectList->nObjects++ ] = object;
    } else {
        InvalidCodePath;
        
        GlobalVar_DebugSystem.ErrorOccurred = true;
        char str[ 512 ] = {};
        sprintf( str, "ERROR! Render Object List ran out of objects for rendering Models! Max Count = %u", objectList->maxObjects );
        CONSOLE_STRING( str );
    }
}

internal void
DrawModel( RENDER_PASS * Pass, MODEL_ID ModelID, TEXTURE_ID TextureID, vec3 Position, vec4 modColor ) {
    mat4 transform = mat4_translate( Position );
    
    DrawModel( &Pass->triObjectList, ModelID, TextureID, transform, modColor );
}

internal void
DrawModel( RENDER_PASS * Pass, MODEL_ID ModelID, TEXTURE_ID TextureID, vec2 Position, vec2 scale, vec4 modColor ) {
    mat4 transform = mat4_translate( Position, 0.0f ) * mat4_scale( scale, 1.0f );
    
    DrawModel( &Pass->triObjectList, ModelID, TextureID, transform, modColor );
}

internal void
DrawModel( RENDER_PASS * Pass, MODEL_ID ModelID, TEXTURE_ID TextureID, vec2 Position, vec2 scale, flo32 Radians, vec4 modColor ) {
    mat4 transform = mat4_translate( Position, 0.0f ) * mat4_rotate( Vec3( 0.0f, 0.0f, 1.0f ), Radians ) * mat4_scale( scale, 1.0f );
    
    DrawModel( &Pass->triObjectList, ModelID, TextureID, transform, modColor );
}

internal void
DrawQuad( RENDER_PASS * Pass, TEXTURE_ID TextureID, vec2 Position, vec2 xAxis, vec2 yAxis, vec4 modColor ) {
    mat4 transform = mat4_translate( Position, 0.0f ) * mat4_column( Vec3( xAxis, 0.0f ), Vec3( yAxis, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ) );
    DrawModel( &Pass->triObjectList, ModelID_rect, TextureID, transform, modColor );
}

internal void
DrawQuadOutline( RENDER_PASS * Pass, vec2 Position, vec2 xAxis, vec2 yAxis, vec4 modColor ) {
    mat4 transform = mat4_translate( Position, 0.0f ) * mat4_column( Vec3( xAxis, 0.0f ), Vec3( yAxis, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ) );
    DrawModel( &Pass->lineObjectList, ModelID_rectOutline, TextureID_WhiteTexture, transform, modColor );
}

internal void
DrawQuad( RENDER_PASS * Pass, TEXTURE_ID TextureID, vec2 Position, flo32 radians, vec2 scale, vec4 modColor ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = GetPerp( xAxis );
    
    xAxis *= scale.x;
    yAxis *= scale.y;
    
    DrawQuad( Pass, TextureID, Position, xAxis, yAxis, modColor );
}

internal void
DrawQuad( RENDER_PASS * Pass, TEXTURE_ID TextureID, vec2 Position, vec2 scale, vec4 modColor ) {
    mat4 transform = mat4_translate( Position, 0.0f ) * mat4_scale( scale, 1.0f );
    DrawModel( &Pass->triObjectList, ModelID_rect, TextureID, transform, modColor );
}

internal void
DrawQuadOutline( RENDER_PASS * Pass, vec2 Position, vec2 scale, vec4 modColor ) {
    mat4 transform = mat4_translate( Position, 0.0f ) * mat4_scale( scale, 1.0f );
    DrawModel( &Pass->lineObjectList, ModelID_rectOutline, TextureID_WhiteTexture, transform, modColor );
}

internal void
DrawRect( RENDER_PASS * Pass, TEXTURE_ID TextureID, rect Bound, vec4 Color ) {
    vec2 Position = GetCenter ( Bound );
    vec2 scale    = getHalfDim( Bound );
    DrawQuad( Pass, TextureID, Position, scale, Color );
}

internal void
DrawRect( RENDER_PASS * Pass, rect Bound, vec4 Color ) {
    DrawRect( Pass, TextureID_WhiteTexture, Bound, Color );
}

internal void
DrawORect( RENDER_PASS * Pass, TEXTURE_ID TextureID, orect Bound, vec4 Color ) {
    vec2 xAxis = Bound.xAxis * Bound.HalfDim.x;
    vec2 yAxis = Bound.yAxis * Bound.HalfDim.y;
    DrawQuad( Pass, TextureID, Bound.Center, xAxis, yAxis, Color );
}

internal void
DrawORect( RENDER_PASS * Pass, orect Bound, vec4 Color ) {
    DrawORect( Pass, TextureID_WhiteTexture, Bound, Color );
}

internal void
DrawPoint( RENDER_PASS * Pass, vec2 Position, vec2 Dim, vec4 Color ) {
    rect Bound = RectCD( Position, Dim );
    DrawRect( Pass, TextureID_WhiteTexture, Bound, Color );
}

internal void
DrawRectOutline( RENDER_PASS * Pass, rect Bound, vec4 Color ) {
    vec2 Position = GetCenter ( Bound );
    vec2 scale    = getHalfDim( Bound );
    DrawQuadOutline( Pass, Position, scale, Color );
}

internal void
DrawORectOutline( RENDER_PASS * Pass, orect Bound, vec4 Color ) {
    vec2 xAxis = Bound.xAxis * Bound.HalfDim.x;
    vec2 yAxis = Bound.yAxis * Bound.HalfDim.y;
    DrawQuadOutline( Pass, Bound.Center, xAxis, yAxis, Color );
}

internal void
DrawLine( RENDER_PASS * Pass, vec2 A, vec2 B, vec4 Color ) {
    vec2 xAxis = B - A;
    vec2 yAxis = GetPerp( xAxis );
    
    mat4 transform = mat4_translate( A, 0.0f ) *  mat4_column( Vec3( xAxis, 0.0f ), Vec3( yAxis, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ) );
    DrawModel( &Pass->lineObjectList, ModelID_line, TextureID_WhiteTexture, transform, Color );
}

internal void
DrawCircle( RENDER_PASS * Pass, vec2 Center, flo32 Radius, vec4 Color ) {
    mat4 transform = mat4_translate( Center, 0.0f ) * mat4_scale( Radius, Radius, 1.0f );
    DrawModel( &Pass->triObjectList, ModelID_circle, TextureID_WhiteTexture, transform, Color );
}

internal void
DrawCircleOutline( RENDER_PASS * Pass, vec2 Position, flo32 Radius, vec4 modColor ) {
    mat4 transform = mat4_translate( Position, 0.0f ) * mat4_scale( Radius, Radius, 1.0f );
    DrawModel( &Pass->lineObjectList, ModelID_circleOutline, TextureID_WhiteTexture, transform, modColor );
}

internal void
DrawOBox( RENDER_PASS * Pass, TEXTURE_ID TextureID, obox Bound, vec4 modColor ) {
    mat4 transform = mat4_translate( Bound.Center ) * mat4_column( Bound.xAxis, Bound.yAxis, Bound.zAxis ) * mat4_scale( Bound.HalfDim );
    DrawModel( &Pass->triObjectList, ModelID_box, TextureID_WhiteTexture, transform, modColor );
}

internal void
DrawHexagon( RENDER_PASS * Pass, vec2 Center, flo32 Radius, vec4 Color ) {
    mat4 transform = mat4_translate( Center, 0.0f ) * mat4_scale( Radius, Radius, 1.0f );
    DrawModel( &Pass->triObjectList, ModelID_Hexagon, TextureID_WhiteTexture, transform, Color );
}

internal void
DrawOctagon( RENDER_PASS * Pass, vec2 Center, flo32 Radius, vec4 Color ) {
    mat4 transform = mat4_translate( Center, 0.0f ) * mat4_scale( Radius, Radius, 1.0f );
    DrawModel( &Pass->triObjectList, ModelID_Octagon, TextureID_WhiteTexture, transform, Color );
}