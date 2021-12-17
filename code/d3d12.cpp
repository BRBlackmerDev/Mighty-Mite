
//----------
// TODO: DISPLAY_SETTINGS : clean up
// TODO: VERTEX1 : how should I organize this? This data is dependent on the shader.
//----------
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>

struct DIRECTX_12_DISPLAY_SETTINGS {
    IDXGIFactory2 * dxgiFactory;
    IDXGIAdapter  * adapter;
    
    DXGI_FORMAT       format;
    D3D_FEATURE_LEVEL featureLevel;
    
    uint32 Width;
    uint32 Height;
    uint32 refreshRate_numerator;
    uint32 refreshRate_Denominator;
};

struct VERTEX1 {
    vec3 Position;
    vec2 texCoord;
};

internal VERTEX1
Vertex1( vec3 pos, vec2 tex ) {
    VERTEX1 Result = {};
    Result.Position = pos;
    Result.texCoord = tex;
    return Result;
}

struct VERTEX1_BUFFER {
    D3D12_VERTEX_BUFFER_VIEW vertexView;
    D3D12_INDEX_BUFFER_VIEW   indexView;
    
    //#define VERTEX_COUNT  ( 131072 )
    //#define  INDEX_COUNT  ( 262144 )
    uint32 maxVertex;
    uint32 maxIndex;
    uint32 nVertex;
    uint32 nIndex;
    VERTEX1 * vertex_Start;
    uint32  *  index_Start;
    
};

