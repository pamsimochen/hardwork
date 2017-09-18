/* ======================================================================== */
/*                                                                          */
/*  AUTHORS                                                                 */
/*      Deepu Talla                                                         */
/*      Ching-Yu Hung                                                       */
/*                                                                          */
/*  CONTACT                                                                 */ 
/*      deepu@ti.com, (214)480-4619                                         */
/*      cy-hung@ti.com, (214)480-3634                                       */
/*                                                                          */
/*  FILE NAME                                                               */ 
/*      bmp_util.h                                                          */
/*                                                                          */ 
/*  REVISION HISTORY                                                        */ 
/*      See bmp_util.c                                                      */
/*                                                                          */ 
/*  DESCRIPTION                                                             */
/*      Header for BMP read/write utility functions                         */
/* ======================================================================== */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*  bmp_load_short takes a bmp filename, reads the bmp, and returns the
    sizes of the image as well as arrays of the rgb values */

int bmp_load_short (char *filename, short *hsize, short *vsize, unsigned short **red_out,
              unsigned short **green_out, unsigned short **blue_out );

int bmp_load_char (char *filename, short *hsize, short *vsize,
              unsigned char **red_out, unsigned char **green_out,
              unsigned char **blue_out );

int bmp_save_short (char *filename, int hsize, int vsize, unsigned short *red_in,
              unsigned short *green_in, unsigned short *blue_in, int right_shift );


int bmp_save_long (char *filename, int hsize, int vsize, long *red_in,
              long *green_in, long *blue_in, int right_shift );


int bmp_save_char(char *filename, int hsize, int vsize, 
      unsigned char *red_in, unsigned char *green_in, unsigned char *blue_in, 
      int right_shift );

void rgb_2_ycbcr422(short hsize, short vsize, 
              unsigned char *red, unsigned char *green, unsigned char *blue,
              unsigned char *y, unsigned char *cb, unsigned char *cr);
void ycbcr422_2_rgb(short hsize, short vsize, 
              unsigned char *y, unsigned char *cb, unsigned char *cr,
              unsigned char *red, unsigned char *green, unsigned char *blue);

void pack_ycbcr(int format, int iw, int ih, int y_en, int cbcr_en,
                unsigned char *y_in, unsigned char *cb_in, unsigned char *cr_in, 
                unsigned char *img_out);

void unpack_ycbcr(int format, int iw, int ih, int y_en, int cbcr_en,
                unsigned char *img_in, unsigned char *y_out, 
                unsigned char *cb_out, unsigned char *cr_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */
