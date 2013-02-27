/*~~*/
/*
000000382



3
*/
/*=====================================================================
project   : TRAIN COMMUNICATION NETWORK 
-----------------------------------------------------------------------
department: ABB AHE/STL3   author:  M. Reismann        date: 31.10.1995
-----------------------------------------------------------------------
filename  : WTBDRV.H
-----------------------------------------------------------------------
outline   : 1. Constants
            2. Data Structures
            3. Interface Procedures
              (numbering is consistent with the specification document)
-----------------------------------------------------------------------
function  : specification of TCN-DRIVER-INTERFACE  REVISION H
            cmp. TCN -Specification of WTB Driver Interface 
            with implementation notes SP.G/GH019  Revision F

-----------------------------------------------------------------------
purpose:
  This driver interface is designed to control low-level hardware
  equipement.
  It is the interface to the link layer. This interface provides an
  abstraction from hardware details. It allows the implementation of
  portable software running on different hardware platforms.

error handling:
  Functions defined at the driver interface implement an error handling
  by return values or by an user-defined function call for asynchronous
  error indication. 
   
  A return value is an integer code, which reports the execution status
  of the called function. Each individual return value has an
  associated constant identifier. Error codes range from 1 to 127. They
  depend on the possibilities of the controlled hardware and on the
  current software implementation of the driver. These implementation-
  dependant return values are defined in a separate header file. This
  header file is not used by the link layer, but the driver source file
  and each module performing error handling shall include this file. It
  is called "WTBD960.H". The last four characters indicate the
  dependancy to a specific implementation.

  If no error and no exceptional situation occures the function returns
  "D_OK". Only the function "d_X_receive_request" returns values which
  are not equal to D_OK and which are no errors. They are called
  "reports". These reports are "D_RPT_FRAME_RECEIVED", "D_RPT_TIMEOUT",
  "D_RPT_STANDBY", and "D_RPT_NO_ENTRY". All other return codes (range
  1...127) indicate an error message to the function caller.
   
  These messages shall be quite detailed, so diagnosis of system
  failures of hardware and software are well supported. But detailed
  error messages require an additional amount of runtime. Therefore
  specific driver implementations will report different error messages.
*/
/*~9*/
/*
1. Constants
*/
#define D_ON                0 /* Switch Position ON                  */
#define D_OFF               1 /* Switch Position OFF                 */
/*~0*/
#define D_OPEN              0 /* Open Switch                         */
#define D_CLOSE             1 /*Close Switch                        */

/* D_LINE-assignments are in accordance to AMED II status
   bit assignments */

#define D_LINE_A            1 /* Select Receive Line A               */
#define D_LINE_B            2 /* Select Receive Line B               */
#define D_LINE_A_ERR	    4 /* Error on line A*/
#define D_LINE_B_ERR	    8 /* Error on line B*/

#define D_DA1 	0x80
#define D_DA2   0x40
#define D_DB1   0x20
#define D_DB2   0x10


#define D_LINE_AUTO         2 /* driver switches automatically       */
/*~0*/
#define D_DIRECT_1          1 /* Select Main Channel in Direction 1  */
#define D_DIRECT_2          2 /* Select Main Channel in Direction 2  */
/*~0*/
#define D_MODE_NOACK        0 /* Send without Acknowledge            */
#define D_MODE_ACK          1 /* Send with Acknowledge               */
/*~0*/
#define D_MODE_NONCYCLIC    0 /* Timer no cyclic running             */
#define D_MODE_CYCLIC       1 /* Timer cyclic running (Tick Handler) */
/*~0*/
#define D_NONSELECTIVE      0 /* Receiver enabled for all frames     */
/*~0*/
#define D_TC_NOPROC         0 /* Disable Time-Out Procedure Call       
                                    (see 3.4.4)                         */
#define D_TC_PROC           1 /* Enable Time-Out Procedure Call      */
/*~0*/
#define D_MAX_DATA_SIZE   128 /* max size of data in a frame         */
/*~9*/
   /*
   Return Value
   */
#define D_OK                0 /* Function successfully completed     */
/*~0*/
#define D_MIN_ERROR_NO      1 /* minimum and maximum number of       */
#define D_MAX_ERROR_NO    127 /* implementation-dependant error code
                                       (caused by HW or SW problems)       */
