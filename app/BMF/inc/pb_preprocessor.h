/*
 * Copyright (c) 2008 - 2016, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_PREPROCESSOR_H
#define PB_PREPROCESSOR_H

#include "pb_preprocessorI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Generic preprocessor that reduces noise and enhances the ridges of 
  * the fingerprint for all sensor types, which should improve the 
  * biometric performance. 
  *
  * Notes:
  *
  *   Some images may not be suitable for this preprocessor, e.g. if 
  *   an enhance has already been made by another preprocessing filter or
  *   if the images contains extreme types of noise that is not seen in 
  *   "normal images". 
  *   
  *   The resolution and size of the preprocessed images is always the
  *   same as the unprocessed image. 
  *
  *   Not all resolutions are supported. The currently supported resolutions are:
  *    333-393 dpi (see BMF_PreprocessorModule/README.txt for details) 
  *    400-440 dpi (see BMF_PreprocessorModule/README.txt for details)
  *    490-510 dpi (see BMF_PreprocessorModule/README.txt for details)
  */ 
extern pbif_const pb_preprocessorI pb_preprocessor;

#ifdef __cplusplus
}
#endif

#endif /* PB_PREPROCESSOR_H */
