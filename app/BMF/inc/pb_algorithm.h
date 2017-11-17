/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_ALGORITHM_H
#define PB_ALGORITHM_H

#include "pb_alignment.h"
#include "pb_defenderI.h"
#include "pb_extractorI.h"
#include "pb_finger.h"
#include "pb_image.h"
#include "pb_match_result.h"
#include "pb_qualityI.h"
#include "pb_returncodes.h"
#include "pb_session.h"
#include "pb_template.h"
#include "pb_types.h"
#include "pb_verifierI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An algorithm is a combination of an extractor, a verifier and
  * a template type. An algorithm further defines which features
  * that it uses as well as the default configuration parameters.
  *
  * Different algorithms are used for different sensor types and
  * sensor sizes. The platform on which the system is about to be
  * run can also affect the choice of algorithm.
  *
  * Note that only one algorithm may be created for each session
  * object. */
typedef struct pb_algorithm_st pb_algorithm_t;

/** The available features for the algorithms. Each feature has two
  * states, on (1) or off (0).
  * Each algorithm will have a default state for each feature. If the
  * state of the feature may be changed or not is determined by each
  * algorithm, meaning that some algorithms may not allow changing
  * the default state for some features.*/
typedef enum {
    /** Support of 360 degree search or not. Setting this feature to off
      * will result in that the verifier only searches +-45 degrees. */
    PB_ALGORITHM_FEATURE_360 = 0,
    /** This feature is applicable for the hybrid verifier where the
      * spectral verification part may search all angles for the best
      * possible match or only search a limited number of angles based
      * on pre-alignment information from the minutiae verification
      * part. It will always be used for the smaller (M, S, XS) sensor
      * sizes, but may be turned on for larger ones, for in some cases
      * improved biometric performance at the cost of increased
      * verification times. */
    PB_ALGORITHM_FEATURE_SPECTRAL_FULL_SEARCH = 1,
    /** Increase the search range for the high resolution search in the
      * spectral verification. For smaller (M, S, XS) sensors this feature
      * boosts performance at a cost of slightly longer verification times. */
    PB_ALGORITHM_FEATURE_SPECTRAL_INCREASE_HR_SEARCH_RANGE = 2,
    /** The number of algorithm features. */
    PB_ALGORITHM_NUMBER_OF_FEATURES
} pb_algorithm_feature_t;

/** The default configuration parameters for the algorithm. Configuration
  * parameters are always mutable and can be changed by calling _get_config
  * and then updating the desired parameters. */
typedef struct pb_algorithm_config_st {
    /** The max number of subtemplates that may be stored in a multitemplate.
      * The value is set as an attribute in the template.
      * Default value is algorithm dependent. */
    uint8_t max_nbr_of_subtemplates;

    /** The max number of subtemplates with extended data. Allows for having
      * only some subtemplates containing more (extended) data required by
      * more complex verifiers while having numerous templates with less data
      * but then covering more of the finger. Requires that a valid template
      * type has been set for extended_data_type. */
    uint8_t max_nbr_of_subtemplates_with_extended_data;

    /** The template data type of the extended data, see
      * max_nbr_of_subtemplates_with_extended_data. Set to PB_TEMPLATE_UNKNOWN
      * if this functionality is not needed. */
    pb_template_type_t extended_data_type;

    /** Tells if the encoded multitemplate shall be locked for further updates
      * when enrollment is completed, i.e. if the ability to enroll additional
      * templates into the multitemplate shall be disabled. The value is set
      * as an attribute in the template. Default 0. */
    int lock_template_from_further_updates;

    /** Tells if multiple fingers are allowed to be enrolled in a multitemplate.
      * Default 0.
      * Note: For smaller sensors it is strongly recommended that
      * prevent_enrollment_of_multiple_fingers is set to 0 to allow for a good
      * enrollment, although that multiple fingers may be enrolled! */
    int prevent_enrollment_of_multiple_fingers;

    /** Maximum template size in bytes. Applicable at enrollment. When
      * non-zero this parameter enforces a limit on how large the
      * template may be, including dynamic updates. The value is set
      * as an attribute in the template. Default value is 0 - no limit. */
    uint32_t max_template_size;

    /** The max number of templates to be collected during an enrollment.
      * Default value is algorithm dependent. */
    uint8_t max_nbr_of_enrollment_templates;

    /** Tells if the enrollment controller shall keep duplicate templates instead of
      * rejecting them. Default 0. */
    int keep_duplicate_templates;

    /** Tells if the enrollment controller shall lock templates, which prevent them
      * from being removed by further update operations. Only applies to the 
      * enrollment controller and to pb_algorithm_create_multitemplate. Default 1. */
    int lock_templates_after_enroll;

    /** The following parameters are only used for controller functionality that
      * captures images, see e.g. pb_multitemplate_enroll_algorithm. */

    /** The minimum fingerprint area, in mm^2, required for each captured image
      * to be used for e.g. enrollment or verification. Default value is
      * algorithm dependent, but ranges from 70-80% of the total image area. */
    uint32_t minimum_area_per_template;

    /** The minimum fingerprint area, in mm^2 for the combined area of all of the
      * templates in the multitemplate, required to complete enrollment. Default
      * value is algorithm dependent. */
    uint32_t minimum_area;

    /** The minimum fingerprint quality, ranging from 1 (worse) to 100 (best),
      * required for each captured image to be used for e.g. enrollment or
      * verification. default 20. */
    uint8_t minimum_quality;
} pb_algorithm_config_t;

