
LISTABS	NOINTERNALS
LISTABS	NOPUBLICS
LISTMAP CROSSREF,PUBLICS/ADDR
CHIP	68360
ALIAS	9,code
ALIAS	13,const
ALIAS	13,strings
ALIAS	13,literals
ALIAS	13,vars
ALIAS   13,ehsftbl
ALIAS   13,ehscfn
ALIAS   13,0
ALIAS	14,zerovars
ALIAS	14,tags
*ABSOLUTE	9,13
ORDER	9,13,initfini
SECT 14	= $100000
SECT 9	= $008000





