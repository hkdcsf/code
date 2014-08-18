/** INCLUDES ***********************************************/
#include "p18f25k20.h"
#include "DCF_typedefs.h"
#include "SCD_driver.h"
#include "CNV_driver.h"
#include "LCC_driver.h"
#include "INT_driver.h"
#include "LED_driver.h"
#include "CNF_config.h"
#include "ErrorCodes.h"

/** PROTOTYPES *********************************************/
static u8_t  L_SCD_u8_GetMaximalDayOfMonth_f(u8_t p_u8_MonthBin);
static u16_t L_SCD_u16_GetCentury_f(u16_t p_u16_YearBin);
static u8_t  L_SCD_u8_DetermineLeapYear_f(u16_t p_u16_YearBin);

void SCD_v_Init_f(void);
void SCD_v_Refresh_f(void);
void SCD_v_UpdateTime_f(   	   u8_t p_u8_HoursBCD,
                           	   u8_t p_u8_MinsBCD,
                           	   u8_t p_u8_SecsBCD,
                           	   u8_t p_u8_CET_CEST_Compensation);
void SCD_v_UpdateDate_f(       u8_t p_u8_YearsBCD,
                               u8_t p_u8_MonthsBCD,
                               u8_t p_u8_WeekDaysBCD,
                               u8_t p_u8_DaysBCD);

void SCD_v_IncrementYears_f(void);
void SCD_v_DecrementYears_f(void);
void SCD_v_IncrementMonths_f(void);
void SCD_v_DecrementMonths_f(void);
void SCD_v_IncrementWeekDays_f(void);
void SCD_v_DecrementWeekDays_f(void);
void SCD_v_IncrementDays_f(void);
void SCD_v_DecrementDays_f(void);
void SCD_v_IncrementHours_f(void);
void SCD_v_IncrementAlarmHours_f(void);
void SCD_v_DecrementHours_f(void);
void SCD_v_DecrementAlarmHours_f(void);
void SCD_v_IncrementMins_f(void);
void SCD_v_IncrementAlarmMins_f(void);
void SCD_v_DecrementMins_f(void);
void SCD_v_DecrementAlarmMins_f(void);
void SCD_v_IncrementSecs_f(void);
void SCD_v_DecrementSecs_f(void);
void SCD_v_DriverInterfaceTest_f(void);

static u8_t L_SCD_u8_CalculateWinterSummerOffset_f(	u8_t u8_Month_p,
													u8_t u8_Days_p,
													u8_t u8_Weekday_p,
													u8_t u8_Hours_p);

/** DEFINES ************************************************/
//#define DEBUGSCD /* if defined debug code included */

#define SCD_U8_MAXIMAL_SEC_BIN              ((u8_t)59u)
#define SCD_U8_MINIMAL_SEC_BIN              ((u8_t)0u)

#define SCD_U8_MAXIMAL_MIN_BIN              ((u8_t)59u)
#define SCD_U8_MINIMAL_MIN_BIN              ((u8_t)0u)

#define SCD_U8_MAXIMAL_HOUR_BIN             ((u8_t)23u)
#define SCD_U8_MINIMAL_HOUR_BIN             ((u8_t)0u)

#define SCD_U8_MAXIMAL_DAY_JAN_BIN          ((u8_t)31u)
#define SCD_U8_MAXIMAL_DAY_APR_BIN          ((u8_t)30u)
#define SCD_U8_MAXIMAL_DAY_FEBR_LEAP_BIN    ((u8_t)29u)
#define SCD_U8_MAXIMAL_DAY_FEBR_NOLEAP_BIN  ((u8_t)28u)
#define SCD_U8_MINIMAL_DAY_BIN              ((u8_t)1u)

#define SCD_U8_MAXIMAL_WEEKDAY_BIN          ((u8_t)7u)
#define SCD_U8_MINIMAL_WEEKDAY_BIN          ((u8_t)1u)

#define SCD_U8_MAXIMAL_MONTH_BIN            ((u8_t)12u)
#define SCD_U8_MINIMAL_MONTH_BIN            ((u8_t)1u)

