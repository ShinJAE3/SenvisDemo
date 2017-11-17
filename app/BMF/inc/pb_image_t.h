/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_IMAGE_T_H
#define PB_IMAGE_T_H

#ifdef __cplusplus
extern "C" {
#endif

/** A image object holds a fingerprint image. The fingerprint image
  * is an 8-bit grey scale image with a specified resolution,
  * width and height. The image object also offers some basic
  * image transformations to get alternate representations of the image
  * like a scaled down version suitable for presentation in a GUI. 
  */
typedef struct pb_image_st pb_image_t;

#ifdef __cplusplus
}
#endif

#endif /* PM_IMAGE_T_H */