internal VERTEX1_BUFFER
Vertex1Buffer( ID3D12Device2 * device, uint32 maxVertex, uint32 maxIndex = 0 ) {
    VERTEX1_BUFFER Result = {};
    Result.maxVertex = maxVertex;
    Result.maxIndex  = maxIndex;
    
    uint32 vertex_size = sizeof( VERTEX1 );
    uint32  index_size = sizeof( uint32  );
    uint32 bufferSize = maxVertex * vertex_size + maxIndex * index_size;
    
    ID3D12Resource * UploadBuffer = 0;
    
    D3D12_HEAP_PROPERTIES prop_dest = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
    D3D12_RESOURCE_DESC   desc_dest = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
    
    HRESULT Status = device->CreateCommittedResource( &prop_dest, D3D12_HEAP_FLAG_NONE, &desc_dest, D3D12_RESOURCE_STATE_GENERIC_READ, 0, IID_PPV_ARGS( &UploadBuffer ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the dest resource to Upload the data!\n" );
    }
    
    // Map the buffer
    void * data = 0;
    D3D12_RANGE range = {};
    UploadBuffer->Map( 0, &range, &data ); 
    
    uint8 * data_Start = ( uint8 * )data;
    uint8 * data_at    = data_Start;
    
    data_at += ( ( uint64 )data_at % sizeof( flo32 ) );
    Result.vertex_Start  = ( VERTEX1 * )data_at;
    uint32 vertex_offset = ( uint32 )( data_at - data_Start );
    data_at += ( maxVertex * vertex_size );
    
    Result.vertexView.BufferLocation = UploadBuffer->GetGPUVirtualAddress() + vertex_offset;
    Result.vertexView.StrideInBytes  = vertex_size;
    
    if( maxIndex > 0 ) {
        data_at += ( ( uint64 )data_at % sizeof( uint32 ) );
        Result.index_Start  = ( uint32 * )data_at;
        uint32 index_offset = ( uint32 )( data_at - data_Start );
        data_at += ( maxIndex * index_size );
        
        Result.indexView.BufferLocation = UploadBuffer->GetGPUVirtualAddress() + index_offset;
        Result.indexView.Format         = DXGI_FORMAT_R32_UINT;
    }
    
    return Result;
}

enum TEXTURE_ID {
    TextureID_DefaultTexture,
    TextureID_WhiteTexture,
    TextureID_Font,
    
    TextureID_Level_Bitmap,
    //NOTE: LAYOUT__ROOM_FILE_MAX_COUNT = 1024
    TextureID_Level_Bitmap_End = TextureID_Level_Bitmap + 1024,
    
    TextureID_Count,
};

enum MODEL_ID {
    ModelID_line,
    ModelID_rect,
    ModelID_rectOutline,
    ModelID_circle,
    ModelID_circleOutline,
    ModelID_box,
    ModelID_Triangle,
    ModelID_Hexagon,
    ModelID_Octagon,
    
    ModelID_Player_WallSlide,
    ModelID_Spikes,
    
    ModelID_Count,
};

struct RENDER_OBJECT {
    MODEL_ID   ModelID;
    TEXTURE_ID TextureID;
    mat4       transform;
    vec4       modColor;
};

struct RENDER_OBJECT_LIST {
    uint32 nObjects;
    uint32 maxObjects;
    RENDER_OBJECT * object;
};

struct TEXT_RENDER_OBJECT {
    uint32 bVertex;
    uint32 nVertex;
    vec4   modColor;
};

struct TEXT_RENDER_OBJECT_LIST {
    uint32 nObjects;
    uint32 maxObjects;
    TEXT_RENDER_OBJECT * object;
};

struct MODEL_DATA {
    uint32 nIndex;
    uint32 bIndex;
    uint32 bVertex;
};

struct TEXTURE_RESOURCE {
    ID3D12Resource * buffer;
    ID3D12Resource * buffer_intr;
};

struct DIRECTX_12_RENDER_PASS {
    VERTEX1_BUFFER * textBuffer;
    VERTEX1_BUFFER * ModelBuffer;
    
    TEXT_RENDER_OBJECT_LIST       textList;
    RENDER_OBJECT_LIST       triObjectList;
    RENDER_OBJECT_LIST      lineObjectList;
};

struct DIRECTX_12_RENDERER {
    HWND window_handle;
    
    boo32 IsInitialized;
    
    ID3D12Device2             * device;
    IDXGISwapChain4           * swapChain;
    ID3D12DescriptorHeap      * RTVDescriptorHeap;
    ID3D12DescriptorHeap      * DSVDescriptorHeap;
    ID3D12DescriptorHeap      * SRVHeap;
    uint32                      RTVDescriptorSize;
    uint32                      currentBackBufferIndex;
    
#define DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS  ( 3 )
    ID3D12Resource            * BackBuffers      [ DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS ];
    
    ID3D12CommandQueue        * commandQueue;
    ID3D12CommandAllocator    * commandAllocator;
    ID3D12GraphicsCommandList * CommandList;
    
    ID3D12Fence               * fence;
    HANDLE                      fenceEvent;
    uint64                      fenceValue;
    
    ID3D12PipelineState * PSS_tri_depth_noBlend;
    ID3D12PipelineState * PSS_tri_noDepth_blend;
    ID3D12PipelineState * PSS_line_noDepth_noBlend;
    ID3D12PipelineState * PSS_line_noDepth_blend;
    ID3D12RootSignature * rootSignature;
    
    boo32            Texture_isLoaded[ TextureID_Count ];
    TEXTURE_RESOURCE Texture_resource[ TextureID_Count ];
    
    DIRECTX_12_RENDER_PASS Pass_Game;
    DIRECTX_12_RENDER_PASS Pass_UI;
    
    MODEL_DATA     ModelData[ ModelID_Count ];
    
    VERTEX1_BUFFER textBuffer;
    VERTEX1_BUFFER ModelBuffer;
};

internal void
D3D12_EnableDebug() {
    //---------- Enable Debug Layer
#if DEBUG_BUILD
    ID3D12Debug * DebugInterface = 0;
    HRESULT Status = D3D12GetDebugInterface( IID_PPV_ARGS( &DebugInterface ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to enable Debug interface!\n" );
    }
    DebugInterface->EnableDebugLayer();
#endif // DEBUG_BUILD
    //---------- END Enable Debug Layer
}

internal DIRECTX_12_DISPLAY_SETTINGS
getDisplay( MEMORY * TempMemory ) {
    //---------- Query DXGI Adapter
    IDXGIFactory2 * dxgiFactory = 0;
    
    uint32 factoryFlags = 0;
#if DEBUG_BUILD
    factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    
    // NOTE: IDXGIFactory contains all the functions I need, but IDXGIFactory2 is required to retrieve a Debug factory.
    HRESULT Status = CreateDXGIFactory2( factoryFlags, IID_PPV_ARGS( &dxgiFactory ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the DXGI factory!\n" );
    }
    
    // NOTE: If I use IDXGIFactory to create the swap chain, how do I associate the swap chain with the window handle? Does it need to be associated? The window handle is in the swap chain description, Passed during swap chain creation.
    
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    
    IDXGIAdapter * adapter_temp = 0;
    IDXGIAdapter * adapter      = 0;
    
    uint32 iAdapter  = 0;
    uint64 maxMemory = 0;
    boo32  enumerateAdapters = true;
    while( enumerateAdapters ) {
        Status = dxgiFactory->EnumAdapters( iAdapter, &adapter_temp );
        if( SUCCEEDED( Status ) ) {
            Status = D3D12CreateDevice( adapter_temp, featureLevel, __uuidof( ID3D12Device ), 0 );
            if( SUCCEEDED( Status ) ) {
                DXGI_ADAPTER_DESC adapterDesc = {};
                adapter_temp->GetDesc( &adapterDesc );
                
                if( adapterDesc.DedicatedVideoMemory > maxMemory ) {
                    adapter   = adapter_temp;
                    maxMemory = adapterDesc.DedicatedVideoMemory;
                }
            }
        } else {
            enumerateAdapters = false;
        }
        
        iAdapter++;
    }
    
    if( !adapter ) {
        OutputDebugString( "ERROR! Failed to enumerate any display adapters!\n" );
    }
    
    
    IDXGIOutput * adapterOutput = 0;
    Status = adapter->EnumOutputs( 0, &adapterOutput );
    if( FAILED(  Status ) ) {
        OutputDebugString( "ERROR! Failed to enumerate adapter outputs!\n" );
    }
    
    uint32 nDisplayModes = 0;
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    Status = adapterOutput->GetDisplayModeList( format, 0, &nDisplayModes, 0 );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to get the count of display Modes!\n" );
    }
    
#define DISPLAY_MODE_LIST__MAX_COUNT  ( 128 )
    Assert( nDisplayModes <= DISPLAY_MODE_LIST__MAX_COUNT );
    
    DXGI_MODE_DESC * displayModeList = _pushArray( TempMemory, DXGI_MODE_DESC, nDisplayModes );
    Status = adapterOutput->GetDisplayModeList( format, 0, &nDisplayModes, displayModeList );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to get the list of display Modes!\n" );
    }
    
    uint32 maxWidth  = 0;
    uint32 maxHeight = 0;
    flo32  maxFPS    = 0.0f;
    int32 displayMode_index = -1;
    // This will return the display Mode with the highest resolution and highest Framerate. A feature for later would be to list the possible display Modes to the user and have them select one.
    for( int32 iDisplayMode = nDisplayModes - 1; iDisplayMode > 0; iDisplayMode-- ) {
        DXGI_MODE_DESC * displayMode = displayModeList + iDisplayMode;
        
        DXGI_RATIONAL refresh = displayMode->RefreshRate;
        flo32 fps = ( flo32 )refresh.Numerator / ( flo32 )refresh.Denominator;
        
        boo32 case0 = ( displayMode->Width > maxWidth );
        boo32 case1 = ( displayMode->Width == maxWidth ) && ( displayMode->Height > maxHeight );
        boo32 case2 =  ( displayMode->Width == maxWidth ) && ( displayMode->Height == maxHeight ) && ( fps > maxFPS );
        // NOTE: Case2 will prefer 60.0fps over 59.94fps. There isn't any difference between the two, as far as I can tell.
        
        if( ( case0 ) || ( case1 ) || ( case2 ) ) {
            maxWidth  = displayMode->Width;
            maxHeight = displayMode->Height;
            maxFPS    = fps;
            displayMode_index = iDisplayMode;
        }
    }
    
    DIRECTX_12_DISPLAY_SETTINGS Result = {};
    if( displayMode_index > -1 ) {
        DXGI_MODE_DESC * displayMode = displayModeList + displayMode_index;
        
        Result.dxgiFactory = dxgiFactory;
        Result.adapter     = adapter;
        
        Result.format       = format;
        Result.featureLevel = featureLevel;
        
        Result.Width  = displayMode->Width;
        Result.Height = displayMode->Height;
        Result.refreshRate_numerator   = displayMode->RefreshRate.Numerator;
        Result.refreshRate_Denominator = displayMode->RefreshRate.Denominator;
    } else {
        OutputDebugString( "ERROR! Failed to find a valid display Mode!\n" );
    }
    //---------- END Query DXGI Adapter
    
    _popArray( TempMemory, DXGI_MODE_DESC, nDisplayModes );
    
    return Result;
}

internal DIRECTX_12_RENDERER
initDirectX12( DIRECTX_12_DISPLAY_SETTINGS DisplaySettings, HWND window_handle ) {
    HRESULT Status = 0;
    
    //---------- Create DirectX12 Device
    ID3D12Device2 * device = 0;
    Status = D3D12CreateDevice( DisplaySettings.adapter, DisplaySettings.featureLevel, IID_PPV_ARGS( &device ) );
    // NOTE: D3D12CreateDevice() is throwing the following exception: Exception thrown at 0x00007FFF65F9A799 in fish.exe: Microsoft C++ exception: _com_error at memory location 0x00000058964F7960.
    // 0x00007FFF65F9A799 is an instruction address outside my code.
    // 0x00000058964F7960 is a stack address (i think) from within D3D12CreateDevice()
    // These addresses are close to my current instruction and stack address, but coming after. This leads me to believe the error is coming from inside D3D12CreateDevice(), but I have not encountered this kind of thing before and I don't have access to the D3D12CreateDevice() source.
    // I believe this error only Started occurring after I updated my display drivers.
    // This exception is not prEventing the device from being create, so I will just ignore it until I have more experience.
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the D3D12 device!\n" );
    }
    //---------- END Create DirectX12 Device
    
    //---------- Enable Debug Messages
#if DEBUG_BUILD
    //#if 0
    ID3D12InfoQueue * infoQueue = 0;
    Status = device->QueryInterface( __uuidof( ID3D12InfoQueue ), ( void ** )&infoQueue );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to query the Info Queue interface from the D3D12 device!\n" );
    }
    
    infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE );
    infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR,      TRUE );
    infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_WARNING,    TRUE );
    
    // Suppress whole categories of messages
    //D3D12_MESSAGE_CATEGORY Categories[] = {};
    
    // Suppress messages based on their severity level
    D3D12_MESSAGE_SEVERITY Severities[] = {
        D3D12_MESSAGE_SEVERITY_INFO
    };
    
    // Suppress individual messages by their ID
    D3D12_MESSAGE_ID DenyIds[] = {
        D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture Frame while graphics Debugging.
        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture Frame while graphics Debugging.
    };
    
    D3D12_INFO_QUEUE_FILTER NewFilter = {};
    // NewFilter.DenyList.NumCategories = _ArrayCount( Categories );
    // NewFilter.DenyList.pCategoryList = Categories;
    NewFilter.DenyList.NumSeverities  = _ArrayCount( Severities );
    NewFilter.DenyList.pSeverityList  = Severities;
    NewFilter.DenyList.NumIDs         = _ArrayCount( DenyIds );
    NewFilter.DenyList.pIDList        = DenyIds;
    
    Status = infoQueue->PushStorageFilter( &NewFilter );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to push the error filters onto the info queue!\n" );
    }
