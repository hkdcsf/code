/** INCLUDES ***********************************************/
#include "p18f25k20.h"
#include "DCF_typedefs.h"
#include "GPS_driver.h"
#include "LCC_driver.h"
#include "CNV_driver.h"
#include "stddef.h"
#include "SCD_driver.h"

typedef enum L_GPS_e8_SentenceCheckResults_tag
{
    L_GPS_e8_SentenceCheckInit                     = 255u,
    L_GPS_e8_SentenceCheckDataCollecting        = 2u,
    L_GPS_e8_SentenceCheckBufferError           = 3u,
    L_GPS_e8_SentenceCheckCheckSumError            = 4u,
    L_GPS_e8_SentenceCheckCheckIsOK              = 5u
} L_GPS_e8_SentenceCheckResults_t;

/** PROTOTYPES *********************************************/
void GPS_v_Init_f(void);
void GPS_v_GetReceivedByte_f(u8_t u8_ReceivedByte_p);
void GPS_v_WriteLastGPSSentence_f(void);
void GPS_v_Write_UTC_Latitude_Longitude_f(void);
u8_t GPS_u8_UpdateTime_f(void);

void GPS_v_SwitchGPSModuleOn_f(void);
void GPS_v_SwitchGPSModuleOff_f(void);

static u8_t L_GPS_u8_GetSentenceBeginandEnd_f( u8_t * u8p_BufferAddress_p,
                                               u8_t * u8p_SentenceBegin_p,
                                               u8_t * u8p_SentenceEnd_p,
                                               u8_t u8_BufferLastItemAddress_p);
static u8_t L_GPS_u8_CheckCheckSum_f(           u8_t * u8p_BufferAddress_p,
                                               u8_t   u8_BufferStart_p,
                                               u8_t   u8_BufferEnd_p);
static u8_t L_GPS_u8_GetUTCFromSentence_f(     u8_t *  u8p_BufferAddress_p,
                                               u8_t * au8_GPS_UTCString_p,
                                               u8_t   u8_BufferBeginCnt_p);
static u8_t L_GPS_u8_IdentifySentence_f(     u8_t * u8p_BufferAddress_p,
                                             u8_t   u8_BufferBeginCnt_p);
static u8_t L_GPS_u8_GetLatitudeFromSentence_f(    u8_t * u8p_BufferAddress_p,
                                                      u8_t * au8_GPS_LatitudeString_p,
                                                      u8_t   u8_BufferBeginCnt_p);
static u8_t L_GPS_u8_GetLongitudeFromSentence_f(    u8_t * u8p_BufferAddress_p,
                                                       u8_t * au8_GPS_LongitudeString_p,
                                                       u8_t   u8_BufferBeginCnt_p);
static L_GPS_e8_SentenceCheckResults_t L_GPS_e8_CheckLastSentenceValidity_f(void);



/** DEFINES ************************************************/
#define L_GPS_U16_BUFFER_LENGTH             255u
#define L_GPS_U8_UTC_STRING_LENGTH             12u
#define L_GPS_U8_LATITUDE_STRING_LENGTH     13u
#define L_GPS_U8_LONGITUDE_STRING_LENGTH     14u

#define L_GPS_U8_LATITUDE_CHAR_POS_START       17u
#define L_GPS_U8_LONGITUDE_CHAR_POS_START      29u

/** STATICS ************************************************/
#pragma idata   /* declare statically allocated initialized variables*/

u8_t GPS_au8_GPSSentenceBytesBuffer[L_GPS_U16_BUFFER_LENGTH];
u8_t GPS_au8_GPSSentenceBytesBufferCnt = 0u;
u8_t GPS_au8_GPSSentenceLFCnt = 0u;

u8_t GPS_u8_DataProcessingIsOngoing = 0u;

u8_t GPS_u8_BEResult    = 0u;
u8_t GPS_u8_CSResult    = 0u;
u8_t GPS_u8_BufferStart = 0u;
u8_t GPS_u8_BufferEnd   = 0u;

