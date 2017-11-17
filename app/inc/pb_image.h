/*
 * Copyright (c) 2008 - 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_IMAGE_H
#define PB_IMAGE_H

#include "pb_types.h"
#include "pb_returncodes.h"
#include "pb_image_t.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Note! It is the callers responsibility to make sure that 
 * all image input parameters are valid, i.e. not null (0). 
 * Passing a null image parameter to a function will result 
 * in a runtime error! */

/** Impression type codes. */
typedef enum pb_impression_type_e {
    PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN     = 0,
    PB_IMPRESSION_TYPE_LIVE_SCAN_ROLLED    = 1,
    PB_IMPRESSION_TYPE_NONLIVE_SCAN_PLAIN  = 2,
    PB_IMPRESSION_TYPE_NONLIVE_SCAN_ROLLED = 3,
    PB_IMPRESSION_TYPE_SWIPE               = 8,
    PB_IMPRESSION_TYPE_UNKNOWN             = 29
} pb_impression_type_t;

/** Flip type codes. */
typedef enum pb_flip_e {
    PB_FLIP_NONE = 0,
    PB_FLIP_HORIZONTALLY = 1,
    PB_FLIP_VERTICALLY = 2,
    PB_FLIP_ON_BOTH_SIDES = (PB_FLIP_HORIZONTALLY | PB_FLIP_VERTICALLY),
    PB_FLIP_90_CLOCKWISE = 4,
    PB_FLIP_90_COUNTERCLOCKWISE = 5
} pb_flip_t;

/**
 * Function prototype belonging to the pb_image_renderI interface
 * which may be used as an alternative pixel source. This function can
 * be viewed as a plug-in replacement of the pb_image_create() function
 * and hence parameters that are needed by the pb_image_t object must
 * be sent back through the output parameters of this function such
 * as numbers of rows, cols and image resolution.
 *
 * The data passed on to the renderer as arg1-arg4 is individual to
 * each implementation. Arg1 is typically suited for passing in memory
 * location to data that is to be rendered. The renderer should make a
 * copy if the data it needs. Arg2 to arg4 may be used as suited such
 * as passing size information and options. Arg4 is a pointer which
 * e.g. can be used to pass in feature requests and receive status
 * information e.g. if the initialization fails.
 *
 * @param arg1[in] Generic parameter passed on from the
 *        pb_image_create_render function to the renderer at
 *        initialization time. The purpose and function of this
 *        value depends on the renderer used. The typical use is
 *        that it points to a data area that the renderer uses to
 *        produce the image pixels from. It may even be much
 *        simpler such a pointer to a PNG file name (but that is
 *        to stretch the intended usage a bit far). The needed
 *        data must be copied.
 * @param arg2[in] Generic parameter, but is typically a size
 *        parameter of the data passed to arg1.
 * @param arg3[in] Generic parameter.
 * @param arg4[in/out] Generic parameter, can be used to pass
 *        additional information in or out of the renderer.
 * @param handle[out] The renderer should set this to a value that
 *        will be used in future calls to the render and delete
 *        functions of the renderer interface.
 * @param rows[out] The render must specify the number of rows in the
 *        rendered image.
 * @param cols[out] The render must specify the number of cols in the
 *        rendered image.
 * @param vertical_resolution[out]
 *        The image resolution in dpi.
 * @param horizontal_resolution[out]
 *        The image resolution in dpi.
 * @param impression_type[out]
 *        The impression type of the image, see PB_IMPRESSION_TYPE_X. 
 */
typedef pb_rc_t pb_image_renderI_init_fn(void* arg1, int arg2, int arg3, int* arg4,
                                         void** handle,
                                         uint16_t* rows,
                                         uint16_t* cols,
                                         uint16_t* vertical_resolution,
                                         uint16_t* horizontal_resolution,
                                         pb_impression_type_t* impression_type);

/**
 * Function prototype belonging to the pb_image_renderI interface
 * which may be uses as an alternative pixel source. This function is
 * called when the pb_image_t which the renderer belongs to is deleted
 * (reference count drops to 0). This is a hook for the renderer to
 * clean up any allocated resources it holds.
 *
 * @param[in] handle The handle returned from the init function.
 */
typedef void pb_image_renderI_delete_fn(void* handle);

