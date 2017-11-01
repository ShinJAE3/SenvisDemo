/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_SLICE_RENDERER_H
#define PB_SLICE_RENDERER_H

/**
 * The Slice Renderer provides an alternate pb_image_t implementation
 * based on the pb_image_renderI interface.
 *
 * The intended purpose of using this alternate implementation is to
 * enable BMF applications using swipe sensors to operate on subimages
 * of data from the sensor and thus prevent a fully stitched image to
 * be handled through normal pb_image_t primitives.
 *
 * In order to be effective the application must only request
 * subimages using the pb_image_crop() function. If e.g. using the
 * pb_image_get_pixels() instead then all pixels will be rendered at
 * one time. The renderer support a feature to block such request.
 *
 * With these limitations in mind any and all existing BMF primitives
 * may be used unchanged as the image object created has the same
 * semantics as any other image object.
 *
 * This module currently supports the the following sensors:
 *   - FPC 1080A through BIR input format, 508dpi resolution
 */

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
    PB_SLICE_FEAT_NONE      =        0,

    /** Enforces a maximum rendering size of 104 lines. Use this
     * feature to ensure that no component using the image object
     * by mistake renders the whole image at once. */
    PB_SLICE_FEAT_ENFMAX    =  (1 << 0),
    
    /** Disable image enhancements. Without image enhancement the
     * fingerprint is put directly on a white background which may give
     * sharp edges. Image enhancement is recommended when image is to
     * be used e.g. by a minutia extractor. */
    PB_SLICE_FEAT_NOENH     =  (1 << 1),

    /** Enable shortening of long swipes, see
     * PB_SLICE_RES_LONG. Experimental feature. Excessive long swipes
     * takes longer to feature extract minutia from so shortening long
     * swipes may be a good solution but only if the correct part of
     * the swipe is removed. */
    PB_SLICE_FEAT_SHRINK    =  (1 << 2),

    /** Reserved, has currently no function.
     * (Intention: specify reverse swipe direction). */
    PB_SLICE_FEAT_REV       =  (1 << 3),

    /** Limit the width of the image to 132 pixels, unlimited
        length. */
    PB_SLICE_MAX_DIM_132    =  (1 << 4),

    /** Limit the width of the image to 144 pixels, unlimited
        length. */
    PB_SLICE_MAX_DIM_144    =  (1 << 5),

    /** Limit the height of the image to 350 pixels, unlimited
        width. */
    PB_SLICE_MAX_ROWS_350    =  (1 << 6),

} pb_slice_features_t;

/**
 * Bitmask values for result.
 */
typedef enum {
    /* ---- Error codes ----
     * The first 8 bits represents error codes, these are never combined
     * Swipe quality bits. */

    /** Success, the image should have been created but may still have
     * quality issues */
    PB_SLICE_RES_EOK        =   0,

    /** Unsupported BIR data. See supported sensors list above.
     * May also be caused by using a stub
     * implementation. */
    PB_SLICE_RES_ESUPPORT   =   1,

    /** Bad BIR data. The data is not well-formed or inconsistent. */
    PB_SLICE_RES_EBAD       =   2,

    /** Unreasonable short swipe, refused to be stitched, see also
     * PB_SLICE_RES_SHORT bit. */
    PB_SLICE_RES_ESHORT     =   3,
    
    /* --- Swipe quality feedback bits --- */

    /** Image is considered to short to be used, at least not for
     * enrollment purposes. */
    PB_SLICE_RES_SHORT     =   1 << 8,

    /** Image slice is quite long, swipe probably starts too high up on
     * finger. The image is still usable but better swipes can
     * probably be achieved if user is given feedback. */
    PB_SLICE_RES_LONG     =   1 << 9,

    /** The image contains several poorly overlapping slices which
     * may be due to poor image quality or to fast swipes relative
     * to the sample rate. */
    PB_SLICE_RES_OVER     =   1 << 10,

    /** The image does not contain at least one fairly long continuous
     * set of good overlapping slices. This may be due to poor image
     * quality or to fast swipe. */
    PB_SLICE_RES_DISC     =   1 << 11,

    /** The sampled slices indicate too slow swipe or too fast sample rate.
     * The driver should have a goal presenting data from samples from the
     * finger every 3-5 rows. */
    PB_SLICE_RES_SLOW     =   1 << 12,

    /** The sampled slices indicate too fast swipe
     * speed or too slow sample rate. The driver is encouraged to
     * collect samples from the finger as it moves every 3-5 rows
     * with a maximum of 6. */
    PB_SLICE_RES_FAST     =   1 << 13,

    /** The sampled slices indicate that the swipe was fairly skewed.
     * Skewed images will not be as good as straight swipes. */
    PB_SLICE_RES_SKEW     =   1 << 14,
     
} pb_slice_results_t;