#define SCD_U16_MAXIMAL_YEAR_BIN            ((u16_t)9999u)
#define SCD_U16_MINIMAL_YEAR_BIN            ((u16_t)1583u)

#define SCD_U8_JANUARY                      ((u8_t)1u)
#define SCD_U8_FEBRUARY                     ((u8_t)2u)
#define SCD_U8_MARCH                        ((u8_t)3u)
#define SCD_U8_APRIL                        ((u8_t)4u)
#define SCD_U8_MAY                          ((u8_t)5u)
#define SCD_U8_JUNE                         ((u8_t)6u)
#define SCD_U8_JULY                         ((u8_t)7u)
#define SCD_U8_AUGUST                       ((u8_t)8u)
#define SCD_U8_SEPTEMBER                    ((u8_t)9u)
#define SCD_U8_OCTOBER                      ((u8_t)10u)
#define SCD_U8_NOVEMBER                     ((u8_t)11u)
#define SCD_U8_DECEMBER                     ((u8_t)12u)

/** STATICS ************************************************/
#pragma idata   /* declare statically allocated initialized variables*/
static u16_t L_SCD_u16_CenturyBin  = 2000u;
static u8_t  L_SCD_u8_LeapYearFlag = 0u;

u8_t  SCD_u8_SecondsBin      = 0u;
u8_t  SCD_u8_MinutesBin      = 0u;
u8_t  SCD_u8_AlarmMinutesBin = 0u;
u8_t  SCD_u8_HoursBin        = 0u;
u8_t  SCD_u8_AlarmHoursBin   = 6u;
u8_t  SCD_u8_DaysBin         = CNF_U8_DATE_INIT_DAYS_BIN;
u8_t  SCD_u8_WeekDaysBin     = CNF_U8_DATE_INIT_WEEKDAYS_BIN;
u8_t  SCD_u8_MonthsBin       = CNF_U8_DATE_INIT_MONTH_BIN;
u16_t SCD_u16_YearsBin       = CNF_U16_DATE_INIT_YEARS_BIN;

u8_t L_SCD_au8_ConversionTable[7][7] = {        {1,1,1,1,1,1,0},
                                                {1,1,1,1,1,0,2},
                                                {1,1,1,1,0,2,2},
                                                {1,1,1,0,2,2,2},
                                                {1,1,0,2,2,2,2},
                                                {1,0,2,2,2,2,2},
                                                {0,2,2,2,2,2,2}
                                        };

/** DECLARATIONS *******************************************/
#pragma code    /* declare executable instructions */
static u8_t L_SCD_u8_GetMaximalDayOfMonth_f(u8_t p_u8_MonthBin)
{
    u8_t u8_MaximalDayBin = SCD_U8_MAXIMAL_DAY_JAN_BIN;

    switch(p_u8_MonthBin)
    {
    case SCD_U8_JANUARY: /* 31 days months */
    case SCD_U8_MARCH:
    case SCD_U8_MAY:
    case SCD_U8_JULY:
    case SCD_U8_AUGUST:
    case SCD_U8_OCTOBER:
    case SCD_U8_DECEMBER:
        u8_MaximalDayBin = SCD_U8_MAXIMAL_DAY_JAN_BIN;
        break;
    case SCD_U8_APRIL: /* 30 days months */
    case SCD_U8_JUNE:
    case SCD_U8_SEPTEMBER:
    case SCD_U8_NOVEMBER:
        u8_MaximalDayBin = SCD_U8_MAXIMAL_DAY_APR_BIN;
        break;
    case SCD_U8_FEBRUARY:
        if(1u == L_SCD_u8_LeapYearFlag)
        {
            u8_MaximalDayBin = SCD_U8_MAXIMAL_DAY_FEBR_LEAP_BIN;
        }
        else
        {
            u8_MaximalDayBin = SCD_U8_MAXIMAL_DAY_FEBR_NOLEAP_BIN;
        }
        break;
    default:
        LCC_v_WriteErrorCode_f(SCD_U8_ERR1);
        break;
    }
    
    return u8_MaximalDayBin;
}

