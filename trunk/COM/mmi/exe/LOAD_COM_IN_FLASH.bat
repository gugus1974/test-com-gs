echo off
del c:\pemicro\pkg32z\startup.icd

echo macro C:\sw\mmi\exe\ram2flash.icd >startup.icd

copy ram2flash.cpy ram2flash.icd

copy startup.icd c:\pemicro\pkg32z

c:\pemicro\pkg32z\icd32z.exe io_delay_cnt 10 quiet

sleep 20
