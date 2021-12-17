
#ifdef	STD_INCLUDE_DECL

//----------
// proFile declarations
//----------

#define FUNCTION_NAME_MAX_LENGTH  48
struct PROFILE_EVENT {
	char functionName[ FUNCTION_NAME_MAX_LENGTH + 1 ];
	uint64 clock_Start;
	uint64 clock_end;
	uint32 threadID;
    
#define PROFILE__SUBCLOCK_COUNT  ( 8 )
    uint64  subclock[ PROFILE__SUBCLOCK_COUNT ];
    uint32 nSubclock;
};

#define PROFILE_EVENT_LIST_MAX_COUNT  16384
struct PROFILE_EVENT_LIST {
	PROFILE_EVENT Event[ PROFILE_EVENT_LIST_MAX_COUNT ];
	uint64 FrameClockStart;
	int32 count;
};

struct PROFILE_NODE {
	char functionName[ FUNCTION_NAME_MAX_LENGTH ];
	uint64 clock_total;
	uint32 count_total;
    
    uint64  subclock[ PROFILE__SUBCLOCK_COUNT ];
    uint32 nSubclock;
};

#define PROFILE_NODE_LIST_MAX_COUNT  128
struct PROFILE_NODE_LIST {
	PROFILE_NODE node[ PROFILE_NODE_LIST_MAX_COUNT ];
	int32 count;
};

#define PROFILE_EVENT_STACK_MAX_COUNT  16
struct PROFILE_EVENT_STACK {
	PROFILE_EVENT Event[ PROFILE_EVENT_STACK_MAX_COUNT ];
	int32 count;
};

#define PROFILE_EVENT_DATABASE_MAX_COUNT  ( 5 * 60 )
struct PROFILE_EVENT_DATABASE {
	PROFILE_EVENT_LIST * EventList;
	int32 maxCount;
	int32 index;
	int32 selectIndex;
};

#if DEBUG_BUILD
#define PROFILE_FUNCTION()  PROFILE_OBJ proFileObj = PROFILE_OBJ( GlobalVar_proFileSystem.EventList_currentFrame, __FUNCTION__ )
#else
#define PROFILE_FUNCTION()
#endif

#if DEBUG_BUILD
#define PROFILE_BREAK()  proFileObj.Event->subclock[ proFileObj.Event->nSubclock++ ] = __rdtsc();
#else
#define PROFILE_BREAK()
#endif // DEBUG_BUILD

struct PROFILE_EVENT_DATABASE_CLOCKS {
	uint64 FrameStart;
	uint64 PlatformStart;
	uint64 appStart;
	uint64 renderStart;
	uint64 FrameEnd;
	int64  counter_FrameStart;
	int64  counter_FrameEnd;
};

class PROFILE_OBJ {
	public:
    PROFILE_EVENT * Event;
    PROFILE_OBJ( PROFILE_EVENT_LIST * EventList, char * funcName );
    ~PROFILE_OBJ();
};

typedef flo32 ( GET_MS_ELAPSED )( int64 StartCounter, int64 endCounter, int64 perfFrequency );

struct PROFILE_SYSTEM {
	boo32 ShowProFile;
	PROFILE_EVENT_DATABASE	EventDatabase;
	PROFILE_EVENT_LIST *		EventList_currentFrame;
	PROFILE_NODE_LIST * 		nodeList_currentFrame;
	
	GET_MS_ELAPSED * getMSElapsed;
	
	PROFILE_EVENT_DATABASE_CLOCKS		EventDatabase_clocks_currentFrame;
	PROFILE_EVENT_DATABASE_CLOCKS 	EventDatabase_clocks_previousFrame;
	PROFILE_EVENT_DATABASE_CLOCKS *	EventDatabase_clocks;
	
