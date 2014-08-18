/** INCLUDES ***********************************************/
#include "DCF_typedefs.h"
#include "STM_driver.h"
#include "SCD_driver.h"
#include "DCF_driver.h"
#include "GPS_driver.h"
#include "LED_driver.h"

/** PROTOTYPES *********************************************/
static void L_STM_v_RefreshDate_f( u8_t p_u8_UpperButtonPressed,
                                   u8_t p_u8_MiddleButtonPressed,
                                   u8_t p_u8_LowerButtonPressed);

static void L_STM_v_RefreshTime_f( u8_t p_u8_UpperButtonPressed,
                                   u8_t p_u8_MiddleButtonPressed,
                                   u8_t p_u8_LowerButtonPressed);

static void L_STM_v_RefreshMenu_f( u8_t p_u8_UpperButtonPressed,
                                   u8_t p_u8_MiddleButtonPressed,
                                   u8_t p_u8_LowerButtonPressed);
void STM_v_Init_f(void);

void STM_v_Refresh_f(u8_t p_u8_UpperButtonPressed,
                     u8_t p_u8_MiddleButtonPressed,
                     u8_t p_u8_LowerButtonPressed);

/** DEFINES ************************************************/
//
/** STATICS ************************************************/
#pragma idata   /* declare statically allocated initialized variables*/
STM_e8_SM_MenuSub_t                  STM_e8_ReceiverState          = STM_e8_MenuSub_GPS;
STM_e8_SM_MenuSub_t                  STM_e8_BuzzerState            = STM_e8_MenuSub_On;
STM_e8_SM_MenuSub_t                  STM_e8_BackLightState         = STM_e8_MenuSub_On;
STM_e8_SM_MenuSub_t                  STM_e8_SetAlarmState          = STM_e8_MenuSub_Off;
u8_t                                 STM_u8_AlarmDaySelection      = 0x3Eu; /* weekdays */
u8_t                                 STM_u8_AlarmDay2Invert        = 0x01u;
STM_e8_SM_MainStates_t               STM_e8_StateMachine           = STM_e8_StandAloneClock;
STM_e8_SM_DateSettingStates_t        STM_e8_StateMachineDate       = STM_e8_SetYears;
STM_e8_SM_TimeSettingStates_t        STM_e8_StateMachineTime       = STM_e8_SetHours;
STM_e8_SM_SetMenuStates_t            STM_e8_StateMachineMenu       = STM_e8_MenuSetReceiver;

/** DECLARATIONS *******************************************/
#pragma code    /* declare executable instructions */
static void L_STM_v_RefreshDate_f( u8_t p_u8_UpperButtonPressed,
                                   u8_t p_u8_MiddleButtonPressed,
                                   u8_t p_u8_LowerButtonPressed)
{
    switch(STM_e8_StateMachineDate)
    {
    case STM_e8_SetYears:
        {
            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_StateMachineDate = STM_e8_SetMonths;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                SCD_v_IncrementYears_f();
            }

            if(1u == p_u8_LowerButtonPressed )
            {
                SCD_v_DecrementYears_f();
            }
        }
        break;
    case STM_e8_SetMonths:
        {
            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_StateMachineDate = STM_e8_SetDays;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                SCD_v_IncrementMonths_f();
            }

            if(1u == p_u8_LowerButtonPressed )
            {
                SCD_v_DecrementMonths_f();
            }
        }
        break;
    case STM_e8_SetDays:
        {
            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_StateMachineDate = STM_e8_SetWeekDays;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                SCD_v_IncrementDays_f();
            }

            if(1u == p_u8_LowerButtonPressed )
            {
                SCD_v_DecrementDays_f();
            }
        }
        break;
    case STM_e8_SetWeekDays:
        {
            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_StateMachineTime = STM_e8_SetHours;
                STM_e8_StateMachine =  STM_e8_SetTime;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                SCD_v_IncrementWeekDays_f();
            }

            if(1u == p_u8_LowerButtonPressed )
            {
                SCD_v_DecrementWeekDays_f();
            }
        }
        break;
    default:
        STM_e8_StateMachine =  STM_e8_StandAloneClock;
        break;
    };
}

