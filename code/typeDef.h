

#define internal				static
#define local_persist		static
#define global_variable		static

#if DEBUG_BUILD
#define Assert( Expression ) if( !( Expression ) ) { *( int32 * )0 = 0; }
#else
#define Assert( Expression )
#endif

#define InvalidCodePath		Assert( 0 )
#define NotImplemented  	Assert( 0 )

#define _ArrayCount( array )  ( sizeof( array ) / sizeof( array[0] ) )
#define _arrayBytes( array )  ( sizeof( array ) )

#define _KB( value ) ( value * 1024LL )
#define _MB( value ) ( value * 1024LL * 1024LL )
#define _GB( value ) ( value * 1024LL * 1024LL * 1024LL )

#define COLOR_BLACK          Vec4( 0.0f, 0.0f, 0.0f, 1.0f )
#define COLOR_WHITE          Vec4( 1.0f, 1.0f, 1.0f, 1.0f )
#define COLOR_BLUE           Vec4( 0.0f, 0.0f, 1.0f, 1.0f )
#define COLOR_CYAN           Vec4( 0.0f, 1.0f, 1.0f, 1.0f )
#define COLOR_GREEN          Vec4( 0.0f, 1.0f, 0.0f, 1.0f )
#define COLOR_MAGENTA        Vec4( 1.0f, 0.0f, 1.0f, 1.0f )
#define COLOR_ORANGE         Vec4( 1.0f, 0.5f, 0.0f, 1.0f )
#define COLOR_PURPLE         Vec4( 0.5f, 0.0f, 1.0f, 1.0f )
#define COLOR_RED            Vec4( 1.0f, 0.0f, 0.0f, 1.0f )
#define COLOR_YELLOW         Vec4( 1.0f, 1.0f, 0.0f, 1.0f )
#define COLOR_GRAY( value )  Vec4( value, value, value, 1.0f )
#define COLOR_TRANSPARENT    Vec4( 0.0f, 0.0f, 0.0f, 0.0f )

#define _MinValue( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define _MaxValue( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )

#define _swap( Type, valueA, valueB )  { Type _temp_ = valueA; valueA = valueB; valueB = _temp_; }
#define _addr( ptr, Type )  ( Type * )ptr;  ptr += sizeof( Type );
#define _addra( ptr, Type, count )  ( Type * )ptr;  ptr += ( sizeof( Type ) * count );
#define _addrStr( ptr )  ( char * )ptr;  while( *ptr ) { ptr++; }  ptr++;
#define _read( ptr, Type )  ( ( Type * )ptr )[0];  ptr += sizeof( Type );
#define _readStr( src, dest )  while( *src ) { *dest++ = *src++; }  *dest = 0;
#define _write( ptr, Type, value )   ( ( Type * )ptr )[0] = value;  ptr += sizeof( Type );
#define _writem( memory, Type, value )   { Type * var = _pushType( memory, Type );  *var = value; }
#define _writeb( memory, src, size ) { void * dest = _pushSize( memory, size );  memcpy( dest, src, size ); }
#define _writea( memory, src, Type, count )  { Type * dest = _pushArray( memory, Type, count ); memcpy( dest, src, sizeof( Type ) * ( count ) ); }
#define _copys( src, dest, count )  { for( int32 index = 0; index < count; index++ ) { *dest++ = *src++; } }
#define _copy( Type, src, dest, count ) { Type * ptrA = ( Type * )src; Type * ptrB = ( Type * )dest; for( int32 index = 0; index < count; index++ ) { ptrB[ index ] = ptrA[ index ]; } }
#define _copym( memory, ptr, size ) { uint8 * dest = ( uint8 * )_pushSize( memory, size ); uint8 * src = ( uint8 * )ptr; for( int32 iter = 0; iter < size; iter++ ) { *dest++ = *src++; } }
#define _copya( memory, src, Type, count )  ( Type * )copyBlock( memory, src, sizeof( Type ) * ( count ) );  src += ( sizeof( Type ) * ( count ) );

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float  flo32;
typedef double flo64;

typedef int32 boo32;