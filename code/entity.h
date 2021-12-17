
//----------
// SHARED/TOOLS
//----------

enum ENTITY_MODE {
    EntityMode_Move,
    EntityMode_Jump,
};

enum ENTITY_TYPE {
    EntityType_Count,
};

#define ENTITY_TYPE_TAGS  char * EntityTypeTag[ EntityType_Count ] = {}

enum HIT_TYPE {
    HitType_Wall,
    
    HitType_Count,
};

struct BULLET_HIT {
    HIT_TYPE       Type;
    int32          iHit;
    RAY2_INTERSECT bHit;
};

internal BULLET_HIT
BulletHit( HIT_TYPE Type, int32 iHit, RAY2_INTERSECT bHit ) {
    BULLET_HIT Result = {};
    Result.Type = Type;
    Result.iHit = iHit;
    Result.bHit = bHit;
    return Result;
}

#define WORLD_GRAVITY_ACCEL  ( -40.0f )

//----------
// DEBUG_PANEL
//----------

#define PANEL_LABEL_Y  ( 14.0f )
struct PANEL {
    boo32 Show;
    rect  Bound;
    
    rect  Inner;
    vec2  AtPos;
    flo32 advanceY;
    flo32 ScrollT;
};

//----------
// STRING
//----------

struct STRING {
    char * string;
    uint32 nChar;
};

internal boo32
MatchString( char * string0, char * string1 ) {
    boo32 doMatch = true;
    while( ( *string0 ) && ( *string1 ) ) {
        if( *string0 != *string1 ) { doMatch = false; }
        string0++;
        string1++;
    }
    if( ( *string0 ) || ( *string1 ) ) { doMatch = false; }
    return doMatch;
}

internal boo32
MatchSegment( char * string0, char * string1, uint32 nChar ) {
    boo32 doMatch = true;
    for( uint32 iChar = 0; ( doMatch ) && ( iChar < nChar ); iChar++ ) {
        if( string0[ iChar ] != string1[ iChar ] ) { doMatch = false; }
    }
    return doMatch;
}

internal boo32
MatchSegmentNoCase( char * StringA, char * StringB, uint32 nChar ) {
    boo32 DoMatch = true;
    for( uint32 iChar = 0; ( DoMatch ) && ( iChar < nChar ); iChar++ ) {
        uint32 CharA = ( uint32 )StringA[ iChar ];
        uint32 CharB = ( uint32 )StringB[ iChar ];
        if( CharA < 'a' ) { CharA += 0X00000020; }
        if( CharB < 'a' ) { CharB += 0x00000020; }
        if( CharA != CharB ) { DoMatch = false; }
    }
    return DoMatch;
}

internal void
writeSegment( MEMORY * memory, char * str ) {
    uint32 length = ( uint32 )strlen( str );
    uint8 * dest = ( uint8 * )_pushSize( memory, length );
    memcpy( dest, str, length );
}

internal void
writeString( MEMORY * memory, char * str, uint32 nChar ) {
    uint8 * dest = ( uint8 * )_pushSize( memory, nChar );
    memcpy( dest, str, nChar );
    
    _writem( memory, uint8, 0 );
}

internal void
writeString( MEMORY * memory, char * str ) {
    uint32 nChar = ( uint32 )strlen( str );
    writeString( memory, str, nChar );
}

internal boo32
MatchString( STRING strA, STRING strB ) {
    boo32 Result = ( strA.nChar == strB.nChar );
    if( Result ) {
        Result = MatchSegment( strA.string, strB.string, strA.nChar );
    }
    return Result;
}

internal boo32
MatchString( STRING strA, char * str ) {
    uint32 nChar = ( uint32 )strlen( str );
    boo32 Result = ( strA.nChar == nChar );
    if( Result ) {
        Result = MatchSegment( strA.string, str, strA.nChar );
    }
    return Result;
}

//----------
// STRING BUFFER
//----------

struct STRING_BUFFER {
    uint32 nStr;
    uint32 nStrMax;
    char ** str;
    MEMORY  memory;
};

internal void
Reset( STRING_BUFFER * buffer ) {
    buffer->nStr        = 0;
    buffer->memory.used = 0;
}

internal STRING_BUFFER
StringBuffer( MEMORY * parent_memory, uint32 maxString, uint32 maxSize ) {
    STRING_BUFFER Result = {};
    Result.str     = _pushArray_clear( parent_memory, char *, maxString );
    Result.memory  = SubArena( parent_memory, maxSize );
    Result.nStrMax = maxString;
    return Result;
}

internal char *
AddString( STRING_BUFFER * s, char * string ) {
    char * dest = 0;
    if( s->nStr < s->nStrMax ) {
        uint32 length = ( uint32 )strlen( string );
        dest = ( char * )_pushSize( &s->memory, length + 1 );
        strcpy( dest, string );
        
        s->str[ s->nStr++ ] = dest;
    }
    
    return dest;
}

//----------
// COLLISION
//----------

enum COLLISION_TYPE {
    CollisionType_Left,
    CollisionType_Bottom,
    CollisionType_Right,
    CollisionType_Top,
    
    CollisionType_Count,
};

struct COLLISION_STATE {
    UINT32_PAIR Type[ CollisionType_Count ];
    uint32 nEdge;
    vec4  * Edge;
    
    uint32 nJumpBound;
    rect *  JumpBound;
    
    uint32 nWallSlideLeft;
    rect *  WallSlideLeft;
    
    uint32 nWallSlideRight;
    rect *  WallSlideRight;
};

//----------
// PARTICLES
//----------

struct PARTICLE {
    vec2 Position;
    vec2 Velocity;
    vec2 Accel;
    
    vec2  Dim;
    flo32 Radians;
    
    flo32 Timer;
    flo32 TargetTime;
    vec4  Color;
};

#define PARTICLE_MAX_COUNT  ( 2048 )
struct PARTICLE_STATE {
    uint32    nParticle;
    PARTICLE * Particle;
};

//----------
// ROOM STATS
//----------

struct LEVEL_STATS {
    rect  Bound;
    flo32 EnterLeftY;
    flo32 EnterRightY;
    flo32 EnterBottomX;
    flo32 EnterTopX;
};

//----------
// EVENT
//----------

struct PLATFORM;
struct APP_STATE;
typedef void ( EVENT_FUNC )( PLATFORM * Platform, APP_STATE * AppState, uint8 * ptr, uint32 iEvent );

struct EVENT {
    boo32        IsActive;
    EVENT_FUNC * Func;
    uint8      * Data;
    // TODO: this Timer variable should be a pointer to a data block in Event_State memory. Event_State will Pass out empty blocks of memory to EVENT_FUNCs that need it (block allocator)
    flo32        Timer;
};

#define EVENT_MAX_COUNT  ( 128 )
struct EVENT_STATE {
    uint32 nEvent;
    EVENT   Event[ EVENT_MAX_COUNT ];
    
    // TODO: WORLD_STATE needs to keep track of event.IsActive and save it to File. Event data and function pointers are Loaded at Game Init.
};

//----------
// LEVEL
//----------

#define FILE_SAVE_OPEN__FILENAME_MAX_CHAR  ( 128 )
struct LEVEL {
    char  FileNameOfLevel[ FILE_SAVE_OPEN__FILENAME_MAX_CHAR + 1 ];
    
    boo32 iLinkLeft_IsActive;
    boo32 iLinkRight_IsActive;
    boo32 iLinkBottom_IsActive;
    boo32 iLinkTop_IsActive;
    
    int32 iLinkLeft;
    int32 iLinkRight;
    int32 iLinkBottom;
    int32 iLinkTop;
    
#define LEVEL__EVENT_MAX_COUNT  ( 4 )
    uint32 nEvent;
    EVENT   Event[ LEVEL__EVENT_MAX_COUNT ];
};

#define LEVEL_MAX_COUNT  ( 64 )
struct LEVEL_STATE {
    uint32 nLevel;
    LEVEL   Level[ LEVEL_MAX_COUNT ];
    
    uint32 iCurrentLevel;
    
    uint32 Checkpoint_iLevel;
    uint32 Checkpoint_iCheckpoint;
};

//----------
// TERRAIN
//----------

#define WALL_JUMP_NEG_Y_EXTENSION           ( -TILE_HEIGHT * 2.0f )
#define WALL_JUMP_MIN_LENGTH                (  TILE_HEIGHT * 2.0f )
#define WALL_JUMP_MIN_LENGTH_FOR_EXTENSION  (  TILE_HEIGHT * 3.0f )

#define TILE_WIDTH  ( 1.0f )
#define TILE_HEIGHT ( 1.0f )
#define TILE_DIM    ( Vec2( TILE_WIDTH, TILE_HEIGHT ) )

#define LEVEL_DEFAULT_WIDTH   ( 32.0f )
#define LEVEL_DEFAULT_HEIGHT  ( 18.0f )
internal vec2
GetPOfLevel( vec2 Pos, rect Bound ) {
    vec2 Result = {};
    Result.x = Bound.Left   + Pos.x * TILE_WIDTH;
    Result.y = Bound.Bottom + Pos.y * TILE_HEIGHT;
    return Result;
}