u8_t GPS_au8_UTCString[]       = {'123456.000#'};
u8_t GPS_au8_LatitudeString[]  = {'1234.5678,N#'};
u8_t GPS_au8_LongitudeString[] = {'12345.6789,E#'};

/** DECLARATIONS *******************************************/
#pragma code    /* declare executable instructions */

void GPS_v_Init_f(void)
{
    u16_t u16_i;
    
    for (u16_i=0u; u16_i<L_GPS_U16_BUFFER_LENGTH;u16_i++)
    {
        GPS_au8_GPSSentenceBytesBuffer[u16_i]='0';
    }
}

void GPS_v_GetReceivedByte_f(u8_t u8_ReceivedByte_p)
{
    if(1u != GPS_u8_DataProcessingIsOngoing) /* if processing of sentence is ongoing in the background we do no overwrite it*/
    {
        GPS_au8_GPSSentenceBytesBuffer[GPS_au8_GPSSentenceBytesBufferCnt] = (u8_ReceivedByte_p);

        if(LCC_U8_CHARACTER_LF == u8_ReceivedByte_p) /* if we received the second sentence end, we stop further reception and process buffer content*/
        {
            GPS_au8_GPSSentenceLFCnt++;

            if(3u == GPS_au8_GPSSentenceLFCnt) /* second full sentence i the buffer is processed */
            {
                GPS_u8_DataProcessingIsOngoing = 1u;
                GPS_au8_GPSSentenceLFCnt = 0u;
            }
        }

        GPS_au8_GPSSentenceBytesBufferCnt++;

        if(GPS_au8_GPSSentenceBytesBufferCnt >= L_GPS_U16_BUFFER_LENGTH)
        {
            GPS_au8_GPSSentenceBytesBufferCnt = 0u;
        }
    }
}

static u8_t L_GPS_u8_GetSentenceBeginandEnd_f( u8_t * u8p_BufferAddress_p,
                                               u8_t * u8p_SentenceBegin_p,
                                               u8_t * u8p_SentenceEnd_p,
                                               u8_t u8_BufferLastItemAddress_p)
{
    u8_t    u8_return = 0u;
    u8_t     u8_BufferEndCnt = 0u;
    u8_t     u8_BufferBeginCnt = 0u;

    u8_BufferEndCnt = u8_BufferLastItemAddress_p;


    while( ! ( ((LCC_U8_CHARACTER_CR ==  u8p_BufferAddress_p[u8_BufferEndCnt-1])  &&
                (LCC_U8_CHARACTER_LF ==  u8p_BufferAddress_p[u8_BufferEndCnt+0])) ||
                ( u8_BufferEndCnt < 2u)      /* at least 3 byte shall be in the buffer LF,...,CR,LF */
              )
         )
    {
        u8_BufferEndCnt--;
    };

    u8_BufferBeginCnt = u8_BufferEndCnt;

    while( ! (  (('$' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+0u])   &&
                 ('G' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+1u])   &&
                 ('P' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+2u]))  ||
                 ( u8_BufferBeginCnt == 0u)
                 )
         )
    {
        u8_BufferBeginCnt--;
    };

    /* Validity check */
    if(        ('$' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+0u])                  &&
            ('G' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+1u])                  &&
            ('P' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+2u])                 &&
            (LCC_U8_CHARACTER_CR ==  u8p_BufferAddress_p[u8_BufferEndCnt-1])      &&
            (LCC_U8_CHARACTER_LF ==  u8p_BufferAddress_p[u8_BufferEndCnt+0])    &&
            (u8_BufferBeginCnt < u8_BufferEndCnt)
      )
    {
        *u8p_SentenceBegin_p = u8_BufferBeginCnt;
        *u8p_SentenceEnd_p = u8_BufferEndCnt;
        u8_return = 1u;
    }
    else
    {
        *u8p_SentenceBegin_p = NULL;
        *u8p_SentenceEnd_p = NULL;
        u8_return = 0u;
    }

    return u8_return;
}

