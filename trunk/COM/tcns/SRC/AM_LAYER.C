/************************************************************************/
/*  Asea Brown Boveri Research Center, CH-5405 Baden, Switzerland       */
/*  Copyright 1991                                                      */
/*                                                                      */
/*  NAME:                                                               */
/*    AM_LAYER.C                                                        */
/*                                                                      */
/*  ABSTRACT:                                                           */
/*    TCN Message Transport Protocol, part of session layer which is    */
/*    common for caller and replier.                                    */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  AUTHOR  :   06-Jan-1992     CRBC1   Hoepli                          */
/*  MODIFIED:   <dd-mm-yy>  <dep.>   <name>                             */
/*                                                                      */
/*  1.0          03.08.95    BATL     R.Flum  inserted                  */
/*                                            am_read_messenger_status  */
/*  1.1          09.10.95    BATL     R.Flum  am_write_messenger_control*/
/*                                            added                     */
/************************************************************************/

#include "stdio.h"
#include "am_sys.h"
#include "basetype.h"
#include "md_layer.h"
#include "nm_layer.h"
#include "tm_layer.h"
#include "pi_sys.h"
/* number of successful sent messages by an application */
/* calls to the own device are also countered */
unsigned long am_messages_sent     = 0;

/* number of successful received messages by an application */
/* received messages from the own device are also countered */
unsigned long am_messages_received = 0; /* number of received messages */

/* number of supported instances for all replier */
unsigned char am_instances         = 0;

/* PROTOTYPES: -------------------------------------------------------- */

void am_system_init_caller (void);
void am_system_init_replier (void);
void am_init_replier (unsigned int max_inst);
void am_init_caller (unsigned int def_reply_tmo, unsigned int max_calls);

/* FUNCTIONS: --------------------------------------------------------- */

void _am_announce_device (unsigned int max_call_number,
			  unsigned int max_inst_number,
			  unsigned int default_reply_tmo,
			  unsigned short my_credit)
{
  am_init_replier (max_inst_number);
  am_init_caller (default_reply_tmo, max_call_number);
  tm_init (my_credit, max_call_number + max_inst_number);
} /* _am_announce_device */

/************************************************************************/

void _am_init (void)
{
  tm_system_init ();
  am_clear_dir ();
  am_system_init_caller ();
  am_system_init_replier ();
} /* _am_init */

/******************************************************************************

	 FUNCTION: am_read_messenger_status

	 INPUT:

	 OUTPUT:   STR_AM_RD_MSNGR_STATUS

	 RETURN:   -

	 ABSTRACT: Read status informations of the Messenger

******************************************************************************/


int _am_read_messenger_status(struct STR_AM_RD_MSNGR_STATUS *p_stat)
{
  int am_read_error = AM_OK;

  if (p_stat == NULL)
  {
    am_read_error = AM_FAILURE;
  }
  else
  {
    pi_copy8(p_stat->messenger_id, MESSENGER_ID, MESSENGER_ID_SIZE);
    p_stat->messages_sent     = am_messages_sent;
    p_stat->messages_received = am_messages_received;
    p_stat->instances         = am_instances;

    /* read objects from the transport layer */

    tm_read_messenger_status(&p_stat->packets_sent,
                             &p_stat->packet_retries,
                             &p_stat->multicast_retries,
                             &p_stat->send_time_out,
                             &p_stat->ack_time_out,
                             &p_stat->alive_time_out,
                             &p_stat->credit,
                             &p_stat->packet_size,
                             &p_stat->multicast_window);


  }
  return am_read_error;
}

/******************************************************************************

	 FUNCTION: am_write_messenger_control

	 INPUT:    STR_AM_WT_MSNGR_CTRL

	 OUTPUT:

	 RETURN:   -

	 ABSTRACT: Set parameters in the Messenger

******************************************************************************/

int _am_write_messenger_control( struct STR_AM_WT_MSNGR_CTRL *p_ctrl)
{
  int am_write_error = AM_OK;

  if (p_ctrl == NULL)
  {
    am_write_error = AM_FAILURE;
  }
  else
  {
    am_write_error = tm_write_messenger_control(p_ctrl->send_time_out,
                                                p_ctrl->ack_time_out,
                                                p_ctrl->alive_time_out,
                                                p_ctrl->packet_size,
                                                p_ctrl->packet_lifetime,
                                                p_ctrl->multicast_window,
                                                p_ctrl->credit);
  }
  return am_write_error;
}