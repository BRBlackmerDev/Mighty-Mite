
#ifdef	STD_INCLUDE_DECL

//----------
// Mouse declarations
//----------

enum MOUSE_BUTTON_ID {
	MouseButton_Left,
	MouseButton_Right,
	MouseButton_middle,
	
	MouseButton_Count,
};

struct MOUSE_BUTTON_STATE {
	boo32 IsDown;
	int32 HalfTransitionCount;
};

struct MOUSE_STATE {
	MOUSE_BUTTON_STATE button[ MouseButton_Count ];
	vec2  Position;
    vec2  PrevPosition;
    
    vec2  dPos;
    
    vec2  downPos;
    vec2  BasePos;
    boo32 ResetPos;
    
    uint8 flags;
	int32 WheelClicks;
};

//----------
// Keyboard declarations
//----------

enum KEY_STATE_CODE {
	KeyCode_null,
	
	KeyCode_Backspace,
	KeyCode_shift,
	KeyCode_control,
	KeyCode_alt,
	KeyCode_capsLock,
	KeyCode_Escape,
	KeyCode_pageUp,
	KeyCode_pageDown,
	KeyCode_end,
	KeyCode_home,
	KeyCode_Left,
	KeyCode_Right,
	KeyCode_Up,
	KeyCode_Down,
	KeyCode_insert,
	KeyCode_delete,
	KeyCode_Enter,
	KeyCode_tab,
	KeyCode_space,
	KeyCode_0,
	KeyCode_1,
	KeyCode_2,
	KeyCode_3,
	KeyCode_4,
	KeyCode_5,
	KeyCode_6,
	KeyCode_7,
	KeyCode_8,
	KeyCode_9,
	KeyCode_a,
	KeyCode_b,
	KeyCode_c,
	KeyCode_d,
	KeyCode_e,
	KeyCode_f,
	KeyCode_g,
	KeyCode_h,
	KeyCode_i,
	KeyCode_j,
	KeyCode_k,
	KeyCode_l,
	KeyCode_m,
	KeyCode_n,
	KeyCode_o,
	KeyCode_p,
	KeyCode_q,
	KeyCode_r,
	KeyCode_s,
	KeyCode_t,
	KeyCode_u,
	KeyCode_v,
	KeyCode_w,
	KeyCode_x,
	KeyCode_y,
	KeyCode_z,
	KeyCode_period,
	KeyCode_comma,
	KeyCode_quote,
	KeyCode_semicolon,
	KeyCode_equal,
	KeyCode_dash,
	KeyCode_tilde,
	KeyCode_forwardSlash,
	KeyCode_BackSlash,
	KeyCode_openBracket,
	KeyCode_closeBracket,
	KeyCode_num0,
	KeyCode_num1,
	KeyCode_num2,
	KeyCode_num3,
	KeyCode_num4,
	KeyCode_num5,
	KeyCode_num6,
	KeyCode_num7,
	KeyCode_num8,
	KeyCode_num9,
    KeyCode_numAdd,
    KeyCode_numSub,
    KeyCode_numMul,
    KeyCode_numDiv,
    KeyCode_numPeriod,
    KeyCode_F1,
    KeyCode_F2,
    KeyCode_F3,
    KeyCode_F4,
    KeyCode_F5,
    KeyCode_F6,
    KeyCode_F7,
    KeyCode_F8,
    KeyCode_F9,
    KeyCode_F10,
    KeyCode_F11,
    KeyCode_F12,
	
	KeyCode_Count,
};

struct KEY_STATE {
    uint16 KeyCode;
    uint8  IsDown;
	uint8  flags;
};

#define KEYBOARD_FLAGS__CONTROL  ( 0x01 )
#define KEYBOARD_FLAGS__ALT      ( 0x02 )
#define KEYBOARD_FLAGS__SHIFT    ( 0x04 )

#define KEYBOARD_STATE__MAX_EVENT_COUNT  ( 16 )
struct KEYBOARD_STATE {
	uint8    flags;
    uint32   nEvents;
    KEY_STATE Event[ KEYBOARD_STATE__MAX_EVENT_COUNT ];
};

//----------
// Controller declarations
//----------

enum CONTROLLER_BUTTON_ID {
	ControllerButton_dPad_Up,
	ControllerButton_dPad_Down,
	ControllerButton_dPad_Left,
	ControllerButton_dPad_Right,
    
    ControllerButton_Start,
    ControllerButton_Back,
    
    ControllerButton_Thumb_Left,
    ControllerButton_Thumb_Right,
    ControllerButton_Shoulder_Left,
    ControllerButton_Shoulder_Right,
    ControllerButton_Trigger_Left,
    ControllerButton_Trigger_Right,
    
    ControllerButton_A,
    ControllerButton_B,
    ControllerButton_X,
    ControllerButton_Y,
	
	ControllerButton_Count,
};

struct CONTROLLER_BUTTON_STATE {
	boo32 IsDown;
	int32 HalfTransitionCount;
};

struct CONTROLLER_THUMBSTICK {
    INT32_PAIR xy;
    int32      dead;
};

