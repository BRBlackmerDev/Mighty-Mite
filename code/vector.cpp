
#ifdef	STD_INCLUDE_DECL

//----------
// Vector declarations
//----------

union vec2 {
	struct {
		flo32 x;
		flo32 y;
	};
	flo32 elem[2];
};

union vec3 {
	struct {
		flo32 x;
		flo32 y;
		flo32 z;
	};
	struct {
		flo32 r;
		flo32 g;
		flo32 b;
	};
    struct {
        vec2  xy;
        flo32 z;
    };
	flo32 elem[3];
};

union vec4 {
	struct {
		flo32 x;
		flo32 y;
		flo32 z;
		flo32 w;
	};
	struct {
		flo32 r;
		flo32 g;
		flo32 b;
		flo32 a;
	};
	struct {
		vec3 xyz;
		flo32  w;
	};
    struct {
        vec3 rgb;
        flo32  w;
    };
    struct {
        vec2 xy;
        vec2 zw;
    };
    struct {
        vec2 P;
        vec2 Q;
    };
	flo32 elem[4];
};

//----------
// quaternion declarations
//----------

union quat {
    struct {
        flo32 x;
        flo32 y;
        flo32 z;
        flo32 w;
    };
    struct {
        vec3  xyz;
        flo32 w;
    };
    flo32 elem[ 4 ];
};

//----------
// rect declarations
//----------

enum RECT_SIDE {
    RectSide_Left,
    RectSide_Bottom,
    RectSide_Right,
    RectSide_Top,
    
    RectSide_All,
};

union rect {
	struct {
		vec2 min;
		vec2 max;
	};
	struct {
		flo32 Left;
		flo32 Bottom;
		flo32 Right;
		flo32 Top;
	};
	flo32 elem[4];
};

//----------
// oriented rect declarations
//----------

struct orect {
    vec2 Center;
    vec2 HalfDim;
    vec2 xAxis;
    vec2 yAxis;
};

//----------
// circle declarations
//----------

struct circ {
    vec2  Center;
    flo32 Radius;
};

//----------
// capsule2 declarations
//----------

enum CAPSULE_SIDE {
    CapsuleSide_Left,
    CapsuleSide_Bottom,
    CapsuleSide_Right,
    CapsuleSide_Top,
    
    CapsuleSide_All,
};

struct CAPSULE2 {
    vec2  P;
    vec2  Q;
    flo32 Radius;
};

//----------
// oriented box declarations
//----------

struct obox {
    vec3 Center;
    vec3 HalfDim;
    vec3 xAxis;
    vec3 yAxis;
    vec3 zAxis;
};

//----------
// mat 3x3 declarations
//----------

union mat3 {
	vec3  column[ 3 ];
	flo32 elem[ 9 ];
};

//----------
// mat 4x4 declarations
//----------

union mat4 {
	vec4 column[ 4 ];
	flo32 elem[ 16 ];
};

//----------
// UINT16_PAIR declarations
//----------

union UINT16_QUAD {
    struct {
        uint16 m;
        uint16 n;
        uint16 o;
        uint16 p;
    };
    struct {
        uint16 x;
        uint16 y;
        uint16 z;
        uint16 w;
    };
    uint16 elem[ 4 ];
};

//----------
// INT32_PAIR declarations
//----------

union INT32_PAIR {
    struct {
        int32 m;
        int32 n;
    };
    struct {
        int32 x;
        int32 y;
    };
    int32 elem[ 2 ];
};

//----------
// INT32_TRI declarations
//----------

union INT32_TRI {
    struct {
        int32 m;
        int32 n;
        int32 o;
    };
    struct {
        int32 x;
        int32 y;
        int32 z;
    };
    int32 elem[ 3 ];
};

//----------
// UINT32_PAIR declarations
//----------

union UINT32_PAIR {
    struct {
        uint32 m;
        uint32 n;
    };
    struct {
        uint32 x;
        uint32 y;
    };
    uint32 elem[ 2 ];
};

//----------
// UINT32_TRI declarations
//----------

union UINT32_TRI {
    struct {
        uint32 m;
        uint32 n;
        uint32 o;
    };
    struct {
        uint32 x;
        uint32 y;
        uint32 z;
    };
    uint32 elem[ 3 ];
};

//----------
// UINT32_QUAD declarations
//----------

union UINT32_QUAD {
    struct {
        uint32 m;
        uint32 n;
        uint32 o;
        uint32 p;
    };
    struct {
        uint32 x;
        uint32 y;
        uint32 z;
        uint32 w;
    };
    struct {
        UINT32_PAIR min;
        UINT32_PAIR max;
    };
    uint32 elem[ 4 ];
};

#endif   // STD_INCLUDE_DECL
#ifdef   STD_INCLUDE_FUNC

//----------
// vec2 functions
//----------

inline vec2 Vec2( flo32 a, flo32 b ) { vec2 Result = { a, b }; return Result; }
inline vec2 operator+( vec2 a, vec2 b ) { vec2 Result = { a.x + b.x, a.y + b.y }; return Result; }
inline vec2 operator-( vec2 a, vec2 b ) { vec2 Result = { a.x - b.x, a.y - b.y }; return Result; }
inline vec2 operator-( vec2 a ) { vec2 Result = { -a.x, -a.y }; return Result; }
inline vec2 operator*( vec2 a, flo32 f ) { vec2 Result = { a.x * f, a.y * f }; return Result; }
inline vec2 operator*( flo32 f, vec2 a ) { vec2 Result = a * f; return Result; }
inline vec2 operator*( vec2 a, vec2 b ) { vec2 Result = { a.x * b.x, a.y * b.y }; return Result; }
inline vec2 operator/( vec2 a, flo32 f ) { vec2 Result = {}; if( f != 0.0f ) { Result = Vec2( a.x / f, a.y / f ); } return Result; }
inline vec2 operator/( vec2 a, vec2 b ) { vec2 Result = { ( b.x != 0.0f ) ? a.x / b.x : 0.0f, ( b.y != 0.0f ) ? a.y / b.y : 0.0f }; return Result; }
inline vec2 & operator+=( vec2 & a, vec2 b ) { a = a + b; return a; }
inline vec2 & operator-=( vec2 & a, vec2 b ) { a = a - b; return a; }
inline vec2 & operator*=( vec2 & a, flo32 f ) { a = a * f; return a; }
inline vec2 & operator*=( vec2 & a, vec2 b ) { a.x *= b.x; a.y *= b.y; return a; }
inline vec2 & operator/=( vec2 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec2( 0.0f, 0.0f );	return a; }
inline boo32 operator==( vec2 a, vec2 b ) { boo32 Result = ( a.x == b.x ) && ( a.y == b.y ); return Result; }
inline boo32 operator!=( vec2 a, vec2 b ) { boo32 Result = ( a.x != b.x ) || ( a.y != b.y ); return Result; }
inline flo32 GetLengthSq( vec2 a ) { flo32 Result = ( a.x * a.x ) + ( a.y * a.y ); return Result; }
inline flo32 GetLength( vec2 a ) { flo32 Result = sqrtf( GetLengthSq( a ) ); return Result; }
inline vec2 GetNormal( vec2 a ) { flo32 Denom = GetLength( a ); vec2 Result = a / Denom; return Result; }
inline vec2 GetPerp( vec2 a ) { vec2 Result = { -a.y, a.x }; return Result; }
inline vec2 getVector( flo32 degrees ) { vec2 Result = { cosDegrees( degrees ), sinDegrees( degrees ) }; return Result; }
inline flo32 dot( vec2 a, vec2 b ) { flo32 Result = a.x * b.x + a.y * b.y; return Result; }
inline vec2 lerp( vec2 a, flo32 t, vec2 b ) { vec2 Result = Vec2( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ) ); return Result; }

