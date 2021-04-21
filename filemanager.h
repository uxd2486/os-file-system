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
typedef struct dir_node {
    int id,
    char *name,
    int parent_id,
    int *files,
    int num_files,
    Directory *children,
    int num_children
} Directory;

/*
** Globals
*/
Directory *directory_tree;

/*
** Prototypes
*/

int create_file( char *filename, int current_dir );

int delete_file( char *filename, int current_dir );

int create_directory( char *dir_name, int current_dir );

int delete_directory( char *dir_name, int current_dir );

int change_directory( char *dir_name, int current_dir );

#endif
/* SP_ASM_SRC */

#endif