/** A chain of algorithm steps. */
typedef struct pb_algorithm_chain_st pb_algorithm_chain_t;

/** Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_algorithm_t*
pb_algorithm_retain (pb_algorithm_t* algorithm);

/** Decreases the reference counter for the object by 1. If the reference
  * counter reaches 0 then the object will also be deleted.
  * Deleting a null pointer has no effect. */
void
pb_algorithm_delete (pb_algorithm_t* algorithm);


/** Extracts a template from an image.
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] image is the image to extract information from
  *            into a template.
  * @param[in] finger is the finger associated with the image. May
  *            be set to 0 in which case no information about the
  *            finger will be stored in the template.
  * @param[out] T is the returned fully extracted template.
  *            The caller is responsible for deleting the returned
  *            template.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_algorithm_extract_template (pb_algorithm_t* algorithm,
                               const pb_image_t* image,
                               const pb_finger_t* finger,
                               pb_template_t** T);

/** Extracts or completes a partially extracted a template from an image.
  * 
  * Works like pb_algorithm_extract_template() but parameter T is an in/out
  * parameter. If T points to a null template on input then the operation
  * is the same. If T points to a partial (or complete) template from e.g.
  * pb_algorithm_verify_image() it will be made complete by adding missing
  * information that is needed to do a dynamic update which requires a fully
  * extracted template.
  *
  * The ownership of input template is taken from the caller and
  * deleted, the ownership of the retuned returned template is transferred
  * to the caller.
  * 
  * Any data already present in template T is not extracted again, thus saving
  * execution time.
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] image is the image to extract information from
  *            into a template.
  * @param[in] finger is the finger associated with the image. May
  *            be set to 0 in which case no information about the
  *            finger will be stored in the template.
  * @param[in/out] On input T may point to null or a partially
  *            extracted template. It will be deleted.
  *            On output it contains a fully extracted template.
  *            The caller is responsible for deleting the returned
  *            template.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_algorithm_extract_template_ex (pb_algorithm_t* algorithm,
                                  const pb_image_t* image,
                                  const pb_finger_t* finger,
                                  pb_template_t** T);

/** Creates a multitemplate from a number of templates. This function may
  * be used when the enrollment functionality is not needed and the templates
  * have been collected in another way, e.g. from a database or from a
  * proprietary enrollment procedure.
  *
  * The recommended way to create a multitemplate is by using the enrollment
  * functionality defined in pb_multitemplate_enrollment_algorithm.h.
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] templates is the templates to be enrolled.
  * @param[in] nbr_of_templates is the number of templates to be enrolled.
  * @param[out] multitemplate is the returned enrolled multitemplate. The
  *             caller is responsible for deleting the multitemplate.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_algorithm_create_multitemplate (pb_algorithm_t* algorithm,
                                   pb_template_t* templates[],
                                   uint8_t nbr_of_templates,
                                   pb_template_t** multitemplate);

/** Creates a incremental update multitemplate. Incremental templates
  * uses less resources at enrollment and update but only supports
  * updates until max capacity is reached. The incremental updates
  * are more suited on MCU environments and the nature of incremental
  * update makes them more suited for update directly in Flash
  * without flash erase (typically).
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] templates is the templates to be enrolled.
  * @param[in] nbr_of_templates is the number of templates to be enrolled.
  * @param[out] multitemplate is the returned enrolled multitemplate. The
  *             caller is responsible for deleting the multitemplate.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_algorithm_create_incr_multitemplate (pb_algorithm_t* algorithm,
                                        pb_template_t* templates[],
                                        uint8_t nbr_of_templates,
                                        pb_template_t** multitemplate);

/** Updates a multitemplate with one or more new templates. This function does not
  * check that the new templates are from the same finger as the multitemplate. The
  * caller must assure that they are at least from the same person by e.g. only
  * calling this function with templates that has been previously verified against
  * the multitemplate. Calling this function with unverified templates could
  * compromise the security of the system.
  *
  * Use this function mainly to dynamically update the multitemplate after a true
  * verification.
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] multitemplate is the multitemplate to be updated.
  * @param[in] templates is the templates to update the multitemplate with.
  * @param[in] nbr_of_templates is the number of templates to update the
  *            multitemplate with.
  * @param[out] updated_multitemplate is the returned updated multitemplate. The
  *             caller is responsible for deleting the updated multitemplate.
  *             If no update is made it will be set to 0.
  */
