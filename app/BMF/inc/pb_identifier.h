/*
 * Copyright (c) 2008 - 2014, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_IDENTIFIER_H
#define PB_IDENTIFIER_H

#include "pb_types.h"
#include "pb_returncodes.h"
#include "pb_session.h"
#include "pb_finger.h"
#include "pb_template.h"
#include "pb_alignment.h"
#include "pb_verifierI.h"

#ifdef __cplusplus
extern "C" {
#endif

/* False Positive Identification rates, (1/X). */
typedef enum pb_fpir_e {
    PB_FPIR_5 = 1,
    PB_FPIR_10 = 2,
    PB_FPIR_50 = 3,
    PB_FPIR_100 = 4,
    PB_FPIR_500 = 5,
    PB_FPIR_1000 = 6,
    PB_FPIR_5000 = 7,
    PB_FPIR_10000 = 8,
    PB_FPIR_50000 = 9,
    PB_FPIR_100000 = 10,
    PB_FPIR_500000 = 11,
    PB_FPIR_1000000 = 12
} pb_fpir_t;

typedef struct pb_identifier_st pb_identifier_t;


/** Adds enrolled templates and their associated fingers to
 * the identifier. Call this function whenever new templates
 * have been added to the database or after the application
 * has been restarted. The identifier uses this information
 * to keep track of the enrolled templates/fingers(users)
 * and to pre-process the templates in order to speed up the
 * identication process.
 *
 * Templates must have been added using this function or
 * they won't be identified against when calling identify.
 *
 * Note: The identifier does not store the templates
 * permanently, i.e. when the application shuts down all the
 * internal pre-processed templates will be deleted. The
 * original templates must be stored in a permanent storage
 * somewhere else (preferably in the database module).
 *
 * @param[in] identifier is the identifier object.
 * @param[in] templates is the templates to be added to the
 *     identifier.
 * @param[in] fingers is the fingers associated with the
 *     templates. The fingers must contain a valid user,
 *     since the user id will be returned when identifying.
 *     Adding identical fingers (identical user id and position)
 *     is allowed, but not recommended.
 * @param[in] references are custom reference values used to
 *     associate a specific enrolled template/finger tuple.
 *     This parameter is optional and can be set to 0 if
 *        a reference isn't needed.
 * @param nbr_of_fingers is the number of fingers/templates to
 *     be added to the identifier.
 *
 * @return PB_RC_OK if successful, or an error code.
 */
typedef pb_rc_t pb_identifierI_add_templates_fn_t(pb_identifier_t* identifier,
                                                  pb_template_t* templates[],
                                                  pb_finger_t* fingers[],
                                                  void* references[],
                                                  uint16_t nbr_of_fingers);

/** Removes all templates from the identifier. Call this function
 * whenever templates have been removed from the database to
 * update the identifier.
 *
 * Removed templates will not be identified against when
 * calling identify.
 *
 * @param[in] identifier is the identifier object.
 *
 * @return PB_RC_OK if successful, or an error code.
 */
typedef pb_rc_t pb_identifierI_remove_all_templates_fn_t(pb_identifier_t* identifier);

/** Removes templates from the identifier. Call this function
 * whenever templates have been removed from the database to
 * update the identifier.
 *
 * Removed templates will not be identified against when
 * calling identify.
 *
 * @param[in] identifier is the identifier object.
 * @param[in] fingers is the associated fingers of the
 *     templates to be removed from the identifier. If
 *     duplicates exist, they will all be removed.
 * @param nbr_of_fingers is the number of fingers/templates to
 *     be removed from the identifier.
 *
 * @return PB_RC_OK if successful, or an error code.
 */
typedef pb_rc_t pb_identifierI_remove_templates_fn_t(pb_identifier_t* identifier,
                                                     const pb_finger_t* fingers[],
                                                     uint16_t nbr_of_fingers);


/** Identifies the template among the templates added to the identifier. If a
 * match is found, the finger for the matching template is returned. The search
 * criteria can be limited by specifying the finger parameter.
 *
 * @param[in] identifier is the identifier object.
 * @param[in] template_ is the template to be identified.
 * @param[in] filter is used to limit the database set used for identification.
 *     Set to 0 to perform identification against the entire enrolled database.
 * @param[in] false_positive_identification_rate is the requested false
 *     positive identification rate, see PB_FPIR_X.
 * @param[out] identified_finger is the returned identified finger of the
 *     template, or 0 if no finger matched. Use pb_finger_get_user to get the
 *     identified user from the identified finger.
 * @param[out] reference is the associated client reference supplied when the
 *     finger/template tuple was added to the database. Can be set to 0 if no
 *     reference value is expected or needed.
 * @param[out] alignment is the returned alignment of the match. May be set to
 *     0 if no alignment information is needed.
 * Note: The caller is responsible for deleting the returned finger and
 * alignment.
 *
 * @return PB_RC_OK if successful, or an error code.
 */