#define CAMERA_TILE_Y_COUNT  ( 18.2f )

#define TERRAIN_COLOR_U32     ( 0xFF7A5A02 )
#define TERRAIN_COLOR         ( ToColor( 122, 90, 2 ) )
#define TERRAIN_OUTLINE_COLOR ( ToColor( 92,  68, 3 ) )

struct TERRAIN {
    rect Bound;
};

#define TERRAIN_MAX_COUNT  ( 2048 )
struct TERRAIN_STATE {
    uint32 nTerrain;
    TERRAIN Terrain[ TERRAIN_MAX_COUNT ];
};

//----------
// GRENADE
//----------

#define GRENADE_RADIUS           ( TILE_WIDTH * 0.65f )
#define GRENADE_COLOR            ( ToColor( 90, 105, 5 ) )

#define GRENADE_DAMAGE_RADIUS     ( TILE_WIDTH * 8.0f )
#define GRENADE_DAMAGE_TO_PLAYER  ( 20 )
#define GRENADE_DAMAGE_TO_JUMPER  ( 20 )
#define GRENADE_DAMAGE_TO_NEST    ( 60 )

#define GRENADE_THROW_SPEED_HI   ( 20.0f )
#define GRENADE_THROW_SPEED_LO   ( 10.0f )
#define GRENADE_AIR_FRICTION     ( 0.1f )
#define GRENADE_GROUND_FRICTION  ( 6.0f )
#define GRENADE_GRAVITY          ( 32.0f )

#define GRENADE_DETONATE_TARGET_TIME  ( 80.0f / 60.0f )

struct GRENADE {
    vec2  Position;
    vec2  Velocity;
    flo32 Timer;
};

#define GRENADE_MAX_COUNT  ( 8 )
struct GRENADE_STATE {
    uint32 nGrenade;
    GRENADE Grenade[ GRENADE_MAX_COUNT ];
};

//----------
// CHECKPOINT
//----------

struct CHECKPOINT {
    rect Bound;
};

#define CHECKPOINT_MAX_COUNT  ( 128 )
struct CHECKPOINT_STATE {
    uint32    nCheckpoint;
    CHECKPOINT Checkpoint[ CHECKPOINT_MAX_COUNT ];
};

struct EDITOR__CHECKPOINT {
    rect Bound;
};

struct EDITOR__CHECKPOINT_STATE {
    uint32            nCheckpoint;
    EDITOR__CHECKPOINT Checkpoint[ CHECKPOINT_MAX_COUNT ];
};

//----------
// EXERCISE_BALL
//----------

#define EXERCISE_BALL_RADIUS  ( TILE_WIDTH * 0.8f )
#define EXERCISE_BALL_DIM     ( Vec2( EXERCISE_BALL_RADIUS, EXERCISE_BALL_RADIUS ) * 2.0f )

#define EXERCISE_BALL_AIR_FRICTION     ( 0.1f )
#define EXERCISE_BALL_GROUND_FRICTION  ( 4.0f )
#define EXERCISE_BALL_GRAVITY          ( 30.0f )
#define EXERCISE_BALL_THROW_SPEED      ( 12.0f )

struct EXERCISE_BALL {
    vec2   Position;
    vec2   Velocity;
    uint32 nBounce;
};

#define EXERCISE_BALL_MAX_COUNT  ( 128 )
struct EXERCISE_BALL_STATE {
    uint32       nExerciseBall;
    EXERCISE_BALL ExerciseBall[ EXERCISE_BALL_MAX_COUNT ];
};

struct EDITOR__EXERCISE_BALL {
    vec2 Position;
};

struct EDITOR__EXERCISE_BALL_STATE {
    uint32               nExerciseBall;
    EDITOR__EXERCISE_BALL ExerciseBall[ EXERCISE_BALL_MAX_COUNT ];
};

//----------
// CAMPER
//----------

#define CAMPER_WIDTH        ( TILE_WIDTH  * 0.7f )
#define CAMPER_HALF_WIDTH   ( CAMPER_WIDTH * 0.5f )
#define CAMPER_HEIGHT       ( TILE_HEIGHT * 1.0f )
#define CAMPER_HALF_HEIGHT  ( CAMPER_HEIGHT * 0.5f )
#define CAMPER_DIM          ( Vec2( CAMPER_WIDTH, CAMPER_HEIGHT ) )

#define CAMPER_FRICTION     ( 8.0f )
#define CAMPER_GRAVITY      ( 28.0f )

struct CAMPER {
    vec2  Position;
    vec2  Velocity;
    
    rect  HazardBound;
};

#define CAMPER_MAX_COUNT  ( 128 )
struct CAMPER_STATE {
    uint32 nCamper;
    CAMPER  Camper[ CAMPER_MAX_COUNT ];
};

struct EDITOR__CAMPER {
    vec2  Position;
    rect  HazardBound;
};

struct EDITOR__CAMPER_STATE {
    uint32        nCamper;
    EDITOR__CAMPER Camper[ CAMPER_MAX_COUNT ];
};

//----------
// PLAYER
//----------

// COMMON STATS
#define LOAD_CHECKPOINT_TARGET_TIME  ( 72.0f / 60.0f )

#define PLAYER_HEALTH                          ( 6 )
#define PLAYER_SHOW_DAMAGE_CYCLE_TARGET_TIME   ( 0.1f )

#define PLAYER_NO_DAMAGE_TARGET_TIME  ( 90.0f / 60.0f )
#define PLAYER_DAMAGE_BOOST_SPEED     ( Vec2( 12.0f, 6.0f ) )

#define PLAYER_WIDTH       ( TILE_WIDTH  * 0.8f )
#define PLAYER_HEIGHT      ( TILE_HEIGHT * 1.4f )
#define PLAYER_HALF_WIDTH  ( PLAYER_WIDTH * 0.5f )
#define PLAYER_DIM         ( Vec2( PLAYER_WIDTH, PLAYER_HEIGHT ) )
#define PLAYER_COLLISION_OFFSET  ( Rect( PLAYER_HALF_WIDTH, 0.0f, PLAYER_HALF_WIDTH, PLAYER_HEIGHT ) )

#define PLAYER__RUNNER_COLLISION_OFFSET  ( Rect( RUNNER_HALF_WIDTH, RUNNER_HEIGHT, RUNNER_HALF_WIDTH, 0.0f ) )

#define PLAYER_DEAD_TARGET_TIME  ( 0.3f )
#define PLAYER_DEAD_FRICTION     ( 6.0f )

#define PLAYER_GRAVITY  ( 30.0f )
#define PLAYER_JUMP_SPEED        ( 14.25f )
#define PLAYER_JUMP_TARGET_TIME  ( 22.0f / 60.0f )

#define PLAYER_WALL_JUMP_SPEED_X  ( 9.0f  )
#define PLAYER_WALL_JUMP_SPEED_Y  ( 9.75f )

#define PLAYER_WALK_SPEED     ( 5.0f  )
#define PLAYER_WALK_FRICTION  ( 20.0f )
#define PLAYER_AIR_FRICTION   ( 6.0f  )

#define PLAYER_RUN_SPEED      ( 8.0f )
#define PLAYER_RUN_FRICTION   ( 20.0f )

#define PLAYER_CARRY_SPEED       (  3.0f )
#define PLAYER_CARRY_FRICTION    ( 20.0f )
#define PLAYER_CARRY_JUMP_SPEED  (  9.0f )

#define PLAYER_STAMINA_COOLDOWN_TARGET_TIME     ( 13.0f / 60.0f )
#define PLAYER_STAMINA_CHARGE_FAST_TARGET_TIME  ( 5.0f )
#define PLAYER_STAMINA_CHARGE_SLOW_TARGET_TIME  ( 8.0f )

#define PLAYER_STAMINA_RUN_RATE     ( 3.2f ) // Per Second
#define PLAYER_STAMINA_CHARGE_RATE  ( 4.4f ) // Per Second
#define PLAYER_STAMINA_JUMP_RATE    ( 4.0f ) // Per Second
#define PLAYER_STAMINA_PUNCH_RATE   ( 2.0f ) // Per Second
#define PLAYER_STAMINA_CARRY_RATE   ( 5.0f ) // Per Second
#define PLAYER_STAMINA_GRAB_RATE    ( 6.0f ) // Per Second
#define PLAYER_STAMINA_JUMP_COST    ( 1.6f )
#define PLAYER_STAMINA_DASH_COST    ( 2.0f )
#define PLAYER_STAMINA_PUNCH_COST   ( 5.0f )
#define PLAYER_STAMINA_PICKUP_COST  ( 3.6f )
#define PLAYER_STAMINA_THROW_COST   ( 2.4f )