static u16_t L_SCD_u16_GetCentury_f(u16_t p_u16_YearBin)
{
    u16_t u16_Century  = 2000u;
    u16_t u16_Reminder;

    u16_Reminder = p_u16_YearBin % 100u;
    u16_Century = p_u16_YearBin - u16_Reminder;

    return u16_Century;
}

static u8_t L_SCD_u8_DetermineLeapYear_f(u16_t p_u16_YearBin)
{
    u16_t u16_YearBin;
    u8_t  u8_LeapYearFlag   = 0u;
    u16_t u16_Reminder4;
    u16_t u16_Reminder100;
    u16_t u16_Reminder400;

    u16_YearBin = p_u16_YearBin;

    u16_Reminder4   = u16_YearBin % 4u;
    u16_Reminder100 = u16_YearBin % 100u;
    u16_Reminder400 = u16_YearBin % 400u;

    if( 0u == u16_Reminder4)
    {
        u8_LeapYearFlag = 1u;
    }

    if( 0u == u16_Reminder100)
    {
        u8_LeapYearFlag = 0u;
    }

    if( 0u == u16_Reminder400)
    {
        u8_LeapYearFlag = 1u;
    }

    return u8_LeapYearFlag;
}

void SCD_v_Init_f(void)
{
    ; /* intentionally left blank because of flash optimization*/
}

void SCD_v_Refresh_f(void)
{         
    SCD_v_IncrementSecs_f();
}

void SCD_v_UpdateTime_f(   u8_t p_u8_HoursBCD,
                           u8_t p_u8_MinsBCD,
                           u8_t p_u8_SecsBCD,
                           u8_t p_u8_CET_CEST_Compensation)
{
    u8_t u8_HoursBin;
    u8_t u8_MinsBin;
    u8_t u8_SecsBin;

    u8_HoursBin = CNV_u8_u8BCDtou8Dec_f(p_u8_HoursBCD);
    u8_MinsBin  = CNV_u8_u8BCDtou8Dec_f(p_u8_MinsBCD);
    u8_SecsBin  = CNV_u8_u8BCDtou8Dec_f(p_u8_SecsBCD);

    if(SCD_U8_MAXIMAL_HOUR_BIN < u8_HoursBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR2);
    }
    else if(SCD_U8_MAXIMAL_MIN_BIN < u8_MinsBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR3);
    }
    else if (SCD_U8_MAXIMAL_SEC_BIN < u8_SecsBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR4);
    }
    else
    {
    	if (SCD_U8_CET_CEST_COMPENSATION_ON == p_u8_CET_CEST_Compensation)
    	{
    		SCD_u8_HoursBin   = u8_HoursBin;
    		SCD_v_IncrementHours_f();
    		SCD_v_IncrementHours_f();
    	}
    	else
    	{
    		SCD_u8_HoursBin   = u8_HoursBin;
    	}
        SCD_u8_MinutesBin = u8_MinsBin;

    	if (SCD_U8_CET_CEST_COMPENSATION_ON == p_u8_CET_CEST_Compensation)
    	{
    		SCD_u8_SecondsBin = u8_SecsBin;
    		SCD_v_IncrementSecs_f();
    	}
    	else
    	{
    		SCD_u8_SecondsBin = u8_SecsBin;
    	}
    }
}