static u8_t L_GPS_u8_CheckCheckSum_f(           u8_t * u8p_BufferAddress_p,
                                               u8_t   u8_BufferStart_p,
                                               u8_t   u8_BufferEnd_p)
{

    u8_t u8_calculatedchecksum = 0u;
    static u8_t u8_receivedchecksum = 0u;
    u8_t u8_cnt = 0u;
    u8_t u8_cnt_start = 0u;
    u8_t u8_cnt_end = 0u;
    u8_t u8_return = 0u;
    u8_t u8_1stCheckSumChr = 0u;
    u8_t u8_2ndCheckSumChr = 0u;

    /* get checksum from the GPS sentence */
    u8_1stCheckSumChr = u8p_BufferAddress_p[u8_BufferEnd_p-3];
    u8_2ndCheckSumChr = u8p_BufferAddress_p[u8_BufferEnd_p-2];

    u8_1stCheckSumChr = CNV_u8_Char2Num_f(u8_1stCheckSumChr);
    u8_2ndCheckSumChr = CNV_u8_Char2Num_f(u8_2ndCheckSumChr);

    u8_receivedchecksum = (u8_1stCheckSumChr<<4) | u8_2ndCheckSumChr;

    u8_cnt_start = (u8_BufferStart_p + 1u); /* We start counting checksum after the first character of  the $ sign */
    u8_cnt_end   = (u8_BufferEnd_p   - 4u);  /* We end counting checksum before the first character of  the * sign */

    /* calculate checksum from received bytes */
    for(u8_cnt = u8_cnt_start; u8_cnt < u8_cnt_end; u8_cnt++ )
    {
        u8_calculatedchecksum ^= u8p_BufferAddress_p[u8_cnt];
    }

    /* verify checksum */
    if(u8_calculatedchecksum == u8_receivedchecksum )
    {
        u8_return = 1u;
    }
    else
    {
        u8_return = 0u;
    }

    return u8_return;
}

static L_GPS_e8_SentenceCheckResults_t L_GPS_e8_CheckLastSentenceValidity_f(void)
{
    L_GPS_e8_SentenceCheckResults_t e8_StChReturn = L_GPS_e8_SentenceCheckInit;

    if (1u == GPS_u8_DataProcessingIsOngoing)
    {

        GPS_u8_BEResult = L_GPS_u8_GetSentenceBeginandEnd_f(    &GPS_au8_GPSSentenceBytesBuffer[0],
                                                                &GPS_u8_BufferStart,
                                                                &GPS_u8_BufferEnd,
                                                                GPS_au8_GPSSentenceBytesBufferCnt);

        if (1u == GPS_u8_BEResult)
        {
            GPS_u8_CSResult = L_GPS_u8_CheckCheckSum_f( &GPS_au8_GPSSentenceBytesBuffer[0],
                                                        GPS_u8_BufferStart,
                                                        GPS_u8_BufferEnd);
            if (1u == GPS_u8_CSResult)
            {
                e8_StChReturn = L_GPS_e8_SentenceCheckCheckIsOK;
            }
            else
            {
                e8_StChReturn = L_GPS_e8_SentenceCheckCheckSumError;
            }
        }
        else
        {
            e8_StChReturn = L_GPS_e8_SentenceCheckBufferError;
        }

        GPS_u8_DataProcessingIsOngoing = 0u; /* we processed the data, start collecting next samples */
        GPS_au8_GPSSentenceBytesBufferCnt = 0u;
    }
    else
    {
        e8_StChReturn = L_GPS_e8_SentenceCheckDataCollecting;
    }

    return e8_StChReturn;
}

