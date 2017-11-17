/*
 * Copyright (c) 2008 - 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_QUALITY_PB_H
#define PB_QUALITY_PB_H

#include "pb_qualityI.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Calculates image quality metrics. The calculations is done using
 * the same algorithm used by the EmbeddedExtractor.
 *
 * As a side effect the image object is also tagged with the
 * calculated values, which then can be used in subsequent steps.
 * See pb_image_get_quality_values().
 */
extern pbif_const pb_qualityI pb_quality;

/**
 * Calculates image quality metrics. Same as pb_quality but operates
 * at 250 dpi. Use this when image is in 250 dpi in which case the image
 * will not be scaled up to 500 dpi first.
 *
 * The speed version saves memory but is not very much faster.
 */
extern pbif_const pb_qualityI pb_quality_speedmem;

#ifdef __cplusplus
}
#endif

#endif /* PB_QUALITY_PB_H */