#endif // DEBUG_BUILD
    //---------- END Enable Debug Messages
    
    //---------- Create Command Queue
    ID3D12CommandQueue * commandQueue = 0;
    
    D3D12_COMMAND_QUEUE_DESC commandQueue_desc = {};
    commandQueue_desc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    commandQueue_desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueue_desc.NodeMask = 0;
    
    Status = device->CreateCommandQueue( &commandQueue_desc, IID_PPV_ARGS( &commandQueue ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the command queue!\n" );
    }
    //---------- END Create Command Queue
    
    //---------- Create Swap Chain
#define DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS  ( 3 )
    uint32 nBuffers = DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS;
    
    DXGI_SWAP_CHAIN_DESC swapChain_desc = {};
    swapChain_desc.BufferDesc.Width  = DisplaySettings.Width;
    swapChain_desc.BufferDesc.Height = DisplaySettings.Height;
    swapChain_desc.BufferDesc.Format = DisplaySettings.format;
    swapChain_desc.BufferDesc.RefreshRate.Numerator   = DisplaySettings.refreshRate_numerator;
    swapChain_desc.BufferDesc.RefreshRate.Denominator = DisplaySettings.refreshRate_Denominator;
    swapChain_desc.SampleDesc   = { 1, 0 };
    swapChain_desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChain_desc.BufferCount  = nBuffers; 
    swapChain_desc.OutputWindow = window_handle;
    swapChain_desc.Windowed     = true;
    swapChain_desc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChain_desc.Flags        = 0;
    // NOTE: DXGI_SWAP_CHAIN_DESC1 has options to specify the scaling Mode (if the size of the backbuffer does not match the output target) and the alpha Mode. Both of these options would be good to investigate in the future.
    
    IDXGISwapChain  * swapChain_temp = 0;
    IDXGISwapChain4 * swapChain      = 0;
    Status = DisplaySettings.dxgiFactory->CreateSwapChain( commandQueue, &swapChain_desc, &swapChain_temp );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the swap chain from the window handle!\n" );
    }
    
    // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
    // will be handled manually.
    Status = DisplaySettings.dxgiFactory->MakeWindowAssociation( window_handle, DXGI_MWA_NO_ALT_ENTER );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to disable DXGI handling of Alt+Enter fullscreen toggle!" );
    }
    
    Status = swapChain_temp->QueryInterface( __uuidof( IDXGISwapChain4 ), ( void ** )&swapChain );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to query the DXGISwapChain4 interface from IDXGISwapChain1!\n" );
    }
    //---------- END Create Swap Chain
    
    //---------- Create Descriptor Heap
    ID3D12DescriptorHeap *  descriptorHeap = 0;
    
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeap_desc = {};
    descriptorHeap_desc.NumDescriptors = nBuffers;
    descriptorHeap_desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    
    Status = device->CreateDescriptorHeap( &descriptorHeap_desc, IID_PPV_ARGS( &descriptorHeap ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the descriptor heap!\n" );
    }
    //---------- END Create Descriptor Heap
    
    //---------- Create Render Target Views
    ID3D12Resource * BackBuffers[ DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS ] = {};
    
    uint32 rtvDescriptorSize = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( descriptorHeap->GetCPUDescriptorHandleForHeapStart() );
    
    for( uint32 iRenderTarget = 0; iRenderTarget < nBuffers; iRenderTarget++ ) {
        ID3D12Resource * BackBuffer = 0;
        Status = swapChain->GetBuffer( iRenderTarget, IID_PPV_ARGS( &BackBuffer ) );
        if( FAILED( Status ) ) {
            OutputDebugString( "ERROR! Failed to get the Ith buffer from the swap chain!\n" );
        }
        
        device->CreateRenderTargetView( BackBuffer, 0, rtvHandle );
        BackBuffers[ iRenderTarget ] = BackBuffer;
        
        rtvHandle.Offset( rtvDescriptorSize );
    }
    //---------- END Create Render Target Views
    
    //---------- Create Command Allocator
    ID3D12CommandAllocator * commandAllocator = 0;
    Status = device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &commandAllocator ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the command allocator for the command list!\n" );
    }
    //---------- END Create Command Allocator
    
    //---------- Create Command List
    ID3D12GraphicsCommandList * CommandList = 0;
    Status = device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, 0, IID_PPV_ARGS( &CommandList ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the command list!\n" );
    }
    
    Status = CommandList->Close();
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to close the command list for use!\n" );
    }
    //---------- END Create Command List
    
    //---------- Create Fence
    ID3D12Fence * fence = 0;
    Status = device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &fence ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the fence object!\n" );
    }
    //---------- END Create Fence
    
    //---------- Create Fence Event
    HANDLE fenceEvent = CreateEvent( 0, 0, 0, 0 );
    if( !fenceEvent ) {
        OutputDebugString( "ERROR! Failed to create the wait Event for the fence!\n" );
    }
    //---------- END Create Fence
    
    DIRECTX_12_RENDERER Result = {};
    Result.window_handle = window_handle;
    
    Result.IsInitialized = true;
    
    Result.device                 = device;
    Result.swapChain              = swapChain;
    Result.RTVDescriptorHeap      = descriptorHeap;
    Result.RTVDescriptorSize      = rtvDescriptorSize;
    Result.currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    
    for( uint32 iRenderTarget = 0; iRenderTarget < nBuffers; iRenderTarget++ ) {
        Result.BackBuffers      [ iRenderTarget ] = BackBuffers      [ iRenderTarget ];
    }
    
    Result.commandQueue     = commandQueue;
    Result.commandAllocator = commandAllocator;
    Result.CommandList      = CommandList;
    
    Result.fence      = fence;
    Result.fenceEvent = fenceEvent;
    
    return Result;
}

