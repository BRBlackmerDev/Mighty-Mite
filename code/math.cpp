
#define PI							( 3.14159274f )
#define TAU                   ( 6.28318531f )
#define RADIANS_TO_DEGREES		( 57.2957795f )
#define DEGREES_TO_RADIANS  	( 0.0174533f )

internal flo32
MinValue( flo32 a, flo32 b ) {
	flo32 Result = ( ( a < b ) ? a : b );
	return Result;
}

internal flo32
MaxValue( flo32 a, flo32 b ) {
	flo32 Result = ( ( a > b ) ? a : b );
	return Result;
}

internal int32
MinValue( int32 a, int32 b ) {
	int32 Result = ( ( a < b ) ? a : b );
	return Result;
}

internal int32
MaxValue( int32 a, int32 b ) {
	int32 Result = ( ( a > b ) ? a : b );
	return Result;
}

internal uint32
MinValue( uint32 a, uint32 b ) {
	uint32 Result = ( ( a < b ) ? a : b );
	return Result;
}

internal uint32
MaxValue( uint32 a, uint32 b ) {
	uint32 Result = ( ( a > b ) ? a : b );
	return Result;
}

internal int16
MinValue( int16 a, int16 b ) {
	int16 Result = ( ( a < b ) ? a : b );
	return Result;
}

internal int16
MaxValue( int16 a, int16 b ) {
	int16 Result = ( ( a > b ) ? a : b );
	return Result;
}

internal uint16
MinValue( uint16 a, uint16 b ) {
	uint16 Result = ( ( a < b ) ? a : b );
	return Result;
}

internal uint16
MaxValue( uint16 a, uint16 b ) {
	uint16 Result = ( ( a > b ) ? a : b );
	return Result;
}

internal flo32
Clamp( flo32 value, flo32 lo, flo32 hi ) {
    if( lo > hi ) {
        _swap( flo32, lo, hi );
    }
	flo32 Result = MaxValue( MinValue( value, hi ), lo );
	return Result;
}

internal flo32
Clamp01( flo32 value ) {
	flo32 Result = Clamp( value, 0.0f, 1.0f );
	return Result;
}

internal flo32
clout( flo32 value, flo32 lo, flo32 hi ) {
    // NOTE: opposite of Clamp; forces value outside the range of [lo..hi]; rounds to the nearest value
    flo32 Result = value;
    if( ( value > lo ) && ( value < hi ) ) {
        flo32 mid = ( lo + hi ) * 0.5f;
        Result = ( value < mid ) ? lo : hi;
    }
    return Result;
}

internal int32
Clamp( int32 value, int32 min, int32 max ) {
    if( min > max ) {
        _swap( int32, min, max );
    }
    
    int32 Result = value;
    if( min > Result ) { Result = min; }
    if( max < Result ) { Result = max; }
    return Result;
}

internal uint32
Clamp( uint32 value, uint32 min, uint32 max ) {
    if( min > max ) {
        _swap( uint32, min, max );
    }
    
    uint32 Result = value;
    if( min > Result ) { Result = min; }
    if( max < Result ) { Result = max; }
    return Result;
}

internal int32
roundToInt32( flo32 value ) {
	int32 Result = ( int32 )( value >= 0.0f ? value + 0.5f : value - 0.5f );
	return Result;
}

inline uint32
roundToU32( flo32 value ) {
	uint32 Result = ( uint32 )( value + 0.5f );
	return Result;
}

internal int32
ceilToInt32( flo32 value ) {
	int32 Result = ( int32 )value;
	if( value - ( int32 )value != 0.0f ) {
		Result = ( int32 )( value >= 0.0f ? value + 1.0f : value - 1.0f );
	}
	return Result;
}

internal flo32
roundToPoint5( flo32 value ) {
	int32 truncatedValue = ( int32 )value;
	flo32 Result = ( value >= 0.0f ? ( flo32 )truncatedValue + 0.5f : ( flo32 )truncatedValue - 0.5f );
	return Result;
}

internal flo32
floorValue( flo32 value ) {
	flo32 Result = ( flo32 )( ( int32 )value );
	return Result;
}

internal flo32
ceilValue( flo32 value ) {
	flo32 Result = value;
	if( value - ( int32 )value != 0.0f ) {
		Result = ( flo32 )( ( int32 )( value >= 0.0f ? value + 1.0f : value - 1.0f ) );
	}
	return Result;
}

internal uint32
truncToU32( uint64 value ) {
	Assert( value <= 0xFFFFFFFF );
	uint32 Result = ( uint32 )value;
	return Result;
}

internal uint16
truncToU16( uint32 value ) {
	Assert( value <= 0xFFFF );
	uint16 Result = ( uint16 )value;
	return Result;
}

internal uint8
truncToU8( uint32 value ) {
	Assert( value <= 0xFF );
	uint8 Result = ( uint8 )value;
	return Result;
}

internal uint8
truncToU8( int32 value ) {
	Assert( value >= 0 && value <= 0xFF );
	uint8 Result = ( uint8 )value;
	return Result;
}

internal uint8
toU8( flo32 value ) {
	Assert( value >= 0.0f && value < ( flo32 )0xFF + 1.0f );
	uint8 Result = ( uint8 )value;
	return Result;
}

internal uint16
toU16( flo32 value ) {
	Assert( value >= 0.0f && value < ( flo32 )0xFFFF + 1.0f );
	uint16 Result = ( uint16 )value;
	return Result;
}

