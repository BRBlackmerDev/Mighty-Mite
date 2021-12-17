
#pragma pack( push, 1 )
struct WAV_HEADER {
	uint32 ckID;
	uint32 ckSize;
	uint32 waveID;
};

struct BLOCK_HEADER {
	uint32 marker;
	uint32 size;
};

struct FMT_BLOCK {
	uint16 format;
	uint16 channels;
	uint32 sampleRate;
	uint32 avgDataRate;
	uint16 blockAlign;
	uint16 bitsPerSample;
	uint16 extensionSize;
};
#pragma pack( pop )

internal AUDIO_DATA
WAV_ReadFile( PLATFORM * Platform, char * SaveDir, char * FileName ) {
	AUDIO_DATA Result = {};
	FILE_DATA File = Platform->ReadFile( &Platform->TempMemory, SaveDir, FileName, "wav" );
	if( File.contents ) {
		uint32 riffTag = *( ( uint32 * )&"RIFF" );
		uint32 waveTag = *( ( uint32 * )&"WAVE" );
		uint32  fmtTag = *( ( uint32 * )&"fmt " );
		uint32 dataTag = *( ( uint32 * )&"data" );
		
		uint8 * Start = ( uint8 * )File.contents;
		uint8 * at = Start;
		
		WAV_HEADER * header = _addr( at, WAV_HEADER );
		Assert( header->ckID == riffTag );
		Assert( header->waveID == waveTag );
		
		BLOCK_HEADER * blockA = _addr( at, BLOCK_HEADER );
		Assert( blockA->marker == fmtTag );
		
		FMT_BLOCK * fmt = _addr( at, FMT_BLOCK );
		Assert( fmt->format == 1 ); // PCM format
		// Assert( fmt->channels == 1 );
		Assert( fmt->channels == 2 );
		Assert( fmt->sampleRate == 48000 );
		Assert( fmt->bitsPerSample == 16 );
		
		BLOCK_HEADER * blockB = _addr( at, BLOCK_HEADER );
		Assert( blockB->marker == dataTag );
		
		uint8 * src = at;
		uint8 * dest = ( uint8 * )_pushSize( &Platform->PermMemory, blockB->size );
		memcpy( dest, src, blockB->size );
		
		Assert( blockB->size % ( ( fmt->bitsPerSample / 8 ) * fmt->channels ) == 0 );
		uint32 samples = blockB->size / ( ( fmt->bitsPerSample / 8 ) * fmt->channels );
		
		Result = AudioData( dest, samples );
	}
	return Result;
}