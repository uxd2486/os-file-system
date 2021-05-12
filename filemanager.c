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
#include "kmem.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/
static int file_id_assigner;
static nameMap_t *map;
static map_count;
static file_t **open_files;
static int open_files_count;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/
char *get_file_name( int file_id ){
    for ( int i = 0; i < map_count; i++ ){
        if ( map[i].id == file_id ){
            return map.name;
	}
    }
    return NULL;
}

int get_file_id( char *file_name ){
    for ( int i = 0; i < map_count; i++ ){
        if ( strcmp(map[i].name, file_name) == 0 ){
            return map.id;
	}
    }
    return -1;
}

/*
** PUBLIC FUNCTIONS
*/


void _fs_init(){
    // start at 0
    file_id_assigner = 0;

    // allocate map of names to file ids
    map = ( nameMap_t * ) _km_page_alloc( 2 );
    map_count = 0;

    // allocate list of open file i-nodes
    open_files = ( (file_t *) * ) _km_page_alloc( 2 );
    open_files_count = 0;

    // call the file init
    _fl_init();
}

int _fs_create( char *filename ){

    // check if name is correct length
    if ( strlen( filename ) >= 16 ){
        return E_FAILURE;
    }
    
    // check if file already exists
    for ( int i = 0; i < map_count; i++ ){
        if ( strcmp( map[i].name, filename ) == 0 ){
            return E_FAILURE;
	}
    }

    // make file in storage first
    int file_id = _fl_create( file_id_assigner );
    if ( file_id < 0 ){
        return E_FAILURE;
    }
    file_id_assigner++;

    // add file to filename list
    nameMap_t *new_map = ( nameMap_t *) _km_slice_alloc( 1 );
    new_map->id = id;
    strcpy( new_map->name, filename );
    map[file_count] = *new_map;
    file_count++;
    _km_slice_free( new_map );

    return SUCCESS;
}

int _fs_delete( char *filename ){
    
    // find the file first
    int id = -1;
    int index = -1;
    for ( int i = 0; i < map_count; i++ ){
        if ( strcmp( map[i].name, filename ) == 0 ){
            id = map[i].id;
	    index = i;
	    break;
	}
    }

    if ( id == -1 || index == -1){
        return E_FAILURE; // file doesn't exist
    }
    
    // delete file from disk
    int result = _fl_delete( id );
    if ( result < 0 ){
        return E_FAILURE; // unable to delete file
    }

    // delete file from the map
    for ( int i = index; i < map_count; i++ ){
        if ( i + 1 < map_count ){
             nameMap_t temp = map[i+1];
	     map[i].id = temp.id;
	     strcpy( map[i].name, temp.name );
	}
    }
    map_count--;

    return SUCCESS;
}

int _fs_open( char *filename ){
    
    // find the file first
    int id = -1;
    for ( int i = 0; i < map_count; i++ ){
        if ( strcmp( map[i].name, filename ) == 0 ){
            id = map[i].id;
	    break;
	}
    }

    if ( id == -1){
        return E_FAILURE; // file doesn't exist
    }

    //check if file is already open
    for ( int i = 0; i < open_files_count; i++ ){
        file_t *file = open_files[i];
	if ( file->id == id ){
	    return E_FAILURE; // file is already open
	}
    }

    // load the file
    file_t *file = _fl_open( id );
    if ( file == NULL ){
        return E_FAILURE;
    }
    
    // Add it to the open files list
    open_files[open_files_count] = file;
    open_files_count++;

    return SUCCESS;
}

int _fs_close( char *filename ){
    
    // find the file first
    int id = -1;
    int index = -1;
    for ( int i = 0; i < map_count; i++ ){
        if ( strcmp( map[i].name, filename ) == 0 ){
            id = map[i].id;
	    index = i;
	    break;
	}
    }

    if ( id == -1 || index == -1){
        return E_FAILURE; // file doesn't exist
    }

    // find the file in the open file list
    file_t *file = NULL;
    for ( int i = 0; i < open_files_count; i++ ){
        if ( open_files[i].id == id ){
            file = &open_files[i];
	}
    }

    if ( file == NULL ){
        return E_FAILURE; // file isn't in the open list
    }

    // close the file
    int result = _fl_close( file );
    if ( result < 0 ){
        return E_FAILURE; // something went wrong
    }

    // remove the file from the open list
    for ( int i = index; i < open_files_count; i++ ){
        if ( i + 1 < open_files_count ){
	    open_files[i] = open_files[i+1];
	}
    }
    open_files_count--;
}

int _fs_read( char *filename, char *buf ){
    
    // get the file id
    int file_id = get_file_id( filename );
    if ( file_id < 0 ){
        return E_FAILURE; // file doesn't exist
    }

    // find the file in the open file list
    File *file = NULL;
    for ( int i = 0; i < open_files_count; i++ ){
        if ( open_files[i].id == id ){
            file = &open_files[i];
	}
    }

    if ( file == NULL ){
        return E_FAILURE; // file isn't in the open list
    }

    int result = _fl_read( file, buf);
    if( result < 0 ){
        return E_FAILURE; //something went wrong
    }

    // return the number of characters read
    return result;
}

int _fs_write( char *filename, char *buf, int buf_size ){
   
    // get the file id
    int file_id = get_file_id( filename );
    if ( file_id < 0 ){
        return E_FAILURE; // file doesn't exist
    }

    // find the file in the open file list
    File *file = NULL;
    for ( int i = 0; i < open_files_count; i++ ){
        if ( open_files[i].id == id ){
            file = &open_files[i];
	}
    }

    if ( file == NULL ){
        return E_FAILURE; // file isn't in the open list
    }

    int result = _fl_write( file, buf, buf_size );
    if( result < 0 ){
        return E_FAILURE; //something went wrong
    }

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
