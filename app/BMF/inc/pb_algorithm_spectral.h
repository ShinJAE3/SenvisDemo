/*
 * Copyright (c) 2008 - 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_ALGORITHM_SPECTRAL_H
#define PB_ALGORITHM_SPECTRAL_H

#include "pb_algorithmI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** All algorithms below uses the spectral extractor and verifier. 
  *
  * Choose the algorithm that best matches the size of the sensor to be used. 
  */

/** Algorithm used for extra large square sensors, approximately 10x10 mm 
  * (200x200 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI spectral_square_xl_algorithm;
/* Same as above, but with a slice extractor. */
extern pbif_const pb_algorithmI spectral_square_xl_slice_algorithm;

/** Algorithm used for large square sensors, approximately 8x8 mm 
  * (160x160 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI spectral_square_l_algorithm;
/* Same as above, but with a slice extractor. */
extern pbif_const pb_algorithmI spectral_square_l_slice_algorithm;

/** Algorithm used for medium square sensors, approximately 6.5x6.5 mm 
  * (130x130 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI spectral_square_m_algorithm;

/** Algorithm used for small square sensors, approximately 5.8x5.8 mm 
  * (115x115 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI spectral_square_s_algorithm;


/** Algorithm used for full size swipe sensors. */
extern pbif_const pb_algorithmI spectral_swipe_algorithm;
/* Same as above, but with a slice extractor. */
extern pbif_const pb_algorithmI spectral_swipe_slice_algorithm;

#ifdef __cplusplus
}
#endif

#endif /* PB_ALGORITHM_SPECTRAL_H */