void GPS_v_WriteLastGPSSentence_f(void)
{
    L_GPS_e8_SentenceCheckResults_t e8_SentenceCheckResult = 0u;

    LCC_v_ClearScreen_f();

    e8_SentenceCheckResult = L_GPS_e8_CheckLastSentenceValidity_f();

    /* write result on LCD screen */
    switch(e8_SentenceCheckResult)
    {
    case L_GPS_e8_SentenceCheckBufferError:    /* data in the buffer is not OK for processing */
        LCC_v_DrawGPSStatus_f(1u);
        break;
    case L_GPS_e8_SentenceCheckCheckSumError: /* we detected a checksum error write this to the LCD */
        LCC_v_DrawGPSStatus_f(2u);
        break;
    case L_GPS_e8_SentenceCheckCheckIsOK:     /* everything was fine, write actual sentence to LCD */
        LCC_v_WriteGPSSentence_f(&GPS_au8_GPSSentenceBytesBuffer[0], GPS_u8_BufferStart, GPS_u8_BufferEnd);
        break;
    case L_GPS_e8_SentenceCheckDataCollecting: /* in all other cases, write data processing ongoing on the LCD */
    case L_GPS_e8_SentenceCheckInit:
    default:
        LCC_v_DrawGPSStatus_f(0u);
        break;
    };
}

void GPS_v_Write_UTC_Latitude_Longitude_f(void)
{
    u8_t u8_UTCCheckResult = 255u;
    L_GPS_e8_SentenceCheckResults_t e8_SentenceCheckResult = L_GPS_e8_SentenceCheckInit ;
    u8_t u8_SentenceIdentificationResult = 0u;
    u8_t u8_LatitudeCheckResult = 0u;
    u8_t u8_LongitudeCheckResult = 0u;

    e8_SentenceCheckResult = L_GPS_e8_CheckLastSentenceValidity_f();

    /* write result on LCD screen */
    switch(e8_SentenceCheckResult)
    {
    case L_GPS_e8_SentenceCheckCheckIsOK:     /* everything was fine, write actual sentence to LCD */


        u8_SentenceIdentificationResult = L_GPS_u8_IdentifySentence_f(        &GPS_au8_GPSSentenceBytesBuffer[0],
                                                                            GPS_u8_BufferStart);
        if(0u != u8_SentenceIdentificationResult)
        {

            u8_UTCCheckResult = L_GPS_u8_GetUTCFromSentence_f(                &GPS_au8_GPSSentenceBytesBuffer[0],
                                                                            &GPS_au8_UTCString[0],
                                                                            GPS_u8_BufferStart);
            /*GPGGA sentence*/
            if (1u== u8_SentenceIdentificationResult)
            {
                u8_LatitudeCheckResult = L_GPS_u8_GetLatitudeFromSentence_f(    &GPS_au8_GPSSentenceBytesBuffer[0],
                                                                                &GPS_au8_LatitudeString[0],
                                                                                GPS_u8_BufferStart);
                u8_LongitudeCheckResult = L_GPS_u8_GetLongitudeFromSentence_f(    &GPS_au8_GPSSentenceBytesBuffer[0],
                                                                                &GPS_au8_LongitudeString[0],
                                                                                GPS_u8_BufferStart);
            }
            else
            {
                /*GPRMC sentence*/
                u8_LatitudeCheckResult = L_GPS_u8_GetLatitudeFromSentence_f(    &GPS_au8_GPSSentenceBytesBuffer[1],
                                                                                &GPS_au8_LatitudeString[0],
                                                                                 GPS_u8_BufferStart);
                u8_LongitudeCheckResult = L_GPS_u8_GetLongitudeFromSentence_f(    &GPS_au8_GPSSentenceBytesBuffer[1],
                                                                                &GPS_au8_LongitudeString[0],
                                                                                GPS_u8_BufferStart);
            }
            LCC_v_DrawUTC_f(&GPS_au8_UTCString[0],1u);
            LCC_v_DrawLatitude_f(&GPS_au8_LatitudeString[0],1u);
            LCC_v_DrawLongitude_f(&GPS_au8_LongitudeString[0],1u);
        }
        else
        {
            LCC_v_DrawUTC_f(&GPS_au8_UTCString[0],0u);
            LCC_v_DrawLatitude_f(&GPS_au8_LatitudeString[0],0u);
            LCC_v_DrawLongitude_f(&GPS_au8_LongitudeString[0],0u);
        }
        break;
    case L_GPS_e8_SentenceCheckBufferError:        /* Write waiting in case of errors or data collecting */
    case L_GPS_e8_SentenceCheckCheckSumError:
    case L_GPS_e8_SentenceCheckDataCollecting:
    case L_GPS_e8_SentenceCheckInit:
    default:
        LCC_v_DrawUTC_f(&GPS_au8_UTCString[0],0u);
        LCC_v_DrawLatitude_f(&GPS_au8_LatitudeString[0],0u);
        LCC_v_DrawLongitude_f(&GPS_au8_LongitudeString[0],0u);
        break;
    };


}

