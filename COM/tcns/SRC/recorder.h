/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Modified the UINT16 type to be 32-bit compatible                                    */
/*==============================================================================================*/


/******************************************************************************
*         Copyright (C) Siemens AG 1993 All Rights Reserved                   *
*******************************************************************************
*
*    project:     TCN Train Communication Network
*                 JDP Joint Development Project
*
*    title:       Error Recorder
*
*******************************************************************************
*    
*    description: Include file for the error recorder 
*
*    department:  VT 592 Busgruppe
*    version:     0.0		
*    language:    ANSI-'C'
*    system:      iRMK960 (hidden by Processor Interface Lib [PIL])
*
*    author:      Arnold	
*    history:     date      sign   remark
*                 05.08.93  Ar     start of development         
*                                                                          
******************************************************************************/

#ifndef NSE
#define NSE 1

#ifndef UINT8
#define UINT8 unsigned char
#endif
#define MAX_UINT8 0xff

/* <ATR:01> */
#ifndef UINT16
#define UINT16 unsigned short
#endif
#define MAX_UINT16 0xffff

#ifndef UINT32
#define UINT32 unsigned long int
#endif
#define MAX_UINT32 0xffffffff

#ifndef FALSE
#define FALSE (1 == 0)
#endif

#ifndef TRUE
#define TRUE (0 == 0)
#endif

#ifndef BOOLEAN
#define BOOLEAN unsigned char
#endif


#define NSE_ERR_OK               0
#define NSE_ERR_NOT_INIT         1
#define NSE_ERR_ALREADY_INIT     2
#define NSE_ERR_PARAM            3
#define NSE_ERR_OVERRUN          4
#define NSE_ERR_NO_EVENTS        5
#define NSE_ERR_SHARE            6

#define NSE_BUFFER_OVERWRITE    0
#define NSE_BUFFER_NO_OVERWRITE 1

#define NSE_EVENT_TYPE_FIRST    0  /* the values for event_types must be */
#define NSE_EVENT_TYPE_ERROR    0  /* with NSE_EVENT_TYPE_FIRST = 0.     */
#define NSE_EVENT_TYPE_WARNINGS 1
#define NSE_EVENT_TYPE_SPECIAL  2
#define NSE_EVENT_TYPE_UNKNOWN  3
#define NSE_EVENT_TYPE_LOST     4  
#define NSE_EVENT_TYPE_LAST     4  /* NSE_EVENT_TYPE_LAST must hold the  */
                                   /* index of the last event.           */

#define NSE_ACTION_FIRST    0      /* see above */
#define NSE_ACTION_CONTINUE 0                                    
#define NSE_ACTION_SUSPEND  1
#define NSE_ACTION_HALT     2
#define NSE_ACTION_REBOOT   3
#define NSE_ACTION_LAST     3

#define NSE_READ_MODE_OLDEST  0
#define NSE_READ_MODE_CURRENT 1


#define NSE_MAX_LIST_LENGTH   32
#define NSE_MAX_NAME_LENGTH    7


typedef struct
{
  BOOLEAN locked;                          /* reserved for internal use */
  char    name[NSE_MAX_NAME_LENGTH + 1];
  UINT8   list_length;
  UINT16  line_number;                     /* number of valid bytes in list */
  UINT32  time;
  UINT8   list[NSE_MAX_LIST_LENGTH];
} NSE_EVENT_TYPE;


extern int nse_init(NSE_EVENT_TYPE *  buffer,
                    int               buffer_size,
                    int               buffer_type );


extern int nse_record(char *    module_name,
                      UINT16    line_number,
                      int       event_type,
                      int       event_action,
                      UINT8     list_length,
                      UINT8 *   p_list      );


/*
Example for using nse_record

  err = nse_record(__FILE__,                  name of the file where nse_record is called
                   __LINE__,                  linenumber where nse_record is called
                   NSE_EVENT_TYPE_ERROR,      type of the event (error)
                   NSE_ACTION_REBOOT,         action to be taken after the event was recorded (reboot)
                   sizeof(arg),               number of bytes to be recorded
                   &arg);                     address of the bytes to be recorded
*/


extern int nse_read(int            read_mode,
                    int            number_of_events,
                    NSE_EVENT_TYPE buffer[],
                    int *          entries_filled);


extern int nse_clear(void);


extern int nse_read_counter(int       event_type,
                            UINT16 *  counter);


extern int nse_read_clear_counter(int       event_type,
                                  UINT16 *  counter);


#endif