#define PLAYER_STAMINA_XP_NO_STAMINA_XP_BOOST  ( 6.0f )
#define PLAYER_STAMINA_XP_RUN_RATE    ( PLAYER_STAMINA_RUN_RATE    )
#define PLAYER_STAMINA_XP_CHARGE_RATE ( PLAYER_STAMINA_CHARGE_RATE )
#define PLAYER_STAMINA_XP_JUMP_RATE   ( PLAYER_STAMINA_JUMP_RATE   )
#define PLAYER_STAMINA_XP_PUNCH_RATE  ( PLAYER_STAMINA_PUNCH_RATE  )
#define PLAYER_STAMINA_XP_CARRY_RATE  ( PLAYER_STAMINA_CARRY_RATE  )
#define PLAYER_STAMINA_XP_GRAB_RATE   ( PLAYER_STAMINA_GRAB_RATE   )
#define PLAYER_STAMINA_XP_JUMP_XP     ( PLAYER_STAMINA_JUMP_COST   )
#define PLAYER_STAMINA_XP_DASH_XP     ( PLAYER_STAMINA_DASH_COST   )
#define PLAYER_STAMINA_XP_PUNCH_XP    ( PLAYER_STAMINA_PUNCH_COST  )
#define PLAYER_STAMINA_XP_PICKUP_XP   ( PLAYER_STAMINA_PICKUP_COST )
#define PLAYER_STAMINA_XP_THROW_XP    ( PLAYER_STAMINA_THROW_COST  )

#define PLAYER_PUNCH_CHARGE_TARGET_TIME  ( 72.0f / 60.0f )
#define PLAYER_PUNCH_COLLISION_DIM_LO    ( Vec2( TILE_WIDTH * 1.5f, TILE_HEIGHT * 1.15f ) )
#define PLAYER_PUNCH_COLLISION_DIM_HI    ( Vec2( TILE_WIDTH * 2.5f, TILE_HEIGHT * 2.0f ) )
#define PLAYER_PUNCH_ART_OFFSET          ( Vec2( TILE_WIDTH * 0.6f, TILE_HEIGHT * 0.8f ) )
#define PLAYER_PUNCH_ART_VIBRATE_OFFSET  ( TILE_WIDTH * 0.15f )
#define PLAYER_PUNCH_ART_TARGET_TIME     ( 3.0f / 60.0f )
#define PLAYER_PUNCH_STOMPER_SPEED_LO    ( Vec2( 12.0f, 4.0f ) )
#define PLAYER_PUNCH_STOMPER_SPEED_HI    ( Vec2( 22.0f, 6.0f ) )

#define PLAYER_WALL_SLIDE_PARTICLE_CYCLE_TARGET_TIME  ( 0.1f )
#define PLAYER_WALL_SLIDE_SPEED     (  3.75f )
#define PLAYER_WALL_SLIDE_FRICTION  (  20.0f )
#define PLAYER_FALL_MAX_SPEED       ( -28.0f )

#define PLAYER_GRENADE_COOLDOWN_TARGET_TIME  ( 120.0f / 60.0f )
#define PLAYER_BULLET_TARGET_TIME            ( 2.0f   / 60.0f )

#define PLAYER_DASH_COOLDOWN_TARGET_TIME   ( 24.0f / 60.0f )
#define PLAYER_DASH_TARGET_TIME            ( 10.0f / 60.0f )
#define PLAYER_DASH_SPEED                  ( 12.0f )
#define PLAYER_DASH_COLLISION_BOOST_SPEED  ( Vec2( 10.0f, 8.0f ) )

enum PLAYER_DRAW_ID {
    DrawID_Default,
    DrawID_WallSlide,
};

enum PLAYER_CARRY_ID {
    PlayerCarryID_None,
    PlayerCarryID_Camper,
    PlayerCarryID_Boulder,
    PlayerCarryID_ExerciseBall,
    PlayerCarryID_Puncher,
    PlayerCarryID_Popper,
    PlayerCarryID_Barbell,
};

struct ENTITY_VALID_RESULT {
    boo32  IsValid;
    uint32 iEntity;
};

struct GAME_STATE {
    uint32 MaxHealth;
    
    boo32  Stamina_IsFeatured;
    boo32  StaminaXP_IsFeatured;
    uint32 Stamina_Level;
    flo32  MaxStamina;
    flo32  MaxStaminaXP;
    flo32  StaminaXP;
    
    boo32 Run_IsFeatured;
    boo32 WallJump_IsFeatured;
    boo32 WallSlide_IsFeatured;
    boo32 Dash_IsFeatured;
    boo32 Grab_IsFeatured;
    boo32 Carry_IsFeatured;
    boo32 Punch_IsFeatured;
};

struct PLAYER_STATE {
    vec2  Position;
    vec2  dPos;
    vec2  Velocity;
    flo32 Control_xDir;
    flo32 Face_DirX;
    
    uint32 Health;
    flo32  ShowDamage_Timer;
    boo32  IsDead;
    flo32  IsDead_Timer;
    
    flo32  Money;
    
    boo32  NoDamage;
    flo32  NoDamage_Timer;
    
    PLAYER_DRAW_ID Draw_ID;
    boo32          Draw_FaceLeft;
    
    flo32  Stamina;
    boo32  Stamina_IsEnabled;
    flo32  Stamina_CooldownTimer;
    boo32  Stamina_IsBeingDrained;
    
    boo32 Run_IsActive;
    
    boo32 Jump_IsActive;
    flo32 Jump_Timer;
    boo32 Jump_DoDampen;
    
    flo32 WallSlide_ParticleTimer;
    
    boo32 Throw_WasPressed;
    flo32 GrenadeThrow_CooldownTimer;
    
#define RUN_DOUBLE_TAP_TARGET_TIME  ( 20.0f / 60.0f )
    flo32 Run_DoubleTapTimer;
    boo32 Run_DoDoubleTap;
    
    boo32 Dash_IsEnabled;
    boo32 Dash_IsActive;
    flo32 Dash_Timer;
    flo32 Dash_CooldownTimer;
    boo32 Dash_DidTouchGround;
    vec2  Dash_Direction;
    
    boo32  Grab_IsActive;
    boo32  Grab_IsDown;
    boo32  CanGrab;
    flo32  Grab_DirX;
    flo32  Grab_ArtDirX;
    boo32  GrabExerciseMiniBoss03_IsActive;
    
    boo32  Scaffold_SkipCollision;
    uint32 Scaffold_iScaffold;
    
    boo32 Use_WasPressed;
    boo32 Use_WasReleased;
    boo32 Use_IsActive;
    flo32 Use_t;
    
    boo32  CanPickUp;
    flo32  PickUp_DirX;
    PLAYER_CARRY_ID Carry_ID;
    PLAYER_CARRY_ID Carry_IsBy_ID;
    uint32          Carry_IsBy_iEntity;
    flo32           Carry_IsBy_Dist;
    vec2            CarryP;
    uint32          Carry_PuncherHealth;
    //boo32  CarryBoulder_IsActive;
    //boo32  CarryExerciseBall_IsActive;
    ENTITY_VALID_RESULT Boulder_IsBy;
    ENTITY_VALID_RESULT ExerciseBall_IsBy;
    
    CAMPER Camper_IsCarrying;
    boo32  DoSaveCamper;
    
    boo32  Punch_IsCharging;
    flo32  Punch_Charge;
    boo32  Punch_DoPunch;
    rect   Punch_Bound;
    boo32  Punch_DoDraw;
    vec2   Punch_ArtPos;
    flo32  Punch_ArtTimer;
    flo32  Punch_ArtCharge;
    boo32  Punch_FaceLeft;
    
    boo32 IsOnConveyorHor;
    boo32 IsOnConveyorVer;
    boo32 IsOnConveyorVerLeft;
    boo32 IsOnConveyorVerRight;
    vec2  ConveyorAccel;
    
    vec2  Aim_Dir;
    boo32 Fire_IsActive;
    flo32 Fire_Timer;
    vec2  Fire_pos;
    vec2  Fire_dir;
    flo32 Fire_lo;
    flo32 Fire_hi;
    boo32 Fire_doHitScan;
    
    boo32 DoBossBlowback;
    flo32 BossBlowback_Timer;
};

//----------
// PROFILE
//----------

struct PROFILE_BLOCK {
    char   Label[ 32 ];
    uint64 CycleCounter_Start;
    uint64 CycleCounter_End;
};

#define PROFILE_BLOCK_MAX_COUNT  ( 16 )
struct PROFILE_STATE {
    boo32         ProfilingIsActive;
    uint32       nProFileBlock;
    PROFILE_BLOCK ProFileBlock[ PROFILE_BLOCK_MAX_COUNT ];
};

//----------
// CONVEYOR
//----------

#define CONVEYOR_ART_THICKNESS  ( TILE_HEIGHT * 1.0f )
#define CONVEYOR_ACCEL_THICKNESS_HORIZONTAL  ( TILE_HEIGHT * 1.0f )
#define CONVEYOR_ACCEL_THICKNESS_VERTICAL    ( PLAYER_HALF_WIDTH + TILE_WIDTH * 0.1f )

#define CONVEYOR_HORIZONTAL_SPEED     ( 3.0f )
#define CONVEYOR_HORIZONTAL_FRICTION  ( 20.0f )
#define CONVEYOR_VERTICAL_SPEED       ( 6.0f )
#define CONVEYOR_VERTICAL_FRICTION    ( 20.0f )