/** pb_slicememI:
  * Read nslices number of slices from external memory starting with
  * slice at offset and write it to dest in internal memory.
  *
  * param[inout] handle is a pointer that can be used by the
  *     implementation as needed. It comes from the create() function in
  *     the slice renderer that uses the interface.
  *
  * param[in] dest is an address in internal memory where data should
  *     be written.
  *
  * param[in] offset is the slice where the read operation should
  *     start. The first slice has offset 0, the second has offset 1 and
  *     so on.
  *
  * param[in] nslices is the number of slices to copy.
  */
typedef pb_rc_t pb_slicememI_read_slices_fn_t(void *handle,
                                            uint8_t *dest,
                                            uint32_t offset,
                                            uint32_t nslices);

/** pb_slicememI:
  * Stop using the interface. No more call to read_slices() is allowed
  * after a call to close().
  *
  * param[inout] handle is a pointer that can be used by the
  *     implementation as needed. It comes from the create() function in
  *     the slice renderer that uses the interface.
  */
typedef pb_rc_t pb_slicememI_close_fn_t(void *handle);

/* The interface pb_slicememI is used by the slice renderer to read
 * slice data from a large external memory into a smaller internal
 * memory when needed by the algorithms.
 *
 * When the slice renderer is created, the pb_slicememI implementation
 * is supplied together with a void *handle, that can be used by the
 * implementation. A number of random access calls to read_slices()
 * will follow, ending with a call to close() when the image is
 * deleted. This is illustrated with the pseudocode below.
 *
 * pb_slicememI *slicemem_impl;
 * void *slicemem_handle;
 * int slices;
 * int features;
 * int result;
 * pb_image_t *image;
 * image = pb_fpc1080_slicemem_renderer_create(slicemem_impl,
 *                                             slicemem_handle,
 *                                             slices,
 *                                             features,
 *                                             0,
 *                                             0,
 *                                             &result);
 *
 * pb_finger_t *finger;
 * pb_template_t *template;
 * pb_rc_t res;
 * res = spectral_slice_extractor.extract_template(0,
 *                                                 image, 
 *                                                 finger,
 *                                                 PB_TEMPLATE_PB_SPECTRAL,
 *                                                 &template);
 * ---> read_slices(handle, dest, offset, nslices);
 * ---> ...
 *
 * pb_image_delete(image);
 * ---> close(handle);
 */
typedef struct pb_slicememI_s {
    pb_slicememI_read_slices_fn_t* read_slices;
    pb_slicememI_close_fn_t* close;
} pb_slicememI;

