#ifndef STM_DRIVER /* prevent multiple include */
#define STM_DRIVER

typedef enum STM_e8_SM_MainStates_tag
{
    STM_e8_StandAloneClock           = 2u,
    STM_e8_WriteBuildInfo            = 3u,
    STM_e8_SetDate   	             = 4u,
    STM_e8_SetTime  	             = 5u,
    STM_e8_SetMenu                   = 6u,
    STM_e8_UnderVoltage              = 7u,
    STM_e8_WriteDisplayTest		     = 8u,
    STM_e8_WriteGPSData			     = 9u,
    STM_e8_WriteGPSSentence		     = 10u,
    STM_e8_Invalid                   = 255u
} STM_e8_SM_MainStates_t;

typedef enum STM_e8_SM_DateSettingStates_tag
{
    STM_e8_SetYears		             = 1u,
    STM_e8_SetMonths	             = 2u,
    STM_e8_SetDays		             = 3u,
    STM_e8_SetWeekDays	             = 4u,
    STM_e8_Invalid                   = 255u
} STM_e8_SM_DateSettingStates_t;

typedef enum STM_e8_SM_TimeSettingStates_tag
{
    STM_e8_SetHours		             = 5u,
    STM_e8_SetMinutes	             = 6u,
    STM_e8_SetSeconds	             = 7u,
    STM_e8_Invalid                   = 255u
} STM_e8_SM_TimeSettingStates_t;

typedef enum STM_e8_SM_SetMenuStates_tag
{
    STM_e8_MenuSetReceiver           = 1u,
    STM_e8_MenuSetBuzzer             = 2u,
    STM_e8_MenuSetBacklight          = 3u,
    STM_e8_MenuSetAlarm              = 4u,
    STM_e8_MenuSetAlarmTimeHours     = 5u,
    STM_e8_MenuSetAlarmTimeMinutes   = 6u,
    STM_e8_MenuSetAlarmDaysSunday    = 7u,
    STM_e8_MenuSetAlarmDaysMonday    = 8u,
    STM_e8_MenuSetAlarmDaysTuesday   = 9u,
    STM_e8_MenuSetAlarmDaysWednesday = 10u,
    STM_e8_MenuSetAlarmDaysThursday  = 11u,
    STM_e8_MenuSetAlarmDaysFriday    = 12u,
    STM_e8_MenuSetAlarmDaysSaturday  = 13u,
    STM_e8_Invalid                   = 255u
} STM_e8_SM_SetMenuStates_t;

typedef enum STM_e8_SM_MenuSub_tag
{
    STM_e8_MenuSub_Off      = 0u,
    STM_e8_MenuSub_On       = 1u,
    STM_e8_MenuSub_GPS      = 2u,
    STM_e8_MenuSub_DCF      = 3u,
    STM_e8_MenuSub_Invalid  = 255u
} STM_e8_SM_MenuSub_t;

#define XTIM_STM_e8_ReceiverState          ((STM_e8_SM_MenuSub_t)STM_e8_ReceiverState)
#define XTIM_STM_e8_BuzzerState            ((STM_e8_SM_MenuSub_t)STM_e8_BuzzerState)
#define XTIM_STM_e8_BackLightState         ((STM_e8_SM_MenuSub_t)STM_e8_BackLightState)
#define XTIM_STM_e8_SetAlarmState          ((STM_e8_SM_MenuSub_t)STM_e8_SetAlarmState)
#define XTIM_STM_u8_AlarmDaySelection      ((u8_t)STM_u8_AlarmDaySelection)
#define XTIM_STM_u8_AlarmDay2Invert        ((u8_t)STM_u8_AlarmDay2Invert)
#define XTIM_STM_e8_StateMachine           ((STM_e8_SM_MainStates_t)STM_e8_StateMachine)
#define XINT_STM_e8_StateMachine           ((STM_e8_SM_MainStates_t)STM_e8_StateMachine)
#define XTIM_STM_e8_StateMachineDate       ((STM_e8_SM_DateSettingStates_t)STM_e8_StateMachineDate)
#define XTIM_STM_e8_StateMachineTime       ((STM_e8_SM_TimeSettingStates_t)STM_e8_StateMachineTime)
#define XTIM_STM_e8_StateMachineMenu       ((STM_e8_SM_SetMenuStates_t)STM_e8_StateMachineMenu)

extern STM_e8_SM_MenuSub_t                  STM_e8_ReceiverState;
extern STM_e8_SM_MenuSub_t                  STM_e8_BuzzerState;
extern STM_e8_SM_MenuSub_t                  STM_e8_BackLightState;
extern STM_e8_SM_MenuSub_t                  STM_e8_SetAlarmState;
extern u8_t                                 STM_u8_AlarmDaySelection;
extern u8_t                                 STM_u8_AlarmDay2Invert;
extern STM_e8_SM_MainStates_t               STM_e8_StateMachine;
extern STM_e8_SM_DateSettingStates_t        STM_e8_StateMachineDate;
extern STM_e8_SM_TimeSettingStates_t        STM_e8_StateMachineTime;
extern STM_e8_SM_SetMenuStates_t            STM_e8_StateMachineMenu;

extern void        STM_v_Init_f(void);
extern void        STM_v_Refresh_f(u8_t p_u8_UpperButtonPressed,
                                   u8_t p_u8_MiddleButtonPressed,
                                   u8_t p_u8_LowerButtonPressed);
extern void        STM_v_SetUnderVoltage_f(void);

#endif
