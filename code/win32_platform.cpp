
internal void
genFilePath( char * FilePath, char * SaveDir, char * FileName, char * extension ) {
	if( SaveDir ) {
        strcpy( FilePath, SaveDir );
        strcat( FilePath, "/" );
	}
    strcat( FilePath, FileName );
    if( extension ) {
        strcat( FilePath, "." );
        strcat( FilePath, extension );
    }
}

internal FILE_DATA
Win32_ReadFile( MEMORY * memory, char * SaveDir, char * FileName, char * extension ) {
	FILE_DATA Result = {};
	
	char FilePath[ 2048 ] = {};
    genFilePath( FilePath, SaveDir, FileName, extension );
	
	HANDLE File = CreateFile( FilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if( File != INVALID_HANDLE_VALUE ) {
		LARGE_INTEGER _FileSize = {};
		if( GetFileSizeEx( File, &_FileSize ) ) {
            uint32 FileSize = ( uint32 )_FileSize.QuadPart;
            uint8 * dest = ( uint8 * )_pushSize( memory, FileSize );
			
			DWORD bytesRead = {};
            BOOL  Status    = ReadFile( File, dest, FileSize, &bytesRead, 0 );
            if( ( Status ) && ( FileSize == bytesRead ) ) {
                Result.contents = dest;
                Result.size     = FileSize;
			} else {
				_popSize( memory, Result.size );
                
                char Debug[ 128 ] {};
                sprintf( Debug, "ERROR! File: %s : unable to read contents!\n", FilePath );
                OutputDebugString( Debug );
			}
			CloseHandle( File );
		} else {
            char Debug[ 128 ] = {};
            sprintf( Debug, "ERROR! File: %s : does not contain any data!\n", FilePath );
            OutputDebugString( Debug );
		}
	} else {
        char Debug[ 128 ] = {};
        sprintf( Debug, "ERROR! File: %s : unable to open!\n", FilePath );
        OutputDebugString( Debug );
	}
	
	return Result;
}

internal boo32
Win32_WriteFile( char * SaveDir, char * FileName, char * extension, void * data, uint32 size ) {
	boo32 Result = false;
	
	char FilePath[ 2048 ] = {};
    genFilePath( FilePath, SaveDir, FileName, extension );
    
	HANDLE FileHandle = CreateFile( FilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0 );
	if( FileHandle != INVALID_HANDLE_VALUE ) {
		DWORD bytesWritten;
		if( WriteFile( FileHandle, data, size, &bytesWritten, 0 ) ) {
			Result = ( bytesWritten == size );
		}
        
		CloseHandle( FileHandle );
	}
    
	return Result;
}

internal boo32
Win32_DoesFileExist( char * SaveDir, char * FileName, char * extension ) {
	char FilePath[ 2048 ] = {};
    genFilePath( FilePath, SaveDir, FileName, extension );
    
    boo32 Result = PathFileExists( FilePath );
    return Result;
}

internal boo32
Win32_DeleteFile( char * SaveDir, char * FileName, char * extension ) {
	char FilePath[ 2048 ] = {};
    genFilePath( FilePath, SaveDir, FileName, extension );
    
    boo32 Result = DeleteFile( FilePath );
    return Result;
}

uint32 Frame_Counter = 0;
internal void
Win32_outputMessage( char * header, char * message ) {
    char str[ 64 ] = {};
    sprintf( str, "%10u : %8s : %s\n", Frame_Counter, header, message );
    OutputDebugString( str );
}

internal void
Win32_logMessage( uint32 message, boo32 fromWndProc ) {
    char * header0 = "WndProc";
    char * header1 = "my";
    
    char * header = ( fromWndProc ? header0 : header1 );
    
    switch( message ) {
        case WM_ACTIVATE: {
            // Sent to both the window being activated and the window being deactivated. If the windows use the same input queue, the message is sent synchronously, first to the window procedure of the Top-level window being deactivated, then to the window procedure of the Top-level window being activated. If the windows use different input queues, the message is sent asynchronously, so the window is activated immediately.
            Win32_outputMessage( header, "WM_ACTIVATE" );
        } break;
        
        case WM_ACTIVATEAPP: {
            // Sent when the active window changes. Sent to both the activated window and the deactivated window.
            Win32_outputMessage( header, "WM_ACTIVATEAPP" );
        } break;
        
        case WM_CAPTURECHANGED: {
            // Sent to the window that is losing the Mouse capture.
            Win32_outputMessage( header, "WM_CAPTURECHANGED" );
        } break;
        
        case WM_CLOSE: {
            // Sent as a signal that a window or an application should terminate.
            Win32_outputMessage( header, "WM_CLOSE" );
        } break;
        
        case WM_DESTROY: {
            // Sent when a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen.
            Win32_outputMessage( header, "WM_DESTROY" );
        } break;
        
        case WM_GETMINMAXINFO: {
            // Sent to a window when its size or Position is about to change.
            Win32_outputMessage( header, "WM_GETMINMAXINFO" );
        } break;
        
        case WM_GETICON: {
            // Sent to a window to retrieve the handle to the large or small icon associated with the window.
            Win32_outputMessage( header, "WM_GETICON" );
        } break;
        
        case WM_IME_SETCONTEXT: {
            // Sent to an application when a window is activated. A window receives this message through its WindowProc function.
            Win32_outputMessage( header, "WM_IME_SETCONTEXT" );
        } break;
        
        case WM_IME_NOTIFY: {
            // Sent to an application to notify it of changes to the IME window. A window receives this message through its WindowProc function.
            Win32_outputMessage( header, "WM_IME_NOTIFY" );
        } break;
        
        case WM_KILLFOCUS: {
            // Sent to a window immediately before it loses the Keyboard focus.
            Win32_outputMessage( header, "WM_KILLFOCUS" );
        } break;
        
        case WM_SETFOCUS: {
            // Sent to a window after it has gained the Keyboard focus.
            Win32_outputMessage( header, "WM_KILLFOCUS" );
        } break;
        
        case WM_MOUSEMOVE: {
            // Posted to a window when the cursor moves. If the Mouse is not captured, the message is posted to the window that contains the cursor. Otherwise, the message is posted to the window that has captured the Mouse.
            // Win32_outputMessage( header, "WM_MOUSEMOVE" );
        } break;
        
        case WM_NCACTIVATE: {
            // Sent to a window when its nonclient area needs to be changed to indicate an active or inactive State.
            Win32_outputMessage( header, "WM_NCACTIVATE" );
        } break;
        
        case WM_NCCREATE: {
            // Sent when a window is first created. Sent prior to the WM_CREATE message.
            Win32_outputMessage( header, "WM_NCCREATE" );
        } break;
        
        case WM_NCCALCSIZE: {
            // Sent when the size and Position of a window's client area must be calculated.
            Win32_outputMessage( header, "WM_NCCALCSIZE" );
        } break;
        
        case WM_NCDESTROY: {
            // Notifies a window that its nonclient area is being destroyed. The DestroyWindow function sends the WM_NCDESTROY message to the window following the WM_DESTROY message.WM_DESTROY is used to free the allocated memory object associated with the window. The WM_NCDESTROY message is sent after the child windows have been destroyed. In contrast, WM_DESTROY is sent before the child windows are destroyed. A window receives this message through its WindowProc function.
            Win32_outputMessage( header, "WM_NCDESTROY" );
        } break;
        
        case WM_NCHITTEST: {
            // Sent to a window to determine what part of the window corresponds to a particular screen coordinate. Sent, for example, in response to Mouse cursor movement or when a Mouse button is pressed/released.
            // Win32_outputMessage( header, "WM_NCHITTEST" );
        } break;
        
        case WM_NCLBUTTONDOWN: {
            // Sent to a window when the user presses the Left Mouse button while the cursor is within the nonclient area of the window.
            Win32_outputMessage( header, "WM_NCLBUTTONDOWN" );
        } break;
        
        case WM_NCMOUSEMOVE: {
            // Posted to a window when the cursor is moved within the nonclient area of the window. This message is posted to the window that contains the cursor. If a window has captured the Mouse, this message is not posted.
            // Win32_outputMessage( header, "WM_NCMOUSEMOVE" );
        } break;
        
        case WM_PAINT: {
            // Sent when the system or another application makes a request to paint a portion of the application's window.
            Win32_outputMessage( header, "WM_PAINT" );
        } break;
        
        case WM_QUIT: {
            // Indicates a request to terminate an application, and is generated when the application calls the PostQuitMessage function.
            Win32_outputMessage( header, "WM_QUIT" );
        } break;
        
        case WM_SETCURSOR: {
            // Sent if the Mouse cursor moves, but the input is not captured by the window.
            //Win32_outputMessage( header, "WM_SETCURSOR" );
        } break;
        
        case WM_SYSCOMMAND: {
            // A window receives this message when the user chooses a command from the Window menu (formerly known as the system or control menu) or when the user chooses the maximize button, minimize button, restore button, or close button.
            Win32_outputMessage( header, "WM_SYSCOMMAND" );
        } break;
		
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            Win32_outputMessage( header, "Windows Key Message" );
            // ERROR! Keyboard message was Passed directly to WindowProc! All Keyboard messages should be handled directly by the Win32_processPendingMessages()!
        } break;
        
        case WM_TIMER: {
            // Posted to the installing thread's message queue when a Timer expires. The message is posted by the GetMessage or PeekMessage function.
            // Win32_outputMessage( header, "WM_TIMER" );
        } break;
        
        case WM_WINDOWPOSCHANGED: {
            // Sent to a window whose size, Position, or place in the Z order has changed as a Result of a call to the SetWindowPos function or another window-management function.
            Win32_outputMessage( header, "WM_WINDOWPOSCHANGED" );
        } break;
        
        case WM_WINDOWPOSCHANGING: {
            // Sent to a window whose size, Position, or place in the Z order is about to change as a Result of a call to the SetWindowPos function or another window-management function.
            Win32_outputMessage( header, "WM_WINDOWPOSCHANGING" );
        } break;
        
        default: {
            char str[ 8 ] = {};
            sprintf( str, "0x%04X", ( uint32 )message );
            Win32_outputMessage( header, str );
        } break;
    }
}

