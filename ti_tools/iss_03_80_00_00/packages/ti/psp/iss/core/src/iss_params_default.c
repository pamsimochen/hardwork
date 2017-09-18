/** ==================================================================
 *  @file   iss_params_default.c
 *
 *  @path   /ti/psp/iss/core/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

// #include <ti/iss/drivers/hal/iss/isp/isp.h>
#include "../inc/iss_drv_common.h"

/* Green Imbalance Config */
const ipipe_gic_cfg_t gic_iss_default_params = {
    1,                                                     // uint16 enable;
    IPIPE_GIC_LSC_GAIN_ON,                                 // ipipe_gic_lsc_gain
                                                           // lsc_gain;
    IPIPE_GIC_NF2THR,                                      // ipipe_gic_sel
                                                           // sel;
    IPIPE_GIC_DIFF_INDEX,                                  // ipipe_gic_index
                                                           // typ;
    128,                                                   // uint8 gic_gain;
    31,                                                    // uint8
                                                           // gic_nfgain;
    512,                                                   // uint8 gic_thr;
    512                                                    // uint8
                                                           // gic_slope;
};

const ipipe_gic_cfg_t gic_default_params_no_gic = {
    0,                                                     // uint16 enable;
    IPIPE_GIC_LSC_GAIN_ON,                                 // ipipe_gic_lsc_gain
                                                           // lsc_gain;
    IPIPE_GIC_NF2THR,                                      // ipipe_gic_sel
                                                           // sel;
    IPIPE_GIC_DIFF_INDEX,                                  // ipipe_gic_index
                                                           // typ;
    128,                                                   // uint8 gic_gain;
    31,                                                    // uint8
                                                           // gic_nfgain;
    512,                                                   // uint8 gic_thr;
    512                                                    // uint8
                                                           // gic_slope;
};

/* LSC GAIN */

const ipipe_lsc_cfg_t lsc_iss_default_params = {

    0,                                                     // uint16
                                                           // v_offset;
    0,                                                     // int16
                                                           // v_linear_coeff;//va1
    0,                                                     // int16
                                                           // v_quadratic_coeff;//va2
    0,                                                     // uint8
                                                           // v_linear_shift;//vs1
    0,                                                     // uint8
                                                           // v_quadratic_shift;//vs2

    0,                                                     // uint16
                                                           // h_offset;
    0,                                                     // int16
                                                           // h_linear_coeff;//va1
    0,                                                     // int16
                                                           // h_quadratic_coeff;//va2
    0,                                                     // uint8
                                                           // h_linear_shift;//vs1
    0,                                                     // uint8
                                                           // h_quadratic_shift;//vs2

    255,                                                   // uint8 gain_r;

    255,                                                   // uint8 gain_gr;

    255,                                                   // uint8 gain_gb;

    255,                                                   // uint8 gain_b;

    32,                                                    // uint8 off_r;
    32,                                                    // uint8 off_gr;
    32,                                                    // uint8 off_gb;
    32,                                                    // uint8 off_b;

    0,                                                     // uint8 shift;
    511                                                    // uint16 max
};

/* Noise Filter */

const uint16 iss_default_nf_thr[8] = {20, 30, 35, 40, 40, 40, 40, 40};
const uint8  iss_default_nf_str[8] = {16, 16, 16, 16, 16, 16, 16, 16};
const uint8  iss_default_nf_spr[8] = {3, 3, 3, 3, 3, 3, 3, 3 };

const ipipe_noise_filter_cfg_t nf1_iss_default_params = {
    0, //uint8 enable;
    NOISE_FILTER_1, //noise_filter_t nf_num;
    IPIPE_NF_SPR_LUT, //###########TODO###############################   ipipe_nf_sel sel;
    IPIPE_NF_LSC_GAIN_OFF, //  IPIPE_NF_LSC_GAIN_T lsc_gain;/*0*/
    IPIPE_NF_SAMPLE_BOX, //   IPIPE_NF_SAMPLE_METHOD_T typ;/*1*/
    2, // uint8 down_shift_val;/*3*/
    3, //uint8 spread;/*4*/
    (uint16*)iss_default_nf_thr, /*10*/
    (uint8*)iss_default_nf_str, /*5*/
    (uint8*)iss_default_nf_spr, /*5*/
    0, //   uint16 edge_min;/*10*/
    2047//   uint16 edge_max;/*10*/
};

const ipipe_noise_filter_cfg_t nf2_iss_default_params = {
    1, //uint8 enable;
    NOISE_FILTER_2, //noise_filter_t nf_num;
    IPIPE_NF_SPR_LUT, //###########TODO###############################   ipipe_nf_sel sel;
    IPIPE_NF_LSC_GAIN_OFF, //  IPIPE_NF_LSC_GAIN_T lsc_gain;/*0*/
    IPIPE_NF_SAMPLE_BOX, //   IPIPE_NF_SAMPLE_METHOD_T typ;/*1*/
    2, // uint8 down_shift_val;/*3*/
    3, //uint8 spread;/*4*/
    (uint16*)iss_default_nf_thr, /*10*/
    (uint8*)iss_default_nf_str, /*5*/
    (uint8*)iss_default_nf_spr, /*5*/
    0, //   uint16 edge_min;/*10*/
    2047//   uint16 edge_max;/*10*/
};

const ipipe_noise_filter_cfg_t nf2_iss_high_res_params = {
    0, //uint8 enable;
    NOISE_FILTER_2, //noise_filter_t nf_num;
    IPIPE_NF_SPR_LUT, //###########TODO###############################   ipipe_nf_sel sel;
    IPIPE_NF_LSC_GAIN_OFF, //  IPIPE_NF_LSC_GAIN_T lsc_gain;/*0*/
    IPIPE_NF_SAMPLE_BOX, //   IPIPE_NF_SAMPLE_METHOD_T typ;/*1*/
    2, // uint8 down_shift_val;/*3*/
    3, //uint8 spread;/*4*/
    (uint16*)iss_default_nf_thr, /*10*/
    (uint8*)iss_default_nf_str, /*5*/
    (uint8*)iss_default_nf_spr, /*5*/
    0, //   uint16 edge_min;/*10*/
    2047//   uint16 edge_max;/*10*/
};

const ipipe_noise_filter_cfg_t nf_default_params_no_nf = {
    0,                                                     // uint8 enable;
    NOISE_FILTER_1,                                        // noise_filter_t
                                                           // nf_num;
    IPIPE_NF_SPR_LUT,                                      // ###########TODO###############################
                                                           // ipipe_nf_sel
                                                           // sel;
    IPIPE_NF_LSC_GAIN_ON,                                  // IPIPE_NF_LSC_GAIN_T
                                                           // lsc_gain;/*0*/
    IPIPE_NF_SAMPLE_BOX,                                   // IPIPE_NF_SAMPLE_METHOD_T
                                                           // typ;/*1*/
    0,                                                     // uint8
                                                           // down_shift_val;/*3*/
    3,                                                     // uint8
                                                           // spread;/*4*/
    (uint16 *) iss_default_nf_thr,                         /* 10 */
    (uint8 *) iss_default_nf_str,                          /* 5 */
    (uint8 *) iss_default_nf_spr,                          /* 5 */
    0,                                                     // uint16
                                                           // edge_min;/*10*/
    2048                                                   // uint16
                                                           // edge_max;/*10*/
};

/* WHITE BALANCE PARAMETER */
const uint16 wb_iss_default_config_offset[4] = { 0, 0, 0, 0 };

// uint16 wb_iss_default_config_gain[4] = {1200, 512, 512, 880};
uint16 wb_iss_default_config_gain[4] = { 760, 512, 512, 1100 };

// uint16 wb_iss_default_config_gain[4] = { 1024, 1024, 1024, 1024};

const ipipe_wb_cfg_t wb_iss_default_params = {

    (uint16 *) wb_iss_default_config_offset,
    (uint16 *) wb_iss_default_config_gain
};

