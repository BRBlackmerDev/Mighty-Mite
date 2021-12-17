
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

internal flo32
absValue( flo32 value ) {
	flo32 Result = ( value < 0.0f ? -value : value );
	return Result;
}

inline flo32
squareValue( flo32 value ) {
	flo32 Result = value * value;
	return Result;
}