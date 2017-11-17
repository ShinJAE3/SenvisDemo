/*
 * Copyright (c) 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_SEGMENTED_SLICEMEM_H
#define PB_SEGMENTED_SLICEMEM_H

#include "pb_slice_renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the segmented slicemem with the memory segments to be
 * used for data storage. An example using 64kB CCMRAM and 36kB static
 * RAM:
 *
 * static uint8_t* segment0 = (uint8_t*)0x10000000;
 * static uint8_t segment1[128*8*36];
 *
 * uint8_t* segments[2] = {segment0, segment1};
 * uint32_t sizes[2] = {0x10000, 128*8*36};
 * void* ss_handle = pb_ss_init(segments, sizes, 2);
 *
 * This can then be used by the Slice Renderer:
 *
 * image =
 *      pb_fpc1080_slicemem_renderer_create(&pb_segmented_slicemem,
 *                                          ss_handle,
 *                                          nslices,
 *                                          PB_SLICE_MAX_DIM_144,
 *                                          0,
 *                                          0,
 *                                          &result);
 *
 * ss_handle will be invalid to use after the image has been deleted.
 *
 * @param[in] segments is an array with pointers to memory segments.
 * @param[in] sizes is an array with segment sizes.
 * @param[in] nsegs is the number of segments to use.
 * @returns a slicemem handle to be used with Slice Renderer.
 *          0 if initialization failed.
 */
extern void* pb_ss_init(uint8_t** segments, uint32_t* sizes, uint8_t nsegs);

/**
 * Get a pointer to the memory holding a specific slice. This is the
 * address to use when data from the sensor are to be copied into the
 * segmentet slicemem.
 * 
 * @param[in] h is the slicemem handle from ss_init().
 * @param[in] offset is the slice offset starting at 0.
 * @returns a pointer to the slice.
 */
extern uint8_t* pb_ss_get_slice(void* h, uint8_t offset);

/**
 * Get the number of slices the slicemem can hold.
 * 
 * @param[in] h is the slicemem handle from ss_init().
 * @returns the capacity of the slicemem in number of slices.
 */
extern uint32_t pb_ss_get_capacity(void* h);

extern pbif_const pb_slicememI pb_segmented_slicemem;

#ifdef __cplusplus
}
#endif

#endif