void SCD_v_UpdateDate_f(       u8_t p_u8_YearsBCD,
                               u8_t p_u8_MonthsBCD,
                               u8_t p_u8_WeekDaysBCD,
                               u8_t p_u8_DaysBCD)
{
    u16_t u16_YearsBin;
    u8_t u8_MonthsBin;
    u8_t u8_WeekDaysBin;
    u8_t u8_DaysBin;
    u8_t u8_MaximalDayBin;

    u16_YearsBin   = CNV_u8_u8BCDtou8Dec_f(p_u8_YearsBCD);
    u16_YearsBin  += L_SCD_u16_CenturyBin;
    L_SCD_u8_LeapYearFlag = L_SCD_u8_DetermineLeapYear_f(u16_YearsBin);
    u8_MonthsBin   = CNV_u8_u8BCDtou8Dec_f(p_u8_MonthsBCD);
    u8_WeekDaysBin = CNV_u8_u8BCDtou8Dec_f(p_u8_WeekDaysBCD);
    u8_DaysBin     = CNV_u8_u8BCDtou8Dec_f(p_u8_DaysBCD);

    u8_MaximalDayBin = L_SCD_u8_GetMaximalDayOfMonth_f(u8_MonthsBin);

    if (SCD_U16_MAXIMAL_YEAR_BIN < u16_YearsBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR5);
    }
    else if (SCD_U8_MAXIMAL_MONTH_BIN < u8_MonthsBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR6);
    }
    else if (SCD_U8_MINIMAL_MONTH_BIN > u8_MonthsBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR7);
    }
    else if (SCD_U8_MAXIMAL_WEEKDAY_BIN < u8_WeekDaysBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR8);
    }
    else if (SCD_U8_MINIMAL_WEEKDAY_BIN > u8_WeekDaysBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR9);
    }
    else if(u8_MaximalDayBin < u8_DaysBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR10);
    }
    else if (SCD_U8_MINIMAL_DAY_BIN > u8_DaysBin)
    {
        LCC_v_WriteErrorCode_f(SCD_U8_ERR11);
    }
    else
    {
        SCD_u16_YearsBin   = u16_YearsBin; /* ToDo: what RTC interrupt runs here and increments menawhile STD refresh? */
        SCD_u8_MonthsBin   = u8_MonthsBin;
        SCD_u8_WeekDaysBin = u8_WeekDaysBin;
        SCD_u8_DaysBin     = u8_DaysBin;
    }
}

void SCD_v_IncrementYears_f(void)
{
    if(SCD_U16_MAXIMAL_YEAR_BIN <= SCD_u16_YearsBin)
    {
        SCD_u16_YearsBin = SCD_U16_MINIMAL_YEAR_BIN; 
    }
    else
    {
        SCD_u16_YearsBin++;
    } 
    L_SCD_u16_CenturyBin  = L_SCD_u16_GetCentury_f(SCD_u16_YearsBin);
    L_SCD_u8_LeapYearFlag = L_SCD_u8_DetermineLeapYear_f(SCD_u16_YearsBin);
    /* TODO: update 02.29 */

}

void SCD_v_DecrementYears_f(void)
{
    if(SCD_U16_MINIMAL_YEAR_BIN == SCD_u16_YearsBin)
    {
        SCD_u16_YearsBin = SCD_U16_MAXIMAL_YEAR_BIN; 
    }    
    else
    {
        SCD_u16_YearsBin--;
    } 
    L_SCD_u16_CenturyBin  = L_SCD_u16_GetCentury_f(SCD_u16_YearsBin);
    L_SCD_u8_LeapYearFlag = L_SCD_u8_DetermineLeapYear_f(SCD_u16_YearsBin);
    /* TODO: update 02.29 */
}

void SCD_v_IncrementMonths_f(void)
{
    if(SCD_U8_MAXIMAL_MONTH_BIN <= SCD_u8_MonthsBin)
    {
        SCD_u8_MonthsBin = SCD_U8_MINIMAL_MONTH_BIN;   
        SCD_v_IncrementYears_f(); 
    }
    else
    {
        SCD_u8_MonthsBin++;
    }
}
void SCD_v_DecrementMonths_f(void)
{
    if(SCD_U8_MINIMAL_MONTH_BIN == SCD_u8_MonthsBin)
    {
        SCD_u8_MonthsBin = SCD_U8_MAXIMAL_MONTH_BIN; 
        SCD_v_DecrementYears_f(); 
    }    
    else
    {
        SCD_u8_MonthsBin--;
    }  
} 

void SCD_v_IncrementWeekDays_f(void)
{
    if(SCD_U8_MAXIMAL_WEEKDAY_BIN <= SCD_u8_WeekDaysBin)
    {
    	SCD_u8_WeekDaysBin = SCD_U8_MINIMAL_WEEKDAY_BIN;
    }
    else
    {
    	SCD_u8_WeekDaysBin++;
    }
}

