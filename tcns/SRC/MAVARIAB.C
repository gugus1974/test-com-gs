/******************************************************************************
*         Copyright (C) Siemens AG 1994 All Rights Reserved                   *
*******************************************************************************
*
*    project:     TCN Train Communication Network
*                 JDP Joint Development Project
*
*    title:       Network Management Agent
*
*
*******************************************************************************
*
*    description: the agent part of TRAIN NETWORK MANAGEMENT
*
*    department:  VT 596 Communication
*    version:     0.0
*    language:    ANSI-'C'
*    system:
*
*    author:      E. Renner
*    history:     date        sign   remark
*                 11.10.1994  Re     created
*                 29.02.1995  Hil    adapted to new norm
*                                    some errors removed
*
*
******************************************************************************/


/******************************************************************************
*
*   name:         mavariab.c
*
*   function:     Serve Request PDU's for Variables Objects
*
*
*
*   input:        -
*
*   output:       -
*
*   return value: -
*
******************************************************************************/



#include <stdio.h>
#include "tcntypes.h"
#include "tnm.h"
#include "lp_sys.h"
#include "apd_incl.h"

#include "tcn_util.h"

#define ReadPortsConfiguration  30
#define ReadVariables           32
#define ForceVariables          33
#define UnForceVariables        35
#define UnForceAllVariables     37
#define AttachPort              29