internal void
clear( DIRECTX_12_RENDERER * Renderer, flo32 * Color ) {
    uint32 index = Renderer->currentBackBufferIndex;
    ID3D12Resource         * BackBuffer       = Renderer->BackBuffers[ index ];
    ID3D12CommandAllocator * commandAllocator = Renderer->commandAllocator;
    
    commandAllocator->Reset();
    Renderer->CommandList->Reset( commandAllocator, 0 );
    
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( BackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET );
    
    Renderer->CommandList->ResourceBarrier( 1, &barrier );
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv( Renderer->RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                                      Renderer->currentBackBufferIndex, Renderer->RTVDescriptorSize );
    
    Renderer->CommandList->ClearRenderTargetView( rtv, Color, 0, 0 );
}

internal void
Signal( ID3D12CommandQueue * commandQueue, ID3D12Fence * fence, uint64 * fenceValue ) {
    uint64 fenceValueForSignal = ++( *fenceValue );
    
    HRESULT Status = commandQueue->Signal( fence, fenceValueForSignal );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to issue the signal command for this fence!\n" );
    }
}

internal void
WaitForFenceValue( ID3D12Fence * fence, uint64 fenceValue, HANDLE fenceEvent,
                  uint32 milliseconds = 0xFFFFFFFF ) {
    if( fence->GetCompletedValue() < fenceValue ) {
        HRESULT Status = fence->SetEventOnCompletion( fenceValue, fenceEvent );
        if( FAILED( Status ) ) {
            OutputDebugString( "ERROR! Failed to associate the fence Event with the fence for waiting!\n" );
        }
        WaitForSingleObject( fenceEvent, ( DWORD )milliseconds );
    }
}

internal void
Present( DIRECTX_12_RENDERER * Renderer ) {
    uint32 index = Renderer->currentBackBufferIndex;
    ID3D12Resource * BackBuffer = Renderer->BackBuffers[ index ];
    
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( BackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    Renderer->CommandList->ResourceBarrier( 1, &barrier );
    
    HRESULT Status = Renderer->CommandList->Close();
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to close the command list for rendering!\n" );
    }
    
    ID3D12CommandList * const CommandLists[] = { Renderer->CommandList };
    Renderer->commandQueue->ExecuteCommandLists( 1, CommandLists );
    Signal( Renderer->commandQueue, Renderer->fence, &Renderer->fenceValue );
    
    Status = Renderer->swapChain->Present( 1, 0 );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Renderer failed when Presenting the render view!\n" );
    }
    
    Renderer->currentBackBufferIndex = Renderer->swapChain->GetCurrentBackBufferIndex();
    
    WaitForFenceValue( Renderer->fence, Renderer->fenceValue, Renderer->fenceEvent );
}

internal void
updateBufferResource( DIRECTX_12_RENDERER * Renderer, ID3D12Resource ** dest, ID3D12Resource ** intr, uint32 nElem, uint32 elemSize, void * data ) {
    if( data ) {
        ID3D12Device2 * device = Renderer->device;
        
        uint32 bufferSize = nElem * elemSize;
        
        D3D12_HEAP_PROPERTIES prop_dest = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );
        D3D12_HEAP_PROPERTIES prop_intr = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD  );
        D3D12_RESOURCE_DESC   desc_dest = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
        D3D12_RESOURCE_DESC   desc_intr = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
        
        HRESULT Status = device->CreateCommittedResource( &prop_dest, D3D12_HEAP_FLAG_NONE, &desc_dest, D3D12_RESOURCE_STATE_COPY_DEST, 0, IID_PPV_ARGS( dest ) );
        if( FAILED( Status ) ) {
            OutputDebugString( "ERROR! Failed to create the dest resource to Upload the data!\n" );
        }
        
        Status = device->CreateCommittedResource( &prop_intr, D3D12_HEAP_FLAG_NONE, &desc_intr, D3D12_RESOURCE_STATE_GENERIC_READ, 0, IID_PPV_ARGS( intr ) );
        if( FAILED( Status ) ) {
            OutputDebugString( "ERROR! Failed to create the Intermediate Resource to Upload the Buffer Data!\n" );
        }
        
        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData      = data;
        subresourceData.RowPitch   = bufferSize;
        subresourceData.SlicePitch = subresourceData.RowPitch;
        
        UpdateSubresources( Renderer->CommandList, *dest, *intr, 0, 0, 1, &subresourceData );
    }
}

internal D3D12_GPU_DESCRIPTOR_HANDLE
getGPUHandle( ID3D12Device2 * device, ID3D12DescriptorHeap * heap, uint32 slotIndex ) {
    uint32 handleSize = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    CD3DX12_GPU_DESCRIPTOR_HANDLE Result( heap->GetGPUDescriptorHandleForHeapStart() );
    for( uint32 iSlot = 0; iSlot < slotIndex; iSlot++ ) {
        Result.Offset( handleSize );
    }
    return Result;
}

internal D3D12_CPU_DESCRIPTOR_HANDLE
getCPUHandle( ID3D12Device2 * device, ID3D12DescriptorHeap * heap, uint32 slotIndex ) {
    uint32 handleSize = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    CD3DX12_CPU_DESCRIPTOR_HANDLE Result( heap->GetCPUDescriptorHandleForHeapStart() );
    for( uint32 iSlot = 0; iSlot < slotIndex; iSlot++ ) {
        Result.Offset( handleSize );
    }
    return Result;
}

