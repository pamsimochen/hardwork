/** ==================================================================
 *  @file   jpeg_test_dec.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/test/simcop/test_algo/jpeg_dec/jpeg_test/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "jpeg_test_dec.h"
#include "jpeg_dec.h"
/* global variable declarations */
extern Uint16 NUMMCUPERBLOCK;

Uint16 hmem[4096];

Uint16 qmem[512];

Uint8 OutBuf[INPUT_SIZE_YUV422];

Uint8 InpBuf[JPEG_BUFFER_SIZE];

#define BREAK_LONG_LINES 0

/* ===================================================================
 *  @func     write_hex_short                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void write_hex_short(char *filename, Uint16 * image, int width, int height)
{
    int x, y;

    FILE *ff;

    /* open a file for writing */
    ff = fopen(filename, "w");

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            fprintf(ff, "%04X ", image[y * width + x]);
            if (BREAK_LONG_LINES && (x % 16 == 15))
                fprintf(ff, "\n");
        }
        if (BREAK_LONG_LINES && width > 16)
            fprintf(ff, "\n");
        fprintf(ff, "\n");
    }

    fclose(ff);
}

/****************************************************************************
  parse_header
****************************************************************************/
/* ===================================================================
 *  @func     parse_header                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void parse_header(FILE * fp, short *iw, short *ih, short *yuv_mode,
                  DecTblParams * DecTblParam)
{
    int len, i, j, k;

    int h1v1, h2v2, h3v3;

    int mcu_found, parse_err;

    Uint8 marker[2], temp_str[2];

    Uint8 buffer[256];

    Uint16 dht_len;

    Uint8 dht_process;

    Uint8 *dht_data;

    Uint8 dht_tc, dht_th, size;

    Uint16 dht_ptr, dht_idx, last_k, code;

    Uint8 dht_tot_size;

    Uint8 bits[16];

    Uint8 huffsize[200];

    Uint16 huffcode[200];

    Uint16 huffcode_ladj[200];

    Uint8 log_num_entries_code[200];

    Uint8 ctl_grp[200];

    Uint8 max_size_grp[17];

    Uint8 min_code_idx_grp[17];

    Uint8 rshift_grp[17];

    Uint16 total_entries_grp[17];

    Uint16 dcd_tbl[500];                                   /* worst case is a 
                                                            * bit over
                                                            * 2*16*11 */
    Uint16 temp_ctl_tbl[17];

    Uint16 grp_start;

    Uint16 dcd_entry;

    short dcd_idx;

