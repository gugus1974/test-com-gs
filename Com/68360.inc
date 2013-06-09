/* some useful symbols */
ADD char  *quicc  = 0xFFFF8000
ADD char  *regb   = quicc + 0x1000
ADD char  *clkocr = regb + 0x0C
ADD short *pepar  = regb + 0x16
ADD long  *or0    = regb + 0x54
ADD long  *br0    = regb + 0x50
ADD long  *or1    = regb + 0x64
ADD long  *br1    = regb + 0x60
ADD long  *or7    = regb + 0xC4
ADD long  *br7    = regb + 0xC0
ADD long  *bkar   = regb + 0x30
ADD long  *bkcr   = regb + 0x34

/* A nice macro with aliases to set an hardware breakpoint */
DEFINE bk(addr, rw, siz)
int addr;
int rw;
int siz;
{
	*bkar = addr;
	*bkcr = (rw << 16) | (siz << 13) | 1;
}
.
DEFINE bkc()
{
	*bkcr = 0;
}
.
alias 'BKRL' = bk($*, 0, 0)
alias 'BKWL' = bk($*, 1, 0)
alias 'BKXL' = bk($*, 2, 0)
alias 'BKRB' = bk($*, 0, 1)
alias 'BKWB' = bk($*, 1, 1)
alias 'BKXB' = bk($*, 2, 1)
alias 'BKRW' = bk($*, 0, 2)
alias 'BKWW' = bk($*, 1, 2)
alias 'BKXW' = bk($*, 2, 2)
alias 'BKR3' = bk($*, 0, 3)
alias 'BKW3' = bk($*, 1, 3)
alias 'BKX3' = bk($*, 2, 3)
alias 'BKRX' = bk($*, 0, 4)
alias 'BKWX' = bk($*, 1, 4)
alias 'BKXX' = bk($*, 2, 4)
alias 'BKC'  = bkc()

alias 'NILL'  = setreg @pc=@pc+2

