/*=============================================================================================*/
/* Gateway utilities                 														   */
/* Header file (gtw_util.h)         														   */
/* 																							   */
/* Version 1.0																				   */
/*																							   */
/* (c)1998 Ansaldo Trasporti																   */
/* Written by Giuseppe Carnevale                                    		                   */
/*=============================================================================================*/


#define LEG(Condition)          Acc = (Condition)
#define AND(A)          Acc = Acc && (A)
#define OR(A)           Acc = Acc || (A)
#define STA1(A)         A   = Acc
#define STA0(A)         A   = !Acc

#define ONTD_N_NEW(A,n)   static unsigned int CN_##A[n],XN_##A[n]
#define ONTD_N_INIT(A,V,n)  {                       \
                               char i;              \
                               for (i=0; i<n; i++)  \
                               {                    \
                                   CN_##A[i]= V;    \
                                   XN_##A[i] = V;   \
                               }                    \
							 }


#define ONTD_NEW(A,V)   static unsigned int CN_##A = V, XN_##A = V
#define ONTD(A)         if (!Acc)               \
                            CN_##A = XN_##A;    \
                        else if (CN_##A)        \
                        {                       \
                            CN_##A--;           \
                            Acc=0;          \
                        }

#define OFTD_NEW(A,V)   static unsigned int CF_##A = V, XF_##A = V
#define OFTD(A)         if ( Acc)               \
                            CF_##A = XF_##A;    \
                        else if (CF_##A)        \
                        {                       \
                            CF_##A--;           \
                            Acc=1;           \
                        }
/*se cond passa va a 0 e ci rimane per cnt cicli, out =0 *
#define NOFLICK_ON(cond,cnt,out)                                            \
                        LEG(cond);// se la cond � 1 Acc=1                       \
                        OFTD(cnt);se cond ==0 decrementa il cont e cond=1     \
                                    se cnt =0 allora cond=0                   \
                        STA1(out);// ritorna cond che vale 0



*se cond passa va a 1 e ci rimane per cnt cicli, out =1 *
#define NOFLICK_OFF(cond,cnt,out)                                           \
                        LEG(cond);// se la cond � 1 Acc=1                       \
                        ONTD(cnt);se cond ==0 decrementa il cont e cond=1     \
                                    se cnt =0 allora cond=0 *                  \
                        STA1(out);// ritorna cond che vale 0
*/

/*=============================================================================================*/
/* Typedefs */

typedef struct {
	unsigned long	reboot_count;	/* number of reboots                               */
	unsigned char	veh_serial[4];	/* BCD coded vehicle serial number (0 = ignore)    */
	char			unused[4];
	char            noRido;
	char			was_gtwm;		/* TRUE if the last reboot we was the GTWM station */
	unsigned short	crc;			/* CRC calculated for this structure               */
} GtwApplConf;


/*=============================================================================================*/
/* Prototypes */

unsigned char tcn2uic(map_TrainTopography *map_topo, unsigned char tcn_address);
unsigned char uic2tcn(map_TrainTopography *map_topo, unsigned char uic_address);
unsigned char tcn2ord(unsigned char tcn_address);
char get_driving_direction(map_TrainTopography *map_topo);

short get_conf(GtwApplConf *conf, int *pos);
short set_conf(GtwApplConf *conf);

short carica_ncdb_cmd(short argc, char *argv[]);
short configura_cmd(short argc, char *argv[]);

//unsigned char bcd2bin(unsigned char );

unsigned short bin2bcd(unsigned char);

/*=============================================================================================*/