/* CFAI PARMATERS */
const ipipe_cfa_cfg_t cfa_iss_default_params = {
    1,                                                     // uint8 enable;
    IPIPE_CFA_MODE_2DIR,                                   // IPIPE_CFA_MODE_T
                                                           // mode;
    {
       600, //uint16 hpf_thr;
       57, //   uint16 hpf_slope;
                                                           // hpf_slope;
       100, // uint16 mix_thr;
     10,                                                   // uint16
                                                           // mix_slope;
     4,                                                    // uint16 dir_thr;
     10,                                                   // uint16
                                                           // dir_slope;
       16//   uint16 dir_ndwt;
                                                           // dir_ndwt;
     },                                                    // ipipe_cfa_dir_t
                                                           // dir;
    {
     24,                                                   // uint8
                                                           // mono_hue_fra;
     25,                                                   // uint8
                                                           // mono_edg_thr;
     27,                                                   // uint16
                                                           // mono_thr_min;
     20,                                                   // uint16
                                                           // mono_thr_slope;
     50,                                                   // uint16
                                                           // mono_slp_min;
     40,                                                   // uint16
                                                           // mono_slp_slp;
     16                                                    // uint16
                                                           // mono_lpwt;
     }                                                     // ipipe_cfa_daa_t
                                                           // daa;
};

const ipipe_cfa_cfg_t cfa_default_params_no_cfa = {
    0,                                                     // uint8 enable;
    IPIPE_CFA_MODE_2DIR,                                   // IPIPE_CFA_MODE_T
                                                           // mode;
    {
     1024,                                                 // uint16 hpf_thr;
     0,                                                    // uint16
                                                           // hpf_slope;
     30,                                                   // uint16 mix_thr;
     10,                                                   // uint16
                                                           // mix_slope;
     4,                                                    // uint16 dir_thr;
     10,                                                   // uint16
                                                           // dir_slope;
     16                                                    // uint16
                                                           // dir_ndwt;
     },                                                    // ipipe_cfa_dir_t
                                                           // dir;
    {
     24,                                                   // uint8
                                                           // mono_hue_fra;
     25,                                                   // uint8
                                                           // mono_edg_thr;
     27,                                                   // uint16
                                                           // mono_thr_min;
     20,                                                   // uint16
                                                           // mono_thr_slope;
     50,                                                   // uint16
                                                           // mono_slp_min;
     40,                                                   // uint16
                                                           // mono_slp_slp;
     16                                                    // uint16
                                                           // mono_lpwt;
     }                                                     // ipipe_cfa_daa_t
                                                           // daa;
};

const ipipe_cfa_cfg_t cfa_iss_high_res_params = {
    1,                                                     // uint8 enable;
    IPIPE_CFA_MODE_2DIR_DA,                                   // IPIPE_CFA_MODE_T
                                                           // mode;
    {
       0, //uint16 hpf_thr;
       1023, //   uint16 hpf_slope;
                                                           // hpf_slope;
       100, // uint16 mix_thr;
     10,                                                   // uint16
                                                           // mix_slope;
     5,                                                    // uint16 dir_thr;
     127,                                                   // uint16
                                                           // dir_slope;
       16//   uint16 dir_ndwt;
                                                           // dir_ndwt;
     },                                                    // ipipe_cfa_dir_t
                                                           // dir;
    {
     0,                                                   // uint8
                                                           // mono_hue_fra;
     20,                                                   // uint8
                                                           // mono_edg_thr;
     7,                                                   // uint16
                                                           // mono_thr_min;
     20,                                                   // uint16
                                                           // mono_thr_slope;
     200,                                                   // uint16
                                                           // mono_slp_min;
     20,                                                   // uint16
                                                           // mono_slp_slp;
     32                                                    // uint16
                                                           // mono_lpwt;
     }                                                     // ipipe_cfa_daa_t
                                                           // daa;
};

