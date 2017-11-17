/*
 * Copyright (c) 2008 - 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_MULTITEMPLATE_ENROLLMENT_H
#define PB_MULTITEMPLATE_ENROLLMENT_H

#include "pb_algorithm.h"
#include "pb_sensorI.h"
#include "pb_qualityI.h"
#include "pb_databaseI.h"
#include "pb_guiI.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The multi-template enrollment controller drives the enrollment
 * process by taking several image captures representing the same
 * finger and creates a multi-template representation of it. While
 * the main target for mult-templates is small sensors it can be used
 * for any sensor.
 *
 * The controller can be used in two different ways:
 *
 *  - Fully automated  - see pb_multitemplate_enroll()
 *  - Image driven     - see pb_multitemplate_enroll_create()
 *
 * The fully automated mode requires a full BMF integration with
 * a sensor driver/reader object, database etc. In this mode the
 * controller is just started and will continue to capture image
 * samples until the enrollment process is competed.
 *
 * The image driven mode expects image capture, quality and area
 * screening to be done outside of the controller. Hence it does
 * not require a full BMF integration. In this mode no screening
 * of samples of poor quality or small area is done, this should
 * be handled by the capturing logic.
 *
 * In both modes there is a GUI feedback interface that will
 * receive events and data that should be presented to the user
 * to drive the enrollment. Both visual and textual data exist.
 *
 * In the image driven mode events regarding the image capturing
 * process will not be given (such as place/lift finger and display
 * image quality).
 *
 * A configuration is used to configure the enrollment process,
 * like max number of captures to make. The fully automated mode
 * relies on more parameters than the image driven as these are
 * needed to control the capturing part.
 */

/** The default minimum required quality for enrollment. */
#define PB_MULTITEMPLATE_ENROLLMENT_DEFAULT_MIN_QUALITY             20
/** The default minimum required area, in mm^2, for enrollment. */
#define PB_MULTITEMPLATE_ENROLLMENT_DEFAULT_MIN_AREA                (11 * 11)
/** The default minimum required area for each template, in mm^2. */
#define PB_MULTITEMPLATE_ENROLLMENT_DEFAULT_MIN_AREA_PER_TEMPLATE   (8 * 8)