void SCD_v_DecrementWeekDays_f(void)
{
    if(SCD_U8_MINIMAL_WEEKDAY_BIN == SCD_u8_WeekDaysBin)
    {
        SCD_u8_WeekDaysBin = SCD_U8_MAXIMAL_WEEKDAY_BIN;
    }
    else
    {
    	SCD_u8_WeekDaysBin--;
    }
}  
  
void SCD_v_IncrementDays_f(void)
{
    u8_t u8_MaximalDayBin = SCD_U8_MAXIMAL_DAY_JAN_BIN;
    u8_MaximalDayBin = L_SCD_u8_GetMaximalDayOfMonth_f(SCD_u8_MonthsBin);
    
    if(u8_MaximalDayBin <= SCD_u8_DaysBin)
    {
        SCD_u8_DaysBin = SCD_U8_MINIMAL_DAY_BIN;   
        SCD_v_IncrementMonths_f();
    }
    else
    {
        SCD_u8_DaysBin++;
    } 
    SCD_v_IncrementWeekDays_f();
}   

void SCD_v_DecrementDays_f(void)
{
    if(SCD_U8_MINIMAL_HOUR_BIN == SCD_u8_DaysBin)
    {
        SCD_v_DecrementMonths_f();
        SCD_u8_DaysBin = L_SCD_u8_GetMaximalDayOfMonth_f(SCD_u8_MonthsBin);
    }    
    else
    {
        SCD_u8_DaysBin--;
    } 
    SCD_v_DecrementWeekDays_f();
}   

void SCD_v_IncrementHours_f(void)
{
    if(SCD_U8_MAXIMAL_HOUR_BIN <= SCD_u8_HoursBin)
    {
        SCD_u8_HoursBin = SCD_U8_MINIMAL_HOUR_BIN;   
        SCD_v_IncrementDays_f(); 
    }
    else
    {
        SCD_u8_HoursBin++;
    }   
}    

void SCD_v_IncrementAlarmHours_f(void)
{
    if(SCD_U8_MAXIMAL_HOUR_BIN <= SCD_u8_AlarmHoursBin)
    {
        SCD_u8_AlarmHoursBin = SCD_U8_MINIMAL_HOUR_BIN;
    }
    else
    {
        SCD_u8_AlarmHoursBin++;
    }
}

void SCD_v_DecrementHours_f(void)
{
    if(SCD_U8_MINIMAL_HOUR_BIN == SCD_u8_HoursBin)
    {
        SCD_u8_HoursBin = SCD_U8_MAXIMAL_HOUR_BIN; 
        SCD_v_DecrementDays_f(); 
    }    
    else
    {
        SCD_u8_HoursBin--;
    }     
} 

void SCD_v_DecrementAlarmHours_f(void)
{
    if(SCD_U8_MINIMAL_HOUR_BIN == SCD_u8_AlarmHoursBin)
    {
        SCD_u8_AlarmHoursBin = SCD_U8_MAXIMAL_HOUR_BIN;
    }
    else
    {
        SCD_u8_AlarmHoursBin--;
    }
}

void SCD_v_IncrementMins_f(void)
{
    if(SCD_U8_MAXIMAL_MIN_BIN <= SCD_u8_MinutesBin)
    {
        SCD_u8_MinutesBin = SCD_U8_MINIMAL_MIN_BIN;   
        SCD_v_IncrementHours_f(); 
    }
    else
    {
        SCD_u8_MinutesBin++;
    }  
}

void SCD_v_IncrementAlarmMins_f(void)
{
    if(SCD_U8_MAXIMAL_MIN_BIN <= SCD_u8_AlarmMinutesBin)
    {
        SCD_u8_AlarmMinutesBin = SCD_U8_MINIMAL_MIN_BIN;
    }
    else
    {
        SCD_u8_AlarmMinutesBin++;
    }
}

void SCD_v_DecrementMins_f(void)
{
    if(SCD_U8_MINIMAL_MIN_BIN == SCD_u8_MinutesBin)
    {
        SCD_u8_MinutesBin = SCD_U8_MAXIMAL_MIN_BIN; 
        SCD_v_DecrementHours_f(); 
    }    
    else
    {
        SCD_u8_MinutesBin--;
    }   
} 