/**
 * Function prototype belonging to the pb_image_renderI interface
 * which may be uses as an alternative pixel source. This function is
 * called whenever a request is made on the pb_image_t object for
 * pixel data. 
 * 
 * The renderer is connected only to the original created image
 * object. Primitives such as pb_image_copy() that creates derived
 * pb_image_t objects are regular image objects with a rendered pixel
 * blob. When the entire pixel blob is requested this data is cached
 * and the renderer is deleted.
 *
 * Note that only the only function not requesting full pixel data is
 * the pb_image_crop().
 *
 * The renderer will never be asked to render data from outside the
 * backing image size as set in the init function, hence no additional
 * boundary checks are needed.
 *
 * @param[in] handle The handle returned from the init function.
 * @param[in] offset_row The row offset to start render from.
 * @param[in] offset_col The column offset to start render from.
 * @param[in] rows The number of rows to be rendered.
 * @param[in] cols The number of columns to be rendered.
 * @param[in] stride The number of bytes to advance one row
 *            in the pixel buffer.
 * @parm[out] pixels The buffer to put render output in. The
 *            first pixel at (offset_row, offset_cols) goes into pixels[0]
 *            The 10:th pixel of the third row goes into pixels[2*stride+9]
 *            etc.
 */
typedef void pb_image_renderI_render_fn(void* handle,
                                        uint16_t offset_row,
                                        uint16_t offset_col,
                                        uint16_t rows,
                                        uint16_t cols,
                                        uint16_t stride,
                                        uint8_t* pixels);

/**
 * The pb_image_t render interface allows extending the pb_image_create
 * functions with a application dependent pixel source implementation
 * but still accessed through the standardized pb_image object primitives.
 *
 * For more description see the documentation of the individual function
 * types and the pb_image_create_render() function.
 *
 *  init   - called at creation time
 *  delet  - called when the pb_image object is reclaimed
 *  render - called to render pixels.
 */
typedef struct {
    pb_image_renderI_init_fn*   init;
    pb_image_renderI_delete_fn* delet;
    pb_image_renderI_render_fn* render;
} pb_image_renderI;


/** Allocates an image object, with gray-level 8-bit pixel values, and 
  * sets the reference counter to 1.
  *
  * @param[in] rows is the number of rows (height) of the image. 
  * @param[in] cols is the number of columns (width) of the image. 
  * @param[in] vertical_resolution is the vertical resolution of the
  *     image, in dpi. 
  * @param[in] horizontal_resolution is the horizontal resolution of 
  *     the image, in dpi. 
  * @param[in] pixels is the pixel buffer of the image. The pixels 
  *     are ordered so that pixels[r * cols + c] returns 
  *     the c:th pixel on the r:th row. The pixels have 
  *     values ranging from 0 (black) to 255 (white). If 0,
  *     then a completely white image is created.
  * @param[in] impression_type is the impression type of the image, 
  *     see PB_IMPRESSION_TYPE_X. 
  *
  * @return the allocated image object if successful, else 0.
  */
pb_image_t* 
pb_image_create (uint16_t rows,
                 uint16_t cols,
                 uint16_t vertical_resolution,
                 uint16_t horizontal_resolution,
                 const uint8_t* pixels,
                 pb_impression_type_t impression_type);


/** Allocates an image object, with gray-level 8-bit pixel values. This
  * create function also takes a mask that tells which part of the image 
  * that contains pixels that should be ignored when processing the image. 
  *
  * @param[in] rows is the number of rows (height) of the image. 
  * @param[in] cols is the number of columns (width) of the image. 
  * @param[in] vertical_resolution is the vertical resolution of the
  *     image, in dpi. 
  * @param[in] horizontal_resolution is the horizontal resolution of 
  *     the image, in dpi. 
  * @param[in] pixels is the pixel buffer of the image. The pixels 
  *     are ordered so that pixels[r * cols + c] returns 
  *     the c:th pixel on the r:th row. The pixels have 
  *     values ranging from 0 (black) to 255 (white). If 0,
  *     then a completely white image is created.
  * @param[in] impression_type is the impression type of the image, 
  *     see PB_IMPRESSION_TYPE_X. 
  * @param[in] mask is the mask telling which pixels to ignore when
  *            processing the image. The mask must have the same size
  *            as the pixel buffer and must only contain values of 
  *            0 (ignore) and 255 (use). Setting mask to 0 is the
  *            same as calling pb_image_create. 
  *
  * @return the allocated image object if successful, else 0. 
  */
