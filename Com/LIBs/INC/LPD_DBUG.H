#ifndef LPD_DBUG_H
#define LPD_DBUG_H

#if defined LPI_MODULE_NAME
#include <recorder.h>
    static const char *lpi_module_name = LPI_MODULE_NAME;

#ifndef LPI_POSITION
   #define LPI_POSITION (char *)lpi_module_name,__LINE__
#endif

#define lpi_nse_record(type, action) \
	nse_record (LPI_POSITION, type, action, 0, (UINT8 *) 0x0L);

#define hamster_continue(COND)                                              \
	{ if((COND))                                                        \
          { lpi_nse_record (NSE_EVENT_TYPE_WARNINGS, NSE_ACTION_CONTINUE); }\
        }

#define hamster_suspend(COND)                                               \
        { if((COND))                                                        \
          { lpi_nse_record (NSE_EVENT_TYPE_ERROR, NSE_ACTION_SUSPEND); }    \
        }

#define hamster_halt(COND)                                                  \
	{ if((COND))                                                        \
          { lpi_nse_record (NSE_EVENT_TYPE_ERROR, NSE_ACTION_HALT); }       \
        }

#define hamster_reset(COND)                                                 \
	{ if((COND))                                                        \
          { lpi_nse_record (NSE_EVENT_TYPE_ERROR, NSE_ACTION_REBOOT); }     \
        }
#endif
#endif