pb_rc_t
pb_algorithm_update_multitemplate (pb_algorithm_t* algorithm,
                                   const pb_template_t* multitemplate,
                                   pb_template_t* templates[],
                                   uint8_t nbr_of_templates,
                                   pb_template_t** updated_multitemplate);

//
pb_rc_t
pb_algorithm_update_multitemplate_ex(pb_algorithm_t* algorithm,
                                     const pb_template_t* multitemplate,
                                     pb_match_result_t* match_result,
                                     pb_template_t** updated_multitemplate);

/** Verifies a verification template against one or more enrolled multitemplates and
  * delivers a decision based on the requested FAR level.
  *
  * NOTE: This function does not support automatic dynamic update of the enrolled
  *       template, this needs to be handled by the caller. This allows for notifying
  *       the user as fast as possible of an accept, and then the more time consuming
  *       dynamic update shall be done afterwards, typically using a background thread.
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] enrolled_templates the enrolled templates to verify against.
  * @param[in] nbr_of_enrolled_templates is the number of enrolled templates
  *            to verify against.
  * @param[in] verification_template is the template to be verified.
  * @param[in] false_accept_rate is the requested false accept rate, see
  *            PB_FAR_X.
  * @param[out] decision is the returned decision of the match, see
  *             PB_DECISION_X.
  * @param[out] alignment is the returned alignment of the match. Note that
  *             the alignment may not be very reliable in the event of a poor match.
  *             The caller is responsible for deallocating the returned alignment. May
  *             be set to 0 if no alignment information is needed.
  * @param[out] matching_enrolled_template_index is the index of the enrolled template
  *             that matched the verification template, if any. May be set to 0
  *             if not needed.
  * @param[out] matching_subtemplate_index is the index of the subtemplate
  *             that matched the verification template, if any. May be set to 0
  *             if not needed. Note that this is only applicable for when the
  *             enrolled templates are multitemplates, in single template cases,
  *             0 is always returned.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_algorithm_verify_templates (pb_algorithm_t* algorithm,
                               pb_template_t* enrolled_templates[],
                               uint8_t nbr_of_enrolled_templates,
                               const pb_template_t* verification_template,
                               pb_far_t false_accept_rate,
                               int* decision,
                               pb_alignment_t** alignment,
                               int* matching_enrolled_template_index,
                               int* matching_subtemplate_index);

//
pb_rc_t
pb_algorithm_verify_templates_ex(pb_algorithm_t* algorithm,
                                 pb_template_t* enrolled_templates[],
                                 uint8_t nbr_of_enrolled_templates,
                                 const pb_template_t* verification_template,
                                 pb_far_t false_accept_rate,
                                 pb_match_result_t** match_result);

/** Verifies an image against one or more enrolled multitemplates and delivers a
  * decision based on the requested FAR level. This function may improve the total
  * verification time compared to calling _extract_template followed by a call to
  * _verify_templates.
  *
  * IMPORTANT: The function returns an extracted template that may be used e.g. for dynamic
  * update. However, the template may not be fully extracted and the caller must call
  * _extract_template_ex with the returned template before using it, e.g. for dynamic update,
  * else it may only be a partial template.
  *
  * NOTE: This function does not support automatic dynamic update of the enrolled
  *       template, this needs to be handled by the caller. This allows for notifying
  *       the user as fast as possible of an accept, and then the more time consuming
  *       dynamic update shall be done afterwards, typically using a background thread.
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] enrolled_templates the enrolled templates to verify against.
  * @param[in] nbr_of_enrolled_templates is the number of enrolled templates
  *            to verify against.
  * @param[in] verification_image is the image to be verified.
  * @param[in] false_accept_rate is the requested false accept rate, see
  *            PB_FAR_X.
  * @param[out] decision is the returned decision of the match, see
  *             PB_DECISION_X.
  * @param[out] alignment is the returned alignment of the match. Note that
  *             the alignment may not be very reliable in the event of a poor match.
  *             The caller is responsible for deallocating the returned alignment. May
  *             be set to 0 if no alignment information is needed.
  * @param[out] matching_enrolled_template_index is the index of the enrolled template
  *             that matched the verification image, if any. May be set to 0
  *             if not needed.
  * @param[out] matching_subtemplate_index is the index of the subtemplate
  *             that matched the verification image, if any. May be set to 0
  *             if not needed. Note that this is only applicable for when the
  *             enrolled templates are multitemplates, in single template cases,
  *             0 is always returned.
  * @param[out] extracted_template is the template extracted from the image during the
  *             verification. The template may not be fully extracted and hence the
  *             caller must call _extract_template_ex to fully extract the template before
  *             using the template, e.g. for dynamic update. The caller is responsible
  *             for deleting the returned template. May be set to 0 if not needed.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_algorithm_verify_image (pb_algorithm_t* algorithm,
                           pb_template_t* enrolled_templates[],
                           uint8_t nbr_of_enrolled_templates,
                           const pb_image_t* verification_image,
                           pb_far_t false_accept_rate,
                           int* decision,
                           pb_alignment_t** alignment,
                           int* matching_enrolled_template_index,
                           int* matching_subtemplate_index,
                           pb_template_t** extracted_template);

//
pb_rc_t
pb_algorithm_verify_image_ex(pb_algorithm_t* algorithm,
                             pb_template_t* enrolled_templates[],
                             uint8_t nbr_of_enrolled_templates,
                             const pb_image_t* verification_image,
                             pb_far_t false_accept_rate,
                             pb_match_result_t** match_result);

/** Verifies a verification template against one or more enrolled templates with
  * the purpose of getting a similarity score. This function is primarily useful for
  * biometric evaluation and generating DET curves. For production code use the default
  * pb_algorithm_verify_templates() function as it is faster and the only way to
  * retrieve a correct decision.
  *
  * The DET curve generated from scores using this function is representative of the
  * actual performance achieved when calling pb_algorithm_verify_templates().
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] enrolled_templates the enrolled templates to verify against.
  * @param[in] nbr_of_enrolled_templates is the number of enrolled templates
  *            to verify against.
  * @param[in] verification_template is the template to be verified.
  * @param[out] score is the returned similarity score of the match. The higher
  *             the score the higher the similarity of the two templates.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_algorithm_get_similarity_score (pb_algorithm_t* algorithm,
                                   pb_template_t* enrolled_templates[],
                                   uint8_t nbr_of_enrolled_templates,
                                   const pb_template_t* verification_template,
                                   uint16_t* score);

/** Sets the state of a specific algorithm feature to on or off.
  * Note: The algorithm determines if a certain feature may change
  * its state or if the default state shall always be used. If the
  * state of the feature may not be changed, PB_RC_NOT_SUPPORTED is
  * returned.
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] feature is the feature to be turned on or off.
  * @param[in] state is the state to be set for the feature, on (1) or off (0).
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_algorithm_set_feature_state (pb_algorithm_t* algorithm,
                                pb_algorithm_feature_t feature,
                                int state);

/** Returns the state of a certain feature.
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] feature is the feature to get the state for.
  *
  * @return the state of the feature, on (1) or off (0).
  */