internal void
UploadTexture( DIRECTX_12_RENDERER * Renderer, TEXTURE_ID TextureID, uint32 Width, uint32 Height, void * data ) {
    TEXTURE_RESOURCE * resource = Renderer->Texture_resource + TextureID;
    if( data ) {
        ID3D12Device2 * device = Renderer->device;
        
        if( resource->buffer ) {
            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( resource->buffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST );
            Renderer->CommandList->ResourceBarrier( 1, &barrier );
        } else {
            ID3D12Resource * TextureBuffer_intr = 0;
            ID3D12Resource * TextureBuffer      = 0;
            
            D3D12_RESOURCE_DESC TextureDesc = {};
            TextureDesc.MipLevels = 1;
            TextureDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
            TextureDesc.Width     = Width;
            TextureDesc.Height    = Height;
            TextureDesc.Flags     = D3D12_RESOURCE_FLAG_NONE;
            TextureDesc.DepthOrArraySize   = 1;
            TextureDesc.SampleDesc.Count   = 1;
            TextureDesc.SampleDesc.Quality = 0;
            TextureDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            
            D3D12_HEAP_PROPERTIES prop_dest = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );
            HRESULT Status = device->CreateCommittedResource( &prop_dest, D3D12_HEAP_FLAG_NONE, &TextureDesc, D3D12_RESOURCE_STATE_COPY_DEST, 0, IID_PPV_ARGS( &TextureBuffer ) );
            if( FAILED( Status ) ) {
                OutputDebugString( "ERROR! Failed to create the dest resource to Upload the Texture data!\n" );
            }
            
            uint64 bufferSize = GetRequiredIntermediateSize( TextureBuffer, 0, 1 );
            
            D3D12_HEAP_PROPERTIES prop_intr = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD  );
            D3D12_RESOURCE_DESC   desc_intr = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
            Status = device->CreateCommittedResource( &prop_intr, D3D12_HEAP_FLAG_NONE, &desc_intr, D3D12_RESOURCE_STATE_GENERIC_READ, 0, IID_PPV_ARGS( &TextureBuffer_intr ) );
            if( FAILED( Status ) ) {
                OutputDebugString( "ERROR! Failed to create the intermediate resource to Upload the Texture data!\n" );
            }
            
            resource->buffer      = TextureBuffer;
            resource->buffer_intr = TextureBuffer_intr;
        }
        
        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData      = data;
        subresourceData.RowPitch   = Width  * sizeof( uint32 );
        subresourceData.SlicePitch = Height * Width * sizeof( uint32 );
        
        UpdateSubresources( Renderer->CommandList, resource->buffer, resource->buffer_intr, 0, 0, 1, &subresourceData );
        
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( resource->buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE );
        Renderer->CommandList->ResourceBarrier( 1, &barrier );
        
        D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        SRVDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
        SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        SRVDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels     = 1;
        
        D3D12_CPU_DESCRIPTOR_HANDLE handle = getCPUHandle( Renderer->device, Renderer->SRVHeap, ( uint32 )TextureID );
        Renderer->device->CreateShaderResourceView( resource->buffer, &SRVDesc, handle );
        Renderer->Texture_isLoaded[ TextureID ] = true;
    }
}

internal CD3DX12_BLEND_DESC
Blend( boo32 enableBlend ) {
    CD3DX12_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable  = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[ 0 ].BlendEnable   = enableBlend;
    blendDesc.RenderTarget[ 0 ].LogicOpEnable = false;
    // NOTE: This blend function is for pre-multiplied alpha.
    blendDesc.RenderTarget[ 0 ].SrcBlend       = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[ 0 ].DestBlend      = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[ 0 ].BlendOp        = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[ 0 ].SrcBlendAlpha  = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[ 0 ].BlendOpAlpha   = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[ 0 ].LogicOp               = D3D12_LOGIC_OP_NOOP;
    blendDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    
    return blendDesc;
}

internal CD3DX12_DEPTH_STENCIL_DESC
DepthStencil( boo32 enableDepth ) {
    CD3DX12_DEPTH_STENCIL_DESC DSDesc = {};
    DSDesc.DepthEnable                  = enableDepth;
    DSDesc.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ALL;
    DSDesc.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
    DSDesc.StencilEnable                = false;
    DSDesc.StencilReadMask              = D3D12_DEFAULT_STENCIL_READ_MASK;
    DSDesc.StencilWriteMask             = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    DSDesc.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
    DSDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    DSDesc.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
    DSDesc.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_ALWAYS;
    DSDesc.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
    DSDesc.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
    DSDesc.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
    DSDesc.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_ALWAYS;
    
    return DSDesc;
}

struct PIPELINE_STATE_STREAM {
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE        RootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT          InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY    PrimitiveTopologyType;
    CD3DX12_PIPELINE_STATE_STREAM_VS                    VertexShader;
    CD3DX12_PIPELINE_STATE_STREAM_PS                    PixelShader;
    CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC            BlendDesc;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT  DepthStencilFormat;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RenderTargetFormats;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER            Rasterizer;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL         DepthStencil;
};

