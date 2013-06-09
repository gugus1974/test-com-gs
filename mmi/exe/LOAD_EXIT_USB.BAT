rem echo off
del c:\pemicro\pkg32z\startup.icd
copy c:\swdel\testcom\exe\startup.icd c:\pemicro\pkg32z
echo exit >startup.icd
c:\pemicro\pkg32z\icd32z.exe io_delay_cnt 10 quiet
