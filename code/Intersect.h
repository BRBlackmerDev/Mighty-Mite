

#define T_VALID_EPSILON ( -0.0001f )
#define RAY_INTERSECT__VALID_NEG_DIST_EPSILON ( 0.025f )
#define DOT__DEGENERATE_EPSILON  ( 0.000001f )
#define DOT__COLLINEAR_EPSILON   ( 0.000001f )
#define DOT__ORTHOGONAL_EPSILON  ( 0.000001f )

struct LINE2 {
    vec2 Origin;
    vec2 Vector;
};

struct LINE2_INTERSECT {
    boo32 IsValid;
    flo32 tA;
    flo32 tB;
    vec2  P;
};

struct RAY2 {
    vec2 Origin;
    vec2 Vector;
};

struct RAY2_INTERSECT {
    boo32 IsValid;
    flo32 t;
    vec2  P;
    vec2  N;
    
    flo32 Denom;
    flo32 tMin;
    flo32 s;
};

struct PENETRATE2D {
    boo32 IsValid;
    vec2  P;
    flo32 Dist;
};