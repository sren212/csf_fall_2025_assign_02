#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tctest.h"
#include "imgproc.h"

// An expected color identified by a (non-zero) character code.
// Used in the "struct Picture" data type.
struct ExpectedColor {
  char c;
  uint32_t color;
};

// Type representing a "picture" of an expected image.
// Useful for creating a very simple Image to be accessed
// by test functions.
struct Picture {
  struct ExpectedColor colors[40];
  int width, height;
  const char *data;
};

// Some "basic" colors to use in test struct Pictures.
// Note that the ranges '1'-'5', 'A'-'E', and 'P'-'T'
// are (respectively) colors 'r','g','b','c', and 'm'
// with just the red, green, and blue color component values
#define TEST_COLORS \
    { \
      { ' ', 0xFFFFFFFF }, \
      { '_', 0x000000FF }, \
      { 'r', 0xFF0000FF }, \
      { 'g', 0x00FF00FF }, \
      { 'b', 0x0000FFFF }, \
      { 'c', 0x00FFFFFF }, \
      { 'm', 0xFF00FFFF }, \
      { '1', 0xFF0000FF }, \
      { '2', 0x000000FF }, \
      { '3', 0x000000FF }, \
      { '4', 0x000000FF }, \
      { '5', 0xFF0000FF }, \
      { 'A', 0x000000FF }, \
      { 'B', 0x00FF00FF }, \
      { 'C', 0x000000FF }, \
      { 'D', 0x00FF00FF }, \
      { 'E', 0x000000FF }, \
      { 'P', 0x000000FF }, \
      { 'Q', 0x000000FF }, \
      { 'R', 0x0000FFFF }, \
      { 'S', 0x0000FFFF }, \
      { 'T', 0x0000FFFF }, \
    }

// Data type for the test fixture object.
// This contains data (including Image objects) that
// can be accessed by test functions. This is useful
// because multiple test functions can access the same
// data (so you don't need to create/initialize that
// data multiple times in different test functions.)
typedef struct {
  // smiley-face picture
  struct Picture smiley_pic;

  // original smiley-face Image object
  struct Image *smiley;

  // empty Image object to use for output of
  // transformation on smiley-face image
  struct Image *smiley_out;

  // a square image (same width/height) to use as a test
  // for the transpose transformation
  struct Picture sq_test_pic;

  // original square Image object
  struct Image *sq_test;

  // empty image for output of transpose transformation
  struct Image *sq_test_out;
} TestObjs;

// Functions to create and clean up a test fixture object
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// Helper functions used by the test code
struct Image *picture_to_img( const struct Picture *pic );
uint32_t lookup_color(char c, const struct ExpectedColor *colors);
bool images_equal( struct Image *a, struct Image *b );
void destroy_img( struct Image *img );

// Test functions
void test_complement_basic( TestObjs *objs );
void test_transpose_basic( TestObjs *objs );
void test_ellipse_basic( TestObjs *objs );
void test_emboss_basic( TestObjs *objs );
// TODO: add prototypes for additional test functions

int main( int argc, char **argv ) {
  // allow the specific test to execute to be specified as the
  // first command line argument
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  // Run tests.
  // Make sure you add additional TEST() macro invocations
  // for any additional test functions you add.
  TEST( test_complement_basic );
  TEST( test_transpose_basic );
  TEST( test_ellipse_basic );
  TEST( test_emboss_basic );

  TEST_FINI();
}

////////////////////////////////////////////////////////////////////////
// Test fixture setup/cleanup functions
////////////////////////////////////////////////////////////////////////

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof(TestObjs) );

  struct Picture smiley_pic = {
    TEST_COLORS,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };
  objs->smiley_pic = smiley_pic;
  objs->smiley = picture_to_img( &smiley_pic );

  objs->smiley_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->smiley_out, objs->smiley->width, objs->smiley->height );

  struct Picture sq_test_pic = {
    TEST_COLORS,
    12, // width
    12, // height
    "rrrrrr      "
    " ggggg      "
    "  bbbb      "
    "   mmm      "
    "    cc      "
    "     r      "
    "            "
    "            "
    "            "
    "            "
    "            "
    "            "
  };
  objs->sq_test_pic = sq_test_pic;
  objs->sq_test = picture_to_img( &sq_test_pic );
  objs->sq_test_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->sq_test_out, objs->sq_test->width, objs->sq_test->height );

  return objs;
}

void cleanup( TestObjs *objs ) {
  destroy_img( objs->smiley );
  destroy_img( objs->smiley_out );
  destroy_img( objs->sq_test );
  destroy_img( objs->sq_test_out );

  free( objs );
}

////////////////////////////////////////////////////////////////////////
// Test code helper functions
////////////////////////////////////////////////////////////////////////

struct Image *picture_to_img( const struct Picture *pic ) {
  struct Image *img;

  img = (struct Image *) malloc( sizeof(struct Image) );
  img_init( img, pic->width, pic->height );

  for ( int i = 0; i < pic->height; ++i ) {
    for ( int j = 0; j < pic->width; ++j ) {
      int index = i * img->width + j;
      uint32_t color = lookup_color( pic->data[index], pic->colors );
      img->data[index] = color;
    }
  }

  return img;
}

