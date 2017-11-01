/*
 * Copyright (c) 2008 - 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_ALGORITHMI_H
#define PB_ALGORITHMI_H

#include "pb_returncodes.h"
#include "pb_session.h"
#include "pb_algorithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The interface to be implemented for an algorithm to be used with the
  * algorithm API. 
  *
  * The algorithm API consists of this interface along with the algorithm
  * object API, see pb_algorithm.h, and the API:s for any controller implementing
  * support for the algorithm object, see pb_multitemplate_enrollment_algorithm.h
  * (and pb_verification_algorithm.h).
  *
  * The algorithm API is intended to be the main API to be used by customers to 
  * ensure that the optimal performance is achieved for each use case. The customer
  * chooses the algorithm implementation that best suits their needs, specifically
  * an algorithm implementation is chosen based on which sensor size it supports
  * as well as which platform it is going to be run at. Some configurations are 
  * also possible for each algorithm, see e.g. pb_algorithm_config_t and 
  * pb_algorithm_feature_t. 
  *
  * Each algorithm implementation will determine which extractor, verifier and 
  * template type that will be used. The implementation also decides for which 
  * sensor type and size that it should be used. Finally, it sets the default 
  * features and configuration parameters to be used by the algorithm. 
  *
  * Several algorithm implementations have been implemented to fit all current 
  * customer requirements. If no implementation exists that matches certain 
  * requirements, new implementations can easily be written. 
  *
  * For the currently available algorithm implementations, please refer to the
  * Developer Guide document (SPA 133 1000 - 4160/devg). 
  *
  * Example:
  * 
  *     pb_session_t* session = pb_session_create ();
  *     pb_rc_t status;
  * 
  *     // Create an algorithm object.
  *     pb_algorithm_t* algorithm = hybrid_square_l_algorithm.create (session);
  * 
  *     // Enrollment
  *     pb_multitemplate_enroll_t* mte;
  *     pb_template_t* multitemplate;
  *     // Create a multitemplate enrollment controller object.
  *     mte = pb_multitemplate_enroll_create_algorithm (algorithm, ...);
  *     ...
  *     // Run the enrollment controller (for each captured image)
  *     status = pb_multitemplate_enroll_run (mte, image, ...);
  *     ...
  *     // Complete the enrollment.
  *     status = pb_multitemplate_enroll_finalize_template_algorithm (algorithm, mte, &multitemplate);
  *     pb_multitemplate_enroll_delete (mte);
  *     ...
  * 
  *     // Verification
  *     pb_template_t* multitemplates[5];
  *     pb_template_t* verification_template;
  *     int decision;
  *     int matching_multitemplate_index;
  *     int matching_subtemplate_index;
  *     // Extract a template from the captured image.
  *     status = pb_algorithm_extract_template (algorithm, image, ..., &verification_template);
  *     // Verify against one or more enrolled multitemplates.
  *     status = pb_algorithm_verify_templates (algorithm, multitemplates, 5, 
  *                                             verification_template, PB_FAR_50K, &decision, 0,
  *                                             &matching_multitemplate_index, &matching_subtemplate_index);
  *
  *     // Dynamic Update
  *     pb_template_t* updated_multitemplate;
  *     // Update the enrolled multitemplate with the matching template
  *     if (decision == PB_DECISION_MATCH) {
  *         status = pb_algorithm_update_multitemplate (algorithm, multitemplates[matching_multitemplate_index],
  *                                                     &verification_template, 1, &updated_multitemplate);
  *     }
  * 
  *     // Delete the algorithm object.
  *     pb_algorithm_delete (algorithm);
  * 
  *     pb_session_delete (session);
  * 
  */

/** Creates a specific algorithm where e.g. the extactor, verifier
  * and template type is defined as well as other default features
  * and configuration parameters.
  * 
  * The implementation shall call pb_algorithm_create with the appropriate
  * parameters for the specific algorithm. 
  *
  * @param[in] session is the session object.
  *
  * @return the allocated algorithm object if successful, else 0. The 
  *         caller is responsible for deleting the returned object by
  *         calling pb_algorithm_delete.
  */
typedef pb_algorithm_t* pb_algorithmI_create_fn_t (pb_session_t* session);

/** The algorithm object API. */
typedef struct {
    pb_algorithmI_create_fn_t* create;
} pb_algorithmI;

#ifdef __cplusplus
}
#endif

#endif /* PB_ALGORITHMI_H */
