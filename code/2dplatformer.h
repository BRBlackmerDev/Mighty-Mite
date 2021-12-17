
typedef DIRECTX_12_RENDERER    RENDERER;
typedef DIRECTX_12_RENDER_PASS RENDER_PASS;

struct PLATFORM {
    boo32 IsRunning;
    
    flo32 TargetSec;
    
    KEYBOARD_STATE   Keyboard;
    MOUSE_STATE      Mouse;
    CONTROLLER_STATE Controller;
    
    MEMORY PermMemory;
    MEMORY TempMemory;
    
    AUDIO_SYSTEM AudioSystem;
    
    RENDERER Renderer;
    
    READ_FILE       * ReadFile;
    WRITE_FILE      * WriteFile;
    DOES_FILE_EXIST * DoesFileExist;
    DELETE_FILE     * DeleteFile;
};

//----------
// CAMERA
//----------

struct CAMERA_SHAKE {
    vec2 P;
    vec2 Q;
    
    flo32 Timer;
    flo32 upTime;
    flo32 downTime;
    
    flo32 t;
    flo32 tarGetT;
    
    flo32 strength;
    vec2  offset;
};

struct CAMERA_STATE {
    vec2  Pos;
    vec2  Dim;
    vec2  HalfDim;
    flo32 Scale_Ratio;
    
    boo32 Move_Left;
    boo32 Move_Right;
    boo32 Move_Down;
    boo32 Move_Up;
    boo32 Move_Out;
    boo32 Move_In;
};

//----------
// FILE SAVE/OPEN
//----------

struct FILE_SAVE_OPEN {
    INPUT_STRING  input_string;
    STRING_BUFFER FileList;
    boo32 overwrite;
    boo32 UnsavedChanges;
    boo32 recentSave;
    flo32 Timer;
    boo32 ConfirmNew;
    
    uint32 Select_iSelect;
    int32  Select_iFile;
    
    char  current[ FILE_SAVE_OPEN__FILENAME_MAX_CHAR + 1 ];
};

//----------
// EDITOR PANEL
//----------

struct EDITOR_PANEL {
    rect Bound;
    
    rect Inner;
    vec2 AtPos;
    
    flo32 advanceY;
};

//----------
// EDITOR STATE
//----------

enum EDITOR_MODE {
    EditorMode_Default,
    
    EditorMode_MoveEntity,
    EditorMode_CopyEntity,
    EditorMode_MirrorEntity,
    
    EditorMode_LevelBound,
    EditorMode_Camper,
    EditorMode_Terrain,
    EditorMode_Spikes,
    EditorMode_Conveyor,
    EditorMode_Scaffold,
    EditorMode_Seesaw,
    EditorMode_CycleBlock,
    EditorMode_PushBlock,
    EditorMode_BreakBlock,
    EditorMode_CollapsePlatform,
    EditorMode_Puncher,
    EditorMode_Barbell,
    EditorMode_Money,
    EditorMode_Popper,
    EditorMode_Spotter,
    EditorMode_Boulder,
    EditorMode_ExerciseBall,
    
    EditorMode_Event,
    EditorMode_Checkpoint,
};

enum GRID_CELL_TYPE {
    CellType_Empty,
    CellType_Terrain,
};

enum EDITOR_EVENT__DATA_TYPE {
    dataType_point,
    dataType_rect,
    
    dataType_Count,
};

#define DATA_TYPE_LABELS  char * dataTypeLabel[ dataType_Count ] = { "PONT", "RECT", }
#define DATA_TYPE_TOKENS  char * dataTypeToken[ dataType_Count ] = { "vec2", "rect", }
#define DATA_TYPE_SIZES   uint32 dataTypeSize [ dataType_Count ] = { sizeof( vec2 ), sizeof( rect ), }

#define EDITOR_EVENT__DATA_MAX_COUNT  ( 16 )
struct EDITOR__EVENT {
    char * name;
    boo32  IsActive;
    
    uint32 nData;
    uint32  data_size;
    char *  data_name[ EDITOR_EVENT__DATA_MAX_COUNT ];
    uint32  data_Type[ EDITOR_EVENT__DATA_MAX_COUNT ];
    uint8 * data_ptr [ EDITOR_EVENT__DATA_MAX_COUNT ];
};