int
pb_algorithm_get_feature_state (pb_algorithm_t* algorithm,
                                pb_algorithm_feature_t feature);

/** Tells if a feature may be changed for the algorithm.
  *
  * @param[in] algorithm is the algorithm object.
  * @param[in] feature is the feature to check if it may be changed.
  *
  * @return 1 if the feature may be changed, or 0 if not.
  */
int
pb_algorithm_feature_is_mutable (pb_algorithm_t* algorithm,
                                 pb_algorithm_feature_t feature);

/** Returns the session object from the algorithm object.
 *  The object must be retained if kept after algorithm is deleted. */
pb_session_t*
pb_algorithm_get_session (pb_algorithm_t* algorithm);

/** Returns the supported sensor size of the algorithm.
  * Note: Using a sensor size that differ too much from the supported
  * sensor size may result in undefined behavior. */
pb_sensor_size_t
pb_algorithm_get_sensor_size (pb_algorithm_t* algorithm);

/** Returns the supported sensor type of the algorithm.
  * Note: Using a sensor type that differs from the supported
  * sensor type may result in undefined behavior. */
pb_sensor_type_t
pb_algorithm_get_sensor_type (pb_algorithm_t* algorithm);

/** Returns the extractor preferred image dimensions. Normally this
  * function should not be needed as the extractor scales image
  * internally as needed but for systems with small mempory it may
  * be preferred to scale down an image to suite the extractor
  * before extraction and then this function should be used to
  * get the correct scaling parameters.
  *
  * In some cases the preferred image dimensions may simply be the
  * corresponding native resolution of the extractor while in other
  * cases the image itself may be enlarged or adjusted to an even
  * block size.
  *
  * The preferred image should be scaled to the specified rows/cols while
  * setting the dpi as specified, e.g. using:
  *
  *   pb_image_scale_size_res(image, irows, icols, ivres, ihres);
  *
  * This will ensure the image is not unnecessary scaled internally as well.
  */