uint32_t lookup_color(char c, const struct ExpectedColor *colors) {
  for (int i = 0; ; i++) {
    assert(colors[i].c != 0);
    if (colors[i].c == c) {
      return colors[i].color;
    }
  }
}

// Returns true IFF both Image objects are identical
bool images_equal( struct Image *a, struct Image *b ) {
  if ( a->width != b->width || a->height != b->height )
    return false;

  for ( int i = 0; i < a->height; ++i )
    for ( int j = 0; j < a->width; ++j ) {
      int index = i*a->width + j;
      if ( a->data[index] != b->data[index] )
        return false;
    }

  return true;
}

void destroy_img( struct Image *img ) {
  if ( img != NULL )
    img_cleanup( img );
  free( img );
}

////////////////////////////////////////////////////////////////////////
// Test functions
////////////////////////////////////////////////////////////////////////

void test_complement_basic( TestObjs *objs ) {
  {
    imgproc_complement( objs->smiley, objs->smiley_out );

    int height = objs->sq_test->height;
    int width = objs->sq_test->width;

    for ( int i = 0; i < height; ++i ) {
      for ( int j = 0; j < width; ++j ) {
        int index = i*width + j;
        uint32_t pixel = objs->smiley_out->data[ index ];
        uint32_t expected_color = ~( objs->smiley->data[ index ] ) & 0xFFFFFF00;
        uint32_t expected_alpha = objs->smiley->data[ index ] & 0xFF;
        ASSERT( pixel == (expected_color | expected_alpha ) );
      }
    }
  }

  {
    imgproc_complement( objs->sq_test, objs->sq_test_out );

    int height = objs->sq_test->height;
    int width = objs->sq_test->width;

    for ( int i = 0; i < height; ++i ) {
      for ( int j = 0; j < width; ++j ) {
        int index = i*width + j;
        uint32_t pixel = objs->sq_test_out->data[ index ];
        uint32_t expected_color = ~( objs->sq_test->data[ index ] ) & 0xFFFFFF00;
        uint32_t expected_alpha = objs->sq_test->data[ index ] & 0xFF;
        ASSERT( pixel == (expected_color | expected_alpha ) );
      }
    }
  }
}

void test_transpose_basic( TestObjs *objs ) {
  struct Picture sq_test_transpose_expected_pic = {
    {
      { ' ', 0x000000ff },
      { 'a', 0xff0000ff },
      { 'b', 0xffffffff },
      { 'c', 0x00ff00ff },
      { 'd', 0x0000ffff },
      { 'e', 0xff00ffff },
      { 'f', 0x00ffffff },
    },
    12, // width
    12, // height
    "abbbbbbbbbbb"
    "acbbbbbbbbbb"
    "acdbbbbbbbbb"
    "acdebbbbbbbb"
    "acdefbbbbbbb"
    "acdefabbbbbb"
    "bbbbbbbbbbbb"
    "bbbbbbbbbbbb"
    "bbbbbbbbbbbb"
    "bbbbbbbbbbbb"
    "bbbbbbbbbbbb"
    "bbbbbbbbbbbb"
  };

  struct Image *sq_test_transpose_expected =
    picture_to_img( &sq_test_transpose_expected_pic );

  imgproc_transpose( objs->sq_test, objs->sq_test_out );

  ASSERT( images_equal( objs->sq_test_out, sq_test_transpose_expected ) );

  destroy_img( sq_test_transpose_expected );
}

void test_ellipse_basic( TestObjs *objs ) {
  struct Picture smiley_ellipse_expected_pic = {
    {
      { ' ', 0x000000ff },
      { 'a', 0x00ff00ff },
      { 'b', 0xffffffff },
      { 'c', 0x0000ffff },
      { 'd', 0xff0000ff },
      { 'e', 0x00ffffff },
      { 'f', 0xff00ffff },
    },
    16, // width
    10, // height
    "        a       "
    "    bbbbbbbbc   "
    "  dbbbdbbcbbbeb "
    " cbbbbbbbbbbbbcb"
    " cbbbbbbbbbbbbdb"
    "babbbcbbbbebbbdb"
    " bebbbaadcbbbcbb"
    " bbfbbbbbbbbebbb"
    "  bbaaaddcfebbb "
    "    bbbbbbbbb   "
  };

  struct Image *smiley_ellipse_expected =
    picture_to_img( &smiley_ellipse_expected_pic );

  imgproc_ellipse( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( objs->smiley_out, smiley_ellipse_expected ) );

  destroy_img( smiley_ellipse_expected );
}

void test_emboss_basic( TestObjs *objs ) {
  struct Picture smiley_emboss_expected_pic = {
    {
      { ' ', 0x000000ff },
      { 'a', 0x808080ff },
      { 'b', 0xffffffff },
    },
    16, // width
    10, // height
    "aaaaaaaaaaaaaaaa"
    "aaaba       baaa"
    "aaba abaabaaa aa"
    "aba aaa aa aaaba"
    "ab aaaaaaaaaaab "
    "ab aabaaaabaaab "
    "aa aaa bbba aba "
    "aaa aaa    aba a"
    "aaaabbbbbbbba aa"
    "aaaaa        aaa"
  };

  struct Image *smiley_emboss_expected =
    picture_to_img( &smiley_emboss_expected_pic );

  imgproc_emboss( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( objs->smiley_out, smiley_emboss_expected ) );

  destroy_img( smiley_emboss_expected );
}