inline vec2 Clamp01( vec2 a ) {
    vec2 Result = {
        Clamp01( a.x ),
        Clamp01( a.y ),
    };
    return Result;
}

internal boo32
IsCCW( vec2 A, vec2 B, vec2 C ) {
    boo32 Result = false;
    
    vec2 AB = B - A;
    vec2 AC = C - A;
    if( dot( GetPerp( AB ), AC ) > 0.0f ) {
        Result = true;
    }
    return Result;
}

//----------
// vec3 functions
//----------

inline vec3 Vec3( flo32 a ) { vec3 Result = { a, a, a }; return Result; }
inline vec3 Vec3( flo32 a, flo32 b, flo32 c ) { vec3 Result = { a, b, c }; return Result; }
inline vec3 Vec3( vec2 v, flo32 z ) { vec3 Result = { v.x, v.y, z }; return Result; }
inline vec3 operator+( vec3 a, vec3 b ) { vec3 Result = { a.x + b.x, a.y + b.y, a.z + b.z }; return Result; }
inline vec3 operator-( vec3 a, vec3 b ) { vec3 Result = { a.x - b.x, a.y - b.y, a.z - b.z }; return Result; }
inline vec3 operator-( vec3 a ) { vec3 Result = { -a.x, -a.y, -a.z }; return Result; }
inline vec3 operator*( vec3 a, flo32 f ) { vec3 Result = { a.x * f, a.y * f, a.z * f }; return Result; }
inline vec3 operator*( flo32 f, vec3 a ) { vec3 Result = a * f; return Result; }
inline vec3 operator/( vec3 a, flo32 f ) { vec3 Result = {}; if( f != 0.0f ) { Result = Vec3( a.x / f, a.y / f, a.z / f ); } return Result; }
inline vec3 & operator+=( vec3 & a, vec3 b ) { a = a + b; return a; }
inline vec3 & operator-=( vec3 & a, vec3 b ) { a = a - b; return a; }
inline vec3 & operator*=( vec3 & a, flo32 f ) { a = a * f; return a; }
inline vec3 & operator/=( vec3 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec3( 0.0f, 0.0f, 0.0f ); return a; }
inline boo32 operator==( vec3 a, vec3 b ) { boo32 Result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ); return Result; }
inline boo32 operator!=( vec3 a, vec3 b ) { boo32 Result = ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z ); return Result; }
inline flo32 GetLengthSq( vec3 a ) { flo32 Result = ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ); return Result; }
inline flo32 GetLength( vec3 a ) { flo32 Result = sqrtf( GetLengthSq( a ) ); return Result; }
inline vec3  GetNormal( vec3 a ) { flo32 Denom = GetLength( a ); vec3 Result = a / Denom; return Result; }
inline vec3 cross( vec3 a, vec3 b ) {
	vec3 Result = {};
	Result.x = ( a.y * b.z ) - ( a.z * b.y );
	Result.y = ( a.z * b.x ) - ( a.x * b.z );
	Result.z = ( a.x * b.y ) - ( a.y * b.x );
	return Result;
}
inline vec3 cross( vec3 a, vec3 b, vec3 c ) { vec3 Result = cross( b - a, c - a ); return Result; }
inline flo32 dot( vec3 a, vec3 b ) { flo32 Result = a.x * b.x + a.y * b.y + a.z * b.z; return Result; }
inline vec3 lerp( vec3 a, flo32 t, vec3 b ) { vec3 Result = Vec3( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ), lerp( a.z, t, b.z ) ); return Result; }

//----------
// vec4 functions
//----------

inline vec4 Vec4() { vec4 Result = {}; return Result; }
inline vec4 Vec4( flo32 a, flo32 b, flo32 c, flo32 d ) { vec4 Result = { a, b, c, d }; return Result; }
inline vec4 Vec4( vec2 v, flo32 z, flo32 w ) { vec4 Result = { v.x, v.y, z, w }; return Result; }
inline vec4 Vec4( vec2 xy, vec2 zw ) { vec4 Result = { xy.x, xy.y, zw.x, zw.y }; return Result; }
inline vec4 Vec4( vec3 Vector, flo32 w ) { vec4 Result = { Vector.x, Vector.y, Vector.z, w }; return Result; }
inline vec4 Vec4( flo32 a ) { vec4 Result = { a, a, a, a }; return Result; }
inline vec4 operator+( vec4 a, vec4 b ) { vec4 Result = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; return Result; }
inline vec4 operator-( vec4 a, vec4 b ) { vec4 Result = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; return Result; }
inline vec4 operator*( vec4 a, vec4 b ) { vec4 Result = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; return Result; }
inline vec4 operator*( vec4 a, flo32 f ) { vec4 Result = { a.x * f, a.y * f, a.z * f, a.w * f }; return Result; }
inline vec4 operator*( flo32 f, vec4 a ) { vec4 Result = a * f; return Result; }
inline vec4 operator/( vec4 a, flo32 f ) { vec4 Result = {}; if( f != 0.0f ) { Result = Vec4( a.x / f, a.y / f, a.z / f, a.w / f ); } return Result; }
inline vec4 operator-( vec4 a ) { vec4 Result = { -a.x, -a.y, -a.z, -a.w }; return Result; }
inline vec4 & operator+=( vec4 & a, vec4 b ) { a = a + b; return a; }
inline vec4 & operator-=( vec4 & a, vec4 b ) { a = a - b; return a; }
inline vec4 & operator*=( vec4 & a, flo32 f ) { a = a * f; return a; }
inline vec4 & operator*=( vec4 & a, vec4  b ) { a = a * b; return a; }
inline vec4 & operator/=( vec4 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec4( 0.0f, 0.0f, 0.0f, 0.0f ); return a; }
inline boo32 operator==( vec4 a, vec4 b ) { boo32 Result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) && ( a.w == b.w ); return Result; }
inline boo32 operator!=( vec4 a, vec4 b ) { boo32 Result = ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z ) || ( a.w != b.w ); return Result; }
inline flo32 GetLengthSq( vec4 a ) { flo32 Result = ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ) + ( a.w * a.w ); return Result; }
inline flo32 GetLength( vec4 a ) { flo32 Result = sqrtf( GetLengthSq( a ) ); return Result; }
inline vec4 GetNormal( vec4 a ) { flo32 Denom = GetLength( a ); vec4 Result = a / Denom; return Result; }
inline flo32 dot( vec4 a, vec4 b ) { flo32 Result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; return Result; }
inline vec4 lerp( vec4 a, flo32 t, vec4 b ) { vec4 Result = Vec4( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ), lerp( a.z, t, b.z ), lerp( a.w, t, b.w ) ); return Result; }

