
#ifdef	STD_INCLUDE_DECL

struct MEMORY {
	void * base;
	uint64 size;
	uint64 used;
    
#define MEMORY__SNAP_COUNT  ( 8 )
    uint32 snap_index;
    uint64 snapUsed[ MEMORY__SNAP_COUNT ];
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// memory functions
//----------

internal MEMORY
Memory( void * base, uint64 size ) {
	MEMORY Result = { base, size };
	return Result;
}

internal void
ResetMemory( MEMORY * memory ) {
	memset( memory->base, 0, memory->size );
	memory->used = 0;
}

#define _pushType( memory, Type ) ( Type * )pushSize_( memory, sizeof( Type ) )
#define _pushArray( memory, Type, count ) ( Type * )pushSize_( memory, sizeof( Type ) * ( count ) )
#define _pushSize( memory, size ) pushSize_( memory, size )

internal void *
pushSize_( MEMORY * memory, uint64 size ) {
	Assert( memory->used + size <= memory->size );
	void * Result = ( uint8 * )memory->base + memory->used;
	memory->used += size;
	return Result;
}

#define _pushType_clear( memory, Type ) ( Type * )pushSize_clear( memory, sizeof( Type ) )
#define _pushArray_clear( memory, Type, count ) ( Type * )pushSize_clear( memory, sizeof( Type ) * ( count ) )
#define _pushSize_clear( memory, size ) pushSize_clear( memory, size )

internal void *
pushSize_clear( MEMORY * memory, uint64 size ) {
	void * Result = pushSize_( memory, size );
	memset( Result, 0, size );
	return Result;
}

#define _popType(  memory, Type ) 			popSize_( memory, sizeof( Type ) )
#define _popArray( memory, Type, count ) 	popSize_( memory, sizeof( Type ) * ( count ) )
#define _popSize(  memory, size ) 			popSize_( memory, size )

internal void
popSize_( MEMORY * memory, uint64 size ) {
	Assert( memory->used - size >= 0 );
	memory->used -= size;
}

inline void
pushValue( MEMORY * memory, uint8 value ) {
	Assert( memory->used + sizeof( uint8 ) <= memory->size );
	uint8 * slot = ( uint8 * )memory->base + memory->used;
	*slot = value;
	memory->used += sizeof( uint8 );
}

internal void *
copyBlock( MEMORY * memory, void * data, uint32 size ) {
    void * Result = _pushSize( memory, size );
    memcpy( Result, data, size );
    return Result;
}

inline uint8 *
getUsed( MEMORY * memory ) {
    uint8 * Result = ( uint8 * )memory->base + memory->used;
    return Result;
}

internal MEMORY
SubArena( MEMORY * parentArena, int32 size ) {
	MEMORY Result = {};
	Result.base = _pushSize_clear( parentArena, size );
	Result.size = size;
	return Result;
}

internal MEMORY
SubArena( MEMORY * parentArena ) {
    MEMORY Result = SubArena( parentArena, ( int32 )( parentArena->size - parentArena->used ) );
    return Result;
}

internal MEMORY *
SnapMemory( MEMORY * memory ) {
    Assert( memory->snap_index < MEMORY__SNAP_COUNT );
    memory->snapUsed[ memory->snap_index++ ] = memory->used;
    
    MEMORY * Result = memory;
    return Result;
}

internal uint8 *
getSnapBase( MEMORY * memory ) {
    Assert( memory->snap_index > 0 );
    uint8 * Result = ( uint8 * )memory->base + memory->snapUsed[ memory->snap_index - 1 ];
    return Result;
}

internal uint32
getSnapUsed( MEMORY * memory ) {
    Assert( memory->snap_index > 0 );
    uint32 Result = ( uint32 )( memory->used - memory->snapUsed[ memory->snap_index - 1 ] );
    return Result;
}

internal void
popToSnap( MEMORY * memory ) {
    Assert( memory->snap_index > 0 );
    memory->used = memory->snapUsed[ --memory->snap_index ];
}

#endif	// STD_INCLUDE_FUNC