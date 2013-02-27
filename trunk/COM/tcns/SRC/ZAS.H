/*



*/
/* 
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                T  C  N                                      บ
บ                                                                             บ
บ                 H E A D E R F I L E   O F   Z A S                           บ
บ                                                                             บ
บ  Author: G. Hans                                         Date: 10-06-1992   บ
บ                                                                             บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
/*****************************************************************************
                         P R O T O T Y P E S
 *****************************************************************************/
   void zas_activate (void);                        /* ZAS activate   */
   void zas_init (void);                            /* ZAS initialize */
   void zas_norm (void);                            /* clear ZAS queues */
   unsigned char zas_subscribe(unsigned char pnr,   /* Announce procedure */
                    void (*p_proc) (void *p_msg));
   unsigned char zas_send(unsigned char priority,   /* Send Message */
                    void *p_msg);
   void zas_wait (void);                            /* ZAS in waiting state */
   void zas_unwait (void);                          /* ZAS running */
   void zas_halt (void);                            /* ZAS in halt state */
   void zas_continue (void);                        /* ZAS running */
