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

struct str_to_int {
    char name[16],
    int id
} file_map;

/*
** Globals
*/

/*
** Prototypes
*/

void _fs_init();

int create_file( char *filename );

int delete_file( char *filename );

int open_file( char *filename );

int close_file( char *filename );

int read_file( char *filename, char *buf, int buf_size);

int write_file( char *filename, char *buf, int buf_size );

#endif
/* SP_ASM_SRC */

#endif