/** Configuration parameters for the enrollment controller (on MPU). */
typedef struct pb_multitemplate_enrollment_config_st {
    /** The minimum quality required for a fingerprint image to
      * be used for enrollment, default _DEFAULT_MIN_QUALITY. */
    uint8_t minimum_quality;

    /** The minimum area, in mm^2 for the combined area of all of the templates
      * in the multitemplate, required for a fingerprint image to be used for
      * enrollment, default _DEFAULT_MIN_AREA. */
    uint32_t minimum_area;

    /** The minimum area, in mm^2, required for each template, default
      * _DEFAULT_MIN_AREA_PER_TEMPLATE. Note that this value is very dependant
      * on the sensor being used. The default value is set based on a sensor
      * size of 192 x 192 pixels and corresponds to 67% of the total sensor
      * area. If smaller sensors are used, the default value must be replaced
      * with a value of approximately 60-90% of the total sensor area (use a
      * higher percentage for smaller sensors). */
    uint32_t minimum_area_per_template;

    /** Tells if the image has to be stable (1) or not (0), in order to be used
      * for enrollment, default 1. A stable image is an image that does not
      * differ too much from the previously captured image.
      * Note: In the case of image driven enrollment, the caller is responsible
      * for this and no check will be done by the controller. */
    int require_stable_image;

    /** The timeout, in ms, for the image capture, per image. Default 30000. If
      * the enrollment process times out, it is seen as a failure to enroll.
      * Note: Not applicable for image driven enrollment. */
    uint16_t timeout;

    /** The (maximum) number of captures required to complete the enrollment. A
      * capture is an image with a corresponding template that is not too
      * similar (close) to any if the previous captures. Default 20 but shall be
      * highly dependant on the size of the sensor being used. A small sensor
      * shall require more number of captures for the enrollment to complete. */
    uint16_t max_nbr_of_captures;

    /** Tells if the encoded multitemplate shall be locked for further updates
      * when enrollment is completed, i.e. if the ability to enroll additional
      * templates into the multitemplate shall be disabled. Default 1. */
    int lock_template_from_further_updates;

    /** Tells if the enrollment shall prevent that multiple fingers are enrolled
      * in the multitemplate, i.e. that the enrollment shall only include the
      * templates that matched the best matching template, other templates will
      * only be included as unenrolled templates. Default 0.
      * Note: For smaller sensors it is strongly recommended that
      * prevent_enrollment_of_multiple_fingers is set to 0 to allow for a good
      * enrollment, although that multiple fingers may be enrolled! */
    int prevent_enrollment_of_multiple_fingers;

    /** Maximum template size in bytes. When non-zero this parameter
      * enforces a limit on how large the template may be, including
      * dynamic updates. The value is set as an attribute in the
      * template. Default value is 0 - no limit. */
    uint32_t max_template_size;

    /** Lock the enrolled templates from further modification, but otherwise keep
      * the enrolled template open for modification. Compare with 
      * lock_template_from_further_updates which will completely block any further
      * updates. Default value 1. */
    int lock_templates_after_enroll;

    /** The synthetic image to be used for displaying which parts of the finger that
      * has been captured. If set to 0, no synthetic image will be created and
      * the function display_synthetic_image will never be called. Default 0. Set it
      * to 0 if the display_islands function will be used to display the captured parts.
      * An example image can be found in pb_synthetic_image.h.
      * Note: The resolution of the synthetic image should be 500 dpi. */
    pb_image_t* synthetic_image;

    /** Tells if the enrollment controller shall keep duplicate templates instead of
      * rejecting them. Default 0. */
    int keep_duplicate_templates;

    /** The max number of subtemplates with extended data. Allows for having
      * only some subtemplates containing more (extended) data required by
      * more complex verifiers while having numerous templates with less data
      * but then covering more of the finger. Requires that a valid template
      * type has been set for extended_data_type, else defaults to 0.
      * Used for template decoupling to set the "original" number of
      * subtemplates, e.g. when calculating thresholds. */
    uint8_t max_nbr_of_templates_with_extended_data;

    /** The template data type of the extended data, see
      * max_nbr_of_subtemplates_with_extended_data. Set to PB_TEMPLATE_UNKNOWN
      * if this functionality is not needed (default).
      * Used for template decoupling to know which templates to pass on to the
      * next verifier link.*/
    pb_template_type_t extended_data_type;
} pb_multitemplate_enrollment_config_t;

/** Default MPU configuration parameters.
 *
 * note: always make a copy of these and change individual parameters
 * instead of using agregate initialization as struct may change. */
static const pb_multitemplate_enrollment_config_t pb_multitemplate_enrollment_default_config = {
        PB_MULTITEMPLATE_ENROLLMENT_DEFAULT_MIN_QUALITY,
        PB_MULTITEMPLATE_ENROLLMENT_DEFAULT_MIN_AREA,
        PB_MULTITEMPLATE_ENROLLMENT_DEFAULT_MIN_AREA_PER_TEMPLATE,
        1, 30000, 20, 1, 0, 0, 1, 0, 0, 
        0, PB_TEMPLATE_UNKNOWN};

/** Configuration parameters for the slimmer MCU enrollment controller. */
typedef struct pb_multitemplate_enrollment_mcu_config_st {
    /** The (maximum) number of captures required to complete the enrollment. A
      * capture is an image with a corresponding template that is not too
      * similar (close) to any if the previous captures. Default 20 but shall be
      * highly dependant on the size of the sensor being used. A small sensor
      * shall require more number of captures for the enrollment to complete.
      */
    uint16_t max_nbr_of_captures;

    /** The maximum number of verifications to be done against
      * accepted samples to rejext too similar samples. A higher value
      * is typically better but may take too long time to give a
      * good response time. The default 0 is to test against all
      * accepted captures.
      */
    uint8_t max_verifications;

    /** Tells if the encoded multitemplate shall be locked for further updates
      * when enrollment is completed, i.e. if the ability to enroll additional
      * templates into the multitemplate shall be disabled. Default 1.
      */
    int lock_template_from_further_updates;

    /** Maximum template size in bytes. When non-zero this parameter
      * enforces a limit on how large the template may be, including
      * dynamic updates. The value is set as an attribute in the
      * template. Default value is 0 - no limit.
      */
    uint32_t max_template_size;

} pb_multitemplate_enrollment_mcu_config_t;
    