static void L_STM_v_RefreshTime_f( u8_t p_u8_UpperButtonPressed,
                                   u8_t p_u8_MiddleButtonPressed,
                                   u8_t p_u8_LowerButtonPressed)
{
    switch(STM_e8_StateMachineTime)
    {
    case STM_e8_SetHours:
        {
            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_StateMachineTime = STM_e8_SetMinutes;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                SCD_v_IncrementHours_f();
            }

            if(1u == p_u8_LowerButtonPressed )
            {
                SCD_v_DecrementHours_f();
            }
        }
        break;
    case STM_e8_SetMinutes:
        {
            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_StateMachineTime = STM_e8_SetSeconds;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                SCD_v_IncrementMins_f();
            }
               
            if(1u == p_u8_LowerButtonPressed )
            {
                SCD_v_DecrementMins_f();
            }
        }
        break;
    case STM_e8_SetSeconds:
        {
            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_StateMachine =  STM_e8_StandAloneClock;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                SCD_v_IncrementSecs_f();
            }
             
            if(1u == p_u8_LowerButtonPressed )
            {
                SCD_v_DecrementSecs_f();
            }
        }
        break;
    default:
        STM_e8_StateMachine =  STM_e8_StandAloneClock;
        break;
    };
}

static void L_STM_v_RefreshMenu_f( u8_t p_u8_UpperButtonPressed,
                                   u8_t p_u8_MiddleButtonPressed,
                                   u8_t p_u8_LowerButtonPressed)
{
    switch(STM_e8_StateMachineMenu)
    {
    case STM_e8_MenuSetReceiver:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetBuzzer;
            }
            
            switch (STM_e8_ReceiverState)
            {
            case STM_e8_MenuSub_GPS:
            	{
                    if(1u == p_u8_UpperButtonPressed )
                    {
                        STM_e8_ReceiverState = STM_e8_MenuSub_Off;
                        DCF_v_SwitchDCFModuleOff_f();
                        GPS_v_SwitchGPSModuleOff_f();
                    }

                    if(1u == p_u8_MiddleButtonPressed )
                    {
                        STM_e8_ReceiverState = STM_e8_MenuSub_DCF;
                        DCF_v_SwitchDCFModuleOn_f();
                        GPS_v_SwitchGPSModuleOff_f();
                    }
            	}
            	break;

            case STM_e8_MenuSub_DCF:
            	{
                    if(1u == p_u8_UpperButtonPressed )
                    {
                        STM_e8_ReceiverState = STM_e8_MenuSub_GPS;
                        DCF_v_SwitchDCFModuleOff_f();
                        GPS_v_SwitchGPSModuleOn_f();
                    }

                    if(1u == p_u8_MiddleButtonPressed )
                    {
                        STM_e8_ReceiverState = STM_e8_MenuSub_Off;
                        DCF_v_SwitchDCFModuleOff_f();
                        GPS_v_SwitchGPSModuleOff_f();
                    }
            	}
            	break;
            
            case STM_e8_MenuSub_Off:
            	{
                    if(1u == p_u8_UpperButtonPressed )
                    {
                        STM_e8_ReceiverState = STM_e8_MenuSub_DCF;
                        DCF_v_SwitchDCFModuleOn_f();
                        GPS_v_SwitchGPSModuleOff_f();
                    }

                    if(1u == p_u8_MiddleButtonPressed )
                    {
                        STM_e8_ReceiverState = STM_e8_MenuSub_GPS;
                        DCF_v_SwitchDCFModuleOff_f();
                        GPS_v_SwitchGPSModuleOn_f();
                    }
            	}
            	break;

            };
        }
        break;
    case STM_e8_MenuSetBuzzer:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetBacklight;
            }
            
            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_BuzzerState = STM_e8_MenuSub_On;
            }
            
            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_e8_BuzzerState = STM_e8_MenuSub_Off;
            }
         }
         break;
    case STM_e8_MenuSetBacklight:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetAlarm;
            }
            
            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_BackLightState = STM_e8_MenuSub_On;
                LED_v_SwitchOnLCDBackLight_f();
            }
            
            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_e8_BackLightState = STM_e8_MenuSub_Off;
                LED_v_SwitchOffLCDBackLight_f();
            }
        }
        break;
    case STM_e8_MenuSetAlarm:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                if (STM_e8_MenuSub_On == STM_e8_SetAlarmState)
                {
                    STM_e8_StateMachineMenu = STM_e8_MenuSetAlarmTimeHours;
                }
                else
                {
                    STM_e8_StateMachine = STM_e8_StandAloneClock;
                }
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                STM_e8_SetAlarmState = STM_e8_MenuSub_On;
            }
            
            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_e8_SetAlarmState = STM_e8_MenuSub_Off;
            }
        }
        break;
    case STM_e8_MenuSetAlarmTimeHours:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetAlarmTimeMinutes;
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                SCD_v_IncrementAlarmHours_f();
            }
            
            if(1u == p_u8_MiddleButtonPressed )
            {
                SCD_v_DecrementAlarmHours_f();
            }
        }
        break;
    case STM_e8_MenuSetAlarmTimeMinutes:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetAlarmDaysMonday;
                STM_u8_AlarmDay2Invert = 1u; /* ToDo: this is redundat with the state, check where it is used*/
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                SCD_v_IncrementAlarmMins_f();
            }
            
            if(1u == p_u8_MiddleButtonPressed )
            {
                SCD_v_DecrementAlarmMins_f();
            }
        }
        break;
    case STM_e8_MenuSetAlarmDaysMonday:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetAlarmDaysTuesday;
                STM_u8_AlarmDay2Invert = 2u;
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                STM_u8_AlarmDaySelection |= 0x01u;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_u8_AlarmDaySelection &= ~0x01u;
            }
        }
        break;
    case STM_e8_MenuSetAlarmDaysTuesday:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetAlarmDaysWednesday;
                STM_u8_AlarmDay2Invert = 3u;
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                STM_u8_AlarmDaySelection |= 0x02u;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_u8_AlarmDaySelection &= ~0x02u;
            }
        }
        break;
    case STM_e8_MenuSetAlarmDaysWednesday:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetAlarmDaysThursday;
                STM_u8_AlarmDay2Invert = 4u;
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                STM_u8_AlarmDaySelection |= 0x04u;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_u8_AlarmDaySelection &= ~0x04u;
            }
        }
        break;
    case STM_e8_MenuSetAlarmDaysThursday:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetAlarmDaysFriday;
                STM_u8_AlarmDay2Invert = 5u;
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                STM_u8_AlarmDaySelection |= 0x08u;
            }
            
            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_u8_AlarmDaySelection &= ~0x08u;
            }
        }
        break;
    case STM_e8_MenuSetAlarmDaysFriday:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetAlarmDaysSaturday;
                STM_u8_AlarmDay2Invert = 6u;
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                STM_u8_AlarmDaySelection |= 0x10u;
            }
            
            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_u8_AlarmDaySelection &= ~0x10u;
            }
        }
        break;
    case STM_e8_MenuSetAlarmDaysSaturday:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachineMenu = STM_e8_MenuSetAlarmDaysSunday;
                STM_u8_AlarmDay2Invert = 7u;
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                STM_u8_AlarmDaySelection |= 0x20u;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_u8_AlarmDaySelection &= ~0x20u;
            }
        }
        break;
    case STM_e8_MenuSetAlarmDaysSunday:
        {
            if(1u == p_u8_LowerButtonPressed )
            {
                STM_e8_StateMachine = STM_e8_StandAloneClock;
                STM_u8_AlarmDay2Invert = 1u;
            }

            if(1u == p_u8_UpperButtonPressed )
            {
                STM_u8_AlarmDaySelection |= 0x40u;
            }

            if(1u == p_u8_MiddleButtonPressed )
            {
                STM_u8_AlarmDaySelection &= ~0x40u;
            }
        }
        break;
    default:
        STM_e8_StateMachine = STM_e8_StandAloneClock;
        break;
    };
}

