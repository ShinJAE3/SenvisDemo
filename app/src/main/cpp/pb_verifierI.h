/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_VERIFIERI_H
#define PB_VERIFIERI_H

#include "pb_returncodes.h"
#include "pb_session.h"
#include "pb_template.h"
#include "pb_alignment.h"

#ifdef __cplusplus
extern "C" {
#endif

/* False Accept Rates, (1/X). */
typedef enum pb_far_e {
    PB_FAR_1               = 0,
    PB_FAR_2               = 1,
    PB_FAR_5               = 2,
    PB_FAR_10              = 3,
    PB_FAR_20              = 4,
    PB_FAR_50              = 5,
    PB_FAR_100             = 6,
    PB_FAR_200             = 7,
    PB_FAR_500             = 8,
    PB_FAR_1000            = 9,
    PB_FAR_2K              = 10,
    PB_FAR_5000            = 11,
    PB_FAR_10000           = 12,
    PB_FAR_20K             = 13,
    PB_FAR_50000           = 14,
    PB_FAR_100000          = 15,
    PB_FAR_200K            = 16,
    PB_FAR_500000          = 17,
    PB_FAR_1000000         = 18,
    PB_FAR_2M              = 19,
    PB_FAR_5M              = 20,
    PB_FAR_10M             = 21,
    PB_FAR_20M             = 22,
    PB_FAR_50M             = 23,
    PB_FAR_100M            = 24,
    PB_FAR_200M            = 25,
    PB_FAR_500M            = 26,
    PB_FAR_1000M           = 27,
    PB_FAR_Inf             = 28
} pb_far_t;

#define PB_FAR_1K   PB_FAR_1000
#define PB_FAR_5K   PB_FAR_5000
#define PB_FAR_10K  PB_FAR_10000   
#define PB_FAR_50K  PB_FAR_50000
#define PB_FAR_100K PB_FAR_100000
#define PB_FAR_500K PB_FAR_500000
#define PB_FAR_1M   PB_FAR_1000000

#define PB_FAR_MAX PB_FAR_1000M

/** Moves the FAR level a number of steps up or down.
  * 
  * This function will not shift FAR past PB_FAR_1 or PB_FAR_MAX.
  * 
  * PB_FAR_Inf will always remain unmodified.
  * 
  * Different platforms handle enums differently, so this function provides
  * a safe way to move FAR level without having to account for different
  * signedness, and bit size.
  * 
 **/
pb_far_t
pb_far_shift(pb_far_t input, int16_t steps);

/* Decisions. */
#define PB_DECISION_MATCH       1
#define PB_DECISION_NON_MATCH   0

/**
 * Internal verifier model type.
 *
 */
typedef struct pb_vermodel_s pb_vermodel_t;

/* Forward declaration, see definition below! */
typedef struct pb_verifierI_s pb_verifierI;


/** The number of multitemplate sizes of the threshold table. The following
  * values are supported: 1 (single template), 2, 4, 8, 16, 32, 64. */
#define PB_VERIFIER_NBR_OF_MULTITEMPLATE_SIZES  7

/** The number of FAR values of the threshold table. The following values are
  * supported: PB_FAR_1, _2, _5, _10, _20, _50, _100 ... PB_FAR_1000M. */
#define PB_VERIFIER_NBR_OF_FAR_LEVELS           (PB_FAR_MAX+1)

/** If a certain threshold is not supported by the threshold table (e.g. due to
  * the extreme amount of time and templates needed to compute accurate threshold
  * values for the really low FAR levels), then this value may be used in the 
  * table instead, indicating that no threshold has been computed for this 
  * combination of image size, multitemplat size and FAR level. A verification 
  * using this threshold value will always result in a reject. */
#define PB_VERIFIER_THRESHOLD_NOT_SUPPORTED           65535

/** A threshold table for a verifier. 
  *
  * Thresholds for a certain verification algorithm may vary depending on the image
  * size and the multitemplate size (the number of templates in the multitemplate)
  * and hence a threshold table is often needed ro represent the entire spectrum of
  * thresholds based on these parameters as well as the requested FAR level. 
  *
  * Example of a threshold table:
  * static const pb_verifier_threshold_table_t ehmhr_thresholds_square_xs =
  *     { // 100x100
  *       //     1     2     5    10    20    50   100   200   500    1K    2K    5K   10K   20K   50K  100K  200K  500K    1M    2M    5M   10M   20M   50M  100M  200M  500M    1B
  *         {    0,  941, 1901, 2495, 3006, 3592, 3989, 4361, 4830, 5175, 5517, 5967, 6307, 6646, 7093, 7428, 7760, 8192, 8514, 8830, 9241, 9547, 9850,10245,10541,10835,11217,11500,}, // 1of1
  *         {    0, 1494, 2458, 3044, 3544, 4117, 4509, 4879, 5349, 5697, 6039, 6485, 6815, 7138, 7552, 7853, 8143, 8510, 8778, 9040, 9380, 9637, 9899,10254,10533,10821,11211,11504,}, // 2of2
  *         {    0, 2145, 2946, 3477, 3957, 4530, 4928, 5303, 5773, 6114, 6445, 6871, 7187, 7497, 7898, 8196, 8487, 8861, 9135, 9402, 9741, 9988,10227,10532,10757,10978,11269,11493,}, // 4of4
  *         {    0, 2590, 3381, 3900, 4364, 4914, 5295, 5653, 6102, 6427, 6745, 7155, 7460, 7761, 8152, 8442, 8728, 9096, 9368, 9633, 9971,10219,10459,10766,10991,11210,11492,11701,}, // 8of8
  *         {    0, 2916, 3710, 4211, 4650, 5166, 5523, 5861, 6291, 6608, 6920, 7327, 7631, 7930, 8317, 8603, 8881, 9238, 9498, 9750,10073,10309,10540,10838,11058,11273,11545,11738,}, // 16of16
  *         {    0, 3275, 4010, 4486, 4910, 5412, 5761, 6091, 6506, 6809, 7104, 7486, 7768, 8045, 8404, 8670, 8930, 9266, 9514, 9757,10073,10307,10538,10838,11059,11275,11547,11737,}, // 32of32
  *         {    0, 3489, 4225, 4688, 5092, 5563, 5887, 6193, 6580, 6865, 7145, 7512, 7786, 8057, 8410, 8673, 8931, 9266, 9513, 9756,10072,10306,10537,10838,11060,11277,11548,11736,}  // 64of64
  *     };
  */
typedef uint16_t pb_verifier_threshold_table_t[PB_VERIFIER_NBR_OF_MULTITEMPLATE_SIZES][PB_VERIFIER_NBR_OF_FAR_LEVELS];

/** The available multitemplate sizes of the threshold table. */
static const uint8_t pb_verifier_multitemplate_sizes[PB_VERIFIER_NBR_OF_MULTITEMPLATE_SIZES] = {1, 2, 4, 8, 16, 32, 64};

/** Verifies a verification template against an enrolled template and returns
  * a vector of feature scores instead of one final score or a decision. The 
  * feature scores may be used to either train a model or use a previously 
  * trained model to predict a final score.
  * Note: This function is mainly for internal use, external usage needs to be 
  * handled with care since a verifier may change the order, values or number 
  * of features returned between different versions.
  *
  * @param[in] session is the session object.
  * @param[in] enrolled_template is the enrolled template to verify against.
  * @param[in] verification_template is the template to be verified.
  * @param[out] feature_scores is the returned vector of feature scores. The 
  *     caller is responsible for deleting the returned vector.
  * @param[out] nbr_of_scores is the number of feature scores returned.
  * @param[in/out] alignment is an input/output parameter or null if no parameter.
  *     - On input ownership of the alignment object is transferred
  *     from the caller. The input alignment object itself may be null if
  *     there is no input alignment but an output alignment is requested.
  *     Some verifiers require an input alignment to function and hence
  *     some other verifier in a chain has been used to create the input
  *     alignment.
  *     - On output there may be an output alignment object (typically this
  *     is expected by most code) otherwise a null object is set.
  *     The ownership of the output alignment object is transfered to
  *     the caller. It may e.g. be used as input to another link in
  *     a chain.
  * @param[in] vermodels is an optional parameter that is supported by certain
  *            verifiers for testing purposes.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_verifierI_extract_feature_scores_fn_t (pb_session_t* session,
                                                          const pb_template_t* enrolled_template, 
                                                          const pb_template_t* verification_template,
                                                          uint16_t* feature_scores[],
                                                          uint8_t* nbr_of_feature_scores,
                                                          pb_alignment_t** alignment);

/** Computes the score and decision from the feature scores. */
typedef pb_rc_t pb_verifierI_verify_feature_scores_fn_t (pb_session_t* session,
                                                         const pb_template_t* enrolled_template, 
                                                         const pb_template_t* verification_template,
                                                         const uint16_t feature_scores[],
                                                         uint8_t nbr_of_feature_scores,
                                                         const pb_vermodel_t* verifier_model,
                                                         uint16_t* score);

/** The fingerprint matcher interface allows different fingerprint
  * template verifiers to be implemented.
  * 
  * The purpose of a fingerprint verifier is to calculate a similarity
  * score between two templates and optionally also a decision if they
  * match or not.
  * 
  * Different implementations supports different types of templates.
  * The application must be configured to use the appropriate verifier
  * type for the templates used.
  *
  * See separate descriptions for each function.
  *
  * A verifier not supporting all of the functions shall return
  * PB_RC_NOT_SUPPORTED when a non-supported function is called. In this way the
  * calling application can easily determine which function to call.
  */
struct pb_verifierI_s {
    pb_verifierI_extract_feature_scores_fn_t* extract_feature_scores;
    pb_verifierI_verify_feature_scores_fn_t* verify_feature_scores;
};
/* pb_verifierI declared above: typedef struct pb_verifierI_s pb_verifierI */


pb_rc_t
pb_verifier_verify_feature_scores (pb_session_t* session,
                                   const uint16_t feature_scores[],
                                   uint8_t nbr_of_feature_scores,
                                   const pb_vermodel_t* verifier_model,
                                   uint16_t* score);

pb_rc_t
pb_verifier_get_raw_similarity_score (pb_session_t* session,
                                      const pb_verifierI* verifier,
                                      const pb_vermodel_t* verifier_model,
                                      const pb_template_t* enrolled_template, 
                                      const pb_template_t* verification_template,
                                      uint16_t* score,
                                      pb_alignment_t** alignment);

/** Helper function for verifier implementations to computes decision from score 
  * based on multitemplate size and FAR level. The function also uses the sensor
  * sensor type and sensor size set in the session object to determine which 
  * thresholds to use.
  * Returns PB_RC_NOT_SUPPORTED if the corresponding threshold value is 
  * PB_VERIFIER_THRESHOLD_NOT_SUPPORTED or if a corresponding threshold table
  * does not exist.
  * 
  * @param[in] verifier is the verifier implementation.
  * @param[in] score is the verification score.
  * @param[in] nbr_of_subtemplates is the number of subtemplates in the enrolled
  *            template, assuming that the enrolled template is a multitemplate.
  *            If this is not the case, i.e. if the enrolled template is a 
  *            single template, set multitemplate_size to 1. 
  * @param[in] false_accept_rate is the requested false accept rate, see
  *            PB_FAR_X.
  * @param[out] decision is the returned decision.
  * @param[out] threshold is the returned threshold used when the decision was
  *             made. May be 0 if not needed.
  * @param[in] vermodels is an optional parameter that is supported by certain
  *            verifiers for testing purposes.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
pb_rc_t
pb_verifier_decision_from_score (pb_session_t* session,
                                 uint16_t score, 
                                 uint8_t nbr_of_subtemplates,
                                 pb_far_t false_accept_rate,
                                 const pb_verifier_threshold_table_t* threshold_table,
                                 int* decision,
                                 uint16_t* threshold);

#ifdef __cplusplus
}
#endif

#endif /* PB_VERIFIERI_H */