struct CONTROLLER_STATE {
	CONTROLLER_BUTTON_STATE button[ ControllerButton_Count ];
    CONTROLLER_THUMBSTICK   Left;
    CONTROLLER_THUMBSTICK   Right;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// Mouse functions
//----------

internal void
MOUSE_endOfFrame( MOUSE_STATE * Mouse ) {
	Mouse->WheelClicks  = 0;
    //Mouse->PrevPosition = Mouse->Position;
	for( uint32 buttonIndex = 0; buttonIndex < MouseButton_Count; buttonIndex++ ) {
		MOUSE_BUTTON_STATE * button = &Mouse->button[ buttonIndex ];
		button->HalfTransitionCount = 0;
	}
}

internal boo32
IsDown( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID buttonID, uint8 flags = 0 ) {
    MOUSE_BUTTON_STATE * button = Mouse->button + buttonID;
	boo32 Result = ( button->IsDown ) && ( Mouse->flags == flags );
	return Result;
}

internal boo32
wasButtonPressed( MOUSE_BUTTON_STATE * button ) {
	boo32 Result = false;
	if( ( button->IsDown && button->HalfTransitionCount >= 1 ) ||
       ( !button->IsDown && button->HalfTransitionCount >= 2 ) ) {
		Result = true;
	}
	return Result;
}

internal boo32
WasPressed( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID buttonID, uint8 flags = 0 ) {
	MOUSE_BUTTON_STATE * button = &Mouse->button[ buttonID ];
	boo32 Result = ( wasButtonPressed( button ) ) && ( Mouse->flags == flags );
	return Result;
}

internal boo32
WasReleased( MOUSE_BUTTON_STATE * button ) {
	boo32 Result = false;
	if( ( !button->IsDown && button->HalfTransitionCount >= 1 ) ||
       ( button->IsDown && button->HalfTransitionCount >= 2 ) ) {
		Result = true;
	}
	return Result;
}

internal boo32
WasReleased( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID buttonID ) {
	MOUSE_BUTTON_STATE * button = &Mouse->button[ buttonID ];
	boo32 Result = WasReleased( button );
	return Result;
}

internal void
addButtonEvent( MOUSE_STATE * Mouse, MOUSE_BUTTON_ID buttonID, boo32 IsDown, uint8 flags ) {
    Mouse->flags = flags;
    
    MOUSE_BUTTON_STATE * button = Mouse->button + buttonID;
    button->IsDown = IsDown;
    button->HalfTransitionCount++;
}

//----------
// Keyboard functions
//----------

internal KEY_STATE
KeyState( KEY_STATE_CODE KeyCode, uint8 IsDown, uint8 flags ) {
	KEY_STATE Result = {};
	Result.KeyCode     = ( uint16 )KeyCode;
	Result.IsDown      = IsDown;
	Result.flags       = flags;
	return Result;
}

internal boo32
WasPressed( KEYBOARD_STATE * Keyboard, KEY_STATE_CODE KeyCode, uint8 targetFlags = 0 ) {
	boo32 Result = false;
	for( uint32 iEvent = 0; iEvent < Keyboard->nEvents; iEvent++ ) {
		KEY_STATE Event = Keyboard->Event[ iEvent ];
		if( ( Event.KeyCode == KeyCode ) && ( Event.IsDown ) && ( Event.flags == targetFlags ) ) {
			Result = true;
		}
	}
	return Result;
}

internal boo32
WasReleased( KEYBOARD_STATE * Keyboard, KEY_STATE_CODE KeyCode ) {
	boo32 Result = false;
	for( uint32 iEvent = 0; iEvent < Keyboard->nEvents; iEvent++ ) {
		KEY_STATE Event = Keyboard->Event[ iEvent ];
		if( ( Event.KeyCode == KeyCode ) && ( !Event.IsDown ) ) {
			Result = true;
		}
	}
	return Result;
}

internal void
addKeyEvent( KEYBOARD_STATE * Keyboard, KEY_STATE_CODE KeyCode, boo32 IsDown, uint8 flags ) {
	Assert( Keyboard->nEvents < KEYBOARD_STATE__MAX_EVENT_COUNT );
	Keyboard->Event[ Keyboard->nEvents++ ] = KeyState( KeyCode, ( uint8 )IsDown, flags );
}

//----------
// Controller functions
//----------

internal boo32
IsDown( CONTROLLER_STATE * Controller, CONTROLLER_BUTTON_ID buttonID ) {
    CONTROLLER_BUTTON_STATE * button = Controller->button + buttonID;
    boo32  Result = button->IsDown;
    return Result;
}

internal boo32
wasButtonPressed( CONTROLLER_BUTTON_STATE * button ) {
	boo32 Result = false;
	if( (  button->IsDown && ( button->HalfTransitionCount >= 1 ) ) ||
       ( !button->IsDown && ( button->HalfTransitionCount >= 2 ) ) ) {
		Result = true;
	}
	return Result;
}

internal boo32
WasPressed( CONTROLLER_STATE * Controller, CONTROLLER_BUTTON_ID buttonID ) {
	CONTROLLER_BUTTON_STATE * button = Controller->button + buttonID;
	boo32 Result = wasButtonPressed( button );
	return Result;
}

internal boo32
WasReleased( CONTROLLER_BUTTON_STATE * button ) {
	boo32 Result = false;
	if( ( !button->IsDown && ( button->HalfTransitionCount >= 1 ) ) ||
       (  button->IsDown && ( button->HalfTransitionCount >= 2 ) ) ) {
		Result = true;
	}
	return Result;
}

internal boo32
WasReleased( CONTROLLER_STATE * Controller, CONTROLLER_BUTTON_ID buttonID ) {
	CONTROLLER_BUTTON_STATE * button = Controller->button + buttonID;
	boo32 Result = WasReleased( button );
	return Result;
}

internal void
UpdateButton( CONTROLLER_STATE * Controller, CONTROLLER_BUTTON_ID buttonID, boo32 IsDown ) {
    CONTROLLER_BUTTON_STATE * button = Controller->button + buttonID;
    if( (  button->IsDown && !IsDown ) ||
       ( !button->IsDown &&  IsDown ) ) {
        button->HalfTransitionCount++;
    }
    button->IsDown = IsDown;
}

#endif	// STD_INCLUDE_FUNC