u8_t GPS_u8_UpdateTime_f(void)
{
    u8_t u8_Return = 0u;

    u8_t u8_UTCCheckResult = 255u;
    L_GPS_e8_SentenceCheckResults_t e8_SentenceCheckResult = L_GPS_e8_SentenceCheckInit ;
    u8_t u8_SentenceIdentificationResult = 0u;

    u8_t u8_HoursHigh = 0u;
    u8_t u8_HoursLow = 0u;
    u8_t u8_HoursBCD = 0u;
    u8_t u8_MinutesHigh = 0u;
    u8_t u8_MinutesLow = 0u;
    u8_t u8_MinutesBCD = 0u;
    u8_t u8_SecondsHigh = 0u;
    u8_t u8_SecondsLow = 0u;
    u8_t u8_SecondsBCD = 0u;

    e8_SentenceCheckResult = L_GPS_e8_CheckLastSentenceValidity_f();

    /* write result on LCD screen */
    switch(e8_SentenceCheckResult)
    {
    case L_GPS_e8_SentenceCheckCheckIsOK:     /* everything was fine, write actual sentence to LCD */


        u8_SentenceIdentificationResult = L_GPS_u8_IdentifySentence_f(        &GPS_au8_GPSSentenceBytesBuffer[0],
                                                                            GPS_u8_BufferStart);
        if(0u != u8_SentenceIdentificationResult)
        {

            u8_UTCCheckResult = L_GPS_u8_GetUTCFromSentence_f(                &GPS_au8_GPSSentenceBytesBuffer[0],
                                                                            &GPS_au8_UTCString[0],
                                                                            GPS_u8_BufferStart);
            if (0u == u8_UTCCheckResult)
            {
                u8_HoursHigh = CNV_u8_Char2Num_f(GPS_au8_UTCString[0]);
                u8_HoursLow  = CNV_u8_Char2Num_f(GPS_au8_UTCString[1]);

                u8_HoursBCD = (u8_HoursHigh<<4u | u8_HoursLow);

                u8_MinutesHigh = CNV_u8_Char2Num_f(GPS_au8_UTCString[2]);
                u8_MinutesLow  = CNV_u8_Char2Num_f(GPS_au8_UTCString[3]);

                u8_MinutesBCD = (u8_MinutesHigh<<4u | u8_MinutesLow);

                u8_SecondsHigh = CNV_u8_Char2Num_f(GPS_au8_UTCString[4]);
                u8_SecondsLow  = CNV_u8_Char2Num_f(GPS_au8_UTCString[5]);

                u8_SecondsBCD = (u8_SecondsHigh<<4u | u8_SecondsLow);

                SCD_v_UpdateTime_f(u8_HoursBCD, u8_MinutesBCD, u8_SecondsBCD, SCD_U8_CET_CEST_COMPENSATION_ON);
                u8_Return = 1u;
            }
        }
        break;
    case L_GPS_e8_SentenceCheckBufferError:        /* Write waiting in case of errors or data collecting */
    case L_GPS_e8_SentenceCheckCheckSumError:
    case L_GPS_e8_SentenceCheckDataCollecting:
    case L_GPS_e8_SentenceCheckInit:
    default:
        break;
    };

    return u8_Return;
}