enum CONVEYOR_TYPE {
    ConveyorType_OnLeft,
    ConveyorType_OnBottom,
    ConveyorType_OnRight,
    ConveyorType_OnTop,
    
    ConveyorType_Count,
};

struct CONVEYOR_RESULT {
    boo32 IsOnConveyor;
    vec2  ConveyorAccel;
};

struct CONVEYOR {
    rect Bound_Art;
    rect Bound_Mvt;
    vec2 Accel;
};

#define CONVEYOR_MAX_COUNT  ( 128 )
struct CONVEYOR_STATE {
    CONVEYOR_TYPE ActiveType;
    
    UINT32_PAIR Type[ ConveyorType_Count ];
    
    uint32  nConveyor;
    CONVEYOR Conveyor[ CONVEYOR_MAX_COUNT ];
};

enum EDITOR__CONVEYOR_TYPE {
    EditorConveyorType_VerticalOnLeftWall,
    EditorConveyorType_VerticalOnRightWall,
    EditorConveyorType_HorizontalMoveLeft,
    EditorConveyorType_HorizontalMoveRight,
    
    EditorConveyorType_Count,
};

struct EDITOR__CONVEYOR {
    EDITOR__CONVEYOR_TYPE Type;
    rect                  Bound;
};

struct EDITOR__CONVEYOR_STATE {
    uint32          nConveyor;
    EDITOR__CONVEYOR Conveyor[ CONVEYOR_MAX_COUNT ];
};

//----------
// SPIKES
//----------

enum SPIKES_TYPE {
    SpikesType_PointLeft,
    SpikesType_PointDown,
    SpikesType_PointRight,
    SpikesType_PointUp,
    
    SpikesType_Count,
};

struct SPIKES {
    vec2  Art_Origin;
    flo32 Art_Radians;
    rect  Bound_Dmg;
};

#define SPIKES_MAX_COUNT  ( 512 )
struct SPIKES_STATE {
    uint32 nSpikes;
    SPIKES  Spikes[ SPIKES_MAX_COUNT ];
};

struct EDITOR__SPIKES {
    SPIKES_TYPE Type;
    rect        Bound;
};

struct EDITOR__SPIKES_STATE {
    SPIKES_TYPE    ActiveType;
    uint32        nSpikes;
    EDITOR__SPIKES Spikes[ SPIKES_MAX_COUNT ];
};

//----------
// SCAFFOLD
//----------

#define SCAFFOLD_ART_THICKNESS  ( TILE_HEIGHT * 0.5f )

struct SCAFFOLD_DOWN_RESULT {
    boo32  IsOnScaffoldDown;
    uint32 iScaffold;
};

struct SCAFFOLD {
    rect Bound_Art;
    rect Bound_JumpUp;
    rect Bound_JumpDown;
    vec4 CollisionEdge;
};

#define SCAFFOLD_MAX_COUNT  ( 128 )
struct SCAFFOLD_STATE {
    uint32  nScaffold;
    SCAFFOLD Scaffold[ SCAFFOLD_MAX_COUNT ];
};

struct EDITOR__SCAFFOLD {
    rect Bound;
};

struct EDITOR__SCAFFOLD_STATE {
    uint32          nScaffold;
    EDITOR__SCAFFOLD Scaffold[ SCAFFOLD_MAX_COUNT ];
};

//----------
// RUNNER
//----------

#define RUNNER_WIDTH       ( TILE_WIDTH  * 1.2f )
#define RUNNER_HALF_WIDTH  ( RUNNER_WIDTH * 0.5f )
#define RUNNER_HEIGHT      ( TILE_HEIGHT * 1.8f )
#define RUNNER_DIM         ( Vec2( RUNNER_WIDTH, RUNNER_HEIGHT ) )

#define RUNNER_SPEED    ( 2.0f  )
#define RUNNER_GRAVITY  ( 26.0f )

#define RUNNER_SPAWN_CAMERA_OFFSET    ( TILE_WIDTH *  2.0f )
#define RUNNER_DESPAWN_CAMERA_OFFSET  ( TILE_WIDTH * 12.0f )
#define RUNNER_COLLISION_OFFSET  ( Rect( RUNNER_HALF_WIDTH, 0.0f, RUNNER_HALF_WIDTH, RUNNER_HEIGHT ) )

struct RUNNER {
    vec2 Position;
    vec2 Velocity;
    vec4 Color;
};

#define RUNNER_MAX_COUNT  ( 10 )
struct RUNNER_STATE {
    boo32  PlayerIsMovingRight;
    flo32  Stationary_Timer;
    uint32 Spawn_CameraXCell;
    flo32  Spawn_Timer;
    flo32  Spawn_TargetTime;
    
    flo32  Race_Timer;
    flo32  Race_StartX;
    flo32  Race_FinishX;
    
    uint32 nRunner;
    RUNNER  Runner[ RUNNER_MAX_COUNT ];
};

//----------
// SEESAW
//----------

#define SEESAW_BLOCK_WIDTH  ( TILE_WIDTH  * 2.0f )
#define SEESAW_BLOCK_HEIGHT ( TILE_HEIGHT * 1.0f )
#define SEESAW_BLOCK_DIM    ( Vec2( SEESAW_BLOCK_WIDTH, SEESAW_BLOCK_HEIGHT ) )
#define SEESAW_DIM          ( Vec2( SEESAW_BLOCK_WIDTH * 2.0f, SEESAW_BLOCK_HEIGHT ) )

//#define SEESAW_LAUNCH_SPEED  ( 18.25f )
#define SEESAW_LAUNCH_SPEED  ( 23.75f )

struct SEESAW {
    boo32 BlockAIsUp;
    rect  BlockA;
    rect  BlockB;
};

#define SEESAW_MAX_COUNT  ( 128 )
struct SEESAW_STATE {
    uint32 nSeesaw;
    SEESAW  Seesaw[ SEESAW_MAX_COUNT ];
};

struct EDITOR__SEESAW {
    rect Bound;
};

struct EDITOR__SEESAW_STATE {
    uint32        nSeesaw;
    EDITOR__SEESAW Seesaw[ SEESAW_MAX_COUNT ];
};

//----------
// BOULDER
//----------

#define BOULDER_RADIUS  ( TILE_WIDTH * 0.8f )
#define BOULDER_DIM     ( Vec2( BOULDER_RADIUS, BOULDER_RADIUS ) * 2.0f )

#define BOULDER_AIR_FRICTION     ( 0.1f )
#define BOULDER_GROUND_FRICTION  ( 4.0f )
#define BOULDER_GRAVITY          ( 30.0f )
#define BOULDER_THROW_SPEED      ( 12.0f )

struct BOULDER {
    vec2 Position;
    vec2 Velocity;
};

#define BOULDER_MAX_COUNT  ( 128 )
struct BOULDER_STATE {
    uint32 nBoulder;
    BOULDER Boulder[ BOULDER_MAX_COUNT ];
};

struct EDITOR__BOULDER {
    vec2 Position;
};

struct EDITOR__BOULDER_STATE {
    uint32         nBoulder;
    EDITOR__BOULDER Boulder[ BOULDER_MAX_COUNT ];
};

//----------
// STOMPER
//----------

#define STOMPER_WIDTH       ( TILE_WIDTH  * 1.0f )
#define STOMPER_HALF_WIDTH  ( STOMPER_WIDTH * 0.5f )
#define STOMPER_HEIGHT      ( TILE_HEIGHT * 0.9f )
#define STOMPER_DIM         ( Vec2( STOMPER_WIDTH, STOMPER_HEIGHT ) )
#define STOMPER_COLLISION_OFFSET  ( Rect( STOMPER_HALF_WIDTH * 1.05f, 0.0f, STOMPER_HALF_WIDTH * 1.05f, STOMPER_HEIGHT ) )

#define STOMPER_STOMP_SPEED      ( 18.0f )
#define STOMPER_MOVE_SPEED       ( Vec2( 6.0f, 4.0f ) )
#define STOMPER_AIR_FRICTION     ( 4.0f  )
#define STOMPER_GROUND_FRICTION  ( 10.0f )
#define STOMPER_GRAVITY          ( 30.0f )

#define STOMPER_JUMP_TARGET_TIME   ( 0.5f )
#define STOMPER_MOVE_TARGET_TIME   ( 0.5f )
#define STOMPER_IDLE_TARGET_TIME   ( 2.0f )
#define STOMPER_DELAY_TARGET_TIME  ( 1.0f ) 

#define STOMPER_ATTACK_MAX_DIST   ( TILE_WIDTH  * 4.0f )
#define STOMPER_STOMP_HEIGHT      ( TILE_HEIGHT * 5.0f )

enum STOMPER_MODE {
    StomperMode_Idle,
    StomperMode_Move,
    StomperMode_Jump,
    StomperMode_Delay,
    StomperMode_Stomp,
};

struct STOMPER {
    STOMPER_MODE Mode;
    
    vec2 Position;
    vec2 Velocity;
    
    flo32 Move_xDir;
    flo32 Timer;
    
    boo32  CanHop;
    uint32 iHop;
    uint32 nHop;
    
    vec2   JumpVel;
};

