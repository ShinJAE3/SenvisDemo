/*****************************************************************************
 *
 * pbbmp.h
 *
 * Description:
 *  This file contains the PB BMP API.
 *
 * Limitations:
 *  API can only handle BMP v3 formatted files
 *  API can only read non-compressed BMP files
 *
 ****************************************************************************/
#ifndef HEADER_PBBMP_H
#define HEADER_PBBMP_H

#include "pbtypes.h"
#include "pberrno.h"

#ifdef __cplusplus
extern "C" {
#endif

int pbbmp_free(void* ptr);
int pbbmp_initialize(int version, const void* init_data);
int pbbmp_release(void);
int pbbmp_bir_to_bmpbuffer(const void* bir, size_t bir_size,
                           void** bmp_data, size_t *bmp_size);
int pbbmp_bir_to_bmpfile(const void* bir, size_t bir_size,
                         const char* file_name);
int pbbmp_buffer_to_bmpbuffer(const void* buffer,
                              unsigned int nof_col, unsigned int nof_row,
                              unsigned int resolution,
                              void** bmp_data, size_t *bmp_size);
int pbbmp_buffer_to_bmpfile(const void* buffer,
                            unsigned int nof_col, unsigned int nof_row,
                            unsigned int resolution,
                            const char* file_name);
int pbbmp_bmpbuffer_to_bir(const void* bmp_data, size_t bmp_size,
                           void** bir, size_t* bir_size);
int pbbmp_bmpfile_to_bir(const char* file_name, void** bir, size_t* bir_size);
int pbbmp_bmpbuffer_to_buffer(const void* bmp_data, size_t bmp_size,
                              void** buffer,
                              unsigned int* nof_col, unsigned int* nof_row);
int pbbmp_bmpfile_to_buffer(const char* file_name, void** buffer,
                            unsigned int* nof_col, unsigned int* nof_row);

#ifdef __cplusplus
}  /* End extern "C" */
#endif

#endif  /* HEADER_PBBMP_H */
