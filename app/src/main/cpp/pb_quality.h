/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_QUALITY_H
#define PB_QUALITY_H

#include "pb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Finger conditions. */

/** The condition of the fingerprint is soaked (very wet). */
#define PB_CONDITION_SOAKED         0
/** The condition of the fingerprint is bone dry (very dry). */
#define PB_CONDITION_BONE_DRY        100

/* Image qualities. */

/** The worst quality. */
#define PB_IMAGE_QUALITY_WORST      0
/** The best quality. */
#define PB_IMAGE_QUALITY_BEST       100


/** A quality object represents quality information regarding a fingerprint 
  * image. The quality measure of a fingerprint image is typically estimated 
  * during image capture and used to determine when a good enough image has 
  * been captured for further processing.
  *
  * The image quality of an image is a measurement of how well the ridges of 
  * the fingerprint has been captured by the sensor. A high image quality value 
  * means that the ridges are clear and easily distinguished, while a low image 
  * quality value means that the ridges are hard to separate from the 
  * background noise.
  */
typedef struct pb_quality_st pb_quality_t;


/** Allocates a quality object and sets the reference counter to 1.
  * 
  * @param[in] image_quality is the fingerprint image quality,
  *     i.e. how well the fingerprint has been captured in
  *     the image, ranging from PB_IMAGE_QUALITY_WORST to
  *     PB_IMAGE_QUALITY_BEST. 
  * @param[in] area is the area of the fingerprint, in mm^2. 0
  *     means that no fingerprint is present in the image.
  * @param[in] condition is the condition of the fingerprint, 
  *     ranging from PB_CONDITION_SOAKED to 
  *     PB_CONDITION_BONE_DRY. 
  *
  * @return the allocated quality object if successful, else 0. 
  */
pb_quality_t*
pb_quality_create (uint8_t image_quality, 
                   uint16_t area, 
                   uint8_t condition);

/** Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_quality_t*
pb_quality_retain (pb_quality_t* quality);

/** Decreases the reference counter for the object by 1. If the reference 
  * counter reaches 0 then the object will also be deleted. Deleting a null
  * pointer has no effect. */
void
pb_quality_delete (pb_quality_t* quality);

/** Creates a copy of the quality object. */
pb_quality_t* 
pb_quality_copy (const pb_quality_t* quality);

/** Returns the image quality. */
uint8_t
pb_quality_get_image_quality (const pb_quality_t* quality);

/** Returns the area of the fingerprint, in mm^2. 0 means
  * that no fingerprint is present in the image. */
uint16_t
pb_quality_get_area (const pb_quality_t* quality);

/** Returns the finger condition. */
uint8_t
pb_quality_get_condition (const pb_quality_t* quality);

#ifdef __cplusplus
}
#endif

#endif /* PB_QUALITY_H */
