#ifndef VMD_DRIVER /* prevent multiple include */
#define VMD_DRIVER

extern void  VMD_v_InitADC_f(void);
extern u16_t VMD_u16_GetADCResult(u8_t p_u8_ChannnelId);
extern void  VMD_v_GetVoltage_f(u16_t *p_pu16_VoltageOut);

#endif
