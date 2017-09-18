//void ApplyLBCE(Word16* inputImage, Word16* outputImage, Word16 imgSizeV, Word16 imgSizeH, Word16* lbceCalibration, Byte* toneCurves);
//short ApplyLBCE(Word16* inputImage,Word16 imgSizeV, Word16 imgSizeH, Word16* lbceCalibration, Byte* toneCurves);
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
short ApplyLBCE(GLBCE_CONTEXT* glbceCon,Word16* inputImage,Word16 imgSizeV, Word16 imgSizeH, Word16* lbceCalibration1, Word16* lbceCalibration2, Word16* lbceCalibration3, Byte* toneCurves);
short getToneCurvesHorzVertNum(GLBCE_CONTEXT* glbceCon, Word16 imgSizeV, Word16 imgSizeH, Byte *numVertBlocks, Byte *numHorzBlocks, Word16 *pBlkShiftV, Word16 *pBlkShiftH);
void getTheSmartGLBCEStrengths(unsigned int gGlbceEnable, char isSystemStabilized, int sensorGain, short WB_RGain, short WB_GrGain, short WB_GbGain, short WB_BGain, 
			 short *GBEStrength,
			 short *GCEStrength,
			 short *LBEStrength,
			 short *LCEStrength);
#ifdef __cplusplus
}
#endif /* __cplusplus */