/*~0*/
#define D_RPT_FRAME_RECEIVED    0 /* Frame received without errors   */
#define D_RPT_TLD             251 /* trusted line disturbed          */
#define D_RPT_OLD             252 /* observed line disturbed         */
#define D_RPT_TIMEOUT         253 /* Receive Time Out                */
#define D_RPT_STANDBY         254 /* Carrier detect on standby line  */
#define D_RPT_NO_ENTRY        255 /* No entry in receive frame queue */
/*~2*/
/*~9*/
   /*
   Fritting Param
   */
      /*        see 3.5.5        */
#define D_FRITTING_OFF      0
#define D_FRITTING_PASSIVE  1
#define D_FRITTING_ACTIVE   2
/*~2*/
/*~2*/
/*~9*/
/*
2. Data Structures
*/
   typedef struct 
   {
   unsigned char dd;                                     /* destination device         */
   unsigned char llc;                                    /* logical link control       */
   unsigned char sd;                                     /* source device              */
   unsigned char size;                                   /* size of data field         */
   unsigned char data[D_MAX_DATA_SIZE];                  /* data field                 */
   } type_idu_drv;
/*~0*/
   typedef struct 
   {
   unsigned long  NoTxF;                   /* counter of transmitted frames            */
   unsigned long  NoRxF;                   /* counter of frames received without error */
   unsigned short NoFE;                    /* counter of frames received with error    */
   unsigned short NoTO;                    /* counter of timeouts (ACK mode only)      */
   } type_LineStatus;
/*~0*/
   typedef struct 
   {
   type_LineStatus A1;                /* counter for line A in direction 1             */
   type_LineStatus A2;                /* counter for line A in direction 2             */
   type_LineStatus B1;                /* counter for line B in direction 1             */
   type_LineStatus B2;                /* counter for line B in direction 2             */
   unsigned short  LineSwitch;        /* counter for switching between redundant lines */
   void            *p_detailed_state; /* counter for line A in direction 1             */
   } type_StatisticData;
/*~2*/
/*~9*/
/*
3. Interface Procedures
*/
/*~9*/
   /*
   3.1 General Initialization
   */
      unsigned char  d_sysinit(void);
/*~0*/
      /*
      with:
        return value:  D_OK      (function execution successfully finished)
		       1 ... 127 (function execution failed)

      Note: This function sets and resets the driver and the hardware    */
/*~2*/
/*~9*/
   /*
   3.2 Interface Procedures for HDLC-channel
   */
/*~9*/
      /*
      3.2.1 Installation
      */
	 unsigned char d_1_install(void (*p_udf_1_send_confirm)
                                        (type_idu_drv *p_idu,
                                   unsigned char send_state),
				   void (*p_udf_1_rcv_event)
				      (
				       type_idu_drv  *p_idu,
				       unsigned char line,
				       unsigned char status
				      ),
				  unsigned long baudrate,
				   unsigned short timeout);
/*~0*/
	 unsigned char d_2_install(void (*p_udf_2_send_confirm)
					(type_idu_drv *p_idu,
				   unsigned char send_state),
				   void (*p_udf_2_rcv_event)(
				       type_idu_drv  *p_idu,
				       unsigned char line,
				       unsigned char status
				      ),
				   unsigned long baudrate,
				   unsigned short timeout);
/*~0*/
	 /*???*/
	 /*
	 with:
	   p_udf_send_confirm: address of a user defined function
			       (see 3.2.2.4)
	   p_udf_rcv_event:   address of a user defined function (see 3.2.3.3)
	   baudrate:          transmission rate in bit/s
	   timeout:           maximum response time in 10 fs

           return value:  D_OK      (function execution successfully finished)
                          1 ... 127 (function execution failed)

         Note:
           If a 'NULL'-pointer  is written, the related procedures may not be
           called by the driver
         */
/*~2*/
/*~9*/
      /*
      3.2.2 Procedures for Transmission
      */
         /*--3.2.2.1 Transmitter enable (optional)----------------------------*/
         unsigned char d_1_tx_enable(void);
         unsigned char d_2_tx_enable(void);
         /*
         with:
           return value:  D_OK      (function execution successfully finished)
                          1 ... 127 (function execution failed)
         */
/*~0*/
         /*--3.2.2.2 Transmitter disable (optional)---------------------------*/
         unsigned char d_1_tx_disable(void);
         unsigned char d_2_tx_disable(void);
         /*
         with:
           return value:  D_OK      (function execution successfully finished)
                          1 ... 127 (function execution failed)
         */