global_variable boo32 global_restoreFocus = false;

LRESULT CALLBACK
Win32_WindowProc( HWND window, uint32 message, WPARAM wParam, LPARAM lParam ) {
    //Win32_logMessage( message, true );
	LRESULT Result = 0;
    switch( message ) {
        case WM_ACTIVATEAPP: {
            global_restoreFocus = true;
        } break;
        
        case WM_DESTROY: {
            PostQuitMessage( 0 );
        } break;
        
        default: {
            Result = DefWindowProc( window, message, wParam, lParam );
        } break;
    }
    return Result;
}

internal void
Win32_processPendingMessages( PLATFORM * Platform, HWND windowHandle ) {
    KEYBOARD_STATE * Keyboard = &Platform->Keyboard;
    Keyboard->nEvents = 0;
    
    MOUSE_STATE * Mouse = &Platform->Mouse;
    MOUSE_endOfFrame( Mouse );
    
	MSG message = {};
    while( PeekMessage( &message, 0, 0, 0, PM_REMOVE ) ) {
        //Win32_logMessage( message.message, false );
        
        switch( message.message ) {
            case WM_QUIT: {
                Platform->IsRunning = false;
            } break;
            
			case WM_MOUSEWHEEL: {
				int16 wParam_hi = ( ( message.wParam >> 16 ) & 0xFFFF );
				int32 WheelClicks = wParam_hi / 120;
				Mouse->WheelClicks = WheelClicks;
			} break;
			
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP: {
				addButtonEvent( Mouse, MouseButton_Left, ( message.wParam & MK_LBUTTON ), Keyboard->flags );
			} break;
			
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP: {
				addButtonEvent( Mouse, MouseButton_middle, ( message.wParam & MK_MBUTTON ), Keyboard->flags );
			} break;
			
			case WM_RBUTTONUP:
			case WM_RBUTTONDOWN: {
				addButtonEvent( Mouse, MouseButton_Right, ( message.wParam & MK_RBUTTON ), Keyboard->flags );
			} break;
			
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP: {
                boo32 wasDown = ( ( message.lParam & ( 1 << 30 ) ) != 0 );
				boo32 IsDown  = ( ( message.lParam & ( 1 << 31 ) ) == 0 );
				if( IsDown != wasDown ) {
                    switch( message.wParam ) {
						case VK_BACK: { addKeyEvent( Keyboard, KeyCode_Backspace, IsDown, Keyboard->flags ); } break;
						case VK_TAB: 		{ addKeyEvent( Keyboard, KeyCode_tab, IsDown, Keyboard->flags ); } break;
						case VK_RETURN:	{ addKeyEvent( Keyboard, KeyCode_Enter, IsDown, Keyboard->flags ); } break;
						case VK_SHIFT: 	{
                            if( IsDown ) {
                                Keyboard->flags |=  KEYBOARD_FLAGS__SHIFT;
                            } else {
                                Keyboard->flags &= ~KEYBOARD_FLAGS__SHIFT;
                            }
							addKeyEvent( Keyboard, KeyCode_shift, IsDown, Keyboard->flags );
						} break;
						case VK_CONTROL: {
                            if( IsDown ) {
                                Keyboard->flags |=  KEYBOARD_FLAGS__CONTROL;
                            } else {
                                Keyboard->flags &= ~KEYBOARD_FLAGS__CONTROL;
                            }
							addKeyEvent( Keyboard, KeyCode_control, IsDown, Keyboard->flags );
						} break;
						case VK_MENU: 		{
                            if( IsDown ) {
                                Keyboard->flags |=  KEYBOARD_FLAGS__ALT;
                            } else {
                                Keyboard->flags &= ~KEYBOARD_FLAGS__ALT;
                            }
                            addKeyEvent( Keyboard, KeyCode_alt, IsDown, Keyboard->flags );
                        } break;
						case VK_CAPITAL: 	{ addKeyEvent( Keyboard, KeyCode_capsLock, 	IsDown, Keyboard->flags ); } break;
						case VK_ESCAPE: 	{ addKeyEvent( Keyboard, KeyCode_Escape, 		IsDown, Keyboard->flags ); } break;
						case VK_SPACE: 	{ addKeyEvent( Keyboard, KeyCode_space, 		IsDown, Keyboard->flags ); } break;
						case VK_PRIOR: 	{ addKeyEvent( Keyboard, KeyCode_pageUp, 		IsDown, Keyboard->flags ); } break;
						case VK_NEXT: 		{ addKeyEvent( Keyboard, KeyCode_pageDown, 	IsDown, Keyboard->flags ); } break;
						case VK_END: 		{ addKeyEvent( Keyboard, KeyCode_end, 			IsDown, Keyboard->flags ); } break;
						case VK_HOME: 		{ addKeyEvent( Keyboard, KeyCode_home, 		IsDown, Keyboard->flags ); } break;
						case VK_LEFT: 		{ addKeyEvent( Keyboard, KeyCode_Left, 		IsDown, Keyboard->flags ); } break;
						case VK_RIGHT: 	{ addKeyEvent( Keyboard, KeyCode_Right, 		IsDown, Keyboard->flags ); } break;
						case VK_UP: 		{ addKeyEvent( Keyboard, KeyCode_Up, 			IsDown, Keyboard->flags ); } break;
						case VK_DOWN: 		{ addKeyEvent( Keyboard, KeyCode_Down,       IsDown, Keyboard->flags ); } break;
						case VK_INSERT: 	{ addKeyEvent( Keyboard, KeyCode_insert, 		IsDown, Keyboard->flags ); } break;
						case VK_DELETE: 	{ addKeyEvent( Keyboard, KeyCode_delete, 		IsDown, Keyboard->flags ); } break;
						case 0x30: { addKeyEvent( Keyboard, KeyCode_0, IsDown, Keyboard->flags ); } break;
						case 0x31: { addKeyEvent( Keyboard, KeyCode_1, IsDown, Keyboard->flags ); } break;
						case 0x32: { addKeyEvent( Keyboard, KeyCode_2, IsDown, Keyboard->flags ); } break;
						case 0x33: { addKeyEvent( Keyboard, KeyCode_3, IsDown, Keyboard->flags ); } break;
						case 0x34: { addKeyEvent( Keyboard, KeyCode_4, IsDown, Keyboard->flags ); } break;
						case 0x35: { addKeyEvent( Keyboard, KeyCode_5, IsDown, Keyboard->flags ); } break;
						case 0x36: { addKeyEvent( Keyboard, KeyCode_6, IsDown, Keyboard->flags ); } break;
						case 0x37: { addKeyEvent( Keyboard, KeyCode_7, IsDown, Keyboard->flags ); } break;
						case 0x38: { addKeyEvent( Keyboard, KeyCode_8, IsDown, Keyboard->flags ); } break;
						case 0x39: { addKeyEvent( Keyboard, KeyCode_9, IsDown, Keyboard->flags ); } break;
						case 0x41: { addKeyEvent( Keyboard, KeyCode_a, IsDown, Keyboard->flags ); } break;
						case 0x42: { addKeyEvent( Keyboard, KeyCode_b, IsDown, Keyboard->flags ); } break;
						case 0x43: { addKeyEvent( Keyboard, KeyCode_c, IsDown, Keyboard->flags ); } break;
						case 0x44: { addKeyEvent( Keyboard, KeyCode_d, IsDown, Keyboard->flags ); } break;
						case 0x45: { addKeyEvent( Keyboard, KeyCode_e, IsDown, Keyboard->flags ); } break;
						case 0x46: { addKeyEvent( Keyboard, KeyCode_f, IsDown, Keyboard->flags ); } break;
						case 0x47: { addKeyEvent( Keyboard, KeyCode_g, IsDown, Keyboard->flags ); } break;
						case 0x48: { addKeyEvent( Keyboard, KeyCode_h, IsDown, Keyboard->flags ); } break;
						case 0x49: { addKeyEvent( Keyboard, KeyCode_i, IsDown, Keyboard->flags ); } break;
						case 0x4A: { addKeyEvent( Keyboard, KeyCode_j, IsDown, Keyboard->flags ); } break;
						case 0x4B: { addKeyEvent( Keyboard, KeyCode_k, IsDown, Keyboard->flags ); } break;
						case 0x4C: { addKeyEvent( Keyboard, KeyCode_l, IsDown, Keyboard->flags ); } break;
						case 0x4D: { addKeyEvent( Keyboard, KeyCode_m, IsDown, Keyboard->flags ); } break;
						case 0x4E: { addKeyEvent( Keyboard, KeyCode_n, IsDown, Keyboard->flags ); } break;
						case 0x4F: { addKeyEvent( Keyboard, KeyCode_o, IsDown, Keyboard->flags ); } break;
						case 0x50: { addKeyEvent( Keyboard, KeyCode_p, IsDown, Keyboard->flags ); } break;
						case 0x51: { addKeyEvent( Keyboard, KeyCode_q, IsDown, Keyboard->flags ); } break;
						case 0x52: { addKeyEvent( Keyboard, KeyCode_r, IsDown, Keyboard->flags ); } break;
						case 0x53: { addKeyEvent( Keyboard, KeyCode_s, IsDown, Keyboard->flags ); } break;
						case 0x54: { addKeyEvent( Keyboard, KeyCode_t, IsDown, Keyboard->flags ); } break;
						case 0x55: { addKeyEvent( Keyboard, KeyCode_u, IsDown, Keyboard->flags ); } break;
						case 0x56: { addKeyEvent( Keyboard, KeyCode_v, IsDown, Keyboard->flags ); } break;
						case 0x57: { addKeyEvent( Keyboard, KeyCode_w, IsDown, Keyboard->flags ); } break;
						case 0x58: { addKeyEvent( Keyboard, KeyCode_x, IsDown, Keyboard->flags ); } break;
						case 0x59: { addKeyEvent( Keyboard, KeyCode_y, IsDown, Keyboard->flags ); } break;
						case 0x5A: { addKeyEvent( Keyboard, KeyCode_z, IsDown, Keyboard->flags ); } break;
						case VK_OEM_1: { addKeyEvent( Keyboard, KeyCode_semicolon, IsDown, Keyboard->flags ); } break;
						case VK_OEM_PLUS: { addKeyEvent( Keyboard, KeyCode_equal, IsDown, Keyboard->flags ); } break;
						case VK_OEM_COMMA: { addKeyEvent( Keyboard, KeyCode_comma, IsDown, Keyboard->flags ); } break;
						case VK_OEM_MINUS: { addKeyEvent( Keyboard, KeyCode_dash, IsDown, Keyboard->flags ); } break;
						case VK_OEM_PERIOD: { addKeyEvent( Keyboard, KeyCode_period, IsDown, Keyboard->flags ); } break;
						case VK_OEM_2: { addKeyEvent( Keyboard, KeyCode_forwardSlash, IsDown, Keyboard->flags ); } break;
						case VK_OEM_3: { addKeyEvent( Keyboard, KeyCode_tilde, IsDown, Keyboard->flags ); } break;
						case VK_OEM_4: { addKeyEvent( Keyboard, KeyCode_openBracket, IsDown, Keyboard->flags ); } break;
						case VK_OEM_5: { addKeyEvent( Keyboard, KeyCode_BackSlash, IsDown, Keyboard->flags ); } break;
						case VK_OEM_6: { addKeyEvent( Keyboard, KeyCode_closeBracket, IsDown, Keyboard->flags ); } break;
						case VK_OEM_7: { addKeyEvent( Keyboard, KeyCode_quote, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD0: { addKeyEvent( Keyboard, KeyCode_num0, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD1: { addKeyEvent( Keyboard, KeyCode_num1, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD2: { addKeyEvent( Keyboard, KeyCode_num2, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD3: { addKeyEvent( Keyboard, KeyCode_num3, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD4: {addKeyEvent( Keyboard, KeyCode_num4, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD5: { addKeyEvent( Keyboard, KeyCode_num5, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD6: { addKeyEvent( Keyboard, KeyCode_num6, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD7: { addKeyEvent( Keyboard, KeyCode_num7, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD8: { addKeyEvent( Keyboard, KeyCode_num8, IsDown, Keyboard->flags ); } break;
						case VK_NUMPAD9: { addKeyEvent( Keyboard, KeyCode_num9, IsDown, Keyboard->flags ); } break;
						case VK_MULTIPLY: { addKeyEvent( Keyboard, KeyCode_numMul, IsDown, Keyboard->flags ); } break;
						case VK_ADD: { addKeyEvent( Keyboard, KeyCode_numAdd, IsDown, Keyboard->flags ); } break;
						case VK_DECIMAL: { addKeyEvent( Keyboard, KeyCode_numPeriod, IsDown, Keyboard->flags ); } break;
						case VK_SUBTRACT: { addKeyEvent( Keyboard, KeyCode_numSub, IsDown, Keyboard->flags ); } break;
						case VK_DIVIDE: { addKeyEvent( Keyboard, KeyCode_numDiv, IsDown, Keyboard->flags ); } break;
						case VK_F1:      { addKeyEvent( Keyboard, KeyCode_F1,   IsDown, Keyboard->flags ); } break;
						case VK_F2:      { addKeyEvent( Keyboard, KeyCode_F2,   IsDown, Keyboard->flags ); } break;
						case VK_F3:      { addKeyEvent( Keyboard, KeyCode_F3,   IsDown, Keyboard->flags ); } break;
						case VK_F4:      { addKeyEvent( Keyboard, KeyCode_F4,   IsDown, Keyboard->flags ); } break;
						case VK_F5:      { addKeyEvent( Keyboard, KeyCode_F5,   IsDown, Keyboard->flags ); } break;
						case VK_F6:      { addKeyEvent( Keyboard, KeyCode_F6,   IsDown, Keyboard->flags ); } break;
						case VK_F7:      { addKeyEvent( Keyboard, KeyCode_F7,   IsDown, Keyboard->flags ); } break;
						case VK_F8:      { addKeyEvent( Keyboard, KeyCode_F8,   IsDown, Keyboard->flags ); } break;
						case VK_F9:      { addKeyEvent( Keyboard, KeyCode_F9,   IsDown, Keyboard->flags ); } break;
						case VK_F10:     { addKeyEvent( Keyboard, KeyCode_F10,  IsDown, Keyboard->flags ); } break;
						case VK_F11:     { addKeyEvent( Keyboard, KeyCode_F11,  IsDown, Keyboard->flags ); } break;
						case VK_F12:     { addKeyEvent( Keyboard, KeyCode_F12,  IsDown, Keyboard->flags ); } break;
						default: {
							char string[ 128 ] = {};
							sprintf( string, "Key message received, but not processed: %lu %s\n", (uint32)message.wParam, ( IsDown ? "PRESSED" : "RELEASED" ) );
							OutputDebugString( string );
						} break;
					};
				}
			} break;
            
            default: {
                TranslateMessage( &message );
                DispatchMessage ( &message );
            } break;
        }
	}
}

internal void
InitPass( RENDERER * Renderer, RENDER_PASS * Pass, MEMORY * memory, uint32 nTri, uint32 nLine, uint32 nText ) {
    Pass->triObjectList.maxObjects  = nTri;
    Pass->lineObjectList.maxObjects = nLine;
    Pass->textList.maxObjects       = nText;
    
    Pass->triObjectList.object  = _pushArray( memory,      RENDER_OBJECT, nTri  );
    Pass->lineObjectList.object = _pushArray( memory,      RENDER_OBJECT, nLine );
    Pass->textList.object       = _pushArray( memory, TEXT_RENDER_OBJECT, nText + DEBUG_SYSTEM__SAFETY_OBJECT_COUNT );
    
    Pass->ModelBuffer = &Renderer->ModelBuffer;
    Pass->textBuffer  = &Renderer->textBuffer;
}

typedef DWORD XINPUT_GET_STATE( DWORD ControllerIndex, XINPUT_STATE     * ControllerState     );
typedef DWORD XINPUT_SET_STATE( DWORD ControllerIndex, XINPUT_VIBRATION * ControllerVibration );

internal void
RenderPass( RENDERER * Renderer, RENDER_PASS * Pass, mat4 Camera_transform ) {
    ID3D12GraphicsCommandList * CommandList = Renderer->CommandList;
    
    { // Draw Models
        VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
        CommandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
        CommandList->IASetIndexBuffer  ( &buffer->indexView );
        
        CommandList->SetGraphicsRoot32BitConstants( 0, 16, Camera_transform.elem, 0 );
        
        { // triangle Models
            RENDER_OBJECT_LIST * objectList = &Pass->triObjectList;
            
            CommandList->SetPipelineState( Renderer->PSS_tri_noDepth_blend );
            CommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
            for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                RENDER_OBJECT object    = objectList->object[ iObject ];
                MODEL_DATA    ModelData = Renderer->ModelData[ object.ModelID ];
                
                CommandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                
                TEXTURE_ID TextureID = TextureID_DefaultTexture;
                if( Renderer->Texture_isLoaded[ object.TextureID ] ) {
                    TextureID = object.TextureID;
                }
                D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, TextureID );
                CommandList->SetGraphicsRootDescriptorTable( 2, handle );
                
                CommandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                
                CommandList->DrawIndexedInstanced( ModelData.nIndex, 1, ModelData.bIndex, ModelData.bVertex, 0 );
            }
            objectList->nObjects = 0;
        } // END triangle Models
        
        { // line Models
            RENDER_OBJECT_LIST * objectList = &Pass->lineObjectList;
            
            CommandList->SetPipelineState( Renderer->PSS_line_noDepth_blend );
            CommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINESTRIP );
            for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                RENDER_OBJECT object    = objectList->object[ iObject ];
                MODEL_DATA    ModelData = Renderer->ModelData[ object.ModelID ];
                
                CommandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                
                D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, object.TextureID );
                CommandList->SetGraphicsRootDescriptorTable( 2, handle );
                
                CommandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                
                CommandList->DrawIndexedInstanced( ModelData.nIndex, 1, ModelData.bIndex, ModelData.bVertex, 0 );
            }
            objectList->nObjects = 0;
        } // END line Models
    }
    
    { // Draw overlay text
        CommandList->SetPipelineState( Renderer->PSS_tri_noDepth_blend );
        
        VERTEX1_BUFFER * buffer = &Renderer->textBuffer;
        buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
        
        CommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        CommandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
        
        D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( Renderer->device, Renderer->SRVHeap, TextureID_Font );
        CommandList->SetGraphicsRootDescriptorTable( 2, handle );
        
        //mat4 Camera_transform = mat4_orthographic( App_Bound );
        CommandList->SetGraphicsRoot32BitConstants( 0, 16, Camera_transform.elem, 0 );
        
        mat4 Model_transform = mat4_idEntity();
        CommandList->SetGraphicsRoot32BitConstants( 0, 16, Model_transform.elem, 16 );
        
        TEXT_RENDER_OBJECT_LIST * objectList = &Pass->textList;
        for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
            TEXT_RENDER_OBJECT object = objectList->object[ iObject ];
            
            CommandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
            
            CommandList->DrawInstanced( object.nVertex, 1, object.bVertex, 0 );
        }
        objectList->nObjects = 0;
    }
}

internal int64
Win32_GetPerfCount() {
	LARGE_INTEGER PerfCounter = {};
	QueryPerformanceCounter( &PerfCounter );
	
	int64 Result = *( ( int64 * )&PerfCounter );
	return Result;
}


internal flo32
Win32_GetMSElapsed( int64 StartCounter, int64 EndCounter, int64 PerfFrequency ) {
	LARGE_INTEGER CounterA  = *( ( LARGE_INTEGER * )&StartCounter  );
	LARGE_INTEGER CounterB  = *( ( LARGE_INTEGER * )&EndCounter    );
	LARGE_INTEGER Frequency = *( ( LARGE_INTEGER * )&PerfFrequency );
	flo32 Result = 1000.0f * ( ( flo32 )( CounterB.QuadPart - CounterA.QuadPart ) / ( flo32 )( Frequency.QuadPart ) );
	return Result;
}

int32 CALLBACK
WinMain( HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int32 windowShowCode ) {
    srand( ( uint32 )time( 0 ) );
    
	boo32 SleepFreqIsHiRes = ( timeBeginPeriod( 1 ) == TIMERR_NOERROR );
    Assert( SleepFreqIsHiRes );
    
    LARGE_INTEGER PerfFrequency;
    QueryPerformanceFrequency( &PerfFrequency );
    
    uint32 memory_size = 0;
    memory_size += sizeof( APP_STATE );
    memory_size += sizeof( PLATFORM  );
    memory_size += APP_PermMemorySize;
    memory_size += APP_TempMemorySize;
    
    MEMORY PlatformMemory = {};
    PlatformMemory.size = memory_size;
    PlatformMemory.base = VirtualAlloc( 0, memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
    
    APP_STATE * AppState = _pushType( &PlatformMemory, APP_STATE );
    PLATFORM  * Platform = _pushType( &PlatformMemory, PLATFORM  );
    Platform->TargetSec  = 1.0f / APP_TargetFPS;
    
    Platform->PermMemory = SubArena( &PlatformMemory, APP_PermMemorySize );
    Platform->TempMemory = SubArena( &PlatformMemory, APP_TempMemorySize );
    Platform->ReadFile      = Win32_ReadFile;
    Platform->WriteFile     = Win32_WriteFile;
    Platform->DoesFileExist = Win32_DoesFileExist;
    Platform->DeleteFile    = Win32_DeleteFile;
    
    D3D12_EnableDebug();
    DIRECTX_12_DISPLAY_SETTINGS DisplaySettings = getDisplay( &Platform->TempMemory );
    
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc   = Win32_WindowProc;
    windowClass.hInstance     = instance;
    windowClass.lpszClassName = "WindowClass";
    windowClass.hCursor       = LoadCursor( 0, IDC_ARROW );
    
    if( RegisterClass( &windowClass ) ) {
        // uint32 window_flags = WS_OVERLAPPEDWINDOW; // Windowed
        uint32 window_flags = ( WS_VISIBLE | WS_EX_TOPMOST | WS_POPUP ); // Fullscreen
        HWND window = CreateWindowEx( 0, "WindowClass", "Caption", window_flags, 0, 0, DisplaySettings.Width, DisplaySettings.Height, 0, 0, instance, 0 );
        
        if( window ) {
            Platform->Renderer = initDirectX12( DisplaySettings, window );
            
            RENDERER     * Renderer    = &Platform->Renderer;
            MEMORY       * PermMemory  = &Platform->PermMemory;
            MEMORY       * TempMemory  = &Platform->TempMemory;
            AUDIO_SYSTEM * AudioSystem = &Platform->AudioSystem;
            
            CreateShader( Renderer, DisplaySettings );
            
            ShowWindow( window, SW_SHOW );
            
            Platform->IsRunning = true;
            
            FONT Font = LoadFont( 16.0f );
            
            ResetCommandList( Renderer );
            
            RENDER_PASS * Pass_Game = &Renderer->Pass_Game;
            RENDER_PASS * Pass_UI   = &Renderer->Pass_UI;
            
            InitPass( Renderer, Pass_Game, PermMemory, 32768, 4096, 512 );
            InitPass( Renderer, Pass_UI,   PermMemory, 4096,  4096, 512 );
            Renderer->ModelData[ ModelID_line          ] = genLine         ( Renderer );
            Renderer->ModelData[ ModelID_rect          ] = genRect         ( Renderer );
            Renderer->ModelData[ ModelID_rectOutline   ] = genRectOutline  ( Renderer );
            Renderer->ModelData[ ModelID_circle        ] = genCircle       ( Renderer );
            Renderer->ModelData[ ModelID_circleOutline ] = genCircleOutline( Renderer );
            Renderer->ModelData[ ModelID_box           ] = genBox          ( Renderer );
            Renderer->ModelData[ ModelID_Triangle      ] = GenTriangle     ( Renderer );
            Renderer->ModelData[ ModelID_Hexagon       ] = GenHexagon      ( Renderer );
            Renderer->ModelData[ ModelID_Octagon       ] = GenOctagon      ( Renderer );
            
            Renderer->ModelData[ ModelID_Player_WallSlide ] = GenModel_PlayerWallSlide( Renderer );
            Renderer->ModelData[ ModelID_Spikes           ] = GenModel_Spikes         ( Renderer );
            
            { // create and Upload white Texture
                uint32 whiteTexture = 0xFFFFFFFF;
                UploadTexture( Renderer, TextureID_WhiteTexture, 1, 1, &whiteTexture );
            }
            { // create and Upload default Debug Texture
                uint32 DefaultTexture[ 256 * 256 ] = {};
                
                uint32 Width  = 256;
                uint32 Height = 256;
                uint32 nTexel = Width * Height;
                for( uint32 iter = 0; iter < nTexel; iter++ ) {
                    uint32 iRow = ( iter / 8192 );
                    uint32 iCol = ( iter / 32 ) % 8;
                    
                    if( ( ( ( iRow + iCol ) % 2 ) == 0 ) ) {
                        DefaultTexture[ iter ] = 0xFFFF00FF;
                    } else {
                        DefaultTexture[ iter ] = 0xFF444444;
                    }
                }
                UploadTexture( Renderer, TextureID_DefaultTexture, Width, Height, &DefaultTexture );
            }
            
            UploadTexture( Renderer, TextureID_Font, Font.Texture_Width, Font.Texture_Height, Font.Texture_data );
            
            executeCommandList( Renderer );
            
            WIN32_AUDIO Win32_Audio = InitAudio( window, AudioSystem, PermMemory );
            
            vec2 App_Dim     = Vec2( ( flo32 )DisplaySettings.Width, ( flo32 )DisplaySettings.Height );
            vec2 App_HalfDim = App_Dim * 0.5f;
            rect App_Bound   = RectBLD( Vec2( 0.0f, 0.0f ), App_Dim );
            
            AppState->App_Dim     = App_Dim;
            AppState->App_HalfDim = App_HalfDim;
            AppState->App_Bound   = App_Bound;
            AppState->Font        = &Font;
            
            vec2 DebugSystem_BasePos = GetTL( App_Bound ) + Vec2( 10.0f, -14.0f );
            GlobalVar_DebugSystem.Pass_Game     = Pass_Game;
            GlobalVar_DebugSystem.Pass_UI       = Pass_UI;
            GlobalVar_DebugSystem.Font          = &Font;
            GlobalVar_DebugSystem.advanceHeight = Font.advanceHeight;
            Frame_Counter++;
            
            MOUSE_STATE      * Mouse      = &Platform->Mouse;
            KEYBOARD_STATE   * Keyboard   = &Platform->Keyboard;
            CONTROLLER_STATE * Controller = &Platform->Controller;
            Mouse->BasePos = App_HalfDim;
            
            HMODULE XInputLib = LoadLibraryA( "xinput1_3.dll" );
            XINPUT_GET_STATE * XInputGetState = 0;
            XINPUT_SET_STATE * XInputSetState = 0;
            if( XInputLib ) {
                XInputGetState = ( XINPUT_GET_STATE * )GetProcAddress( XInputLib, "XInputGetState" );
                XInputSetState = ( XINPUT_SET_STATE * )GetProcAddress( XInputLib, "XInputSetState" );
                
                Controller->Left.dead  = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
                Controller->Right.dead = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
            } else {
                InvalidCodePath;
            }
            
            ShowCursor( true );
            
#if   START_IN_EDITOR
            AppState->Mode = AppMode_Editor;
#elif START_IN_LAYOUT
            AppState->Mode = AppMode_Layout;
#endif
            AppState->Collision_Memory  = SubArena( PermMemory, _MB( 1 ) );
            AppState->Event_Memory      = SubArena( PermMemory, _MB( 1 ) );
            AppState->ParticleS.Particle = _pushArray( PermMemory, PARTICLE, PARTICLE_MAX_COUNT );
            
            while( Platform->IsRunning ) {
                int64  PerfCounter_Start  = Win32_GetPerfCount();
                uint64 CycleCounter_Start = __rdtsc();
                
                GlobalVar_DebugSystem.AtPos = GetBL( AppState->App_Bound ) + Vec2( 10.0f, 14.0f );
                
                Win32_processPendingMessages( Platform, window );
                if( global_restoreFocus ) {
                    global_restoreFocus = false;
                    memset( &Platform->Mouse,    0, sizeof( MOUSE_STATE    ) );
                    memset( &Platform->Keyboard, 0, sizeof( KEYBOARD_STATE ) );
                    Mouse->BasePos = App_HalfDim;
                }
                
                ResetCommandList( Renderer );
                if( WasPressed( Keyboard, KeyCode_F4, ( KEYBOARD_FLAGS__ALT ) ) ) {
                    PostQuitMessage( 0 );
                }
                
                POINT Mouse_Position = {};
                GetCursorPos( &Mouse_Position );
                flo32 MouseX = ( flo32 )Mouse_Position.x;
                flo32 MouseY = ( flo32 )( DisplaySettings.Height - 1 - Mouse_Position.y );
                
                Mouse->PrevPosition = Mouse->Position;
                Mouse->Position     = Vec2( MouseX, MouseY );
                Mouse->dPos         = Mouse->Position - Mouse->PrevPosition;
                
                { // update Controller input
                    XINPUT_STATE Controller_State  = {};
                    DWORD        Controller_Status = XInputGetState( 0, &Controller_State );
                    boo32        Controller_ButtonDown[ 14 ] = {};
                    for( uint32 iControllerButton = 0; iControllerButton < ControllerButton_Count; iControllerButton++ ) {
                        Controller->button[ iControllerButton ].HalfTransitionCount = 0;
                    }
                    
                    Controller->Left.xy  = Int32Pair( 0, 0 );
                    Controller->Right.xy = Int32Pair( 0, 0 );
                    
                    if( Controller_Status == ERROR_SUCCESS ) {
                        UpdateButton( Controller, ControllerButton_dPad_Up,        ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP        ) );
                        UpdateButton( Controller, ControllerButton_dPad_Down,      ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN      ) );
                        UpdateButton( Controller, ControllerButton_dPad_Left,      ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT      ) );
                        UpdateButton( Controller, ControllerButton_dPad_Right,     ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT     ) );
                        UpdateButton( Controller, ControllerButton_Start,          ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_START          ) );
                        UpdateButton( Controller, ControllerButton_Back,           ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK           ) );
                        UpdateButton( Controller, ControllerButton_Thumb_Left,     ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB     ) );
                        UpdateButton( Controller, ControllerButton_Thumb_Right,    ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB    ) );
                        UpdateButton( Controller, ControllerButton_Shoulder_Left,  ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER  ) );
                        UpdateButton( Controller, ControllerButton_Shoulder_Right, ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ) );
                        UpdateButton( Controller, ControllerButton_A,              ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_A              ) );
                        UpdateButton( Controller, ControllerButton_B,              ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_B              ) );
                        UpdateButton( Controller, ControllerButton_X,              ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_X              ) );
                        UpdateButton( Controller, ControllerButton_Y,              ( Controller_State.Gamepad.wButtons & XINPUT_GAMEPAD_Y              ) );
                        uint8 LeftTrigger  = ( uint8 )Controller_State.Gamepad.bLeftTrigger;
                        uint8 RightTrigger = ( uint8 )Controller_State.Gamepad.bRightTrigger;
                        UpdateButton( Controller, ControllerButton_Trigger_Left,  LeftTrigger  > 0 );
                        UpdateButton( Controller, ControllerButton_Trigger_Right, RightTrigger > 0 );
                        
                        Controller->Left.xy.x  = ( int32 )Controller_State.Gamepad.sThumbLX;
                        Controller->Left.xy.y  = ( int32 )Controller_State.Gamepad.sThumbLY;
                        Controller->Right.xy.x = ( int32 )Controller_State.Gamepad.sThumbRX;
                        Controller->Right.xy.y = ( int32 )Controller_State.Gamepad.sThumbRY;
                    }
                }
                
                APP_UpdateAndRender( AppState, Platform );
                UpdateAudio( &Win32_Audio, AudioSystem );
                
                if( Mouse->ResetPos ) {
                    int32 X = ( int32 )Mouse->BasePos.x;
                    int32 Y = DisplaySettings.Height - 1 - ( int32 )Mouse->BasePos.y;
                    SetCursorPos( X, Y );
                    
                    Mouse->Position = Mouse->BasePos;
                }
                
                if( GlobalVar_DebugSystem.ErrorOccurred ) {
                    DrawString_noErrorCheck( GlobalVar_DebugSystem.Pass_UI, GlobalVar_DebugSystem.Font, "ERROR OCCURRED!! Check console for details!", GlobalVar_DebugSystem.AtPos, Vec2( 1.0f, 1.0f ), COLOR_RED );
                    GlobalVar_DebugSystem.AtPos.y -= GlobalVar_DebugSystem.advanceHeight;
                }
                
                uint64 CycleCounter_App = __rdtsc();
                
                ID3D12GraphicsCommandList * CommandList = Renderer->CommandList;
                
                ID3D12Resource * BackBuffer = Renderer->BackBuffers[ Renderer->currentBackBufferIndex ];
                D3D12_CPU_DESCRIPTOR_HANDLE RTV = CD3DX12_CPU_DESCRIPTOR_HANDLE( Renderer->RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), Renderer->currentBackBufferIndex, Renderer->RTVDescriptorSize );
                D3D12_CPU_DESCRIPTOR_HANDLE DSV = Renderer->DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
                
                D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( BackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET );
                CommandList->ResourceBarrier( 1, &barrier );
                
                flo32 Color[ 4 ] = { 0.02f, 0.02f, 0.02f, 1.0f };
                CommandList->ClearRenderTargetView( RTV, Color, 0, 0 );
                CommandList->ClearDepthStencilView( DSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, 0 );
                
                CommandList->SetPipelineState        ( Renderer->PSS_tri_depth_noBlend );
                CommandList->SetGraphicsRootSignature( Renderer->rootSignature );
                
                CommandList->SetDescriptorHeaps( 1, &Renderer->SRVHeap );
                
                flo32 Width  = ( flo32 )DisplaySettings.Width;
                flo32 Height = ( flo32 )DisplaySettings.Height;
                D3D12_VIEWPORT viewport = { 0.0f, 0.0f, Width, Height, 0.0f, 1.0f };
                D3D12_RECT     scissor  = { 0, 0, ( LONG )Width, ( LONG )Height };
                
                CommandList->RSSetViewports   ( 1, &viewport );
                CommandList->RSSetScissorRects( 1, &scissor  );
                
                CommandList->OMSetRenderTargets( 1, &RTV, 0, &DSV );
                
                mat4 ui_transform   = mat4_orthographic( App_Bound );
                
                CAMERA_STATE * Camera = &AppState->Camera;
                if( AppState->Mode == AppMode_Editor ) {
                    Camera = &AppState->Editor.Camera;
                }
                if( AppState->Mode == AppMode_Layout ) {
                    Camera = &AppState->Layout.Camera;
                }
                rect Camera_Bound   = RectCD( Camera->Pos, Camera->Dim );
                mat4 game_transform = mat4_orthographic( Camera_Bound );
                
                RenderPass( Renderer, Pass_Game, game_transform );
                RenderPass( Renderer, Pass_UI,   ui_transform   );
                
                Renderer->textBuffer.nVertex = 0;
                
                int64  PerfCounter_End  = Win32_GetPerfCount();
                uint64 CycleCounter_End = __rdtsc();
                
                char   PerfString[ 128 ] = {};
                uint64 CycleElapsed_App  = CycleCounter_App - CycleCounter_Start;
                { // PROFILE MAIN LOOP
                    flo32  MSElapsed = Win32_GetMSElapsed( PerfCounter_Start, PerfCounter_End, PerfFrequency.QuadPart );
                    uint64 CycleElapsed = CycleCounter_End - CycleCounter_Start;
                    
                    sprintf( PerfString, "FRAME PERF: %6.02fms, %10lluMC", MSElapsed, CycleElapsed );
                    DISPLAY_STRING( PerfString );
                    
                    flo32 Percent = ( flo32 )( CycleElapsed_App ) / ( flo32 )( CycleElapsed ) * 100.0f;
                    sprintf( PerfString, "FRAME PERF: APP: %10llu, %.02f%%, DRAW: %.02f%%", CycleElapsed_App, Percent, 100.0f - Percent );
                    DISPLAY_STRING( PerfString );
                }
                
                { // OTHER PROFILING
                    PROFILE_STATE * State = &AppState->ProFile;
                    for( uint32 iProFileBlock = 0; iProFileBlock < State->nProFileBlock; iProFileBlock++ ) {
                        PROFILE_BLOCK ProFile = State->ProFileBlock[ iProFileBlock ];
                        
                        uint64 CycleElapsed = ProFile.CycleCounter_End - ProFile.CycleCounter_Start;
                        flo32  Percent = ( flo32 )CycleElapsed / ( flo32 )CycleElapsed_App * 100.0f;
                        
                        sprintf( PerfString, "%s: %10llu, %.02f%%", ProFile.Label, CycleElapsed, Percent );
                        DISPLAY_STRING( PerfString );
                    }
                    State->nProFileBlock = 0;
                }
                
                Present( Renderer );
            }
        }
    }
    
    return 0;
}