#define STOMPER_MAX_COUNT  ( 128 )
struct STOMPER_STATE {
    uint32 nStomper;
    STOMPER  Stomper[ STOMPER_MAX_COUNT ];
};

//----------
// EXERCISE_MINIBOSS01
//----------

#define EXERCISE_MINIBOSS01_WIDTH   ( TILE_WIDTH  * 4.0f )
#define EXERCISE_MINIBOSS01_HEIGHT  ( TILE_HEIGHT * 3.0f )
#define EXERCISE_MINIBOSS01_HALF_WIDTH   ( EXERCISE_MINIBOSS01_WIDTH  * 0.5f )
#define EXERCISE_MINIBOSS01_HALF_HEIGHT  ( EXERCISE_MINIBOSS01_HEIGHT * 0.5f )
#define EXERCISE_MINIBOSS01_DIM     ( Vec2( EXERCISE_MINIBOSS01_WIDTH, EXERCISE_MINIBOSS01_HEIGHT ) )

#define EXERCISE_MINIBOSS01_COLLISION_OFFSET  ( Rect( EXERCISE_MINIBOSS01_HALF_WIDTH, EXERCISE_MINIBOSS01_HALF_HEIGHT, EXERCISE_MINIBOSS01_HALF_WIDTH, EXERCISE_MINIBOSS01_HALF_HEIGHT ) )
#define EXERCISE_MINIBOSS01_DAMAGE_PLAYER_DIM       ( EXERCISE_MINIBOSS01_DIM )
#define EXERCISE_MINIBOSS01_DAMAGE_PLAYER_HALF_DIM  ( EXERCISE_MINIBOSS01_DIM * 0.5f )

#define EXERCISE_MINIBOSS01_MOVE_FRICTION  ( 1.0f )
#define EXERCISE_MINIBOSS01_MOVE_SPEED     ( 6.0f )

#define EXERCISE_MINIBOSS01_STOMP_SPEED        ( 56.0f )
#define EXERCISE_MINIBOSS01_STOMP_TARGET_TIME  (  8.0f )

#define EXERCISE_MINIBOSS01_DEATH_TARGET_TIME  ( 2.0f )

#define EXERCISE_MINIBOSS01_MOVE_TO_STOMP_TARGET_TIME      ( 0.5f )
#define EXERCISE_MINIBOSS01_HOLD_BEFORE_STOMP_TARGET_TIME  ( 0.5f )
#define EXERCISE_MINIBOSS01_HOLD_AFTER_STOMP_TARGET_TIME   ( 0.5f )

enum EXERCISE_MINIBOSS01_MODE {
    ExerciseMiniBoss01Mode_Move,
    ExerciseMiniBoss01Mode_MoveToStomp,
    ExerciseMiniBoss01Mode_HoldBeforeStomp,
    ExerciseMiniBoss01Mode_Stomp,
    ExerciseMiniBoss01Mode_PostStomp,
    ExerciseMiniBoss01Mode_IsDying,
    ExerciseMiniBoss01Mode_IsDead,
};

struct EXERCISE_MINIBOSS01 {
    boo32 IsActive;
    
    EXERCISE_MINIBOSS01_MODE Mode;
    
    vec2 Position;
    vec2 Velocity;
    
    uint32 nDest;
    uint32 iDest;
    vec2    Dest[ 2 ];
    
    flo32  Timer;
    uint32 Health;
};

//----------
// CYCLE_BLOCK
//----------

#define CYCLE_BLOCK_MOVE_TARGET_TIME  ( 48.0f / 60.0f )
#define CYCLE_BLOCK_HOLD_TARGET_TIME  ( 48.0f / 60.0f )

struct CYCLE_BLOCK {
    vec2  Position;
    vec2  Dim;
    vec2  MoveVector;
    
    vec2  PosA;
    vec2  PosB;
    flo32 Timer;
};

#define CYCLE_BLOCK_MAX_COUNT  ( 128 )
struct CYCLE_BLOCK_STATE {
    uint32     nCycleBlock;
    CYCLE_BLOCK CycleBlock[ CYCLE_BLOCK_MAX_COUNT ];
};

struct EDITOR__CYCLE_BLOCK {
    rect Bound;
    vec2 Dest;
};

struct EDITOR__CYCLE_BLOCK_STATE {
    uint32             nCycleBlock;
    EDITOR__CYCLE_BLOCK CycleBlock[ CYCLE_BLOCK_MAX_COUNT ];
};

//----------
// EXERCISE BOSS
//----------


#define EXERCISE_BOSS_WIDTH        ( TILE_WIDTH  * 3.5f )
#define EXERCISE_BOSS_HEIGHT       ( TILE_HEIGHT * 4.0f )
#define EXERCISE_BOSS_HALF_WIDTH   ( EXERCISE_BOSS_WIDTH  * 0.5f )
#define EXERCISE_BOSS_HALF_HEIGHT  ( EXERCISE_BOSS_HEIGHT * 0.5f )
#define EXERCISE_BOSS_DIM          ( Vec2( EXERCISE_BOSS_WIDTH, EXERCISE_BOSS_HEIGHT ) )
#define EXERCISE_BOSS_HALF_DIM     ( EXERCISE_BOSS_DIM * 0.5f )

#define EXERCISE_BOSS_COLLISION_OFFSET  ( Rect( EXERCISE_BOSS_HALF_WIDTH, 0.0f, EXERCISE_BOSS_HALF_WIDTH, EXERCISE_BOSS_HEIGHT ) )
#define EXERCISE_BOSS_DAMAGE_PLAYER_DIM       ( EXERCISE_BOSS_DIM )
#define EXERCISE_BOSS_DAMAGE_PLAYER_HALF_DIM  ( EXERCISE_BOSS_DIM * 0.5f )

#define EXERCISE_BOSS_MOVE_FRICTION  ( 1.0f )
#define EXERCISE_BOSS_MOVE_SPEED     ( 6.0f )

#define EXERCISE_BOSS_STOMP_SPEED        ( 56.0f )
#define EXERCISE_BOSS_STOMP_TARGET_TIME  (  8.0f )

#define EXERCISE_BOSS_DEATH_TARGET_TIME  ( 2.0f )

#define EXERCISE_BOSS_MOVE_TO_STOMP_TARGET_TIME      ( 0.5f )
#define EXERCISE_BOSS_HOLD_BEFORE_STOMP_TARGET_TIME  ( 0.5f )
#define EXERCISE_BOSS_HOLD_AFTER_STOMP_TARGET_TIME   ( 0.5f )

#define EXERCISE_BOSS_GRAVITY       ( 30.0f )
#define EXERCISE_BOSS_RUN_SPEED     (  8.0f )
#define EXERCISE_BOSS_RUN_FRICTION  ( 10.0f )

#define EXERCISE_BOSS_JUMP_SPEED    ( 20.0f )

#define EXERCISE_BOSS_HEALTH   ( 20 )

enum EXERCISE_BOSS_MODE {
    ExerciseBossMode_Run,
    ExerciseBossMode_Move,
    ExerciseBossMode_MoveToStomp,
    ExerciseBossMode_HoldBeforeStomp,
    ExerciseBossMode_Stomp,
    ExerciseBossMode_PostStomp,
    ExerciseBossMode_IsDying,
    ExerciseBossMode_IsDead,
};

struct EXERCISE_BOSS {
    boo32 IsActive;
    
    EXERCISE_BOSS_MODE Mode;
    
    vec2  Position;
    vec2  Velocity;
    boo32 RunLeft;
    
    flo32 Jump_Timer;
    flo32 Jump_TargetTime;
    
    flo32  Timer;
    uint32 Health;
    uint32 MaxHealth;
};

//----------
// PUNCHER
//----------

#define PUNCHER_HEALTH  ( 2 )

#define PUNCHER_WIDTH       ( TILE_WIDTH  * 1.2f )
#define PUNCHER_HALF_WIDTH  ( PUNCHER_WIDTH * 0.5f )
#define PUNCHER_HEIGHT      ( TILE_HEIGHT * 1.9f )
#define PUNCHER_DIM         ( Vec2( PUNCHER_WIDTH, PUNCHER_HEIGHT ) )

#define PUNCHER_BARBELL_OFFSET_X  ( TILE_WIDTH  * 0.25f )
#define PUNCHER_BARBELL_OFFSET_Y  ( TILE_HEIGHT * 1.35f )

#define PUNCHER_ARM_DIM       ( PUNCHER_DIM * Vec2( 1.7f, 0.6f ) )
#define PUNCHER_ARM_SIDE_DIM  ( PUNCHER_DIM * Vec2( 0.5f, 0.6f ) )
#define PUNCHER_FIST_DIM      ( TILE_DIM * 0.75f )
#define PUNCHER_PUNCH_ART_OFFSET          ( Vec2( TILE_WIDTH * 0.7f, TILE_HEIGHT * 1.0f ) )
#define PUNCHER_PUNCH_ART_VIBRATE_OFFSET  ( TILE_WIDTH * 0.15f )

#define PUNCHER_PUNCH_COLLISION_DIM       ( Vec2( TILE_WIDTH * 2.3f, PUNCHER_HEIGHT * 0.9f ) )

