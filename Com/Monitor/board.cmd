*	%W%	%G%
*
*	board.lnk
*
* Copyright (c) 1991-1996  Microtec Research, Inc.
* All rights reserved
*
*	Rev 4.2		G. Boehm, MRI, April 30, 1991
*			Add "format s" cause 6.7e linker
*			defaults to IEEE format
*
* 	New rev #1.1	G. Boehm, MRI, October 30, 1990
*			Put under SCCS
*
* This is the generic loader command file for the xdm68k monitor
* The strings in $$ are replaced with information required for loading
* the specific monitor.
* No chip specification, so picked based on input
format s
list x
listabs nointernals
* monbase is the base monitor user routines
load monbase
* board is the board drivers (I/O) and startup
load board
* xmon68k is the core monitor
load xmon68k
* The base is specified based on code start
sect 9,$400000
* The section base for data can be specified or not.
* If not, SECT14 must be order 13,14
order 9,13
SECT 14,$6000
SECT monitor_evt,$7000
end