pb_image_t* 
pb_image_create_mask (uint16_t rows,
                      uint16_t cols,
                      uint16_t vertical_resolution,
                      uint16_t horizontal_resolution,
                      const uint8_t* pixels,
                      pb_impression_type_t impression_type,
                      const uint8_t* mask);

/** Allocates an image object, with gray-level 8-bit
  * pixel values. The pixels will also be flipped according
  * to the flip parameter. This ensures that all images are
  * in an upright position, e.g. when comparing images from 
  * different sensors. An upright position is equal to the
  * orientation that a fingerprint would have when being
  * rolled on a paper (i.e. mirrored horizontally compared
  * with the physical fingerprint).
  *
  * @param[in] rows is the number of rows (height) of the image. 
  * @param[in] cols is the number of columns (width) of the image. 
  * @param[in] vertical_resolution is the vertical resolution of the
  *     image, in dpi. 
  * @param[in] horizontal_resolution is the horizontal resolution of 
  *     the image, in dpi. 
  * @param[in] pixels is the pixel buffer of the image. The image
  *     object creates a copy of the input pixels. The pixels are 
  *     ordered so that pixels[r * cols + c] returns the c:th pixel 
  *     on the r:th row. The pixels have values ranging from 0 
  *     (black) to 255 (white). If 0, then a completely white image 
  *     is created.
  * @param[in] impression_type is the impression type of the image, 
  *     see PB_IMPRESSION_TYPE_X. 
  * @param[in] flip tells how the input pixels shall be flipped to
  *     receive a fingerprint in an upright position, see PB_FLIP_X.
  *     Setting flip to PB_FLIP_NONE will give the same result as
  *     calling pb_image_create.
  *
  * @return the allocated image object if successful, else 0.
  */
pb_image_t* 
pb_image_create_flip (uint16_t rows,
                      uint16_t cols,
                      uint16_t vertical_resolution,
                      uint16_t horizontal_resolution,
                      const uint8_t* pixels,
                      pb_impression_type_t impression_type,
                      pb_flip_t flip);

/** Allocates an image object without allocating and copying pixel
  * data and instead use direct memory referencing, mr.  The
  * use-case for this alternative create function is in systems with
  * low RAM but comes with some additional complexity for an
  * application to consider such that the object and data may
  * be considered non-const despite of the declaration.
  * 
  * The pixel blob must remain valid and intact throughout the existence
  * of the image object up to its release of the pixels. A complicated
  * matter is that different parts an application may retain an object
  * for later use.
  *
  * In most simple applications there is however a bounded lifetime,
  * image data is captured, used to extract/verify and then operation
  * is ended. Hence the following scheme should be followed.
  *
  *   Capture_Sesnsor_ImageData();
  *   pb_image_t* image = pb_image_create_mr(192, 192, 500, 500,
  *                                          ImageData, _SCAN_PLAIN)
  *   extractor->extract_template(image, ....)
  *   assert(pb_image_get_num_refs(image) == 1));
  *   pb_image_detete(image); 
  *
  * See also the extended function pb_image_create_mre(), calling
  * the extended function with all extra parameters set to 0 is
  * equivalent. The extended function e.g. allows wrapping a pixel
  * blob allocated on the heap, this may in turn be usefull together
  * with the _mr_soft(), _mr_weak(), _mr_weakify() and mr_release()
  * functions.
  *
  * NOTE! The default property mr_const is 0 (false) which means
  *       that e.g. the pb_image_crop() function is allowed
  *       to retain the source image and use memory references instead
  *       of allocating a new pixel blob when the conditions allows it.
  *       If this behavior is not wanted use the _create_mre() function
  *       or change the property using _set_mr_const().
  *
  *       Also an exctractor that operates on a non-const image
  *       may release pixels early if they have been marked as weak
  *
  * @return the allocated image object if successful, else 0.
  */
pb_image_t* 
pb_image_create_mr (uint16_t rows,
                    uint16_t cols,
                    uint16_t vertical_resolution,
                    uint16_t horizontal_resolution,
                    const uint8_t* pixels,
                    pb_impression_type_t impression_type);