/** Default MCU configuration parameters.
 *
 * note: always make a copy of these and change individual parameters
 * instead of using agregate initialization as struct may change. */
static const pb_multitemplate_enrollment_mcu_config_t pb_multitemplate_enrollment_mcu_default_config = {
    20, 0, 1, 0
};

/** Callback interface. */

/** Events that are sent to the GUI callback interface. These should
 * be used to instruct the user during enrollment together with other
 * GUI feedback.
 *
 * Events that are only part of the fully automated mode relates to
 * the image capturing process and is commented for each event, the
 * other events can happen in both modes.
 */
#define PB_MTE_EVENT_PROMPT_PLACE_FINGER       PB_EVENT_PROMPT_PLACE_FINGER // capture event
#define PB_MTE_EVENT_PROMPT_LIFT_FINGER        PB_EVENT_PROMPT_LIFT_FINGER  // capture event
/** Prompt the user to move his/her finger more between captures. Will be
  * triggered if the user places his/her finger on the same spot as the last
  * capture during enrollment. */
#define PB_MTE_EVENT_PROMPT_MOVE_FINGER_MORE   PB_EVENT_PROMPT_MOVE_FINGER
/** Prompt the user to move his/her finger less between captures. Will be
  * triggered if the user places his/her finger on very different spots
  * during enrollment. Will also be triggered if the user uses more than
  * one finger during enrollment. */
#define PB_MTE_EVENT_PROMPT_MOVE_FINGER_LESS   PB_EVENT_PROMPT_CENTER_FINGER
#define PB_MTE_EVENT_ALERT_IMAGE_CAPTURED      PB_EVENT_ALERT_IMAGE_CAPTURED // capture event
#define PB_MTE_EVENT_ALERT_TEMPLATE_EXTRACTED  PB_EVENT_ALERT_TEMPLATE_EXTRACTED
#define PB_MTE_EVENT_ALERT_TEMPLATE_ENROLLED   PB_EVENT_ALERT_TEMPLATE_ENROLLED
/** The capture was too similar to one of the previous captures and will not
  * be used. */
#define PB_MTE_EVENT_ALERT_DUPLICATE_TEMPLATE  (PB_EVENT_EXTERNAL | 0x0001)

/** Will be called whenever a certain event has occurred, see PB_MTE_EVENT_X for
  * possible events.
  *
  * @param[in] session is the session object.
  * @param[in] event_ is the type of event, see PB_MTE_EVENT_X.
  * @param[in] finger is the finger in case of the events _PROMPT_PLACE_FINGER,
  *     _PROMPT_LIFT_FINGER. For other events the finger
  *     will be 0. This is to be able to tell the user e.g. which finger he/she
  *     shall place on the sensor.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
typedef pb_rc_t pb_mte_guiI_display_event_fn_t (pb_session_t* session,
                                                int event_,
                                                pb_finger_t* finger);


/** Will be called each time the synthetic image, see synthetic_image in config for more
  * information, has been updated with a new capture and its time to display it to
  * the user. Will not be called if synthetic_image in config is set to 0.
  *
  * @param[in] session is the session object.
  * @param[in] synthetic_image is the synthetic image to be displayed.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
typedef pb_rc_t pb_mte_guiI_display_synthetic_image_fn_t (pb_session_t* session,
                                                          const pb_image_t* synthetic_image);

/** A point. */
typedef struct pb_point_st {
    /** The x-coordinate of the point, in 500 dpi. */
    int16_t x;
    /** The y-coordinate of the point, in 500 dpi. */
    int16_t y;
} pb_point_t;

/** Rectangle flags. */
/** Tells that this is the latest captured template. */
#define PB_MTE_RECTANGLE_FLAG_LATEST    0x01
/** Tells that this is a duplicate of another template and that
  * it will not be included in the final multitemplate. */
#define PB_MTE_RECTANGLE_FLAG_DUPLICATE 0x02

/** A rectangle of a subtemplate. */
typedef struct pb_mte_rectangle_st {
    /** The reference to the subtemplate. Can be seen as the unique id of
      * the rectangle. */
    pb_template_t* subtemplate;
    /** The four corner points of the rectangle. The corner points describes
      * the position of the subtemplate within the island.
      * The corner points will always be sorted starting at the upper left
      * corner and going clockwise, seen from the coordinate system of the
      * template.  */
    pb_point_t corners[4];
    /** The flags for the rectangle, see PB_MTE_RECTANGLE_FLAG_X. */
    uint8_t flag;
} pb_mte_rectangle_t;