struct EDITOR_STATE {
    boo32 IsInitialized;
    EDITOR_MODE Mode;
    
    CAMERA_STATE Camera;
    PANEL        Panel;
    boo32        DrawGrid;
    
    STRING_BUFFER Event_Names;
    
    FILE_SAVE_OPEN SaveOpen;
    
    // NOTE: Any variable listed below this point will be cleared to zero when Reset( Editor ) is called.
    uint32 Reset_Reset;
    
    LEVEL_STATS Stat;
    
    EDITOR__CAMPER_STATE            CamperS;
    EDITOR__SPIKES_STATE            SpikesS;
    EDITOR__CONVEYOR_STATE          ConveyorS;
    EDITOR__SCAFFOLD_STATE          ScaffoldS;
    EDITOR__SEESAW_STATE            SeesawS;
    EDITOR__CYCLE_BLOCK_STATE       CycleBlockS;
    EDITOR__PUSH_BLOCK_STATE        PushBlockS;
    EDITOR__BREAK_BLOCK_STATE       BreakBlockS;
    EDITOR__PUNCHER_STATE           PuncherS;
    EDITOR__BARBELL_STATE           BarbellS;
    EDITOR__MONEY_STATE             MoneyS;
    EDITOR__POPPER_STATE            PopperS;
    EDITOR__SPOTTER_STATE           SpotterS;
    EDITOR__CHECKPOINT_STATE        CheckpointS;
    EDITOR__COLLAPSE_PLATFORM_STATE CollapsePlatformS;
    EDITOR__BOULDER_STATE           BoulderS;
    EDITOR__EXERCISE_BALL_STATE     ExerciseBallS;
    
    boo32       Mouse_DoMouse;
    UINT32_PAIR Mouse_iCell;
    uint32      Mouse_iEntity;
    uint32      Mouse_iSubEntity;
    
    boo32       Move_DoMouse;
    rect        Mouse_TempBound;
    rect        Move_SrcBound;
    rect        Move_DestBound;
    rect        Copy_SrcBound;
    rect        Copy_DestBound;
    rect        Mirror_SrcBound;
    
    boo32 Terrain_DoPlace;
    boo32 Terrain_DoErase;
    boo32 Checkpoint_DoMove;
    
    boo32       Event_DoMouse;
    UINT32_PAIR Event_iCell;
    uint32      Event_Type;
    uint8 *     Event_data;
    uint32      Event_iEvent;
    uint32      Event_iData;
    
    uint8 Reset_Event_Start;
#define EDITOR__EVENT_MAX_COUNT          ( 32 )
#define EDITOR__EVENT_MEMORY_BLOCK_SIZE  ( sizeof( rect ) * EDITOR_EVENT__DATA_MAX_COUNT )
#define EDITOR__EVENT_MEMORY_SIZE        ( EDITOR__EVENT_MAX_COUNT * EDITOR__EVENT_MEMORY_BLOCK_SIZE )
    uint32       nEvent;
    EDITOR__EVENT Event       [ EDITOR__EVENT_MAX_COUNT ];
    uint8         Event_Memory[ EDITOR__EVENT_MEMORY_SIZE ];
    uint8 Reset_Event_end;
    
#define EDITOR__GRID_MAX_HEIGHT  ( 1024 )
#define EDITOR__GRID_MAX_WIDTH   ( 1024 )
    uint8 Grid[ EDITOR__GRID_MAX_HEIGHT ][ EDITOR__GRID_MAX_WIDTH ];
};

//----------
// LAYOUT STATE
//----------

struct LAYOUT_LEVEL {
    boo32 Show;
    char  FileName[ FILE_SAVE_OPEN__FILENAME_MAX_CHAR + 1 ];
    vec2  Pos;
    vec2  Dim;
};

#define LAYOUT_Y_PIXELS_PER_UNIT  ( 8.0f )
struct LAYOUT_STATE {
    boo32 IsInitialized;
    
    CAMERA_STATE Camera;
    PANEL        Panel;
    
    //INPUT_STRING  Filter_String;
    //STRING_BUFFER Filter_FileList;
    