typedef pb_rc_t pb_identifierI_identify_template_fn_t(pb_identifier_t* identifier,
                                                      const pb_template_t* template_,
                                                      const pb_finger_t* finger,
                                                      pb_fpir_t false_positive_identification_rate,
                                                      pb_finger_t** identified_finger,
                                                      void** reference,
                                                      pb_alignment_t** alignment);

/** Identifies the rank best matching templates among the templates added to
 * the identifier, and returns the fingers and scores for those matches. This
 * function shall be used when one wants to use several algorithms in order to
 * determine the true identification. The search criteria can be limited by
 * specifying the finger parameter.
 * E.g. call this function with rank 100 and then identify the template among
 * these 100 best matches using another identification algorithm.
 * Note: This function shall not be called with rank 1 in order to identify
 * the user of the template, since the user of the template may not be enrolled
 * the database (open-set identification). Use the non-rank function above
 * instead for that scenario.
 *
 * @param[in] identifier is the identifier object.
 * @param[in] template_ is the template to be identified.
 * @param[in] filter is used to limit the database set used for identification.
 *     Set to 0 to perform identification against the entire enrolled database.
 * @param[in] rank is the number of fingers and scores returned.
 * @param[out] identified_fingers is the returned identified fingers. The
 *     caller is responsible for allocating memory for the array.
 * @param[out] scores is the returned scores for each identified finger. May be
 *     set to 0 if no scores are needed. The caller is responsible for
 *     allocating memory for the array.
 * @param[out] references is the client references associated with the returned
 *     identified fingers. May be set to 0 if no references are needed. The caller
 *     is responsible for allocating memory for the array.
 * @param[out] alignments is the returned alignments of the matches. May be
 *     set to 0 if no alignment information is needed. The caller is
 *     responsible for allocating memory for the alignments array.
 * Note: The caller is responsible for deleting the returned users and
 * alignments.
 *
 * @return PB_RC_OK if successful, or an error code.
 */
typedef pb_rc_t pb_identifierI_identify_template_rank_fn_t(pb_identifier_t* identifier,
                                                           const pb_template_t* template_,
                                                           const pb_finger_t* finger,
                                                           uint8_t rank,
                                                           pb_finger_t* identified_fingers[],
                                                           uint16_t scores[],
                                                           void* references[],
                                                           pb_alignment_t* alignments[]);

/** Function callback for a identifier listener. Will be called for each match
 * in the identification process. The listener may e.g. be used to display
 * a progress bar of the identification process.
 *
 * @param[in] identifier is the identifier object.
 * @param[in] finger is the finger that was matched against.
 * @param[in] score is the score of the match.
 * @param[in] progress is the progress of the identification, in percent.
 * @param[in] context is the context set when registering the
 *     listeners.
 */
typedef void pb_identifierI_listener_fn_t(pb_identifier_t* identifier,
                                          const pb_finger_t* finger,
                                          uint32_t score,
                                          uint8_t progress,
                                          const void* context);

/** Registers a listener to the identifier. The listener will be called each
 * time any of the specified identifier events (see pb_identifierI_event_t)
 * occurs.
 *
 * @param[in] identifier is the identifier object.
 * @param[in] listener is the listener to be registered.
 * @param[in] context is the context that will be passed to the callbacks.
 *
 * @return PB_RC_OK if successful, or an error code.
 */
typedef pb_rc_t pb_identifierI_register_listener_fn_t(pb_identifier_t* identifier,
                                                      pb_identifierI_listener_fn_t* listener,
                                                      const void* context);

/** Unregisters a listener from the identifier.
 *
 * @param[in] identifier is the identifier object.
 * @param[in] listener is the listener to be unregistered.
 *
 * @return PB_RC_OK if successful, or an error code.
 */
typedef pb_rc_t pb_identifierI_unregister_listener_fn_t(pb_identifier_t* identifier,
                                                        pb_identifierI_listener_fn_t* listener);

typedef void pb_identifier_delete_context_fn_t(void* context);