/* Gamma */
const int16 gamma_iss_default_table[] = {
#if 0 //original
    #include "../inc/tables/gamma_table.h"
#else //Gang: touch for Canon Gamma
0    , 8  ,
8    , 11 ,
19   , 10 ,
29   , 12 ,
41   , 12 ,
53   , 10 ,
63   , 10 ,
73   , 9  ,
82   , 10 ,
92   , 9  ,
101  , 9  ,
110  , 9  ,
119  , 9  ,
128  , 9  ,
137  , 9  ,
146  , 8  ,
154  , 9  ,
163  , 8  ,
171  , 8  ,
179  , 8  ,
187  , 8  ,
195  , 7  ,
202  , 8  ,
210  , 8  ,
218  , 7  ,
225  , 7  ,
232  , 7  ,
239  , 7  ,
246  , 7  ,
253  , 7  ,
260  , 7  ,
267  , 6  ,
273  , 7  ,
280  , 6  ,
286  , 7  ,
293  , 6  ,
299  , 6  ,
305  , 6  ,
311  , 6  ,
317  , 6  ,
323  , 6  ,
329  , 6  ,
335  , 5  ,
340  , 6  ,
346  , 5  ,
351  , 6  ,
357  , 5  ,
362  , 5  ,
367  , 6  ,
373  , 5  ,
378  , 5  ,
383  , 5  ,
388  , 5  ,
393  , 5  ,
398  , 4  ,
402  , 5  ,
407  , 5  ,
412  , 4  ,
416  , 5  ,
421  , 5  ,
426  , 4  ,
430  , 4  ,
434  , 5  ,
439  , 4  ,
443  , 4  ,
447  , 5  ,
452  , 4  ,
456  , 4  ,
460  , 4  ,
464  , 4  ,
468  , 4  ,
472  , 4  ,
476  , 4  ,
480  , 4  ,
484  , 3  ,
487  , 4  ,
491  , 4  ,
495  , 4  ,
499  , 3  ,
502  , 4  ,
506  , 3  ,
509  , 4  ,
513  , 3  ,
516  , 4  ,
520  , 3  ,
523  , 4  ,
527  , 3  ,
530  , 4  ,
534  , 3  ,
537  , 3  ,
540  , 3  ,
543  , 4  ,
547  , 3  ,
550  , 3  ,
553  , 3  ,
556  , 3  ,
559  , 3  ,
562  , 4  ,
566  , 3  ,
569  , 3  ,
572  , 3  ,
575  , 3  ,
578  , 3  ,
581  , 3  ,
584  , 2  ,
586  , 3  ,
589  , 3  ,
592  , 3  ,
595  , 3  ,
598  , 3  ,
601  , 3  ,
604  , 2  ,
606  , 3  ,
609  , 3  ,
612  , 3  ,
615  , 2  ,
617  , 3  ,
620  , 3  ,
623  , 2  ,
625  , 3  ,
628  , 2  ,
630  , 3  ,
633  , 3  ,
636  , 2  ,
638  , 3  ,
641  , 2  ,
643  , 3  ,
646  , 2  ,
648  , 3  ,
651  , 2  ,
653  , 3  ,
656  , 2  ,
658  , 2  ,
660  , 3  ,
663  , 2  ,
665  , 2  ,
667  , 3  ,
670  , 2  ,
672  , 2  ,
674  , 3  ,
677  , 2  ,
679  , 2  ,
681  , 2  ,
683  , 2  ,
685  , 3  ,
688  , 2  ,
690  , 2  ,
692  , 2  ,
694  , 2  ,
696  , 2  ,
698  , 2  ,
700  , 2  ,
702  , 2  ,
704  , 2  ,
706  , 2  ,
708  , 2  ,
710  , 2  ,
712  , 2  ,
714  , 2  ,
716  , 2  ,
718  , 2  ,
720  , 2  ,
722  , 2  ,
724  , 2  ,
726  , 3  ,
729  , 2  ,
731  , 2  ,
733  , 2  ,
735  , 2  ,
737  , 2  ,
739  , 2  ,
741  , 2  ,
743  , 2  ,
745  , 2  ,
747  , 2  ,
749  , 2  ,
751  , 2  ,
753  , 2  ,
755  , 2  ,
757  , 2  ,
759  , 2  ,
761  , 2  ,
763  , 2  ,
765  , 2  ,
767  , 2  ,
769  , 2  ,
771  , 2  ,
773  , 2  ,
775  , 2  ,
777  , 2  ,
779  , 1  ,
780  , 2  ,
782  , 2  ,
784  , 1  ,
785  , 2  ,
787  , 1  ,
788  , 2  ,
790  , 1  ,
791  , 1  ,
792  , 1  ,
793  , 2  ,
795  , 1  ,
796  , 1  ,
797  , 1  ,
798  , 2  ,
800  , 1  ,
801  , 1  ,
802  , 1  ,
803  , 1  ,
804  , 1  ,
805  , 2  ,
807  , 1  ,
808  , 1  ,
809  , 1  ,
810  , 1  ,
811  , 1  ,
812  , 1  ,
813  , 2  ,
815  , 1  ,
816  , 1  ,
817  , 1  ,
818  , 1  ,
819  , 1  ,
820  , 1  ,
821  , 1  ,
822  , 1  ,
823  , 2  ,
825  , 1  ,
826  , 1  ,
827  , 1  ,
828  , 1  ,
829  , 1  ,
830  , 1  ,
831  , 1  ,
832  , 1  ,
833  , 1  ,
834  , 1  ,
835  , 1  ,
836  , 1  ,
837  , 1  ,
838  , 1  ,
839  , 1  ,
840  , 1  ,
841  , 1  ,
842  , 1  ,
843  , 1  ,
844  , 1  ,
845  , 1  ,
846  , 1  ,
847  , 1  ,
848  , 1  ,
849  , 1  ,
850  , 1  ,
851  , 1  ,
852  , 1  ,
853  , 1  ,
854  , 1  ,
855  , 1  ,
856  , 1  ,
857  , 1  ,
858  , 1  ,
859  , 1  ,
860  , 1  ,
861  , 1  ,
862  , 1  ,
863  , 1  ,
864  , 1  ,
865  , 1  ,
866  , 1  ,
867  , 1  ,
868  , 0  ,
868  , 1  ,
869  , 1  ,
870  , 1  ,
871  , 1  ,
872  , 1  ,
873  , 1  ,
874  , 1  ,
875  , 1  ,
876  , 1  ,
877  , 1  ,
878  , 0  ,
878  , 1  ,
879  , 1  ,
880  , 1  ,
881  , 1  ,
882  , 1  ,
883  , 1  ,
884  , 1  ,
885  , 0  ,
885  , 1  ,
886  , 1  ,
887  , 1  ,
888  , 1  ,
889  , 1  ,
890  , 1  ,
891  , 0  ,
891  , 1  ,
892  , 1  ,
893  , 1  ,
894  , 1  ,
895  , 1  ,
896  , 0  ,
896  , 1  ,
897  , 1  ,
898  , 1  ,
899  , 1  ,
900  , 0  ,
900  , 1  ,
901  , 1  ,
902  , 1  ,
903  , 1  ,
904  , 1  ,
905  , 0  ,
905  , 1  ,
906  , 1  ,
907  , 1  ,
908  , 0  ,
908  , 1  ,
909  , 1  ,
910  , 1  ,
911  , 1  ,
912  , 0  ,
912  , 1  ,
913  , 1  ,
914  , 1  ,
915  , 1  ,
916  , 0  ,
916  , 1  ,
917  , 1  ,
918  , 1  ,
919  , 0  ,
919  , 1  ,
920  , 1  ,
921  , 1  ,
922  , 0  ,
922  , 1  ,
923  , 1  ,
924  , 1  ,
925  , 0  ,
925  , 1  ,
926  , 1  ,
927  , 1  ,
928  , 0  ,
928  , 1  ,
929  , 1  ,
930  , 0  ,
930  , 1  ,
931  , 1  ,
932  , 1  ,
933  , 0  ,
933  , 1  ,
934  , 1  ,
935  , 0  ,
935  , 1  ,
936  , 1  ,
937  , 1  ,
938  , 0  ,
938  , 1  ,
939  , 1  ,
940  , 0  ,
940  , 1  ,
941  , 1  ,
942  , 1  ,
943  , 0  ,
943  , 1  ,
944  , 1  ,
945  , 0  ,
945  , 1  ,
946  , 1  ,
947  , 0  ,
947  , 1  ,
948  , 1  ,
949  , 0  ,
949  , 1  ,
950  , 1  ,
951  , 0  ,
951  , 1  ,
952  , 1  ,
953  , 0  ,
953  , 1  ,
954  , 1  ,
955  , 0  ,
955  , 1  ,
956  , 1  ,
957  , 0  ,
957  , 1  ,
958  , 1  ,
959  , 0  ,
959  , 1  ,
960  , 1  ,
961  , 0  ,
961  , 1  ,
962  , 1  ,
963  , 0  ,
963  , 1  ,
964  , 0  ,
964  , 1  ,
965  , 1  ,
966  , 0  ,
966  , 1  ,
967  , 1  ,
968  , 0  ,
968  , 1  ,
969  , 1  ,
970  , 0  ,
970  , 1  ,
971  , 0  ,
971  , 1  ,
972  , 1  ,
973  , 0  ,
973  , 1  ,
974  , 0  ,
974  , 1  ,
975  , 1  ,
976  , 0  ,
976  , 1  ,
977  , 1  ,
978  , 0  ,
978  , 1  ,
979  , 0  ,
979  , 1  ,
980  , 1  ,
981  , 0  ,
981  , 1  ,
982  , 0  ,
982  , 1  ,
983  , 1  ,
984  , 0  ,
984  , 1  ,
985  , 0  ,
985  , 1  ,
986  , 0  ,
986  , 1  ,
987  , 1  ,
988  , 0  ,
988  , 1  ,
989  , 0  ,
989  , 1  ,
990  , 1  ,
991  , 0  ,
991  , 1  ,
992  , 0  ,
992  , 1  ,
993  , 0  ,
993  , 1  ,
994  , 1  ,
995  , 0  ,
995  , 1  ,
996  , 0  ,
996  , 1  ,
997  , 0  ,
997  , 1  ,
998  , 1  ,
999  , 0  ,
999  , 1  ,
1000 , 0  ,
1000 , 1  ,
1001 , 0  ,
1001 , 1  ,
1002 , 0  ,
1002 , 1  ,
1003 , 1  ,
1004 , 0  ,
1004 , 1  ,
1005 , 0  ,
1005 , 1  ,
1006 , 0  ,
1006 , 1  ,
1007 , 0  ,
1007 , 1  ,
1008 , 0  ,
1008 , 1  ,
1009 , 0  ,
1009 , 1  ,
1010 , 1  ,
1011 , 0  ,
1011 , 1  ,
1012 , 0  ,
1012 , 1  ,
1013 , 0  ,
1013 , 1  ,
1014 , 0  ,
1014 , 1  ,
1015 , 0  ,
1015 , 1  ,
1016 , 0  ,
1016 , 1  ,
1017 , 0  ,
1017 , 1  ,
1018 , 0  ,
1018 , 1  ,
1019 , 1  ,
1020 , 0  ,
1020 , 0  ,
1020 , 1  ,
1021 , 0  ,
1021 , 0  ,
1021 , 0  ,
1021 , 1  ,
1022 , 0  ,
1022 , 0  ,
1022 , 0  ,
1022 , 0  ,
1022 , 1  ,
1023 , 0  ,
1023 , 0  ,
1023 , 0  ,
1023 , 0  ,
1023 , 0  ,
1023 , 0  ,
1023 , 0  ,
#endif
};

const ipipe_gamma_cfg_t gamma_iss_default_params = {

    IPIPE_GAMMA_TBL_512,                                   // IPIPE_GAMMA_TABLE_SIZE_T
                                                           // gamma_tbl_size;
    0,                                                     // uint8 tbl;//may
                                                           // not be
                                                           // needed.since
                                                           // table is always
                                                           // in RAM
    IPIPE_GAMMA_BYPASS_DISABLE,                            // IPIPE_GAMMA_BYPASS_T
                                                           // bypass_b;/*o
                                                           // not bypassed*/
    IPIPE_GAMMA_BYPASS_DISABLE,                            // IPIPE_GAMMA_BYPASS_T
                                                           // bypass_g;
    IPIPE_GAMMA_BYPASS_DISABLE,                            // IPIPE_GAMMA_BYPASS_T
                                                           // bypass_r;
    (int16 *) gamma_iss_default_table,                     // uint32 *
                                                           // red_table;
    (int16 *) gamma_iss_default_table,                     // uint32 *
                                                           // blue_table;
    (int16 *) gamma_iss_default_table                      // uint32*
                                                           // green_table;
};