void SCD_v_DecrementAlarmMins_f(void)
{
    if(SCD_U8_MINIMAL_MIN_BIN == SCD_u8_AlarmMinutesBin)
    {
        SCD_u8_AlarmMinutesBin = SCD_U8_MAXIMAL_MIN_BIN;
    }
    else
    {
        SCD_u8_AlarmMinutesBin--;
    }
}

void SCD_v_IncrementSecs_f(void)
{
    if(SCD_U8_MAXIMAL_SEC_BIN <= SCD_u8_SecondsBin)
    {
        SCD_u8_SecondsBin = SCD_U8_MINIMAL_SEC_BIN;   
        SCD_v_IncrementMins_f(); 
    }
    else
    {
        SCD_u8_SecondsBin++;
    }         
}    

void SCD_v_DecrementSecs_f(void)
{
    if(SCD_U8_MINIMAL_SEC_BIN == SCD_u8_SecondsBin)
    {
        SCD_u8_SecondsBin = SCD_U8_MAXIMAL_SEC_BIN; 
        SCD_v_DecrementMins_f(); 
    }    
    else
    {
        SCD_u8_SecondsBin--;
    }
} 

/* 

DCF77 interface:

Minutes:                bits 40 20 10 8 4 2 1           0-59
Hours:                  bits 20 10 8 4 2 1              0-23
Day of months:          bits 20 10 8 4 2 1              1-31
Day of weeks:           bits 4 2 1                      Monday=1, Sunday=7
Month number:           bits 10 8 4 2 1                 January=1, December=12
Year within century:    bits 80 40 20 10 8 4 2 1        0-99

*/