/** The fingerprint identifier interface allows different fingerprint
 * indentifiers to be implemented.
 *
 * The purpose of a fingerprint identifier is to efficiently find
 * the best matching finger or fingers in a large set of templates, also
 * called one to many matching, or identification.
 *
 * Different implementations supports different types of templates.
 * The application must be configured to use the appropriate matcher
 * type for the templates used.
 */
typedef struct {
    pb_identifierI_add_templates_fn_t* add_templates;
    pb_identifierI_remove_templates_fn_t* remove_templates;
    pb_identifierI_remove_all_templates_fn_t* remove_all_templates;
    pb_identifierI_identify_template_fn_t* identify_template;
    pb_identifierI_identify_template_rank_fn_t* identify_template_rank;
    pb_identifierI_register_listener_fn_t* register_listener;
    pb_identifierI_unregister_listener_fn_t* unregister_listener;
} pb_identifier_functionsI;


/** Allocates an identifier object and sets the reference counter to 1. 
 * 
 * @param[in] session is the session object. 
 * @param[in] nbr_of_worker_threads is the number of worker threads 
 *            available for the identifier by the system. The more threads
 *            that can be made available the faster an identification will be.
 * @param[in] context is the context used by an identifier implementation to
 *            store its internal data. 
 * @param[in] delete_context is a function pointer to the function used to 
 *            delete the context data. 
 *
 * @return the allocated finger object if successful, else 0. 
 */
pb_identifier_t*
pb_identifier_create (pb_session_t* session,
                      uint8_t nbr_of_worker_threads,
                      const pb_identifier_functionsI* interface_,
                      void* context,
                      pb_identifier_delete_context_fn_t* delete_context);

/** Retains the object, which means that the reference counter for the object
 * will increase by 1. The caller must make sure that _delete is being called
 * when the object is not needed anymore. Retaining a null pointer has
 * no effect. */
pb_identifier_t*
pb_identifier_retain (pb_identifier_t* identifier);

/** Decreases the reference counter for the object by 1. If the reference 
 * counter reaches 0 then the object will also be deleted. 
 * When deleted, it will also call _delete on the user object, since _retain 
 * was called on the user object when the finger was created.
 * Deleting a null pointer has no effect. */
void
pb_identifier_delete (pb_identifier_t* identifier);

/** Returns the session object. */
pb_session_t* 
pb_identifier_get_session (pb_identifier_t* identifier);

/** Returns the context. */
void* 
pb_identifier_get_context (pb_identifier_t* identifier);


/** The object methods corresponding to the interface functions defined above. 
 * Will automatically forward to interface functions set when creating the 
 * identifier. 
 * 
 * For documentation, see interface functions above. */

pb_rc_t 
pb_identifier_add_templates (pb_identifier_t* identifier,
                             pb_template_t* templates[],
                             pb_finger_t* fingers[],
                             void* references[],
                             uint16_t nbr_of_fingers);

pb_rc_t 
pb_identifier_remove_templates (pb_identifier_t* identifier,
                                const pb_finger_t* fingers[],
                                uint16_t nbr_of_fingers);

pb_rc_t 
pb_identifier_remove_all_templates (pb_identifier_t* identifier);

pb_rc_t 
pb_identifier_identify_template (pb_identifier_t* identifier,
                                 const pb_template_t* template_,
                                 const pb_finger_t* finger,
                                 pb_fpir_t false_positive_identification_rate,
                                 pb_finger_t** identified_finger,
                                 void** reference,
                                 pb_alignment_t** alignment);

pb_rc_t 
pb_identifier_identify_template_rank (pb_identifier_t* identifier,
                                      const pb_template_t* template_,
                                      const pb_finger_t* finger,
                                      uint8_t rank,
                                      pb_finger_t* identified_fingers[],
                                      uint16_t scores[],
                                      void* references[],
                                      pb_alignment_t* alignments[]);

pb_rc_t 
pb_identifier_register_listener (pb_identifier_t* identifier,
                                 pb_identifierI_listener_fn_t* listener,
                                 const void* context);

pb_rc_t 
pb_identifier_unregister_listener (pb_identifier_t* identifier,
                                   pb_identifierI_listener_fn_t* listener);

uint8_t
pb_identifier_get_nbr_of_worker_threads (pb_identifier_t* identifier);

/** Computes the FAR level corresponding to the specified FPIR level. */
pb_far_t
pb_identifier_fpir_to_far (pb_fpir_t fpir, uint32_t database_size);


#ifdef __cplusplus
}
#endif

#endif /* PB_IDENTIFIER_H */