int ma_VariablesObjects(struct TNMPDU *InPDU,struct TNMPDU * OutPDU,int * OutPDULen)
{
	 int  ReturnCode = OK;
	 int  len = 0;
	 int  i,k,l,ix;

	 UNSIGNED16      ipvname[3];

	 int             variableSize;
	 int             variableType;
	 unsigned short  *sr;

	 struct STR_APD_PV_DESCRIPTOR   apd_pv;
	 unsigned char					apd_var[32];   /* reserve maximum space */
	 unsigned char                  apd_val;

	 /* initialize the data pointers */
	 apd_pv.p_variable = (void *)apd_var;
	 apd_pv.p_validity = (void *)&apd_val;

	 switch (InPDU->SIF)
	 {
        case ReadPortsConfiguration:
		{
			 /* LME not yet available */
			 ReturnCode = MM_SIF_NOT_SUPPORTED;
		};
        break;

        case ReadVariables:
		{
			unsigned short i;	/* generic index  */
			unsigned short n;	/* number of vars */

			n = ma_decodeUNSIGNED16(&InPDU->Data[0]);
			len += ma_encodeUNSIGNED16(&OutPDU->Data[0], n);

			for (i = 0; i < n; i++)
			{
				short							size;		/* var size                         */
				struct STR_APD_PV_DESCRIPTOR	pvd;		/* process variable descriptor      */
				unsigned char					chk = 0;	/* check variable                   */
				unsigned short					refresh;	/* variable's freshness             */

				/* parse the arguments */
				unsigned short port_address = ma_decodeUNSIGNED16(&InPDU->Data[(i*8) + 2]);
			 	unsigned char  var_size     = ma_decodeUNSIGNED8 (&InPDU->Data[(i*8) + 4]);
			 	unsigned char  var_type     = ma_decodeUNSIGNED8 (&InPDU->Data[(i*8) + 5]);
				unsigned short var_offset   = ma_decodeUNSIGNED16(&InPDU->Data[(i*8) + 6]);
				unsigned short chk_offset   = ma_decodeUNSIGNED16(&InPDU->Data[(i*8) + 8]);

				/* build the STR_APD_PV_DESCRIPTOR pv_name */
				pvd.pv_name.ts_id      = port_address >> 12;
				pvd.pv_name.prt_addr   = port_address;
				pvd.pv_name.size       = var_size;
				pvd.pv_name.bit_offset = var_offset;
				pvd.pv_name.type       = var_type;
				pvd.pv_name.ctl_offset = chk_offset;

				/* calculate the data size */
				size = size_apdpvname(&pvd.pv_name);

				if (size != LPS_ERROR)
				{
					/* fix the pv_name to get an array if the size is more than 1 (Big Endian copy) */
					if (size > 1)
					{
						pvd.pv_name.type = (size & 1 ? LP_TYPE_A8_ODD : LP_TYPE_A8_EVEN);
						pvd.pv_name.size = (size - 1) / 2;
					}

					/* right justify variables shorter than 16-bits and not CHARACTER8 */
					if (size == 1 && !(pvd.pv_name.type == LP_TYPE_A8_ODD && pvd.pv_name.size == 0))
					{
					    OutPDU->Data[len++] = 0;
					}

					/* tell where we want the data */
					pvd.p_variable = (void*)&OutPDU->Data[len];
					pvd.p_validity = (void*)&chk;

					/* get the variable */
					if (apd_get_variable(&pvd, &refresh) == APD_OK)
					{
						/* sign extend an INTEGER8 */
						if (pvd.pv_name.type == LP_TYPE_SIGN && pvd.pv_name.size == LP_SIZE_8)
						{
							if (OutPDU->Data[len] & 0x80) OutPDU->Data[len-1] = 0xFF;
						}

						/* skip the data */
						len += size;

						/* word align with padding zero */
						if (len & 1) OutPDU->Data[len++] = 0;

						/* put the check and freshness */
						len += ma_encodeUNSIGNED16(&OutPDU->Data[len],
							(chk << 14) | (refresh > 4096 ? 4096 : refresh) );
					}
					else
					{
					    return NOTOK;
					}
				}
				else
				{
				    return NOTOK;
				}
			}

			/* set the total len */
			*OutPDULen = len + 2;
	  	}
	  	break;

	    case ForceVariables:
		{
			int DataOffset;
			int NbVars = ma_decodeUNSIGNED16(&InPDU->Data[0]);

//			len += ma_encodeUNSIGNED16(&OutPDU->Data[0],NbVars);

			for(i=0;i<NbVars;i++)
			{
                /* build up pv_name */
			    ipvname[0] = ma_decodeUNSIGNED16(&InPDU->Data[(i*6) + 2]);
			    ipvname[1] = ma_decodeUNSIGNED16(&InPDU->Data[(i*6) + 4]);
			    ipvname[2] = ma_decodeUNSIGNED16(&InPDU->Data[(i*6) + 6]);

			    /* setup the pv_name_part of apd_pv */
			    sr = (unsigned short *) &apd_pv.pv_name;

				for (k=0;k<3;k++)
				{
					*sr++ = ipvname[k];
				}

				/* determine length and type of the variable */
				variableSize   = (int) apd_pv.pv_name.size;
				variableType   = (int) apd_pv.pv_name.type;

				DataOffset = (NbVars) * 6 + 2;

				/* retrieve value of the variable */

				if ((variableSize == 0) && (variableType <= 0x07))
				{   /* byte variable */
					 apd_var[0] = (char) ma_decodeUNSIGNED16(&InPDU->Data[DataOffset]);
					 DataOffset += 2;
				}
				else if ((variableSize == 1) && (0x04 <= variableType <= 0x07))
				{  /* word variable */
					 *(UNSIGNED16 *) &apd_var[0] = ma_decodeUNSIGNED16(&InPDU->Data[DataOffset]);
					 DataOffset += 2;
				}
				else if ((variableSize == 2) && (0x03 <= variableType <= 0x06))
				{  /* dword variable */
					 *(UNSIGNED32 *) &apd_var[0] = ma_decodeUNSIGNED32(&InPDU->Data[DataOffset]);
					 DataOffset += 4;
				}
				else if ((variableSize == 3) && (variableType == 0x02))
				{       	/* time variable */
					 for(ix=0;ix<=5;ix++) apd_var[ix] = InPDU->Data[DataOffset+5-ix];
					 DataOffset += 6;
				}
				else if ((variableSize == 4) && (0x02 <= variableType <= 0x06))
				{   /* long long variable */
					 for(ix=0;ix<=7;ix++) apd_var[ix] = InPDU->Data[DataOffset+7-ix];
					 DataOffset += 8;
				}
				else
				{      /* its a array */
					switch (variableType)
					{
					    case 7:
						{              /* odd number of byte variables */
							for (l=0;l<variableSize;l++)
							{
								apd_var[l] = ma_decodeUNSIGNED8(&InPDU->Data[DataOffset]);
								DataOffset++;
							}
							/* adjust aligning */
							DataOffset++;
						};
						break;

						case 0xF:
						{              /* even number of byte variables */
							for (l=0;l<variableSize;l++)
							{
								apd_var[l] = ma_decodeUNSIGNED8(&InPDU->Data[DataOffset]);
								DataOffset++;
							}
						};
						break;

						case 0xd:
						/* fallthrough */

						case 0xe:
						{           /* number of word variables */
							for (l=0;l<variableSize;l++)
							{
								 *(UNSIGNED16 *) &apd_var[l*2] = ma_decodeUNSIGNED16(&InPDU->Data[DataOffset]);
								 DataOffset += 2;
							}
						};
						break;

						case 0xb:
						/* fallthrough */

						case 0xc:
						{           /* number of dword variables */
							for (l=0;l<variableSize;l++)
							{
								*(UNSIGNED32 *) &apd_var[l*4] = ma_decodeUNSIGNED32(&InPDU->Data[DataOffset]);
								DataOffset += 4;
							}
						};
						break;

						default:
						{
							ReturnCode = NOTOK;
						}
                    } /* end 'switch' */
	            } /* end 'else' */

			    /* set validity bit */
			    apd_val = 1;
			    if (apd_force_variable(&apd_pv)!= APD_OK)
			    {
				    ReturnCode = NOTOK;
		        };
		    }/* end of 'for' */
            /* default response */
	    };
	    break;


	    case UnForceVariables:
		{
			int NbVars = ma_decodeUNSIGNED16(&InPDU->Data[0]);

			for(i=0;i<NbVars;i++)
			{
			    /* build up pv_name */
			    ipvname[0] = ma_decodeUNSIGNED16(&InPDU->Data[(i*6) + 2]);
			    ipvname[1] = ma_decodeUNSIGNED16(&InPDU->Data[(i*6) + 4]);
			    ipvname[2] = ma_decodeUNSIGNED16(&InPDU->Data[(i*6) + 6]);

			    /* setup the pv_name_part of apd_pv */
			    sr = (unsigned short *) &apd_pv.pv_name;

			    for (k=0;k<3;k++)
			    {
			        *sr++ = ipvname[k];
			    }

			    if (apd_unforce_variable(&apd_pv.pv_name) != APD_OK)
			    {
				    ReturnCode = NOTOK;
                };
            }
            /* default response */
        };
		break;

	    case UnForceAllVariables:
	    {
		    UNSIGNED16 TStores = ma_decodeUNSIGNED16(&InPDU->Data[0]);
		    if (apd_unforce_all(TStores) != APD_OK)
		    {
			    ReturnCode = NOTOK;
		    };
		    /* default response */
	    };
	    break;

	    case AttachPort:
	    {
			ReturnCode = MM_CMD_NOT_EXECUTED;
	    };
	    break;

	    default:
	    {
	    	ReturnCode =MM_SIF_NOT_SUPPORTED;
	    }
    }
	return(ReturnCode);
}
