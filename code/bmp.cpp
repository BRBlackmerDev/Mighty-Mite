
#pragma pack( push, 1 )
struct BMP_HEADER {
	uint16 FileType;             // MUST be 0x4d42
	uint32 FileSize;
	uint16 reserved01;           // UNUSED
	uint16 reserved02;           // UNUSED
	uint32 offsetToData;         // sizeof( BMP_HEADER )
	uint32 bitmapInfoHeaderSize; // MUST be 40
	uint32 Width;
	uint32 Height;
	uint16 planes;               // MUST be 1
	uint16 bitsPerPixel;
	uint32 compressionType;      // UNUSED
	uint32 bitmapSize_inBytes;   // UNUSED
	uint32 xPixelsPerMeter;      // UNUSED
	uint32 yPixelsPerMeter;      // UNUSED
	uint32 ColorCount;           // UNUSED
	uint32 importantColorCount;  // UNUSED
};
#pragma pack( pop )

struct TEXTURE_DATA {
	uint32  Width;
	uint32  Height;
	uint32  BytesPerPixel;
    uint8 * TexelData;
};

internal TEXTURE_DATA
BMP_ReadFile( PLATFORM * Platform, char * SaveDir, char * FileName ) {
	TEXTURE_DATA Result = {};
	
	// TODO: verify requested File extension
	FILE_DATA File = Platform->ReadFile( &Platform->TempMemory, SaveDir, FileName, "bmp" );
	
	if( File.contents ) {
		BMP_HEADER * info = ( BMP_HEADER * )File.contents;
		Result.Width         = info->Width;
		Result.Height        = info->Height;
		Result.BytesPerPixel = 4;
		
		uint32 BMP_bytesPerPixel = info->bitsPerPixel / 8;
		
		uint8 * BMP_Data = ( uint8 * )File.contents + info->offsetToData;
		uint8 * ptr = BMP_Data;
		
		uint32 pixelCount  = Result.Width * Result.Height;
		uint32 TextureSize = pixelCount * Result.BytesPerPixel;
		Result.TexelData = ( uint8 * )_pushSize( &Platform->PermMemory, TextureSize );
		
		flo32 inv255 = 1.0f / 255.0f;
		uint32 * ResultData  = ( uint32 * )Result.TexelData;
        
        if( BMP_bytesPerPixel == 4 ) {
            uint32 * TextureData = ( uint32 * )BMP_Data;
            for( uint32 counter = 0; counter < pixelCount; counter++ ) {
                // TODO: RGBA vs BGRA
                uint32 inColor = TextureData[ counter ];
                
                uint8 r = ( uint8 )( ( inColor >>  0 ) & 0xFF );
                uint8 g = ( uint8 )( ( inColor >>  8 ) & 0xFF );
                uint8 b = ( uint8 )( ( inColor >> 16 ) & 0xFF );
                uint8 a = ( uint8 )( ( inColor >> 24 ) & 0xFF );
                flo32 alpha = ( flo32 )a * inv255;
                
                uint32 outColor =
                    ( ( uint8 )( ( flo32 )b * alpha ) <<  0 ) |
                    ( ( uint8 )( ( flo32 )g * alpha ) <<  8 ) |
                    ( ( uint8 )( ( flo32 )r * alpha ) << 16 ) |
                    ( a << 24 );
                
                ResultData[ counter ] = outColor;
            }
        } else if( BMP_bytesPerPixel == 3 ) {
            uint8 * TextureData = BMP_Data;
            for( uint32 counter = 0; counter < pixelCount; counter++ ) {
                // TODO: RGBA vs BGRA
                uint8 r = *TextureData++;
                uint8 g = *TextureData++;
                uint8 b = *TextureData++;
                uint8 a = 255;
                
                uint32 outColor = ( b <<  0 ) | ( g <<  8 ) | ( r << 16 ) | ( a << 24 );
                
                ResultData[ counter ] = outColor;
            }
        }
		
		_popSize( &Platform->TempMemory, File.size );
	}
	
	return Result;
}