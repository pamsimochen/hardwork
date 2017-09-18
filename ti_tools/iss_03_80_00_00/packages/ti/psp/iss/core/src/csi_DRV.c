
#include <ti/psp/iss/core/msp_types.h>
#include <ti/psp/iss/core/inc/csi_MSP.h>



MSP_ERROR_TYPE CSI2_UserParamsCfgIO(IssCSI2DevT deviceNum, MSP_CsiDev* pCsiDevStruct, MSP_PTR pHwCfg, MSP_U32 numLanes)
{
    CAM_CSI_UserCfg *pUsrCfg = pHwCfg;
    MSP_S8 tmpcnt = numLanes&0x000000FF;

    if(pUsrCfg->nValid & CAM_CSI_VALID_CAOMPLEXIO){
        memcpy(&(pCsiDevStruct->tComplexIO), (CAM_CSI_complexio_cfg*)pUsrCfg->pPrm, sizeof(CAM_CSI_complexio_cfg));
        pCsiDevStruct->tComplexIO.data1_position = (CAM_CSI2_LANE_POSITION_T)(tmpcnt-- > 0)?(CAM_CSI2_POSITION_2):(CAM_CSI2_NOT_USED);
        pCsiDevStruct->tComplexIO.data2_position = (CAM_CSI2_LANE_POSITION_T)(tmpcnt-- > 0)?(CAM_CSI2_POSITION_3):(CAM_CSI2_NOT_USED);
        pCsiDevStruct->tComplexIO.data3_position = (CAM_CSI2_LANE_POSITION_T)(tmpcnt-- > 0)?(CAM_CSI2_POSITION_4):(CAM_CSI2_NOT_USED);
        pCsiDevStruct->tComplexIO.data4_position = (CAM_CSI2_LANE_POSITION_T)(tmpcnt-- > 0)?(CAM_CSI2_POSITION_5):(CAM_CSI2_NOT_USED);
        if(deviceNum==ISS_CSI_DEV_A){
            pCsiDevStruct->tIssCsi2Config.complexio_cfg_1 = (csi2_complexio_cfg_t*)&(pCsiDevStruct->tComplexIO);
        }else if(deviceNum==ISS_CSI_DEV_B){
            pCsiDevStruct->tIssCsi2Config.complexio_cfg_2 = (csi2_complexio_cfg_t*)&(pCsiDevStruct->tComplexIO);
        }
    }
    return MSP_ERROR_NONE;
}
