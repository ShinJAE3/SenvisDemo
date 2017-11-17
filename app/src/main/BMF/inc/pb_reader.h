/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_READER_H
#define PB_READER_H

#include "pb_types.h"
#include "pb_returncodes.h"
#include "pb_image.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Reader capture events. Will be submitted through the callback function for
  * the capturing process. */
typedef enum pb_reader_capture_event_e {
    /** The reader is dirty. */
    PB_READER_IS_DIRTY = 1,
    /** The swipe was too fast. */
    PB_READER_SWIPE_TOO_FAST,
    /** The swipe was too slow. */
    PB_READER_SWIPE_TOO_SLOW,
    /** An image was captured. */
    PB_READER_IMAGE_CAPTURED,
    /** No finger was present on the sensor.
      * @note It's not mandatory to send this event, sending back empty images
      *       (not containing any fingerprint data) is fine too. */
    PB_READER_NO_FINGER_PRESENT
} pb_reader_captureI_event_t;

/** The callback function from the reader. Will be called each time somethings 
  * happens during the capture process. See PB_READER_X for the types of events 
  * that should cause a callback. 
  *
  * @param[in] event_ is the event from the reader, see PB_READER_X. 
  * @param[in] image is the captured image from the reader if the event is 
  *     PB_READER_IMAGE_CAPTURED. For other events this pointer will be null 
  *     (0). The receiver of the callback is responsible of deallocating the
  *     image. 
  * @param[in] context is the context passed to the capture_image function.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_reader_captureI_callback_fn_t (pb_reader_captureI_event_t event_, 
                                                  pb_image_t* image, 
                                                  void* context);

/** A reader object represents a physical fingerprint reader. A 
  * fingerprint reader is used to capture one or several fingerprint
  * images during a session which then typically are used for further
  * fingerprint processing like feature extraction and matching.
  *
  * The reader object itself has no capabilities of actually
  * capturing images from real sensors. The reader object interacts
  * with a sensor module for the details of capturing images and
  * handle low level details of each specific sensor type.
  * It is typically the sensor module that creates the reader object
  * with the specific details for the sensor.
  *
  * During image capture one or more captured images are sent back
  * to the caller using a callback mechanism until the process is
  * either canceled or timed out. During this time the reader is
  * under the exclusive use of the caller. 
  */
typedef struct pb_reader_st pb_reader_t;

/** A function callback for the implementation of the start function, 
  * see pb_reader_capture_start. */ 
typedef pb_rc_t pb_reader_captureI_start_fn_t (pb_reader_t* reader,
                                               uint16_t timeout);

/** A function callback for the implementation of the stop function, 
  * see pb_reader_capture_stop. */ 
typedef pb_rc_t pb_reader_captureI_stop_fn_t (pb_reader_t* reader);

/** A function callback for the implementation of the capture image function, 
  * see pb_reader_capture_image. */ 
typedef pb_rc_t pb_reader_captureI_image_fn_t (pb_reader_t* reader,
                                               pb_reader_captureI_callback_fn_t* callback, 
                                               void* capture_image_context, 
                                               uint16_t timeout);

/** A function callback for the implementation of the cancel function, 
  * see pb_reader_capture_cancel. */ 
typedef pb_rc_t pb_reader_captureI_cancel_fn_t (pb_reader_t* reader);

/** Image capturing process interface. The capture begins by calling the 'start'
  * function to lock the reader from other users and making it ready to capture
  * images. Then 'capture_image' is called to capture an image. If at any time 
  * during the capture the user wants to cancel the capture process, he/she can
  * call 'cancel'. Finally when all the images needed have been captured 'stop' 
  * shall be called to stop the reader and unlock it, making it available for
  * other users.
  *
  * The implementation of this interface is responsible for assuring that the
  * calls are thread safe. The reader object does not supply such functionality.
  */
typedef struct {
    pb_reader_captureI_start_fn_t* start;
    pb_reader_captureI_stop_fn_t* stop;
    pb_reader_captureI_image_fn_t* capture_image;
    pb_reader_captureI_cancel_fn_t* cancel;
} pb_reader_captureI;


/** Allocates a reader object and sets the reference counter to 1.
  * 
  * @param[in] id is the unique identifier for the reader (US-ASCII), max 127 
  *     chars and null terminated. A reader may be disconnected 
  *     from the system and then connected again later on. To be able to track
  *     that reader it needs to have a unique id that remains the same even if
  *     the reader is disconnected and then reconnected. The sensor module is 
  *     responsible of retaining the id for each reader. Providing null means 
  *     that a unique id is not available.
  * @param[in] name is the name of the reader (US-ASCII), max 127 chars and 
  *     null terminated.
  * @param[in] is_swipe tells if this reader is a swipe (1) or not (0).
  * @param[in] width is the width of the images captured by the reader, 0 means 
  *     undefined value.
  * @param[in] height is the height of the images captured by the reader, 0 
  *     means undefined value.
  * @param[in] horizontal_resolution is the horizontal resolution of the images 
  *     captured by the reader, 0 means undefined value.
  * @param[in] vertical_resolution is the vertical resolution of the images 
  *     captured by the reader, 0 means undefined value.
  * @param[in] capturer is an implementation of the capture process interface.
  *     The interface contains functions for capturing an image from 
  *     the reader.
  * @param[in] reader_context is the context that may be used for passing 
  *     special data to the reader in order to retrieve later on by calling 
  *     pb_reader_get_context.
  *
  * @return the allocated reader object if successful, else 0. 
  */
