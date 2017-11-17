/*
 * Copyright (c) 2015 - 2016, Precise Biometrics AB
 * All rights reserved.
 *
 */


/**
 * This algorithm can be used for offline evaluation and testing of
 * the HybridMOC solution. It acts as an BMF algorithm and e.g. can be
 * from PerfEval as verification only algorithm.
 *
 * Actual verifier front end for a real smartcard/SE based
 * implementation requires a smartcard interface such as PC/SC and
 * application specific APDU commands.
 *
 * To find the required Hybrid MOC encoder functions for a real deployment
 * go to pb_hybrid_moc_encoder.h.
 *
 * This front-end verifier accepts EHM multi-templates/single-templates
 * as enroll and verify templates and will automatically use the hybrid
 * moc encoder functions to convert these into the appropriate format before
 * calling the low level hybrid moc implementation.
 *
 * The following session parameters are relevant to use the front-end.
 *
 *    pb.hmoc.model    - Required, must be set. See list of
 *                       suppored models in the pb_hybrid_moc_encoder.h
 *                       file.
 *    pb.hmoc.magic    - Optional, default is to query HybridMOC for
 *                       proper value (how it has been build).
 *    pb.hmoc.size     - Finger container size (per finger).
 *                       Default is 65520 which is max size and virtually
 *                       unlimited. Setting this to a low value will
 *                       remove sub-templates that do not fit the size.
 *
 * The verifer will also pick up the settings of the 360_state,
 * which by default is enabled.
 *
 * While the Hybrid MOC verifier actually can do multi-finger matching
 * this front-end currently do not support this.
 *
 */

#ifndef PB_HMOC_ALGORITHM_H
#define PB_HMOC_ALGORITHM_H

#include "pb_algorithmI.h"

#ifdef __cplusplus
extern "C" {
#endif

extern pbif_const pb_algorithmI hmoc_algorithm;

#ifdef __cplusplus
}
#endif

#endif /* PB_HMOC_ALGORITHM_H */