    FILE_SAVE_OPEN SaveOpen;
    
#define LAYOUT__LEVEL_MAX_COUNT  ( 1024 )
    uint32      nLevel;
    LAYOUT_LEVEL Level[ LAYOUT__LEVEL_MAX_COUNT ];
    
    boo32  DoMove;
    uint32 DoMove_iLevel;
    vec2   DoMove_bPos;
};

//----------
// DRAW STATE
//----------

struct DRAW_STATE {
    int32 Debug_iEntity;
    
    GAME_STATE              * Game;
    PLAYER_STATE            * Player;
    CAMPER_STATE            * CamperS;
    GRENADE_STATE           * Grenade;
    RUNNER_STATE            * RunnerS;
    PUNCHER_STATE           * PuncherS;
    POPPER_STATE            * PopperS;
    SPOTTER_STATE           * SpotterS;
    HEDGEHOG_STATE          * HedgehogS;
    SEESAW_STATE            * SeesawS;
    BOULDER_STATE           * BoulderS;
    EXERCISE_BALL_STATE     * ExerciseBallS;
    BARBELL_STATE           * BarbellS;
    MONEY_STATE             * MoneyS;
    STOMPER_STATE           * StomperS;
    CYCLE_BLOCK_STATE       * CycleBlockS;
    PUSH_BLOCK_STATE        * PushBlockS;
    BREAK_BLOCK_STATE       * BreakBlockS;
    COLLAPSE_PLATFORM_STATE * CollapsePlatformS;
    
    EXERCISE_MINIBOSS01 * ExerciseMiniBoss01;
    EXERCISE_MINIBOSS02 * ExerciseMiniBoss02;
    EXERCISE_MINIBOSS03 * ExerciseMiniBoss03;
    EXERCISE_BOSS       * ExerciseBoss;
};

//----------
// DEBUG REPLAY
//----------

struct REPLAY_FRAME {
    GAME_STATE              Game;
    PLAYER_STATE            Player;
    CAMPER_STATE            CamperS;
    GRENADE_STATE           Grenade;
    RUNNER_STATE            RunnerS;
    PUNCHER_STATE           PuncherS;
    POPPER_STATE            PopperS;
    SPOTTER_STATE           SpotterS;
    HEDGEHOG_STATE          HedgehogS;
    SEESAW_STATE            SeesawS;
    BOULDER_STATE           BoulderS;
    EXERCISE_BALL_STATE     ExerciseBallS;
    BARBELL_STATE           BarbellS;
    MONEY_STATE             MoneyS;
    STOMPER_STATE           StomperS;
    CYCLE_BLOCK_STATE       CycleBlockS;
    PUSH_BLOCK_STATE        PushBlockS;
    BREAK_BLOCK_STATE       BreakBlockS;
    COLLAPSE_PLATFORM_STATE CollapsePlatformS;
    
    EXERCISE_MINIBOSS01 ExerciseMiniBoss01;
    EXERCISE_MINIBOSS02 ExerciseMiniBoss02;
    EXERCISE_MINIBOSS03 ExerciseMiniBoss03;
    EXERCISE_BOSS       ExerciseBoss;
};

#define REPLAY__SECONDS_TO_CAPTURE  ( 10 )
#define REPLAY__FRAMES_PER_SECOND   ( 60 )
#define REPLAY_FRAME_MAX_COUNT  ( REPLAY__SECONDS_TO_CAPTURE * REPLAY__FRAMES_PER_SECOND )
struct REPLAY_STATE {
    boo32  Show;
    uint32 Show_atFrame;
    boo32  Show_holdFrame;
    
    uint32     atFrame;
    REPLAY_FRAME Frame[ REPLAY_FRAME_MAX_COUNT ];
};

//----------
// APP STATE
//----------

enum APP_MODE {
    AppMode_Game,
    AppMode_Editor,
    AppMode_Layout,
    
    AppMode_SaveLevel,
    AppMode_OpenLevel,
    
    AppMode_SaveLayout,
    AppMode_OpenLayout,
};