#define EOB_VAL 0x00
#define ZRL_VAL 0xF0

    mcu_found = parse_err = 0;

    while (!mcu_found && !feof(fp) && !parse_err)
    {
        fread(marker, sizeof(char), 2, fp);
        dht_process = 0;

        if (marker[0] != 0xFF)
            parse_err = 1;
        else
            switch (marker[1])
            {
                case MARKER_SOI:                          /* just skip */
                    break;

                case MARKER_DQT:                          /* capture qmat */
                    fread(temp_str, sizeof(char), 2, fp);
                    len = (temp_str[0] * 256) + temp_str[1] - 2;
                    fread(buffer, sizeof(char), len, fp);
                    /* if (buffer[0] == 0) memcpy(&DecTblParam->qmat[0],
                     * &buffer[1], 64); else memcpy(&DecTblParam->qmat[64],
                     * &buffer[1], 64); */
                    if (len > 66)
                    {
                        if (buffer[0] == 0)
                            memcpy(&DecTblParam->qmat[0], &buffer[1], 64);
                        if (buffer[65] == 1)
                            memcpy(&DecTblParam->qmat[64], &buffer[66], 64);
                    }

                    else
                    {
                        if (buffer[0] == 0)
                            memcpy(&DecTblParam->qmat[0], &buffer[1], 64);
                        else
                            memcpy(&DecTblParam->qmat[64], &buffer[1], 64);

                    }
                    break;

                case MARKER_SOF:                          /* capture iw, ih, 
                                                            * h1v1, h2v2,
                                                            * h3v3 */
                    fread(temp_str, sizeof(char), 2, fp);
                    len = (temp_str[0] * 256) + temp_str[1] - 2;
                    fread(buffer, sizeof(char), len, fp);
                    *ih = (buffer[1] * 256) + buffer[2];
                    *iw = (buffer[3] * 256) + buffer[4];
                    h1v1 = buffer[7];
                    h2v2 = buffer[10];
                    h3v3 = buffer[13];
                    break;

                case MARKER_SOS:                          /* skip header to
                                                            * get to MCU */
                    fread(temp_str, sizeof(char), 2, fp);
                    len = (temp_str[0] * 256) + temp_str[1] - 2;
                    fread(DecTblParam->scan_header, sizeof(char), len, fp);
                    mcu_found = 1;
                    break;

                case MARKER_DHT:                          /* capture Huffman 
                                                            * table */
                    /* JPEG is very flexible, EXIF is more restrictive. Go
                     * with EXIF. There are 4 tables within one DHT, in
                     * order of Y-DC, Y-AC, C-DC, C-AC. There can only be one 
                     * DHT per JPEG file */
                    fread(temp_str, sizeof(char), 2, fp);
                    dht_len = (temp_str[0] * 256) + temp_str[1] - 2;
                    dht_data = malloc(dht_len * sizeof(char));
                    fread(dht_data, sizeof(char), dht_len, fp);
                    dht_process = 1;
                    break;

                case MARKER_APP:                          /* just skip len + 
                                                            * contents */
                case MARKER_DRI:
                default:
                    fread(temp_str, sizeof(char), 2, fp);
                    len = (temp_str[0] * 256) + temp_str[1] - 2;
                    while (len > 0)
                    {
                        if (len > 256)
                        {
                            fread(buffer, sizeof(char), 256, fp);
                            len -= 256;
                        }
                        else
                        {
                            fread(buffer, sizeof(char), len, fp);
                            len = 0;
                        }
                    }
            }                                              /* end of
                                                            * switch(marker[1]) 
                                                            */

        if (dht_process)
        {
            dht_process = 0;

            dht_ptr = 0;
            while (dht_ptr < dht_len)
            {
                /* which Huffman table */

                dht_tc = (dht_data[dht_ptr] >> 4) & 0xF;
                dht_th = dht_data[dht_ptr] & 0xF;
                dht_idx = (dht_th & 1) * 2 + (dht_tc & 1);
                dht_ptr++;

                /* expand BITS into HUFFSIZE */

                dht_tot_size = 0;

                printf("Huffman bits for table %d: ", dht_idx);

                for (k = 0, i = 0; i < 16; i++)
                {
                    bits[i] = dht_data[dht_ptr + i];
                    dht_tot_size += bits[i];

                    for (j = 0; j < bits[i]; j++)
                        huffsize[k++] = i + 1;

                    printf("%d ", bits[i]);
                }
                printf("\n");

                dht_ptr += 16;
                huffsize[k] = 0;
                last_k = k;

                /* build HUFFCODE from HUFFSIZE */
                k = 0;
                code = 0;
                size = bits[0];

                while (huffsize[k] != 0)
                {
                    while (huffsize[k] == size)
                        huffcode[k++] = code++;

                    if (huffsize[k] != 0)
                    {
                        do
                        {
                            code = 2 * code;
                            size++;
                        }
                        while (huffsize[k] != size);
                    }
                }

                /* left-shift HUFFCODE */

                for (i = 0; i < last_k; i++)
                    huffcode_ladj[i] = ~huffcode[i] << (16 - huffsize[i]);

                /* categorize into LM0 groups, 0xxxx = 16, 10xxx = 15, etc */

                for (i = 0; i < last_k; i++)
                {
                    if (huffcode_ladj[i] == 0)
                        ctl_grp[i] = 0;
                    else if ((huffcode_ladj[i] & 0xFFFE) == 0)
                        ctl_grp[i] = 1;
                    else if ((huffcode_ladj[i] & 0xFFFC) == 0)
                        ctl_grp[i] = 2;
                    else if ((huffcode_ladj[i] & 0xFFF8) == 0)
                        ctl_grp[i] = 3;
                    else if ((huffcode_ladj[i] & 0xFFF0) == 0)
                        ctl_grp[i] = 4;
                    else if ((huffcode_ladj[i] & 0xFFE0) == 0)
                        ctl_grp[i] = 5;
                    else if ((huffcode_ladj[i] & 0xFFC0) == 0)
                        ctl_grp[i] = 6;
                    else if ((huffcode_ladj[i] & 0xFF80) == 0)
                        ctl_grp[i] = 7;
                    else if ((huffcode_ladj[i] & 0xFF00) == 0)
                        ctl_grp[i] = 8;
                    else if ((huffcode_ladj[i] & 0xFE00) == 0)
                        ctl_grp[i] = 9;
                    else if ((huffcode_ladj[i] & 0xFC00) == 0)
                        ctl_grp[i] = 10;
                    else if ((huffcode_ladj[i] & 0xF800) == 0)
                        ctl_grp[i] = 11;
                    else if ((huffcode_ladj[i] & 0xF000) == 0)
                        ctl_grp[i] = 12;
                    else if ((huffcode_ladj[i] & 0xE000) == 0)
                        ctl_grp[i] = 13;
                    else if ((huffcode_ladj[i] & 0xC000) == 0)
                        ctl_grp[i] = 14;
                    else if ((huffcode_ladj[i] & 0x8000) == 0)
                        ctl_grp[i] = 15;
                    else
                        ctl_grp[i] = 16;
                }

                /* default longest code within each group */
                max_size_grp[0] = 16;

                for (i = 1; i < 17; i++)
                    max_size_grp[i] = 17 - i;

                /* longest code within each group = last one in the group */
                /* this sets left-shift for group */
                for (i = 0; i < last_k; i++)
                    max_size_grp[ctl_grp[i]] = huffsize[i];

                /* num entries for a code = 2^(max_size_grp - huffsize) */
                for (i = 0; i < last_k; i++)
                    log_num_entries_code[i] =
                        max_size_grp[ctl_grp[i]] - huffsize[i];

                /* total num entries for a group = 2^(max_size_grp - 17-grp) */
                total_entries_grp[0] = 1 << (max_size_grp[0] - 16);

                for (i = 1; i < 17; i++)
                    total_entries_grp[i] = 1 << (max_size_grp[i] - 17 + i);

                /* min code index for each group */
                for (i = last_k - 1; i >= 0; i--)
                    min_code_idx_grp[ctl_grp[i]] = i;

                /* shift = 16 - max_size_grp */
                if (dht_tc)
                    for (i = 0; i < 17; i++)
                        rshift_grp[i] = 16 - max_size_grp[i];
                else
                    for (i = 0; i < 17; i++)
                        rshift_grp[i] = 12 - max_size_grp[i];

                /* fill control and decode tables */

                /* last entry = first coded group, first fill non-coded
                 * groups */

                k = ctl_grp[last_k - 1];
                dcd_idx = 0;
                dcd_tbl[dcd_idx++] = 0;                    /* 0 = no valid
                                                            * code = error */

                for (i = 0; i < k; i++)
                    temp_ctl_tbl[i] = (16 << 11) | 0;      /* rshift 16-bit,
                                                            * offset 0
                                                            * (error) */

                /* for each group, fill from min_code, and fill in error for
                 * un-utilized code (only possible in the first coded group) */

                do                                         /* fill backward,
                                                            * as dcd_tbl is
                                                            * indexed with
                                                            * inverted bits
                                                            * data */
                {
                    /* dcd_tbl indexing adds total_entries_grp, so need to
                     * adjust offset down */
                    temp_ctl_tbl[k] =
                        (rshift_grp[k] << 11) | (dcd_idx -
                                                 total_entries_grp[k]);

                    grp_start = dcd_idx;
                    dcd_idx = dcd_idx + total_entries_grp[k] - 1;

                    for (j = min_code_idx_grp[k]; ctl_grp[j] == k; j++)
                    {
                        code = dht_data[dht_ptr + j];
                        dcd_entry = (huffsize[j] << 11) | (code & 0x0F << 4)
                            | (code & 0xF0 >> 4);

                        if (dht_tc && code == EOB_VAL)
                            dcd_entry |= 0x100;
                        else if (dht_tc && code == ZRL_VAL)
                            dcd_entry |= 0x200;

                        for (i = 0; i < (1 << log_num_entries_code[j]); i++)
                            dcd_tbl[dcd_idx--] = dcd_entry;
                    }

                    for (; dcd_idx >= grp_start; dcd_idx--)
                        dcd_tbl[dcd_idx] = 0;

                    dcd_idx = grp_start + total_entries_grp[k];
                }
                while (++k <= 16);

                if (dht_idx == 0)
                {
                    DecTblParam->dc_y_dcd_size = dcd_idx;
                    // DecTblParam->dc_y_dcd_tbl= malloc(dcd_idx *
                    // sizeof(short));
                    memcpy(DecTblParam->dc_y_dcd_tbl, dcd_tbl,
                           dcd_idx * sizeof(short));
                    memcpy(&DecTblParam->ctl_tbl[0], &temp_ctl_tbl[4],
                           13 * sizeof(short));
                }
                else if (dht_idx == 1)
                {
                    DecTblParam->ac_y_dcd_size = dcd_idx;
                    // DecTblParam->ac_y_dcd_tbl = malloc(dcd_idx *
                    // sizeof(short));
                    memcpy(DecTblParam->ac_y_dcd_tbl, dcd_tbl,
                           dcd_idx * sizeof(short));
                    memcpy(&DecTblParam->ctl_tbl[13], &temp_ctl_tbl[0],
                           17 * sizeof(short));
                }
                else if (dht_idx == 2)
                {
                    DecTblParam->dc_uv_dcd_size = dcd_idx;
                    // DecTblParam->dc_uv_dcd_tbl = malloc(dcd_idx *
                    // sizeof(short));
                    memcpy(DecTblParam->dc_uv_dcd_tbl, dcd_tbl,
                           dcd_idx * sizeof(short));
                    memcpy(&DecTblParam->ctl_tbl[30], &temp_ctl_tbl[4],
                           13 * sizeof(short));
                }
                else
                {
                    DecTblParam->ac_uv_dcd_size = dcd_idx;
                    // DecTblParam->ac_uv_dcd_tbl = malloc(dcd_idx *
                    // sizeof(short));
                    memcpy(DecTblParam->ac_uv_dcd_tbl, dcd_tbl,
                           dcd_idx * sizeof(short));
                    memcpy(&DecTblParam->ctl_tbl[43], &temp_ctl_tbl[0],
                           17 * sizeof(short));
                }

                dht_ptr += dht_tot_size;

            }
        /*** while (dht_ptr < dht_len) ***/

        }

    }                                                      /* end of while
                                                            * (!mcu_found &&
                                                            * !feof(fp) &&
                                                            * !parse_err) */

    if (parse_err)
    {
        printf("Parse_header: parse error, marker = %02x %02x\n",
               marker[0], marker[1]);
        exit(-1);
    }

    if (h1v1 == 0x22 && h2v2 == 0x11 && h3v3 == 0x11)
        *yuv_mode = 420;
    else if (h1v1 == 0x21 && h2v2 == 0x11 && h3v3 == 0x11)
        *yuv_mode = 422;
    else if (h1v1 == 0x11 && h2v2 == 0x11 && h3v3 == 0x11)
        *yuv_mode = 444;
    else
    {
        printf("Parse_header: yuv mode error, h1v1=%d, h2v2=%d, h3v3=%d\n",
               h1v1, h2v2, h3v3);
        exit(-1);
    }
    free(dht_data);
    printf("Parse_header: yuv_mode=%d, iw=%d, ih=%d\n", *yuv_mode, *iw, *ih);
    printf("Huffman decode table size = %d + %d + %d + %d = %d\n",
           DecTblParam->dc_y_dcd_size, DecTblParam->ac_y_dcd_size,
           DecTblParam->dc_uv_dcd_size, DecTblParam->ac_uv_dcd_size,
           DecTblParam->dc_y_dcd_size + DecTblParam->ac_y_dcd_size
           + DecTblParam->dc_uv_dcd_size + DecTblParam->ac_uv_dcd_size);
}