/** Extended version of the _create_mr() function with additional
  * functionality. Calling this function will all extra parameters
  * set to 0 is equivalent of using the _create_mr() function.
  *
  * The extended function adds a callback release function which
  * purpose is to allows using another another BMF object or heap
  * allocated memory as data source which in turn will be deleted
  * when the image object is deleted. This factious example shows
  * the idea.
  *
  *   image = SomeImage();
  *   pb_image_retain(image); // Protect from beein deleted
  *   pixels = pb_image_get_pixels(image); // Internal data
  *   slice = pb_image_create_mre(
  *               pb_image_get_rows(image),
  *               pb_image_get_cols(image),
  *               500, 500, pixels, _TYPE_UNKNOWN, 0, 0,
  *               (pb_memref_release_fn_t*) pb_image_delete, image);
  *   pb_image_delete(image); // safe as previously retained.
  *   DoOperation(slice);
  *   pb_image_delete(slice); // Will also cascade delete image
  *
  * The same mechanism can be used to prevent double allocation
  * when using heap memory that should be freed:
  *
  *   pixels = malloc(rows*cols);
  *   CaptureImage(pixels);
  *   image = pb_image_create_mre(
  *               rows, cols, 500, 500,
  *               pixels, _TYPE_UNKNOWN, 0, 0,
  *               free, pixels);
  *   pb_image_delete(image); // will free(pixels) also
  *
  * This also hooks into the early release of pixels which then may
  * lower total RAM requirements. To make this work the release
  * function must be the heap manager free() function just as in the
  * previous example.
  *
  * Note that a non-const image still must keep pixel data valid
  * (not modified) with the exception of the early release semantics.
  *
  * @param mr_size[in] If not 0 this specifies the actual memory size
  *                    of the pixel buffer and may be larger than
  *                    the size of the pixel blob. When it is larger
  *                    an implementation may take advantage of this
  *                    if the pixel data is also not write protected.
  * @param mr_const[in] If 0 code may cast away const to the image object
  *                    itself e.g. to retain it or make an early release
  *                    release of pixels, yet pixels themself must not
  *                    be changed. 
  * @param mr_release[in] Optional release callback, called when the
  *                    last reference to the image object is deleted.
  *                    On application level it is better to follow
  *                    the scheme described earlier by checking
  *                    the reference counter value. For library
  *                    development it may used to build a chain of
  *                    BMF objects.
  * @param mr_release_obj[in] Callback value passed to the optional
  *                    release function. This may e.g. be same as pixels,
  *                    but may point to a different object related to
  *                    the release of pixels.
  * 
  * @return the allocated image object if successful, else 0.
  */
pb_image_t* 
pb_image_create_mre (uint16_t rows,
                     uint16_t cols,
                     uint16_t vertical_resolution,
                     uint16_t horizontal_resolution,
                     const uint8_t* pixels,
                     pb_impression_type_t impression_type,
                     uint32_t mr_size,
                     uint32_t mr_const,
                     pb_memref_release_fn_t* mr_release,
                     void*    mr_release_obj);

