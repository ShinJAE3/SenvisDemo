/*
 * Copyright (c) 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_MATCH_RESULT_H
#define PB_MATCH_RESULT_H

#include "pb_types.h"
#include "pb_alignment.h"
#include "pb_template.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pb_match_result_st pb_match_result_t;

/** Releases this match result reference. */
void
pb_match_result_delete(pb_match_result_t* match_result);

/** Retains this match result reference. */
pb_match_result_t*
pb_match_result_retain(pb_match_result_t* match_result);

const pb_alignment_t* 
pb_match_result_get_alignment(const pb_match_result_t* match_result);

int
pb_match_result_get_decision(const pb_match_result_t* match_result);

int
pb_match_result_get_matching_subtemplate_index(const pb_match_result_t* match_result);

int
pb_match_result_get_matching_template_index(const pb_match_result_t* match_result);

#ifdef __cplusplus
}
#endif

#endif /* PB_MATCH_RESULT_H */
