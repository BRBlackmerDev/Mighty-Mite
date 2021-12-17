
#if 0
struct MUSIC_SYSTEM {
    ASSET_SYSTEM * assetSystem;
    
    flo32 mainVolume;
    
    uint8 * audio;
    uint32 nSamples;
    
#define MAX_SAMPLES_TO_WRITE  ( ( AUDIO__SAMPLES_PER_SECOND / ( uint32 )APP_TargetFPS ) * AUDIO__WRITE_FRAMES )
    flo32  atSample_t[ MAX_SAMPLES_TO_WRITE ];
    uint32 atSample  [ MAX_SAMPLES_TO_WRITE ];
    
    uint8 * mixBuffer;
    int32   mixBuffer_size;
    
    // TODO: AudioSystem should handle interpolation of rate over time
    flo32 rate;
    flo32 volume;
};
#endif

internal AUDIO_DATA
AudioData( void * audio, uint32 samples ) {
    AUDIO_DATA Result = { ( uint8 * )audio, samples };
    return Result;
}

internal AUDIO_ENTRY
AudioEntry( AUDIO_ID AudioID, AUDIO_DATA Audio, flo32 Volume, flo32 Rate, boo32 IsLooping ) {
    AUDIO_ENTRY Result = {};
    Result.AudioID   = AudioID;
    Result.Audio     = Audio.Audio;
    Result.nSamples  = Audio.nSamples;
    Result.Volume    = Volume;
    Result.Rate      = Rate;
    Result.IsLooping = IsLooping;
    return Result;
}

internal void
AdvanceAudio( AUDIO_SYSTEM * AudioSystem, uint32 SamplesToAdvance ) {
    for( uint32 iAudio = 0; iAudio < AudioSystem->nAudio; iAudio++ ) {
        AudioSystem->Audio[ iAudio ].AtSample += SamplesToAdvance;
    }
    
    int32 newAudioCount = 0;
    for( uint32 iAudio = 0; iAudio < AudioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY Entry = AudioSystem->Audio[ iAudio ];
        if( Entry.AtSample < Entry.nSamples ) {
            AudioSystem->Audio[ newAudioCount++ ] = Entry;
        } else if( Entry.IsLooping ) {
            Entry.AtSample -= Entry.nSamples;
            AudioSystem->Audio[ newAudioCount++ ] = Entry;
        }
    }
    AudioSystem->nAudio = newAudioCount;
}

internal void
MixAudio( AUDIO_SYSTEM * AudioSystem, uint32 SamplesToAdvance ) {
    int32 MaxSamplesToWrite = ( AUDIO__SAMPLES_PER_SECOND / 60 ) * AUDIO__WRITE_FRAMES;
    
    for( uint32 iAudio = 0; iAudio < AudioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY * Entry = AudioSystem->Audio + iAudio;
        
        int32 SamplesToWrite = Entry->nSamples - Entry->AtSample;
        SamplesToWrite = MinValue( SamplesToWrite, MaxSamplesToWrite );
        
        uint16 * Src  = ( uint16 * )( Entry->Audio + ( Entry->AtSample * AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT ) );
        uint16 * Dest = ( uint16 * )AudioSystem->MixBuffer;
        for( int32 Sample_index = 0; Sample_index < SamplesToWrite; Sample_index++ ) {
            int16 Value0 = *Src++;
            int16 Value1 = *Src++;
            
            Value0 = ( int16 )( ( flo32 )Value0 * Entry->Volume * AudioSystem->MainVolume );
            Value1 = ( int16 )( ( flo32 )Value1 * Entry->Volume * AudioSystem->MainVolume );
            
            *Dest++ += Value0;
            *Dest++ += Value1;
        }
    }
}

internal void
PlayAudio( AUDIO_SYSTEM * AudioSystem, AUDIO_DATA * AudioList, AUDIO_ID AudioID, flo32 Volume, flo32 Rate = 1.0f, boo32 IsLooping = false ) {
    Assert( AudioSystem->nAudio < AUDIO__MAX_ENTRY_COUNT );
    AudioSystem->Audio[ AudioSystem->nAudio++ ] = AudioEntry( AudioID, AudioList[ AudioID ], Volume, Rate, IsLooping );
}