/** An island of subtemplates. */
typedef struct pb_mte_island_st {
    /** The rectangles of the subtemplates of the island. */
    pb_mte_rectangle_t* rectangles;
    /** The number of rectangles. */
    uint8_t nbr_of_rectangles;
} pb_mte_island_t;

/** Will be called each time a new capture has been received with information
  * about the positions of each subtemplate within each island of subtemplates.
  * While some of the subtemplates may not be used in the final multitemplate,
  * this function will receive all subtemplates, independent of e.g. if a
  * subtemplate is similar to another subtemplate or not.
  * Some assumptions about the subtemplates and islands can be made:
  *   - The list of islands is sorted with the largest island first.
  *   - Two subtemplates belonging to the same island will always belong to the
  *     same island.
  *   - Two or more islands may merge into one island when a new capture bridging
  *     the islands are found.
  *   - The top (first) subtemplate of each island will remain the same unless
  *     islands are merged. When two or more islands are merged, the top
  *     subtemplate of the largest island will become the top subtemplate of the
  *     new merged island.
  *   - The top subtemplate of each island will always be positioned at (0,0)
  *     with no rotation.
  * The receiver is responsible for deallocating each island using the function
  * pb_mte_delete_islands, unless PB_RC_NOT_SUPPORTED is returned.
  *
  * Note: The positions returned are in 500 dpi, independent of the original
  * resolution of the input images.
  *
  * @param[in] session is the session object.
  * @param[in] islands is the islands to be displayed.
  * @param[in] nbr_of_islands is the number of islands.
  *
  * @return PB_RC_OK if successful, or an error code. If PB_RC_NOT_SUPPORTED is
  *         returned, the multitemplate enrollment controller will deallocate the
  *         islands itself.
  */
typedef pb_rc_t pb_mte_guiI_display_islands_fn_t (pb_session_t* session,
                                                  pb_mte_island_t* islands,
                                                  uint8_t nbr_of_islands);

/** Deallocates the island. */
void
pb_mte_delete_islands (pb_mte_island_t* islands,
                       int nbr_of_islands);

/** Will be called for each capture with the quality information of the capture.
  * Use this information mainly to detect when image quality or fingerprint area
  * is not acceptable and then inform the user how to proceed to get an acceptable
  * image.
  *
  * This function will not be called in image driven mode.
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
typedef pb_rc_t pb_mte_guiI_display_quality_fn_t (pb_session_t* session,
                                                  const pb_quality_t* quality,
                                                  uint8_t image_quality_threshold,
                                                  uint32_t area_threshold);

/** Will be called for each capture to inform about the progress of the
  * enrollment.
  *
  * @param[in] session is the session object.
  * @param[in] completed is how much of the total progress that has been
  *            completed.
  * @param[in] total is the total progress that has to be completed to finish
  *            the process.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
typedef pb_rc_t pb_mte_guiI_display_progress_fn_t (pb_session_t* session,
                                                   uint16_t completed,
                                                   uint16_t total);

/** The callback interface functions. */
typedef struct pb_mte_guiI_st {
    pb_mte_guiI_display_event_fn_t* display_event;
    pb_mte_guiI_display_islands_fn_t* display_islands;
    pb_mte_guiI_display_synthetic_image_fn_t* display_synthetic_image;
    pb_mte_guiI_display_quality_fn_t* display_quality;
    pb_mte_guiI_display_progress_fn_t* display_progress;
} pb_mte_guiI;