internal ID3D12PipelineState *
createPSS( ID3D12Device2 * device, PIPELINE_STATE_STREAM * PSS ) {
    D3D12_PIPELINE_STATE_STREAM_DESC PSSDesc = {
        sizeof( PIPELINE_STATE_STREAM ), PSS
    };
    
    ID3D12PipelineState * pipelineState = 0;
    HRESULT Status = device->CreatePipelineState( &PSSDesc, IID_PPV_ARGS( &pipelineState ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the pipeline State!\n" );
    }
    return pipelineState;
}

internal void
CreateShader( DIRECTX_12_RENDERER * Renderer, DIRECTX_12_DISPLAY_SETTINGS DisplaySettings ) {
    Renderer->commandAllocator->Reset();
    Renderer->CommandList->Reset( Renderer->commandAllocator, 0 );
    
    ID3D12Device2 * device = Renderer->device;
    
    //---------- Create Vertex and Pixel Shaders
    const char * shader_vertexCode = "\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cbuffer MatrixBuffer : register( b0 ) {\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                matrix Camera_transform;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                matrix Model_transform;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             };\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             struct VertexInputType {\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                float4 Position : POSITION;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                float2 texCoord : TEXCOORD0;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             };\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             struct PixelInputType {\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                float4 Position : SV_POSITION;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                float2 texCoord : TEXCOORD0;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             };\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             PixelInputType main( VertexInputType input ) {\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                PixelInputType output;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                input.Position.w = 1.0f;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                output.Position = mul( mul( Camera_transform, Model_transform ), input.Position );\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                output.texCoord = input.texCoord;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             return output;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             }";
    
    const char * shader_pixelCode = "\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        cbuffer ColorBuffer : register( b1 ) {\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           float4 modColor;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        };\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     struct PixelInputType {\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        float4 Position : SV_POSITION;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        float2 texCoord : TEXCOORD0;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     };\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        SamplerState texSampler : register( s0 );\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        Texture2D    tex        : register( t0 );\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     float4 main( PixelInputType input ) : SV_TARGET {\
                                                                                                                                                                                                                                                                                float4 texel = modColor * tex.Sample( texSampler, input.texCoord );\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        return texel;\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     }";
    //float4 texel = float4( input.texCoord.x, input.texCoord.y, 0.0f, 1.0f );\

    ID3DBlob * errorMessage = 0;
    ID3DBlob * vertexShaderBlob = 0;
    HRESULT Status = D3DCompile( shader_vertexCode, strlen( shader_vertexCode ), 0, 0, 0, "main", "vs_5_1", D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBlob, &errorMessage );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! RENDERER failed to compile the vertex shader!\n" );
        char * error = ( char * )( errorMessage->GetBufferPointer() );
        InvalidCodePath;
    }
    
    ID3DBlob * pixelShaderBlob  = 0;
    Status = D3DCompile( shader_pixelCode, strlen( shader_pixelCode ), 0, 0, 0, "main", "ps_5_1", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBlob, &errorMessage );
    if( Status != S_OK ) {
        OutputDebugString( "ERROR! RENDERER failed to compile the pixel shader!\n" );
        char * error = ( char * )( errorMessage->GetBufferPointer() );
        InvalidCodePath;
    }
    //---------- END Create Vertex and Pixel Shaders
    
    //---------- Create Descriptor Heap for Textures
    ID3D12DescriptorHeap * SRVDescriptorHeap = 0;
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = TextureID_Count;
    srvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    Status = device->CreateDescriptorHeap( &srvHeapDesc, IID_PPV_ARGS( &SRVDescriptorHeap ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the descriptor heap for the Textures!\n" );
    }
    //---------- END Create Descriptor Heap for Textures
    
    //---------- Create Descriptor Heap for Depth-Stencil View
    ID3D12DescriptorHeap * DSVDescriptorHeap = 0;
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    Status = device->CreateDescriptorHeap( &dsvHeapDesc, IID_PPV_ARGS( &DSVDescriptorHeap ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the descriptor heap for the depth-stencil buffer!\n" );
    }
    //---------- END Create Descriptor Heap for Depth-Stencil View
    
    //---------- Create Depth/Stencil View
    uint32 Width  = DisplaySettings.Width;
    uint32 Height = DisplaySettings.Height;
    ID3D12Resource * depthBuffer = 0;
    
    D3D12_CLEAR_VALUE optimizedClearValue = {};
    optimizedClearValue.Format       = DXGI_FORMAT_D32_FLOAT;
    optimizedClearValue.DepthStencil = { 1.0f, 0 };
    
    D3D12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );
    D3D12_RESOURCE_DESC   desc = CD3DX12_RESOURCE_DESC::Tex2D( DXGI_FORMAT_D32_FLOAT, Width, Height,
                                                              1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL ); 
    Status = device->CreateCommittedResource( &prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optimizedClearValue,
                                             IID_PPV_ARGS( &depthBuffer ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the commited resource for the depth/stencil view!\n" );
    }
    
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
    dsv.Format        = DXGI_FORMAT_D32_FLOAT;
    dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    
    device->CreateDepthStencilView( depthBuffer, &dsv, DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart() );
    //---------- END Create Depth/Stencil View
    
    //---------- Create Vertex Input Layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    uint32 inputLayout_Count = _ArrayCount( inputLayout );
    //---------- END Create Vertex Input Layout
    
    //---------- Create Sampler Description
    D3D12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC( 0, D3D12_FILTER_MIN_MAG_MIP_POINT );
    samplerDesc.BorderColor      = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    //---------- END Create Sampler Description
    
    //---------- Create Root Signature
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    Status = device->CheckFeatureSupport( D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof( featureData ) );
    if( FAILED( Status ) ) {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
    
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    
    D3D12_ROOT_PARAMETER1 rootParams[ 3 ] = {};
    
    rootParams[ 0 ].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[ 0 ].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParams[ 0 ].Constants.ShaderRegister = 0;
    rootParams[ 0 ].Constants.RegisterSpace  = 0;
    rootParams[ 0 ].Constants.Num32BitValues = 32;
    
    rootParams[ 1 ].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[ 1 ].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[ 1 ].Constants.ShaderRegister = 1;
    rootParams[ 1 ].Constants.RegisterSpace  = 0;
    rootParams[ 1 ].Constants.Num32BitValues = 4;
    
    D3D12_DESCRIPTOR_RANGE1 descRange = {};
    descRange.RangeType          = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descRange.NumDescriptors     = 1;
    descRange.BaseShaderRegister = 0;
    descRange.RegisterSpace      = 0;
    descRange.OffsetInDescriptorsFromTableStart = 0;
    
    rootParams[ 2 ].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[ 2 ].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[ 2 ].DescriptorTable.NumDescriptorRanges = 1;
    rootParams[ 2 ].DescriptorTable.pDescriptorRanges   = &descRange;
    
    uint32 rootParams_Count = _ArrayCount( rootParams );
    
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootSigDesc.Desc_1_1.NumParameters     = rootParams_Count;
    rootSigDesc.Desc_1_1.pParameters       = rootParams;
    rootSigDesc.Desc_1_1.NumStaticSamplers = 1;
    rootSigDesc.Desc_1_1.pStaticSamplers   = &samplerDesc;
    rootSigDesc.Desc_1_1.Flags             = rootSignatureFlags;
    
    ID3DBlob * rootSignatureBlob = 0;
    ID3DBlob * errorBlob         = 0;
    Status = D3DX12SerializeVersionedRootSignature( &rootSigDesc, featureData.HighestVersion, &rootSignatureBlob, &errorBlob );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to serialize the root signature!\n" );
    }
    
    ID3D12RootSignature * rootSignature = 0;
    Status = device->CreateRootSignature( 0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS( &rootSignature ) );
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to create the root signature!\n" );
    }
    //---------- END Create Root Signature
    
    //---------- Create Pipeline State
    PIPELINE_STATE_STREAM PSS = {};
    
    D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
    renderTargetFormats.NumRenderTargets = 1;
    renderTargetFormats.RTFormats[ 0 ]   = DXGI_FORMAT_R8G8B8A8_UNORM;
    
    CD3DX12_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FrontCounterClockwise = true;
    rasterizerDesc.DepthBias             = 0;
    rasterizerDesc.DepthBiasClamp        = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    rasterizerDesc.DepthClipEnable       = true;
    // NOTE: REMEMBER! This is not a depth-buffer clip! This is related to clip-space coords!
    rasterizerDesc.MultisampleEnable     = false;
    rasterizerDesc.AntialiasedLineEnable = false;
    rasterizerDesc.ForcedSampleCount     = 0;
    rasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    
    PSS.RootSignature         = rootSignature;
    PSS.InputLayout           = { inputLayout, inputLayout_Count };
    PSS.VertexShader          = CD3DX12_SHADER_BYTECODE( vertexShaderBlob );
    PSS.PixelShader           = CD3DX12_SHADER_BYTECODE( pixelShaderBlob  );
    PSS.DepthStencilFormat    = DXGI_FORMAT_D32_FLOAT;
    PSS.RenderTargetFormats   = renderTargetFormats;
    PSS.Rasterizer            = rasterizerDesc;
    
    PSS.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSS.DepthStencil = DepthStencil( true );
    PSS.BlendDesc    = Blend( false );
    ID3D12PipelineState * PSS_tri_depth_noBlend = createPSS( device, &PSS );
    
    PSS.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSS.DepthStencil = DepthStencil( false );
    PSS.BlendDesc    = Blend( true );
    ID3D12PipelineState * PSS_tri_noDepth_blend = createPSS( device, &PSS );
    
    PSS.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    PSS.DepthStencil = DepthStencil( false );
    PSS.BlendDesc    = Blend( false );
    ID3D12PipelineState * PSS_line_noDepth_noBlend = createPSS( device, &PSS );
    
    PSS.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    PSS.DepthStencil = DepthStencil( false );
    PSS.BlendDesc    = Blend( true );
    ID3D12PipelineState * PSS_line_noDepth_blend = createPSS( device, &PSS );
    
    //---------- END Create Pipeline State
    
    Renderer->PSS_tri_depth_noBlend   = PSS_tri_depth_noBlend;
    Renderer->PSS_tri_noDepth_blend   = PSS_tri_noDepth_blend;
    Renderer->PSS_line_noDepth_noBlend = PSS_line_noDepth_noBlend;
    Renderer->PSS_line_noDepth_blend   = PSS_line_noDepth_blend;
    Renderer->rootSignature = rootSignature;
    Renderer->ModelBuffer        = Vertex1Buffer( device, 131072, 131072 );
    Renderer->textBuffer         = Vertex1Buffer( device, 131072 + DEBUG_SYSTEM__SAFETY_VERTEX_COUNT );
    Renderer->DSVDescriptorHeap  = DSVDescriptorHeap;
    Renderer->SRVHeap            = SRVDescriptorHeap;
    
    //---------- Execute the Command List
    Status = Renderer->CommandList->Close();
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to close the command list for rendering!\n" );
    }
    
    ID3D12CommandList * const CommandLists[] = { Renderer->CommandList };
    Renderer->commandQueue->ExecuteCommandLists( 1, CommandLists );
    Signal( Renderer->commandQueue, Renderer->fence, &Renderer->fenceValue );
    WaitForFenceValue( Renderer->fence, Renderer->fenceValue, Renderer->fenceEvent );
    //---------- END Execute the Command List
    
    /*
    // Load the vertex shader.
    ID3DBlob * vertexShaderBlob = 0;
    HRESULT Status = D3DReadFileToBlob( L"VertexShader.cso", &vertexShaderBlob );
    if( FAILED( Status ) ) {
       OutputDebugString( "ERROR! Failed to create/Load the vertex shader!\n" );
    }
    
    // Load the pixel shader.
    ID3DBlob * pixelShaderBlob = 0;
    Status = D3DReadFileToBlob( L"PixelShader.cso", &pixelShaderBlob );
    if( FAILED( Status ) ) {
       OutputDebugString( "ERROR! Failed to create/Load the pixel shader!\n" );
    }
    */
}