internal void
PlayAudio( APP_STATE * AppState, AUDIO_ID AudioID, flo32 Volume, flo32 Rate = 1.0f, boo32 IsLooping = false ) {
    AUDIO_SYSTEM * AudioSystem = AppState->AudioSystem;
    AUDIO_DATA   * AudioList   = AppState->AudioList;
    
    Assert( AudioSystem->nAudio < AUDIO__MAX_ENTRY_COUNT );
    AudioSystem->Audio[ AudioSystem->nAudio++ ] = AudioEntry( AudioID, AudioList[ AudioID ], Volume, Rate, IsLooping );
}

internal void
StopAudio( AUDIO_SYSTEM * AudioSystem, AUDIO_ID AudioID ) {
    uint32 NewCount = 0;
    for( uint32 iAudio = 0; iAudio < AudioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY Entry = AudioSystem->Audio[ iAudio ];
        if( Entry.AudioID != AudioID ) {
            AudioSystem->Audio[ NewCount++ ] = Entry;
        }
    }
    AudioSystem->nAudio = NewCount;
}

//----------
// music functions
//----------

#if 0
internal void
advanceMusic( MUSIC_SYSTEM * musicSystem, uint32 samplesToAdvance ) {
    flo32 samplesf = ( flo32 )samplesToAdvance * musicSystem->ratePrev;
    int32 samples  = ( int32 )samplesf;
    flo32 t        = samplesf - ( flo32 )samples;
    
    musicSystem->samplesPlayed += samples;
    musicSystem->t              = ( musicSystem->t + t ) - ( flo32 )( ( int32 )musicSystem->t );
    
    if( musicSystem->samplesPlayed > musicSystem->samples ) {
        musicSystem->samplesPlayed = 0;
        musicSystem->t             = 0.0f;
    }
}
#endif

#if 0
internal void
mixMusic( MUSIC_SYSTEM * musicSystem, uint32 samplesToAdvance ) {
    uint32 atSample   = musicSystem->atSample  [ samplesToAdvance ];
    flo32  atSample_t = musicSystem->atSample_t[ samplesToAdvance ];
    
    int16 * dest = ( int16 * )musicSystem->mixBuffer;
    for( int32 iSample = 0; iSample < MAX_SAMPLES_TO_WRITE; iSample++ ) {
        musicSystem->atSample  [ iSample ] = atSample;
        musicSystem->atSample_t[ iSample ] = atSample_t;
        
        flo32  t      = atSample_t;
        uint32 index0 = atSample;
        uint32 index1 = ( index0 + 1 ) % musicSystem->nSamples;
        
        uint32 src0 = ( ( uint32 * )musicSystem->audio )[ index0 ];
        uint32 src1 = ( ( uint32 * )musicSystem->audio )[ index1 ];
        
        int16 lo0 = ( int16 )( src0 ^ 0x0000FFFF );
        int16 lo1 = ( int16 )( src1 ^ 0x0000FFFF );
        
        int16 hi0 = ( int16 )( ( src0 >> 16 ) ^ 0x0000FFFF );
        int16 hi1 = ( int16 )( ( src1 >> 16 ) ^ 0x0000FFFF );
        
        int16 lo = ( int16 )( ( 1.0f - t ) * ( flo32 )lo0 + t * ( flo32 )lo1 );
        int16 hi = ( int16 )( ( 1.0f - t ) * ( flo32 )hi0 + t * ( flo32 )hi1 );
        
        lo = ( int16 )( ( flo32 )lo * musicSystem->volume * musicSystem->mainVolume );
        hi = ( int16 )( ( flo32 )hi * musicSystem->volume * musicSystem->mainVolume );
        
        *dest++ += lo;
        *dest++ += hi;
        
        atSample_t += musicSystem->rate;
        while( atSample_t >= 1.0f ) {
            atSample_t -= 1.0f;
            atSample++;
            if( atSample >= musicSystem->nSamples ) {
                atSample -= musicSystem->nSamples;
            }
        }
    }
}