static u8_t L_GPS_u8_IdentifySentence_f(     u8_t * u8p_BufferAddress_p,
                                             u8_t   u8_BufferBeginCnt_p)
{

    u8_t u8_BufferBeginCnt = 0u;
    u8_t u8_IdentificationResult = 0u;

    u8_BufferBeginCnt = u8_BufferBeginCnt_p;


    if(        ('$' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+0u])                  &&
            ('G' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+1u])                  &&
            ('P' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+2u])                 &&
            ('G' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+3u])                 &&
            ('G' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+4u])                 &&
            ('A' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+5u])                    &&
            (',' !=  u8p_BufferAddress_p[u8_BufferBeginCnt+18u])
      )
    {
        u8_IdentificationResult = 1u;
    }

    if(        ('$' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+0u])                  &&
            ('G' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+1u])                  &&
            ('P' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+2u])                 &&
            ('R' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+3u])                 &&
            ('M' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+4u])                 &&
            ('C' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+5u])                    &&
            ('A' ==  u8p_BufferAddress_p[u8_BufferBeginCnt+18u])
      )
    {
        u8_IdentificationResult = 2u;
    }

    return u8_IdentificationResult;
}

static u8_t L_GPS_u8_GetUTCFromSentence_f(     u8_t * u8p_BufferAddress_p,
                                               u8_t * au8_GPS_UTCString_p,
                                               u8_t   u8_BufferBeginCnt_p)
{
    u8_t u8_GPSUTCResult   = 255u;
    u8_t u8_BufferBeginCnt = 0u;
    u8_t u8_i = 0u;

    u8_BufferBeginCnt = u8_BufferBeginCnt_p;

    /* check format of the content*/
    for(u8_i = 0u; u8_i< 6u; u8_i++)
    {
        if ( ! (('0' <= u8p_BufferAddress_p[u8_BufferBeginCnt+7u+u8_i]) &&
                ('9' >= u8p_BufferAddress_p[u8_BufferBeginCnt+7u+u8_i]))
            )
        {
            u8_GPSUTCResult = 4u;
            break;
        }
    }

    if(255u == u8_GPSUTCResult)
    {
        if (u8p_BufferAddress_p[u8_BufferBeginCnt+7u+u8_i] != '.')
        {
            u8_GPSUTCResult = 5u;
        }
    }

    if(255u == u8_GPSUTCResult)
    {
        for(u8_i = 7u; u8_i< (7u+3u); u8_i++)
        {
            if ( ! (('0' <= u8p_BufferAddress_p[u8_BufferBeginCnt+7u+u8_i]) &&
                    ('9' >= u8p_BufferAddress_p[u8_BufferBeginCnt+7u+u8_i]))
                )
            {
                u8_GPSUTCResult = 6u;
                break;
            }
        }
    }

    /* copy UTC string */
    if(255u == u8_GPSUTCResult)
    {
        for(u8_i = 0u; u8_i< (L_GPS_U8_UTC_STRING_LENGTH-2);u8_i++)
        {
            au8_GPS_UTCString_p[u8_i] = u8p_BufferAddress_p[u8_BufferBeginCnt+7u+u8_i];
        }
        u8_GPSUTCResult = 0u;
    }


    /* if sentence has empty UTC content skip */
    if(255u != u8_GPSUTCResult)
    {
        /* invalidate string if something is wrong */
        if( 0!= u8_GPSUTCResult)
        {
            au8_GPS_UTCString_p[0u] = 'E';
            au8_GPS_UTCString_p[1u] = 'r';
            au8_GPS_UTCString_p[2u] = 'r';
            au8_GPS_UTCString_p[3u] = 'o';
            au8_GPS_UTCString_p[4u] = 'r';
            au8_GPS_UTCString_p[5u] = ':';
            au8_GPS_UTCString_p[6u] = ' ';
            au8_GPS_UTCString_p[7u] = '0'+u8_GPSUTCResult;
            au8_GPS_UTCString_p[8u] = ' ';
            au8_GPS_UTCString_p[9u] = ' ';
            au8_GPS_UTCString_p[10u] = ' ';
            au8_GPS_UTCString_p[11u] = '#';
        }
    }

    return u8_GPSUTCResult;
}