/**
 * Allocates an image object backed up by a swipe sensor stitcher for
 * BIR image format used a specific sensor type/family (see list of 
 * supported sensors above). 
 *
 * Image slices are not stitched together at creation time but the
 * slices are analyzed. The renderer stitches together the image
 * slices only when requested so through the pb_image_t primitives
 * applied to the returned image object.
 *
 * When using the pb_image_crop() function only the requested subimage
 * will be stitched together. By carefully using only the crop function
 * the complete image will never be fully reconstructed at any given
 * time.
 *
 * See the documentation for pb_image_create_render() and the renderer
 * interface for a more thorough description. 
 *
 * The input BIR format contains meta information and image slices
 * captured by a low level sensor driver. The function will return a
 * null image object if the BIR describes an unsported configuration
 * or if the image cannot be stitched to an image or if it is very
 * short (less than ~100 rows). A null-pointer can basically be
 * considered error condition.
 *
 * A successfully created image object may still represent an image of
 * poor quality. A result output parameter may carry extra quality
 * information, see PB_SLICE_RES_ constants. A fingerprint system
 * should use this data to e.g. prevent using poor images at enroll or
 * always request a new swipe directly if it is considered bad.
 *
 * A feature mask can be sent to the stitcher which is a bitmask
 * combined from the PB_SLICE_FEAT_ constants. One such feature is
 * a request to limit the renderer from ever producing more than
 * a limited number of consecutive rows of data. This may be valuable
 * to prohibit unaccidental rendering of the whole image by some
 * other component.
 *
 * The BIR has the following content (integers encoded in LSB).
 *
 * Offset   Size   Value   Description
 * ---------------------------------------------------------------------
 *      0      4           Length of entire BIR, inclusive this header
 *      4      1       1   Header version
 *      5      1           BIR data type, N/A
 *      6      2       9   Format owner
 *      8      2      22   Format type 22=line sensor
 *     10      1           Quality, N/A
 *     11      1           Purpose, N/A
 *     12      4           Factors, N/A
 *     ---------------------------------
 *     16      4      >0   Count of views, only first used
 *     ---------------------------------
 *     20      4           Number of bytes in this view, incl this header
 *     24      4       8   Bits per pixel
 *     28      4       8   Number of rows in each slice
 *     32      4           Number of columns in each slice (sensor width)
 *     36      4      >0   Number of slices
 *     40      4           Purpose, N/A
 *     44      N           Pixels, row by row, slice by slice. All in
 *                         all N=slices*rows*cols
 *
 * @param bir[in] The BIR data to be used by the renderer.
 * @param size[in] The size of the BIR data.
 * @param dpi[in] The image resolution in DPI for both horizontal and
 *                vertical resolution 
 * @param features[in] Bitmask of requested features, see constants
 *                PB_SLICE_FEAT_
 * @param result[out] If this pointer is not null it will contain a
 *                result which is bitmask of information about the
 *                data, see PB_SLICE_RES_.
 *
 * @return the allocated image object if successful, else 0. In case
 *         0 is returned result will contain an error code.
 */
pb_image_t* 
pb_fpc_renderer_create(uint8_t* bir, int size, int dpi, int features, int* result);