/** Allocates an image object backed up by a render implementation
 * and sets the reference counter to 1.
 *
 * NOTE: This creator function is more advanced that the other image
 * creators which should be used in favor of this one when possible.
 *
 * A renderer backed image object is suited when e.g. it is not
 * permissible or practical to render a complete fingerprint image but
 * it is allowed to render small slices of it at a time that can be
 * processed individually.
 *
 * !IMPORTANT! To fully benefit from these properties the entire
 * application must be written so that the entire image is newer
 * retrieved. This means e.g. that traditional extractors and quality
 * modules cannot be used. Controllers typically can be used as is.
 * Naturally a GUI module may also not retrieve and display the entire
 * image.
 *
 * In fact all image primitives that needs pixel data except the
 * pb_image_crop() function will cause the entire pixel blob to be
 * rendered and cached inside the image object. This is required
 * due to the function and semantics of the primitives. To summarize
 * the behavior:
 *
 *  Primitive             Action
 *  ----------------------------------------------------------------
 *  pb_image_get_pixels   Render and caches all pixels, hands out
 *                        a pointer to the pixel data.
 *  => pb_image_crop      Render only the requested pixels to a
 *                        new regular image object.
 *  pb_image_copy         Render and caches all pixels and creates
 *                        a new regular image object
 *  pb_image_scale_res    Render and caches all pixels and creates
 *                        a new regular image object
 *  pb_image_scale_size   Render and caches all pixels and creates
 *                        a new regular image object
 *
 * Since the main purpose for using a render is to avoid creating the
 * whole pixel blob an application should only use the pb_image_crop()
 * function on the renderer image object itself, else the sole purpose
 * of using it is defeated.
 *
 * A specially designed extractor may e.g. be operating on an image
 * object backed by a renderer and using the crop function to create
 * successive image slices on which to operate. The individual slices
 * can then be e.g. rescaled and copied freely.
 *
 * Due to these semantics a renderer backed image object is
 * functionally compatible with normal image objects but the
 * differences behind the scene may be very important.
 *
 * The actual semantics of the function arguments depends on the
 * renderer function. See also description for pb_rc_t
 * pb_image_renderI_init_fn as function wraps that functionality but
 * returns an image on success.
 *
 * @param renderer[in] The renderer implementation used to produce
 *        the image pixels on demand. This is a set of function
 *        pointers, each separately documented.
 * @param arg1[in] Generic parameter that will be passed on to the
 *        renderer's init function. The typical use is that it points
 *        to data that the renderer uses to produce the image pixels
 *        from. The renderer will make a a copy of data it needs.
 * @param arg2[in] Generic parameter passed on to the renderer init
 *        function. This parameter is typically the data size of
 *        arg1.
 * @param arg3[in] Generic parameter passed on to the renderer init
 *        function.
 * @param arg4[in/out] Generic parameter passed on to the renderer
 *        init function. Can e.g. be used to request special rendering
 *        features if a non-null pointer is passed and return a status
 *        or failure indication.
 *
 * @return the allocated image object if successful, else 0.
 */
pb_image_t* 
pb_image_create_render (const pb_image_renderI* renderer,
                        void* arg1,
                        int   arg2,
                        int   arg3,
                        int*  arg4);

/** Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_image_t*
pb_image_retain (pb_image_t* image);

/** Decreases the reference counter for the object by 1. If the reference 
  * counter reaches 0 then the object will also be deleted. Deleting a null
  * pointer has no effect. */
void
pb_image_delete (pb_image_t* image);

/** Returns the number of references to this object. */
uint32_t pb_image_get_num_refs (const pb_image_t* image);

/** Creates a copy of the image.
 * Note that copying consumes far more resources than just retaining the
 * object. When copying a renderer backed image object a copy does
 * not clone the renderer but instead triggers a complete rendering of the
 * pixel blob.
 */
pb_image_t* 
pb_image_copy (const pb_image_t* image);

/** Returns the number of rows (height) of the image. */
uint16_t
pb_image_get_rows (const pb_image_t* image);

/** Returns the number of columns (width) of the image. */
uint16_t
pb_image_get_cols (const pb_image_t* image);

/** Returns the total number of pixels (rows * cols) of 
  * the image. */
uint32_t
pb_image_get_nbr_of_pixels (const pb_image_t* image);

/** Returns the vertical resolution of the image, in dpi. */
uint16_t
pb_image_get_vertical_resolution (const pb_image_t* image);

/** Returns the horizontal resolution of the image, in dpi. */
uint16_t
pb_image_get_horizontal_resolution (const pb_image_t* image);

/** Returns the impression type of the image, see 
  * PB_IMPRESSION_TYPE_X. */
pb_impression_type_t
pb_image_get_impression_type (const pb_image_t* image);

/** Returns a reference to the pixels of the image.
  * In case of a weak image object 0 may be returned if the
  * pixels has been released. */
const uint8_t*
pb_image_get_pixels (const pb_image_t* image);

/** Scales an image to the specified resolution. 
  * 
  * @param[in] image is the original image.
  * @param[in] vertical_resolution is the new vertical resolution
  *     of the scaled image.
  * @param[in] horizontal_resolution is the new horizontal resolution
  *     of the scaled image.
  *
  * @return a new scaled image.
  */
pb_image_t*
pb_image_scale_res (const pb_image_t* image, 
                    uint16_t vertical_resolution,
                    uint16_t horizontal_resolution);

/** Scales an image to the specified size. 
  * 
  * @param[in] image is the original image.
  * @param[in] rows is the new number of rows (height) of the 
  *     scaled image.
  * @param[in] cols is the new number of columns (width) of the 
  *     scaled image.
  *
  * @return a new scaled image.
  */
pb_image_t*
pb_image_scale_size (const pb_image_t* image, 
                     uint16_t rows,
                     uint16_t cols);

