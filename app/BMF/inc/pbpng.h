/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

/*****************************************************************************
 *
 * pbpng.h
 *
 * Description:
 *  This file contains the PB PNG API as specified in MAG/SPC/0041/PB.
 *
 ****************************************************************************/
#ifndef HEADER_PBPNG_H
#define HEADER_PBPNG_H

#include "pbtypes.h"
#include "pberrno.h"
#include "pbbir.h" /* PB_BIR_ENCODE_BMxxx */

#ifdef __cplusplus
extern "C" {
#endif

int pbpng_free(void* ptr);
int pbpng_initialize(int version, const void* init_data);
int pbpng_release(void);
int pbpng_bir_to_pngfile(const void* bir, size_t bir_size,
                         const char* file_name);
int pbpng_buffer_to_pngfile(const void* buffer,
                            unsigned int nof_col, unsigned int nof_row,
                            unsigned int encoding,
                            const char* file_name);
int pbpng_pngfile_to_bir(const char* file_name, void** bir, size_t* bir_size);
int pbpng_pngfile_to_buffer(const char* file_name,
                            unsigned int encoding, void** buffer,
                            unsigned int* nof_col, unsigned int* nof_row);

#ifdef __cplusplus
}  /* End extern "C" */
#endif

#endif  /* HEADER_PBPNG_H */
