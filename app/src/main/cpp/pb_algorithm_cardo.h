/*
 * Copyright (c) 2016, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_ALGORITHM_CARDO_H
#define PB_ALGORITHM_CARDO_H

#include "pb_algorithmI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The algorithms below uses the Cardo extractor in combination
  * with the Cardo verifier and Cardo templates.
  *
  * The various alternatives are targeted towards mobile platforms.
  *
  * Choose the algorithm that best matches the size of the sensor to
  * be used.
  */
 
/** Algorithm used for extra small square sensors, approximately 5x5 mm
  * (100x100 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI cardo_square_xs_algorithm;

/** Algorithm used for extra extra small square sensors, approximately 4x3 mm
  * (80x64 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI cardo_square_xxs_algorithm;

/** Algorithm used for small rectangular sensors, approximately 10x3 mm
  * (200x60 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI cardo_rectangular_s_algorithm;


#ifdef __cplusplus
}
#endif

#endif /* PB_ALGORITHM_CARDO_H */
