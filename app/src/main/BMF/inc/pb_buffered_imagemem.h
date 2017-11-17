/*
 * Copyright (c) 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_BUFFERED_IMAGEMEM_H
#define PB_BUFFERED_IMAGEMEM_H

#include "pb_image_renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Standard implementation of the pb_imagememI interface that reads
  * image pixels from a memory buffer, whose pointer is sent in as the
  * imagemem handle to the ImageRenderer.
  * 
  * uint8_t pixels[cols*rows];
  * image = pb_image_renderer_create(&pb_buffered_imagemem,
  *                                  pixels,
  *                                  cols,
  *                                  rows,
  *                                  dpi,
  *                                  0,
  *                                  &result);
  */
extern pbif_const pb_imagememI pb_buffered_imagemem;

#ifdef __cplusplus
}
#endif

#endif