/**
 * Extended variant of the pb_fpc_renderer_create() function which
 * also accepts a render hook function for post image stitching
 * processing.
 *
 * The post processing hook function profile is that of the renderer
 * function defined in pb_image. The hook should be seen as a final
 * rendering step that can be done on the stitched slices just before a
 * rendered pb_image object is created.
 *
 * The renderer hook operate directly on the pixel blob. The rendering
 * is done in a canvas matching the requested size and offsets. Hence
 * the renderer hook must use the row stride parameter to index pixels
 * within the pixel blob. The renderer parameters passed to the hook
 * function is described here with details relevant for this special
 * use case, for more information see pb_image.
 *
 *  - handle       The value of the hook_handle parameter, see below.
 *                 Can be used to pass in a context for hook processing.
 *
 *  - offset_row   The requested start row offset for rendering, this
 *                 parameter may be of no interest but tells what
 *                 part is being rendered.
 *
 *  - offset_col   The requested start col offset for rendering, this
 *                 parameter may be of no interest but tells what
 *                 part is being rendered.
 *
 *  - rows         How many pixel rows is rendered in the pixel blob.
 *
 *                 NOTE: The first pixel of the first row is located in
 *                 pixels[0] and the first pixel of the second row is
 *                 located in pixels[stride].
 *
 * - cols          How many pixel cols is rendered in the pixel blob.
 *
 *                 NOTE: The last pixel of the last row is located
 *                 in pixels[(rows-1) * stride + (cols-1)].
 * 
 * - stride        The row stride step to advance to the next row.
 * 
 * - pixels        The pixel blob containing the rendered image.
 *                 The hook function may operate directly on this data
 *                 but must keep within the boundaries given by the
 *                 rows and cols dimensions.
 *
 * @param bir[in] The BIR data to be used by the renderer.
 * @param size[in] The size of the BIR data.
 * @param dpi[in] The image resolution in DPI for both horizontal and
 *                vertical resolution 
 * @param features[in] Bitmask of requested features, see constants
 *                PB_SLICE_FEAT_
 * @param result[out] If this pointer is not null it will contain a
 *                result which is bitmask of information about the
 *                data, see PB_SLICE_RES_.
 * @param hook_fn[in] The render hook function. A null pointer disables
 *                calling the renderer hook.
 * @param hook_handle[in] An optional context handle to passed on to
 *                the renderer hook function.   
 *
 * @return the allocated image object if successful, else 0. In case
 *         0 is returned result will contain an error code.
 */
pb_image_t* 
pb_fpc_renderer_create_hook(uint8_t* bir, int size, int dpi, int features, int* result,
                            pb_image_renderI_render_fn* hook_fn, void* hook_handle);

/**
 * Allocates an image object backed up by a swipe sensor stitcher for
 * a specific sensor type/family (see list of supported sensors above).
 *
 * For more information see the pb_fpc_renderer_create() function.
 *
 * @param pixels[in] The slice pixel blob. The pixel data is assumed
 *                   to be organized slice by slice where each slice
 *                   in turn is composed of 8 rows each with 128
 *                   pixels per row.
 * @param slices[in] Number of slices represented by the pixels argument.
 * @param features[in] Bitmask of requested features, see constants
 *                PB_SLICE_FEAT_
 * @param result[out] If this pointer is not null it will contain a
 *                result which is bitmask of information about the
 *                data, see PB_SLICE_RES_.
 *
 * @return the allocated image object if successful, else 0. In case
 *         0 is returned result will contain an error code.
 *
 */
pb_image_t* 
pb_fpc1080_renderer_create(uint8_t* pixels, int slices, int features, int* result);

/**
 * Corresponding to pb_fpc1080_renderer_create() but will not make a copy
 * of the pixel slices. Pixel slices must hence remain untouched until
 * the returned pb_image is deleted.
 *
 * NOTE: Using this function is syntactic sugar for the simplest use-case
 *       of pb_fpc1080_slicemem_renderer_create(), which however can
 *       be used in more complex situations e.g. where slices are not
 *       continuous in memory.
 *
 */
pb_image_t* 
pb_fpc1080_renderer_create_mr(uint8_t* pixels, int slices, int features, int* result);