/** Like pb_image_scale_size but sets the given resolution on
  * the scaled image.
  * 
  * @param[in] image is the original image.
  * @param[in] rows is the new number of rows (height) of the 
  *     scaled image.
  * @param[in] cols is the new number of columns (width) of the 
  *     scaled image.
  * @param[in] vres is the vertical resolution to encode.
  * @param[in] hres is the horizontal resolution to encode.
  *
  * @return a new scaled image.
  */
pb_image_t*
pb_image_scale_size_res (const pb_image_t* image,
                         uint16_t rows,
                         uint16_t cols,
                         uint16_t vres,
                         uint16_t hres);

/** Crops an image to the specified size and offset.  It is possible
  * to crop to a larger image or set the crop offset in such a way
  * that the original image does not cover all parts of the cropped
  * image. In those cases, the uncovered parts will get pixel values
  * of 255 (white).
  *
  * If the mr_const property of the image is 0 (false) and the cropped
  * image lies inside image and same number of cols a mr-image is
  * created without allocating a copy of the pixels and the source
  * image will be retained until safe to be deleted. Such created
  * image slice will have property mr_const 1 (true).
  *
  * @param image is the original image.
  * @param[in] rows is the new number of rows (height) of the 
  *     cropped image.
  * @param[in] cols is the new number of columns (width) of the 
  *     cropped image.
  * @param[in] offset_rows is the row offset for the crop.
  * @param[in] offset_cols is the column offset for the crop.
  *
  * @return a new cropped image.
  */
pb_image_t*
pb_image_crop (const pb_image_t* image, 
               uint16_t rows,
               uint16_t cols,
               int32_t offset_rows,
               int32_t offset_cols);

/** Crops an image to the specified size, centered.
  * It is possible to crop to a larger image. In that 
  * case, the extended parts will get pixel values 
  * of 255 (white). 
  * 
  * @param[in] image is the original image.
  * @param[in] rows is the new number of rows (height) of the 
  *     cropped image.
  * @param[in] cols is the new number of columns (width) of the 
  *     cropped image.
  *
  * @return a new cropped image.
  */
pb_image_t*
pb_image_crop_centered (const pb_image_t* image, 
                        uint16_t rows,
                        uint16_t cols);

/** Flips the image according to the flip parameter. 
  *
  * @param[in] image is the image to flip. 
  * @param[in] flip tells how the input pixels shall be flipped to
  *     receive a fingerprint in an upright position, see PB_FLIP_X.
  *     Setting flip to PB_FLIP_NONE will give the same result as
  *     calling pb_image_create.
  *
  * @return the flipped image object if successful, else 0. The caller is 
  *         responsible of deleting the flipped image. 
  */
pb_image_t*
pb_image_flip (const pb_image_t* image, 
               pb_flip_t flip);

/** Blends an image with another image by taking the average between the two 
  * images. 
  * 
  * Pixels with values equal to 255 are considered background and will not be 
  * blended but instead replaced with the pixel value of the other image. If 
  * all pixels shall be blended, make sure that to set any 255 values to 254 
  * before calling this function. 
  *
  * @param[in] image1 is the original image.
  * @param[in] image2 is the image to blend with the original image.
  * @param[in] offset_x is the horizontal offset of image2 within image1.
  * @param[in] offset_y is the vertical offset of image2 within image1.
  *
  * @return the blended image object if successful, else 0. The size of the
  *         blended image will be equal to the size of image1. The caller is 
  *         responsible of deleting the blended image.
  */
pb_image_t*
pb_image_blend (const pb_image_t* image1, 
                const pb_image_t* image2, 
                int32_t offset_x, 
                int32_t offset_y);

/** Draws the image2 on top of image1.
  * 
  * Pixels with values equal to 255 are considered background and will not be 
  * drawn. If all pixels shall be drawn, make sure that to set any 255 values to 
  * 254 before calling this function. 
  *
  * @param[in] image1 is the original image.
  * @param[in] image2 is the image to draw on top of the original image.
  * @param[in] offset_x is the horizontal offset of image2 within image1.
  * @param[in] offset_y is the vertical offset of image2 within image1.
  *
  * @return the new image object if successful, else 0. The size of the
  *         new image will be equal to the size of image1. The caller is 
  *         responsible of deleting the new image.
  */