#define PUNCHER_ALERT_DIM          ( Vec2( TILE_WIDTH * 14.4f, PUNCHER_HEIGHT ) )
#define PUNCHER_SHOULD_PUNCH_DIM   ( Vec2( TILE_WIDTH * 2.6f, PUNCHER_HEIGHT ) )

#define PUNCHER_PUNCH_TARGET_TIME           ( 48.0f / 60.0f )
#define PUNCHER_PUNCH_COOLDOWN_TARGET_TIME  ( 72.0f / 60.0f )

#define PUNCHER_STUNNED_TARGET_TIME  ( 180.0f / 60.0f )

#define PUNCHER_DYING_TARGET_TIME    ( 180.0f / 60.0f )

#define PUNCHER_MOVE_SPEED        ( 3.5f )
#define PUNCHER_MOVE_FRICTION     ( 8.0f )
#define PUNCHER_STUNNED_FRICTION  ( 1.0f )
#define PUNCHER_GRAVITY           ( 26.0f )
#define PUNCHER_THROW_SPEED       ( 12.0f )

#define PUNCHER_SPAWN_CAMERA_OFFSET    ( TILE_WIDTH *  2.0f )
#define PUNCHER_DESPAWN_CAMERA_OFFSET  ( TILE_WIDTH * 12.0f )
#define PUNCHER_COLLISION_OFFSET  ( Rect( PUNCHER_HALF_WIDTH, 0.0f, PUNCHER_HALF_WIDTH, PUNCHER_HEIGHT ) )

#define PUNCHER_BARBELL_THROW_TARGET_TIME  ( 30.0f / 60.0f )

#define PUNCHER_REPEL_MAX_STRENGTH  ( 4.0f )
#define PUNCHER_REPEL_MAX_DISTANCE  ( TILE_WIDTH * 0.5f )

enum PUNCHER_DRAW_MODE {
    PuncherDrawMode_Idle,
    PuncherDrawMode_MoveLeft,
    PuncherDrawMode_MoveRight,
};

struct PUNCHER {
    boo32 IsAggressive;
    vec2  Position;
    vec2  Velocity;
    vec2  Repel;
    
    uint32 Health;
    
    boo32 Punch_IsCharging;
    flo32 Punch_Timer;
    flo32 Punch_CooldownTimer;
    boo32 Punch_DoPunch;
    boo32 Punch_DoDraw;
    flo32 Punch_ArtTimer;
    vec2  Punch_ArtPos;
    boo32 Punch_FaceLeft;
    
    boo32 IsStunned;
    flo32 IsStunned_Timer;
    
    flo32 CanBeStunned_Timer;
    boo32 CanStunOtherPunchers;
    boo32 CanSeePlayer;
    
    boo32 Barbell_CanThrow;
    flo32 Barbell_ThrowTimer;
    
    PUNCHER_DRAW_MODE DrawMode;
};

#define PUNCHER_MAX_COUNT  ( 16 )
struct PUNCHER_STATE {
    uint32 nPuncher;
    PUNCHER  Puncher[ PUNCHER_MAX_COUNT ];
};

struct EDITOR__PUNCHER {
    vec2 Position;
};

struct EDITOR__PUNCHER_STATE {
    uint32         nPuncher;
    EDITOR__PUNCHER Puncher[ PUNCHER_MAX_COUNT ];
};

//----------
// TEXT BOX
//----------

enum TEXT_BOX_SPEAKER_ID {
    TextBoxSpeakerID_None,
    
    TextBoxSpeakerID_Player,
    TextBoxSpeakerID_Coach,
};

#define TEXT_BOX_CHAR_MAX_COUNT  ( 256 )
struct TEXT_BOX {
    TEXT_BOX_SPEAKER_ID SpeakerID;
    char                Text[ TEXT_BOX_CHAR_MAX_COUNT + 1 ];
    int32               NextText;
};

#define TEXT_BOX_MAX_COUNT  ( 128 )
struct TEXT_BOX_STATE {
    boo32         Show;
    int32         Show_iTextBox;
    
    uint32       nTextBox;
    TEXT_BOX      TextBox[ TEXT_BOX_MAX_COUNT ];
};

//----------
// PUSH BLOCK
//----------

#define PUSH_BLOCK_MOVE_TARGET_TIME  ( 48.0f / 60.0f )
#define PUSH_BLOCK_HOLD_TARGET_TIME  ( 48.0f / 60.0f )

#define PUSH_BLOCK_MOVE_WALK_SPEED  (  2.0f )
#define PUSH_BLOCK_MOVE_RUN_SPEED   (  4.0f )
#define PUSH_BLOCK_START_FRICTION   (  2.0f )
#define PUSH_BLOCK_STOP_FRICTION    ( 26.0f )
#define PUSH_BLOCK_GRAVITY          ( 30.0f )

struct PUSH_BLOCK {
    vec2  Position;
    vec2  Velocity;
    vec2  Dim;
    flo32 DirX;
    boo32 IsOnGround;
};

#define PUSH_BLOCK_MAX_COUNT  ( 128 )
struct PUSH_BLOCK_STATE {
    uint32    nPushBlock;
    PUSH_BLOCK PushBlock[ PUSH_BLOCK_MAX_COUNT ];
};

struct EDITOR__PUSH_BLOCK {
    rect Bound;
};

struct EDITOR__PUSH_BLOCK_STATE {
    uint32            nPushBlock;
    EDITOR__PUSH_BLOCK PushBlock[ PUSH_BLOCK_MAX_COUNT ];
};

//----------
// CRUMBLE_PLATFORM
//----------

#define COLLAPSE_PLATFORM_COLLAPSE_TARGET_TIME   (  80.0f / 60.0f )
#define COLLAPSE_PLATFORM_DOWN_TARGET_TIME       ( 200.0f / 60.0f )

struct COLLAPSE_PLATFORM {
    rect  Bound;
    boo32 Collision_IsActive;
    boo32 Collapse_IsActive;
    flo32 Collapse_Timer;
    rect  Collapse_Bound;
    flo32 Art_Radians;
};

#define COLLAPSE_PLATFORM_MAX_COUNT  ( 128 )
struct COLLAPSE_PLATFORM_STATE {
    uint32           nCollapsePlatform;
    COLLAPSE_PLATFORM CollapsePlatform[ COLLAPSE_PLATFORM_MAX_COUNT ];
};

struct EDITOR__COLLAPSE_PLATFORM {
    rect Bound;
};

struct EDITOR__COLLAPSE_PLATFORM_STATE {
    uint32                   nCollapsePlatform;
    EDITOR__COLLAPSE_PLATFORM CollapsePlatform[ COLLAPSE_PLATFORM_MAX_COUNT ];
};

//----------
// BREAK BLOCK
//----------

#define BREAK_BLOCK_GRAVITY  ( 20.0f )

struct BREAK_BLOCK {
    vec2 Position;
    vec2 Velocity;
    vec2 Dim;
};

#define BREAK_BLOCK_MAX_COUNT  ( 128 )
struct BREAK_BLOCK_STATE {
    uint32    nBreakBlock;
    BREAK_BLOCK BreakBlock[ BREAK_BLOCK_MAX_COUNT ];
};

struct EDITOR__BREAK_BLOCK {
    rect Bound;
};

struct EDITOR__BREAK_BLOCK_STATE {
    uint32             nBreakBlock;
    EDITOR__BREAK_BLOCK BreakBlock[ BREAK_BLOCK_MAX_COUNT ];
};

//----------
// EXERCISE_MINIBOSS02
//----------

#define EXERCISE_MINIBOSS02_WIDTH   ( TILE_WIDTH  * 4.0f )
#define EXERCISE_MINIBOSS02_HEIGHT  ( TILE_HEIGHT * 3.0f )
#define EXERCISE_MINIBOSS02_HALF_WIDTH   ( EXERCISE_MINIBOSS02_WIDTH  * 0.5f )
#define EXERCISE_MINIBOSS02_HALF_HEIGHT  ( EXERCISE_MINIBOSS02_HEIGHT * 0.5f )
#define EXERCISE_MINIBOSS02_DIM     ( Vec2( EXERCISE_MINIBOSS02_WIDTH, EXERCISE_MINIBOSS02_HEIGHT ) )

#define EXERCISE_MINIBOSS02_COLLISION_OFFSET  ( Rect( EXERCISE_MINIBOSS02_HALF_WIDTH, EXERCISE_MINIBOSS02_HALF_HEIGHT, EXERCISE_MINIBOSS02_HALF_WIDTH, EXERCISE_MINIBOSS02_HALF_HEIGHT ) )
#define EXERCISE_MINIBOSS02_DAMAGE_PLAYER_DIM       ( EXERCISE_MINIBOSS02_DIM )
#define EXERCISE_MINIBOSS02_DAMAGE_PLAYER_HALF_DIM  ( EXERCISE_MINIBOSS02_DIM * 0.5f )

#define EXERCISE_MINIBOSS02_MOVE_FRICTION  ( 4.0f )
#define EXERCISE_MINIBOSS02_MOVE_SPEED     ( 6.0f )

