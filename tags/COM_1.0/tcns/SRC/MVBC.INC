/********************************************************************/
/* 2005/giu/28,lun 14:30                   \SWISV\TCNS\SRC\mvbc.inc */
/********************************************************************/
/* MVBC Register dump                                               */
/********************************************************************/
/*
    $Date: 2005/06/28 13:45:46 $          $Revision: 1.3 $
    $Author: buscob $
*/

/* === General Functions ====================================================================== */

/* Returns the value of the specified register */
static int get_reg(ulong _addr, int _bit, int _size)
{
    int _mask;

    switch (_size)
    {
        case 0:  _mask =  0x0000; break;
        case 1:  _mask =  0x0001; break;
        case 2:  _mask =  0x0003; break;
        case 3:  _mask =  0x0007; break;
        case 4:  _mask =  0x000F; break;
        case 5:  _mask =  0x001F; break;
        case 6:  _mask =  0x003F; break;
        case 7:  _mask =  0x007F; break;
        case 8:  _mask =  0x00FF; break;
        case 9:  _mask =  0x01FF; break;
        case 10: _mask =  0x03FF; break;
        case 11: _mask =  0x07FF; break;
        case 12: _mask =  0x0FFF; break;
        case 13: _mask =  0x1FFF; break;
        case 14: _mask =  0x3FFF; break;
        case 15: _mask =  0x7FFF; break;
        default: _mask =  0xFFFF; break;
    }

    return (uint(_addr) >> _bit) & _mask;
}

/* Returns the value of a MVBC internal register */
static int get_mvbc_reg(int _off, int _bit, int _size)
{
    return get_reg(TM0_BASE+0xf000 + _off, _bit, _size);
}

/* Returns the address of the specified Device Address PCS */
static ulong get_mvbc_da_pcs_addr(int _index)
{
    return (TM0_BASE+0x4000 + ((ulong)uint(0x2000 + (ulong)_index*2) << 3));
}

/* Returns the address of the specified Device Address Data page 0 */
static ulong get_mvbc_da_data0_addr(int _index)
{
    ulong _pi;

    _pi = uint(TM0_BASE+0x2000 + (ulong)_index*2);
    return (TM0_BASE+0x8000 + (((_pi & 0xFFFC) << 4) | ((_pi & 3) << 3)));
}

/* Returns the address of the specified Logical Address PCS */
static ulong get_mvbc_la_pcs_addr(int _index)
{
    return (TM0_BASE + ((ulong)uint((ulong)_index*2) << 3));
}

/* Returns the address of the specified Logical Address Data page 0 */
static ulong get_mvbc_la_data0_addr(int _index)
{
    ulong _pi;

    _pi = uint(TM0_BASE + (ulong)_index*2);
    return (TM0_BASE + 0x10000 + (((_pi & 0xFFFC) << 4) | ((_pi & 3) << 3)));
}

/* Returns the address of the specified Logical Address Force page 0 */
static ulong get_mvbc_la_force0_addr(int _index)
{
    ulong _pi;

    _pi = uint(TM0_BASE + (ulong)_index*2);
    return (TM0_BASE+0x20000 + (((_pi & 0xFFFC) << 4) | ((_pi & 3) << 3)));
}

/* Returns the address of the specified Phisical Port PCS */
static ulong get_mvbc_pp_pcs_addr(int _index)
{
    return (TM0_BASE+0xf000 + 0xe00 + _index*8);
}

/* Returns the address of the specified Phisical Port data page 0 */
static ulong get_mvbc_pp_data0_addr(int _index)
{
    return (TM0_BASE+0xf000 + 0xc00 + (((_index & 0xFFFC) << 4) | ((_index & 3) << 3)));
}

/* Returns the message lenght in words associated with an F-code */
static int fcode2size(int fcode)
{
    switch (fcode)
    {
        case 0: case 8: case 9: case 13: case 14: case 15:
            return 1;
        case 1:
            return 2;
        case 2:
            return 4;
        case 3:
            return 8;
        case 4: case 12:
            return 16;
    }
    return 0;
}


/* === Dump Functions ========================================================================= */