pb_reader_t* 
pb_reader_create (const char* id,
                  const char* name,
                  int is_swipe,
                  uint16_t width,
                  uint16_t height,
                  uint16_t horizontal_resolution,
                  uint16_t vertical_resolution,
                  const pb_reader_captureI* capturer,
                  void* reader_context);

/** Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_reader_t*
pb_reader_retain (pb_reader_t* reader);

/** Decreases the reference counter for the object by 1. If the reference 
  * counter reaches 0 then the object will also be deleted. Deleting a null
  * pointer has no effect. */
void
pb_reader_delete (pb_reader_t* reader);

/** Returns the unique identifier for the reader, max 127 chars.
  * Return of an empty string means that a unique id do not exist
  * for this reader. */
const char*
pb_reader_get_id (const pb_reader_t* reader);

/** Returns the name of the reader, max 127 chars. */
const char*
pb_reader_get_name (const pb_reader_t* reader);

/** Tells if the reader is a swipe (1) or not (0). */
int
pb_reader_is_swipe (const pb_reader_t* reader);

/** Returns the width in pixels of the images captured by the reader,
  * 0 means undefined value. */
uint16_t
pb_reader_get_width (const pb_reader_t* reader);

/** Returns the height in pixels of the images captured by the reader,
  * 0 means undefined value. */
uint16_t
pb_reader_get_height (const pb_reader_t* reader);

/** Returns the horizontal resolution in DPI of the images captured 
  * by the reader, 0 means undefined value. */
uint16_t
pb_reader_get_horizontal_resolution (const pb_reader_t* reader);

/** Returns the vertical resolution in DPI of the images captured 
  * by the reader, 0 means undefined value. */
uint16_t
pb_reader_get_vertical_resolution (const pb_reader_t* reader);

/** Returns the image area in mm^2. */
uint32_t
pb_reader_get_image_area (const pb_reader_t* reader);

/** Starts a reader. Assures that the reader is operational and ready to grab 
  * an image. Also acts as a lock of the reader, i.e. another call to start 
  * before a call to stop has been made results in an error code. The lock is 
  * only an advisory lock which means that concurrent use from other processes 
  * must be cooperating.
  *
  * Note: The implementation of the reader capture interface is responsible for
  * assuring that the call is thread safe. The reader object itself does not 
  * supply such functionality.
  *
  * @param[in] reader is the reader to operate on.
  * @param[in] timeout is the timeout, in ms, until the function shall return 
  *     in case the reader is busy.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t
pb_reader_capture_start (pb_reader_t* reader,
                         uint16_t timeout);

/** Stops a reader. After calling this function the reader is
  * no longer ready to grab an image, and a call to 
  * capture_image will fail. Also unlocks the reader to be
  * used by someone else.
  *
  * @param[in] reader is the reader to operate on.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t
pb_reader_capture_stop (pb_reader_t* reader);

/** Tells the reader to start capturing images. The call to this 
  * function will block until cancelled or until it times out. 
  * Updates on the capture will be sent to the specified callback. 
  * The start function must have been called before calling this 
  * function. The normal scenario would be that a user calls 
  * capture_image and then calls cancel_capture from the callback 
  * as soon as an image with good enough quality has been received.
  *
  * @param[in] reader is the reader to operate on.
  * @param[in] callback is a function pointer to the callback 
  *     that will be called whenever something happens in the 
  *     reader.
  * @param[in] context is the context that will be sent along 
  *     with the event and image to the callback.
  * @param[in] timeout is the time in ms until the capture function 
  *     shall return, unless cancelled. 
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t 
pb_reader_capture_image (pb_reader_t* reader,
                         pb_reader_captureI_callback_fn_t* callback, 
                         void* context, 
                         uint16_t timeout);

/** Cancels the capture process. Can e.g. be called within the 
  * callback function. 
  *
  * @param[in] reader is the reader to operate on.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t 
pb_reader_capture_cancel (pb_reader_t* reader);

/** Returns the reader context. */
void*
pb_reader_get_context (const pb_reader_t* reader);

#ifdef __cplusplus
}
#endif

#endif /* PB_READER_H */
