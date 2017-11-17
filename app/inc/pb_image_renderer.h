/*
 * Copyright (c) 2008 - 2014, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_IMAGE_RENDERER_H
#define PB_IMAGE_RENDERER_H

#include "pb_types.h"
#include "pb_image.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Bitmask values for special requested features.
 */
typedef enum {
    /** No special features requested (0) */
    PB_IMAGERENDERER_FEAT_NONE      =        0,


} pb_memoryr_features_t;

/**
 * Bitmask values for result.
 */
typedef enum {

    /** Success, the image should have been created but may still have
     * quality issues */
    PB_IMAGERENDERER_RES_EOK        =   0,

} pb_imagerenderer_results_t;

/** pb_imagememI:
  * Read npixels number of pixels from external memory starting with
  * the pixel at offset and write it to dest in internal memory.
  *
  * param[inout] handle is a pointer that can be used by the
  *     implementation as needed. It comes from the create() function in
  *     the image renderer that uses the interface.
  *
  * param[in] dest is an address in internal memory where data should
  *     be written.
  *
  * param[in] offset is the pixel where the read operation should
  *     start. The first pixel has offset = 0.
  *
  * param[in] npixels is the number of pixels to copy.
  */
typedef pb_rc_t pb_imagememI_read_pixels_fn_t(void *handle,
                                              uint8_t *dest,
                                              uint32_t offset,
                                              uint32_t npixles);

/** pb_imagememI:
  * Stop using the interface. No more call to read_pixels() is allowed
  * after a call to close().
  *
  * param[inout] handle is a pointer that can be used by the
  *     implementation as needed. It comes from the create() function in
  *     the image renderer that uses the interface.
  */
typedef pb_rc_t pb_imagememI_close_fn_t(void *handle);

/* The interface pb_imagememI is used by the image renderer to read
 * image data from a large external memory into a smaller internal
 * memory when needed by the algorithms.
 *
 * When the image renderer is created, the pb_imagememI implementation
 * is supplied together with a void *handle, that can be used by the
 * implementation. A number of random access calls to read_pixels()
 * will follow, ending with a call to close() when the image is
 * deleted. This is illustrated with the pseudocode below.
 *
 * pb_imagememI *imagemem_impl;
 * void *imagemem_handle;
 * int cols;
 * int rows;
 * int dpi;
 * int features;
 * int result;
 * pb_image_t *image;
 * image = pb_image_renderer_create(imagemem_impl,
 *                                  imagemem_handle,
 *                                  cols,
 *                                  rows,
 *                                  dpi,
 *                                  features,
 *                                  &result);
 *
 * pb_finger_t *finger;
 * pb_template_t *template;
 * pb_rc_t res;
 * res = embedded_extractor.extract_template(0,
 *                                           image, 
 *                                           finger,
 *                                           PB_TEMPLATE_PB_EHM,
 *                                           &template);
 * ---> read_pixels(handle, dest, offset, npixels);
 * ---> ...
 *
 * pb_image_delete(image);
 * ---> close(handle);
 */
typedef struct pb_imagememI_s {
    pb_imagememI_read_pixels_fn_t* read_pixels;
    pb_imagememI_close_fn_t* close;
} pb_imagememI;

pb_image_t* 
pb_image_renderer_create(const pb_imagememI *imagemem,
                         void* imagemem_handle,
                         int cols,
                         int rows,
                         int dpi,
                         int features,
                         int* result);

#ifdef __cplusplus
}
#endif


#endif