/* Histogram */
uint8 ipipe_histogram_iss_default_mult[4] = {
    4, 4, 4, 4
};

const ipipe_hist_dim_t ipipe_hist_dims_default[4] = {
    {
     0,                                                    // uint16 v_pos;
     479,                                                  // uint16 v_size;
     0,                                                    // uint16 h_pos;
     639                                                   // uint16 h_size;
     },
    {
     0,                                                    // uint16 v_pos;
     398,                                                  // uint16 v_size;
     600,                                                  // uint16 h_pos;
     398                                                   // uint16 h_size;
     },
    {
     600,                                                  // uint16 v_pos;
     398,                                                  // uint16 v_size;
     0,                                                    // uint16 h_pos;
     398                                                   // uint16 h_size;
     },
    {
     600,                                                  // uint16 v_pos;
     398,                                                  // uint16 v_size;
     600,                                                  // uint16 h_pos;
     398                                                   // uint16 h_size;
     }

};

ipipe_hist_cfg_t iss_default_histogram_params = {
    1,                                                     // uint8 enable;
    1,                                                     // uint8 ost;
    0,                                                     // uint8 sel;
    // ######## This is not in documentation(IPIPE_CONFIG.RTF) so
    // removed############
    0,                                                     // uint8
                                                           // type;doesnt
                                                           // matter if sel=1
    3,                                                     // uint8 bins;
    4,                                                     // uint8 shift;
    0x0f,                                                  // uint8
                                                           // col;/*bits
                                                           // [3:0], 0 is
                                                           // disable*/
    0x1,                                                   // uint8 regions;
                                                           // /*[3:0], 0 is
                                                           // disable*/
    (ipipe_hist_dim_t *) ipipe_hist_dims_default,          // ipipe_hist_dim_t
                                                           // *hist_dim;/*pointer
                                                           // to array of 4
                                                           // structs*/
    1,                                                     // uint8
                                                           // clear_table;
    0,                                                     // uint8
                                                           // table_sel;
    ipipe_histogram_iss_default_mult                       // uint8 mul4;
};

/* RGB Colour Correction */
// const uint16 rgb_rgb1_iss_default_gainparams[9] = {256, 0, 0, 0, 256, 0,
// 0, 0, 256};
const int16 rgb_rgb1_iss_default_gainparams[9] = {
    442, -194, 8,
    -136, 441, -49,
    -18, -315, 400
};

const int16 rgb_rgb1_iss_default_offsetparams[3] = { 0, 0, 0 };

const ipipe_rgb_rgb_cfg_t rgb_rgb_iss_default_params = {

    (int16 *) rgb_rgb1_iss_default_gainparams,
    (int16 *) rgb_rgb1_iss_default_offsetparams
};

/* RGB to RGB2 */
const int16 rgb_rgb2_iss_default_gainparams[9] =
    { 256, 0, 0, 0, 256, 0, 0, 0, 256 };
// const uint16 rgb_rgb2_iss_default_offsetparams[3] = {-100, -100, -100};
const int16 rgb_rgb2_iss_default_offsetparams[3] = { 0, 0, 0 };

const ipipe_rgb_rgb_cfg_t rgb_rgb2_iss_default_params = {

    (int16 *) rgb_rgb2_iss_default_gainparams,
    (int16 *) rgb_rgb2_iss_default_offsetparams
};

/* RGB to YUV */

const int16 rgb_yuv_iss_default_mult[9] = {
    77,
    150,                                                   // #
                                                           // IPIPE_YUV_MUL_GY
                                                           //
    29,                                                    // #
                                                           // IPIPE_YUV_MUL_BY
                                                           //
    -43,                                                   // #
                                                           // IPIPE_YUV_MUL_RCB
                                                           //
    -85,                                                   // #
                                                           // IPIPE_YUV_MUL_GCB
                                                           //
    128,                                                   // #
                                                           // IPIPE_YUV_MUL_BCB
                                                           //
    128,                                                   // #
                                                           // IPIPE_YUV_MUL_RCR
                                                           //
    -107,                                                  // #
                                                           // IPIPE_YUV_MUL_GCR
    -21,                                                   // #
                                                           // IPIPE_YUV_MUL_BCR
                                                           //
};

const int16 rgb_yuv_iss_default_offset[3] = { 0, 0x80, 0x80 };  /* #####changed */

const ipipe_rgb_yuv_cfg_t rgb_yuv_iss_default_params = {

    0,                                                     // uint8
                                                           // brightness;
    16,                                                    // uint8 contrast;
    (int16 *) rgb_yuv_iss_default_mult,                    // int16
                                                           // *mul_val;/*RY,
                                                           // GY, BY, RCB,
                                                           // GCB, BCB , RCR,
                                                           // GCR, BCR 12
                                                           // bits*/

    (int16 *) rgb_yuv_iss_default_offset,                  // int16*
                                                           // offset;/*Y, CB,
                                                           // CR -11bits*/
};

 /*GBCE*/ const uint16 gbce_iss_default_table[] = {
#include "../inc/tables/gbce_table.h"
};

const ipipe_gbce_cfg_t gbce_iss_default_params = {
    0,                                                     // uint16 enable;
    IPIPE_GBCE_METHOD_Y_VALUE,                            // uint16 typ;
    (uint16 *) gbce_iss_default_table                      // uint8 *
                                                           // lookup_table;
};

const ipipe_yuv444_yuv422_cfg_t yuv444_yuv_422_iss_default_params = {
    IPIPE_YUV_PHS_POS_COSITED,
    0
};

/* EE */
const int16 ee_mul_val_default_preview[9] = {
    84,
    -8,
    -4,
    -8,
    -4,
    -2,
    -4,
    -2,
    -1
};

const int16 ee_mul_val_strong_preview[9] = {
    48,
    12,
    -10,
    12,
    0,
    -6,
    -10,
    -6,
    -2
};
const int16 ee_mul_val_high_res_preview[9] = {
    0,
    0,
    0,
    0,
    -10,
    -10,
    0,
    -10,
    80
};

const ipipe_car_cfg_t iss_default_car_params = {
    0,                                                     // uint8 enable;

    1,                                                     // uint8 typ;

    0xff,                                                  // uint8 sw0_thr;
    0xc0,                                                  // uint8 sw1_thr;

    0x3,                                                   // uint8 hpf_type;
    0,                                                     // uint8
                                                           // hpf_shift;
    32,                                                    // uint8 hpf_thr;

    255,                                                   // uint8 gn1_gain;
    5,                                                     // uint8
                                                           // gn1_shift;
    128,                                                   // uint16 gn1_min;

    255,                                                   // uint8 gn2_gain;
    6,                                                     // uint8
                                                           // gn2_shift;
    128                                                    // uint16 gn2_min;
};

const int16 ee_iss_default_table[] = {
#include "../inc/tables/edge_enhance_table.h"
};

const ipipe_ee_cfg_t edge_enhance_iss_default_params = {
    1,                                                     // uint16 enable;
    IPIPE_HALO_REDUCTION_ENABLE,                           // uint8
                                                           // halo_reduction;
    (int16 *) ee_mul_val_default_preview,                  // int16
                                                           // *mul_val;/*9 co
                                                           // -efficients*/
    1,                                                     // uint8 sel;
    10,                                                    // uint8 shift_hp;
    12,                                                    // uint16
                                                           // threshold;
    32,                                                    // uint16 gain;
    128,                                                   // uint8
                                                           // hpf_low_thr;
    12,                                                    // 768, //
                                                           // ####################TO
                                                           // check these
                                                           // values uint8
                                                           // hpf_high_thr;
    32,                                                    // uint8
                                                           // hpf_gradient_gain;
    0,                                                     // uint8
                                                           // hpf_gradient_offset;
    (int16 *) ee_iss_default_table
};

