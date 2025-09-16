// C implementations of image processing functions

#include <stdlib.h>
#include <assert.h>
#include "imgproc.h"

// TODO: define your helper functions here

//! Given a pixel, extract its 8-bit red component (bits 24-31).
//!
//! @param pixel 32 bits representing a RGBA pixel
//! @return the red value of the pixel, 0-255 
uint32_t get_r( uint32_t pixel ){
  uint32_t r = pixel >> 24;
  return r;
}

//! Given a pixel, extract its 8-bit green component (bits 16-23).
//!
//! @param pixel 32 bits representing a RGBA pixel
//! @return the green value of the pixel, 0-255 
uint32_t get_g( uint32_t pixel ){
  uint32_t g = (pixel << 8) >> 24;
  return g;
}

//! Given a pixel, extract its 8-bit blue component (bits 8-15).
//!
//! @param pixel 32 bits representing a RGBA pixel
//! @return the blue value of the pixel, 0-255 
uint32_t get_b( uint32_t pixel ){
  uint32_t b = (pixel << 16) >> 24;
  return b;
}

//! Given a pixel, extract its 8-bit alpha component (bits 0-7).
//!
//! @param pixel 32 bits representing a RGBA pixel
//! @return the alpha value of the pixel, 0-255 
uint32_t get_a( uint32_t pixel ){
  uint32_t a = (pixel << 24) >> 24;
  return a;
}

//! Given red, green, blue, and alpha values, construct a 32-bit
//! pixel with r being in bits 24-31, b in bits 16-23, g in bits
//! 8-15, and a in bits 0-7.
//!
//! @param r a 32-bit int 0-255 representing the red component
//! @param g a 32-bit int 0-255 representing the green component
//! @param b a 32-bit int 0-255 representing the blue component
//! @param a a 32-bit int 0-255 representing the alpha component
//! @return the pixel value
uint32_t make_pixel( uint32_t r, uint32_t g, uint32_t b, uint32_t a ) {
  uint32_t pixel = 0;
  pixel += r;
  pixel = pixel << 8;
  pixel += g;
  pixel = pixel << 8;
  pixel += b;
  pixel = pixel << 8;
  pixel += a;
  return pixel;
}

//! Compute the 1D index of a pixel given an image, row, and column.
//!
//! @param img the Image
//! @param row the row location of the pixel
//! @param col the column location of the pixel
//! @return 1D location of the pixel
uint32_t compute_index( struct Image *img, int32_t row, int32_t col ) {
  return row*(img->width) + col;
}

//! Transform the color component values in each input pixel
//! by applying the bitwise complement operation. I.e., each bit
//! in the color component information should be inverted
//! (1 becomes 0, 0 becomes 1.) The alpha value of each pixel should
//! be left unchanged.
//!
//! @param input_img pointer to the input Image
//! @param output_img pointer to the output Image (in which the
//!                   transformed pixels should be stored)
void imgproc_complement( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
}

//! Transform the input image by swapping the row and column
//! of each source pixel when copying it to the output image.
//! E.g., a pixel at row i and column j of the input image
//! should be copied to row j and column i of the output image.
//! Note that this transformation can only be applied to square
//! images (where the width and height are identical.)
//!
//! @param input_img pointer to the input Image
//! @param output_img pointer to the output Image (in which the
//!                   transformed pixels should be stored)
//!
//! @return 1 if the transformation succeeded, or 0 if the
//!         transformation can't be applied because the image
//!         width and height are not the same
int imgproc_transpose( struct Image *input_img, struct Image *output_img ) {
  // check if transpose is possible
  if (input_img->width != input_img->height) {
    return 0;
  }

  // put transposed pixels into output_img->data
  for (int row = 0; row < output_img->height; row++) {
    for (int col = 0; col < output_img->width; col++) {
      int index_in = compute_index(input_img, col, row);
      int index_out = compute_index(output_img, row, col);
      output_img->data[index_out] = input_img->data[index_in];
    }
  }

  return 1;
}