/** Enrolls x templates into a multitemplate that is then stored in the
  * database. The function guides the user into moving his/her finger so that x
  * partly overlapping templates can be enrolled. This type of enrollment is
  * primarily for small area sensors with which multiple slightly overlapping
  * templates is preferred to improve biometric performance but may also be
  * used for larger sensors.
  *
  *
  * @param[in] algorithm is the algorithm to use.
  * @param[in] session is the session object.
  * @param[in] finger is the finger to enroll. Will be input to
  *            the database for how to store the template.
  * @param[in] reader is the reader to capture images from.
  * @param[in] quality is the quality module.
  * @param[in] database is the database module.
  * @param[in] gui is an implementation of the gui callback interface.
  * @param[in] nbr_of_templates is the maximum number of templates
  *            accepted into the multitemplate. This value becomes
  *            part of the configuration in the multitemplate and
  *            used in future updates of the template. A higher
  *            value should be used for small sensors while a lower
  *            value may consume less memory and shorter verification
  *            times.
  * @param[in] template_type is the type of template to be enrolled.
  * @param[in] config is the config parameters for the enrollment.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_multitemplate_enroll (pb_algorithm_t* algorithm,
                         pb_session_t* session,
                         pb_finger_t* finger,
                         pb_reader_t* reader,
                         const pb_qualityI* quality,
                         const pb_databaseI* database,
                         const pb_mte_guiI* gui,
                         uint8_t nbr_of_templates,
                         pb_template_type_t template_type,
                         const pb_multitemplate_enrollment_config_t* config);

/** Cancels the multitemplate enrollment process.
  *
  * @param[in] session is the session object.
  * @param[in] reader is the reader to capture images from.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_multitemplate_cancel_enroll (pb_session_t* session,
                                pb_reader_t* reader);

/** The multitemplate enrollment object. */
typedef struct pb_multitemplate_enroll_st pb_multitemplate_enroll_t;

/** Image driven multitemplate enrollment.
  *
  * For embedded systems the image driven version is typically the easiest
  * to integrate as the image capture process is not integrated. Instead
  * images are pushed to the controller as they are captured. The capture
  * logic will however be responsible of area and quality of samples.
  * In this scenario the configuration parameters for capture is
  * not relevant to the controller.
  *
  * There are two main versions of the controller.
  *
  *  MPU - The most advanced and suited for MPU environments with a bit
  *        more resources. The controller supports rich user feedback,
  *        template selection and creates templates that can be
  *        continuously updated.
  *
  *  MCU - A simpler controller using less resources and suited for MCU
  *        environments. Still provides some user feedback and template
  *        selections. Creates templates that allows incremental update
  *        until max capacity is reached.
  *
  * To run an image driven multitemplate enrollment, follow these steps:
  *     1. Create a pb_multitemplate_enroll_t object
  *             pb_multitemplate_enroll_t* mte = pb_multitemplate_enroll_create (...);
  *     2. Continously push images to the mte object until all captures have
  *        been collected.
  *             pb_multitemplate_enroll_run (mte, image, 0, &coverage);
  *        The _run function returns the coverage of a thought area (250x350) of
  *        the merged fingerprint.
  *     3. When all captures have been collected, call pb_multitemplate_finalize_template
  *        to start the process of choosing which templates to store in the
  *        multitemplate. Returns the enrolled multitemplate.
  *     4. Delete the mte object
  *             pb_multitemplate_enroll_delete (mte);
  *
  * When running this type of enrollment, more responsibility is put on the
  * caller. E.g. the caller needs to ensure that the pushed images really
  * contains a fingerprint of a certain quality and area. Further, the caller is
  * responsible for storing the multitemplate.
  */

/** Creates the multitemplate enroll object used for image driven
  * multitemplate enrollment.
  *
  * @param[in] algorithm is the algorithm to use.
  * @param[in] session is the session object.
  * @param[in] finger is the finger to enroll.
  * @param[in] quality is the quality module.
  *            In image driven mode this is not actually used.
  * @param[in] gui is an implementation of the gui callback interface.
  * @param[in] template_type is the type of template to be enrolled.
  * @param[in] config is the config parameters for the enrollment.
  *
  * @return the multitemplate enrollment object.
  */
pb_multitemplate_enroll_t*
pb_multitemplate_enroll_create (pb_algorithm_t* algorithm,
                                pb_session_t* session,
                                pb_finger_t* finger,
                                const pb_qualityI* quality,
                                const pb_mte_guiI* gui,
                                pb_template_type_t template_type,
                                const pb_multitemplate_enrollment_config_t* config);

