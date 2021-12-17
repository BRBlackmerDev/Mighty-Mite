
// #ifdef	STD_INCLUDE_DECL
// #endif	// STD_INCLUDE_DECL
// #ifdef	STD_INCLUDE_FUNC
// #endif	// STD_INCLUDE_FUNC

//#pragma optimize( "", off )

#include <math.h> // sqrtf, cosf, sinf
#include <stdint.h> // Type definitions
#include <stdlib.h> // rand
#include <time.h> // time
#include <cstdio> // sprintf
#include <shlwapi.h>
#include <windows.h>
#include <xinput.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_trueType.h"

#include "TypeDef.h"
#include "math.cpp"

#define 	STD_INCLUDE_DECL
#include "Vector.cpp"
#include "memory.cpp"
#include "Font.cpp"
#include "userInput.cpp"
#undef	STD_INCLUDE_DECL

#define	STD_INCLUDE_FUNC
#include "Vector.cpp"
#include "memory.cpp"
#include "Font.cpp"
#include "userInput.cpp"
#undef	STD_INCLUDE_FUNC

#define DEBUG_SYSTEM__SAFETY_OBJECT_COUNT  ( 4 )
#define DEBUG_SYSTEM__SAFETY_VERTEX_COUNT  ( 512 )
// NOTE: These variables add additional memory to the vertex buffer and text object list as a graceful failsafe to display the 'ERROR OCCURRED! Check console' message without crashing the program. This is just the cleaNest way to have a graceful failsafe doomsday error message without interfering with a bunch of systems. This system was added to prEvent Asserts from crashing our program and risk losing work.

#include "2dplatformer_config.h"
#include "d3d12.cpp"
#include "inputString.cpp"

#include "Win32_Platform.h"

#define APP_TargetFPS  ( 60.0f )

#include <dsound.h>
#include "AudioSystem.h"

#include "Intersect.h"

#include "Entity.h"
#include "2dplatformer.h"
#include "Debug_Win32.cpp"
#include "Intersect.cpp"

#include "AudioSystem.cpp"

#include "bmp.cpp"
#include "wav.cpp"

#include "Draw.cpp"
#include "File.cpp"
#include "tools.cpp"
#include "Editor_tools.cpp"

internal void DamagePlayer( APP_STATE * AppState, uint32 Damage );

#include "Collision.cpp"
#include "Particle.cpp"
#include "Terrain.cpp"
#include "Event.cpp"
#include "LevelStats.cpp"
//#include "Grenade.cpp"
#include "checkpoint.cpp"
#include "Conveyor.cpp"
#include "Spikes.cpp"
#include "Scaffold.cpp"
#include "CollapsePlatform.cpp"
#include "Runner.cpp"
#include "Puncher.cpp"
#include "Seesaw.cpp"
#include "CycleBlock.cpp"
#include "PushBlock.cpp"
#include "BreakBlock.cpp"
#include "Spotter.cpp"
#include "Boulder.cpp"
#include "ExerciseBall.cpp"
#include "Stomper.cpp"
#include "ExerciseMiniBoss01.cpp"
#include "ExerciseMiniBoss02.cpp"
#include "ExerciseMiniBoss03.cpp"
#include "ExerciseBoss.cpp"
#include "Popper.cpp"
#include "Hedgehog.cpp"
#include "Camper.cpp"
#include "Barbell.cpp"
#include "Money.cpp"
#include "TextBox.cpp"
#include "Player.cpp"

#include "hit.cpp"

#include "parser.cpp"
#include "save.cpp"
#include "Load.cpp"

// EVENTS
#include "TempA.cpp"

#include "Events_Stomper.cpp"
#include "Events_ExerciseLevel.cpp"
// EVENTS

#include "gameplay.cpp"

#include "Layout.cpp"
#include "Replay.cpp"
#include "Panel.cpp"
#include "DebugPanel.cpp"
#include "EditorPanel.cpp"
#include "LayoutPanel.cpp"

#include "2dplatformer.cpp"
#include "Win32_Platform.cpp"
