/*   BAD_DIAG.H
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland
|    COPYRIGHT    :   (c) 1992 ABB Verkehrssysteme AG
|
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|
|    File Name    :   BAD_DIAG
|
|    Document     :
|
|    Abstract     :   Diagnostic Module
|
|    Remarks      :
|
|    Dependencies :
|
|    Accepted     :
|
 ------------------------------------------------------------------------------
|
|    HISTORY:
|
|   Version     :   1   |
|   Date (Week) :   310 |     |     |     |     |     |     |     |     |
|   Name        :   Ma  |
|
 ------------------------------------------------------------------------------
*/

#ifndef     BAD_DIAG_H
#define     BAD_DIAG_H

#include    "bax_incl.h"
#include    "bal_link.h"

/* new page
 ----------------------------------------------------------------------------
|
|   name        BAD_STR_LLUT        Location Look Up Table
|
 ------------   A B S T R A C T   -------------------------------------------


	This structure is a look up table for the location of the DSW polls
	within the DMA lists. Even though the scan itself only defines 64
	scans per 512 ms, life sign requirements might mean that a scan is
	located in each of the 512 2 ms slots.

*/

struct  BAD_STR_DBUG
{
    unsigned short c_task;
};

struct  BAD_STR_SHORT_ARRAY
{
		unsigned short dsw[8];
};

struct BAD_STR_LLUT_ITEM
{
		struct BAD_STR_SHORT_ARRAY * ptr;
		unsigned                     cnt;
};

struct  BAD_STR_LLUT
{
        unsigned short               mf[512];
        unsigned short               mf_padding;
		unsigned 					 ix_cfgd;
		struct BAD_STR_LLUT_ITEM     cfgd[1024];
};


/* new page
 ----------------------------------------------------------------------------
|
|   name        BAD_STR_DTA         static data
|
 ------------   A B S T R A C T   -------------------------------------------


*/


struct          BAD_STR_DTA_ARRAY
{
	void        (*p_func)           (struct BA_STR_DIAGNOSIS * p_pt);
	unsigned    short               ivp;
    unsigned    long                c_error_frame;
	struct      BA_STR_DIAGNOSIS    pt[2];
	struct      BA_STR_SCAN_RESULT	scan_rslt[4];
    struct      BA_STR_POLL_STAT    poll_stat[2];
};

struct			BAD_STR_TASK
{
    struct BA_STR_SCAN_RESULT * pb_scan_rslt;
    unsigned                    strategy;
	unsigned 					ix_dvc;
	unsigned  					window;
	unsigned  					stp;
	unsigned 	 				prt_ad;
	unsigned                    prt_ix;
};

struct          BAD_STR_DTA
{
    struct      BAD_STR_DBUG        dbug;
	struct		BAD_STR_LLUT		llut;           /* location look up tab */
	struct		BAD_STR_TASK		task;
	struct      BAD_STR_DTA_ARRAY   id[1];
};




#define         bad_subscrbd        (*(bad_dta.id[0].p_func))


/*
 ---------------------------------------------------------------------------ƒ
|
|   Prototypes
|
 ----------------------------------------------------------------------------
*/

void 	bad_open			(struct BAD_STR_DTA ** 		p_bad_dta);

void    bad_init            (void);

void    bad_create          (struct BAM_STR_CFG_PT *  p_pt);

short   bad_reset           (void);

void    bad_diag            (void (*p_func) (struct BA_STR_DIAGNOSIS * p_pt));

void    bad_dummy           (struct BA_STR_DIAGNOSIS * p_pt);

void    bad_announce        (unsigned short 	*p_dest,
							 unsigned int   	cnt);

void	bad_invest          (void);

void    bad_c_error_frame_put (unsigned short c_error_frame);

void    bad_harvest	        (void);

void    bad_task            (void);

int     bad_ncfg            (void);

void    bad_dsw_read        (unsigned short                 dvc_nr,
                             struct BA_STR_SCAN_RSLT_MBR *  p_dvc_trgt);


/*
 ---------------------------------------------------------------------------ƒ
|
|   M O D I F I C A T I O N S       BAD_DIAG.H
|   - - - - - - - - - - - - -       ----------
|
 ----------------------------------------------------------------------------


    name: ......................... Nr: ....    date:   ..........

    description of what was done to object

 ----------------------------------------------------------------------------

*/

#endif