/*~0*/
         /*--3.2.2.3 Frame sending--------------------------------------------*/
	 unsigned char d_send_request(unsigned char dir_code,
				      type_idu_drv *p_idu,
				      unsigned char mode);
	 /*
	 with:
	   dir_code:	  0   for direction 1
			| 1   for direction 2 ;
			  Attention:observe coherence with MAC-module !

	   p_idu:         pointer to idu_drv containing the send frame
	   mode:          D_MODE_NOACK send without acknowledge
				       (no resp.time supervision)
                          D_MODE_ACK   send with acknowledge
				       (with resp. time supervision)

           return value:  D_OK      (function execution successfully finished)
                          1 ... 127 (function execution failed)

         Note: The timer value for the response time supervision must be in
               accordance to the WTB standard. If the timer expires before a
               frame reception starts a timeout event shall be reported. The
               driver has to ensure that in ACK mode either a timer event or a
               receive event is reported to the link layer (see interface
               protocol diagram)

	 Restrictions: During execution of d_X_send_request process it is not
                       allowed to call one of the following interface functions:

                                   d_X_tx_enable
                                   d_X_tx_disable
                                   d_X_rx_enable
                                   d_X_rx_disable
                                   d_X_install
                                   d_sw_direction
                                   d_sw_interruption
                                   d_sw_line_attachment
                                   d_sw_fritting_control

                       The process starts with the function call and ends with
		       p_udf_send_confirm report (NOACK mode) or the
		       p_udf_rcv_event report /ACK mode).
         */
/*~0*/
         /*--3.2.2.4 Send confirmation (call back function)-------------------*/
	 /*
         These functions are called by the driver:

	   void p_udf_1_send_confirm(type_idu_drv *p_idu,
				     unsigned char send_state);

	   void p_udf_2_send_confirm(type_idu_drv *p_idu,
                                     unsigned char send_state);

	 with:
           p_idu          pointer to sent IDU_DRV

           return value:  D_OK      (function execution successfully finished)
                          1 ... 127 (function execution failed)
         */
/*~2*/
/*~9*/
      /*
      3.2.3 Procedures for Reception
      */
         /*--3.2.3.1 receiver enable------------------------------------------*/
         unsigned char d_1_rx_enable(unsigned char rx_address);
         unsigned char d_2_rx_enable(unsigned char rx_address);
         /*
         with:
	   rx_address:    link address or D_NONSELECTIVE (= 0, all frames will
                          be received)

           return value:  D_OK      (function execution successfully finished)
                          1 ... 127 (function execution failed)
         */
/*~0*/
         /*--3.2.3.2----------------------------------------------------------*/
         unsigned char d_1_rx_disable(void);
         unsigned char d_2_rx_disable(void);
         /*
         with:
	   return value:  D_OK      (function execution successfully finished)
			  1 ... 127 (function execution failed)
         */
/*~0*/
         /*--3.2.3.3----------------------------------------------------------*/
         /*
         These functions are called by the driver:

	   void p_udf_1_rcv_event(void);

	   void p_udf_2_rcv_event(void);

         */
/*~0*/
         /*--3.2.3.4----------------------------------------------------------*/
         unsigned char d_1_receive_request(type_idu_drv **pp_idu,
					 unsigned char *p_line);
         unsigned char d_2_receive_request(type_idu_drv **pp_idu,
                                         unsigned char *p_line);
         /*
	 with:
           p_idu:         pointer variable, which contains the address of the
                          received frame after function call
	   line:          line from which the frame is received
			  (D_LINE_A or D_LINE_B)

	   return value:
			  D_RPT_FRAME_RECEIVED (frame received without error)
			  1 ... 127            (error occured during receive)
			  D_RPT_TIMEOUT        (time-out, no frame received)
			  D_RPT_STANDBY        (valid frame on standby line
						detected)
			  D_RPT_NO_ENTRY       (no entry in event queue)

	 Note:
	 1. If the return value is not equal to RPT_RECEIVE_ERROR or 1 .. 127,
	    the value of *p_idu is undefined.
	 2. always quit the request by calling the d_X_receive_quit function,
	    unless the report type is RPT_NO_ENTRY
	 3. the report RPT_STANDBY only indicates that a valid frame was
	    detected on the standby line.
	 */
/*~0*/
	 /*--3.2.3.5----------------------------------------------------------*/
 unsigned char d_clear_statistic(void) ;
	 /*
	 Sets all statistic counters to zero
	 */
 unsigned char d_read_statistic (type_StatisticData **pp_StatisticCnt);
 unsigned char d_get_cur_line_status(unsigned char *status);
 unsigned char d_get_int_line_status(unsigned char *status);