static u8_t L_GPS_u8_GetLatitudeFromSentence_f(    u8_t * u8p_BufferAddress_p,
                                                      u8_t * au8_GPS_LatitudeString_p,
                                                      u8_t   u8_BufferBeginCnt_p)
{
    u8_t u8_GPSLatitudeResult   = 255u;
    u8_t u8_BufferBeginCnt = 0u;
    u8_t u8_i = 0u;

    /* check format of the content*/
    for(u8_i = 0u; u8_i< 4u;u8_i++)
    {
        if ( ! (('0' <= u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LATITUDE_CHAR_POS_START+u8_i]) &&
                ('9' >= u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LATITUDE_CHAR_POS_START+u8_i]))
            )
        {
            u8_GPSLatitudeResult = 1u;
            break;
        }
    }

    if(255u == u8_GPSLatitudeResult)
    {
        if (u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LATITUDE_CHAR_POS_START+4u] != '.')
        {
            u8_GPSLatitudeResult = 2u;
        }
    }

    if(255u == u8_GPSLatitudeResult)
    {
        for(u8_i = 0u; u8_i< 4;u8_i++)
        {
            if ( ! (('0' <= u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LATITUDE_CHAR_POS_START+5u+u8_i]) &&
                    ('9' >= u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LATITUDE_CHAR_POS_START+5u+u8_i]))
                )
            {
                u8_GPSLatitudeResult = 3u;
                break;
            }
        }
    }

    if(255u == u8_GPSLatitudeResult)
    {
        if( !( ('N' == u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LATITUDE_CHAR_POS_START+10u]) ||
               ('S' == u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LATITUDE_CHAR_POS_START+10u])
              )
           )
        {
            u8_GPSLatitudeResult   = 4u;
        }
    }


    /* copy UTC string */
    if(255u == u8_GPSLatitudeResult)
    {
        for(u8_i = 0u; u8_i< (L_GPS_U8_LATITUDE_STRING_LENGTH-2u);u8_i++)
        {
            au8_GPS_LatitudeString_p[u8_i] = u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LATITUDE_CHAR_POS_START+u8_i];
        }
        u8_GPSLatitudeResult = 0u;
    }

    /* invalidate strin is something wrong */
    if( 0!= u8_GPSLatitudeResult)
    {
        au8_GPS_LatitudeString_p[0u] = 'E';
        au8_GPS_LatitudeString_p[1u] = 'r';
        au8_GPS_LatitudeString_p[2u] = 'r';
        au8_GPS_LatitudeString_p[3u] = 'o';
        au8_GPS_LatitudeString_p[4u] = 'r';
        au8_GPS_LatitudeString_p[5u] = ':';
        au8_GPS_LatitudeString_p[6u] = ' ';
        au8_GPS_LatitudeString_p[7u] = '0'+u8_GPSLatitudeResult;
        au8_GPS_LatitudeString_p[8u] = ' ';
        au8_GPS_LatitudeString_p[9u] = ' ';
        au8_GPS_LatitudeString_p[10u] = ' ';
        au8_GPS_LatitudeString_p[11u] = ' ';
        au8_GPS_LatitudeString_p[12u] = '#';
    }

    return u8_GPSLatitudeResult;
}