/* The following two routines dump the interrupt registers */
static void out_mvbc_int_reg0(int _off)
{
    printf("    EMF = %x, ESF = %x, DMF = %x, DSF = %x, AMFX = %x, MFC = %x, SFC = %x, RTI = %x\n",
        get_mvbc_reg(_off, 15, 1),
        get_mvbc_reg(_off, 14, 1),
        get_mvbc_reg(_off, 13, 1),
        get_mvbc_reg(_off, 12, 1),
        get_mvbc_reg(_off, 11, 1),
        get_mvbc_reg(_off, 10, 1),
        get_mvbc_reg(_off, 9, 1),
        get_mvbc_reg(_off, 8, 1));
    printf("    BTI = %x, DTI7 = %x, DTI6 = %x, DTI5 = %x, DTI4 = %x, DTI3 = %x, DTI2 = %x, DTI1 = %x\n",
        get_mvbc_reg(_off, 7, 1),
        get_mvbc_reg(_off, 6, 1),
        get_mvbc_reg(_off, 5, 1),
        get_mvbc_reg(_off, 4, 1),
        get_mvbc_reg(_off, 3, 1),
        get_mvbc_reg(_off, 2, 1),
        get_mvbc_reg(_off, 1, 1),
        get_mvbc_reg(_off, 0, 1));
}

static void out_mvbc_int_reg1(int _off)
{
    printf("    TI2 = %x, XI3 = %x, XI2 = %x, XQE = %x, RQE = %x, XQ1C = %x, XQ0C = %x, RQC = %x\n",
        get_mvbc_reg(_off, 15, 1),
        get_mvbc_reg(_off, 14, 1),
        get_mvbc_reg(_off, 13, 1),
        get_mvbc_reg(_off, 12, 1),
        get_mvbc_reg(_off, 11, 1),
        get_mvbc_reg(_off, 10, 1),
        get_mvbc_reg(_off, 9, 1),
        get_mvbc_reg(_off, 8, 1));
    printf("    FEV = %x, TI1 = %x, XI1 = %x, XI0 = %x\n",
        get_mvbc_reg(_off, 7, 1),
        get_mvbc_reg(_off, 2, 1),
        get_mvbc_reg(_off, 1, 1),
        get_mvbc_reg(_off, 0, 1));
}

/* Outputs a PCS register */
static void out_mvbc_pcs(ulong _addr)
{
    printf("Port Control and Status Register PCS (@%lx) = %x %x %x %x\n",
        _addr,
        get_reg(_addr, 0, 16),
        get_reg(_addr+2, 0, 16),
        get_reg(_addr+4, 0, 16),
        get_reg(_addr+6, 0, 16));
    printf("    F = %x, SRC = %x, SINK = %x, TWCS = %x, WA = %x, IE = %x, CPE = %x, QA = %x, NUM = %x, FE = %x\n",
        get_reg(_addr, 12, 4),
        get_reg(_addr, 11, 1),
        get_reg(_addr, 10, 1),
        get_reg(_addr, 9, 1),
        get_reg(_addr, 8, 1),
        get_reg(_addr, 5, 3),
        get_reg(_addr, 3, 2),
        get_reg(_addr, 2, 1),
        get_reg(_addr, 1, 1),
        get_reg(_addr, 0, 1));
    printf("    DEC = %x, PTD = %x, VP = %x, CRC = %x, SQE = %x, ALO = %x, BNI = %x, TERR = %x, STO = %x\n",
        get_reg(_addr+2, 8, 8),
        get_reg(_addr+2, 7, 1),
        get_reg(_addr+2, 6, 1),
        get_reg(_addr+2, 5, 1),
        get_reg(_addr+2, 4, 1),
        get_reg(_addr+2, 3, 1),
        get_reg(_addr+2, 2, 1),
        get_reg(_addr+2, 1, 1),
        get_reg(_addr+2, 0, 1));
    printf("    TACK = %x, CS1 = %x, CS0 = %x\n",
        get_reg(_addr+4, 0, 16),
        get_reg(_addr+6, 8, 8),
        get_reg(_addr+6, 0, 8));
}

/* Outputs an MVBC port data area */
static void out_mvbc_port_data_area(ulong _addr, int fcode)
{
    int _n, _i;

    _n = fcode2size(fcode);
    for (_i = 0; _i < _n; _i++) printf("    %x\n", uint(_addr + _i*2));
}