/*~2*/
/*~2*/
/*~9*/
   /*
   3.4 Interface Procedures for Timing Control
   */
      /*--3.4.1 Timer subscribing------------------------------------------*/
      unsigned char d_tc_timer_subscribe(unsigned char timer_no,
					 void (*p_timeout_handler)(void),
					 unsigned char mode);
      /*
      with:
        timer_no:           selection of timer 0..2
        p_timeout_handler:  address of time-out handler
        mode:               D_MODE_NONCYCLIC (non-cyclic running,
                                              after reaching zero
                                              the timer stops)
			    D_MODE_CYCLIC   (cyclic running, after reaching
                                             zero the timer starts again)

        return value:  D_OK      (function execution successfully finished)
                       1 ... 127 (function execution failed)
      */
/*~0*/
      /*--3.4.2 Timer release----------------------------------------------*/
      unsigned char d_tc_timer_release(unsigned char timer_no);
      /*
      with:
        timer_no:      selection of timer 0..2

        return value:  D_OK      (function execution successfully finished)
                       1 ... 127 (function execution failed)
      */
/*~0*/
      /*--3.4.3 Set/Reset Timer--------------------------------------------*/
      unsigned char d_tc_timer_reset(unsigned char timer_no,
                                     unsigned short time);
      /*
      with:
        timer_no:      selection of timer 0..2
        time:          multiple of 1 ms , if = 0 than timer gets inactive

        return value:  D_OK      (function execution successfully finished)
                       1 ... 127 (function execution failed)
      */
/*~0*/
      /*--3.4.4 Timer Control----------------------------------------------*/
      unsigned char d_tc_timer_control(unsigned char timer_no,
                                       unsigned char control);
      /*
      with:
        timer_no:      selection of timer 0..2
        control:       D_TC_NOPROC        (no procedure call after timeout)
                       D_TC_PROC             (procedure call after timeout)

        return value:  D_OK      (function execution successfully finished)
                       1 ... 127 (function execution failed)
      */
/*~0*/
      /*--3.4.5 Getting timers actual value--------------------------------*/
      unsigned char d_tc_get_time(unsigned char timer_no,
				  unsigned short *p_timer_value);
      /*
      with:
        timer_no:      selection of timer 0..2
        p_timer_value: pointer to counter variable. After call, this 
                       variable contains the actual timer value.

        return value:  D_OK      (function execution successfully finished)
                       1 ... 127 (function execution failed)
      */
/*~0*/
      /*--3.4.6 Reading System Time----------------------------------------*/
      unsigned long d_tc_get_systime(void);
      /*
      with:
        return value:  32-bit relative system time
      */
/*~0*/
      /*???*/
      /*--3.4.X------------------------------------------------------------*/
      void  d_tc_sync_enable(unsigned short);
/*~0*/
      /*???*/
      /*--3.4.Y------------------------------------------------------------*/
      void d_tc_sync(void);
/*~2*/
/*~9*/
   /*
   3.5 Interface Procedures for Physical Layer Switches
   */
      /*--3.5.1 Direction switch-------------------------------------------*/
      unsigned char d_sw_direction(unsigned char direction);
      /*
      with:
        direction:    set direction of the main channel (auxiliary channel
                      always switched to the other direction)
                        D_DIRECT_1 (main channel switched to direction 1)
                        D_DIRECT_2 (main channel switched to direction 2)

                      The default settings will be:
                        Direction 1 = Main Channel
                        Direction 2 = Auxiliary Channel

        return value:  D_OK      (function execution successfully finished)
		       1 ... 127 (function execution failed)
      */
/*~0*/
      /*--3.5.2 Interruption switch----------------------------------------*/
      unsigned char  d_sw_interruption(unsigned char line,
                                       unsigned char position);
      /*
      with:
        position:      set switch position
                         D_ON  (interrupt the lines)
                         D_OFF (connect the lines)
        line:          set line in case of redundant line:
                         D_LINE_A (direction switch of coupling unit A)
                         D_LINE_B (direction switch of coupling unit B)

        return value:  D_OK      (function execution successfully finished)
                       1 ... 127 (function execution failed)
      */