/* ===================================================================
 *  @func     main                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int main()
{
    JPEGParams *Jpegconfig;

    DecTblParams *DecTblConfig;

    FILE *finp, *foup;

    char fin_name[100], fout_name[100], buffer[10];

    Int16 i, k, iw, ih, yuv_mode;

    long bitstream_start, bitstream_size;

    Jpeg_enc_status status = JPEG_ENC_SOK;

    Jpegconfig = (JPEGParams *) malloc(sizeof(JPEGParams));
    DecTblConfig = (DecTblParams *) malloc(sizeof(DecTblParams));
    DecTblConfig->ctl_tbl = malloc(60 * sizeof(short));
    DecTblConfig->dc_y_dcd_tbl = malloc(14 * sizeof(short));
    DecTblConfig->ac_y_dcd_tbl = malloc(182 * sizeof(short));
    DecTblConfig->dc_uv_dcd_tbl = malloc(14 * sizeof(short));
    DecTblConfig->ac_uv_dcd_tbl = malloc(182 * sizeof(short));

    strcpy(fin_name, INPUT_PATH);
    strcat(fin_name, INPUT_IMAGE);
    // read_hex_short(fin_name, (Uint16 *)Jpegconfig->pInput,
    // Jpegconfig->uImageWidth,Jpegconfig->uImageHeight);
    finp = fopen(fin_name, "rb");
    JPEG_EXIT_IF((finp == NULL), JPEG_ENC_FAILED);

    strcpy(fout_name, OUTPUT_PATH);
    strcat(fout_name, INPUT_IMAGE);
    strcat(fout_name, ".yuv");
    foup = fopen(fout_name, "wb");
    parse_header(finp, &iw, &ih, &yuv_mode, DecTblConfig);
    if (DecTblConfig->scan_header[0] != 3)
        Jpegconfig->Isplanar = TRUE;
    else
        Jpegconfig->Isplanar = FALSE;

    for (i = 0; i < 128; i++)
        qmem[i] = DecTblConfig->qmat[i];

    for (i = 0; i < HMEM_SIZE; i++)
        hmem[i] = i & 0xFF;

    // memset(hmem,0,HMEM_SIZE);
    /* put control tables in settings */
    k = 0;
    for (i = 0; i < 60; i++)
        hmem[k++] = DecTblConfig->ctl_tbl[i];

    /* put decode tables in hmem */
    k = DCDTBL0OFFST;
    for (i = 0; i < DecTblConfig->dc_y_dcd_size; i++)
        hmem[k++] = DecTblConfig->dc_y_dcd_tbl[i];

    k = DCDTBL1OFFST;
    for (i = 0; i < DecTblConfig->ac_y_dcd_size; i++)
        hmem[k++] = DecTblConfig->ac_y_dcd_tbl[i];

    k = DCDTBL2OFFST;
    for (i = 0; i < DecTblConfig->dc_uv_dcd_size; i++)
        hmem[k++] = DecTblConfig->dc_uv_dcd_tbl[i];

    k = DCDTBL3OFFST;
    for (i = 0; i < DecTblConfig->ac_uv_dcd_size; i++)
        hmem[k++] = DecTblConfig->ac_uv_dcd_tbl[i];

    for (i = 0; i < 1592; i++)
        hmem[k++] = 0;

    // write_hex_short("dbg_qmem.hex", qmem, 8, QMEM_SIZE/8);
    // write_hex_short("dbg_hmem.hex", hmem, 8, HMEM_SIZE/8);
    Jpegconfig->pInput = InpBuf;                           // Jpeg buffer
    Jpegconfig->pOutput = OutBuf;

    Jpegconfig->nOupBufsize = JPEG_BUFFER_SIZE;
    Jpegconfig->pQuantTab = qmem;                          /* Quantization
                                                            * tables */
    Jpegconfig->pHuffTab = hmem;                           /* Huffman tables */
    if (yuv_mode == 420)
        Jpegconfig->Format = YUV420;
    else
        Jpegconfig->Format = YUV422;

    Jpegconfig->uImageHeight = ih;
    Jpegconfig->uImageWidth = iw;

    bitstream_start = ftell(finp);
    fseek(finp, 0, SEEK_END);
    bitstream_size = ftell(finp) - bitstream_start + 1;
    fseek(finp, -(bitstream_size - 1), SEEK_END);
    fread(InpBuf, sizeof(char), bitstream_size, finp);
    fclose(finp);
    Jpegconfig->nOupBufsize = bitstream_size;
    /* open and gethandle JPEG_init_decode();
     * JPEG_EXIT_IF((JPEG_config_decode(Jpegconfig))!=JPEG_ENC_SOK,JPEG_ENC_INVPARAMS);
     * /* kick start SIMCOP for the Intended functionality set by JPEG_config() *
     * JPEG_EXIT_IF((JPEG_run_decode(Jpegconfig))!=JPEG_ENC_SOK,JPEG_ENC_FAILED);
     * JPEG_deinit_decode(); */

    // open and gethandle
    JPEG_init_decode();