/* Outputs an MVBC port */
static void out_mvbc_port(ulong _pcs_addr, ulong _data_addr, ulong _force_addr)
{
    out_mvbc_pcs(_pcs_addr);
    printf("Data Area (VP=0, @%lx):\n", _data_addr);
    out_mvbc_port_data_area(_data_addr, get_reg(_pcs_addr, 12, 4));
    printf("Data Area (VP=1, @%lx):\n", _data_addr+0x20);
    out_mvbc_port_data_area(_data_addr+0x20, get_reg(_pcs_addr, 12, 4));
    if (_force_addr != 0)
    {
        printf("Force Area (VP=0, @%lx):\n", _force_addr);
        out_mvbc_port_data_area(_force_addr, get_reg(_pcs_addr, 12, 4));
        printf("Force Area (VP=1, @%lx):\n", _force_addr+0x20);
        out_mvbc_port_data_area(_force_addr+0x20, get_reg(_pcs_addr, 12, 4));
    }
}

/* Outputs an MVBC Device Status Word */
static void out_mvbc_ds_word(ulong _addr)
{
    printf("    SP = %x, BA = %x, GW = %x, MD = %x\n",
        get_reg(_addr, 15, 1),
        get_reg(_addr, 14, 1),
        get_reg(_addr, 13, 1),
        get_reg(_addr, 12, 1));

    if (!get_reg(_addr, 15, 1))
    {
        if (get_reg(_addr, 14, 1))
        {
            printf("    AX1 = %x, AX0 = %x, ACT = %x, MAS = %x\n",
                get_reg(_addr, 11, 1),
                get_reg(_addr, 10, 1),
                get_reg(_addr, 9, 1),
                get_reg(_addr, 8, 1));
        }
        else if (get_reg(_addr, 13, 1))
        {
            printf("    STD = %x, DYD = %x\n",
                get_reg(_addr, 11, 1),
                get_reg(_addr, 10, 1));
        }
    }

    printf("    LAT = %x, OLD = %x, SSD = %x, SDD = %x, ERD = %x, FRC = %x, DNR = %x, SER = %x\n",
        get_reg(_addr, 7, 1),
        get_reg(_addr, 6, 1),
        get_reg(_addr, 5, 1),
        get_reg(_addr, 4, 1),
        get_reg(_addr, 3, 1),
        get_reg(_addr, 2, 1),
        get_reg(_addr, 1, 1),
        get_reg(_addr, 0, 1));
}


/* === User Functions ========================================================================= */

static void mvbc_pp(int _index)
{
    ulong _x, _y;

    _x = get_mvbc_pp_pcs_addr(_index);
    _y = get_mvbc_pp_data0_addr(_index);
    out_mvbc_port(_x, _y, 0);
}

static void mvbc_da(int _index)
{
    ulong _x;

    out_mvbc_pcs(get_mvbc_da_pcs_addr(_index));
    _x = get_mvbc_da_data0_addr(_index);
    printf("Data Area (VP=0, @%lx): %x\n", _x, uint(_x));
    out_mvbc_ds_word(_x);
    printf("Data Area (VP=1, @%lx): %x\n", _x+0x20, uint(_x+0x20));
    out_mvbc_ds_word(_x+0x20);
}

static void mvbc_la(int _index)
{
    ulong _x, _y, _z;

    _x = get_mvbc_la_pcs_addr(_index);
    _y = get_mvbc_la_data0_addr(_index);
    _z = get_mvbc_la_force0_addr(_index);
    out_mvbc_port(_x, _y, _z);
}

