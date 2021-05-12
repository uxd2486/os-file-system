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

void _fs_init();

int _fs_create( char *filename );

int _fs_delete( char *filename );

int _fs_open( char *filename );

int _fs_close( char *filename );

int _fs_read( char *filename, char *buf );

int _fs_write( char *filename, char *buf, int buf_size );

#endif
/* SP_ASM_SRC */

#endif
