/*
 * Copyright (c) 2008 - 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_ALGORITHM_HYBRID_H
#define PB_ALGORITHM_HYBRID_H

#include "pb_algorithmI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The algorithms below uses the embedded extractor in combination
  * with the EHM verifier. The large and medium square algorithms uses
  * plain EHM templates while the three smaller versions uses EHM HR
  * templates.
  *
  * The various alternatives are targeted towards different mobile and
  * embedded platforms with varying amount of RAM and CPU power.
  *
  * Choose the algorithm that best matches the size of the sensor to
  * be used.
  *
  * The session parameter pb.algorithm.speed can be used to reduce the
  * impostor verification latency. Use values between 0 and 32. The
  * exact behaviour of each speed value depends on the algorithm.
  */

/** Algorithm used for extra large square sensors, approximately 10x10 mm
  * (200x200 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI hybrid_square_xl_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_square_xl_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_square_xl_speed_mem_algorithm;
/* Same as above, but with slice extractor. */
extern pbif_const pb_algorithmI hybrid_square_xl_slice_algorithm;
/* Same as above, but with slice extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_square_xl_speed_slice_algorithm;
/* Same as above, but with slice extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_square_xl_speed_mem_slice_algorithm;

/** Algorithm used for large square sensors, approximately 8x8 mm
  * (160x160 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI hybrid_square_l_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_square_l_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_square_l_speed_mem_algorithm;
/* Same as above, but with slice extractor. */
extern pbif_const pb_algorithmI hybrid_square_l_slice_algorithm;
/* Same as above, but with slice extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_square_l_speed_slice_algorithm;
/* Same as above, but with slice extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_square_l_speed_mem_slice_algorithm;

/** Algorithm used for medium square sensors, approximately 6.5x6.5 mm
  * (130x130 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI hybrid_square_m_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_square_m_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_square_m_speed_mem_algorithm;
/* Same as above, but with using EHM instead of EHM-HR templates. */
extern pbif_const pb_algorithmI hybrid_square_m_non_hr_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_square_m_non_hr_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_square_m_non_hr_speed_mem_algorithm;
/* Same as above, but optimized for limited platforms. */
extern pbif_const pb_algorithmI hybrid_square_m_pro_algorithm;

/** Algorithm used for small square sensors, approximately 5.8x5.8 mm
  * (115x115 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI hybrid_square_s_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_square_s_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_square_s_speed_mem_algorithm;
/* Same as above, but with using EHM instead of EHM-HR templates. */
extern pbif_const pb_algorithmI hybrid_square_s_non_hr_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_square_s_non_hr_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_square_s_non_hr_speed_mem_algorithm;
/* Same as above, but optimized for limited platforms. */
extern pbif_const pb_algorithmI hybrid_square_s_pro_algorithm;

/** Algorithm used for extra small square sensors, approximately 5x5 mm
  * (100x100 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI hybrid_square_xs_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_square_xs_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_square_xs_speed_mem_algorithm;
/* Same as above, but with using EHM instead of EHM-HR templates. */
extern pbif_const pb_algorithmI hybrid_square_xs_non_hr_algorithm;

/** Algorithm used for extra extra small square sensors, approximately 4x3 mm
  * (80x64 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI hybrid_square_xxs_non_hr_algorithm;

/** Algorithm used for medium rectangular sensors, approximately 10x4 mm
  * (200x80 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI hybrid_rectangular_m_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_rectangular_m_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_rectangular_m_speed_mem_algorithm;
/* Same as above, but with using EHM instead of EHM-HR templates. */
extern pbif_const pb_algorithmI hybrid_rectangular_m_non_hr_algorithm;
/* Same as above, but with extractor 'optimized for speed '. */
extern pbif_const pb_algorithmI hybrid_rectangular_m_non_hr_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_rectangular_m_non_hr_speed_mem_algorithm;
/* Same as above, but optimized for limited platforms. */
extern pbif_const pb_algorithmI hybrid_rectangular_m_pro_algorithm;

/** Algorithm used for small rectangular sensors, approximately 10x3 mm
  * (200x60 pixels @ 500 dpi). */
extern pbif_const pb_algorithmI hybrid_rectangular_s_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_rectangular_s_speed_mem_algorithm;
/* Same as above, but with using EHM instead of EHM-HR templates. */
extern pbif_const pb_algorithmI hybrid_rectangular_s_non_hr_speed_mem_algorithm;
/* Same as above, but optimized for limited platforms. */
extern pbif_const pb_algorithmI hybrid_rectangular_s_pro_algorithm;

/** Algorithm for tuning only */
extern pbif_const pb_algorithmI hybrid_rectangular_s_non_hr_algorithm;

/** Algorithm used for full size swipe sensors. */
extern pbif_const pb_algorithmI hybrid_swipe_algorithm;
/* Same as above, but with extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_swipe_speed_algorithm;
/* Same as above, but with extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_swipe_speed_mem_algorithm;
/* Same as above, but with slice extractor. */
extern pbif_const pb_algorithmI hybrid_swipe_slice_algorithm;
/* Same as above, but with slice extractor 'optimized for speed'. */
extern pbif_const pb_algorithmI hybrid_swipe_speed_slice_algorithm;
/* Same as above, but with slice extractor 'optimized for speed and memory'. */
extern pbif_const pb_algorithmI hybrid_swipe_speed_mem_slice_algorithm;

#ifdef __cplusplus
}
#endif

#endif /* PB_ALGORITHM_HYBRID_H */
