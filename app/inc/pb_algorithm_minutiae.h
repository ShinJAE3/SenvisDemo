/*
 * Copyright (c) 2008 - 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_ALGORITHM_MINUTIAE_H
#define PB_ALGORITHM_MINUTIAE_H

#include "pb_algorithmI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** All algorithms below uses minutiae templates in ISO Compactcard standard format.
  *
  * Choose the algorithm that best matches the size of the sensor to be used. 
  */

/** Algorithm used for extra large square sensors, approximately 10x10 mm 
  * (200x200 pixels @ 500 dpi). Also used for full size sensors. */
extern pbif_const pb_algorithmI minutiae_square_xl_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI minutiae_square_xl_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI minutiae_square_xl_speed_mem_algorithm;

/** Algorithm used for large square sensors, approximately 8x8 mm 
  * (160x160 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI minutiae_square_l_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI minutiae_square_l_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI minutiae_square_l_speed_mem_algorithm;

/** Algorithm used for full size swipe sensors. */
extern pbif_const pb_algorithmI minutiae_swipe_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI minutiae_swipe_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI minutiae_swipe_speed_mem_algorithm;

#ifdef __cplusplus
}
#endif

#endif /* PB_ALGORITHM_MINUTIAE_H */
