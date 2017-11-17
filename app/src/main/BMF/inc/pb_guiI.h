/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_GUII_H
#define PB_GUII_H

#include "pb_returncodes.h"
#include "pb_types.h"
#include "pb_image.h"
#include "pb_finger.h"
#include "pb_quality.h"
#include "pb_session.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Important notice: 
  * This gui interface will be changed into several gui interfaces, one for each
  * controller. Most of the functionality here will be moved over to the 
  * BMF_ImageCapturingController in the future. 
  * The BMF_MultiTemplateEnrollmentController has already gotten a specific gui
  * interface. 
  * The reason for these changes is to make it easier to integrate each controller
  * by defining a specific interface for each controller, containing only the 
  * functionality that is applicable for that controller. */

/* Event types. */

#define PB_EVENT_PROMPT                        0x1000
#define PB_EVENT_ALERT                         0x2000
#define PB_EVENT_EXTERNAL                      0xF000

/* Events. */

/** Prompt the user to place finger on sensor. */
#define PB_EVENT_PROMPT_PLACE_FINGER           (PB_EVENT_PROMPT | 0x0001)
/** Prompt the user to lift finger from sensor. */
#define PB_EVENT_PROMPT_LIFT_FINGER            (PB_EVENT_PROMPT | 0x0002)
/** Prompt the user to swipe finger on sensor, only sent for swipe sensors. */
#define PB_EVENT_PROMPT_SWIPE_FINGER           (PB_EVENT_PROMPT | 0x0003)
/** Prompt the user to clean sensor. */
#define PB_EVENT_PROMPT_CLEAN_SENSOR           (PB_EVENT_PROMPT | 0x0004)

/** Prompt the user to insert a smartcard. */
#define PB_EVENT_PROMPT_INSERT_SMARTCARD       (PB_EVENT_PROMPT | 0x0005)
/** Prompt the user to remove the smartcard. */
#define PB_EVENT_PROMPT_REMOVE_SMARTCARD       (PB_EVENT_PROMPT | 0x0006)

/** DEPRECATED: Only used by BMF_MultiTemplateEnrollmentController which now
  * has its own defined events. Will be removed in the future.
  * Prompt the user to move his/her finger. */
#define PB_EVENT_PROMPT_MOVE_FINGER            (PB_EVENT_PROMPT | 0x0007)
/** DEPRECATED: Only used by BMF_MultiTemplateEnrollmentController which now
  * has its own defined events. Will be removed in the future.
  * Prompt the user to center his/her finger on the sensor. */
#define PB_EVENT_PROMPT_CENTER_FINGER          (PB_EVENT_PROMPT | 0x0008)

/** Alert the user that the swipe was too fast. */
#define PB_EVENT_ALERT_SWIPE_TOO_FAST          (PB_EVENT_ALERT | 0x0001)
/** Alert the user that the swipe was too slow. */
#define PB_EVENT_ALERT_SWIPE_TOO_SLOW          (PB_EVENT_ALERT | 0x0002)
/** Alert the user that the swipe was too far to the left. */
#define PB_EVENT_ALERT_SWIPE_TOO_FAR_LEFT      (PB_EVENT_ALERT | 0x0003)
/** Alert the user that the swipe was too far to the right */
#define PB_EVENT_ALERT_SWIPE_TOO_FAR_RIGHT     (PB_EVENT_ALERT | 0x0004)

/** Alert the user that the quality of the image was too bad. */
#define PB_EVENT_ALERT_QUALITY_TOO_BAD         (PB_EVENT_ALERT | 0x0005)
/** Alert the user that the area of the fingerprint within the 
  * image was too small. */
#define PB_EVENT_ALERT_AREA_TOO_SMALL          (PB_EVENT_ALERT | 0x0006)

/** Alert the user that an image was captured, this event will be sent for each
  * captured image, regardless if the quality of the image is suited for further
  * processing or not. */
#define PB_EVENT_ALERT_IMAGE_CAPTURED          (PB_EVENT_ALERT | 0x0007)

/** Alert the user that a template has been extracted. */
#define PB_EVENT_ALERT_TEMPLATE_EXTRACTED      (PB_EVENT_ALERT | 0x000B)
/** Alert the user that a template has been enrolled in the
  * database. */
#define PB_EVENT_ALERT_TEMPLATE_ENROLLED       (PB_EVENT_ALERT | 0x000C)

/** DEPRECATED: Only used by BMF_VerificationController which now
  * has its own defined events. Will be removed in the future.
  * Alert the user that the fingerprint was latent. */
#define PB_EVENT_ALERT_LATENT_FINGER           (PB_EVENT_ALERT | 0x000F)

/** DEPRECATED: Only used by BMF_VerificationController which now
  * has its own defined events. Will be removed in the future.
  * Alert the user that the finger presented for verification was rejected. */
#define PB_EVENT_ALERT_FINGER_REJECTED         (PB_EVENT_ALERT | 0x0011)


/* User interactions. */

/** Ask the user if he/she allows his/her template to be stored in a 
  * database. In some applications the user must explicitly be asked
  * to allow templates to be stored in a database. 
  * Return values (int): 
  *     - 1, allow the template to be stored.
  *     - 0, refuse the template to be stored. */
#define PB_INTERACTION_ALLOW_STORING_OF_TEMPLATE        0x01

/** Ask the user if he/she is satisfied with the quality of the captured
  * image. May be used to allow the user to re-enroll if he/she thinks the
  * quality is too bad. 
  * Return values (int): 
  *     - 1, satisfied with image quality, proceed.
  *     - 0, dissatisfied, try again. */