internal vec4
toV4( uint8 r, uint8 g, uint8 b, uint8 a ) {
	vec4 Result = { ( flo32 )r / 255.0f, ( flo32 )g / 255.0f, ( flo32 )b / 255.0f, ( flo32 )a / 255.0f };
	return Result;
}

internal vec4
modAlpha( vec4 Color, flo32 alpha ) {
    vec4 Result = Color;
    Result.rgb *= alpha;
    Result.a    = alpha;
    return Result;
}

//----------
// quaternion functions
//----------

inline quat Quat( flo32 x, flo32 y, flo32 z, flo32 w ) { quat Result = { x, y, z, w }; return Result; }
inline quat Quat( vec3 axis, flo32 radians ) {
    flo32 rad = radians * 0.5f;
    flo32 c   = cosf( rad );
    flo32 s   = sinf( rad );
    
    quat Result = {};
    Result.x = axis.x * s;
    Result.y = axis.y * s;
    Result.z = axis.z * s;
    Result.w = c;
    return Result;
}
inline quat QuatDef() { quat Result = Quat( 0.0f, 0.0f, 0.0f, 1.0f ); return Result; }
inline quat QuatInv( quat A ) { quat Result = { -A.x, -A.y, -A.z, A.w }; return Result; }

internal quat
GetNormal( quat a ) {
    flo32 length = sqrtf( a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w );
    
    quat Result = {};
    for( uint32 iter = 0; iter < 4; iter++ ) {
        Result.elem[ iter ] = a.elem[ iter ] / length;
    }
    return Result;
}

internal quat
operator*( quat b, quat a ) {
    vec3 vA = Vec3( a.x, a.y, a.z );
    vec3 vB = Vec3( b.x, b.y, b.z );
    vec3 vAxis = ( vB * a.w ) + ( vA * b.w ) + cross( vB, vA );
    //vec3 vAxis = ( vB * a.w ) + ( vA * b.w ) + cross( vA, vB );
    
    quat Result = {};
    Result.x = vAxis.x;
    Result.y = vAxis.y;
    Result.z = vAxis.z;
    Result.w = ( a.w * b.w ) - dot( vA, vB );
    Result = GetNormal( Result );
    return Result;
}

internal quat
lerp( quat a, flo32 t, quat b ) {
    quat Result = {};
    for( uint32 iter = 0; iter < 4; iter++ ) {
        Result.elem[ iter ] = lerp( a.elem[ iter ], t, b.elem[ iter ] );
    }
    Result = GetNormal( Result );
    return Result;
}

internal quat
operator*( quat a, flo32 t ) {
    quat Result = a;
    for( uint32 iter = 0; iter < 4; iter++ ) {
        Result.elem[ iter ] *= t;
    }
    return Result;
}

internal quat
RightToLeft( quat a ) {
    quat Result = {};
    Result.x = -a.x;
    Result.y = -a.z;
    Result.z = -a.y;
    Result.w =  a.w;
    return Result;
}

//----------
// rect functions
//----------

