/**
**
** @file filemanager.h
**
** @author Utkarsh Dayal CSCI-452 class of 20205
**
** Functions for handling files and directories
*/

#define	SP_KERNEL_SRC

#include "common.h"
#include "filemanager.h"
#include "ulib.h"
#include "file.h"

/*
** PRIVATE DEFINITIONS
*/
#define ROOT_ID 0
#define PARENT_DIR_STRING ".."

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/
static int file_id_assigner;
static Directory *directory_tree;
static StrIntMap *file_name_map;
static file_count;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/
Directory *get_dir_recursive( int dir_id, Directory *cur_dir ){
    if ( cur_dir->id == dir_id ) {
        return cur_dir;
    }
    Directory *result = NULL;
    for ( int i = 0; i < cur_dir->num_children; i++ ){
        result = get_dir_recursive( dir_id, &cur_dir[i] );
	if ( result != NULL ){
            break;
	}
    }
    return result;
}

Directory get_dir( int dir_id ){
    return get_dir_recursive( dir_id, directory_tree );    
}

char *get_file_name( int file_id ){
    for ( int i = 0; i < file_count; i++ ){
        if ( file_name_map[i].id == file_id ){
            return file_name_map.name;
	}
    }
    return NULL;
}

/*
** PUBLIC FUNCTIONS
*/


void init_filesystem(){
    
    Directory *root; // TODO malloc properly

    if ( root == NULL ){
        // TODO print an error here
    }

    // assign stuff to the root. TODO malloc files and children
    root->id = ROOT_ID;
    root->name = "home";
    root->parent_id = ROOT_ID; // should never be used anyway
    root->num_files = 0;
    root->files = NULL;
    root->num_children = 0;
    root->children = NULL;

    // put in the tree
    directory_tree = root;

    // start at 0
    file_id_assigner = 0;
}

int create_file( char *filename, int current_dir ){
    
    // get working directory
    Directory *dir = get_dir( current_dir );
    if ( dir == NULL ){
        return ERROR; // maybe fatal error?
    }

    // check if file already exists
    for ( int i = 0; i < dir->num_files; i++ ){
        char *name = get_file_name( dir->files[i] );
	if ( strcmp( name, filename ) == 0 ){ 
            return ERROR;
	}
    }

    // make file in storage first
    int file_id = create_file( file_id_assigner );
    file_id_assigner++;
    // TODO check return

    // add file to filename list
    StrIntMap new_map = { filename, id };
    file_name_map[file_count] = new_map;
    file_count++;

    //add file to directory
    dir->files[dir->num_files] = file_id;
    num_files++;

    return SUCCESS;
}


/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