const ipipe_ee_cfg_t edge_enhance_default_cap_params_no_ee = {
    0,                                                     // uint16 enable;
    IPIPE_HALO_REDUCTION_ENABLE,                           // uint8
                                                           // halo_reduction;
    (int16 *) ee_mul_val_strong_preview,                  // int16
                                                           // *mul_val;/*9 co
                                                           // -efficients*/
    1,                                                     // uint8 sel;
    10,                                                    // uint8 shift_hp;
    12,                                                    // uint16
                                                           // threshold;
    32,                                                    // uint16 gain;
    128,                                                   // uint8
                                                           // hpf_low_thr;
    12,                                                    // 768, //
                                                           // ####################TO
                                                           // check these
                                                           // values uint8
                                                           // hpf_high_thr;
    32,                                                    // uint8
                                                           // hpf_gradient_gain;
    0,                                                     // uint8
                                                           // hpf_gradient_offset;
    (int16 *) ee_iss_default_table
};

const ipipe_ee_cfg_t edge_enhance_default_cap_params_high_res_ee = {
    1,                                                     // uint16 enable;
    IPIPE_HALO_REDUCTION_ENABLE,                           // uint8
                                                           // halo_reduction;
    (int16 *) ee_mul_val_high_res_preview,                  // int16
                                                           // *mul_val;/*9 co
                                                           // -efficients*/
    1,                                                     // uint8 sel;
    2,                                                    // uint8 shift_hp;
    32,                                                    // uint16
                                                           // threshold;
    0,                                                    // uint16 gain;
    192,                                                   // uint8
                                                           // hpf_low_thr;
    16,                                                    // 768, //
                                                           // ####################TO
                                                           // check these
                                                           // values uint8
                                                           // hpf_high_thr;
    64,                                                    // uint8
                                                           // hpf_gradient_gain;
    0,                                                     // uint8
                                                           // hpf_gradient_offset;
    (int16 *) ee_iss_default_table
};


ipipe_bsc_cfg_t default_preview_bsc_params = {
    0,                                                     // uint8 enable;
    1,                                                     // uint8 mode;
    1,                                                     // uint8
                                                           // col_sample;
    0,                                                     // uint8
                                                           // row_sample;
    0x1,                                                   // uint8
                                                           // element;/*Y or
                                                           // CB or CR*/
    // ipipe_bsc_pos_params_t col_pos;
    {
     0x3,                                                  // uint8 vectors;
     0x0,                                                  // uint8 shift;
     0x128,                                                // uint16 v_pos;
     0x128,                                                // uint16 h_pos;
     0x63,                                                 // uint16 v_num;
     0x31,                                                 // uint16 h_num;
     0x1,                                                  // uint8 v_skip;
     0x7                                                   // uint8 h_skip;
     },
    // ipipe_bsc_pos_params_t row_pos;
    {
     0x3,                                                  // uint8 vectors;
     0x0,                                                  // uint8 shift;
     0x32,                                                 // uint16 v_pos;
     0x32,                                                 // uint16 h_pos;
     0x32,                                                 // uint16 v_num;
     0x31,                                                 // uint16 h_num;
     0x5,                                                  // uint8 v_skip;
     0x7                                                   // uint8 h_skip;
     }
};

ipipe_boxcar_cfg_t iss_default_boxcar_params = {
    0,                                                     // enable;
    1,                                                     // ost;
    1,                                                     // box_size;
    3,                                                     // shift;
    0,                                                     // addr;
};

const uint32 iss_3d_lut_default_table[] = {
    0                                                      // #include
                                                           // "../inc/tables/3dlut_table.h"
};

const ipipe_3d_lut_cfg_t default_3dcc_params = {
    0,                                                     /* disable 3d
                                                            * color corection
                                                            */
    &iss_3d_lut_default_table
        // 0 /* 32 bit input buffer */
        // IPIPE_3DLUTtable0 /*address table #0*/
};

const ipipe_dpc_otf_cfg_t default_dpc_otf_params = {
#if 0 //original
    0,
    IPIPE_DPC_OTF_MAX1_MIN1,
    IPIPE_DPC_OTF_ALG_MINMAX3,
    {
        (uint16 )0x01000002,
        (uint16 )0x03200000
    }
#else //Gang: touch for DPC3
        1,
        IPIPE_DPC_OTF_MAX1_MIN1,
        IPIPE_DPC_OTF_ALG_MINMAX3,
        {
            .dpc3_params = {
                3,
                { 1024, 1, 20, 200},
                { 1024, 1, 20, 200}
            }
        }
#endif
};

const ipipe_dpc_otf_cfg_t high_res_dpc_otf_params = {
        0,
        IPIPE_DPC_OTF_MAX1_MIN1,
        IPIPE_DPC_OTF_ALG_MINMAX3,
        {
            .dpc3_params = {
                3,
                { 1024, 1, 20, 200},
                { 1024, 1, 20, 200}
            }
        }
};


const ipipe_dpc_lut_cfg_t default_dpc_lut_params = {
    0,
    // FIXME: Set some valid defaults here
};

IssIpipeFillDefaultParams(iss_config_processing_t * iss_config)
{
    iss_config->nf1         = (ipipe_noise_filter_cfg_t*  )&nf1_iss_default_params;    //Gang: touch for nf1
    iss_config->nf2         = (ipipe_noise_filter_cfg_t*  )&nf2_iss_default_params;    //Gang: touch for nf2
    iss_config->gic         = (ipipe_gic_cfg_t*           )&gic_default_params_no_gic; //Gang: touch for gic off
    iss_config->wb          = (ipipe_wb_cfg_t*            )&wb_iss_default_params;
    iss_config->cfa         = (ipipe_cfa_cfg_t*           )&cfa_iss_default_params;
    iss_config->gamma       = (ipipe_gamma_cfg_t*         )&gamma_iss_default_params;
    iss_config->rgb2rgb1    = (ipipe_rgb_rgb_cfg_t*       )&rgb_rgb_iss_default_params;
    iss_config->rgb2rgb2    = (ipipe_rgb_rgb_cfg_t*       )&rgb_rgb2_iss_default_params;
    iss_config->rgb2yuv     = (ipipe_rgb_yuv_cfg_t*       )&rgb_yuv_iss_default_params;
    iss_config->gbce        = (ipipe_gbce_cfg_t*          )&gbce_iss_default_params;
    iss_config->yuv2yuv     = (ipipe_yuv444_yuv422_cfg_t* )&yuv444_yuv_422_iss_default_params;
    iss_config->ee          = (ipipe_ee_cfg_t*            )&edge_enhance_default_cap_params_no_ee;
    iss_config->car         = (ipipe_car_cfg_t*           )&iss_default_car_params;
    iss_config->lsc         = (ipipe_lsc_cfg_t*           )&lsc_iss_default_params;
    iss_config->histogram   = (ipipe_hist_cfg_t*          )&iss_default_histogram_params;
    iss_config->boxcar      = (ipipe_boxcar_cfg_t*        )&iss_default_boxcar_params;
    iss_config->bsc         = (ipipe_bsc_cfg_t*           )&default_preview_bsc_params;
    iss_config->cc3dlut     = (ipipe_3d_lut_cfg_t*        )&default_3dcc_params;
    iss_config->dpc_otf     = (ipipe_dpc_otf_cfg_t*       )&default_dpc_otf_params;
    iss_config->dpc_lut     = (ipipe_dpc_lut_cfg_t*       )&default_dpc_lut_params;

    iss_config->color_pattern = IPIE_BAYER_PATTERN_RGGB;

    iss_config->f_proc_ipipe_validity = PROC_IPIPE_VALID_NO;
    iss_config->f_proc_ipipe_validity =(PROC_IPIPE_VALID_ID)(PROC_IPIPE_VALID_GIC    |
                                                            PROC_IPIPE_VALID_EE      |
                                                            PROC_IPIPE_VALID_NF1     |
                                                            PROC_IPIPE_VALID_NF2     |
                                                            PROC_IPIPE_VALID_CAR     |
                                                            PROC_IPIPE_VALID_GAMMA   |
                                                            PROC_IPIPE_VALID_LSC     |
                                                            PROC_IPIPE_VALID_CFA     |
                                                            PROC_IPIPE_VALID_RGB2YUV |
                                                            PROC_IPIPE_VALID_RGB2RGB1|
                                                            PROC_IPIPE_VALID_RGB2RGB2|
//                                                            PROC_IPIPE_VALID_HIST    |
//                                                            PROC_IPIPE_VALID_BOXCAR  |
                                                            PROC_IPIPE_VALID_WB      |
                                                            PROC_IPIPE_VALID_YUV2YU  |
                                                            PROC_IPIPE_VALID_DPC_OTF |
                                                            PROC_IPIPE_VALID_DPC_LUT |
                                                            PROC_IPIPE_VALID_3DCC);

}