void SCD_v_DriverInterfaceTest_f(void)
{
#ifdef DEBUGSCD
    static u8_t  u8_stop = 0u;
    static u8_t  u8_Result = 0u;
    static u8_t  u8_executetest = 0u;
    static u8_t  u8_testparameter = 0u;
    static u16_t u16_testparameter = 0u;
    static u16_t u16_Result = 0u;
    static u8_t  u8_HoursBCD = 0u;
    static u8_t  u8_MinsBCD = 0u;
    static u8_t  u8_SecsBCD = 0u;
    static u8_t  u8_YearsBCD = 0u; 
    static u8_t  u8_MonthsBCD = 0u;
    static u8_t  u8_WeekDaysBCD = 0u;
    static u8_t  u8_DaysBCD = 0u;

    L_SCD_u8_LeapYearFlag = 1u;
    u8_stop = 1u;
    /* Input: 0..13, 255
     * Runtime: 200usec, 35msec
     */
    while(1u == u8_executetest)
    {  
        LED_v_SwitchOnLED1_f();
        u8_Result = L_SCD_u8_GetMaximalDayOfMonth_f(u8_testparameter);
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    /* Input: 0, 1, 99, 100, 101, 1999, 2000, 2001, 8999, 9000, 9999, 10000
    * Runtime: 1.2 msec
    */
    while(1u == u8_executetest)
    { 
        LED_v_SwitchOnLED1_f();  
        u16_Result = L_SCD_u16_GetCentury_f(u16_testparameter);
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }

    u8_stop = 1u;
    /* Input: 0, 96, 100, 1800, 1801, 1804, 1999, 2001, 2012, 2013, 2300, 2400, 2404, 65535
     * Runtime: 2.5 msec
     */
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();   
        u8_Result = L_SCD_u8_DetermineLeapYear_f(u16_testparameter);
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }

    u8_stop = 1u;
    /* Input: 80 40 20 10, 8, 4, 2, 1
     *        00, 00, 00
     *        23, 59, 59
     *        24, 59, 59
     *        23, 60, 59
     *        23, 59, 60
     * Runtime: 1msec, 35 msec when error reported
     */
    while(1u == u8_executetest)
    { 
        LED_v_SwitchOnLED1_f();
        SCD_v_UpdateTime_f(u8_HoursBCD,u8_MinsBCD,u8_SecsBCD);
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }

    u8_stop = 1u;
    /* Input: 80 40 20 10, 8, 4, 2, 1
     *        0x00, 0x01, 0x01, 0x01
     *        0x99, 0x01, 0x01, 0x01
     *        0x89, 0x00, 0x01, 0x01
     *        0x89, 0x13, 0x01, 0x01
     *        0x89, 0x12, 0x00, 0x01
     *        0x89, 0x12, 0x08, 0x01
     *        0x89, 0x12, 0x01, 0x00
     *        0x89, 0x12, 0x01, 0x32
     *        0x89, 0x13, 0x00, 0x32
     * Runtime: 4, 38 or 72 msec when error reported
     */
    while(1u == u8_executetest)
    {       
        LED_v_SwitchOnLED1_f();                    
        SCD_v_UpdateDate_f(u8_YearsBCD,u8_MonthsBCD,u8_WeekDaysBCD,u8_DaysBCD);
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }                                                 

    u8_stop = 1u;
    while(1u == u8_executetest)
    { 
        LED_v_SwitchOnLED1_f();
        SCD_v_IncrementYears_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    { 
        LED_v_SwitchOnLED1_f();
        SCD_v_DecrementYears_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    { 
        LED_v_SwitchOnLED1_f();
        SCD_v_IncrementMonths_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_DecrementMonths_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_IncrementWeekDays_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_DecrementWeekDays_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }  
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_IncrementDays_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_DecrementDays_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_IncrementHours_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_DecrementHours_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_IncrementMins_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_DecrementMins_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_IncrementSecs_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
    
    u8_stop = 1u;
    while(1u == u8_executetest)
    {
        LED_v_SwitchOnLED1_f();
        SCD_v_DecrementSecs_f();
        LED_v_SwitchOffLED1_f();
        u8_executetest = 0u;
    }
#endif
}


static u8_t L_SCD_u8_CalculateWinterSummerOffset_f(	u8_t u8_Month_p,
													u8_t u8_Days_p,
													u8_t u8_Weekday_p,
													u8_t u8_Hours_p)
{
    u8_t u8_offset = 0u;

    switch(u8_Month_p)
    {
    case SCD_U8_JANUARY:
    case SCD_U8_FEBRUARY:
    case SCD_U8_NOVEMBER:
    case SCD_U8_DECEMBER:
        u8_offset = 1u;
        break;
    case SCD_U8_APRIL:
    case SCD_U8_MAY:
    case SCD_U8_JUNE:
    case SCD_U8_JULY:
    case SCD_U8_AUGUST:
    case SCD_U8_SEPTEMBER:
    	u8_offset = 2u;
        break;
    case SCD_U8_MARCH:
    case SCD_U8_OCTOBER:
    	{
			if(u8_Days_p<25u)
			{
				u8_offset = 1u;
			}
			else if (u8_Days_p<32u)
			{

				//if(u8_Sunday == u8_Weekday_p)
				if(7u == u8_Weekday_p) //Todo: correct
				{
					if(u8_Hours_p < 01u)
					{
							u8_offset = 1u;
					}
					else
					{
							u8_offset = 2u;
					}
				}
				else
				{
					u8_offset = L_SCD_au8_ConversionTable[u8_Weekday_p][u8_Days_p-25u];
				}
			}
			else
			{
				u8_offset = 0u;
				/* Report error */
			}

            if(u8_Month_p == SCD_U8_OCTOBER)
			{
				if( 1u == u8_offset )
				{
						u8_offset = 2u;
				}
				else
				{
						u8_offset = 1u;
				}
			}
        }
        break;
    default:
    	u8_offset = 0u;
        /* Report error */
        break;
    }

    return u8_offset;
}

/*
                25        26        27        28        29        30        31
                M        T        W        T        F        S        S
                T        W        T        F        S        S        M
                W        T        F        S        S        M        T
                T        F        S        S        M        T        W
                F        S        S        M        T        W        T
                S        S        M        T        W        T        F
                S        M        T        W        T        F        S
 */



