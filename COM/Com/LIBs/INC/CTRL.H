#ifndef CTRL_H
#define CTRL_H
/* 
FILE:      CTRL.H
AUTHOR:    EH, ABB CHCRC.C1
DATE:      02-Dec-1993
ABSTRACT:  interface between messenger and WTB train inauguration SW
*/   

/* DEFINES: --------------------------------------------------------------*/

#define topo_size   64
#define L_OK        0x0000

/* TYPES: ----------------------------------------------------------------*/

typedef unsigned short l_result;

typedef struct {
unsigned char  sw_version;
unsigned char  hw_state;
unsigned char  state;
unsigned char  link_adr;
unsigned char  direction;
unsigned char  reserved5;
unsigned char  reserved6;
unsigned char  operation_mode;
unsigned char  position;
unsigned char  strength;
unsigned char  reserved10;
unsigned char  reserved11;
unsigned char  reserved12;
unsigned char  reserved13;
unsigned char  reserved14;
unsigned char  reserved15;
} l_status_field;

typedef struct {
unsigned char status;
unsigned char lla;
} l_status;

typedef struct {
l_status        node_status;
unsigned short  node_descriptor;
} l_topography_field_member;

typedef struct {
unsigned short              index;
unsigned short              number_of_nodes;
l_topography_field_member   topography_field[topo_size-1];
} l_topography;
 
/* PROTOTYPES: -----------------------------------------------------------*/

l_result ls_t_GetNodeStatus (l_status_field *);
l_result ls_t_GetTopography (l_topography *);

#endif