inline rect Rect( flo32 Left, flo32 Bottom, flo32 Right, flo32 Top ) { rect Result = { Left, Bottom, Right, Top }; return Result; }
inline vec2 getP( rect r, vec2 t ) {
    vec2 Result = {};
    Result.x = r.Left   + ( r.Right - r.Left   ) * t.x;
    Result.y = r.Bottom + ( r.Top   - r.Bottom ) * t.y;
    return Result;
}
inline flo32 GetWidth(  rect r ) { flo32 Result = ( r.Right - r.Left ); return Result; }
inline flo32 GetHeight( rect r ) { flo32 Result = ( r.Top - r.Bottom ); return Result; }
inline vec2 GetDim( rect r ) { vec2 Result = { GetWidth( r ), GetHeight( r ) }; return Result; }
inline vec2 getHalfDim( rect r ) { vec2 Result = GetDim( r ) * 0.5f; return Result; }
inline vec2 GetTL( rect r ) { vec2 Result = Vec2( r.Left,  r.Top    ); return Result; }
inline vec2 GetTR( rect r ) { vec2 Result = Vec2( r.Right, r.Top    ); return Result; }
inline vec2 GetBL( rect r ) { vec2 Result = Vec2( r.Left,  r.Bottom ); return Result; }
inline vec2 GetBR( rect r ) { vec2 Result = Vec2( r.Right, r.Bottom ); return Result; }
inline vec2 GetTC( rect r ) { vec2 Result = Vec2( ( r.Left + r.Right ) * 0.5f, r.Top    ); return Result; }
inline vec2 GetBC( rect r ) { vec2 Result = Vec2( ( r.Left + r.Right ) * 0.5f, r.Bottom ); return Result; }
inline vec2 GetLC( rect r ) { vec2 Result = Vec2( r.Left,  ( r.Bottom + r.Top ) * 0.5f );  return Result; }
inline vec2 GetRC( rect r ) { vec2 Result = Vec2( r.Right, ( r.Bottom + r.Top ) * 0.5f );  return Result; }
inline rect RectTLD( vec2 TopLeft,     vec2 Dim ) { rect Result = Rect( TopLeft.x,             TopLeft.y - Dim.y,  TopLeft.x + Dim.x,    TopLeft.y             ); return Result; }
inline rect RectTRD( vec2 TopRight,    vec2 Dim ) { rect Result = Rect( TopRight.x - Dim.x,    TopRight.y - Dim.y, TopRight.x,           TopRight.y            ); return Result; }
inline rect RectBLD( vec2 BottomLeft,  vec2 Dim ) { rect Result = Rect( BottomLeft.x,          BottomLeft.y,       BottomLeft.x + Dim.x, BottomLeft.y  + Dim.y ); return Result; }
inline rect RectBRD( vec2 BottomRight, vec2 Dim ) { rect Result = Rect( BottomRight.x - Dim.x, BottomRight.y,      BottomRight.x,        BottomRight.y + Dim.y ); return Result; }
inline rect RectLCD( vec2   LeftCenter, vec2 Dim ) { flo32 HalfDimY = Dim.y * 0.5f; rect Result = Rect(  LeftCenter.x,  LeftCenter.y - HalfDimY,  LeftCenter.x + Dim.x,           LeftCenter.y + HalfDimY ); return Result; }
inline rect RectRCD( vec2  RightCenter, vec2 Dim ) { flo32 HalfDimY = Dim.y * 0.5f; rect Result = Rect( RightCenter.x - Dim.x,         RightCenter.y - HalfDimY, RightCenter.x, RightCenter.y + HalfDimY ); return Result; }
inline rect RectBCD( vec2 BottomCenter, vec2 Dim ) { flo32 HalfDimX = Dim.x * 0.5f; rect Result = Rect( BottomCenter.x - HalfDimX, BottomCenter.y,      BottomCenter.x + HalfDimX, BottomCenter.y + Dim.y ); return Result; }
inline rect RectTCD( vec2    TopCenter, vec2 Dim ) { flo32 HalfDimX = Dim.x * 0.5f; rect Result = Rect(    TopCenter.x - HalfDimX, TopCenter.y - Dim.y,    TopCenter.x + HalfDimX, TopCenter.y            ); return Result; }
inline rect RectCD(  vec2 Center, vec2 Dim ) { vec2 HalfDim = Dim * 0.5f; rect Result = Rect( Center.x - HalfDim.x, Center.y - HalfDim.y, Center.x + HalfDim.x, Center.y + HalfDim.y ); return Result; }
inline rect RectCHD( vec2 Center, vec2 HalfDim ) { rect Result = Rect( Center.x - HalfDim.x, Center.y - HalfDim.y, Center.x + HalfDim.x, Center.y + HalfDim.y ); return Result; }
inline rect RectCRad(  vec2 Center, flo32 Radius ) { rect Result = Rect( Center.x - Radius, Center.y - Radius, Center.x + Radius, Center.y + Radius ); return Result; }
inline rect RectMM(  vec2 min, vec2 max ) { rect Result = Rect( min.x, min.y, max.x, max.y ); return Result; }
inline rect AddDim( rect r, flo32 x0, flo32 y0, flo32 x1, flo32 y1 ) { rect Result = Rect( r.Left - x0, r.Bottom - y0, r.Right + x1, r.Top + y1 ); return Result; }
inline rect AddDim( rect r, vec2 Dim ) { rect Result = Rect( r.Left - Dim.x, r.Bottom - Dim.y, r.Right + Dim.x, r.Top + Dim.y ); return Result; }
inline rect AddDim( rect R, rect Offset ) {
    rect Result = Rect( R.Left - Offset.Left, R.Bottom - Offset.Bottom, R.Right + Offset.Right, R.Top + Offset.Top );
    return Result;
}
inline rect AddRadius( rect r, flo32 Radius ) { rect Result = Rect( r.Left - Radius, r.Bottom - Radius, r.Right + Radius, r.Top + Radius ); return Result; }
inline vec2 GetCenter( rect r ) { vec2 Result = Vec2( ( r.Left + r.Right ) * 0.5f, ( r.Bottom + r.Top ) * 0.5f ); return Result; }
inline rect rectCompare( vec2 point0, vec2 point1 ) {
    boo32 xMin_usePoint0 = ( point0.x < point1.x );
    boo32 yMin_usePoint0 = ( point0.y < point1.y );
    rect Result = Rect(
                       ( xMin_usePoint0 ? point0.x : point1.x ), ( yMin_usePoint0 ? point0.y : point1.y ),
                       ( xMin_usePoint0 ? point1.x : point0.x ), ( yMin_usePoint0 ? point1.y : point0.y )
                       );
    return Result;
}
inline rect RectExtrema( vec2 P, vec2 Q ) {
    rect Result = { MinValue( P.x, Q.x ), MinValue( P.y, Q.y ), MaxValue( P.x, Q.x ), MaxValue( P.y, Q.y ) };
    return Result;
}
inline rect RectExtrema( rect R, rect S ) {
    rect Result = {
        MinValue( R.Left,   S.Left   ),
        MinValue( R.Bottom, S.Bottom ),
        MaxValue( R.Right,  S.Right  ),
        MaxValue( R.Top,    S.Top    ),
    };
    return Result;
}
inline rect AddOffset( rect r, vec2 offset ) { rect Result = Rect( r.Left + offset.x, r.Bottom + offset.y, r.Right + offset.x, r.Top + offset.y ); return Result; }

inline boo32 IsInBound( vec2 point, rect Bound ) { boo32 Result = ( point.x >= Bound.Left ) && ( point.y >= Bound.Bottom ) && ( point.x <= Bound.Right ) && ( point.y <= Bound.Top ); return Result; }

internal vec2
ToDirection2D( flo32 radians ) {
    vec2 Result = Vec2( cosf( radians ), sinf( radians ) );
    return Result;
}

internal flo32
ToRadians( vec2 v ) {
    flo32 Result = atan2f( v.y, v.x );
    if( Result < 0.0f ) {
        Result += ( PI * 2.0f );
    }
    return Result;
}

internal vec2
RandomV2() {
    vec2 Result = { RandomF32(), RandomF32() };
    return Result;
}

internal vec2
RandomDirection2D() {
    flo32 radians = RandomF32() * 2.0f * PI;
    vec2 Result = ToDirection2D( radians );
    return Result;
}

internal vec2
GetT( rect a, vec2 t ) {
    vec2 Result = {};
    Result.x = lerp( a.Left,   t.x, a.Right );
    Result.y = lerp( a.Bottom, t.y, a.Top   );
    return Result;
}

internal vec2
getRandomPoint( rect a ) {
    vec2 t = RandomV2();
    vec2 Result = {};
    Result.x = lerp( a.Left,   t.x, a.Right );
    Result.y = lerp( a.Bottom, t.y, a.Top   );
    return Result;
}

internal vec2
ClampToBound( vec2 P, rect A ) {
    vec2 Result = P;
    Result.x = MaxValue( Result.x, A.Left   );
    Result.x = MinValue( Result.x, A.Right  );
    Result.y = MaxValue( Result.y, A.Bottom );
    Result.y = MinValue( Result.y, A.Top    );
    return Result;
}