internal void
playMusic( MUSIC_SYSTEM * musicSystem, AUDIO_DATA music, flo32 volume, flo32 rate ) {
    musicSystem->audio    = music.audio;
    musicSystem->nSamples = music.samples;
    musicSystem->volume   = volume;
    musicSystem->rate     = rate;
    
    memset( musicSystem->atSample, 0, sizeof( flo32 ) * MAX_SAMPLES_TO_WRITE );
}

internal void
setMusicRate( MUSIC_SYSTEM * musicSystem, flo32 rate ) {
    musicSystem->rate = rate;
}
#endif

#define MUSIC__MIX_VOLUME  ( 1.0f )
#define AUDIO__MIX_VOLUME  ( 1.0f )

struct WIN32_AUDIO {
    LPDIRECTSOUNDBUFFER Buffer;
    uint32              BufferSize;
    DWORD               PrevWrite;
    int32               WriteSize;
};

internal WIN32_AUDIO
InitAudio( HWND Window, AUDIO_SYSTEM * AudioSystem, MEMORY * Memory ) {
    boo32 ErrorOccurred = false;
    
    uint32 AudioBuffer_Size = AUDIO__SAMPLES_PER_SECOND * AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT * AUDIO__BUFFER_SECONDS;
    LPDIRECTSOUNDBUFFER AudioBuffer = {};
    HMODULE AudioBuffer_Lib = LoadLibrary( "dsound.dll" );
    if( AudioBuffer_Lib ) {
        DIRECT_SOUND_CREATE * DirectSoundCreate = ( DIRECT_SOUND_CREATE * )GetProcAddress( AudioBuffer_Lib, "DirectSoundCreate" );
        
        LPDIRECTSOUND DirectSound;
        if( DirectSoundCreate && SUCCEEDED( DirectSoundCreate( 0, &DirectSound, 0 ) ) ) {
            WAVEFORMATEX format = {};
            format.wFormatTag      = WAVE_FORMAT_PCM;
            format.nChannels       = ( WORD )AUDIO__CHANNEL_COUNT;
            format.nSamplesPerSec  = AUDIO__SAMPLES_PER_SECOND;
            format.wBitsPerSample  = ( WORD )( AUDIO__BYTES_PER_SAMPLE * 8 );
            format.nBlockAlign     = ( WORD )( AUDIO__CHANNEL_COUNT * AUDIO__BYTES_PER_SAMPLE );
            format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
            
            if( SUCCEEDED( DirectSound->SetCooperativeLevel( Window, DSSCL_PRIORITY ) ) ) {
                DSBUFFERDESC PrimaryDesc = { sizeof( DSBUFFERDESC ) };
                PrimaryDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
                
                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                if(   SUCCEEDED( DirectSound->CreateSoundBuffer( &PrimaryDesc, &PrimaryBuffer, 0 ) )
                   && SUCCEEDED( PrimaryBuffer->SetFormat( &format ) ) ) {
                    CONSOLE_STRING( "Primary Sound Buffer was created successfully.\n" );
                } else {
                    ErrorOccurred = true;
                }
            }
            
            DSBUFFERDESC SecondaryDesc = { sizeof( DSBUFFERDESC ) };
            SecondaryDesc.dwBufferBytes = AudioBuffer_Size;
            SecondaryDesc.lpwfxFormat = &format;
            if( SUCCEEDED( DirectSound->CreateSoundBuffer( &SecondaryDesc, &AudioBuffer, 0 ) ) ) {
                AudioBuffer->Play( 0, 0, DSBPLAY_LOOPING );
                CONSOLE_STRING( "Secondary Sound Buffer was created successfully.\n" );
            } else {
                ErrorOccurred = true;
            }
        } else {
            ErrorOccurred = true;
        }
    } else {
        ErrorOccurred = true;
    }
    
    if( ErrorOccurred ) {
        CONSOLE_STRING( "ERROR! Error occurred when initializing the audio system!" );
        GlobalVar_DebugSystem.ErrorOccurred = true;
    }
    
    int32 Audio_BytesPerFrame   = ( AUDIO__SAMPLES_PER_SECOND / 60 ) * AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT;
    int32 Audio_WriteSize       = AUDIO__WRITE_FRAMES * Audio_BytesPerFrame;
    AudioSystem->MixBuffer      = ( uint8 * )_pushSize_clear( Memory, Audio_WriteSize );
    AudioSystem->MixBuffer_Size = Audio_WriteSize;
    
    DWORD Audio_Ignore    = 0;
    DWORD Audio_PrevWrite = 0;
    AudioBuffer->GetCurrentPosition( &Audio_Ignore, &Audio_PrevWrite );
    
    WIN32_AUDIO Result = {};
    Result.Buffer     = AudioBuffer;
    Result.BufferSize = AudioBuffer_Size;
    Result.PrevWrite  = Audio_PrevWrite;
    Result.WriteSize  = Audio_WriteSize;
    return Result;
}

internal void
SetVolume( AUDIO_ENTRY * Audio, flo32 TargetVolume, flo32 TargetTime, flo32 Delay ) {
    Audio->TargetVolume       = TargetVolume;
    Audio->TargetVolume_dMax  = fabsf( ( TargetVolume - Audio->Volume ) / ( TargetTime * 60.0f ) );
    Audio->TargetVolume_Delay = Delay;
}

internal void
SetVolume( AUDIO_SYSTEM * AudioSystem, AUDIO_ID AudioID, flo32 TargetVolume, flo32 TargetTime, flo32 Delay ) {
    for( uint32 iAudio = 0; iAudio < AudioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY * Audio = AudioSystem->Audio + iAudio;
        if( Audio->AudioID == AudioID ) {
            SetVolume( Audio, TargetVolume, TargetTime, Delay );
        }
    }
}

internal void
UpdateVolume( AUDIO_SYSTEM * AudioSystem, flo32 dT ) {
    for( uint32 iAudio = 0; iAudio < AudioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY * Entry = AudioSystem->Audio + iAudio;
        
        if( Entry->TargetVolume_Delay > 0.0f ) {
            Entry->TargetVolume_Delay -= dT;
        } else {
            flo32 dVolume = Entry->TargetVolume - Entry->Volume;
            flo32 dMax    = Entry->TargetVolume_dMax;
            dVolume = Clamp( dVolume, -dMax, dMax );
            
            Entry->Volume += dVolume;
        }
    }
}

internal void
UpdateAudio( WIN32_AUDIO * Audio, AUDIO_SYSTEM * AudioSystem ) {
    LPDIRECTSOUNDBUFFER Buffer = Audio->Buffer;
    
    void * RegionA   = 0;
    void * RegionB   = 0;
    DWORD  SizeA     = 0;
    DWORD  SizeB     = 0;
    DWORD  CurrRead  = 0;
    DWORD  CurrWrite = 0;
    
    if( ( Buffer->GetCurrentPosition( &CurrRead, &CurrWrite ) == DS_OK ) &&
       ( CurrWrite != Audio->PrevWrite ) &&
       ( Buffer->Lock( CurrWrite, Audio->WriteSize, &RegionA, &SizeA, &RegionB, &SizeB, 0 ) == DS_OK ) ) {
        uint32 BytesToAdvance = MinValue( ( ( CurrWrite - Audio->PrevWrite ) + Audio->BufferSize ) % Audio->BufferSize, Audio->WriteSize );
        uint32 SamplesToAdvance = BytesToAdvance / ( AUDIO__BYTES_PER_SAMPLE * AUDIO__CHANNEL_COUNT );
        
        memset( AudioSystem->MixBuffer, 0, AudioSystem->MixBuffer_Size );
        // #if DEBUG_BUILD
        // if( !GlobalVar_proFileSystem.ShowProFile ) {
        // #endif // DEBUG_BUILD
        //advanceMusic( musicSystem, SamplesToAdvance );
        AdvanceAudio( AudioSystem, SamplesToAdvance );
        MixAudio    ( AudioSystem, SamplesToAdvance );
        
        // #if DEBUG_BUILD
        // }
        // #endif // DEBUG_BUILD
        memcpy( RegionA, AudioSystem->MixBuffer,         SizeA );
        memcpy( RegionB, AudioSystem->MixBuffer + SizeA, SizeB );
        
        Buffer->Unlock( RegionA, SizeA, RegionB, SizeB );
        Audio->PrevWrite = CurrWrite;
    }
}