internal void
ResetCommandList( DIRECTX_12_RENDERER * Renderer ) {
    Renderer->commandAllocator->Reset();
    Renderer->CommandList->Reset( Renderer->commandAllocator, 0 );
}

internal void
executeCommandList( DIRECTX_12_RENDERER * Renderer ) {
    HRESULT Status = Renderer->CommandList->Close();
    if( FAILED( Status ) ) {
        OutputDebugString( "ERROR! Failed to close the command list for rendering!\n" );
    }
    
    ID3D12CommandList * const CommandLists[] = { Renderer->CommandList };
    Renderer->commandQueue->ExecuteCommandLists( 1, CommandLists );
    Signal( Renderer->commandQueue, Renderer->fence, &Renderer->fenceValue );
    WaitForFenceValue( Renderer->fence, Renderer->fenceValue, Renderer->fenceEvent );
}

internal MODEL_DATA
UploadModel( VERTEX1_BUFFER * buffer, VERTEX1 * vertex, uint32 nVertex, uint32 * index, uint32 nIndex ) {
    MODEL_DATA Result = {};
    Result.nIndex  = nIndex;
    Result.bIndex  = buffer->nIndex;
    Result.bVertex = buffer->nVertex;
    
    VERTEX1 * dVertex = buffer->vertex_Start + buffer->nVertex;
    memcpy( dVertex, vertex, sizeof( VERTEX1 ) * nVertex );
    buffer->nVertex += nVertex;
    Assert( buffer->nVertex < buffer->maxVertex );
    
    uint32 * dIndex = buffer->index_Start + buffer->nIndex;
    memcpy( dIndex, index, sizeof( uint32 ) * nIndex );
    buffer->nIndex += nIndex;
    Assert( buffer->nIndex < buffer->maxIndex );
    
    buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
    buffer->indexView.SizeInBytes  = sizeof( uint32  ) * buffer->nIndex;
    
    return Result;
}