pb_image_t*
pb_image_draw (const pb_image_t* image1, 
               const pb_image_t* image2, 
               int32_t offset_x, 
               int32_t offset_y);

/** Rotates an image around it's origo, counter-clockwise. The new rotated image
  * will likely be larger and will receive a new origo.
  *
  * @param[in] image is the image to be rotated.
  * @param[in] rotation is the rotation of the image around it's origo, 
  *            counter-clockwise.
  * @param[out] new_origo_x is the returend x-coordinate of the new origo.
  * @param[out] new_origo_y is the returend y-coordinate of the new origo.
  *
  * @return the rotated image object if successful, else 0. The caller is 
  *         responsible of deleting the rotated image.
  */
pb_image_t* 
pb_image_rotate (const pb_image_t* image, 
                 uint8_t rotation,
                 int32_t* new_origo_x,
                 int32_t* new_origo_y);

/** Inverts an image, i.e. for each pixel changing its graylevel xi to 255 - xi.
  *
  * @param[in] image is the image to be inverted.
  *
  * @return the inverted image. The caller is responsible of deleting the 
  *         inverted image.
  */
pb_image_t* 
pb_image_invert (const pb_image_t* image);

/** Masks the image based on a certain pixel value. */
pb_image_t*
pb_image_mask (const pb_image_t* image, uint8_t mask);

/** Enhances the quality of the image, by trying to increase the contrast and 
  * remove noise. The enhanced image may be used for visualization of the
  * image for the user. 
  * Note: The enhanced image should not be used for further image processing, 
  * e.g. template extraction, since vital information may be removed during
  * the enhancement. */
pb_image_t*
pb_image_enhance_quality (const pb_image_t* image);

/** Enhances the quality of the image, by trying to increase the contrast and 
  * remove noise. The enhanced image may be used for visualization of the
  * image for the user. 
  *
  * This function is a more advanced version of the pb_image_enhance_quality
  * function. It is much slower but produces a much more noise reduced image.
  *
  * It is strongly recommended that the rows and columns of the images used are
  * multiples of 8!
  *
  * Note: The enhanced image should not be used for further image processing, 
  * e.g. template extraction, since vital information may be removed during
  * the enhancement. */
pb_image_t*
pb_image_enhance_quality_advanced (const pb_image_t* image);

#include "pb_template.h"

pb_rc_t
pb_image_encode (const pb_image_t* image,
                 pb_template_t** template_);

pb_rc_t
pb_image_decode (const pb_template_t* template_,
                 pb_image_t** image);

/** An image may include quality information such as image quality, 
  * fingerprint area and fingerprint condition. The image object does
  * not compute these values itself, instead the quality values may be
  * explicitly set using _set_quality_values. 
  * Some image operations may retain the quality values while others 
  * will discard it. The latter will be the case for e.g. scaling and 
  * cropping of an image. */
#include "pb_quality.h"

/** Sets the quality values: image quality, fingerprint area and 
  * condition of the image. */
void 
pb_image_set_quality_values (pb_image_t* image,
                             pb_quality_t* quality);

/** Returns the quality values of the image. 
  * Returns 0 if no quality values has been set. */
pb_quality_t*
pb_image_get_quality_values (const pb_image_t* image);

/** Returns the quality of the fingerprint image, ranging from 
  * 0 (worst) to 100 (best). 
  * Returns 0 if no quality values has been set. */
uint8_t
pb_image_get_quality (const pb_image_t* image);

/** Returns the area of the fingerprint in the image, in mm^2. 
  * Returns 0 if no quality values has been set. */
uint16_t 
pb_image_get_fingerprint_area (const pb_image_t* image);

/** Returns the condition of the fingerprint in the image, ranging 
  * from 0 (soaked) to 100 (bone dry).
  * Returns 0 if no quality values has been set. */
uint8_t
pb_image_get_fingerprint_condition (const pb_image_t* image);

/** Converts an image width and height to the best corresponding sensor size. */
pb_sensor_size_t
pb_image_size_to_sensor_size (uint16_t image_rows, 
                              uint16_t image_cols,
                              uint16_t vertical_resolution,
                              uint16_t horizontal_resolution);

/** Computes the corresponding sensor size. */
pb_sensor_size_t
pb_image_get_sensor_size (const pb_image_t* image);

/** Returns the size of the memory-reference mr_size attribute or
 * if 0 the actual size of the pixel blob (rows*cols). */