#define PB_INTERACTION_SATISFIED_WITH_IMAGE_QUALITY     0x02

/** Ask the user if he/she allows the file to be overwritten.
  * Return values (int): 
  *     - 1, ok to overwrite, proceed.
  *     - 0, do not overwrite. */
#define PB_INTERACTION_OVERWRITE_FILE                   0x03

/** Tells the GUI to display to the user that an event has occurred, for 
  * possible events, see PB_EVENT_X. The GUI chooses what will happen for each 
  * event, it could e.g. display a text or image for the user indicating what 
  * happened, light a LED or simply just ignore the event.
  *
  * @param[in] session is the session object.
  * @param[in] event_ is the type of event, see PB_EVENT_X.
  * @param[in] finger is the finger in case of the events _PROMPT_PLACE_FINGER,
  *     _PROMPT_LIFT_FINGER or _PROMPT_SWIPE_FINGER. For other events the finger
  *     will be 0. This is to be able to tell the user e.g. which finger he/she
  *     shall swipe. 
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_guiI_display_event_fn_t (pb_session_t* session,
                                            int event_,
                                            pb_finger_t* finger);

/** Sometimes a controller may need to interact with the user e.g. to get the 
  * user's approval for storing the template in a database. This function 
  * supports such scenarios. 
  *
  * The GUI may choose which interactions types to support, i.e. for which 
  * types that there will actually be an interaction with the user. 
  *
  * @param[in] session is the session object.
  * @param[in] user_interaction_type is the type of user interaction, see
  *     PB_INTERACTION_.
  * @param[out] result is the result of the interaction, defined by the user
  *     interaction type. Note that any type of result is possible, integers,
  *     strings, objects etc. 
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_guiI_interact_with_user_fn_t (pb_session_t* session,
                                                 uint8_t user_interaction,
                                                 void* result);

/** Tells the GUI to display a fingerprint image from the sensor. The GUI 
  * decides if it wants to show the image or just simply ignore it. This 
  * function will most likely be called together with a posting of the event 
  * PB_EVENT_ALERT_IMAGE_CAPTURED. 
  *
  * @param[in] session is the session object.
  * @param[in] image is the image from the sensor to be displayed.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_guiI_display_image_fn_t (pb_session_t* session,
                                            const pb_image_t* image);

/** Tells the GUI to display a fingerprint image from the sensor. The image has
  * been chosen for further processing, e.g. for enrollment or verification. The 
  * GUI decides if it wants to show the image or just simply ignore it. This 
  * function will likely be called together with a call to _display_image_. 
  * Note that the image in the call to _display_image_ may be different from the
  * image in the call to _display_chosen_image_, but more likely they will be 
  * the same image.
  *
  * @param[in] session is the session object.
  * @param[in] image is the image from the sensor to be displayed.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_guiI_display_chosen_image_fn_t (pb_session_t* session,
                                                   const pb_image_t* image);

/** Tells the GUI to display quality information of the 
  * captured image. The GUI may choose to show parts or all
  * of this quality information or simply ignore it. The
  * GUI may choose to display this information as numbers 
  * or e.g. as bars indicating the values. 
  *
  * @param[in] session is the session object.
  * @param[in] quality is the quality information of the 
  *     captured image.
  * @param[in] image_quality_threshold is the threshold that
  *     are used when checking if the image quality is 
  *     acceptable or not. 
  * @param[in] area_threshold is the threshold that are used
  *     when checking if the fingerprint area is acceptable
  *     (large enough) or not.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_guiI_display_quality_fn_t (pb_session_t* session,
                                              const pb_quality_t* quality, 
                                              uint8_t image_quality_threshold, 
                                              uint32_t area_threshold);

/** Tells the GUI to display score information from the 
  * verification match. This information may not be suitable
  * to display in a live application, since an impostor may
  * find it easier to crack the system if this information 
  * is displayed. However, in a test or debug environment
  * this information may be very useful. The GUI may choose to
  * display this information as a number or e.g. as a bar 
  * indicating the score value. 
  *
  * @param[in] session is the session object. 
  * @param[in] score is the score from the verification.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_guiI_display_score_fn_t (pb_session_t* session,
                                            uint16_t score);

/** Tells the GUI to display debug information
  *
  * @param[in] session is the session object. 
  * @param[in] message is the debug message to be displayed.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_guiI_display_debug_message_fn_t (pb_session_t* session,
                                                    const char* message);

/** Tells the GUI to display progress of a process, e.g. an enrollment. 
  * 
  * @param[in] session is the session object. 
  * @param[in] completed is how much of the total progress that has been 
  *            completed.
  * @param[in] total is the total progress that has to be completed to finish
  *            the process.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_guiI_display_progress_fn_t (pb_session_t* session,
                                               uint16_t completed,
                                               uint16_t total);


/** The GUI API. */
typedef struct {
    pb_guiI_display_event_fn_t* display_event;
    pb_guiI_interact_with_user_fn_t* interact_with_user;
    pb_guiI_display_image_fn_t* display_image;
    pb_guiI_display_quality_fn_t* display_quality;
    pb_guiI_display_score_fn_t* display_score;
    pb_guiI_display_chosen_image_fn_t* display_chosen_image;
    pb_guiI_display_debug_message_fn_t* display_debug_message;
    pb_guiI_display_progress_fn_t* display_progress;
} pb_guiI;

#ifdef __cplusplus
}
#endif

#endif /* PB_GUII_H */
