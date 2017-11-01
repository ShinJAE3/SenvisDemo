/*
 * Copyright (c) 2016, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_ALGORITHM_NEO_H
#define PB_ALGORITHM_NEO_H

#include "pb_algorithmI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The algorithms below uses the Neo extractor in combination
  * with the Neo verifier and Neo templates.
  *
  * The various alternatives are targeted towards mobile platforms.
  *
  * Choose the algorithm that best matches the size of the sensor to
  * be used. 
  */

/** Algorithm used for medium square sensors, approximately 6.5x6.5 mm 
  * (130x130 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI neo_square_m_algorithm;
/** Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI neo_square_m_speed_algorithm;

/** Algorithm used for small square sensors, approximately 5.8x5.8 mm 
  * (115x115 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI neo_square_s_algorithm;
/** Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI neo_square_s_speed_algorithm;

/** Algorithm used for extra small square sensors, approximately 5x5 mm 
  * (100x100 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI neo_square_xs_algorithm;
/** Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI neo_square_xs_speed_algorithm;

/** Algorithm used for extra extra small square sensors, approximately 4x3 mm 
  * (80x64 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI neo_square_xxs_algorithm;
/** Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI neo_square_xxs_speed_algorithm;

/** Algorithm used for medium rectangular sensors, approximately 10x4 mm 
  * (200x80 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI neo_rectangular_m_algorithm;
/** Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI neo_rectangular_m_speed_algorithm;

/** Algorithm used for small rectangular sensors, approximately 10x3 mm 
  * (200x60 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI neo_rectangular_s_algorithm;
/** Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI neo_rectangular_s_speed_algorithm;

#ifdef __cplusplus
}
#endif

#endif /* PB_ALGORITHM_NEO_H */
