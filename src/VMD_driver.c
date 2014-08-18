/** INCLUDES ***********************************************/
#include "p18f25k20.h"
#include "DCF_typedefs.h"
#include "VMD_driver.h"

/** PROTOTYPES *********************************************/
void  VMD_v_InitADC_f(void);
u16_t VMD_u16_GetADCResult(u8_t p_u8_ChannnelId);
void  VMD_v_GetVoltage_f(u16_t *p_pu16_VoltageOut);

/** DEFINES ************************************************/

/** STATICS ************************************************/

/** DECLARATIONS *******************************************/
#pragma code    /* declare executable instructions */
void VMD_v_InitADC_f(void)
{
    ANSELbits.ANS4 = 1u;   /* Disable the digital input buffer */
    TRISAbits.TRISA4 = 1u; /* Disable the digital output buffer */
    
    ANSELbits.ANS3 = 1u;   /* Disable the digital input buffer */
    TRISAbits.TRISA3 = 1u; /* Disable the digital output buffer */

    /* Select ADC conversion clock */
    ADCON2bits.ADCS = 0u; /* Fosc/2 */

    /* Configure voltage reference */
    ADCON1bits.VCFG0 = 0u;
    ADCON1bits.VCFG1 = 0u;
        
    /* Select result format */
    ADCON2bits.ADFM = 1u;

    /* Select acquisition delay */
    ADCON2bits.ACQT = 2u; /* 4 TAD , 1TAD = 2usec, 7.45/2 =~4*/
}

u16_t VMD_u16_GetADCResult(u8_t p_u8_ChannnelId)
{
  u16_t u16_Result;

    /* Turn off ADC */
    ADCON0bits.ADON = 0u;

    /* Select ADC input channel */
    ADCON0bits.CHS = p_u8_ChannnelId;

    /* Turn on ADC */
    ADCON0bits.ADON = 1u;

    /* Wait for acquisition time */

    /* Start conversion by setting Go/~DONE bit*/
    ADCON0bits.GO = 1u;

    /* Wait for the ADC conversion to complete by polling the GO/~DONE bit */
    while(1u ==ADCON0bits.DONE)
    {
        ;
    }

    /* Read ADC result */
    u16_Result = ADRESH;
    u16_Result <<=8u;
    u16_Result &= 0x0300u;
    u16_Result |=ADRESL;

    return u16_Result;
}

void VMD_v_GetVoltage_f(u16_t *p_pu16_VoltageOut)
{
    const u16_t L_VMD_cu16_MaxVoltageOut = 999u;
    u16_t u16_conversion_result;
    u16_t u16_VoltageOut;

    u16_conversion_result = VMD_u16_GetADCResult(4u);

    u16_VoltageOut = 125317u / u16_conversion_result;

    if (L_VMD_cu16_MaxVoltageOut < u16_VoltageOut)
    {
        *p_pu16_VoltageOut = L_VMD_cu16_MaxVoltageOut;
    }
    else
    {
        *p_pu16_VoltageOut = u16_VoltageOut;
    }
}