/**
 * Extended variant of the pb_fpc1080_renderer_create() function which
 * also accepts a render hook function for post image stitching
 * processing.
 *
 * The post processing hook function profile is that of the renderer
 * function defined in pb_image. The hook should be seen as a final
 * rendering step that can be done on the stitched slices just before a
 * rendered pb_image object is created.
 *
 * The renderer hook operate directly on the pixel blob. The rendering
 * is done in a canvas matching the requested size and offsets. Hence
 * the renderer hook must use the row stride parameter to index pixels
 * within the pixel blob. The renderer parameters passed to the hook
 * function is described here with details relevant for this special
 * use case, for more information see pb_image.
 *
 *  - handle       The value of the hook_handle parameter, see below.
 *                 Can be used to pass in a context for hook processing.
 *
 *  - offset_row   The requested start row offset for rendering, this
 *                 parameter may be of no interest but tells what
 *                 part is being rendered.
 *
 *  - offset_col   The requested start col offset for rendering, this
 *                 parameter may be of no interest but tells what
 *                 part is being rendered.
 *
 *  - rows         How many pixel rows is rendered in the pixel blob.
 *
 *                 NOTE: The first pixel of the first row is located in
 *                 pixels[0] and the first pixel of the second row is
 *                 located in pixels[stride].
 *
 * - cols          How many pixel cols is rendered in the pixel blob.
 *
 *                 NOTE: The last pixel of the last row is located
 *                 in pixels[(rows-1) * stride + (cols-1)].
 * 
 * - stride        The row stride step to advance to the next row.
 * 
 * - pixels        The pixel blob containing the rendered image.
 *                 The hook function may operate directly on this data
 *                 but must keep within the boundaries given by the
 *                 rows and cols dimensions.
 *
 * @param pixels[in] The slice pixel blob. The pixel data is assumed
 *                   to be organized slice by slice where each slice
 *                   in turn is composed of 8 rows each with 128
 *                   pixels per row.
 * @param slices[in] Number of slices represented by the pixels argument.
 * @param features[in] Bitmask of requested features, see constants
 *                PB_SLICE_FEAT_
 * @param result[out] If this pointer is not null it will contain a
 *                result which is bitmask of information about the
 *                data, see PB_SLICE_RES_.
 * @param hook_fn[in] The render hook function. A null pointer disables
 *                calling the renderer hook.
 * @param hook_handle[in] An optional context handle to passed on to
 *                the renderer hook function.   
 *
 * @return the allocated image object if successful, else 0. In case
 *         0 is returned result will contain an error code.
 */
pb_image_t* 
pb_fpc1080_renderer_create_hook(uint8_t* pixels, int slices, int features,
                                int* result, 
                                pb_image_renderI_render_fn* hook_fn,
                                void* hook_handle);

/**
 * Corresponding to pb_fpc1080_renderer_create_hook() but will not make a copy
 * of the pixel slices. Pixel slices must hence remain untouched until
 * the returned pb_image is deleted.
 *
 * NOTE: Using this function is syntactic sugar for the simplest use-case
 *       of pb_fpc1080_slicemem_renderer_create_hook(), which however can
 *       be used in more complex situations e.g. where slices are not
 *       continuous in memory.
 */
pb_image_t* 
pb_fpc1080_renderer_create_hook_mr(uint8_t* pixels, int slices, int features,
                                   int* result, 
                                   pb_image_renderI_render_fn* hook_fn,
                                   void* hook_handle);
    
/**
 * Allocates an image object backed up by a swipe sensor stitcher for
 * a specific sensor type/family (see list of supported sensors above).
 *
 * Sensor slices are fetched over the pb_slicememI interface when
 * needed. This makes it possible to use external memory to store
 * slice data, when the internal memory is too small.
 *
 * For more information see the pb_fpc1080_renderer_create() function.
 *
 * @param slicemem[in] Pointer to slicemem implementation to use.
 * @param slicemem_handle[in] Passed on to the slicemem implementation.
 * @param slices[in] Number of slices represented by the pixels argument.
 * @param features[in] Bitmask of requested features, see constants
 *                PB_SLICE_FEAT_
 * @param slice_cache[in] Internal memory buffer to be used by the renderer
 *                to store temporary slice data. If 0, a buffer
 *                with up to slice_cache_size bytes will be allocated
 *                automatically.
 *                If slice_cache_size == 0, a buffer with 2048 bytes
 *                will be allocated.
 *                The size will determine how often
 *                slicemem->read_slices() will have to be called.
 *                It must be at least 2 slices large. (2 * 8 * 128 bytes).
 * @param slice_cache_size[in] Size of slice_cache in bytes.
 * @param result[out] If this pointer is not null it will contain a
 *                result which is bitmask of information about the
 *                data, see PB_SLICE_RES_.
 *
 * @return the allocated image object if successful, else 0. In case
 *         0 is returned result will contain an error code.
 *
 */
