/**
** @file filemanager.h
**
** @author Utkarsh Dayal CSCI-452 class of 20205
**
** Function definitions for handling files and directories
*/

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
*/

/*
** Types
*/

/*
** Used to store file names with
** corresponding file ids
*/
typedef struct str_to_int_s {
    char name[16];
    int id;
} nameMap_t;

/*
** Globals
*/

/*
** Prototypes
*/

/**
** Name:    _fs_init
**
** Initializes the file system
*/
void _fs_init( void );

/**
** Name:    _fs_create
**
** Creates a new file in the file system with the given name
**
** @param filename  The name of the file
**
** @return 0 if successful, -1 if not
*/
int _fs_create( char *filename );

/**
** Name:    _fs_delete
**
** Deletes a file in the file system with the given name
**
** @param filename  The name of the file
**
** @return 0 if successful, -1 if not
*/
int _fs_delete( char *filename );

/**
** Name:    _fs_open
**
** Opens a file in the file system so it can be used
**
** @param filename  The name of the file
**
** @return 0 if successful, -1 if not
*/
int _fs_open( char *filename );

/**
** Name:    _fs_close
**
** Closes a file in the file system after it has been used
**
** @param filename  The name of the file
**
** @return 0 if successful, -1 if not
*/
int _fs_close( char *filename );

/**
** Name:    _fs_read
**
** Reads from a file in the file system with the given name
**
** @param filename  The name of the file
** @param buf       The buffer to be filled with the file contents
**
** @return the number of characters read from the file
*/
int _fs_read( char *filename, char *buf );

/**
** Name:    _fs_write
**
** Writes to a file in the file system with the given name
**
** @param filename  The name of the file
** @param buf       Buffer containing what's to be written
** @param buf_size  Number of characters to be written
**
** @return 0 if successful, -1 if not
*/
int _fs_write( char *filename, char *buf, int buf_size );

#endif
/* SP_ASM_SRC */

#endif