static u8_t L_GPS_u8_GetLongitudeFromSentence_f(    u8_t * u8p_BufferAddress_p,
                                                       u8_t * au8_GPS_LongitudeString_p,
                                                       u8_t   u8_BufferBeginCnt_p)
{
    u8_t u8_GPSLongitudeResult   = 255u;
    u8_t u8_BufferBeginCnt = 0u;
    u8_t u8_i = 0u;

    /* check format of the content*/
    for(u8_i = 0u; u8_i< 5u;u8_i++)
    {
        if ( ! (('0' <= u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LONGITUDE_CHAR_POS_START+u8_i]) &&
                ('9' >= u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LONGITUDE_CHAR_POS_START+u8_i]))
            )
        {
            u8_GPSLongitudeResult = 1u;
            break;
        }
    }

    if(255u == u8_GPSLongitudeResult)
    {
        if (u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LONGITUDE_CHAR_POS_START+5u] != '.')
        {
            u8_GPSLongitudeResult = 2u;
        }
    }

    if(255u == u8_GPSLongitudeResult)
    {
        for(u8_i = 0u; u8_i< 4;u8_i++)
        {
            if ( ! (('0' <= u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LONGITUDE_CHAR_POS_START+6u+u8_i]) &&
                    ('9' >= u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LONGITUDE_CHAR_POS_START+6u+u8_i]))
                )
            {
                u8_GPSLongitudeResult = 3u;
                break;
            }
        }
    }

    if(255u == u8_GPSLongitudeResult)
    {
        if( !( ('E' == u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LONGITUDE_CHAR_POS_START+11u]) ||
               ('W' == u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LONGITUDE_CHAR_POS_START+11u])
              )
           )
        {
            u8_GPSLongitudeResult   = 4u;
        }
    }


    /* copy UTC string */
    if(255u == u8_GPSLongitudeResult)
    {
        for(u8_i = 0u; u8_i< (L_GPS_U8_LONGITUDE_STRING_LENGTH-2u);u8_i++)
        {
            au8_GPS_LongitudeString_p[u8_i] = u8p_BufferAddress_p[u8_BufferBeginCnt+L_GPS_U8_LONGITUDE_CHAR_POS_START+u8_i];
        }
        u8_GPSLongitudeResult = 0u;
    }

    /* invalidate string is something wrong */
    if( 0!= u8_GPSLongitudeResult)
    {
        au8_GPS_LongitudeString_p[0u] = 'E';
        au8_GPS_LongitudeString_p[1u] = 'r';
        au8_GPS_LongitudeString_p[2u] = 'r';
        au8_GPS_LongitudeString_p[3u] = 'o';
        au8_GPS_LongitudeString_p[4u] = 'r';
        au8_GPS_LongitudeString_p[5u] = ':';
        au8_GPS_LongitudeString_p[6u] = ' ';
        au8_GPS_LongitudeString_p[7u] = '0'+u8_GPSLongitudeResult;
        au8_GPS_LongitudeString_p[8u] = ' ';
        au8_GPS_LongitudeString_p[9u] = ' ';
        au8_GPS_LongitudeString_p[10u] = ' ';
        au8_GPS_LongitudeString_p[11u] = ' ';
        au8_GPS_LongitudeString_p[12u] = ' ';
        au8_GPS_LongitudeString_p[13u] = '#';
    }

    return u8_GPSLongitudeResult;
}

void GPS_v_SwitchGPSModuleOn_f(void)
{
   TRISAbits.TRISA7 = 0u; /* Output */
   LATAbits.LATA7 = 1u;
}

void GPS_v_SwitchGPSModuleOff_f(void)
{
   TRISAbits.TRISA7 = 0u; /* Output */
   LATAbits.LATA7 = 0u;
}






