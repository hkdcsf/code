/** INCLUDES ***********************************************/
#include "p18f25k20.h"
#include "DCF_typedefs.h"
#include "BZD_driver.h"
#include "STM_driver.h"
#include "INT_driver.h"

/** PROTOTYPES *********************************************/
void BZD_v_Init_f(void);
void BZD_v_Buzz_f(void);

/** DEFINES ************************************************/

/** STATICS ************************************************/
#pragma idata   /* declare statically allocated initialized variables*/

/** DECLARATIONS *******************************************/
#pragma code    /* declare executable instructions */

/* The piezo electronic buzzer has a maximal sound pressure at 5 kHz*/
void BZD_v_Buzz_f(void)
{
  u8_t ibuz = 0u;
    
    if(STM_e8_MenuSub_On == XTIM_STM_e8_BuzzerState) /* Only buzz when buzzer is enabled from menu */
    {
        while(ibuz<127u)
        {
            LATAbits.LATA4 ^= 1u;
            INT_v_Delay10TCYx(6u);  /* Wait 100usec; TCY = 1/(1MHz/4) = 4usec; 1*25*TCy=1*25*4usec = 100us= 0.1 ms, 1/(2*0.1ms) = 5 kHz */
            ibuz++;
        }
    }
    LATAbits.LATA4 = 0u;
}

void BZD_v_Init_f(void)
{    
    TRISAbits.TRISA4 = 0u;
    LATAbits.LATA4 = 0u;
}
