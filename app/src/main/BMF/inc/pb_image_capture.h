/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_IMAGE_CAPTURE_H
#define PB_IMAGE_CAPTURE_H

#include "pb_sensorI.h"
#include "pb_qualityI.h"
#include "pb_guiI.h"

#include "pb_image.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The image capture controller contains functionality for capturing one or
 * several images from a fingerprint sensor. It also has a function for waiting
 * until the user has lifted his/her finger from the sensor. */

/** The default minimum required quality for capturing. */
#define PB_IMAGE_CAPTURE_DEFAULT_MIN_QUALITY      15
/** The default minimum required area, in mm^2, for capturing. */
#define PB_IMAGE_CAPTURE_DEFAULT_MIN_AREA         (10 * 10)

/** Configuration parameters for the image capture controller. */
typedef struct {
    /** The minimum quality required for a fingerprint image to 
      * be captured, default _DEFAULT_MIN_QUALITY. */
    uint8_t minimum_quality;
    /** The minimum area, in mm^2, required for a fingerprint image to 
      * be captured, default _DEFAULT_MIN_AREA. */
    uint32_t minimum_area;
    /** Tells if the image has to be stable (1) or not (0), in
      * order to be captured, default 0. A stable image is an image that 
      * is similar to the previous image from the reader.*/
    int require_stable_image;
    /** Tells if the user shall determine if the quality of the
      * image is good enough, default 0. May use this to get the
      * user's opinion when enrolling fingers. */
    int user_confirmation_on_image_quality;
} pb_image_capture_config_t;

/** Default configuration parameters. */
static const pb_image_capture_config_t pb_image_capture_default_config = {
        PB_IMAGE_CAPTURE_DEFAULT_MIN_QUALITY, 
        PB_IMAGE_CAPTURE_DEFAULT_MIN_AREA, 0, 0};

/** Captures x images from a fingerprint sensor. The config parameter will
  * set the acceptable quality and area values for the captured image as well
  * as if the image has to be stable.
  *
  * @param[in] session is the session object. 
  * @param[in] reader is the reader to capture images from.
  * @param[in] finger is the finger being captured.
  * @param[in] quality is the quality module.
  * @param[in] gui is the GUI module.
  * @param[in] nbr_of_images is the number of images to capture.
  * @param[in] timeout is the time in ms until the capture function 
  *     shall return, unless cancelled.
  * @param[in] config is the config parameters for the capture. 
  * @param[out] images is the returned captured images. Some images
  *     may be null (0) if enough acceptable images was not 
  *     captured. 
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t
pb_capture_image (pb_session_t* session,
                  pb_reader_t* reader, 
                  pb_finger_t* finger,
                  const pb_qualityI* quality, 
                  const pb_guiI* gui,
                  uint8_t nbr_of_images,
                  uint16_t timeout,
                  const pb_image_capture_config_t* config,
                  pb_image_t* images[]);

/** Captures x raw images from a fingerprint sensor, without requirements on
  * image quality.
  *
  * @param[in] session is the session object. 
  * @param[in] reader is the reader to capture images from.
  * @param[in] finger is the finger being captured.
  * @param[in] quality is the quality module.
  * @param[in] gui is the GUI module.
  * @param[in] nbr_of_images is the number of images to capture.
  * @param[out] images is the returned captured images. Some images
  *     may be null (0) if enough acceptable images was not 
  *     captured. 
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t
pb_capture_raw_image (pb_session_t* session,
                      pb_reader_t* reader, 
                      pb_finger_t* finger,
                      const pb_qualityI* quality, 
                      const pb_guiI* gui,
                      uint8_t nbr_of_images,
                      pb_image_t* images[]);

/** Waits until there is no finger present on the sensor.
  *
  * @param[in] session is the session object. 
  * @param[in] reader is the reader to capture images from.
  * @param[in] quality is the quality module.
  * @param[in] gui is the GUI module. May be set to 0 if no
  *     notifications shall be sent to a GUI while waiting for 
  *     the user to lift his/her finger.
  * @param[in] timeout is the time (in ms) until the function 
  *     returns, whether or not a finger is still present.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t
pb_wait_for_finger_absent (pb_session_t* session,
                           pb_reader_t* reader, 
                           const pb_qualityI* quality, 
                           const pb_guiI* gui,
                           uint16_t timeout);

/** Waits until there is a finger present on the sensor.
  *
  * @param[in] session is the session object. 
  * @param[in] reader is the reader to capture images from.
  * @param[in] quality is the quality module.
  * @param[in] gui is the GUI module. May be set to 0 if no
  *     notifications shall be sent to a GUI while waiting for 
  *     the user to place his/her finger on the sensor.
  * @param[in] timeout is the time (in ms) until the function 
  *     returns, whether or not a finger was presented.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t
pb_wait_for_finger_present (pb_session_t* session,
                            pb_reader_t* reader, 
                            const pb_qualityI* quality, 
                            const pb_guiI* gui,
                            uint16_t timeout);

/** Cancels a capture operation. 
  *
  * @param[in] session is the session object. 
  * @param[in] reader is the reader to capture images from.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t
pb_cancel_capture (pb_session_t* session,
                   pb_reader_t* reader);

#ifdef __cplusplus
}
#endif

#endif /* PB_IMAGE_CAPTURE_H */