const isif_gain_offset_cfg_t iss_default_gain_offset_params = {
    0,                                                     // uint8
                                                           // gain_offset_featureflag;
    0,                                                     // uint16 gain_r;
                                                           // //CGR CRGAIN
    0,                                                     // uint16 gain_gr;
                                                           // //CGGR CGRGAIN
    0,                                                     // uint16 gain_gb;
                                                           // //CGGB CGBGAIN
    0,                                                     // uint16 gain_bg;
                                                           // //CGB CBGAIN
    0                                                      // uint16 offset;
                                                           // //COFT COFSTA
};

const isif_vldc_defect_line_params_t iss_default_vldc[] = {
    0,                                                     // uint16
                                                           // vertical_defect_position;
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position;
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc;
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc;
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc;
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position;
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position;
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc;
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc;
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc;
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position;
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position;
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc;
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc;
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc;
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position;
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position;
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc;
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc;
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc;
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position;
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position;
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc;
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc;
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc;
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position;
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position;
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc;
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc;
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc;
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position;
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position;
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc;
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc;
                                                           // //DFCMEM3
    0,                                                     // uint8
                                                           // sub3_greater_than_vldc;
                                                           // //DFCMEM4

    0,                                                     // uint16
                                                           // vertical_defect_position;
                                                           // //DFCMEM0
    0,                                                     // uint16
                                                           // horizontal_defect_position;
                                                           // //DFCMEM1 12-0
    0,                                                     // uint8
                                                           // sub1_value_vldc;
                                                           // //DFCMEM2
    0,                                                     // uint8
                                                           // sub2_less_than_vldc;
                                                           // //DFCMEM3
    0                                                      // uint8
                                                           // sub3_greater_than_vldc;
                                                           // //DFCMEM4
};

const isif_vldc_cfg_t iss_default_vldc_params = {
    0,                                                     // uint8 enable;
                                                           // //VDFCEN DFCCTL
    ISIF_VDLC_DISABLE_ABOVE_UPPER_PIXEL,                   // ISIF_VDLC_PIXEL_DEPENDENCY
                                                           // disable_vldc_upper_pixels;
                                                           // //VDFCUDA
                                                           // DFCCTL
    3,                                                     // uint8
                                                           // vldc_shift_value;
                                                           // //VDFLSFT
                                                           // DFCCTL
    ISIF_VLDC_HORIZONTAL_INTERPOLATION,                    // ISIF_VLDC_MODE_SEL_T
                                                           // vldc_mode_select;
                                                           // //VDFCSL DFCCTL
    0x1ff,                                                 // uint16
                                                           // vldc_saturation_level;
                                                           // //VDFSLV
                                                           // VDFSATLVuint12
                                                           // range 0 - 4095
    1,                                                     // uint8
                                                           // defect_lines;
                                                           // // no of
                                                           // vdefect lines
                                                           // max 8
    (isif_vldc_defect_line_params_t *) & iss_default_vldc[0]    // isif_vldc_defect_line_params_t*
                                                                // vldc_defect_line_params;
                                                                // //DFCMEM0
                                                                // -8
};

const isif_2dlsc_cfg_t iss_default_lsc_params = {
    0,                                                     // uint8 enable;
                                                           // //DLSCCFG
                                                           // ENABLE
    640,                                                   // uint16
                                                           // lsc_hsize;
                                                           // //HVAL LSCHVAL
    480,                                                   // uint16
                                                           // lsc_vsize;
                                                           // //VVAL LSCVVAL

    0,                                                     // uint16
                                                           // hdirection_data_offset;
                                                           // //HOFST
                                                           // LSCHOFST
                                                           // DATAHOFST
                                                           // 0-16383
    0,                                                     // uint16
                                                           // vdirection_data_offset;
                                                           // //VOFST
                                                           // LSCVOFST
                                                           // DATAVOFST

    0,                                                     // uint8
                                                           // hposin_paxel;
                                                           // //X DLSCINI 6:0
                                                           //
    0,                                                     // uint8
                                                           // vposin_paxel;
                                                           // //Y DLSCINI 6:0
                                                           // ypos_distance_from_first_paxel
    // ISIF_LSC
    ISIF_LSC_16_PIXEL,                                     // ISIF_LSC_GAIN_MODE_MN_T
                                                           // pax_height;
                                                           // //GAIN_MODE_M
                                                           // DLSCCFG
    ISIF_LSC_16_PIXEL,                                     // ISIF_LSC_GAIN_MODE_MN_T
                                                           // pax_length;
                                                           // //GAIN_MODE_N
                                                           // DLSCCFG
    // busy_field
    ISIF_LSC_GAIN_2Q6,                                     // ISIF_LSC_GAIN_FORMAT
                                                           // gain_format;
                                                           // //GAIN_FORMAT
                                                           // DLSCCFG

    0,                                                     // uint8
                                                           // offset_scaling_factor;
                                                           // //OFSTSF
                                                           // DLSCOFST
    ISIF_LSC_OFFSET_NO_SHIFT,                              // ISIF_LSC_OFFSET_SHIFT_T
                                                           // offset_shift_value;
                                                           // //OFSTSFT
                                                           // DLSCOFST
    ISIF_LSC_OFFSET_OFF,                                   // ISIF_LSC_OFFSET_CTRL_T
                                                           // offset_enable;
                                                           // //OFSTEN
                                                           // DLSCOFST

    0xFFFFFF00,                                            // uint32
                                                           // gain_table_address;
                                                           // //BASE31_16
                                                           // DLSCGRBU+
                                                           // BASE15_0
                                                           // DLSCGRBL 32bit
                                                           // aligned
    0xff,                                                  // uint16
                                                           // gain_table_length;
                                                           // //OFFSET
                                                           // DLSCGROF

    0xFFFFFF00,                                            // uint32
                                                           // offset_table_address;
                                                           // //BASE31_16
                                                           // DLSCORBU +
                                                           // BASE31_16
                                                           // DLSCORBU 32bit
                                                           // aligned
    0xff                                                   // uint16
                                                           // offset_table_length;
                                                           // //OFFSET
                                                           // DLSCOROF
        // interupt enable
        // interupt config
};