internal rect
getIntersect( rect A, rect B ) {
    rect Result = {};
    if( !( ( A.Right < B.Left ) || ( A.Left > B.Right ) || ( A.Top < B.Bottom ) || ( A.Bottom > B.Top ) ) ) {
        Result.Left   = MaxValue( A.Left,   B.Left   );
        Result.Right  = MinValue( A.Right,  B.Right  );
        Result.Bottom = MaxValue( A.Bottom, B.Bottom );
        Result.Top    = MinValue( A.Top,    B.Top    );
    }
    return Result;
}

internal boo32
DoesRectIntersectRectInc( rect A, rect B ) {
    boo32 Result = !( ( A.Right < B.Left ) || ( A.Left > B.Right ) || ( A.Top < B.Bottom ) || ( A.Bottom > B.Top ) );
    return Result;
}

internal boo32
DoesRectIntersectRectExc( rect A, rect B ) {
    boo32 Result = !( ( A.Right <= B.Left ) || ( A.Left >= B.Right ) || ( A.Top <= B.Bottom ) || ( A.Bottom >= B.Top ) );
    return Result;
}

internal vec2
Clamp( vec2 a, flo32 lo, flo32 hi ) {
    vec2 Result = {};
    Result.x = Clamp( a.x, lo, hi );
    Result.y = Clamp( a.y, lo, hi );
    return Result;
}

internal vec4
GetLeft( rect R ) {
    vec4 Result = {};
    Result.P = GetBL( R );
    Result.Q = GetTL( R );
    return Result;
}

internal vec4
GetBottom( rect R ) {
    vec4 Result = {};
    Result.P = GetBR( R );
    Result.Q = GetBL( R );
    return Result;
}

internal vec4
GetRight( rect R ) {
    vec4 Result = {};
    Result.P = GetTR( R );
    Result.Q = GetBR( R );
    return Result;
}

internal vec4
GetTop( rect R ) {
    vec4 Result = {};
    Result.P = GetTL( R );
    Result.Q = GetTR( R );
    return Result;
}

internal rect
RectLeft( vec2 P, vec2 Q, flo32 DimX ) {
    flo32 Bottom = MinValue( P.y, Q.y );
    flo32 Top    = MaxValue( P.y, Q.y );
    
    rect Result = Rect( P.x, Bottom, P.x + DimX, Top );
    return Result;
}

internal rect
RectBottom( vec2 P, vec2 Q, flo32 DimY ) {
    flo32 Left  = MinValue( P.x, Q.x );
    flo32 Right = MaxValue( P.x, Q.x );
    
    rect Result = Rect( Left, P.y, Right, P.y + DimY );
    return Result;
}

internal rect
RectRight( vec2 P, vec2 Q, flo32 DimX ) {
    flo32 Bottom = MinValue( P.y, Q.y );
    flo32 Top    = MaxValue( P.y, Q.y );
    
    rect Result = Rect( P.x - DimX, Bottom, P.x, Top );
    return Result;
}

internal rect
RectTop( vec2 P, vec2 Q, flo32 DimY ) {
    flo32 Left  = MinValue( P.x, Q.x );
    flo32 Right = MaxValue( P.x, Q.x );
    
    rect Result = Rect( Left, P.y - DimY, Right, P.y );
    return Result;
}

//----------
// oriented rect functions
//----------

internal orect
toORect( rect R ) {
    orect Result = {};
    Result.Center  = GetCenter( R );
    Result.xAxis   = Vec2( 1.0f, 0.0f );
    Result.yAxis   = Vec2( 0.0f, 1.0f );
    Result.HalfDim = getHalfDim( R );
    return Result;
}

internal orect
addLeft( orect R, flo32 t ) {
    flo32 tA = t * 0.5f;
    
    orect Result = R;
    Result.Center    -= ( R.xAxis * tA );
    Result.HalfDim.x += tA;
    return Result;
}

internal orect
addRight( orect R, flo32 t ) {
    flo32 tA = t * 0.5f;
    
    orect Result = R;
    Result.Center    += ( R.xAxis * tA );
    Result.HalfDim.x += tA;
    return Result;
}

internal orect
addTop( orect R, flo32 t ) {
    flo32 tA = t * 0.5f;
    
    orect Result = R;
    Result.Center    += ( R.yAxis * tA );
    Result.HalfDim.y += tA;
    return Result;
}

internal orect
ORectCD( vec2 Center, vec2 Dim, flo32 radians = 0.0f ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = GetPerp( xAxis );
    
    orect Result = {};
    Result.xAxis   = xAxis;
    Result.yAxis   = yAxis;
    Result.Center  = Center;
    Result.HalfDim = Dim * 0.5f;
    return Result;
}

internal orect
ORectCHD( vec2 Center, vec2 HalfDim, flo32 radians = 0.0f ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = GetPerp( xAxis );
    
    orect Result = {};
    Result.xAxis   = xAxis;
    Result.yAxis   = yAxis;
    Result.Center  = Center;
    Result.HalfDim = HalfDim;
    return Result;
}

internal orect
orectXD( vec2 Left, vec2 xAxis, vec2 Dim ) {
    vec2 yAxis = GetPerp( xAxis );
    
    orect Result = {};
    Result.xAxis   = xAxis;
    Result.yAxis   = yAxis;
    Result.Center  = Left + xAxis * ( Dim.x * 0.5f );
    Result.HalfDim = Dim * 0.5f;
    return Result;
}

internal orect
orectCXD( vec2 Center, vec2 xAxis, vec2 Dim ) {
    vec2 yAxis = GetPerp( xAxis );
    
    orect Result = {};
    Result.xAxis   = xAxis;
    Result.yAxis   = yAxis;
    Result.Center  = Center;
    Result.HalfDim = Dim * 0.5f;
    
    return Result;
}

internal boo32
IsInBound( vec2 P, orect Bound ) {
    vec2 V = P - Bound.Center;
    boo32 Result = ( fabsf( dot( V, Bound.xAxis ) ) <= Bound.HalfDim.x ) && ( fabsf( dot( V, Bound.yAxis ) ) <= Bound.HalfDim.y );
    return Result;
}

//----------
// circle functions
//----------

internal circ
Circ( vec2 Center, flo32 Radius ) {
    circ Result = {};
    Result.Center = Center;
    Result.Radius = Radius;
    return Result;
}

internal boo32
IsInBound( vec2 p, circ c ) {
    boo32 Result = false;
    
    flo32 RadiusSq = c.Radius * c.Radius;
    flo32 distSq   = GetLengthSq( p - c.Center );
    if( distSq <= RadiusSq ) {
        Result = true;
    }
    return Result;
}

//----------
// tri functions
//----------

