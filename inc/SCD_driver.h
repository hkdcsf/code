#ifndef SCD_DRIVER /* prevent multiple include */
#define SCD_DRIVER

extern void SCD_v_Init_f(void);
extern void SCD_v_Refresh_f(void);
extern void SCD_v_UpdateTime_f(       u8_t p_u8_HoursBCD,
        							  u8_t p_u8_MinsBCD,
        							  u8_t p_u8_SecsBCD,
        							  u8_t p_u8_CET_CEST_Compensation);

extern void SCD_v_UpdateDate_f(       u8_t p_u8_YearsBCD,
                                      u8_t p_u8_MonthsBCD,
                                      u8_t p_u8_WeekDaysBCD,
                                      u8_t p_u8_DaysBCD);

extern void SCD_v_IncrementYears_f(void);
extern void SCD_v_DecrementYears_f(void);
extern void SCD_v_IncrementMonths_f(void);
extern void SCD_v_DecrementMonths_f(void);
extern void SCD_v_IncrementWeekDays_f(void);
extern void SCD_v_DecrementWeekDays_f(void);
extern void SCD_v_IncrementDays_f(void);
extern void SCD_v_DecrementDays_f(void);
extern void SCD_v_IncrementHours_f(void);
extern void SCD_v_IncrementAlarmHours_f(void);
extern void SCD_v_DecrementHours_f(void);
extern void SCD_v_DecrementAlarmHours_f(void);
extern void SCD_v_IncrementMins_f(void);
extern void SCD_v_IncrementAlarmMins_f(void);
extern void SCD_v_DecrementMins_f(void);
extern void SCD_v_DecrementAlarmMins_f(void);
extern void SCD_v_IncrementSecs_f(void);
extern void SCD_v_DecrementSecs_f(void);
extern void SCD_v_DriverInterfaceTest_f(void);

#define XTIM_SCD_u8_SecondsBin         		((u8_t)SCD_u8_SecondsBin)
#define XTIM_SCD_u8_MinutesBin         		((u8_t)SCD_u8_MinutesBin)
#define XTIM_SCD_u8_AlarmMinutesBin    		((u8_t)SCD_u8_AlarmMinutesBin)
#define XTIM_SCD_u8_HoursBin           		((u8_t)SCD_u8_HoursBin)
#define XTIM_SCD_u8_AlarmHoursBin      		((u8_t)SCD_u8_AlarmHoursBin)
#define XTIM_SCD_u8_DaysBin            		((u8_t)SCD_u8_DaysBin)
#define XTIM_SCD_u8_WeekDaysBin        		((u8_t)SCD_u8_WeekDaysBin)
#define XTIM_SCD_u8_MonthsBin          		((u8_t)SCD_u8_MonthsBin)
#define XTIM_SCD_u16_YearsBin          		((u16_t)SCD_u16_YearsBin)

#define SCD_U8_CET_CEST_COMPENSATION_ON		((u8_t)1u)
#define SCD_U8_CET_CEST_COMPENSATION_OFF	((u8_t)0u)

extern u8_t  SCD_u8_SecondsBin;
extern u8_t  SCD_u8_MinutesBin;
extern u8_t  SCD_u8_AlarmMinutesBin;
extern u8_t  SCD_u8_HoursBin;
extern u8_t  SCD_u8_AlarmHoursBin;
extern u8_t  SCD_u8_DaysBin;
extern u8_t  SCD_u8_WeekDaysBin;
extern u8_t  SCD_u8_MonthsBin;
extern u16_t SCD_u16_YearsBin;

#endif