void STM_v_Init_f(void)
{
    switch (STM_e8_ReceiverState)
    {
    case STM_e8_MenuSub_GPS:
        DCF_v_SwitchDCFModuleOff_f();
        GPS_v_SwitchGPSModuleOn_f();
        break;
    case STM_e8_MenuSub_DCF:
        DCF_v_SwitchDCFModuleOn_f();
        GPS_v_SwitchGPSModuleOff_f();
        break;
   default:
        DCF_v_SwitchDCFModuleOff_f();
        GPS_v_SwitchGPSModuleOff_f();
        break;
    }

    if (STM_e8_BackLightState = STM_e8_MenuSub_On)
    {
        LED_v_SwitchOnLCDBackLight_f();
    }
    else
    {
        LED_v_SwitchOffLCDBackLight_f();
    }
}

void STM_v_Refresh_f(u8_t p_u8_UpperButtonPressed,
                     u8_t p_u8_MiddleButtonPressed,
                     u8_t p_u8_LowerButtonPressed)
{
     switch(STM_e8_StateMachine)
     {
     case STM_e8_StandAloneClock:
         {
             if(1u == p_u8_MiddleButtonPressed )
             {
                  STM_e8_StateMachine = STM_e8_WriteBuildInfo;
             }

             if(1u == p_u8_UpperButtonPressed )
             {
                  STM_e8_StateMachine = STM_e8_SetDate;
                  STM_e8_StateMachineDate = STM_e8_SetYears;
                  STM_e8_StateMachineTime = STM_e8_SetHours;
             }

             if(1u == p_u8_LowerButtonPressed )
             {
                  STM_e8_StateMachine = STM_e8_SetMenu;
                  STM_e8_StateMachineMenu = STM_e8_MenuSetReceiver;
             }
         }
         break;
     case STM_e8_WriteBuildInfo:
         {
             if(1u == p_u8_MiddleButtonPressed )
             {
                  STM_e8_StateMachine = STM_e8_StandAloneClock;
             }

             if(1u == p_u8_LowerButtonPressed )
             {
                  STM_e8_StateMachine = STM_e8_WriteDisplayTest;
             }

             if(1u == p_u8_UpperButtonPressed )
             {
                  STM_e8_StateMachine = STM_e8_WriteGPSData;
                  GPS_v_SwitchGPSModuleOn_f();
             }
         }
         break;
     case STM_e8_WriteDisplayTest:
         {
             if(1u == p_u8_LowerButtonPressed )
             {
                  STM_e8_StateMachine = STM_e8_StandAloneClock;
             }
         }
         break;
     case STM_e8_WriteGPSData:
         if(1u == p_u8_UpperButtonPressed )
         {
              STM_e8_StateMachine = STM_e8_WriteGPSSentence;
         }
    	 break;
     case STM_e8_WriteGPSSentence:
         if(1u == p_u8_UpperButtonPressed )
         {
              STM_e8_StateMachine = STM_e8_StandAloneClock;
         }
    	 break;
     case STM_e8_SetDate:
         {
             L_STM_v_RefreshDate_f(p_u8_UpperButtonPressed, p_u8_MiddleButtonPressed, p_u8_LowerButtonPressed);
         }
         break;
     case STM_e8_SetTime:
         {
             L_STM_v_RefreshTime_f(p_u8_UpperButtonPressed, p_u8_MiddleButtonPressed, p_u8_LowerButtonPressed);
         }
         break;
     case STM_e8_SetMenu:
         {
             L_STM_v_RefreshMenu_f(p_u8_UpperButtonPressed, p_u8_MiddleButtonPressed, p_u8_LowerButtonPressed);
         }
         break;
     default:
         STM_e8_StateMachine = STM_e8_StandAloneClock;
         break;
     }
}

void STM_v_SetUnderVoltage_f(void)
{
    STM_e8_StateMachine = STM_e8_UnderVoltage;
}



