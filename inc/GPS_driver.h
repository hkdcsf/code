#ifndef GPS_DRIVER /* prevent multiple include */
#define GPS_DRIVER

extern void GPS_v_Init_f(void);   
extern void GPS_v_GetReceivedByte_f(u8_t u8_ReceivedByte_p);
extern void GPS_v_WriteLastGPSSentence_f(void);
extern void GPS_v_Write_UTC_Latitude_Longitude_f(void);
extern u8_t GPS_u8_UpdateTime_f(void);

extern void GPS_v_SwitchGPSModuleOn_f(void);
extern void GPS_v_SwitchGPSModuleOff_f(void);

#endif