/*~0*/
      /*--3.5.3 Line Attachment--------------------------------------------*/
      unsigned char d_sw_line_attachment(unsigned char switch_1A,
                                         unsigned char switch_1B,
                                         unsigned char switch_2A,
                                         unsigned char switch_2B);
      /*
      with:
        switch_xy:   (x in [1,2], y in [A,B]) switch for channel x and line y
                       D_OPEN   (open switch)
                       D_CLOSE  (close switch)

        return value:  D_OK      (function execution successfully finished)
                       1 ... 127 (function execution failed)
      */
/*~0*/
      /*--3.5.4 Select Receive Line----------------------------------------*/
      unsigned char d_sw_line_receive(unsigned char line_ch1,
                                      unsigned char line_ch2);
      /*
      with:
        line_ch1   Direction 1:
                     D_LINE_A    (line A will be trusted, line B is observed)
                     D_LINE_B    (line B will be trusted, line A is observed)
                     D_LINE_AUTO (driver switches automatically)

	line_ch2   Direction 2:
                     D_LINE_A    (line A will be trusted, line B is observed)
                     D_LINE_B    (line B will be trusted, line A is observed)

        return value:  D_OK      (function execution successfully finished)
                       1 ... 127 (function execution failed)

      Note: This function must be called during initalisation phase in order 
            to preconfigure the driver
      */
/*~0*/
      /*--3.5.5 Frittung on/off(option)------------------------------------*/
 unsigned char d_sw_fritting_source(
	     unsigned char command,
#define D_FRITTING_SOURCE_ENABLE 1
#define D_FRITTING_SOURCE_DISABLE 0
	     unsigned char direction,
	     unsigned char line);


      unsigned char d_sw_frittung_control(unsigned char command,
					  unsigned char direction,
					  unsigned char line);
      /*
      with:
        command:      D_FRITTING_OFF     (fritting OFF)
                      D_FRITTING_PASSIVE (fritting ON and passive)
                      D_FRITTING_ACTIVE  (fritting ON and active)
        direction:    D_DIRECT_1         (fritting switched to direction 1)
                      D_DIRECT_2         (fritting switched to direction 2)
        line:         D_LINE_A           (fritting switched to line A)
                      D_LINE_B           (fritting switched to line B)

	return value:  D_OK      (function execution successfully finished)
                       1 ... 127 (function execution failed)

      Note: Since ordinary relais are used for switching the link layer shall
            care for a 10 msec switching time after issuing the command
      */
/*~2*/
/*~9*/
   /*
   3.6 Interface Procedures for General I/O
   */
      /*--3.6.1 Installation-----------------------------------------------*/
      unsigned char d_io_install(void (*p_udf_io_interrupt_indicate)(void));
      /*
      with:
	p_udf_io_interrupt_indicate:
                       address of a user defined function (see 3.6.2)

        return value:  D_OK      (function execution successfully finished)
		       1 ... 127 (function execution failed)

      Note: 
        If a 'NULL'-pointer is used the related function will not be
        called by the driver
      */
/*~0*/
      /*--3.6.2 Interrupt input--------------------------------------------*/
      /* The driver calls this procedure if it detects a change in level of  
         interrupt input line (edje driven). This procedure was previously   
         announced to the driver during installation phase

      void p_udf_io_interrupt_indicate(void);

      */
/*~0*/
      /*--3.6.3 Interrupt output-------------------------------------------*/
      unsigned char d_io_interrupt_request(void);
      /*
      with:
	return value:  D_OK      (function execution successfully finished)
		       1 ... 127 (function execution failed)
      */
/*~2*/
/*~9*/
   /*
   3.7 Miscalleneous Services
   */
      /*--3.7.1 Get Random Number------------------------------------------*/
      unsigned char d_get_rand(void);
      /*
      This function returns a random  number in the range of 0...127
      */
/*~0*/
      /*--3.7.2 Set Sleep mode---------------------------------------------*/
      void d_sleep(void);
      /*
      This function forces the node hardware to enter sleep mode (low power
      consuption mode)
      */
/*~2*/
/*~2*/
/*~+*/
void d_led_update (unsigned char mask, unsigned char led);
void d_led_set (unsigned char led);
void d_led_reset (unsigned char led);


#define D_RECEIVER_IS_BUSY (    (*((unsigned char *)0x03f34L) & 0x80) \
			     || (*((unsigned char *)0x03f74L) & 0x80) )

unsigned char d_1_receive_quit(type_idu_drv *p_idu);
unsigned char d_2_receive_quit(type_idu_drv *p_idu);