#if !PIPELINE
    JPEG_EXIT_IF((JPEG_config_decode(Jpegconfig)) != JPEG_ENC_SOK,
                 JPEG_ENC_INVPARAMS);
    JPEG_EXIT_IF((JPEG_run_decode(Jpegconfig)) != JPEG_ENC_SOK,
                 JPEG_ENC_FAILED);
#else
    if (Jpegconfig->Isplanar == FALSE)
    {
        Jpegconfig->PipelineStage = PIPE_UP;
        JPEG_EXIT_IF((JPEG_config_decode(Jpegconfig)) != JPEG_ENC_SOK,
                     JPEG_ENC_INVPARAMS);
        JPEG_EXIT_IF((JPEG_run_decode(Jpegconfig)) != JPEG_ENC_SOK,
                     JPEG_ENC_FAILED);
        Jpegconfig->PipelineStage = PIPE;
        JPEG_EXIT_IF((JPEG_config_decode(Jpegconfig)) != JPEG_ENC_SOK,
                     JPEG_ENC_INVPARAMS);
        JPEG_EXIT_IF((JPEG_run_decode(Jpegconfig)) != JPEG_ENC_SOK,
                     JPEG_ENC_FAILED);
        Jpegconfig->PipelineStage = PIPE_DOWN;
        JPEG_EXIT_IF((JPEG_config_decode(Jpegconfig)) != JPEG_ENC_SOK,
                     JPEG_ENC_INVPARAMS);
        JPEG_EXIT_IF((JPEG_run_decode(Jpegconfig)) != JPEG_ENC_SOK,
                     JPEG_ENC_FAILED);

    }

    else
    {
        for (Jpegconfig->YuvComp = Y; Jpegconfig->YuvComp <= V;
             Jpegconfig->YuvComp++)
        {
            Jpegconfig->PipelineStage = PIPE_UP;
            JPEG_EXIT_IF((JPEG_config_decode(Jpegconfig)) != JPEG_ENC_SOK,
                         JPEG_ENC_INVPARAMS);
            JPEG_EXIT_IF((JPEG_run_decode(Jpegconfig)) != JPEG_ENC_SOK,
                         JPEG_ENC_FAILED);
            Jpegconfig->PipelineStage = PIPE;
            JPEG_EXIT_IF((JPEG_config_decode(Jpegconfig)) != JPEG_ENC_SOK,
                         JPEG_ENC_INVPARAMS);
            JPEG_EXIT_IF((JPEG_run_decode(Jpegconfig)) != JPEG_ENC_SOK,
                         JPEG_ENC_FAILED);
            Jpegconfig->PipelineStage = PIPE_DOWN;
            JPEG_EXIT_IF((JPEG_config_decode(Jpegconfig)) != JPEG_ENC_SOK,
                         JPEG_ENC_INVPARAMS);
            JPEG_EXIT_IF((JPEG_run_decode(Jpegconfig)) != JPEG_ENC_SOK,
                         JPEG_ENC_FAILED);

        }

    }
