/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_EXTRACTORI_H
#define PB_EXTRACTORI_H

#include "pb_returncodes.h"
#include "pb_session.h"
#include "pb_image.h"
#include "pb_finger.h"
#include "pb_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Extracts a template from the image, if the specified template
  * type is supported by the extractor.
  *
  * @param[in] session is the session object.
  * @param[in] image is the image to extract information from
  *     into a template.
  * @param[in] finger is the finger associated with the image.
  * @param[in] template_type is the template type to be extracted.
  *     If that type is not supported by the extractor, 
  *     PB_RC_NOT_SUPPORTED is returned.
  * @param[out] template_ is the returned extracted template. The 
  *     caller is responsible for deleting the template.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */

typedef pb_rc_t pb_extractorI_extract_template_fn_t (pb_session_t* session,
                                                     const pb_image_t* image, 
                                                     const pb_finger_t* finger,
                                                     pb_template_type_t template_type,
                                                     pb_template_t** template_);


/** Returns the preferred image dimensions. This function should be used
  * in cases where an image needs to be rescaled outside the extractor,
  * external scaling is however not required as the extractor should scale
  * the image internally when required.
  *
  * In some cases the preferred image dimensions may simply be the corresponding
  * native resolution of the extractor while in other cases the image itself
  * may be enlarged or adjusted to an even block size.
  *
  * The preferred image should be scaled to the specified rows/cols while
  * setting the dpi as specified, e.g. using:
  *
  *   pb_image_scale_size_res(image, irows, icols, ivres, ihres);
  *
  * This will ensure the image is not unnecessary scaled internally as well.
  *
  * The intentions of this function is to not actually change spatial size
  * of the image but rather adjust the image dpi, hence any change in rows/cols
  * is coupled to a scale in the new dpi which means that basically the same image
  * is still represented.
  *
  * A simple dummy implementation would simply return the input parameters. 
  *
  * @param[in] session is the session object.
  * @param[in] vres is the original image vertical resolution (dpi).
  * @param[in] hres is the original image horizontal resolution.
  * @param[in] rows is the original image number of rows.
  * @param[in] cols is the original image number of cols.
  * @param[out] ivres is the preferred image vertical resolution (dpi).
  * @param[out] ihres is the preferred image horizontal resolution.
  * @param[out] irows is the preferred image number of rows.
  * @param[out] icols is the preferred image number of cols.
  */
typedef void pb_extractorI_pref_idim_fn_t (pb_session_t* session,
                                           uint16_t vres, uint16_t hres, uint16_t rows, uint16_t cols,
                                           uint16_t* ivres, uint16_t* ihres, uint16_t* irows, uint16_t* icols);

/** A fingerprint feature extractor implements the extractor interface.
  * 
  * The purpose of an extractor implementation is to create a fingerprint
  * template from a fingerprint image.
  * 
  * Different implementations may support different kinds of templates.
  * An application needs to be configured to use the extractor for the
  * intended purpose. Some implementations may be targeted to embedded
  * systems while others are more resource demanding.
  * 
  * Some extractors may offer much more customization parameters
  * offered by the extractor interface. Such parameters can be passed
  * via the session object.
  */
typedef struct {
    pb_extractorI_extract_template_fn_t* extract_template;
    pb_extractorI_pref_idim_fn_t*        preferred_idim;
} pb_extractorI;

#ifdef __cplusplus
}
#endif

#endif /* PB_EXTRACTORI_H */
