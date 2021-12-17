
internal vec3
getCameraPos( CAMERA * Camera ) {
    vec3 Result = Camera->Position + Camera->zAxis * Camera->dist;
    return Result;
}

internal mat4
genCameraTransform( vec3 Position, vec3 xAxis, vec3 yAxis, vec3 zAxis ) {
	mat3 rotMatrix   = mat3_byRow( xAxis, yAxis, zAxis );
	vec3 adjPosition = -( rotMatrix * Position );
	
	mat4 Result = {};
	Result.elem[  0 ] = rotMatrix.elem[ 0 ];
	Result.elem[  1 ] = rotMatrix.elem[ 1 ];
	Result.elem[  2 ] = rotMatrix.elem[ 2 ];
	Result.elem[  3 ] = 0.0f;
	Result.elem[  4 ] = rotMatrix.elem[ 3 ];
	Result.elem[  5 ] = rotMatrix.elem[ 4 ];
	Result.elem[  6 ] = rotMatrix.elem[ 5 ];
	Result.elem[  7 ] = 0.0f;
	Result.elem[  8 ] = rotMatrix.elem[ 6 ];
	Result.elem[  9 ] = rotMatrix.elem[ 7 ];
	Result.elem[ 10 ] = rotMatrix.elem[ 8 ];
	Result.elem[ 11 ] = 0.0f;
	Result.elem[ 12 ] = adjPosition.x;
	Result.elem[ 13 ] = adjPosition.y;
	Result.elem[ 14 ] = adjPosition.z;
	Result.elem[ 15 ] = 1.0f;
	return Result;
}

internal void
UpdateCameraMovement( CAMERA * c, KEYBOARD_STATE * Keyboard, MOUSE_STATE * Mouse, flo32 dT, rect Bound ) {
    flo32 Camera_hRadians = c->hRadians;
    vec3  Camera_xAxis    = c->xAxis;
    vec3  Camera_yAxis    = c->yAxis;
    vec3  Camera_zAxis    = c->zAxis;
    
    if( ( IsInBound( Mouse->Position, Bound ) ) && ( Mouse->WheelClicks != 0 ) ) {
        c->dist -= ( ( flo32 )Mouse->WheelClicks * 1.0f );
    }
    
    // Camera movement
    if( c->MouseMove_IsActive ) {
        vec2 Move_Vector = Mouse->dPos * -0.005f;
        c->Position += ( Camera_xAxis * ( Move_Vector.x ) + Camera_yAxis * ( Move_Vector.y ) );
    }
    if( c->MouseOrbit_IsActive ) {
        vec2 radians = Mouse->dPos * 0.005f;
        c->hRadians -= radians.x;
        c->vRadians -= radians.y;
    }
    if( WasReleased( Mouse, MouseButton_middle ) ) {
        Mouse->ResetPos = false;
        
        c->MouseMove_IsActive  = false;
        c->MouseOrbit_IsActive = false;
    }
    if( WasPressed( Mouse, MouseButton_middle ) ) {
        Mouse->downPos  = Mouse->Position;
        Mouse->ResetPos = true;
        if( Keyboard->flags == KEYBOARD_FLAGS__SHIFT ) {
            c->MouseMove_IsActive = true;
        } else {
            c->MouseOrbit_IsActive = true;
        }
    }
    
    flo32 hCos = cosf( c->hRadians );
    flo32 hSin = sinf( c->hRadians );
    flo32 vCos = cosf( c->vRadians );
    flo32 vSin = sinf( c->vRadians );
    
    c->zAxis = Vec3( hSin * vCos, vSin,  hCos * vCos );
    c->xAxis = Vec3( hCos,        0.0f, -hSin        );
    c->yAxis = GetNormal( cross( c->zAxis, c->xAxis ) );
    
#if USE_IMVU_CAMERA
    c->zAxis = Vec3( hSin * vCos, -hCos * vCos, vSin );
    c->xAxis = Vec3( hCos,        hSin,  0.0f        );
    c->yAxis = GetNormal( cross( c->zAxis, c->xAxis ) );
#endif
    
    flo32 speed = 12.5f;
    if( c->moveSlow_IsActive ) { speed = 5.0f; }
    if( c->Move_Left     ) { c->Position -= c->xAxis * ( speed * dT ); }
    if( c->Move_Right    ) { c->Position += c->xAxis * ( speed * dT ); }
    if( c->Move_Down     ) { c->Position -= c->yAxis * ( speed * dT ); }
    if( c->Move_Up       ) { c->Position += c->yAxis * ( speed * dT ); }
    if( c->Move_forward  ) { c->Position -= c->zAxis * ( speed * dT ); }
    if( c->Move_Backward ) { c->Position += c->zAxis * ( speed * dT ); }
}      

internal void
UpdateCameraInput( CAMERA * Camera, KEYBOARD_STATE * Keyboard ) {
    if( WasPressed ( Keyboard, KeyCode_num4 ) ) { Camera->Move_Left     = true;  }
    if( WasReleased( Keyboard, KeyCode_num4 ) ) { Camera->Move_Left     = false; }
    if( WasPressed ( Keyboard, KeyCode_num6 ) ) { Camera->Move_Right    = true;  }
    if( WasReleased( Keyboard, KeyCode_num6 ) ) { Camera->Move_Right    = false; }
    if( WasPressed ( Keyboard, KeyCode_num9 ) ) { Camera->Move_forward  = true;  }
    if( WasReleased( Keyboard, KeyCode_num9 ) ) { Camera->Move_forward  = false; }
    if( WasPressed ( Keyboard, KeyCode_num7 ) ) { Camera->Move_Backward = true;  }
    if( WasReleased( Keyboard, KeyCode_num7 ) ) { Camera->Move_Backward = false; }
    if( WasPressed ( Keyboard, KeyCode_num8 ) ) { Camera->Move_Up       = true;  }
    if( WasReleased( Keyboard, KeyCode_num8 ) ) { Camera->Move_Up       = false; }
    if( WasPressed ( Keyboard, KeyCode_num5 ) ) { Camera->Move_Down     = true;  }
    if( WasReleased( Keyboard, KeyCode_num5 ) ) { Camera->Move_Down     = false; }
    if( WasPressed ( Keyboard, KeyCode_num0 ) ) { Camera->moveSlow_IsActive = true;  }
    if( WasReleased( Keyboard, KeyCode_num0 ) ) { Camera->moveSlow_IsActive = false; }
    
    if( WasPressed( Keyboard, KeyCode_num1 ) ) {
        Camera->hRadians = 0.0f;
        Camera->vRadians = PI * 0.5f;
    }
    if( WasPressed( Keyboard, KeyCode_num2 ) ) {
        Camera->hRadians = 0.0f;
        Camera->vRadians = 0.0f;
    }
    if( WasPressed( Keyboard, KeyCode_num3 ) ) {
        Camera->hRadians = PI * 0.5f;
        Camera->vRadians = 0.0f;
    }
}

internal mat4
genCameraTransform( CAMERA * Camera, vec2 view_Dim ) {
    vec3 Camera_Position  = getCameraPos( Camera );
    mat4 main_Camera      = genCameraTransform( Camera_Position, Camera->xAxis, Camera->yAxis, Camera->zAxis );
    
    mat4 main_perspective = mat4_perspective ( view_Dim, Camera->fovAngle, Camera->nearPlane, Camera->farPlane );
    
    mat4 Result = main_perspective * main_Camera;
    return Result;
}