/* ======================================================================== */
/*                                                                          */
/*  AUTHORS                                                                 */
/*      Ching-Yu Hung                                                       */
/*                                                                          */
/*  CONTACT                                                                 */ 
/*      cy-hung@ti.com, (214)480-3634                                       */
/*                                                                          */
/*  FILE NAME                                                               */ 
/*      hex_util.h                                                          */
/*                                                                          */ 
/*  REVISION HISTORY                                                        */ 
/*      8-02-05 Initial version                                             */
/*                                                                          */ 
/*  DESCRIPTION                                                             */
/*      HEX image read/write utility functions header                       */
/* ======================================================================== */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void read_hex_short(char *filename,
        unsigned short *image,  /* allocated before calling this function */
        int width, int height);  /* width and height provided by caller */

void write_hex_short(char *filename,
         unsigned short *image,
         int width, int height);


void read_hex_char(char *filename,
        unsigned char *image,  /* allocated before calling this function */
        int width, int height); /* width and height provided by caller */

void write_hex_char(char *filename,
         unsigned char *image,
         int width, int height);

#ifdef __cplusplus
}
#endif /* __cplusplus */
