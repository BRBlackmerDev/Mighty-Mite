
typedef HRESULT WINAPI DIRECT_SOUND_CREATE( LPCGUID lpcGuidDevice, LPDIRECTSOUND * ppDS, LPUNKNOWN lpUnkOuter );

#define AUDIO__SAMPLES_PER_SECOND  ( 48000 )
#define AUDIO__BYTES_PER_SAMPLE    ( 2 )
#define AUDIO__CHANNEL_COUNT       ( 2 )
#define AUDIO__BUFFER_SECONDS      ( 2 )
#define AUDIO__WRITE_FRAMES        ( 3 )

struct AUDIO_DATA {
    uint8 * Audio;
    uint32  nSamples;
};

enum AUDIO_ID {
    AudioID_Music_Start,
    
    AudioID_Music_Exercise_Boss01,
    AudioID_Music_Exercise_Stage01,
    
    AudioID_Music_End,
    
    AudioID_Sound_Barbell_Bounce01,
    AudioID_Sound_Barbell_Bounce02,
    AudioID_Sound_Barbell_Hit01,
    AudioID_Sound_Barbell_Hit02,
    AudioID_Sound_Boulder,
    AudioID_Sound_BreakBlock,
    AudioID_Sound_Camper_Drop,
    AudioID_Sound_Camper_PickUp,
    AudioID_Sound_Camper_Save,
    AudioID_Sound_Checkpoint,
    AudioID_Sound_ExerciseBall_Bounce01,
    AudioID_Sound_ExerciseBall_Bounce02,
    AudioID_Sound_ExerciseBall_Hit01,
    AudioID_Sound_ExerciseBall_Hit02,
    AudioID_Sound_ExerciseBall_Hit03,
    AudioID_Sound_ExerciseBall_Hit04,
    AudioID_Sound_ExerciseBall_Hit05,
    AudioID_Sound_ExerciseBall_Hit06,
    AudioID_Sound_ExerciseBall_Hit07,
    AudioID_Sound_Money,
    AudioID_Sound_Player_Bash,
    AudioID_Sound_Player_Dash,
    AudioID_Sound_Popper_Explode01,
    AudioID_Sound_Popper_Explode02,
    
    AudioID_Count,
};

struct AUDIO_ENTRY {
    AUDIO_ID AudioID;
    
    uint8 * Audio;
    uint32 nSamples;
    
    uint32 AtSample;
    flo32  AtSample_T;
    
    flo32 Volume;
    flo32 Rate;
    boo32 IsLooping;
    
    flo32 TargetVolume;
    flo32 TargetVolume_dMax;
    flo32 TargetVolume_Delay;
};

struct ASSET_SYSTEM;
#define AUDIO__MAX_ENTRY_COUNT  ( 256 )
struct AUDIO_SYSTEM {
    ASSET_SYSTEM * AssetSystem;
    
    flo32 MainVolume;
    
    uint32     nAudio;
    AUDIO_ENTRY Audio[ AUDIO__MAX_ENTRY_COUNT ];
    
    uint8 * MixBuffer;
    int32   MixBuffer_Size;
};