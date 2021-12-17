
struct FILE_PARSER {
    char * Start;
    char * at;
    uint32 size;
};

internal FILE_PARSER
FileParser( void * memory, int32 size ) {
    FILE_PARSER Result = { ( char * )memory, ( char * )memory, (uint32)size };
    return Result;
}

internal FILE_PARSER
FileParser( STRING str ) {
    FILE_PARSER Result = {};
    Result.Start = str.string;
    Result.at    = str.string;
    Result.size  = str.nChar;
    return Result;
}

internal boo32
hasTextRemaining( FILE_PARSER * parser ) {
    boo32 Result = ( ( parser->at - parser->Start ) < parser->size );
    return Result;
}

internal boo32
isWhiteSpace( char c ) {
    boo32 Result = ( c == 0 ) || ( c == ' ' ) || ( c == '\n' ) || ( c == '\t' ) || ( c == '\r' );
    return Result;
}

internal void
eatWhiteSpace( FILE_PARSER * parser ) {
    while( hasTextRemaining( parser ) && ( isWhiteSpace( *parser->at ) ) ) { parser->at++; }
}

internal void
eatNonWhiteSpace( FILE_PARSER * parser ) {
    while( hasTextRemaining( parser ) && ( !isWhiteSpace( *parser->at ) ) ) { parser->at++; }
}

internal char *
getEnd( char * str ) {
    Assert( ( str ) && ( str[ 0 ] ) );
    
    char * Result = str;
    while( Result[ 0 ] ) { Result++; }
    return Result;
}

internal STRING
parseToken( FILE_PARSER * parser ) {
    Assert( parser->size > 0 );
    
    STRING Result = {};
    
    eatWhiteSpace( parser );
    
    if( hasTextRemaining( parser ) ) {
        Result.string = parser->at;
    }
    
    eatNonWhiteSpace( parser );
    Result.nChar = ( uint32 )( parser->at - Result.string );
    
    eatWhiteSpace( parser );
    
    return Result;
}

internal STRING
parseLine( FILE_PARSER * parser ) {
    STRING Result = {};
    
    eatWhiteSpace( parser );
    
    if( hasTextRemaining( parser ) ) {
        Result.string = parser->at;
    }
    
    while( ( *parser->at != 0 ) && ( *parser->at != '\n' ) && ( *parser->at != '\r' ) ) { parser->at++; }
    Result.nChar = ( uint32 )( parser->at - Result.string );
    
    eatWhiteSpace( parser );
    return Result;
}

internal uint8
parseU8( STRING token ) {
    uint8 Result = ( uint8 )strtoul( token.string, 0, 0 );
    return Result;
}

internal uint8
parseU8( FILE_PARSER * parser ) {
    STRING token  = parseToken( parser );
    uint8  Result = parseU8( token );
    return Result;
}

internal uint16
parseU16( STRING token ) {
    uint16 Result = ( uint16 )strtoul( token.string, 0, 0 );
    return Result;
}

internal uint32
parseU32( STRING token ) {
    uint32 Result = strtoul( token.string, 0, 0 );
    return Result;
}

internal uint32
parseU32( FILE_PARSER * parser ) {
    STRING token  = parseToken( parser );
    uint32 Result = parseU32( token );
    return Result;
}

internal int32
parseS32( STRING token ) {
    int32  Result = strtol( token.string, 0, 0 );
    return Result;
}

internal int32
parseS32( FILE_PARSER * parser ) {
    STRING token  = parseToken( parser );
    int32  Result = parseS32( token );
    return Result;
}

internal flo32
parseF32( STRING token ) {
    flo32  Result = strtof( token.string, 0 );
    return Result;
}

internal flo32
parseF32( FILE_PARSER * parser ) {
    STRING token  = parseToken( parser );
    flo32  Result = parseF32( token );
    return Result;
}

internal vec2
parseV2( STRING token ) {
    FILE_PARSER parser = FileParser( token.string, token.nChar );
    
    vec2 Result = {};
    for( uint32 iElem = 0; iElem < 2; iElem++ ) {
        Result.elem[ iElem ] = parseF32( &parser );
    }
    return Result;
}

internal vec3
parseV3( STRING token ) {
    FILE_PARSER parser = FileParser( token.string, token.nChar );
    
    vec3 Result = {};
    for( uint32 iElem = 0; iElem < 3; iElem++ ) {
        Result.elem[ iElem ] = parseF32( &parser );
    }
    return Result;
}

internal vec4
parseV4( STRING token ) {
    FILE_PARSER parser = FileParser( token.string, token.nChar );
    
    vec4 Result = {};
    for( uint32 iElem = 0; iElem < 4; iElem++ ) {
        Result.elem[ iElem ] = parseF32( &parser );
    }
    return Result;
}

internal quat
parseQuat( STRING token ) {
    FILE_PARSER parser = FileParser( token.string, token.nChar );
    
    quat Result = {};
    for( uint32 iElem = 0; iElem < 4; iElem++ ) {
        Result.elem[ iElem ] = parseF32( &parser );
    }
    return Result;
}