internal boo32
IsInBound( vec2 P, vec2 A, vec2 B, vec2 C ) {
    boo32 Result = false;
    
    vec2 X = B - A;
    vec2 Y = C - A;
    vec2 Z = P - A;
    
    flo32 dotXX = dot( X, X );
    flo32 dotXY = dot( X, Y );
    flo32 dotXZ = dot( X, Z );
    flo32 dotYY = dot( Y, Y );
    flo32 dotYZ = dot( Y, Z );
    flo32 dotZZ = dot( Z, Z );
    
    flo32 detA = ( dotXX * dotYY ) - ( dotXY * dotXY );
    if( detA != 0.0f ) {
        flo32 detX = ( dotXZ * dotYY ) - ( dotXY * dotYZ );
        flo32 detY = ( dotXX * dotYZ ) - ( dotXY * dotXZ );
        
        flo32 v = detX / detA;
        flo32 w = detY / detA;
        if( ( v >= 0.0f ) && ( w >= 0.0f ) && ( v + w <= 1.0f ) ) {
            Result = true;
        }
    }
    
    return Result;
}

//----------
// capsule2 functions
//----------

internal CAPSULE2
Capsule2( vec2 P, vec2 Q, flo32 Radius ) {
    CAPSULE2 Result = {};
    Result.P = P;
    Result.Q = Q;
    Result.Radius = Radius;
    return Result;
}

//----------
// 3x3 matrix functions
//----------

inline mat3 Mat3( vec3 a, vec3 b, vec3 c ) {
	mat3 Result = { a, b, c };
	return Result;
}

inline vec3 row( mat3 matrix, int32 rowIndex ) {
	vec3 Result = {};
	for( int32 counter = 0; counter < 3; counter++ ) {
		Result.elem[ counter ] = matrix.elem[ counter * 3 + rowIndex ];
	}
	return Result;
}

internal mat3
mat3_transpose( mat3 a ) {
	mat3 Result = {};
	for( int32 index = 0; index < 3; index++ ) {
		Result.column[ index ] = row( a, index );
	}
	return Result;
}

internal mat3
mat3_inverse( mat3 a ) {
    mat3 Result = {};
    
    vec3 U = a.column[ 0 ];
    vec3 V = a.column[ 1 ];
    vec3 W = a.column[ 2 ];
    
    flo32 Denom = dot( U, cross( V, W ) );
    if( Denom != 0.0f ) {
        vec3 A = cross( V, W ) / Denom;
        vec3 B = cross( W, U ) / Denom;
        vec3 C = cross( U, V ) / Denom;
        
        Result = mat3_transpose( Mat3( A, B, C ) );
    }
    
    return Result;
}

internal mat3
mat3_byRow( vec3 rowA, vec3 rowB, vec3 rowC ) {
	mat3 Result = { rowA.x, rowB.x, rowC.x, rowA.y, rowB.y, rowC.y, rowA.z, rowB.z, rowC.z };
	return Result;
}

// internal vec3
// operator*( mat3 a, vec3 b ) {
// vec3 Result = {};
// for( int32 index = 0; index < 3; index++ ) {
// Result.elem[ index ] = dot( row( a, index ), b );
// }
// return Result;
// }

internal vec3
operator*( mat3 a, vec3 b ) {
	vec3 Result = {};
    for( int32 index = 0; index < 3; index++ ) {
        Result.elem[ index ] = dot( row( a, index ), b );
    }
	return Result;
}

internal mat3
operator*( mat3 a, mat3 b ) {
    mat3 Result = {};
    for( int32 columnIndex = 0; columnIndex < 3; columnIndex++ ) {
        for( int32 rowIndex = 0; rowIndex < 3; rowIndex++ ) {
            int32 elemIndex = columnIndex * 3 + rowIndex;
            Result.elem[ elemIndex ] = dot( row( a, rowIndex ), b.column[ columnIndex ] );
        }
    }
    return Result;
}

internal mat3
mat3_toRotation( flo32 x, flo32 y, flo32 z, flo32 radians ) {
	flo32 cos0 = cosf( radians );
	flo32 sin0 = sinf( radians );
	flo32 invCos = 1.0f - cos0;
	
	flo32 xyInv = x * y * invCos;
	flo32 xzInv = x * z * invCos;
	flo32 yzInv = y * z * invCos;
	flo32 x2Inv = x * x * invCos;
	flo32 y2Inv = y * y * invCos;
	flo32 z2Inv = z * z * invCos;
	flo32 xSin  = x * sin0;
	flo32 ySin  = y * sin0;
	flo32 zSin  = z * sin0;
	
	mat3 Result = {
		x2Inv + cos0, xyInv + zSin, xzInv - ySin,
		xyInv - zSin, y2Inv + cos0, yzInv + xSin,
		xzInv + ySin, yzInv - xSin, z2Inv + cos0,
	};
	return Result;
}

internal flo32
mat3_cofactor( mat3 A, uint32 row_index, uint32 column_index ) {
    // cofactor = -1^i+j * det( minor )
    flo32  mat2_elem[ 4 ] = {};
    uint32 dest_index = 0;
    for( uint32 j = 0; j < 3; j++ ) {
        if( j != column_index ) {
            for( uint32 i = 0; i < 3; i++ ) {
                if( i != row_index ) {
                    mat2_elem[ dest_index++ ] = A.elem[ j * 3 + i ];
                }
            }
        }
    }
    
    flo32 Result = mat2_elem[ 0 ] * mat2_elem[ 3 ] - mat2_elem[ 1 ] * mat2_elem[ 2 ];
    if( ( ( row_index + column_index ) % 2 ) == 1 ) {
        Result = -Result;
    }
    return Result;
}

internal flo32
mat3_determinant( mat3 A ) {
    // det = sum of element * cofactor
    flo32 Result = 0.0f;
    for( uint32 iter = 0; iter < 3; iter++ ) {
        vec3 column = A.column[ iter ];
        
        flo32 element  = column.z;
        flo32 cofactor = mat3_cofactor( A, 2, iter );
        
        Result += ( element * cofactor );
    }
    return Result;
}

//----------
// 4x4 matrix functions
//----------

inline mat4
operator-( mat4 a ) {
    mat4 Result = {};
    for( uint32 iElem = 0; iElem < 16; iElem++ ) {
        Result.elem[ iElem ] = -a.elem[ iElem ];
    }
    return Result;
}

inline vec4 getRow( mat4 matrix, int32 rowIndex ) {
	vec4 Result = {};
	for( int32 counter = 0; counter < 4; counter++ ) {
		Result.elem[ counter ] = matrix.elem[ counter * 4 + rowIndex ];
	}
	return Result;
}