void
pb_algorithm_get_pref_idim (pb_algorithm_t* algorithm,
                            uint16_t vres, uint16_t hres, uint16_t rows, uint16_t cols,
                            uint16_t* ivres, uint16_t* ihres, uint16_t* irows, uint16_t* icols);

/** Returns the chain of verifiers used by the algorithm. */
pb_algorithm_chain_t*
pb_algorithm_get_algorithm_chain (pb_algorithm_t* algorithm);

/** Returns the main verifier in the verifier chain. */
const pb_verifierI*
pb_algorithm_get_verifier (pb_algorithm_t* algorithm);

/** Returns the template type used by the algorithm. */
pb_template_type_t
pb_algorithm_get_template_type (pb_algorithm_t* algorithm);

/** Returns the configuration parameters used by the algorithm.
  * The returned config may be changed by the caller in order to update
  * a certain configuration setting. All configuration settings are
  * mutable. */
pb_algorithm_config_t*
pb_algorithm_get_config (pb_algorithm_t* algorithm);

/** Set number of slice rows. Only affects algorithms that use a slice
    extractor. */
pb_rc_t
pb_algorithm_set_slice_rows(pb_algorithm_t* algorithm, int slice_rows);

/** Get number of slice rows in use by slice extractors. */
int
pb_algorithm_get_slice_rows(pb_algorithm_t* algorithm);


#ifdef __cplusplus
}
#endif

#endif /* PB_ALGORITHM_H */