pb_image_t*
pb_fpc1080_slicemem_renderer_create(const pb_slicememI* slicemem,
                                    void* slicemem_handle,
                                    int slices,
                                    int features,
                                    uint8_t* slice_cache,
                                    uint32_t slice_cache_size,
                                    int* result);

/**
 * Extended variant of the pb_fpc1080_slicemem_renderer_create()
 * function which also accepts a render hook function for post image
 * stitching processing.
 *
 * The post processing hook function profile is that of the renderer
 * function defined in pb_image. The hook should be seen as a final
 * rendering step that can be done on the stitched slices just before a
 * rendered pb_image object is created.
 *
 * The renderer hook operate directly on the pixel blob. The rendering
 * is done in a canvas matching the requested size and offsets. Hence
 * the renderer hook must use the row stride parameter to index pixels
 * within the pixel blob. The renderer parameters passed to the hook
 * function is described here with details relevant for this special
 * use case, for more information see pb_image.
 *
 *  - handle       The value of the hook_handle parameter, see below.
 *                 Can be used to pass in a context for hook processing.
 *
 *  - offset_row   The requested start row offset for rendering, this
 *                 parameter may be of no interest but tells what
 *                 part is being rendered.
 *
 *  - offset_col   The requested start col offset for rendering, this
 *                 parameter may be of no interest but tells what
 *                 part is being rendered.
 *
 *  - rows         How many pixel rows is rendered in the pixel blob.
 *
 *                 NOTE: The first pixel of the first row is located in
 *                 pixels[0] and the first pixel of the second row is
 *                 located in pixels[stride].
 *
 * - cols          How many pixel cols is rendered in the pixel blob.
 *
 *                 NOTE: The last pixel of the last row is located
 *                 in pixels[(rows-1) * stride + (cols-1)].
 * 
 * - stride        The row stride step to advance to the next row.
 * 
 * - pixels        The pixel blob containing the rendered image.
 *                 The hook function may operate directly on this data
 *                 but must keep within the boundaries given by the
 *                 rows and cols dimensions.
 *
 * @param slicemem[in] Pointer to slicemem implementation to use.
 * @param slicemem_handle[in] Passed on to the slicemem implementation.
 * @param slices[in] Number of slices represented by the pixels argument.
 * @param features[in] Bitmask of requested features, see constants
 *                PB_SLICE_FEAT_
 * @param slice_cache[in] Internal memory buffer to be used by the renderer
 *                to store temporary slice data. If 0, a buffer
 *                with room for 10 slices will be allocated automatically.
 *                The size will determine how often
 *                slicemem->read_slices() will have to be called.
 *                It must be at least 2 slices large. (2 * 8 * 128 bytes).
 * @param slice_cache_size[in] Size of slice_cache in bytes.
 * @param result[out] If this pointer is not null it will contain a
 *                result which is bitmask of information about the
 *                data, see PB_SLICE_RES_.
 * @param hook_fn[in] The render hook function. A null pointer disables
 *                calling the renderer hook.
 * @param hook_handle[in] An optional context handle to passed on to
 *                the renderer hook function.   
 *
 * @return the allocated image object if successful, else 0. In case
 *         0 is returned result will contain an error code.
 */
pb_image_t*
pb_fpc1080_slicemem_renderer_create_hook(const pb_slicememI* slicemem,
                                         void* slicemem_handle,
                                         int slices,
                                         int features,
                                         uint8_t* slice_cache,
                                         uint32_t slice_cache_size,
                                         int* result,
                                         pb_image_renderI_render_fn* hook_fn,
                                         void* hook_handle);

#ifdef __cplusplus
}
#endif


#endif