const isif_clamp_cfg_t iss_default_digital_clamp_params = {
    0,                                                     // uint8 enable;
                                                           // //CLEN CLAMPCFG
    ISIF_HORIZONTAL_CLAMP_ENABLED,                         // ISIF_HORIZONTAL_CLAMP_MODE_T
                                                           // hclamp_mode;
                                                           // //CLMD CLAMPCFG
    ISIF_FOUR_COLOR_CLAMP,                                 // ISIF_BLACK_CLAMP_MODE_T
                                                           // black_clamp_mode;
                                                           // //CLHMD
                                                           // CLAMPCFG
    0xfff,                                                 // uint16
                                                           // dcoffset_clamp_value;
                                                           // //CLDCOFST CLDC
                                                           // s13
    0xA,                                                   // uint16
                                                           // black_clamp_v_start_pos;
                                                           // //CLSV 12-0
                                                           // range 0-8191 ??
                                                           // 2 vstart pos
    // {
    0xF,                                                   // uint16 v_pos;
                                                           // //CLHSV CLHWIN2
                                                           // 12-0
    ISIF_HBLACK_16PIXEL_TALL,                              // ISIF_HBLACK_PIXEL_HEIGHT
                                                           // v_size; //CLHWN
                                                           // CLHWIN0
    0xD,                                                   // uint16 h_pos;
                                                           // //CLHSH CLHWIN1
                                                           // 12-0
    ISIF_HBLACK_256PIXEL_WIDE,                             // ISIF_HBLACK_PIXEL_WIDTH
                                                           // h_size; //CLHWM
                                                           // CLHWIN0
    ISIF_VBLACK_PIXEL_LIMITED,                             // ISIF_HBLACK_PIXEL_VALUE_LIM
                                                           // pixel_value_limit;
                                                           // //CLHLMT
                                                           // CLHWIN0
    ISIF_VBLACK_BASE_WINDOW_RIGHT,                         // ISIF_HBLACK_BASE_WINDOW_T
                                                           // right_window;
                                                           // //CLHWBS
                                                           // CLHWIN0
    1,                                                     // uint8
                                                           // window_count_per_color;
                                                           // //CLHWC CLHWIN0
    // } isif_h_black_params_t horizontal_black; //CLHWIN0
    // {
    0xA,                                                   // uint16 v_pos;
                                                           // //CLVSV CLVWIN2
                                                           // 12-0
    0xB,                                                   // uint16 v_size;
                                                           // //CLVOBV
                                                           // CLVWIN3 12-0
                                                           // range 0-8192
    0xC,                                                   // uint16 h_pos;
                                                           // //CLVSH CLVWIN1
                                                           // 12-0
    ISIF_VBLACK_64PIXEL_WIDE,                              // ISIF_VBLACK_PXL_WIDTH_T
                                                           // h_size;
                                                           // //CLVOBH
                                                           // CLVWIN0
    0xE,                                                   // uint8
                                                           // line_avg_coef;
                                                           // //CLVCOEF
                                                           // CLVWIN0
    ISIF_VALUE_NOUPDATE,                                   // ISIF_VBLACK_RESET_MODE_T
                                                           // reset_mode;
                                                           // //CLVRVSL
                                                           // CLVWIN0
    0xEA,                                                  // uint16
                                                           // reset_value;
                                                           // //CLVRV reset
                                                           // value U12

    // }isif_vertical_black_params_t vertical_black; //CLVWIN0
};

const isif_flash_cfg_t iss_default_flash_timing_params = {
    0,                                                     // uint8 enable;
                                                           // // FLSHEN
                                                           // FLSHCFG0
    3,                                                     // uint16
                                                           // flash_timing_start_line;
                                                           // // SFLSH
                                                           // FLSHCFG1
    5                                                      // uint16
                                                           // flash_timing_width;
                                                           // // VFLSH
                                                           // FLSHCFG2
};

uint8 *lsc_tbl = NULL;

IssIsifFillDefaultParams(iss_config_processing_t * iss_config)
{
    isif_2dlsc_cfg_t *iss_lsc_params =
        (isif_2dlsc_cfg_t *) & iss_default_lsc_params;
    if (NULL == lsc_tbl)
    {
        // lsc_tbl = TIMM_OSAL_Malloc(40240, TIMM_OSAL_TRUE, 256,
        // TIMMOSAL_MEM_SEGMENT_EXT);
        // memset(lsc_tbl, 0x80, 40240);
        iss_lsc_params->enable = 0;
        iss_lsc_params->gain_table_address = (uint32) lsc_tbl;
        iss_lsc_params->gain_table_length = iss_lsc_params->lsc_hsize / 16 * 4;
        iss_lsc_params->gain_format = ISIF_LSC_GAIN_0Q8_PLUS_1;
    }

    iss_config->lsc_2d = (isif_2dlsc_cfg_t *) & iss_default_lsc_params;
    iss_config->clamp = (isif_clamp_cfg_t *) & iss_default_digital_clamp_params;
    iss_config->flash = (isif_flash_cfg_t *) & iss_default_flash_timing_params;
    iss_config->gain_offset =
        (isif_gain_offset_cfg_t *) & iss_default_gain_offset_params;
    iss_config->vlcd = (isif_vldc_cfg_t *) & iss_default_vldc_params;

#if defined(APPRO_SENSOR_VENDOR) && defined(IMGS_SONY_IMX136)
	iss_config->msb_pos = IPIPE_BAYER_MSB_BIT13;
#elif defined SENSOR_12BIT	
    iss_config->msb_pos = IPIPE_BAYER_MSB_BIT11;
#else
    iss_config->msb_pos = IPIPE_BAYER_MSB_BIT9;
#endif                                                     // _PRI_SENSOR__MT9P_031

    // iss_config->f_proc_isif_validity = PROC_ISIF_VALID_ALL;
    iss_config->f_proc_isif_validity =
        (PROC_ISIF_VALID_ID) (PROC_ISIF_VALID_2DLSC | PROC_ISIF_VALID_DC);

#ifdef IMGS_PANASONIC_MN34041
    iss_config->clamp->dcoffset_clamp_value = (uint16)-252;
#elif defined IMGS_SONY_IMX136
#ifdef APPRO_SENSOR_VENDOR
	iss_config->clamp->dcoffset_clamp_value = 0;
#else
    iss_config->clamp->dcoffset_clamp_value = (uint16)-212;
#endif
#elif defined IMGS_SONY_IMX140
    iss_config->clamp->dcoffset_clamp_value = (uint16)-212;
#elif defined IMGS_MICRON_MT9M034
    iss_config->clamp->dcoffset_clamp_value = 0;
#elif defined IMGS_MICRON_AR0331_WDR
    iss_config->clamp->dcoffset_clamp_value = 0;
#elif defined IMGS_SONY_IMX104
    iss_config->clamp->dcoffset_clamp_value = (uint16)-48; //match the sensor OB setting 0x020a,0x30
#elif defined IMGS_SONY_IMX122
    iss_config->clamp->dcoffset_clamp_value = (uint16)-0x39;
#else
    iss_config->clamp->dcoffset_clamp_value = (uint16)-168; //Gang: touch for black level
#endif

}

rsz_lpf_cfg_t iss_default_rsz_h_lpf_cfg = {
    22,                                                    // c_intensity
    21                                                     // yintensity
};

rsz_lpf_cfg_t iss_default_rsz_v_lpf_cfg = {
    15,                                                    // c_intensity
    14                                                     // yintensity
};

IssRszFillDefaultParams(iss_config_processing_t * iss_config)
{
    iss_config->h_lpf = &iss_default_rsz_h_lpf_cfg;
    iss_config->v_lpf = &iss_default_rsz_v_lpf_cfg;
    iss_config->f_proc_rsz_validity = PROC_IPIPE_RSZ_VALID_ALL;
}

/* ================================================================ */
/**
 *Description:-cfg struct for Dark frame subtract
 */
 /*================================================================== */
const ipipeif_dfs_cfg_t ipipeif_dfs_cfg = {
    IPIPEIF_FEATURE_ENABLE,                                // IPIPEIF_FEATURE_SELECT_T
                                                           // dfs_gain_en;
    512,                                                   // uint16
                                                           // dfs_gain_val;/*valid
                                                           // only if
                                                           // dfs_gain_en
                                                           // =IPIPEIF_FEATURE_ENABLE
                                                           // */
    16,                                                    // uint16
                                                           // dfs_gain_thr;/*valid
                                                           // only if
                                                           // dfs_gain_en
                                                           // =IPIPEIF_FEATURE_ENABLE
                                                           // */
    255,                                                   // uint16
                                                           // oclip;/*valid
                                                           // only if
                                                           // dfs_gain_en
                                                           // =IPIPEIF_FEATURE_ENABLE
                                                           // */
    0                                                      // uint8
                                                           // dfs_dir;/*set
                                                           // to 0 if Sensor
                                                           // Parallel
                                                           // interface data
                                                           // is to be
                                                           // subtracted by
                                                           // DRK frm in
                                                           // SDRAM*/
};

/* ================================================================ */
/**
 *Description:-struct to control defect pixel corrction in ipipeif
 */
 /*================================================================== */
const ipipeif_dpc_cfg_t ipipeif_dpc_cfg = {
    IPIPEIF_FEATURE_ENABLE,                                // IPIPEIF_FEATURE_SELECT_T
                                                           // dpc2_en;
    20                                                     // uint16
                                                           // dpc2_thr;
};