	flo32 EventDatabase_posOffset;
	flo32 EventDatabase_Width;
	flo32 EventDatabase_Height;
	boo32 EventDatabase_scrollActive;
	boo32 FrameBreakdown_scrollActive_bar;
	boo32 FrameBreakdown_scrollActive_Left;
	boo32 FrameBreakdown_scrollActive_Right;
	flo32 FrameBreakdown_scrollX_Left;
	flo32 FrameBreakdown_scrollX_Right;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

#if DEBUG_BUILD
#define PROFILE_SYSTEM__SIZE_IN_BYTES	( ( ( PROFILE_EVENT_DATABASE_MAX_COUNT + 1 ) * sizeof( PROFILE_EVENT_LIST ) ) + sizeof( PROFILE_NODE_LIST ) + ( PROFILE_EVENT_DATABASE_MAX_COUNT * sizeof( PROFILE_EVENT_DATABASE_CLOCKS ) ) )
PROFILE_SYSTEM GlobalVar_proFileSystem = {};

//----------
// proFile functions
//----------

internal uint32
GetThreadID() {
	uint8 * thread_memory = ( uint8 * )__readgsqword( 0x30 );
	uint32  threadID = *( ( uint32 * )( thread_memory + 0x48 ) );
	return threadID;
}

internal void
setString( char * srcStr, char * destStr ) {
	char * dest = destStr;
	char * src = srcStr;
	while( *src ) {
		*dest++ = *src++;
	}
	*dest = 0;
}

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

internal PROFILE_EVENT *
newEvent( PROFILE_EVENT_LIST * proFileList ) {
	int32 Event_index = InterlockedIncrement( ( volatile long * )&proFileList->count ) - 1;
	PROFILE_EVENT * Event = proFileList->Event + Event_index;
	Assert( proFileList->count < PROFILE_EVENT_LIST_MAX_COUNT );
	Event->clock_Start = 0;
	Event->clock_end = 0;
    Event->nSubclock = 0;
	for( int32 iter = 0; iter < FUNCTION_NAME_MAX_LENGTH; iter++ ) {
		Event->functionName[ iter ] = 0;
	}
	return Event;
}

internal PROFILE_NODE *
newNode( PROFILE_NODE_LIST * proFileList ) {
	PROFILE_NODE * node = proFileList->node + proFileList->count++;
	Assert( proFileList->count < PROFILE_NODE_LIST_MAX_COUNT );
	node->clock_total = 0;
	node->count_total = 0;
    node->nSubclock   = 0;
    memset( node->subclock, 0, sizeof( uint64 ) * PROFILE__SUBCLOCK_COUNT );
	for( int32 iter = 0; iter < FUNCTION_NAME_MAX_LENGTH; iter++ ) {
		node->functionName[ iter ] = 0;
	}
	return node;
}

internal void
collateEvents( PROFILE_EVENT_LIST * EventList, PROFILE_NODE_LIST * nodeList ) {
	PROFILE_EVENT_STACK _EventStack = {};
	PROFILE_EVENT_STACK * EventStack = &_EventStack;
	
	for( int32 Event_index = 0; Event_index < EventList->count; Event_index++ ) {
		PROFILE_EVENT * Event = EventList->Event + Event_index;
		
		PROFILE_NODE * currentNode = 0;
		boo32 isInList = false;
		
		for( int32 node_index = 0; node_index < nodeList->count && !isInList; node_index++ ) {
			PROFILE_NODE * node = nodeList->node + node_index;
			
			if( MatchString( Event->functionName, node->functionName ) ) {
				isInList = true;
				currentNode = node;
			}
		}
		
		if( !isInList ) {
			currentNode = newNode( nodeList );
			setString( Event->functionName, currentNode->functionName );
		}
		Assert( currentNode );
		currentNode->count_total++;
		uint64 clock_total = Event->clock_end - Event->clock_Start;
		currentNode->clock_total += clock_total;
        
        currentNode->nSubclock = Event->nSubclock;
        for( uint32 iSubclock = 0; iSubclock < currentNode->nSubclock; iSubclock++ ) {
            currentNode->subclock[ iSubclock ] += ( Event->subclock[ iSubclock ] - Event->clock_Start );
        }
		
		for( int32 iter = EventStack->count - 1; iter >= 0; iter-- ) {
			PROFILE_EVENT * stackEvent = EventStack->Event + iter;
			
			if( Event->clock_Start > stackEvent->clock_end ) {
				EventStack->count--;
			}
		}
		
		if( EventStack->count > 0 ) {
			PROFILE_EVENT * parentEvent = EventStack->Event + EventStack->count - 1;
			for( int32 node_index = 0; node_index < nodeList->count; node_index++ ) {
				PROFILE_NODE * node = nodeList->node + node_index;
				
				if( MatchString( parentEvent->functionName, node->functionName ) ) {
					node->clock_total -= clock_total;
					break;
				}
			}
		}
		
		PROFILE_EVENT * pushEvent = EventStack->Event + EventStack->count++;
		Assert( EventStack->count < PROFILE_EVENT_STACK_MAX_COUNT );
		*pushEvent = *Event;
	}
	
	boo32 isSorted = true;
	do {
		isSorted = true;
		for( int32 node_index = 0; node_index < nodeList->count - 1; node_index++ ) {
			PROFILE_NODE * nodeA = nodeList->node + node_index;
			PROFILE_NODE * nodeB = nodeList->node + node_index + 1;
			
			if( nodeA->clock_total < nodeB->clock_total ) {
				PROFILE_NODE _temp = {};
				PROFILE_NODE * temp = &_temp;
				
				setString( nodeA->functionName, temp->functionName );
				temp->clock_total = nodeA->clock_total;
				temp->count_total = nodeA->count_total;
				
				setString( nodeB->functionName, nodeA->functionName );
				nodeA->clock_total = nodeB->clock_total;
				nodeA->count_total = nodeB->count_total;
				
				setString( temp->functionName, nodeB->functionName );
				nodeB->clock_total = temp->clock_total;
				nodeB->count_total = temp->count_total;
				
				isSorted = false;
			}
		}
	} while( !isSorted );
}

PROFILE_OBJ::PROFILE_OBJ( PROFILE_EVENT_LIST * EventList, char * funcName ) {
	Event = newEvent( EventList );
	setString( funcName, Event->functionName );
	Event->threadID = GetThreadID();
	Event->clock_Start = __rdtsc();
}

PROFILE_OBJ::~PROFILE_OBJ() {
	Event->clock_end = __rdtsc();
}

internal void
updateProFile( PROFILE_SYSTEM * proFileSystem, PLATFORM * Platform, int64 perfFrequency, RENDERER * Renderer, rect client_Bound ) {
	if( GlobalVar_proFileSystem.ShowProFile ) {
		vec2  EventDatabase_BasePos 	= GetBL( client_Bound ) + Vec2( proFileSystem->EventDatabase_posOffset, proFileSystem->EventDatabase_posOffset );
		rect  EventDatabase_Bound		= RectBLD( EventDatabase_BasePos, Vec2( proFileSystem->EventDatabase_Width, proFileSystem->EventDatabase_Height ) );
		flo32 EventDatabase_slotWidth = GetWidth( EventDatabase_Bound ) / ( flo32 )PROFILE_EVENT_DATABASE_MAX_COUNT;
		
		MOUSE_STATE * Mouse = &Platform->Mouse;
		rect FrameBreakdown_scrollBound = RectBLD( GetTL( EventDatabase_Bound ) + Vec2( 0.0f, 4.0f ), Vec2( GetWidth( EventDatabase_Bound ), 12.0f ) );
		
		vec2 scrollBar_pos = Vec2( proFileSystem->FrameBreakdown_scrollX_Left, FrameBreakdown_scrollBound.Bottom );
		vec2 scrollBar_Dim = Vec2( proFileSystem->FrameBreakdown_scrollX_Right - proFileSystem->FrameBreakdown_scrollX_Left, GetHeight( FrameBreakdown_scrollBound ) );
		rect FrameBreakdown_scrollBarBound = RectBLD( scrollBar_pos, scrollBar_Dim );
		rect FrameBreakdown_LeftBound  = RectBLD( GetBL( FrameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_Dim.y ) );
		rect FrameBreakdown_RightBound = RectBLD( GetBR( FrameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_Dim.y ) );
		
		rect EventDatabase_resizeBound = RectBLD( GetTL( EventDatabase_Bound ) + Vec2( -2.0f, -2.0f ), Vec2( GetWidth( EventDatabase_Bound ) + 4.0f, 4.0f ) );
		if( proFileSystem->EventDatabase_scrollActive ) {
			proFileSystem->EventDatabase_Height = MaxValue( Mouse->Position.y - EventDatabase_Bound.Bottom, 100.0f );
		}
		if( proFileSystem->FrameBreakdown_scrollActive_Left ) {
			proFileSystem->FrameBreakdown_scrollX_Left  = Clamp( Mouse->Position.x, proFileSystem->EventDatabase_posOffset, proFileSystem->FrameBreakdown_scrollX_Right - 40.0f );
		}
		if( proFileSystem->FrameBreakdown_scrollActive_Right ) {
			proFileSystem->FrameBreakdown_scrollX_Right = Clamp( Mouse->Position.x, proFileSystem->FrameBreakdown_scrollX_Left + 40.0f, FrameBreakdown_scrollBound.Right );
		}
		if( proFileSystem->FrameBreakdown_scrollActive_bar ) {
			flo32 Width = proFileSystem->FrameBreakdown_scrollX_Right - proFileSystem->FrameBreakdown_scrollX_Left;
			
			flo32 Left  = Mouse->Position.x - Width * 0.5f;
			flo32 Right = Mouse->Position.x + Width * 0.5f;
			if( Left < proFileSystem->EventDatabase_posOffset ) {
				Left  = proFileSystem->EventDatabase_posOffset;
				Right = proFileSystem->EventDatabase_posOffset + Width;
			}
			if( Right > FrameBreakdown_scrollBound.Right ) {
				Right = FrameBreakdown_scrollBound.Right;
				Left  = FrameBreakdown_scrollBound.Right - Width;
			}
			
			proFileSystem->FrameBreakdown_scrollX_Left  = Left;
			proFileSystem->FrameBreakdown_scrollX_Right = Right;
		}
		if( IsInBound( Mouse->Position, EventDatabase_resizeBound ) ) {
			if( WasPressed( Mouse, MouseButton_Left ) ) {
				proFileSystem->EventDatabase_scrollActive = true;
			}
		}
		if( IsInBound( Mouse->Position, FrameBreakdown_LeftBound ) ) {
			if( WasPressed( Mouse, MouseButton_Left ) ) {
				proFileSystem->FrameBreakdown_scrollActive_Left = true;
			}
		} else if( IsInBound( Mouse->Position, FrameBreakdown_RightBound ) ) {
			if( WasPressed( Mouse, MouseButton_Left ) ) {
				proFileSystem->FrameBreakdown_scrollActive_Right = true;
			}
		} else if( IsInBound( Mouse->Position, FrameBreakdown_scrollBarBound ) ) {
			if( WasPressed( Mouse, MouseButton_Left ) ) {
				proFileSystem->FrameBreakdown_scrollActive_bar = true;
			}
		}
		if( WasReleased( Mouse, MouseButton_Left ) ) {
			proFileSystem->EventDatabase_scrollActive        = false;
			proFileSystem->FrameBreakdown_scrollActive_bar   = false;
			proFileSystem->FrameBreakdown_scrollActive_Left  = false;
			proFileSystem->FrameBreakdown_scrollActive_Right = false;
		}
		scrollBar_pos = Vec2( proFileSystem->FrameBreakdown_scrollX_Left, FrameBreakdown_scrollBound.Bottom );
		scrollBar_Dim = Vec2( proFileSystem->FrameBreakdown_scrollX_Right - proFileSystem->FrameBreakdown_scrollX_Left, GetHeight( FrameBreakdown_scrollBound ) );
		FrameBreakdown_scrollBarBound = RectBLD( scrollBar_pos, scrollBar_Dim );
		FrameBreakdown_LeftBound  = RectBLD( GetBL( FrameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_Dim.y ) );
		FrameBreakdown_RightBound = RectBLD( GetBR( FrameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_Dim.y ) );
		
		int32 collateIndex = GlobalVar_proFileSystem.EventDatabase.index;
		if( WasPressed( Mouse, MouseButton_Right ) ) {
			GlobalVar_proFileSystem.EventDatabase.selectIndex = -1;
		}
		if( GlobalVar_proFileSystem.EventDatabase.selectIndex != -1 ) {
			collateIndex = GlobalVar_proFileSystem.EventDatabase.selectIndex;
		}
		if( IsInBound( Mouse->Position, EventDatabase_Bound ) ) {
			int32 hoverIndex = ( int32 )( ( Mouse->Position.x - EventDatabase_Bound.Left ) / EventDatabase_slotWidth );
			if( WasPressed( Mouse, MouseButton_Left ) ) {
				GlobalVar_proFileSystem.EventDatabase.selectIndex = hoverIndex;
				collateIndex = hoverIndex;
			}
			if( GlobalVar_proFileSystem.EventDatabase.selectIndex == -1 ) {
				collateIndex = hoverIndex;
			}
		}
		PROFILE_EVENT_LIST * collateList = GlobalVar_proFileSystem.EventDatabase.EventList + collateIndex;
		PROFILE_EVENT_DATABASE_CLOCKS * collateClocks = GlobalVar_proFileSystem.EventDatabase_clocks + collateIndex;
        
		collateEvents( collateList, GlobalVar_proFileSystem.nodeList_currentFrame );
		
#define CLOCK_BOUND_MAX_STACK_COUNT  16
		uint64 clockBound_stack[ CLOCK_BOUND_MAX_STACK_COUNT ] = {};
		int32 stack_Count = 0;
		
		flo32 FrameBreakdown_minWidth = 40.0f;
		flo32 FrameBreakdown_maxWidth = GetWidth( FrameBreakdown_scrollBound );
		flo32 FrameBreakdown_ratio = ( proFileSystem->FrameBreakdown_scrollX_Right - proFileSystem->FrameBreakdown_scrollX_Left - FrameBreakdown_minWidth ) / ( FrameBreakdown_maxWidth - FrameBreakdown_minWidth );
		// DISPLAY_VALUE( flo32, FrameBreakdown_ratio );
		
		vec2	client_Dim = GetDim( client_Bound );
		flo32 client_Width = client_Dim.x;
		flo32 proFile_minWidth = client_Dim.x;
		flo32 proFile_maxWidth = proFile_minWidth * 10.0f;
		flo32 proFile_Width   = lerp( proFile_maxWidth, FrameBreakdown_ratio, proFile_minWidth );
		
		vec2  proFile_BasePos = Vec2( 0.0f, EventDatabase_Bound.Top + 20.0f );
		flo32 row_Height = 20.0f;
		
		flo32 valid_Width = GetWidth( FrameBreakdown_scrollBound ) - ( proFileSystem->FrameBreakdown_scrollX_Right - proFileSystem->FrameBreakdown_scrollX_Left );
		if( valid_Width > 0.0f ) {
			flo32 offset_ratio = ( proFileSystem->FrameBreakdown_scrollX_Left - FrameBreakdown_scrollBound.Left ) / valid_Width;
			proFile_BasePos.x -= ( proFile_Width - client_Width ) * offset_ratio;
		}
		uint64 proFile_FrameClockStart = collateList->FrameClockStart;
		uint64 proFile_clocksPerFrame = 60000000;
		
		rect clear_BoundA = Rect( client_Bound.Left, client_Bound.Bottom, client_Bound.Right, EventDatabase_Bound.Top );
		DEBUG_DrawRect( clear_BoundA, Vec4( 0.0f, 0.0f, 0.0f, 0.95f ) );
		
		DEBUG_DrawRect( EventDatabase_Bound, COLOR_GRAY( 0.80f ) );
		for( int32 database_index = 0; database_index < PROFILE_EVENT_DATABASE_MAX_COUNT; database_index++ ) {
			PROFILE_EVENT_DATABASE_CLOCKS clock = GlobalVar_proFileSystem.EventDatabase_clocks[ database_index ];
			
			rect BoundA = {};
			BoundA.Left  = EventDatabase_BasePos.x + EventDatabase_slotWidth * ( flo32 )database_index;
			BoundA.Right = BoundA.Left + EventDatabase_slotWidth;
			BoundA.Bottom = EventDatabase_BasePos.y + ( ( flo32 )( clock.PlatformStart	- clock.FrameStart ) / ( flo32 )proFile_clocksPerFrame ) * proFileSystem->EventDatabase_Height;
			BoundA.Top 	 = EventDatabase_BasePos.y + ( ( flo32 )( clock.appStart 		- clock.FrameStart ) / ( flo32 )proFile_clocksPerFrame ) * proFileSystem->EventDatabase_Height;
			DEBUG_DrawRect( BoundA, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
			
			rect BoundB = {};
			BoundB.Left  = EventDatabase_BasePos.x + EventDatabase_slotWidth * ( flo32 )database_index;
			BoundB.Right = BoundB.Left + EventDatabase_slotWidth;
			BoundB.Bottom = EventDatabase_BasePos.y + ( ( flo32 )( clock.appStart		- clock.FrameStart ) / ( flo32 )proFile_clocksPerFrame ) * proFileSystem->EventDatabase_Height;
			BoundB.Top 	 = EventDatabase_BasePos.y + ( ( flo32 )( clock.renderStart	- clock.FrameStart ) / ( flo32 )proFile_clocksPerFrame ) * proFileSystem->EventDatabase_Height;
			DEBUG_DrawRect( BoundB, Vec4( 0.0f, 0.0f, 0.5f, 1.0f ) );
			
			rect BoundC = {};
			BoundC.Left  = EventDatabase_BasePos.x + EventDatabase_slotWidth * ( flo32 )database_index;
			BoundC.Right = BoundC.Left + EventDatabase_slotWidth;
			BoundC.Bottom = EventDatabase_BasePos.y + ( ( flo32 )( clock.renderStart	- clock.FrameStart ) / ( flo32 )proFile_clocksPerFrame ) * proFileSystem->EventDatabase_Height;
			BoundC.Top 	 = EventDatabase_BasePos.y + ( ( flo32 )( clock.FrameEnd		- clock.FrameStart ) / ( flo32 )proFile_clocksPerFrame ) * proFileSystem->EventDatabase_Height;
			DEBUG_DrawRect( BoundC, Vec4( 0.5f, 0.0f, 0.0f, 1.0f ) );
		}
		
		rect clear_BoundB = Rect( client_Bound.Left, EventDatabase_Bound.Top, client_Bound.Right, client_Bound.Top );
		DEBUG_DrawRect( clear_BoundB, Vec4( 0.0f, 0.0f, 0.0f, 0.95f ) );
		
		rect EventDatabase_highlightBound = RectBLD( EventDatabase_BasePos + Vec2( EventDatabase_slotWidth * ( flo32 )GlobalVar_proFileSystem.EventDatabase.index, 0.0f ), Vec2( EventDatabase_slotWidth, proFileSystem->EventDatabase_Height ) );
		DEBUG_DrawRect( EventDatabase_highlightBound, Vec4( 0.25f, 0.25f, 0.0f, 0.25f ) );
		
		if( collateIndex != -1 ) {
			vec2 collateIndex_Position = EventDatabase_BasePos + Vec2( EventDatabase_slotWidth * ( flo32 )collateIndex, 0.0f );
			DEBUG_DrawRect( RectBLD( collateIndex_Position, Vec2( EventDatabase_slotWidth, proFileSystem->EventDatabase_Height ) ), Vec4( 0.5f, 0.0f, 0.0f, 0.5f ) );
		}
		
		DEBUG_DrawRect( FrameBreakdown_scrollBound,    COLOR_GRAY( 0.20f ) );
		DEBUG_DrawRect( FrameBreakdown_scrollBarBound, COLOR_GRAY( 0.50f ) );
		
		if( IsInBound( Mouse->Position, EventDatabase_resizeBound ) ) {
			DEBUG_DrawRect( EventDatabase_resizeBound, COLOR_RED );
		}
		if( IsInBound( Mouse->Position, FrameBreakdown_LeftBound ) ) {
			DEBUG_DrawRect( FrameBreakdown_LeftBound, COLOR_RED );
		} else if( IsInBound( Mouse->Position, FrameBreakdown_RightBound ) ) {
			DEBUG_DrawRect( FrameBreakdown_RightBound, COLOR_RED );
		} else if( IsInBound( Mouse->Position, FrameBreakdown_scrollBarBound ) ) {
			DEBUG_DrawRect( FrameBreakdown_scrollBarBound, COLOR_RED );
		}
		
		vec4 ColorTable[ 15 ] = {};
		ColorTable[  0 ] = Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
		ColorTable[  1 ] = Vec4( 0.4f, 0.2f, 0.2f, 1.0f );
		ColorTable[  2 ] = Vec4( 0.6f, 0.2f, 0.2f, 1.0f );
		ColorTable[  3 ] = Vec4( 0.2f, 0.4f, 0.2f, 1.0f );
		ColorTable[  4 ] = Vec4( 0.2f, 0.6f, 0.2f, 1.0f );
		ColorTable[  5 ] = Vec4( 0.2f, 0.2f, 0.4f, 1.0f );
		ColorTable[  6 ] = Vec4( 0.2f, 0.2f, 0.6f, 1.0f );
		ColorTable[  7 ] = Vec4( 0.4f, 0.4f, 0.2f, 1.0f );
		ColorTable[  8 ] = Vec4( 0.6f, 0.6f, 0.2f, 1.0f );
		ColorTable[  9 ] = Vec4( 0.4f, 0.2f, 0.4f, 1.0f );
		ColorTable[ 10 ] = Vec4( 0.6f, 0.2f, 0.6f, 1.0f );
		ColorTable[ 11 ] = Vec4( 0.2f, 0.4f, 0.4f, 1.0f );
		ColorTable[ 12 ] = Vec4( 0.2f, 0.6f, 0.6f, 1.0f );
		ColorTable[ 13 ] = Vec4( 0.4f, 0.4f, 0.4f, 1.0f );
		ColorTable[ 14 ] = Vec4( 0.6f, 0.6f, 0.6f, 1.0f );
		int32 Color_select = 0;
		
        int32 threadID_Count = 0;
		uint32 threadID[ 12 ] = {};
		
		PROFILE_EVENT * proFile_tooltip = 0;
		for( int32 Event_index = 0; Event_index < collateList->count; Event_index++ ) {
			PROFILE_EVENT * Event = collateList->Event + Event_index;
			
			int32 level = -1;
			for( int32 threadID_index = 0; ( level == -1 ) && ( threadID_index < threadID_Count ); threadID_index++ ) {
				if( threadID[ threadID_index ] == Event->threadID ) {
					level = threadID_index;
				}
			}
			if( level == -1 ) {
				threadID[ threadID_Count ] = Event->threadID;
				level = threadID_Count++;
			}
			Assert( threadID_Count < 12 );
			
			rect Bound = {};
			Bound.Left  = proFile_BasePos.x + ( ( flo32 )( Event->clock_Start - proFile_FrameClockStart ) / ( flo32 )proFile_clocksPerFrame ) * proFile_Width;
			Bound.Right = proFile_BasePos.x + ( ( flo32 )( Event->clock_end   - proFile_FrameClockStart ) / ( flo32 )proFile_clocksPerFrame ) * proFile_Width;
			Bound.Top 	 = proFile_BasePos.y + row_Height * ( flo32 )( level + 1 );
			Bound.Bottom = proFile_BasePos.y + row_Height * ( flo32 )level;
			
			if( IsInBound( Mouse->Position, Bound ) ) {
				proFile_tooltip = Event;
			}
			
			DEBUG_DrawRect( Bound, ColorTable[ Color_select ] );
			
			Color_select = ( Color_select + 2 ) % _ArrayCount( ColorTable );
		}
		
		if( proFile_tooltip ) {
			PROFILE_tooltip( proFile_tooltip->functionName, Mouse->Position );
		}
		
		flo32  Frame_msElapsed = proFileSystem->getMSElapsed( collateClocks->counter_FrameStart, collateClocks->counter_FrameEnd, perfFrequency );
		uint64 Frame_cycElapsed = collateClocks->FrameEnd    - collateClocks->FrameStart;
		uint64   app_cycElapsed = collateClocks->renderStart - collateClocks->appStart;
		
		char time_stringA[ 128 ] = {};
		sprintf( time_stringA, "Prev Frame: %10.04f ms  %10I64u cyc  %6I64u MC", Frame_msElapsed, Frame_cycElapsed, Frame_cycElapsed / 1000 );
		
		char time_stringB[ 128 ] = {};
		sprintf( time_stringB, "App Update:                %10I64u cyc  %6I64u MC  %.02f", app_cycElapsed, app_cycElapsed / 1000, ( flo32 )app_cycElapsed / ( flo32 )Frame_cycElapsed * 100.0f );
		
		PROFILE_string( time_stringA );
		PROFILE_string( time_stringB );
		
		if( GlobalVar_proFileSystem.nodeList_currentFrame->count > 0 ) {
			PROFILE_string( "------------------------|---nCyc---||-MCyc-||-count-||-%Frame-||-%app-|" );
		}
		for( int32 node_index = 0; node_index < GlobalVar_proFileSystem.nodeList_currentFrame->count; node_index++ ) {
			PROFILE_NODE * node = GlobalVar_proFileSystem.nodeList_currentFrame->node + node_index;
			
			char proFile_string[ 512 ] = {};
			sprintf( proFile_string, "%-24s %10I64u  %6I64u  %7d  %8.02f  %6.02f", node->functionName, node->clock_total, node->clock_total / 1000, node->count_total, ( ( flo32 )node->clock_total / ( flo32 )Frame_cycElapsed ) * 100.0f, ( ( flo32 )node->clock_total / ( flo32 )app_cycElapsed ) * 100.0f );
			PROFILE_string( proFile_string );
            
            for( uint32 iSubclock = 0; iSubclock < node->nSubclock; iSubclock++ ) {
                flo32 percent = ( flo32 )node->subclock[ iSubclock ] / ( flo32 )node->clock_total;
                
                char str[ 512 ] = {};
                sprintf( str, "  %.02f", percent );
                PROFILE_string( str );
            }
            // average each subclock for the Event??
            // should this only work for nodes that have a single call
		}
	} else {
		flo32  Frame_msElapsed 	= proFileSystem->getMSElapsed( GlobalVar_proFileSystem.EventDatabase_clocks_previousFrame.counter_FrameStart, GlobalVar_proFileSystem.EventDatabase_clocks_previousFrame.counter_FrameEnd, perfFrequency );
		uint64 Frame_cycElapsed = GlobalVar_proFileSystem.EventDatabase_clocks_previousFrame.FrameEnd - GlobalVar_proFileSystem.EventDatabase_clocks_previousFrame.FrameStart;
		uint64 app_cycElapsed = GlobalVar_proFileSystem.EventDatabase_clocks_previousFrame.renderStart - GlobalVar_proFileSystem.EventDatabase_clocks_previousFrame.appStart;
		char time_stringA[ 128 ] = {};
		sprintf( time_stringA, "Prev Frame: %10.04f ms  %10I64u cyc  %6I64u MC", Frame_msElapsed, Frame_cycElapsed, Frame_cycElapsed / 1000 );
		
		char time_stringB[ 128 ] = {};
		sprintf( time_stringB, "App Update:                %10I64u cyc  %6I64u MC  %.02f", app_cycElapsed, app_cycElapsed / 1000, ( flo32 )app_cycElapsed / ( flo32 )Frame_cycElapsed * 100.0f );
		
		DISPLAY_STRING( time_stringA );
		DISPLAY_STRING( time_stringB );
		
		PROFILE_EVENT_LIST * database_EventList = GlobalVar_proFileSystem.EventDatabase.EventList + GlobalVar_proFileSystem.EventDatabase.index;
		database_EventList->count = 0;
		database_EventList->FrameClockStart = GlobalVar_proFileSystem.EventDatabase_clocks_previousFrame.FrameStart;
		
		GlobalVar_proFileSystem.EventDatabase_clocks[ GlobalVar_proFileSystem.EventDatabase.index ] = GlobalVar_proFileSystem.EventDatabase_clocks_previousFrame;
		
		for( int32 Event_index = 0; Event_index < GlobalVar_proFileSystem.EventList_currentFrame->count; Event_index++ ) {
			PROFILE_EVENT * srcEvent = GlobalVar_proFileSystem.EventList_currentFrame->Event + Event_index;
			PROFILE_EVENT * destEvent = database_EventList->Event + database_EventList->count++;
			Assert( database_EventList->count < PROFILE_EVENT_LIST_MAX_COUNT );
			
			*destEvent = *srcEvent;
		}
		GlobalVar_proFileSystem.EventDatabase.index = ( GlobalVar_proFileSystem.EventDatabase.index + 1 ) % GlobalVar_proFileSystem.EventDatabase.maxCount;
	}
	GlobalVar_proFileSystem.EventList_currentFrame->count = 0;
	GlobalVar_proFileSystem.nodeList_currentFrame->count = 0;
	
	KEYBOARD_STATE * Keyboard = &Platform->Keyboard;
	if( WasPressed( Keyboard, KeyCode_p ) ) {
		if( GlobalVar_proFileSystem.ShowProFile ) {
			proFileSystem->EventDatabase_scrollActive = false;
		}
	}
    
	
	GlobalVar_proFileSystem.EventDatabase_clocks_currentFrame.PlatformStart = __rdtsc();
}

#endif // DEBUG_BUILD

#endif	// STD_INCLUDE_FUNC