internal uint16
toU16( char * string ) {
	Assert( strlen( string ) <= 2 );
	char * at = string;
	uint16 Result = 0;
	uint32 char_index = 0;
	while( *at ) {
		Result |= ( *at++ << ( 8 * char_index++ ) );
	}
	return Result;
}

internal uint32
toU32( flo32 value ) {
	Assert( value >= 0.0f && value < ( flo32 )0xFFFFFFFF + 1.0f );
	uint32 Result = ( uint32 )value;
	return Result;
}

internal uint32
toU32( char * string ) {
	Assert( strlen( string ) <= 4 );
	char * at = string;
	uint32 Result = 0;
	uint32 char_index = 0;
	while( *at ) {
		Result |= ( *at++ << ( 8 * char_index++ ) );
	}
	return Result;
}

internal uint32
absValue( int32 value ) {
	uint32 Result = ( value < 0 ? -value : value );
	return Result;
}

internal flo32
absValue( flo32 value ) {
	flo32 Result = ( value < 0.0f ? -value : value );
	return Result;
}

internal flo32
expValue( flo32 base, int32 exponent ) {
	uint32 count = absValue( exponent );
	flo32 Result = 1.0f;
	for( uint32 counter = 0; counter < count; counter++ ) {
		Result *= base;
	}
	return Result;
}

internal int32
expValue( int32 base, int32 exponent ) {
	uint32 count = absValue( exponent );
	int32 Result = 1;
	for( uint32 counter = 0; counter < count; counter++ ) {
		Result *= base;
	}
	return Result;
}

internal uint32
expValue( uint32 base, uint32 exponent ) {
	Assert( exponent >= 0 );
	uint32 count = exponent;
	uint32 Result = 1;
	for( uint32 counter = 0; counter < count; counter++ ) {
		Result *= base;
	}
	return Result;
}

internal int32
randomValue( int32 lowBound, int32 highBound ) {
	int32 range = highBound - lowBound;
	int32 value = rand() % range;
	int32 Result = value + lowBound;
	return Result;
}

internal flo32
randomFloat( flo32 lowBound, flo32 highBound ) {
	int32 value = randomValue( ( int32 )( lowBound * 10.0f ), ( int32 )( highBound * 10.0f ) );
	flo32 Result = ( flo32 )value / 10.0f;
	return Result;
}

internal flo32
Random4() {
    flo32 Result = ( ( flo32 )( rand() % 10001 ) * 0.0001f );
    return Result;
}

inline flo32
squareValue( flo32 value ) {
	flo32 Result = value * value;
	return Result;
}

inline flo32
toDegrees( flo32 radians ) {
	flo32 Result = radians * RADIANS_TO_DEGREES;
	return Result;
}

inline flo32
ToRadians( flo32 degrees ) {
	flo32 Result = degrees * DEGREES_TO_RADIANS;
	return Result;
}

inline flo32
cosRadians( flo32 radians ) {
	flo32 Result = cosf( radians );
	return Result;
}

inline flo32
sinRadians( flo32 radians ) {
	flo32 Result = sinf( radians );
	return Result;
}

inline flo32
cosDegrees( flo32 degrees ) {
	flo32 Result = cosRadians( ToRadians( degrees ) );
	return Result;
}

inline flo32
getDegrees( flo32 x, flo32 y ) {
	flo32 Result = toDegrees( ( flo32 )atan2f( y, x ) );
	if( Result < 0.0f ) {
		Result += 360.0f;
	}
	return Result;
}

inline flo32
sinDegrees( flo32 degrees ) {
	flo32 Result = sinRadians( ToRadians( degrees ) );
	return Result;
}

inline flo32
lerp( flo32 a, flo32 t, flo32 b ) {
	flo32 Result = ( 1.0f - t ) * a + t * b;
	return Result;
}

internal flo32
RandomF32() {
    flo32 Result = ( flo32 )rand() / ( flo32 )RAND_MAX;
    return Result;
}

internal flo32
RandomF32_2Dice() {
    flo32 Result = ( RandomF32() + RandomF32() ) / 2.0f;
    return Result;
}

internal flo32
RandomF32_3Dice() {
    flo32 Result = ( RandomF32() + RandomF32() + RandomF32() ) / 3.0f;
    return Result;
}

internal flo32
RandomF32_4Dice() {
    flo32 Result = ( RandomF32() + RandomF32() + RandomF32() + RandomF32() ) / 4.0f;
    return Result;
}

internal flo32
RandomF32InRange( flo32 lo, flo32 hi ) {
    flo32 Result = lo + RandomF32() * ( hi - lo );
    return Result;
}

internal uint32
RandomU32InRange( uint32 lo, uint32 hi ) {
    uint32 Result = lo + rand() % ( hi - lo + 1 );
    return Result;
}

internal flo32
getDecimal( flo32 value ) {
    flo32 Result = value - ( flo32 )( ( int32 )value );
    return Result;
}

internal uint32
UInt32Wrap( uint32 Init, int32 Change, uint32 Wrap ) {
    Assert( Wrap > 0 );
    
    uint32 Result = Init;
    
    if( Change != 0 ) {
        if( Change > 0 ) {
            while( ( uint32 )(      Change   ) >= Wrap ) { Change -= Wrap; }
        } else {
            while( ( uint32 )( abs( Change ) ) >= Wrap ) { Change += Wrap; }
        }
        
        Result = ( uint32 )( ( int32 )( Init + Wrap ) + Change ) % Wrap;
    }
    return Result;
}