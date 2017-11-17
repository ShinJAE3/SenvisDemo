/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_SLICE_QUALITY_PB_H
#define PB_SLICE_QUALITY_PB_H

#include "pb_qualityI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The slice quality module estimates image quality by by iterating
 * over the input image in slices. The basic functionality is the
 * same as for the regular BMF QualityModule.
 *
 * Quality for slice is calculated just as for a full image and at the
 * end each individual value is combined into result corresponding to
 * (but not equal to) the full image quality value.
 * 
 * The key implementation difference is that when used with an image
 * renderer backed pb_image_t as input the entire image will never
 * be rendered at any one time. Hence it is most suitable in combination
 * also with other slice modules.
 *
 * As a side effect the image object is also tagged with the
 * calculated values, which then can be used in subsequent steps. See
 * pb_image_get_quality_values().
 */
extern pbif_const pb_qualityI pb_slice_quality;

/** Calculates image quality metrics. Same as pb_slice_quality but operates
 * at 250 dpi. Use this when image is in 250 dpi in which case the image
 * will not be scaled up to 500 dpi first.
 *
 */
extern pbif_const pb_qualityI pb_slice_quality_speedmem;


#ifdef __cplusplus
}
#endif

#endif /* PB_SLICE_QUALITY_PB_H */