enum LEVEL_TRANSITION_MODE {
    LevelTransitionMode_ExitLeft,
    LevelTransitionMode_ExitRight,
    LevelTransitionMode_ExitBottom,
    LevelTransitionMode_ExitTop,
};

enum LEVEL_BUILDER_MODE {
    LevelBuilderMode_AddToLeft,
    LevelBuilderMode_AddToRight,
    LevelBuilderMode_AddToBottom,
    LevelBuilderMode_AddToTop,
};

struct LEVEL_BUILDER {
    LEVEL_BUILDER_MODE Mode;
    int32              PrevLevel;
};

struct APP_STATE {
	boo32 IsRunning;
	boo32 IsInitialized;
    boo32 isPaused;
    
    AUDIO_SYSTEM * AudioSystem;
    AUDIO_DATA     AudioList[ AudioID_Count ];
    
    APP_MODE Mode;
    boo32    Mode_IsInitialized;
    
    LEVEL_STATE LevelS;
    boo32  DoLoadCheckpoint;
    boo32  LoadCheckpoint_IsActive;
    flo32  LoadCheckpoint_Timer;
    
    boo32  DoLevelTransition;
    LEVEL_TRANSITION_MODE LevelTransition_Mode;
    int32                 LevelTransition_iLevel;
    flo32                 LevelTransition_Exit;
    LEVEL_BUILDER LevelBuilder;
	
    boo32  DoSaveGame;
    boo32  DoRecentSave;
    flo32  RecentSave_Timer;
    
    flo32  GameOver_Timer;
    
	vec2 App_Dim;
	vec2 App_HalfDim;
	rect App_Bound;
    flo32 dT;
    
    rect  TextTrigger_Bound;
    boo32 TextTrigger_IsActive;
    
    FONT * Font;
    
    EDITOR_STATE Editor;
    LAYOUT_STATE Layout;
    
    uint32 Reset_LevelReset_Start;
    
    CAMERA_STATE     Camera;
    
    TERRAIN_STATE           Terrain;
    GRENADE_STATE           Grenade;
    CONVEYOR_STATE          ConveyorS;
    SPIKES_STATE            SpikesS;
    CHECKPOINT_STATE        CheckpointS;
    SCAFFOLD_STATE          ScaffoldS;
    COLLAPSE_PLATFORM_STATE CollapsePlatformS;
    RUNNER_STATE            RunnerS;
    PUNCHER_STATE           PuncherS;
    POPPER_STATE            PopperS;
    SPOTTER_STATE           SpotterS;
    HEDGEHOG_STATE          HedgehogS;
    SEESAW_STATE            SeesawS;
    BOULDER_STATE           BoulderS;
    EXERCISE_BALL_STATE     ExerciseBallS;
    BARBELL_STATE           BarbellS;
    MONEY_STATE             MoneyS;
    STOMPER_STATE           StomperS;
    CYCLE_BLOCK_STATE       CycleBlockS;
    PUSH_BLOCK_STATE        PushBlockS;
    BREAK_BLOCK_STATE       BreakBlockS;
    TEXT_BOX_STATE          TextBoxS;
    CAMPER_STATE            CamperS;
    
    EXERCISE_MINIBOSS01 ExerciseMiniBoss01;
    EXERCISE_MINIBOSS02 ExerciseMiniBoss02;
    EXERCISE_MINIBOSS03 ExerciseMiniBoss03;
    EXERCISE_BOSS       ExerciseBoss;
    
    REPLAY_STATE     Replay;
    
    uint32 Reset_LevelReset_End;
    
    COLLISION_STATE  Collision;
    MEMORY           Collision_Memory;
    
    LEVEL_STATS       Stat;
    boo32            Level_IsInitialized;
    rect             Camera_Bound;
    flo32            DeathPlaneY;
    GAME_STATE       Game;
    PLAYER_STATE     Player;
    EVENT_STATE      Event;
    MEMORY           Event_Memory;
    
    PANEL Debug;
    int32 Debug_iEntity;
#define DEBUG_MAX_COUNT  ( 32 )
    boo32 Debug_IsActive[ DEBUG_MAX_COUNT ];
    
    vec2 LastJump;
    
    PARTICLE_STATE ParticleS;
    PROFILE_STATE  ProFile;
};