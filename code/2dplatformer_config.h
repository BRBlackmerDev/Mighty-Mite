
//----------
//----------

//#define APP_PermMemorySize _MB( 256 )
#define APP_PermMemorySize   _GB( 1   )
#define APP_TempMemorySize   _MB( 256 )

#define APP_isFullscreen  ( 0 )
#define LEVEL_SAVE_DIRECTORY  ( "../../asset/entity" )
#define ART_SAVE_DIRECTORY   ( "../../asset/art" )
#define AUDIO_SAVE_DIRECTORY   ( "../../asset/audio" )
#define CODE_DIRECTORY       ( "../../code" )

#define START_IN_EDITOR  ( 1 )
#define START_IN_LAYOUT  ( 0 )
#define START_IN_ROOM    ( 0 )
#define START_IN_ROOM_NAME   ( "Top01" )
#define DISABLE_ENEMY_SPAWN  ( 0 )

// NOTE: Currently, our Collision is so good that if an Entity lands perfectly on the Top corner of a Platform, they will be unable to move onto the Platform or drop down. The Collision system will hold them in place. This COLLISION_EPSILON_OFFSET will offset the Entity from a surface after a Collision, so the second iteration/Collision will not occur. This strategy really only works in the circumstance because all the Collision geometry is orthogonal on a Grid.
#define COLLISION_EPSILON_OFFSET            ( TILE_WIDTH * 0.001f )
#define COLLISION_EPSILON_MIN_VALID_LENGTH  ( 0.0001f )
#define COLLISION_MAX_ITERATION             ( 4 )

#define WORLD_DEBUG_TEXT_SCALE  ( Vec2( 0.0175f, 0.0175f ) )

