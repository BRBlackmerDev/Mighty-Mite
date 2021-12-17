
struct FILE_DATA {
    uint8 * contents;
    uint32  size;
};

typedef FILE_DATA ( READ_FILE )( MEMORY * memory, char * SaveDir, char * FileName, char * extension );
typedef boo32     ( WRITE_FILE )( char * SaveDir, char * FileName, char * extension, void * data, uint32 size );
typedef boo32     ( DOES_FILE_EXIST )( char * SaveDir, char * FileName, char * extension );
typedef boo32     ( DELETE_FILE )( char * SaveDir, char * FileName, char * extension );