#endif
    JPEG_deinit_decode();
    if (yuv_mode == 420)
        fwrite(Jpegconfig->pOutput, sizeof(char), (iw * ih * 3) / 2, foup);
    else if (yuv_mode == 422)
        fwrite(Jpegconfig->pOutput, sizeof(char), (iw * ih * 2), foup);
    else if (yuv_mode == 444)
        fwrite(Jpegconfig->pOutput, sizeof(char), (iw * ih * 3), foup);

    fclose(foup);

    printf("decode done");

    write_hex_short("dbg_hmem.hex", hmem, HMEM_WIDTH, HMEM_HEIGHT);

    // read_hex_short("dbg_qmem.hex", qmem,8,32);
    // read_hex_char("dbg_bsmemi.hex",Jpegconfig->pInput,8,512);
    free(DecTblConfig->dc_y_dcd_tbl);
    free(DecTblConfig->ac_y_dcd_tbl);
    free(DecTblConfig->dc_uv_dcd_tbl);
    free(DecTblConfig->ac_uv_dcd_tbl);
    free(DecTblConfig->ctl_tbl);
    free(DecTblConfig);
    free(Jpegconfig);

  EXIT:
    fprintf(stderr, "Jpeg Encode %s", (status == 1) ? " " : "Failed to ");
}
