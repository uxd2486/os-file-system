/**
**
** @file file.c
**
** @author Utkarsh Dayal CSCI-452 class of 20205
**
** Functions for reading/writing one file. This also serves as an interface
** between filemanager and block.
*/

#define	SP_KERNEL_SRC

#include "common.h"
#include "kmem.h"
#include "file.h"
#include "block.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** Maps a file id to the block id its i-node is stored in
*/
static filemap_t *file_to_block;

/*
** Number of files
*/
static int file_count;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/**
** Name:  get_block_id
**
** Given the file id, returns the block id of the file i-node from the map.
**
** @param file_id   The id of the file
**
** @return The block id 
**
*/
int get_block_id( int file_id ){
    
    int block_id = -1;
    for ( int i = 0; i < file_count; i++ ){
        if ( file_to_block[i].file_id == file_id ){
	    block_id = file_to_block[i].block_id;
	    break;
	}
    }
    return block_id;
}

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  _fl_init
**
** Initializes global variables and calls the block init function
**
*/
void _fl_init(){
    // Initilize the globals
    file_to_block = ( filemap_t * ) _km_page_alloc( 2 );
    file_count = 0;

    // call the block init
    _blk_init();
}

/**
** Name:  _fl_create
**
** Creates a new file, given an id to assign to it
**
** @param id   The id of the file
**
** @return 0 if successful, -1 if not
*/
int _fl_create( int id ){

    // initialize file
    file_t *file = ( file_t * ) _km_slice_alloc( );
    file->id = id;
    file->bytes = 0;
    file->block = _blk_alloc( NUM_BLOCKS );

    // alloc block to store i-node
    int file_block = _blk_alloc( 1 );
    filemap_t *fl_map = _km_slice_alloc();
    fl_map->block_id = file_block;
    fl_map->file_id = id;
    file_to_block[file_count] = *fl_map;
    file_count++;
    _km_slice_free( fl_map );

    // save i-node to disk
    int result = _blk_save_file( file_block, file );
    if ( result < 0 ){
        return E_FAILURE; // something went wrong
    }

    // free memory
    _km_slice_free( file );

    return SUCCESS;
}

/**
** Name:  _fl_open
**
** Opens a file so it can be used 
**
** @param id   The id of the file
**
** @return 0 if successful, -1 if not
*/
file_t *_fl_open( int id ){
    
    // get the block the i-node is in
    int block_id = get_block_id( id );
    if ( block_id == -1 ){
        __cio_printf( "File %d does not have an i-node??\n", id );
        return NULL; // file i-node not found
    }

    file_t *file = NULL;
    // load the file i-node from disk
    int result = _blk_load_file( block_id, file );
    if ( result < 0 || file == NULL){
        return NULL; // something went wrong
    }

    return file;
}

/**
** Name:  _fl_delete
**
** Deletes a file from the disk
**
** @param id   The id of the file
**
** @return 0 if successful, -1 if not
*/
int _fl_delete( int id ){
    
    // get the block that the i-node is in
    int block_id = get_block_id( id );
    if ( block_id == -1 ){
        __cio_printf( "File %d does not have an i-node??\n", id );
        return E_FAILURE; // file i-node not found
    }

    // load the file i-node from disk
    file_t *file = NULL;
    int result = _blk_load_file( block_id, file );
    if ( result < 0 || file == NULL){
        return E_FAILURE; // something went wrong
    }

    // free file blocks
    int start_block = file->block;
    for( int i = 0; i < NUM_BLOCKS; i++ ){
        _blk_free( start_block + i );
    }

    // free the file i-node block
    _blk_free( block_id );

    // remove the file from the file_to_block
    int index = -1;
    for ( int i = 0; i < file_count; i++ ){
        if ( file_to_block[i].block_id == block_id ){
	    index = -1;
	    break;
	}
    }
    if ( index == -1 ){
        __cio_printf( "File %d does not have an i-node??\n", id );
        return E_FAILURE; // something went wrong
    }
    for ( int i = index; i < file_count; i++ ){
        if ( i + 1 < file_count ){
            file_to_block[i].file_id = file_to_block[i+1].file_id;
            file_to_block[i].block_id = file_to_block[i+1].block_id;
	}
    }

    return SUCCESS;
}

/**
** Name:  _fl_close
**
** Saves an open file to the disk
**
** @param file      The i-node of the file
**
** @return 0 if successful, -1 if not
*/
int _fl_close( file_t *file ){
    
    int block_id = get_block_id( file->id );
    if ( block_id == -1 ){
        __cio_printf( "File %d does not have an i-node??\n", file->id );
        return E_FAILURE; // file i-node not found
    }

    // write the file i-node to the disk
    int result = _blk_save_file( block_id, file );
    if ( result < 0 ){
        return E_FAILURE; // something went wrong
    }

    // free the file
    _km_slice_free( file );

    return SUCCESS;
}

/**
** Name:  _fl_read
**
** Reads contents of a file to a buffer
**
** @param file      The i-node of the file
** @param buf       The buffer to be written to
**
** @return Number of characters written to the buffer
*/
int _fl_read( file_t *file, char *buf){
    
    // get the number of blocks to read
    int num_blocks = ( file->bytes / BLOCK_SIZE ) + \
    ( ( file->bytes % BLOCK_SIZE ) != 0 );

    // read file contents from disk
    int result = _blk_load_filecontents( file->block, buf, num_blocks );

    // check result
    if ( result < 0 ){
        return E_FAILURE;
    }

    // nul-terminate buffer so it can be a string
    buf[file->bytes] = '\0';

    // return the number of characters read (includes NULL-terminator)
    return file->bytes + 1;
}

/**
** Name:  _fl_write
**
** Writes contents of the buffer to a file
**
** @param file      The i-node of the file
** @param buf       The buffer containing stuff to write 
** @param buf_size  Number of characters in the buffer
**
** @return 0 if successful, -1 if not
*/
int _fl_write( file_t *file, char *buf, int buf_size ){
    
    // stores the current contents of the file
    int num_pages = ( file->bytes / PAGE_SIZE ) +
        (( file->bytes % PAGE_SIZE ) != 0);
    char *contents = ( char * ) _km_page_alloc( num_pages );

    // get the current contents of the file
    int result = _fl_read( file, contents );
    if ( result < 0 ){
        return E_FAILURE; // could not load file contents
    }

    // append the new stuff
    int num_bytes = file->bytes + buf_size;
    int num_blocks = ( num_bytes / BLOCK_SIZE ) + 
    ( ( num_bytes % BLOCK_SIZE ) != 0);
    char *new_contents = _km_slice_alloc();
    new_contents = __strcpy( new_contents, contents );
    new_contents = __strcat( new_contents, buf );

    // write new contents to the disk
    result = _blk_save_filecontents( file->block, new_contents, 
    num_blocks );

    // check result
    if ( result < 0 ){
        return E_FAILURE;
    }

    // update file i-node
    file->bytes += buf_size;

    // free memory
    for( int i = 0; i < num_pages; i++ ){
        _km_page_free( contents );
	contents += PAGE_SIZE;
    }

    return SUCCESS;
}