uint32_t pb_image_get_mr_size(const pb_image_t* image);

/** Returns the state of the memory-reference mr_cont attribute.
 * If 0 then the image object itself and in particular the pixel
 * blob may be treated as writable. */
uint32_t pb_image_get_mr_const(const pb_image_t* image);

/** Change the state of  memory-reference mr_cont attribute.
  * If 0 then the image object itself and in particular the pixel
  * blob may be treated as writable. */
void pb_image_set_mr_const(pb_image_t* image, uint32_t mr_const);

/** Makes a hard image pixel reference soft, if not marked as hard this
  * function has no effect. A soft reference has in it self no effect
  * but can be weakened via the _mr_weakify() function. The design
  * pattern of using soft reference is to enable a controller to
  * release pixels early while not disturbing those controllers who ar
  * unaware of this pattern.
  */
void pb_image_mr_soft(pb_image_t* image);

/** Makes a soft pixel reference weak, if not marked as soft this this
  * function has no effect. When pixels are made weak it is like
  * calling _mr_weak().  The design pattern for this function is to be
  * used in aware controllers that can release pixels early yet it
  * depends on the caller application to have enabled it by making
  * pixles soft (since main application may want the pixels to
  * remain).
  */
void pb_image_mr_weakify(pb_image_t* image);

/** Makes pixel references weak, unconditionally. Weakly referenced
  * pixels may be released at earliest convenience in the code that
  * follows. The _mr_release() function can only succeed when the
  * pixel reference is in this state but there are more criterias.
  * The design pattern is that only a few main components make use of
  * early release such as an extractor which then may have a chance of
  * lowering total RAM usage. Functions that do early release should
  * describe this.
  */
void pb_image_mr_weak(pb_image_t* image);

/** Check if image has a weak pixel reference. */
int pb_image_is_mr_weak(const pb_image_t* image);

/** Release the image pixels, if possible. To be released the pixels
  * must be weakly referenced but this is not the only criteria, The
  * image object itself will remain and must still be deleted but the
  * image pixel reference will become 0 if the object was actually released.
  *
  * The intended usage is is from inside an extractor where the image
  * pixels could be reused or released early. The caller must specify
  * the weak attribute of the image just prior to calling the
  * extractor and then delete the image object right after. See
  * the _mr_soft(), _mr_weakify() and _mr_weak() functions.
  *
  * This function may also request taking ownership of the pixels by
  * specify the intended release function, if ownership is granted the
  * pixels will be seen as released from the image object point of
  * view while the caller will be responsible to release the pixels
  * eventually. Getting ownership of pixels also grants the right to
  * modify the pixels before they are finally released. So if e.g. the
  * pixels has been allocated with malloc() then the free() function
  * would be used to get ownership - this is also the intended major
  * use-case.
  *
  * As a consequence of taking ownership it requires the caller to
  * first request and store the image pixels pointer.
  *
  * This function will not release unless the image != 0, image has
  * already been released (pixels==0), the image is not maked as weak,
  * the reference counter is > 1 or there is no release function
  * (possible e.g. when using create_mr()).
  *
  * In addition no ownership transfer is make if the release function
  * is not compatible or the image is renderer backed.
  *
  * The function will return
  *
  *     0 - If nothing was actually released or ownership transfer
  *         This may happen for a number of cases such as if there
  *         are several retains on the image, there is no registered
  *         release function, active renderer backed image or if
  *         an ownership take-over request is not granted.
  *
  *     1 - Pixels where released by some means or ownership transfer
  *         In the case of ownership request the pixels are now
  *         owned by the caller.
  *         If not ownership was requested it means that something was
  *         released but not necessarily finally back to the system
  *         heap as there may be a chain of releases that is not yet
  *         fully unraveled.
  *
  * NOTE: In cases where the image object is a const image this
  *       function can still be used if the object in turn has been
  *       marked as non-const using _set_mr_const() or similar which
  *       allows breaking const for this purpose.
  *
  * @param image   The image to release pixels for.
  * @param release The release function if requesting ownership transfer,
  *                else 0 should be specified.
  *
  * @returns as described above 0 is returned if nothing was released
  *          and otherwise 1 is returned.
  */
int
pb_image_mr_release (pb_image_t* image, pb_memref_release_fn_t* release);

#ifdef __cplusplus
}
#endif

#endif /* PM_IMAGE_H */