/** Creates a simpler less resource demanding version of the enrollment controller,
  * more suited for MCU. This controller gives less user feedback and has less
  * sophisticated sample selection than the MPU version and creates templates that can
  * only be incrementally updated until max capacity is reached.
  *
  * Some differences compared to the fully featured controller:
  *
  *   - gui is optional, may be null.
  *   - can be configured to limit number of checks against accepted
  *     samples, default is all but may be lowered when CPU speed requires it.
  *   - do not support prevent multiple fingers setting.
  *   - creates a more limited but to some extent more practical template format
  *     that can be incrementally updated until max capacity is reached. Max
  *     capacity is either max number of subtemplates or max template size.
  *   - the incremental nature of the template update allows update in Flash
  *     without erase (typically).
  *   - may limit max_nbr_captures if max template size is reached first,
  *     there is no point in collecting more than this many samples with current
  *     implementation (may change in future). In this case enrollment may end with
  *     PB_RC_CAPACITY, while not an error a reconfig may be relevant.
  *   - will not not give synthetic_image or display_island feedback
  *   - will not give calculate image coverage, 0 reported until last then 100.
  *   - will give basic feedback of required finger movements
  *   - will reject too similar samples, as tuned by the limit in number of
  *     verifications done.
  *
  * @param[in] algorithm is the algorithm to use.
  * @param[in] session is the session object.
  * @param[in] finger is the finger to enroll.
  * @param[in] quality is the quality module.
  *            In image driven mode this is not actually used.
  * @param[in] gui is an implementation of the gui callback interface.
  * @param[in] template_type is the type of template to be enrolled.
  * @param[in] config is the config parameters for the enrollment.
  *
  * @return the multitemplate enrollment object.
  *
 */
pb_multitemplate_enroll_t*
pb_multitemplate_enroll_mcu_create (pb_algorithm_t* algorithm,
                                    pb_session_t* session,
                                    pb_finger_t* finger,
                                    const pb_qualityI* quality,
                                    const pb_mte_guiI* gui,
                                    pb_template_type_t template_type,
                                    const pb_multitemplate_enrollment_mcu_config_t* config);

/** Deletes the multitemplate enrollment object. */
void
pb_multitemplate_enroll_delete (pb_multitemplate_enroll_t* mte);

/** Runs the image driven multitemplate enrollment. The enrollment will extract
  * a template from the image and match that against all previous captured
  * templates.
  *
  * Note that this function may not be called with another image once the
  * number of captures has reached maximum configured as the template should
  * instead be finalized. This can be tracked either via the GUI progress
  * feedback or comparing _get_nbr_of_captures() with the configured maximum.
  *
  * @param[in] mte is the multitemplate enrollment object.
  * @param[in] image is the image to use for enrollment. The image may be
  *            accepted as a captured template if it matches any of the previous
  *            captured templates and is not too similar to any of the previous
  *            captured images.
  * @param[in] template_ is the template to be used for enrollment if the caller
  *            has already extracted a template from the image, in which case the
  *            enrollment controller does not need to do an additional extraction.
  *            May be 0 in which case the enrollment controller will extract the
  *            template itself.
  * @param[out] coverage is the estimated coverage, in percent, of largest coherent
  *             area of the multitemplate (computed from all captured images).
  *
  * @return PB_RC_OK if successful, PB_RC_CAPACITY if feed more samples
  *         once max_nbr_of_captures has been reached or a capacity
  *         limit has been reached. In this case enrollment should be
  *         ended without further captures, if deemed acceptable.
  *         Any other code indicates an error.
  */
pb_rc_t
pb_multitemplate_enroll_run (pb_multitemplate_enroll_t* mte,
                             pb_image_t* image,
                             pb_template_t* template_,
                             uint8_t* coverage);

/** Finalizes and creates the multitemplate from the captured templates.
  *
  * @param[in] mte is the multitemplate enrollment object.
  * @param[in] max_nbr_of_templates is the maximum number of templates
  *            accepted into the multitemplate. This value becomes
  *            part of the configuration in the multitemplate and
  *            used in future updates of the template. A higher
  *            value should be used for small sensors while a lower
  *            value may consume less memory and shorter verification
  *            times.
  * @param[out] multitemplate is the returned multitemplate created
  *            from the captured templates.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_multitemplate_enroll_finalize_template (pb_multitemplate_enroll_t* mte,
                                           uint8_t max_nbr_of_templates,
                                           pb_template_t** multitemplate);

/** Returns the number of current captures. */
int
pb_multitemplate_enroll_get_nbr_of_captures (pb_multitemplate_enroll_t* mte);

#ifdef __cplusplus
}
#endif

#endif /* PB_MULTITEMPLATE_ENROLLMENT_H */