const ipipeif_dpcm_cfg_t dfs_ipipeif_dpcm_cfg = {

    IPIPEIF_FEATURE_ENABLE,                                // IPIPEIF_FEATURE_SELECT_T
                                                           // dpcm_en;
    IPIPEIF_DPCM_PREDICTION_ADVANCED,                      // IPIPEIF_DPCM_PRED_TYP_T
                                                           // dpcm_predictor;/*Valid
                                                           // only if DPCM is
                                                           // enabled;
                                                           // dpcm_en=1.*/
    IPIPEIF_DPCM_BIT_SIZE_8_12                             // IPIPEIF_DPCM_BIT_SIZE_T
                                                           // dpcm_bit_size;/*Valid
                                                           // only if DPCM is
                                                           // enabled;
                                                           // dpcm_en=1*/
};

IssIpipeifFillDefaultParams(iss_config_processing_t * iss_config)
{
    iss_config->dfs = (ipipeif_dfs_cfg_t *) & ipipeif_dfs_cfg;
    iss_config->dpc1 = (ipipeif_dpc_cfg_t *) & ipipeif_dpc_cfg;
    iss_config->dpc2 = (ipipeif_dpc_cfg_t *) & ipipeif_dpc_cfg;
    iss_config->dpcm = (ipipeif_dpcm_cfg_t *) & dfs_ipipeif_dpcm_cfg;
    // iss_config->f_proc_ipipeif_validity = PROC_IPIPEIF_VALID_ALL;
    iss_config->f_proc_ipipeif_validity =
        (PROC_IPIPEIF_VALID_ID) (PROC_IPIPEIF_VALID_DFS |
                                 PROC_IPIPEIF_VALID_DPCM |
                                 PROC_IPIPEIF_VALID_DPC1 |
                                 PROC_IPIPEIF_VALID_DPC2);
}

h3a_af_iir_param h3a_iir1_params_preview = {
    {                                                      /* fir co eff */
     0x035b, 0x01ff,
     0x0de8, 0x071d,
     0x0f8a, 0x0e2d,
     0x0bd0, 0x0505,

     0x0bb8, 0x0a5c,

     0x0099},
    0x0000
};

const h3a_af_iir_param h3a_iir2_params_preview = {
    {
     0x0bb1, 0x018a,
     0x0e13, 0x0c72,
     0x059b, 0x0bb0,
     0x0dde, 0x0c3c,
     0x0fc0, 0x05d6,
     0x0e76}
    , 0x0000
};

const h3a_af_fir_param h3a_fir1_params_preview =
    { {0x01, 0x01, 0x01, 0x01, 0x01}, 0x01 };

const h3a_af_fir_param h3a_fir2_params_preview =
    { {0x01, 0x02, 0x01, 0x01, 0x01}, 0x01 };

const h3a_common_cfg_t h3a_common_params_preview = {
    H3A_FEATURE_DISABLE,                                   // aewb_alaw_en
    H3A_FEATURE_DISABLE,                                   // aewb_median_en
    0,                                                     // median_filter_threshold
    H3A_FEATURE_DISABLE,                                   // af_alaw_en
    H3A_FEATURE_DISABLE,                                   // af_median_en
    H3A_FEATURE_DISABLE,                                   // ipiprif_ave_filt_en
    H3A_FEATURE_DISABLE,
    0                                                      // reserved ?
};

const h3a_aewb_blk_dims_t h3a_aewb_blk_win_params_default = {
    0, 0
};

h3a_aewb_param_t h3a_aewb_params_default = {
    H3A_FEATURE_DISABLE,
    NULL,
    NULL,
    H3A_AEWB_OP_FMT_SUM_ONLY,
    0,
    0,
    0,
};

h3a_af_param_t h3a_af_params_default = {
    H3A_FEATURE_DISABLE,                                   // to enable the
                                                           // AF engine
    H3A_AF_RGBPOS_RG_GB_BAYER,                             // valid only if
                                                           // vertical focus
                                                           // is enabled
    H3A_FEATURE_DISABLE,
    H3A_FEATURE_DISABLE,
    0,                                                     // AFIIRSH IIRSH
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0
};

h3a_paxel_cfg_t h3a_aewb_paxel_params = { 4,
    16,
    4,
    8,
    16,
    4,
    8,
    4
};

h3a_paxel_cfg_t h3a_af_paxel_parm = {
    0, 10, 0, 10, 6, 2, 4, 2
};

IssH3aFillDefaultParams(iss_config_processing_t * iss_config)
{

    h3a_aewb_params_default.aewb_enable = H3A_FEATURE_DISABLE;
    h3a_aewb_params_default.aewb_op_addr = (uint32) NULL;
    // h3a_aewb_params_default.aewb_paxel_win =
    // &h3a_aewb_paxel_params_preview;
    // h3a_aewb_params_default.ae_op_fmt = H3A_AEWB_OP_FMT_SUM_OF_SQR;
    h3a_aewb_params_default.ae_op_fmt = H3A_AEWB_OP_FMT_SUM_ONLY;

    h3a_aewb_params_default.blk_win_dims =
        (h3a_aewb_blk_dims_t *) & h3a_aewb_blk_win_params_default;
    h3a_aewb_params_default.saturation_limit = 1000;
    h3a_aewb_params_default.shift_value = 2;

    h3a_af_params_default.af_enable = H3A_FEATURE_DISABLE;
    h3a_af_params_default.af_op_addr = (uint32) NULL;
    // h3a_af_params_default.af_paxel_win = iss_config->h3a_af_paxel_params;
    h3a_af_params_default.fir1 = (h3a_af_fir_param *) & h3a_fir1_params_preview;
    h3a_af_params_default.fir2 = (h3a_af_fir_param *) & h3a_fir2_params_preview;
    h3a_af_params_default.iir1 = (h3a_af_iir_param *) & h3a_iir1_params_preview;
    h3a_af_params_default.iir2 = (h3a_af_iir_param *) & h3a_iir2_params_preview;
    h3a_af_params_default.iir_start_pos = 2;
    h3a_af_params_default.peak_mode_en = H3A_FEATURE_DISABLE;
    h3a_af_params_default.rgb_pos = H3A_AF_RGBPOS_RG_BG_BAYER;
    h3a_af_params_default.vertical_focus_en = H3A_FEATURE_ENABLE;

    h3a_aewb_paxel_params.h_size = 2;
    h3a_aewb_paxel_params.h_incr = 2;
    h3a_aewb_paxel_params.h_pos = 0;
    h3a_aewb_paxel_params.h_count = 2;

    h3a_aewb_paxel_params.v_incr = 2;
    h3a_aewb_paxel_params.v_pos = 0;
    h3a_aewb_paxel_params.v_size = 5;
    h3a_aewb_paxel_params.v_count = 2;

    iss_config->h3a_aewb_params = &h3a_aewb_params_default;
    iss_config->h3a_af_params = &h3a_af_params_default;
    iss_config->h3a_common_params =
        (h3a_common_cfg_t *) & h3a_common_params_preview;

    iss_config->h3a_aewb_params->aewb_paxel_win = &h3a_aewb_paxel_params;
    iss_config->h3a_af_params->af_paxel_win = &h3a_af_paxel_parm;

    iss_config->f_h3a_validity = PROC_H3A_VALID_NO;        // (PROC_H3A_VALID_ID)(PROC_H3A_VALID_AEWB
                                                           // |
                                                           // PROC_H3A_VALID_AF
                                                           // |
                                                           // PROC_H3A_VALID_H3A);

}

/* ===================================================================
 *  @func     IssFillDefaultParams
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
void IssFillDefaultParams(iss_config_processing_t * iss_config)
{
    IssIsifFillDefaultParams(iss_config);
    IssIpipeFillDefaultParams(iss_config);
    IssIpipeifFillDefaultParams(iss_config);
    IssRszFillDefaultParams(iss_config);
    IssH3aFillDefaultParams(iss_config);
    // iss_config->f_compres_in = IPIPE_PROC_COMPR_NO;
    // iss_config->f_compres_out = IPIPE_PROC_COMPR_NO;
}