#define EXERCISE_MINIBOSS02_STOMP_WARMUP_TARGET_TIME    ( 20.0f / 60.0f )
#define EXERCISE_MINIBOSS02_STOMP_COOLDOWN_TARGET_TIME  ( 20.0f / 60.0f )
#define EXERCISE_MINIBOSS02_STOMP_TARGET_TIME  ( 1.0f )

#define EXERCISE_MINIBOSS02_SPIKES_WARMUP_TARGET_TIME    ( 40.0f / 60.0f )
#define EXERCISE_MINIBOSS02_SPIKES_COOLDOWN_TARGET_TIME  ( 24.0f / 60.0f )
#define EXERCISE_MINIBOSS02_SPIKES_TARGET_TIME           ( 60.0f / 60.0f )
#define EXERCISE_MINIBOSS02_SPIKES_HEIGHT                ( TILE_HEIGHT * 2.0f )

#define EXERCISE_MINIBOSS02_SWEEP_WARMUP_TARGET_TIME  ( 60.0f / 60.0f )
#define EXERCISE_MINIBOSS02_SWEEP_SPEED               ( 28.0f )
#define EXERCISE_MINIBOSS02_SWEEP_FRICTION            ( 3.0f )

#define EXERCISE_MINIBOSS02_MOVE_TO_STOMP_TARGET_TIME      ( 0.5f )
#define EXERCISE_MINIBOSS02_HOLD_BEFORE_STOMP_TARGET_TIME  ( 0.5f )
#define EXERCISE_MINIBOSS02_HOLD_AFTER_STOMP_TARGET_TIME   ( 0.5f )

#define EXERCISE_MINIBOSS02_STAGGERED_TARGET_TIME  ( 14.0f / 60.0f )
#define EXERCISE_MINIBOSS02_STUNNED_DOWN_TARGET_TIME     ( 150.0f / 60.0f )
#define EXERCISE_MINIBOSS02_STUNNED_RECOVER_TARGET_TIME  (  90.0f / 60.0f )
#define EXERCISE_MINIBOSS02_GRAVITY  ( 60.0f )

enum EXERCISE_MINIBOSS02_MODE {
    // show spikes and sweep across floor
    // stomp on player position
    // spawn additional spike creatures
    ExerciseMiniBoss02Mode_Move,
    ExerciseMiniBoss02Mode_MoveToSweep,
    ExerciseMiniBoss02Mode_Sweep,
    ExerciseMiniBoss02Mode_Stomp,
    ExerciseMiniBoss02Mode_Spikes,
    ExerciseMiniBoss02Mode_IsStunned,
    ExerciseMiniBoss02Mode_IsDying,
    ExerciseMiniBoss02Mode_EndEncounter,
};

struct EXERCISE_MINIBOSS02 {
    boo32 IsActive;
    
    EXERCISE_MINIBOSS02_MODE Mode;
    
    vec2 Position;
    vec2 Destination;
    vec2 Velocity;
    
    flo32  Timer;
    uint32 MaxHealth;
    uint32 Health;
    
    boo32  DoBlast;
    boo32  DoStomp;
    uint32 MaxStomp;
    uint32 nStomp;
    
    boo32  DoStun;
    flo32  DoStunX;
    boo32  DoStagger;
    flo32  DoStaggerX;
    uint32 nStagger;
    boo32  IsStaggered;
    flo32  IsStaggered_Timer;
};

//----------
// POPPER
//----------

#define POPPER_WIDTH        ( TILE_WIDTH  * 1.0f )
#define POPPER_HALF_WIDTH   ( POPPER_WIDTH * 0.5f )
#define POPPER_HEIGHT       ( TILE_HEIGHT * 0.9f )
#define POPPER_HALF_HEIGHT  ( POPPER_HEIGHT * 0.5f )
#define POPPER_DIM          ( Vec2( POPPER_WIDTH, POPPER_HEIGHT ) )

#define POPPER_MOVE_SPEED       ( 1.25f )
#define POPPER_MOVE_FRICTION    ( 8.0f )
#define POPPER_AIR_FRICTION     ( 0.25f  )
#define POPPER_GRAVITY          ( 28.0f )
#define POPPER_THROW_SPEED      ( 22.0f )

#define POPPER_STUNNED_TARGET_TIME   ( 180.0f / 60.0f )

enum POPPER_MODE {
    PopperMode_Move,
    PopperMode_IsStunned,
    PopperMode_IsThrown,
    PopperMode_IsPopping,
    PopperMode_IsDead,
};

struct POPPER {
    POPPER_MODE Mode;
    
    vec2  Position;
    vec2  Velocity;
    flo32 DirX;
    
    flo32 Timer;
    flo32 Charge;
};

#define POPPER_MAX_COUNT  ( 128 )
struct POPPER_STATE {
    uint32 nPopper;
    POPPER  Popper[ POPPER_MAX_COUNT ];
};

struct EDITOR__POPPER {
    vec2 Position;
};

struct EDITOR__POPPER_STATE {
    uint32        nPopper;
    EDITOR__POPPER Popper[ POPPER_MAX_COUNT ];
};

//----------
// EXERCISE_MINIBOSS03
//----------

#define EXERCISE_MINIBOSS03_STUNNED_WIDTH   ( TILE_WIDTH  * 12.0f )
#define EXERCISE_MINIBOSS03_STUNNED_HEIGHT  ( TILE_HEIGHT *  6.0f )
#define EXERCISE_MINIBOSS03_STUNNED_DIM     ( Vec2( EXERCISE_MINIBOSS03_STUNNED_WIDTH, EXERCISE_MINIBOSS03_STUNNED_HEIGHT ) )

#define EXERCISE_MINIBOSS03_SPAWN_WIDTH   ( TILE_WIDTH  * 10.0f )
#define EXERCISE_MINIBOSS03_SPAWN_HEIGHT  ( TILE_HEIGHT * 14.0f )
#define EXERCISE_MINIBOSS03_SPAWN_DIM     ( Vec2( EXERCISE_MINIBOSS03_SPAWN_WIDTH, EXERCISE_MINIBOSS03_SPAWN_HEIGHT ) )

#define EXERCISE_MINIBOSS03_SQUASH_WARMUP_TARGET_TIME  ( 60.0f  / 60.0f )
#define EXERCISE_MINIBOSS03_SQUASH_DIM  ( EXERCISE_MINIBOSS03_SPAWN_DIM )

#define EXERCISE_MINIBOSS03_PUSH_WALK_SPEED  ( 3.0f )
#define EXERCISE_MINIBOSS03_PUSH_RUN_SPEED   ( 5.0f )
#define EXERCISE_MINIBOSS03_PUSH_FRICTION    ( 2.0f )



#define EXERCISE_MINIBOSS03_WIDTH   ( TILE_WIDTH  * 4.0f )
#define EXERCISE_MINIBOSS03_HEIGHT  ( TILE_HEIGHT * 3.0f )
#define EXERCISE_MINIBOSS03_HALF_WIDTH   ( EXERCISE_MINIBOSS03_WIDTH  * 0.5f )
#define EXERCISE_MINIBOSS03_HALF_HEIGHT  ( EXERCISE_MINIBOSS03_HEIGHT * 0.5f )
#define EXERCISE_MINIBOSS03_DIM     ( Vec2( EXERCISE_MINIBOSS03_WIDTH, EXERCISE_MINIBOSS03_HEIGHT ) )

#define EXERCISE_MINIBOSS03_COLLISION_OFFSET  ( Rect( EXERCISE_MINIBOSS03_HALF_WIDTH, EXERCISE_MINIBOSS03_HALF_HEIGHT, EXERCISE_MINIBOSS03_HALF_WIDTH, EXERCISE_MINIBOSS03_HALF_HEIGHT ) )
#define EXERCISE_MINIBOSS03_DAMAGE_PLAYER_DIM       ( EXERCISE_MINIBOSS03_DIM )
#define EXERCISE_MINIBOSS03_DAMAGE_PLAYER_HALF_DIM  ( EXERCISE_MINIBOSS03_DIM * 0.5f )

#define EXERCISE_MINIBOSS03_MOVE_FRICTION  ( 4.0f )
#define EXERCISE_MINIBOSS03_MOVE_SPEED     ( 2.0f )

#define EXERCISE_MINIBOSS03_STOMP_WARMUP_TARGET_TIME    ( 20.0f / 60.0f )
#define EXERCISE_MINIBOSS03_STOMP_COOLDOWN_TARGET_TIME  ( 20.0f / 60.0f )
#define EXERCISE_MINIBOSS03_STOMP_TARGET_TIME  ( 1.0f )

#define EXERCISE_MINIBOSS03_SPIKES_WARMUP_TARGET_TIME    ( 40.0f / 60.0f )
#define EXERCISE_MINIBOSS03_SPIKES_COOLDOWN_TARGET_TIME  ( 24.0f / 60.0f )
#define EXERCISE_MINIBOSS03_SPIKES_TARGET_TIME           ( 60.0f / 60.0f )
#define EXERCISE_MINIBOSS03_SPIKES_HEIGHT                ( TILE_HEIGHT * 2.0f )