internal MODEL_DATA
genLine( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    VERTEX1 vertex[ 2 ] = {
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    };
    uint32 nVertex = _ArrayCount( vertex );
    
    uint32 index[] = {
        0, 1,
    };
    uint32 nIndex = _ArrayCount( index );
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal MODEL_DATA
genRect( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    VERTEX1 vertex[ 4 ] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    };
    uint32 nVertex = _ArrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 2,
        3, 2, 1,
    };
    uint32 nIndex = _ArrayCount( index );
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal MODEL_DATA
genRectOutline( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    VERTEX1 vertex[ 4 ] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    };
    uint32 nVertex = _ArrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 3, 2, 0,
    };
    uint32 nIndex = _ArrayCount( index );
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal MODEL_DATA
genCircle( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    uint32 nVertex = 64;
    VERTEX1 vertex[ 64 + 1 ] = {};
    for( uint32 iVertex = 0; iVertex < nVertex; iVertex++ ) {
        flo32 radians = ( ( flo32 )iVertex / ( flo32 )nVertex ) * 2.0f * PI;
        flo32 x = cosf( radians );
        flo32 y = sinf( radians );
        
        VERTEX1 vert = {};
        vert.Position = Vec3( x, y, 0.0f );
        vert.texCoord = {};
        vertex[ iVertex ] = vert;
    }
    
    uint32 nIndex = nVertex * 3;
    uint32 index[ 64 * 3 ] = {};
    for( uint32 iIndex = 0; iIndex < nVertex; iIndex++ ) {
        uint32 bIndex = iIndex * 3;
        index[ bIndex + 0 ] = nVertex;
        index[ bIndex + 1 ] = iIndex; // [0..63]
        index[ bIndex + 2 ] = ( iIndex + 1 ) % nVertex; // [1..64(wraps to 0)]
    }
    
    nVertex += 1;
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal MODEL_DATA
genCircleOutline( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    uint32 nVertex = 64;
    VERTEX1 vertex[ 64 ] = {};
    for( uint32 iVertex = 0; iVertex < nVertex; iVertex++ ) {
        flo32 radians = ( ( flo32 )iVertex / ( flo32 )nVertex ) * 2.0f * PI;
        flo32 x = cosf( radians );
        flo32 y = sinf( radians );
        
        VERTEX1 vert = {};
        vert.Position = Vec3( x, y, 0.0f );
        vert.texCoord = {};
        vertex[ iVertex ] = vert;
    }
    
    uint32 nIndex = nVertex + 1;
    uint32 index[ 64 + 1 ] = {};
    for( uint32 iIndex = 0; iIndex < nIndex - 1; iIndex++ ) {
        index[ iIndex ] = iIndex;
    }
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal MODEL_DATA
GenHexagon( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    uint32 nVertex = 6;
    VERTEX1 vertex[ 6 + 1 ] = {};
    for( uint32 iVertex = 0; iVertex < nVertex; iVertex++ ) {
        flo32 radians = ( ( flo32 )iVertex / ( flo32 )nVertex ) * 2.0f * PI;
        flo32 x = cosf( radians );
        flo32 y = sinf( radians );
        
        VERTEX1 vert = {};
        vert.Position = Vec3( x, y, 0.0f );
        vert.texCoord = {};
        vertex[ iVertex ] = vert;
    }
    
    uint32 nIndex = nVertex * 3;
    uint32 index[ 6 * 3 ] = {};
    for( uint32 iIndex = 0; iIndex < nVertex; iIndex++ ) {
        uint32 bIndex = iIndex * 3;
        index[ bIndex + 0 ] = nVertex;
        index[ bIndex + 1 ] = iIndex;
        index[ bIndex + 2 ] = ( iIndex + 1 ) % nVertex;
    }
    
    nVertex += 1;
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal MODEL_DATA
GenOctagon( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    uint32 nVertex = 8;
    VERTEX1 vertex[ 8 + 1 ] = {};
    for( uint32 iVertex = 0; iVertex < nVertex; iVertex++ ) {
        flo32 radians = ( ( flo32 )iVertex / ( flo32 )nVertex ) * 2.0f * PI;
        flo32 x = cosf( radians );
        flo32 y = sinf( radians );
        
        VERTEX1 vert = {};
        vert.Position = Vec3( x, y, 0.0f );
        vert.texCoord = {};
        vertex[ iVertex ] = vert;
    }
    
    uint32 nIndex = nVertex * 3;
    uint32 index[ 8 * 3 ] = {};
    for( uint32 iIndex = 0; iIndex < nVertex; iIndex++ ) {
        uint32 bIndex = iIndex * 3;
        index[ bIndex + 0 ] = nVertex;
        index[ bIndex + 1 ] = iIndex;
        index[ bIndex + 2 ] = ( iIndex + 1 ) % nVertex;
    }
    
    nVertex += 1;
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal MODEL_DATA
genBox( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    uint32 nVertex = 24;
    VERTEX1 vertex[ 24 ] = {
        // front
        Vertex1( Vec3( -1.0f, -1.0f,  1.0f ), Vec2( 0.0f, 0.0f ) ),
        Vertex1( Vec3(  1.0f, -1.0f,  1.0f ), Vec2( 1.0f, 0.0f ) ),
        Vertex1( Vec3( -1.0f,  1.0f,  1.0f ), Vec2( 0.0f, 1.0f ) ),
        Vertex1( Vec3(  1.0f,  1.0f,  1.0f ), Vec2( 1.0f, 1.0f ) ),
        
        // back
        Vertex1( Vec3(  1.0f, -1.0f, -1.0f ), Vec2( 0.0f, 0.0f ) ),
        Vertex1( Vec3( -1.0f, -1.0f, -1.0f ), Vec2( 1.0f, 0.0f ) ),
        Vertex1( Vec3(  1.0f,  1.0f, -1.0f ), Vec2( 0.0f, 1.0f ) ),
        Vertex1( Vec3( -1.0f,  1.0f, -1.0f ), Vec2( 1.0f, 1.0f ) ),
        
        // Left
        Vertex1( Vec3( -1.0f, -1.0f, -1.0f ), Vec2( 0.0f, 0.0f ) ),
        Vertex1( Vec3( -1.0f, -1.0f,  1.0f ), Vec2( 1.0f, 0.0f ) ),
        Vertex1( Vec3( -1.0f,  1.0f, -1.0f ), Vec2( 0.0f, 1.0f ) ),
        Vertex1( Vec3( -1.0f,  1.0f,  1.0f ), Vec2( 1.0f, 1.0f ) ),
        
        // Right
        Vertex1( Vec3(  1.0f, -1.0f,  1.0f ), Vec2( 0.0f, 0.0f ) ),
        Vertex1( Vec3(  1.0f, -1.0f, -1.0f ), Vec2( 1.0f, 0.0f ) ),
        Vertex1( Vec3(  1.0f,  1.0f,  1.0f ), Vec2( 0.0f, 1.0f ) ),
        Vertex1( Vec3(  1.0f,  1.0f, -1.0f ), Vec2( 1.0f, 1.0f ) ),
        
        // Top
        Vertex1( Vec3( -1.0f,  1.0f,  1.0f ), Vec2( 0.0f, 0.0f ) ),
        Vertex1( Vec3(  1.0f,  1.0f,  1.0f ), Vec2( 1.0f, 0.0f ) ),
        Vertex1( Vec3( -1.0f,  1.0f, -1.0f ), Vec2( 0.0f, 1.0f ) ),
        Vertex1( Vec3(  1.0f,  1.0f, -1.0f ), Vec2( 1.0f, 1.0f ) ),
        
        // Bottom
        Vertex1( Vec3( -1.0f, -1.0f, -1.0f ), Vec2( 0.0f, 0.0f ) ),
        Vertex1( Vec3(  1.0f, -1.0f, -1.0f ), Vec2( 1.0f, 0.0f ) ),
        Vertex1( Vec3( -1.0f, -1.0f,  1.0f ), Vec2( 0.0f, 1.0f ) ),
        Vertex1( Vec3(  1.0f, -1.0f,  1.0f ), Vec2( 1.0f, 1.0f ) ),
    };
    
    uint32 nIndex = 36;
    uint32  index[ 36 ] = {
        0,  1,  2,   3,  2,  1,
        4,  5,  6,   7,  6,  5,
        8,  9,  10,  11, 10, 9,
        12, 13, 14,  15, 14, 13,
        16, 17, 18,  19, 18, 17,
        20, 21, 22,  23, 22, 21,
    };
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}

internal MODEL_DATA
GenTriangle( DIRECTX_12_RENDERER * Renderer ) {
    VERTEX1_BUFFER * buffer = &Renderer->ModelBuffer;
    
    VERTEX1 vertex[] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,   0.0f, 1.0f,
    };
    uint32 nVertex = _ArrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 2,
    };
    uint32 nIndex = _ArrayCount( index );
    
    MODEL_DATA Result = UploadModel( buffer, vertex, nVertex, index, nIndex );
    return Result;
}