/*********************************************************************************
* lun.08.sett.03           v.1.00       \sw_st\sw_stb\tststbxx\header\tstflash.h *
**********************************************************************************
* Funzioni per il test della flash memory                                        *
**********************************************************************************
* 					                                         *
*********************************************************************************/


void tstflash(void );

short tstflash_vel(short argc, char *argv[] );

void save_stato (unsigned short stato);
void prn_stato (unsigned int prg, unsigned int cod);
short w_cmd(short argc, char *argv[]);
int chip_erase(void);
void ram2flash (void );
short prn_report(short argc, char *argv[]);