static void mvbc(int _mode)
{
    int _i;
    ulong _x;

    printf("*** MVBC registers dump ***\n");
    if (_mode == 0)
    {
        printf("No interrupt dump.\n");
    }
    else if (_mode == 1)
    {
        printf("Interrupt dump (freeze)\n");
    }

    printf("\nDecoder Register DR = %x : LAA = %x, RLD = %x, LS = %x, SLM = %x\n",
        get_mvbc_reg(0xf88, 0, 16),
        get_mvbc_reg(0xf88, 3, 1),
        get_mvbc_reg(0xf88, 2, 1),
        get_mvbc_reg(0xf88, 1, 1),
        get_mvbc_reg(0xf88, 0, 1));

    printf("\nMaster Frame Register MFR = %x\n", get_mvbc_reg(0xf98, 0, 16));
    printf("Master Frame Register Duplicate Exception MFRE = %x\n", get_mvbc_reg(0xf9C, 0, 16));

    printf("\nFrame Counter FC = %x\n", get_mvbc_reg(0xf90, 0, 16));
    printf("Error Counter EC = %x\n", get_mvbc_reg(0xf94, 0, 16));

    printf("\nStatus Control Register SCR = %x :\n", get_mvbc_reg(0xf80, 0, 16));
    printf("    IM = %x, QUIET = %x, MBC = %x, TMO = %x, WS = %x, ARB = %x,\n",
        get_mvbc_reg(0xf80, 15, 1),
        get_mvbc_reg(0xf80, 14, 1),
        get_mvbc_reg(0xf80, 13, 1),
        get_mvbc_reg(0xf80, 10, 2),
        get_mvbc_reg(0xf80, 8, 2),
        get_mvbc_reg(0xf80, 6, 2));
    printf("    UTS = %x, UTQ = %x, MAS = %x, RCEV = %x, IL = %x\n",
        get_mvbc_reg(0xf80, 5, 1),
        get_mvbc_reg(0xf80, 4, 1),
        get_mvbc_reg(0xf80, 3, 1),
        get_mvbc_reg(0xf80, 2, 1),
        get_mvbc_reg(0xf80, 0, 2));

    printf("\nMaster Register MR = %x :\n", get_mvbc_reg(0xfa0, 0, 16));
    printf("    PAR1 = %x, PAR0 = %x, EA1 = %x, EA0 = %x, EC1 = %x, EC0 = %x,\n",
        get_mvbc_reg(0xfa0, 15, 1),
        get_mvbc_reg(0xfa0, 14, 1),
        get_mvbc_reg(0xfa0, 13, 1),
        get_mvbc_reg(0xfa0, 12, 1),
        get_mvbc_reg(0xfa0, 11, 1),
        get_mvbc_reg(0xfa0, 10, 1));
    printf("    BUSY = %x, CSMF = %x, SMF = %x, SMFM = %x, C = %x\n",
        get_mvbc_reg(0xfa0, 9, 1),
        get_mvbc_reg(0xfa0, 8, 1),
        get_mvbc_reg(0xfa0, 6, 2),
        get_mvbc_reg(0xfa0, 5, 1),
        get_mvbc_reg(0xfa0, 0, 5));
    printf("Secondary Master Register MR2 = %x : SMF = %x, SMFM = %x, C = %x\n",
        get_mvbc_reg(0xfa4, 0, 16),
        get_mvbc_reg(0xfa4, 6, 2),
        get_mvbc_reg(0xfa4, 5, 1),
        get_mvbc_reg(0xfa4, 0, 5));

    printf("\nDispatch Pointer Register DPR = %x\n", get_mvbc_reg(0xfa8, 0, 16));
    printf("Secondary Dispatch Pointer Register DPR2 = %x\n", get_mvbc_reg(0xfaC, 0, 16));

    printf("\nDevice Address Register DAOR = %x\n", get_mvbc_reg(0xfd8, 0, 12));
    printf("Device Address Override Key DAOK = %x\n", get_mvbc_reg(0xfdC, 0, 8));

    printf("\nMemory Configuration Register MCR = %x : MO = %x, QO = %x, MCM = %x\n",
        get_mvbc_reg(0xf84, 0, 16),
        get_mvbc_reg(0xf84, 5, 2),
        get_mvbc_reg(0xf84, 3, 2),
        get_mvbc_reg(0xf84, 0, 3));

    if (_mode == 1)
    {
        printf("\nInterrupt Pending Register 0 IPR0 = %x :\n", get_mvbc_reg(0xfb0, 0, 16));
        out_mvbc_int_reg0(0xfb0);
        printf("Interrupt Pending Register 1 IPR1 = %x :\n", get_mvbc_reg(0xfb4, 0, 16));
        out_mvbc_int_reg1(0xfb4);

        printf("Interrupt Mask Register 0 IMR0 = %x :\n", get_mvbc_reg(0xfb8, 0, 16));
        out_mvbc_int_reg0(0xfb8);
        printf("Interrupt Mask Register 1 IMR1 = %x :\n", get_mvbc_reg(0xfbc, 0, 16));
        out_mvbc_int_reg1(0xfbc);

        printf("Interrupt Status Register 0 ISR0 = %x :\n", get_mvbc_reg(0xfc0, 0, 16));
        out_mvbc_int_reg0(0xfc0);
        printf("Interrupt Status Register 1 ISR1 = %x :\n", get_mvbc_reg(0xfc4, 0, 16));
        out_mvbc_int_reg1(0xfc4);

        printf("Interrupt Vector Register 0 IVR0 = %x : IAV = %x, VEC = %x\n",
            get_mvbc_reg(0xfc8, 0, 16),
            get_mvbc_reg(0xfc8, 8, 1),
            get_mvbc_reg(0xfc8, 0, 4));
        printf("Interrupt Vector Register 1 IVR1 = %x : IAV = %x, VEC = %x\n",
            get_mvbc_reg(0xfcc, 0, 16),
            get_mvbc_reg(0xfcc, 8, 1),
            get_mvbc_reg(0xfcc, 0, 4));
    }

    printf("\nTimer Control Register TCR = %x : RS2 = %x, TA2 = %x, XSYN = %x, RS1 = %x, TA1 = %x\n",
        get_mvbc_reg(0xfe0, 0, 16),
        get_mvbc_reg(0xfe0, 5, 1),
        get_mvbc_reg(0xfe0, 4, 1),
        get_mvbc_reg(0xfe0, 2, 1),
        get_mvbc_reg(0xfe0, 1, 1),
        get_mvbc_reg(0xfe0, 0, 1));
    printf("Timer Reload Register 1 TR1 = %x\n", get_mvbc_reg(0xff0, 0, 16));
    printf("Timer Reload Register 2 TR2 = %x\n", get_mvbc_reg(0xff4, 0, 16));
    printf("Timer Counter Register 1 TC1 = %x\n", get_mvbc_reg(0xff8, 0, 16));
    printf("Timer Counter Register 2 TC2 = %x\n", get_mvbc_reg(0xffc, 0, 16));

    printf("\nSink-Time Supervision Register STSR = %x : SI = %x, R = %x\n",
        get_mvbc_reg(0xf8c, 0, 16),
        get_mvbc_reg(0xf8c, 12, 4),
        get_mvbc_reg(0xf8c, 0, 12));

//    _x = 0xc00000 + ((ulong)get_mvbc_reg(0xf84, 5, 2) << 18) + ((ulong)get_mvbc_reg(0xfa8, 0, 16) << 2);
    _x = TM0_BASE + ((ulong)get_mvbc_reg(0xf84, 5, 2) << 18) + ((ulong)get_mvbc_reg(0xfa8, 0, 16) << 2);
    printf("\nMaster Frame Table (@%lx) :\n", _x);
    for (_i = 0; _i < get_mvbc_reg(0xfa0, 0, 5); _i++) printf("    %x\n", uint(_x + _i*2));
//    _x = 0xc00000 + ((ulong)get_mvbc_reg(0xf84, 5, 2) << 18) + ((ulong)get_mvbc_reg(0xfac, 0, 16) << 2);
    _x = TM0_BASE + ((ulong)get_mvbc_reg(0xf84, 5, 2) << 18) + ((ulong)get_mvbc_reg(0xfac, 0, 16) << 2);
    printf("Secondary Master Frame Table (@%lx) :\n", _x);
    for (_i = 0; _i < get_mvbc_reg(0xfa4, 0, 5); _i++) printf("    %x\n", uint(_x + _i*2));
}

/* ****************************************************************************
    $Log: MVBC.INC,v $
    Revision 1.3  2005/06/28 13:45:46  buscob
    Si sostituiscono gli indirizzi MVBC fissi con la costante TM0_BASE specificata in maniera diversa per ogni singola scheda

*/