//returns whether or not the pixel at a given row or column is within the ellipse
// @param img the image in which the pixel lies
// @param row the row at which the pixel is located
// @param col the column at which the pixel is located
// @return true or false (1 or 0) based on if the given pixel is within the defined ellipse of the image
int is_in_ellipse (struct Image *img, int32_t row, int32_t col){
  int a = img->width / 2;
  int b = img->height / 2;

  int x = a - row;
  int y = b - col;

  return ((10000 * x * x) / (a * a) + ((10000 * y * y) / (b * b)) <= 10000);
}
//! Transform the input image by copying only those pixels that are
//! within an ellipse centered within the bounds of the image.
//! Pixels not in the ellipse should be left unmodified, which will
//! make them opaque black.
//!
//! Let w represent the width of the image and h represent the
//! height of the image. Let a=floor(w/2) and b=floor(h/2).
//! Consider the pixel at row b and column a is being at the
//! center of the image. When considering whether a specific pixel
//! is in the ellipse, x is the horizontal distance to the center
//! of the image and y is the vertical distance to the center of
//! the image. The pixel at the coordinates described by x and y
//! is in the ellipse if the following inequality is true:
//!
//!   floor( (10000*x*x) / (a*a) ) + floor( (10000*y*y) / (b*b) ) <= 10000
//!
//! @param input_img pointer to the input Image
//! @param output_img pointer to the output Image (in which the
//!                   transformed pixels should be stored)
void imgproc_ellipse( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement

  for (int row = 0; row < output_img->height; row++) {
    for (int col = 0; col < output_img->width; col++) {
      int index = compute_index(input_img, row, col);
      if (is_in_ellipse(input_img, row, col)){
	output_img->data[index] = input_img->data[index];
      }
      else {
	output_img->data[index] = make_pixel(255, 255, 255, 255); //sets pixels outside of the ellipse to fully black
      }
    } 
  }
  return;
}

//! Transform the input image using an "emboss" effect. The pixels
//! of the source image are transformed as follows.
//!
//! The top row and left column of pixels are transformed so that their
//! red, green, and blue color component values are all set to 128,
//! and their alpha values are not modified.
//!
//! For all other pixels, we consider the pixel's color component
//! values r, g, and b, and also the pixel's upper-left neighbor's
//! color component values nr, ng, and nb. In comparing the color
//! component values of the pixel and its upper-left neighbor,
//! we consider the differences (nr-r), (ng-g), and (nb-b).
//! Whichever of these differences has the largest absolute value
//! we refer to as diff. (Note that in the case that more than one
//! difference has the same absolute value, the red difference has
//! priority over green and blue, and the green difference has priority
//! over blue.)
//!
//! From the value diff, compute the value gray as 128 + diff.
//! However, gray should be clamped so that it is in the range
//! 0..255. I.e., if it's negative, it should become 0, and if
//! it is greater than 255, it should become 255.
//!
//! For all pixels not in the top or left row, the pixel's red, green,
//! and blue color component values should be set to gray, and the
//! alpha value should be left unmodified.
//!
//! @param input_img pointer to the input Image
//! @param output_img pointer to the output Image (in which the
//!                   transformed pixels should be stored)
void imgproc_emboss( struct Image *input_img, struct Image *output_img ) {
  // Set top row rgb values to 128
  for (int col = 0; col < input_img->width; col++) {
    uint32_t a = get_a(input_img->data[col]);
    uint32_t pixel = make_pixel(128, 128, 128, a);
    output_img->data[col] = pixel;
  }

  // Set left column rgb values to 128
  for (int row = 0; row < input_img->height; row++) {
    int index = compute_index(output_img, row, 0);
    uint32_t a = get_a(input_img->data[index]);
    uint32_t pixel = make_pixel(128, 128, 128, a);
    output_img->data[index] = pixel;
  }

  // calculate each pixel's value and store it in output_img->data
  for (int row = 1; row < input_img->height; row++) {
    for (int col = 1; col < input_img->width; col++) {
      // get current pixel info
      int index = compute_index(output_img, row, col);
      uint32_t r = get_r(input_img->data[index]);
      uint32_t g = get_g(input_img->data[index]);
      uint32_t b = get_b(input_img->data[index]);

      // get upper left pixel info
      int ul_index = compute_index(output_img, row - 1, col - 1);
      uint32_t nr = get_r(input_img->data[ul_index]);
      uint32_t ng = get_g(input_img->data[ul_index]);
      uint32_t nb = get_b(input_img->data[ul_index]);

      // calculate embossed effect
      int diffR = (int)nr - (int)r;
      int diffG = (int)ng - (int)g;
      int diffB = (int)nb - (int)b;
      int diff = diffR;
      if (abs(diffG) > abs(diff)) {
        diff = diffG;
      } else if (abs(diffB) > abs(diff)) {
        diff = diffB;
      }
      
      // apply embossed effect
      int val = diff + 128;
      if (val > 255) {
        val = 255;
      } else if (val < 0) {
        val = 0;
      }
      uint32_t pixel = make_pixel((uint32_t)val, (uint32_t)val, (uint32_t)val, get_a(input_img->data[index]));
      output_img->data[index] = pixel;
    }
  }
}