inline mat4 mat4_idEntity() {
    mat4 Result = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return Result;
}

internal mat4
mat4_frustum( flo32 Left, flo32 Right, flo32 Bottom, flo32 Top, flo32 nearPlane, flo32 farPlane ) {
	flo32 Width = Right - Left;
	flo32 Height = Top - Bottom;
	flo32 depth = farPlane - nearPlane;
	flo32 nearPlane2x = nearPlane * 2.0f;
	
	mat4 Result = {
        nearPlane2x / Width, 							  0.0f, 										 0.0f,  0.0f,	// column 1
        0.0f, 		 nearPlane2x / Height, 										 0.0f,  0.0f,	// column 2
		( Right + Left ) / Width, ( Top + Bottom ) / Height, -( farPlane + nearPlane ) / depth, -1.0f,	// column 3
        0.0f, 							  0.0f, 	 -nearPlane2x * farPlane / depth,  0.0f,	// column 4
	};
	
	return Result;
}

// NOTE: opengl
// internal mat4
// mat4_perspective( vec2 window_Dim, flo32 fovAngle, flo32 nearPlane, flo32 farPlane ) {
// flo32 aspectRatio = window_Dim.x / window_Dim.y;
// flo32 fovHalfAngle = fovAngle * 0.5f;
// flo32 cot = cosDegrees( fovHalfAngle ) / sinDegrees( fovHalfAngle );
// flo32 depth = nearPlane - farPlane;

// mat4 Result = {
// cot / aspectRatio, 0.0f, 0.0f, 0.0f,	// column 1
// 0.0f, cot, 0.0f, 0.0f,	// column 2
// 0.0f, 0.0f, ( farPlane + nearPlane ) / depth, -1.0f,	// column 3
// 0.0f, 0.0f, ( 2.0f * nearPlane * farPlane ) / depth,  0.0f,	// column 4
// };
// return Result;
// }

// NOTE: direct3d
internal mat4
mat4_perspective( vec2 window_Dim, flo32 fovRadians, flo32 nearPlane, flo32 farPlane ) {
	flo32 aspectRatio = window_Dim.x / window_Dim.y;
	flo32 fovHalfRadians = fovRadians * 0.5f;
	flo32 cot = cosf( fovHalfRadians ) / sinf( fovHalfRadians );
	//flo32 depth = farPlane - nearPlane;
	flo32 depth = nearPlane - farPlane;
	
	//mat4 Result = {
    //cot / aspectRatio, 0.0f, 0.0f, 0.0f,	// column 1
    //0.0f, cot, 0.0f, 0.0f,	// column 2
    //0.0f, 0.0f, farPlane / depth, 1.0f,	// column 3
    //0.0f, 0.0f, -( farPlane * nearPlane ) / depth, 0.0f,	// column 4
	//};
	
	mat4 Result = {
		cot / aspectRatio, 0.0f, 0.0f, 0.0f,	// column 1
		0.0f, cot, 0.0f, 0.0f,	// column 2
		0.0f, 0.0f, ( farPlane + nearPlane ) / depth, -1.0f,	// column 3
		0.0f, 0.0f, ( 2.0f * nearPlane * farPlane ) / depth,  0.0f,	// column 4
	};
    
	return Result;
}

// NOTE: opengl z-coord [-1..1]
// internal mat4
// mat4_orthographic( flo32 Left, flo32 Bottom, flo32 Right, flo32 Top, flo32 nearPlane, flo32 farPlane ) {
// flo32 Width = Right - Left;
// flo32 Height = Top - Bottom;
// flo32 depth = farPlane - nearPlane;

// mat4 Result = {
// 2.0f / Width, 0.0f, 0.0f, 0.0f,	// column 1
// 0.0f, 2.0f / Height, 0.0f, 0.0f,	// column 2
// 0.0f, 0.0f, -2.0f / depth, 0.0f,	// column 3
// -( Right + Left ) / Width, -( Top + Bottom ) / Height, -( farPlane + nearPlane ) / depth, 1.0f,	// column 4
// };
// return Result;
// }

// NOTE: direct3d z-coord [0..1]
internal mat4
mat4_orthographic( flo32 Left, flo32 Bottom, flo32 Right, flo32 Top ) {
	flo32 Width = Right - Left;
	flo32 Height = Top - Bottom;
	
	mat4 Result = {
		2.0f / Width, 0.0f, 0.0f, 0.0f,	// column 1
		0.0f, 2.0f / Height, 0.0f, 0.0f,	// column 2
		0.0f, 0.0f, 1.0f, 0.0f,	// column 3
		-( Right + Left ) / Width, -( Top + Bottom ) / Height, 0.0f, 1.0f,	// column 4
	};
	return Result;
}

internal mat4
mat4_orthographic( rect Bound ) {
	mat4 Result = mat4_orthographic( Bound.Left, Bound.Bottom, Bound.Right, Bound.Top );
	return Result;
}

internal mat4
mat4_transpose( mat4 a ) {
	mat4 Result = {};
	for( int32 index = 0; index < 4; index++ ) {
		Result.column[ index ] = getRow( a, index );
	}
	return Result;
}

internal mat4
mat4_translate( flo32 x, flo32 y, flo32 z ) {
	mat4 Result = mat4_idEntity();
	Result.column[3] = Vec4( x, y, z, 1.0f );
	return Result;
}

inline mat4
mat4_translate( vec2 xy, flo32 z = 0.0f ) {
	mat4 Result = mat4_translate( xy.x, xy.y, z );
	return Result;
}

inline mat4
mat4_translate( vec3 a ) {
	mat4 Result = mat4_translate( a.x, a.y, a.z );
	return Result;
}

