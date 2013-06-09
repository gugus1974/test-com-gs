/* stop the target */
ss stop

/* Include some 68360 macros */
include 'c:\sw\com\68360.inc'

/* This macro initializes the COM board */
DEFINE init()
{
	/* Set the MBAR */
	$setstatus dfc=7$;
	$setstatus sfc=7$;
	*(long*)0x3FF00 = quicc + 1;
	$setstatus dfc=5$;
	$setstatus sfc=5$;

	/* Basic register configuration */
	*clkocr = 0x0C;       /* CLKOWP = x, COM2 = 3, COM1 = 0 */
	*pepar  = 0x00B4;     /* Bit7 = 1, PWW = 1, Bit4 = 1, Bit2 = 1 */
	*or0    = 0x3FF00002; /* DSSEL = 0, SPS = 1, FCM = 0, AM = 0x?FF00000, TCYC = 3 */
	*br0    = 0x00400001; /* V = 1, WP = 0, PAREN= 0, CSNTQ = 0, TRLXQ = 0, FC = 0, BA = 0x00400000 */
	*or1    = 0x1FE00000; /* DSSEL = 0, SPS = 0, FCM = 0, AM = 0x?FE00000, TCYC = 1 */
	*br1    = 0x00000001; /* V = 1, WP = 0, PAREN= 0, CSNTQ = 0, TRLXQ = 0, FC = 0, BA = 0 */
	*or7    = 0x10000000; /* Map all the remaining space to avoid bus errors in XOM68K */
	*br7    = 0x00000001;
}
.

c *or7    = 0x10000000 /* Map all the remaining space to avoid bus errors in XOM68K */
c *br7    = 0x00000001


/* put ourself in a safe place */
;;d 0
;;setreg @sp=0
;;setreg @pc=0

