/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_PREPROCESSORI_H
#define PB_PREPROCESSORI_H

#include "pb_returncodes.h"
#include "pb_session.h"
#include "pb_image.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Enhances the raw image by improving image quality and reducing
  * noise. 
  *
  * @param[in] session is the session object.
  * @param[in] image is the raw image to be enhanced. 
  * @param[out] enhanced_image is the returned enhanced image. The 
  *     caller is responsible for deleting the image.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_preprocessorI_enhance_image_fn_t (pb_session_t* session,
                                                     const pb_image_t* image, 
                                                     pb_image_t** enhanced_image);


/** A fingerprint image preprocessor implements the preprocessor interface.
  * 
  * The purpose of a preprocessor implementation is to preprocess the raw
  * fingerprint image to enhance image quality and reduce noise. The 
  * enhancement shall focus on things that are specific to a certain sensor. 
  * More generic image enhancement shall be taken care of by the extractor 
  * module. 
  *
  * Typical noise that could be handled by a preprocessor module are things 
  * like:
  *  - Striped patterns, e.g. if certain rows/columns is a bit darker or
  *    brighter than other rows/columns. 
  *  - Histogram stretching, e.g. if images from a certain sensors have 
  *    really poor dynamic range. 
  *  - Removal of dead pixels, e.g. if certain pixels always gets the same
  *    value due to e.g. hardware issues. 
  * 
  * Since different sensors produce different images in terms of image quality 
  * and noise, a preprocessing step might be required for optimal performance. 
  * The typical implementer of these interfaces are sensor vendors, since
  * they have the best knowledge about their sensor characteristics. 
  */
typedef struct {
    pb_preprocessorI_enhance_image_fn_t* enhance_image;
} pb_preprocessorI;

#ifdef __cplusplus
}
#endif

#endif /* PB_PREPROCESSORI_H */
