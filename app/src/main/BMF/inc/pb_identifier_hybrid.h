/*
 * Copyright (c) 2008 - 2014, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_IDENTIFIER_HYBRID_H
#define PB_IDENTIFIER_HYBRID_H

#include "pb_identifierI.h"
#include "pb_algorithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The hybrid identifier. This identifier combines the AFIS identifier and the 
  * verifier identifier to achieve good biometric performance for smaller sensor 
  * sizes using multitemplates. The fast AFIS identifier is first used to return
  * the top rank fingers, which are then sent to the slower but more powerful 
  * verifier identifier that performs the final selection. 
  *
  * The identifier can be tuned for optimal biometric performance or for fast
  * identification times based on the use case, using the configuration parameters 
  * defined below. 
  *
  * The algorithm to be used needs to be set by calling the _set_algorithm function. 
  */ 
extern pbif_const pb_identifierI hybrid_identifier;

/** The configuration parameters for the identifier. */
typedef struct pb_identifier_hybrid_config_st {
    /** The maximum number of minutiae used for each template for the first
      * identification step. Using a larger number of minutiae improves 
      * biometric performance but increases the identification time. The 
      * average amount of minutiae present in a fingerprint depending on the 
      * image size is shown below. Default 20. 
      *
      *     Image size (@ 500dpi)   The average number of minutiae
      *     250x350 (full size)     40
      *     200x200                 18
      *     150x150                 10
      *     200x100                  9
      *
      * Doubling the maximum number of minutiae will quadruple the identification 
      * time. Note that this is only true for values less than or equal to the 
      * average number of minutiae. Going above that will result in that the 
      * majority of the images will not have that many minutiae anyways and the 
      * increase in identification time will decline. 
      */
    uint8_t max_nbr_of_minutiae;

    /** The maximum rotation (in 360/256 degrees) allowed between the enrolled
      * and the verification templates. Accepting a larger rotation improves 
      * usability but increases the identification time. Default 32 (+-45 degrees). 
      *
      * Doubling the maximum rotation will double the identification time. 
      */
    uint8_t max_rotation;

    /** The rank for the first identification step. The top rank identified fingers 
      * from the first identification step (AFIS identifier) will be used as input 
      * to the second identification step (verifier identifier). Choosing a higher
      * rank will increase the identification time for the second step, and choosing
      * a too small rank will decrease biometric performance. Default 32. 
      */
    uint8_t afis_rank;

    /** If multitemplates are used for this identifier then this parameter defines
      * the number of subtemplates to be used as input to the AFIS identifier. More
      * subtemplates improves biometric performance but increases the identification 
      * time. Choosing too few subtemplates may decrease biometric performance 
      * drastically. Set to 0 to use all the available subtemplates. Setting it to
      * a value larger than the number of available subtemplates will give the same
      * result as setting it to 0. If multitemplates are not used, this parameter 
      * will have no effect. Default 0. 
      */
    uint8_t afis_nbr_of_subtemplates;

} pb_identifier_hybrid_config_t;

/** The default configuration parameters. 
  * The default parameters are optimized for good biometric performance with a database 
  * size of 10 000, a sensor size of 200x200 and a multitemplate size of 8 subtemplates. */
static const pb_identifier_hybrid_config_t pb_identifier_hybrid_default_config = {20, 32, 32, 0};

/** Set configuration parameters. Note: The default configuration parameters are set 
  * automatically, when creating the identifier. */
void
pb_identifier_hybrid_set_config (pb_identifier_t* identifier, 
                                 const pb_identifier_hybrid_config_t* config);

/** Sets the algorithm to be used when identifying. Shall be set to the same algorithm
  * that is used to extract the templates used for identifying. */
pb_rc_t
pb_identifier_hybrid_set_algorithm (pb_identifier_t* identifier, 
                                    pb_algorithm_t* algorithm);

#ifdef __cplusplus
}
#endif

#endif /* PB_IDENTIFIER_HYBRID_H */
