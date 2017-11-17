/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_QUALITYI_H
#define PB_QUALITYI_H

#include "pb_returncodes.h"
#include "pb_session.h"
#include "pb_quality.h"
#include "pb_image.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The quality module is responsible for computing quality measurements of the
  * fingerprint image, e.g. image quality, fingerprint area and fingerprint 
  * condition. 
  */

/** Computes quality information about the image, such as image quality,
  * fingerprint area and fingerprint condition. Also sets the quality values
  * in the image for further reference. 
  *
  * @param[in] session is the session object. 
  * @param[in] image is the image to compute quality for. The image shall
  *            be updated with the computed quality values.
  * @param[out] quality is the returned quality measurements. 
  *
  * @return PB_RC_OK if successful, or an error code. 
  */ 
typedef pb_rc_t pb_qualityI_compute_quality_fn_t (pb_session_t* session,
                                                  pb_image_t* image, 
                                                  pb_quality_t** quality);

/** Checks if the user has kept the finger stable between two captures. 
  *
  * @param[in] session is the session object. 
  * @param[in] previous_image is the previously captured image.
  * @param[in] image is the currently captured image.
  * @param[out] stable is the returned state, stable (1) or not (0). 
  *
  * @return PB_RC_OK if successful, or an error code. 
  */ 
typedef pb_rc_t pb_qualityI_finger_stable_fn_t (pb_session_t* session,
                                                const pb_image_t* previous_image,
                                                const pb_image_t* image, 
                                                int* stable);

/** The quality API. */
typedef struct {
    pb_qualityI_compute_quality_fn_t* compute_quality;
    pb_qualityI_finger_stable_fn_t* finger_stable;
} pb_qualityI;

#ifdef __cplusplus
}
#endif

#endif /* PB_QUALITYI_H */