inline mat4
mat4_rotate( vec3 axis, flo32 radians ) {
	axis = GetNormal( axis );
	
	flo32 cosValue = cosf( radians );
	flo32 sinValue = sinf( radians );
	flo32 invCos = 1.0f - cosValue;
	
	flo32 xyInv = axis.x * axis.y * invCos;
	flo32 xzInv = axis.x * axis.z * invCos;
	flo32 yzInv = axis.y * axis.z * invCos;
	flo32 x2Inv = squareValue( axis.x ) * invCos;
	flo32 y2Inv = squareValue( axis.y ) * invCos;
	flo32 z2Inv = squareValue( axis.z ) * invCos;
	flo32 xSin = axis.x * sinValue;
	flo32 ySin = axis.y * sinValue;
	flo32 zSin = axis.z * sinValue;
	
	mat4 Result = {
		x2Inv + cosValue, xyInv + zSin, xzInv - ySin, 0.0f,
		xyInv - zSin, y2Inv + cosValue, yzInv + xSin, 0.0f,
		xzInv + ySin, yzInv - xSin, z2Inv + cosValue, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return Result;
}

internal mat4
operator*( mat4 a, mat4 b ) {
	mat4 Result = {};
	for( int32 columnIndex = 0; columnIndex < 4; columnIndex++ ) {
		for( int32 rowIndex = 0; rowIndex < 4; rowIndex++ ) {
			int32 elemIndex = columnIndex * 4 + rowIndex;
			Result.elem[ elemIndex ] = dot( getRow( a, rowIndex ), b.column[ columnIndex ] );
		}
	}
	return Result;
}

internal vec4
operator*( mat4 a, vec4 b ) {
	vec4 Result = {};
	for( int32 elemIndex = 0; elemIndex < 4; elemIndex++ ) {
		Result.elem[ elemIndex ] = dot( getRow( a, elemIndex ), b );
	}
	return Result;
}

inline mat4 & operator*=( mat4 & a, mat4 b ) { a = a * b; return a; }

internal mat4
mat4_scale( flo32 xScale, flo32 yScale, flo32 zScale ) {
	mat4 Result = { xScale, 0.0f, 0.0f, 0.0f,
        0.0f, yScale, 0.0f, 0.0f,
        0.0f, 0.0f, zScale, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
	};
	return Result;
}

internal mat4
mat4_scale( vec2 xyScale, flo32 zScale ) {
    mat4 Result = mat4_scale( xyScale.x, xyScale.y, zScale );
    return Result;
}

inline mat4
mat4_scale( vec3 scale ) {
	mat4 Result = mat4_scale( scale.x, scale.y, scale.z );
	return Result;
}

inline mat4
mat4_reflect( vec3 normal ) {
    vec3 X = Vec3( 1.0f, 0.0f, 0.0f );
    vec3 Y = Vec3( 0.0f, 1.0f, 0.0f );
    vec3 Z = Vec3( 0.0f, 0.0f, 1.0f );
    
    vec3 X0 = X - normal * ( 2.0f * dot( X, normal ) );
    vec3 Y0 = Y - normal * ( 2.0f * dot( Y, normal ) );
    vec3 Z0 = Z - normal * ( 2.0f * dot( Z, normal ) );
    
    mat4 Result = {};
    Result.column[ 0 ] = Vec4( X0, 0.0f );
    Result.column[ 1 ] = Vec4( Y0, 0.0f );
    Result.column[ 2 ] = Vec4( Z0, 0.0f );
    Result.column[ 3 ] = Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    return Result;
}

internal mat4
operator*( mat4 a, flo32 f ) {
    mat4 Result = a;
    for( uint32 elem_index = 0; elem_index < 16; elem_index++ ) {
        Result.elem[ elem_index ] *= f;
    }
    return Result;
}

internal mat4
mat4_column( vec3 X, vec3 Y, vec3 Z, vec3 W = Vec3( 0.0f, 0.0f, 0.0f ) ) {
    mat4 Result = {};
    Result.column[ 0 ] = Vec4( X, 0.0f );
    Result.column[ 1 ] = Vec4( Y, 0.0f );
    Result.column[ 2 ] = Vec4( Z, 0.0f );
    Result.column[ 3 ] = Vec4( W, 1.0f );
    return Result;
}

internal flo32
mat4_cofactor( mat4 A, uint32 row_index, uint32 column_index ) {
    // cofactor = -1^i+j * det( minor )
    mat3   minor = {};
    uint32 dest_index = 0;
    for( uint32 j = 0; j < 4; j++ ) {
        if( j != column_index ) {
            for( uint32 i = 0; i < 4; i++ ) {
                if( i != row_index ) {
                    minor.elem[ dest_index++ ] = A.elem[ j * 4 + i ];
                }
            }
        }
    }
    
    flo32 Result = mat3_determinant( minor );
    if( ( ( row_index + column_index ) % 2 ) == 1 ) {
        Result = -Result;
    }
    return Result;
}

internal flo32
mat4_determinant( mat4 A ) {
    // sum of element * cofactor
    flo32 Result = 0.0f;
    for( uint32 iter = 0; iter < 4; iter++ ) {
        vec4 column = A.column[ iter ];
        
        flo32 element  = column.w;
        flo32 cofactor = mat4_cofactor( A, 3, iter );
        
        Result += ( element * cofactor );
    }
    return Result;
}

internal mat4
mat4_cofactor( mat4 A ) {
    mat4 Result = {};
    for( uint32 j = 0; j < 4; j++ ) {
        for( uint32 i = 0; i < 4; i++ ) {
            Result.elem[ j * 4 + i ] = mat4_cofactor( A, i, j );
        }
    }
    return Result;
}

internal mat4
mat4_adjugate( mat4 A ) {
    mat4 cofactor = mat4_cofactor( A );
    mat4 Result   = mat4_transpose( cofactor );
    return Result;
}

internal mat4
mat4_inverse( mat4 A ) {
    // 1 / |A| * M
    mat4 Result = {};
    
    flo32 det = mat4_determinant( A );
    mat4  M   = mat4_adjugate   ( A );
    
    if( det != 0.0f ) {
        Result = M * ( 1.0f / det );
    }
    
    return Result;
}

internal mat4
mat4_quaternion( quat orientation ) {
    flo32 _2xSq = 2.0f * ( orientation.x * orientation.x );
    flo32 _2ySq = 2.0f * ( orientation.y * orientation.y );
    flo32 _2zSq = 2.0f * ( orientation.z * orientation.z );
    
    flo32 _2wx = 2.0f * orientation.w * orientation.x;
    flo32 _2wy = 2.0f * orientation.w * orientation.y;
    flo32 _2wz = 2.0f * orientation.w * orientation.z;
    flo32 _2xy = 2.0f * orientation.x * orientation.y;
    flo32 _2xz = 2.0f * orientation.x * orientation.z;
    flo32 _2yz = 2.0f * orientation.y * orientation.z;
    
    mat4 Result = {};
    Result.elem[  0 ] = 1.0f - _2ySq - _2zSq;
    Result.elem[  1 ] = _2xy + _2wz;
    Result.elem[  2 ] = _2xz - _2wy;
    
    Result.elem[  4 ] = _2xy - _2wz;
    Result.elem[  5 ] = 1.0f - _2xSq - _2zSq;
    Result.elem[  6 ] = _2yz + _2wx;
    
    Result.elem[  8 ] = _2xz + _2wy;
    Result.elem[  9 ] = _2yz - _2wx;
    Result.elem[ 10 ] = 1.0f - _2xSq - _2ySq;
    
    Result.elem[ 15 ] = 1.0f;
    
    return Result;
};

#endif	// STD_INCLUDE_FUNC