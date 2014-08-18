/** INCLUDES ***********************************************/
#include "p18f25k20.h"
#include "DCF_typedefs.h"
#include "TMD_driver.h"
#include "VMD_driver.h"

/** PROTOTYPES *********************************************/
void TMD_v_GetTemp_f(s16_t *p_ps16_TempOut);
/** DEFINES ************************************************/

/** STATICS ************************************************/

/** DECLARATIONS *******************************************/
#pragma code    /* declare executable instructions */
void TMD_v_GetTemp_f(s16_t *p_s16_TempOut)
{
    const s16_t L_TMD_cs16_MaxTempOut =  9999; /*  99.99 Celsius */
    const s16_t L_TMD_cs16_MinTempOut = -2100; /* -21.00 Celsius */

    u16_t u16_conversion_result_LM20;
    u16_t u16_conversion_result_1V2;
    s32_t s32_Temp;
    s16_t s16_TempOut;

    u16_conversion_result_LM20 = VMD_u16_GetADCResult(3u);
    u16_conversion_result_1V2  = VMD_u16_GetADCResult(4u);

    s32_Temp = u16_conversion_result_LM20;

    s32_Temp *= 10511;
    s32_Temp = s32_Temp/ u16_conversion_result_1V2;
    s32_Temp = 16002 - s32_Temp;

    s16_TempOut =  (u16_t)s32_Temp;

    /* Limiter */
    if (L_TMD_cs16_MaxTempOut < s16_TempOut)
    {
        *p_s16_TempOut =  L_TMD_cs16_MaxTempOut;
    }
    else if (L_TMD_cs16_MinTempOut > s16_TempOut)
    {
        *p_s16_TempOut =  L_TMD_cs16_MinTempOut;
    }
    else
    {
        *p_s16_TempOut =  (u16_t)s32_Temp;
    }
}