#define EXERCISE_MINIBOSS03_SWEEP_WARMUP_TARGET_TIME  ( 60.0f / 60.0f )
#define EXERCISE_MINIBOSS03_SWEEP_SPEED               ( 28.0f )
#define EXERCISE_MINIBOSS03_SWEEP_FRICTION            ( 3.0f )

#define EXERCISE_MINIBOSS03_MOVE_TO_STOMP_TARGET_TIME      ( 0.5f )
#define EXERCISE_MINIBOSS03_HOLD_BEFORE_STOMP_TARGET_TIME  ( 0.5f )
#define EXERCISE_MINIBOSS03_HOLD_AFTER_STOMP_TARGET_TIME   ( 0.5f )

#define EXERCISE_MINIBOSS03_STAGGERED_TARGET_TIME  ( 14.0f / 60.0f )
#define EXERCISE_MINIBOSS03_STUNNED_DOWN_TARGET_TIME     ( 150.0f / 60.0f )
#define EXERCISE_MINIBOSS03_STUNNED_RECOVER_TARGET_TIME  (  90.0f / 60.0f )
#define EXERCISE_MINIBOSS03_GRAVITY  ( 40.0f )

enum EXERCISE_MINIBOSS03_MODE {
    // show spikes and sweep across floor
    // stomp on player position
    // spawn additional spike creatures
    
    ExerciseMiniBoss03Mode_Spawn,
    ExerciseMiniBoss03Mode_Squash,
    ExerciseMiniBoss03Mode_Move,
    ExerciseMiniBoss03Mode_MoveToSweep,
    ExerciseMiniBoss03Mode_Sweep,
    ExerciseMiniBoss03Mode_Stomp,
    ExerciseMiniBoss03Mode_Spikes,
    ExerciseMiniBoss03Mode_IsStunned,
    ExerciseMiniBoss03Mode_IsDying,
    ExerciseMiniBoss03Mode_EndEncounter,
};

struct EXERCISE_MINIBOSS03 {
    boo32 IsActive;
    
    EXERCISE_MINIBOSS03_MODE Mode;
    
    vec2 Position;
    vec2 Destination;
    vec2 Velocity;
    vec2 Dim;
    
    flo32  Timer;
    uint32 MaxHealth;
    uint32 Health;
    
    flo32  Push_DirX;
    flo32  Push_MaxSpeed;
    flo32  Push_EndTimer;
    
    boo32  DoBlast;
    boo32  DoStomp;
    uint32 MaxStomp;
    uint32 nStomp;
    
    boo32  DoStun;
    flo32  DoStunX;
    boo32  DoStagger;
    flo32  DoStaggerX;
    uint32 nStagger;
    boo32  IsStaggered;
    flo32  IsStaggered_Timer;
    
    boo32  DoPop;
};

//----------
// SPOTTER
//----------

#define SPOTTER_RADIUS       ( TILE_WIDTH  * 0.75f )
#define SPOTTER_HALF_WIDTH   ( SPOTTER_RADIUS )
#define SPOTTER_HALF_HEIGHT  ( SPOTTER_RADIUS )
#define SPOTTER_WIDTH        ( SPOTTER_HALF_WIDTH  * 2.0f )
#define SPOTTER_HEIGHT       ( SPOTTER_HALF_HEIGHT * 2.0f )
#define SPOTTER_DIM          ( Vec2( SPOTTER_WIDTH, SPOTTER_HEIGHT ) )

#define SPOTTER_MOVE_SPEED       ( 4.0f )
#define SPOTTER_STUNNED_SPEED    ( 0.25f )
#define SPOTTER_FRICTION         ( 4.0f )

#define SPOTTER_SEE_DIM    ( TILE_DIM * 14.0f )
#define SPOTTER_DRAIN_DIM  ( TILE_DIM * 6.0f )

#define SPOTTER_STAMINA_DRAIN_RATE    ( 2.6f ) // Per Second
#define SPOTTER_STUNNED_TARGET_TIME   ( 180.0f / 60.0f )

enum SPOTTER_MODE {
    SpotterMode_Idle,
    SpotterMode_IsChasing,
    SpotterMode_IsDraining,
    SpotterMode_IsStunned,
    SpotterMode_IsDead,
};

struct SPOTTER {
    SPOTTER_MODE Mode;
    
    vec2  Position;
    vec2  Velocity;
    
    flo32 IsStunned_Timer;
    
    vec2  Pupil_Position;
    flo32 Pupil_Radius;
    flo32 Pupil_Timer;
};

#define SPOTTER_MAX_COUNT  ( 128 )
struct SPOTTER_STATE {
    uint32  nSpotter;
    SPOTTER  Spotter[ SPOTTER_MAX_COUNT ];
};

struct EDITOR__SPOTTER {
    vec2 Position;
};

struct EDITOR__SPOTTER_STATE {
    uint32         nSpotter;
    EDITOR__SPOTTER Spotter[ SPOTTER_MAX_COUNT ];
};

//----------
// HEDGEHOG
//----------

#define HEDGEHOG_WIDTH        ( TILE_WIDTH  * 1.0f )
#define HEDGEHOG_HEIGHT       ( TILE_HEIGHT * 1.0f )
#define HEDGEHOG_HALF_WIDTH   ( HEDGEHOG_WIDTH  * 0.5f )
#define HEDGEHOG_HALF_HEIGHT  ( HEDGEHOG_HEIGHT * 0.5f )
#define HEDGEHOG_DIM          ( Vec2( HEDGEHOG_WIDTH, HEDGEHOG_HEIGHT ) )

#define HEDGEHOG_GROUND_FRICTION  (  8.0f )
#define HEDGEHOG_AIR_FRICTION     (  2.0f )
#define HEDGEHOG_GRAVITY          ( 30.0f )

struct HEDGEHOG {
    vec2 Position;
    vec2 Velocity;
};

#define HEDGEHOG_MAX_COUNT  ( 128 )
struct HEDGEHOG_STATE {
    uint32   nHedgehog;
    HEDGEHOG  Hedgehog[ HEDGEHOG_MAX_COUNT ];
};

//----------
// BARBELL
//----------

#define BARBELL_WIDTH        ( TILE_WIDTH  * 0.5f )
#define BARBELL_HEIGHT       ( TILE_HEIGHT * 0.8f )
#define BARBELL_HALF_WIDTH   ( BARBELL_WIDTH  * 0.5f )
#define BARBELL_HALF_HEIGHT  ( BARBELL_HEIGHT * 0.5f )
#define BARBELL_DIM          ( Vec2( BARBELL_WIDTH, BARBELL_HEIGHT ) )

#define BARBELL_AIR_FRICTION     ( 0.1f )
#define BARBELL_GROUND_FRICTION  ( 6.0f )
#define BARBELL_GRAVITY          ( 30.0f )
#define BARBELL_THROW_SPEED      ( 22.0f )

struct BARBELL {
    vec2   Position;
    vec2   Velocity;
    flo32  Radians;
    
    boo32  CanHurtPlayer;
    boo32  DoKill;
    int32  Skip_iPuncher;
};

#define BARBELL_MAX_COUNT  ( 128 )
struct BARBELL_STATE {
    uint32  nBarbell;
    BARBELL  Barbell[ BARBELL_MAX_COUNT ];
};

struct EDITOR__BARBELL {
    vec2 Position;
};

struct EDITOR__BARBELL_STATE {
    uint32         nBarbell;
    EDITOR__BARBELL Barbell[ BARBELL_MAX_COUNT ];
};

//----------
// MONEY
//----------

#define MONEY_FRICTION      ( 6.0f )
#define MONEY_GRAVITY       ( 30.0f )
#define MONEY_THROW_SPEED   ( 22.0f )

#define MONEY_DIMS  vec2 MoneyDims[] = {\
TILE_DIM * 0.5f,\
TILE_DIM * 0.6f,\
TILE_DIM * 0.6f,\
TILE_DIM * 0.65f,\
TILE_DIM * Vec2( 0.95f, 0.5f ),\
};

#define MONEY_COLORS  vec4 MoneyColors[] = {\
ToColor( 100,  70,  40 ),\
ToColor(  60,  60,  60 ),\
ToColor( 185, 175,  60 ),\
ToColor( 140, 140, 140 ),\
ToColor(  20, 105,  10 ),\
};

enum MONEY_TYPE {
    MoneyType_1,
    MoneyType_5,
    MoneyType_10,
    MoneyType_25,
    MoneyType_100,
    
    MoneyType_Count,
};

struct MONEY {
    MONEY_TYPE Type;
    boo32      HasGravity;
    
    vec2       Position;
    vec2       Velocity;
};

#define MONEY_MAX_COUNT  ( 128 )
struct MONEY_STATE {
    uint32 nMoney;
    MONEY   Money[ MONEY_MAX_COUNT ];
};

struct EDITOR__MONEY {
    MONEY_TYPE Type;
    vec2       Position;
};

struct EDITOR__MONEY_STATE {
    MONEY_TYPE   ActiveType;
    
    uint32       nMoney;
    EDITOR__MONEY Money[ MONEY_MAX_COUNT ];
};