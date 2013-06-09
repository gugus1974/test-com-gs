                        \SWISV\TCNS\history.mod
************************************************************************
                    Storia globale del modulo TCNS
************************************************************************


************************************************************************
$Log: history.mod,v $
Revision 7.5  2005/10/12 14:08:13  mungiguerrav
----------------------------------------------------------------------
        Release: rel_07e    2005/10/12   15:52:28
FEATURE
    - Sin da 2005/06 (in TEST_07e), Busco corregge baco in STB segnalato da team E402c
      su indirizzamento diretto che non usa TM0_BASE
    - Si introduce una gestione delle variabili I_xxxx dei makefile/incfile
      che consente di individuare quelle vuote per errore
      A causa di questa riorganizzazione si devono eliminare gli include incrociati:
        - in IM_atr_sys     si eliminano:       I_bah_hwre   e   I_bai_ifce
        - in I_bal_link     si elimina:         I_bah_hwre

- Si noti che contrariamente a quanto indicato da Busco nella Release rel_07d,
  vi sono altri 7 file che segnalano warning:
    rtxconf.c, nm_stadi.c, map_npv.c, map_ui.c, bal_link.c, lpx_list.c, lp_var.c

CHANGE
makefile
    - Per la mancata segnalazione delle variabili $(I_xxx) vuote,
      in ogni uso di una variabile, si aggiunge l'estensione dell'ultimo file
incfile
    - Per la mancata segnalazione delle variabili $(I_xxx) vuote,
      in ogni definizione di una variabile:
        - si abolisce l'estensione dell'ultimo file
        - in ogni uso di altre variabili, si aggiunge l'estensione dell'ultimo file

BUGFIX
mvbc.inc
    - Si sostituiscono gli indirizzi MVBC fissi con la costante TM0_BASE specificata in maniera
      diversa per ogni singola scheda
bal_link.h
    - Si commenta l'include incrociato con bah_hwre.h
----------------------------------------------------------------------

Revision 7.4  2005/06/17 14:04:07  buscob
----------------------------------------------------------------------
        Release: rel_07d    2005/06/17   15:57:24

- Si eliminano tutti i riferimenti alla scheda STR

makefile
    - Si inseriscono i test:
        - exist         prima di cancellare gli oggetti
        - not exist     prima di creare directory OBJxx

*.obc
    - Si aggiungono keyword CVS
    - Si elimina il parametro sulla versione del compilatore Keil in uso

defKeil.bat
    - Aggiunto per definire la versione del compilatore keil in uso

Am_call.c, Am_reply.c, BAA_AUXI.C, BAH_HWRE.C, BAP_POLL.C, Bal_link.c, LM_V_LAY.C
LPX_LIST.C, LP_VAR.C, Lpl_lacs.c, MAP_NPV.c, MAVARIAB.C, Map_ui.c, NM_STADI.C, apd_layr.c
    - Si eliminano warning di compilazione.

- Restano ancora da correggere i seguenti file:
  lc_layer.c, lpd_extd.c, lp_init.c, maclock.c, madomain.c, manesse.c, mastatio.c, ma_mvb.c
----------------------------------------------------------------------

Revision 7.3  2005/01/26 10:08:20  mungiguerrav
----------------------------------------------------------------------
        Release: rel_07c    2005/01/26   10:58:14
A\STB\SRC\
    hw_stb.c
        - Si mod. impostazione del registro di configurazione del Baud Rate S0CON
          passando da 0x8011 a 0xA011 per continuare ad avere un BR=38400 con il
          nuovo clock della scheda STB a 24Mhz nel comando di bootload
        - Sostituisce hw_sta.c (che aveva il nome errato)

A\XXX\SRC\
    hw_xxx.c
        - Si aggiungono keyword CVS e si inserisce storia di hw_star.c
        - Si dovrebbero effettuare le modifiche per staccare definitivamente i
          i vari   hw_stX.c o hw_deX.c dall'antenato hw_star.c

    hw_xxx.obc, rtx_conf.obc
        - Si aggiungono keyword CVS e storia invertita
        - Adegua il test di errore a Win2000        (if "%risXX%"=="0")
        - Si introduce un test anche alla fine della fase di compilazione senza assembler,
          per avere notizie anche sul file *.src prodotto
        - Migliora le informazioni fornite in caso di errore di compilazione e assembler

    malloc.c,.h,    pi_rtx.c,.h     pi_rtx_a.a66,.obc
        - Si spostano dalle dir inferiori   A\XXX\SRC
          alla directory superiore          SRC
          in quanto sono comuni a tutte le schede

    makefile.
        - Si eliminano riferimenti a malloc, pi_rtx, pi_rtx_a, spostati nella directory superiore TCNS\SRC
        - Si aggiungono dipendenze dai file di include di primo livello
        - Si aggiunge l'inserimento singolo in tcns.lib con lib166 REPLACE
        - Dopo prime sperimentazioni con Nappo, si decide di non inserire rtxconf.obj
          in tcns.lib, in quantio il linker Keil ne ha bisogno esternamente per risolvere
          i riferimenti alle librerie del sistema operativo RTX
        - Si agg.   include incfile di TCNS, LIB,  LBATR, LBATR\A\HW,
          necessari alla gestione degli include a piu' livelli
        - Nei prerequisiti dei target oggetto si aggiungono le variabili di tipo I_xxxx,
          definite negli incfile
        - Come fatto necessariamente per rtxconf.obj, si decide per correttezza formale
          di abolire l'inserimento di hw_xxx.obj in tcns.lib, in attesa di definire meglio la
          separazione tra driver per la XXX e driver per TCN

SRC\
    default.obc
        - Si aggiungono keyword CVS e storia invertita
        - Adegua il test di errore a Win2000        (if "%risXX%"=="0")
        - Migliora le informazioni fornite in caso di errore di compilazione
        - Si utilizza nuovo batch cglx invece di cgl16x, che permette di dichiarare
          la libreria in cui si agisce (L16x) nel primo parametro

    lib_ver.h
        - Abolito ed assorbito in strt_tcn.c
    strt_tcn.c
        - Si elimina include lib_ver.h e si sost. il define LIB_VER direttamente con
          la stringa  fissa: "Comando Obsoleto: usare [vers] "

    malloc.c,.h,    pi_rtx.c,.h     pi_rtx_a.a66,.obc
        - Si aggiungono immodificati dalle directory inferiori TCNS\A\XXX\SRC,
          prima di iniziare il processo di ripulitura e adeguamento alle regole
          di codifica e alle regole di gestione CVS

    malloc.c,.h,    pi_rtx.c,.h     pi_rtx_a.a66
        - Si aggiungono keyword CVS e storia invertita
        - Si sostituiscono TAB con 4 spazi
        - Si applicano le regole di codifica sulle parentesi

    pi_rtx_a.obc
        - Si trasforma da .obc di un singolo file nelle dir inferiori A\XXX\SRC
          in un file .obc che gestisce un solo sorgente SRC\pi_rtx_a.a66 e
          crea gli oggetti in tutte le DIR inferiori A\XXX\OBJ7L\pi_rtx_a.obj
        - BugFix:   si aggiunge uso di  gtcns   per creare ambiente adatto alla
                    compilazione nelle dir inferiori    A\XXX
        - Si utilizza nuovo batch glx invece di gtcns, che permette di dichiarare
          la libreria in cui si agisce (TCNS) nel primo parametro
          (si noti che non si può usare  cglx  in quanto il file da copiare è un  .a66 e non un  .c)

\
    incfile.
        - Si crea per permettere ai mekefile di gestire gli include a piu' livelli:
          in questo file sono create le variabili con gli include a cascata dei file
          header presenti nella dir  SRC  e  A\DEA,DEV,STA,STB,STR\SRC
        - Si noti la variabile   IM_atr_sys,   ove IM significa Include Multiplo, in quanto
          fa riferimento a   atr_sys.h   presente in tutte le cinque dir di scheda

    makefile.
        - Si aggiungono le variabili per le le dipendenze dai file di include di qualunque livello
        - Si inseriscono tali dipendenze per tutti i file oggetto
        - Si aggiunge la creazione dei file A\XXX\OBJ7L\tcns.lib vuoti alla fine delle esecuzioni 'md' e 'del'
        - Si elimina la creazione e riempimento totale obbligato dei file A\XXX\OBJ7L\tcns.lib
          alla fine di ogni compilazione, anche di un solo file
        - Si aggiunge il riempimento dei file A\XXX\OBJ7L\tcns.lib per ogni singolo file
          compilato, usando il comando REPLACE (non indicato nel manuale Keil !!!)
        - Si definiscono le variabili   DXXX        con le dir oggetto
        - Si corregge la posizione di pi_rtx.h  da   A\XXX\SRC  a   SRC
        - Si richiama il target makeSchede prima di tcns e non per ogni singola scheda
        - Si aggiungono i file malloc e pi_rtx (che prima erano nelle dir A\XXX\SRC di scheda)
        - Si eliminano i target intermedi di tcns (ossia dea dev sta stb str)
        - Si introduce batch  tcnRep   per il comando   lib166 REPLACE
        - Si prova a raggruppare gli oggetti provenienti da un unico sorgente per colonne, ma poi nella
          compilazione si deve usare il raggruppamento per riga: questo fa pensare !
        - Si definisce la variabile inc_atr_sys per tutti i file A\XXX\SRC\atr_sys.h
        - Si unificano in unico gruppo le variabili di include (ossia si eliminano le
          variabili _v, _a, _b, _r che Formato aveva creato per le singole schede
          DEv, STa, STb, STr per gestire la sola differenze dei file atr_sys.h
        - Si semplifica la definizione delle variabili di include usando SRC\ al posto di \swisv\tcns\src\
        - Si passa al raggruppamento per righe degli oggetti di un solo sorgente
        - La stessa riga è poi usata per il comando di compilazione con target multiplo
        - Si aboliscono tutti i riferimenti dedicati alle schede DEV, STA, STB, STR,
          assorbiti nella righe di compilazione con target multiplo
        - BugFix:   dopo prime prove con Nappo su applicativo Los Angeles, si scopre
                    la mancanza dei riferimenti per pi_rtx_a, spostato da A\XXX\SRC
                    a   SRC     (come malloc e pi_rtx)
        - Si agg.   include incfile di TCNS, LIB,  LBATR, necessari alla gestione degli include a piu' livelli
        - Nei prerequisiti dei target oggetto si aggiungono le variabili di tipo I_xxxx,  definite negli incfile
        - Per consentire di distinguere il momento della compilazione da quello
          dell'inserimento in XXX\OBJ7L\tcns.lib:
            - Si agg.   gestione della directory  OJINL con i file *.inL, che indicano
              per ogni oggetto l'effettivo inserimento in tcns.lib: essi sono creati da
              tcnRep al momento dell'inserimento
            - Dopo la compilazione uca *, si abolisce  l'inserimento immediato tcnRep *
            - Per gli oggetti creati da sorgenti assembler, si utilizza tcnRepA *
----------------------------------------------------------------------

Revision 1.6  2004/09/03 13:00:24  mungiguerrav
------------------------------------------------------------
        Release: tst_07b    2004/09/03   14:52:00   Napoli  Mungi
- Si correggono errori in strt_tcn.c per token CVS non commentati di cui è stato
  possibile accorgersi solo con un tentativo di creazione dell'eseguibile di un applicativo
------------------------------------------------------------

Revision 1.5  2004/09/02 15:10:28  mungiguerrav
TST_07a             Napoli              Mungi\Busco
- Questo secondo COMMIT permette di inserire il Template completo
  di come dovrebbe essere iniziato il commento di un History.mod
  in fase di creazione di un tag (ex emissione di Foto)

Revision 1.4  2004/09/02 14:41:48  mungiguerrav
                    Napoli          Mungi\Busco
- Si correggono errori in A\XXX\SRC\atr_sys.h su definizione TM0_BASE

Revision 1.3  2004/09/02 12:41:16  mungiguerrav
2004/09/02,gio 14:00    Napoli          Mungi\Busco
- mrgTo00
- Merge dal branch di sviluppo   makefile
- Le operazioni seguenti sono state fatte anche per rivoluzioanre la struttura
  dell'area di lavoro e renderla simile a quelle standard, come L16x

- Dal 2004/06/23 al 2004/07/06 si modificano i file:
    - Per creare la scheda STB:
        build_hw.bat
	    SRC\HW_STAR.C
	    SRC\hw_stb.src
        INC\atr_sys.h       (indirizzo TMO_BASE diverso)
    - Per alzare il clock a 24 MHz
        INC\rtxsetup.h
        LIB\rtxclk24.obj
   e si ottiene     LIB/tcns.lib    valida solo per STB
- Dal 2004/07/09 al 2004/07/15
    - Busco inserisce nella nuova directory:
            \SWISV\TCNS\SRC
      tutti i sorgenti di:
            \SWTCN\TCN\ATR
            \SWTCN\TCN\JDP\XX
            \SWTCN\TCN\MOD
    - Busco elimina tutti i file sotto INC (sono ora in SRC)
    - Busco elimina tutti i file sotto LIB (sono ora in OBJ7L, da togliere poi dal CVS)
    - Busco inserisce replicati in tutte le nuove directory di:
            \SWISV\TCNS\A\XXX\SRC       ove     XXX=DEA,DEV,STA,STB,STR
      tutti i sorgenti di:
            \SWTCN\STAR\PIL
            \SWISV\TCNS\INC\atr_sys.h       (che ha creato la incompatibilita' su STB)
            \SWISV\L16x\SRC\hw_star.c       (rinominando in     hw_xxx.c)
      in modo da poter gestire le personalizzazioni
      (le schede DEA,DEV e poi STR sono aggiunte in passi successivi,
       correggendo anche errori ed omissioni di file *.obc)
    - Busco/Nappo aggiungono    TCNS\makefile    per gestire:
            - creazione automatica da sorgenti generali     di oggetti di scheda
            - creazione automatica di   TCNS\A\XXX\OBJ7L\tcns.lib
    - Busco/Nappo aggiungono nelle dir A\XXX:
        - application.set   per gestire la compilazione condizionata con #ifdef
                            in stile Carnevale
        - makefile          per gestire la creazione automatica da sorgenti dedicati
- 2004/07/22
    - Busco/Schiavo riapplicano le modifiche delle foto tcns05 e tcns06 a questo branch di sviluppo
      ottenendo la foto tradizionale    SWTCN07.rar
- 2004/09/01
    - Mungi\Busco aggiungono compilazione e inserimento in tcns.lib di tcnsvrs.obj

======================================================
swtcn07.rar             Napoli, Gio.22.lug.2004     Schiavo
Aggiunte modifiche da swtcn06:

- mod. strt_tcn.c: si agg. condizione sull'install. del mapping
  su device MVB si inserisce pragma
- mod. conf_tcn.c: si agg. funzione per config mapping su componenti MVB
- mod. map_npv.c: si introduce possibilita' di richiedere il NADI ogni 800 cicli (64*800 ms)
  fino all'ottenimento dello stesso. Con questa modifica si elimina la non
  installazione del mapping server sui componenti MVB che partono in ritardo
  rispetto al GTW

======================================================
t2cns06.rar                Napoli, Ven.30.nov.2001 Schiavo
- mod. strt_tcn.c ver 4.1: si agg. condizione sull'install. del mapping
  su device MVB si inserisce mod. Busco/Iusto
- mod. conf_tcn.c ver 2.1: si agg. funzione per config mapping su componenti MVB
- mod. map_npv.c: Le richieste per NADI sono distanziate di 800* 64 msec.

======================================================
t2cns05.rar                Napoli, Mar.20.nov.2001 Schiavo
- mod. map_npv.c per avere molteplici richieste del NADI da parte dei
  componenti MVB fino alla transizione del mapping server nello stato
  MAP_UPDATE_READY. Le richieste sono distanziate di 200* 64 msec.


======================================================
    Modifica relativa solo al branch di sviluppo per PCA
    (ossia AIACE\MADRID e AIACE\SIRIO)
tcns04a.rar             Napoli, 2002.dic.11,mar 15:10, Mungi
- Si abolisce la distinzione tra uv1 e uv2
- Si introduce tutta la storia che e' stato possibile ricostruire di
  strt_tcn.c, prima di applicare la fondamentale modifica
        - Si sost. macro        STRT_ROUTINE_NAME
          con   nome fisso      startcn_init()
  con cui si estirpa da questo file il main per le schede DEA e STA
  per portarlo in cpudrv.c, come fatto sara' fatto nella Lib06e
  in fase di preparazione ed emissione


======================================================
t2cns04.rar             Napoli, Ven.25.feb.2000 08:30, Mungi
- Si ripete foto precedente con stesso nome per provare
  gestione della versione con struct gestita da LIB\SRC\versio.c,.h
  e creata da nuovo COMANDI\fotcn.bat 1.01

======================================================
t2cns04.rar             Napoli, Lun.21.feb.2000 11:40, Mungi
- Stesse modifiche della foto precedente, ma utilizzando l'ambiente
  uVision 2.xx e creando quindi la directory TCNS2

======================================================
t1cns04.rar             Napoli, Lun.21.feb.2000 11:35, Mungi
- Le seguenti directory sono eliminate dalla libreria e spostate:
\SW\ATR         ==>     \SWISV\LBATR\SRC
\SW\STAR\HW     ==>     \SWISV\LBATR\A\HW\SRC
\SW\STAR\SCHIO  ==>     \SWISV\LBATR\A\HW\SRC

- Si crea una nuova libreria utilizzando l'ambiente uVision 1.xx

- Si modifica opportunamente \SWTCN\STAR\star1.prj, per creare la nuova
  libreria il cui nome cambia in tcns.lib
- Si agg. \SWISV\LBATR\.. nel path di INC

----Modifiche esterne:
\TCNS1                                  Si elimina update.bat
\TCNS1\INC                              Da 40 a 23 file *.h (spostati in LBATR)
\TCNS1\LIB\star.lib                     diventa         tcns.lib
\TCNS1\LIB\rtxclk16,20                  inalterati
\TCNS1\LIB\hw_XXX.obj,startXXX.obj      spostati in \SWISV\LIB\A\XXX\OBJ7
\TCNS1\LIB\dp_comm1.obj,dp_mbx.obj      spostati in \SWISV\LIB\OBJ7
\TCNS1\SRC\strt_tcn.c                   inalterato
\TCNS1\SRC\dp_comm2.c,dp_mbx.c          spostati in \SWISV\LIB\
\TCNS1\SRC\brdsetup.c                   spostato in \SWISV\LBATR\A\HW\SRC\

- La procedura \SWTCN\STAR\update.bat mantiene per compatibilita'
  INC\lib_ver.h, ma ne cambia il contenuto:
  da:   #define LIB_VER "User: Carnevaleg     Date: 14/04/99   Time: 10.31"
  a:    #define LIB_VER " Comando Osoleto: usare [vers] "

----Modifiche interne:
TCN\ATR\atr_nma.c  1.40 18.feb.00 - Si eliminano inutili include am29f040.h
                                    e crc.h
                                  - Si applicano le regole di editing ATR\ISV
TCN\JDP\LC\lc_layer.c   19.feb.00 Si mod. include lc_head.h
TCN\ATR\tcn_mon.c       19.feb.00 Si mod. include bai_ifce.h

- La procedura \SWISV\COMANDI\fotcn.bat crea:
  LIB\tcnsvrs.obj,.lst  contenente versione,data,ora della libreria
  che l'applicativo dovra' gestire con il nuovo comando [vers] unificato

======================================================
tcns04.rar              Napoli, Lun.21.feb.2000 09:30
    Ultimo rilascio di Carnevale Mar.13.lug.1999,
primo ed unico con l'ambiente uVision 2.xx (compilatore 4.02):

----Modifiche esterne:
INC\xilinx.h    17.mar.99   New entry
INC\atr_sys.h   13.mag.99   ???
INC\rio_mon.h   15.giu.99   New_entry
INC\rio_ssc.h   16.giu.99   New_entry
INC\strt_tcn.h  16.giu.99   3.9 Si utilizza PI_TASK_DEF
INC\conf_tcn.h  21.giu.99   Si rendono  const  i campi  char *txt_name
INC\c16x.h      01.lug.99   Si intr. la macro _pin(type,port,bit) per fare le
                            stesse definizioni che sono in \SWISV\..\bitfield.h
INC\pec.h       07.lug.99   Si intr. protez. ifndef _PEC_
                            Nella macro  pec_wait(chan,timeout)  si agg. un
                            _nop_() dopo IEN=0
INC\pi_rtx.h    07.lug.99   Nella macro  pi_disable()  si agg. un _nop_()
                            dopo _bfld_(PSW,..)
INC\c_utils.h   13.lug.99   Varie modifiche (bug free !!!!!!!!!)
INC\atr_hw.h    13.lug.99   Si agg. define DEVB_CODE 30072
                            Prot. hw_rom_crc(void)  da  Ulong  a   Ushort
LIB\dp_comm1.obj,lst        Abolita
LIB\dp_mbx.obj,lst          Abolita
LIB\ushell.obj,lst          Abolita
LIB\hw_devb.obj,lst         New entry
SRC\dp_comm1.c  17.dic.97   New entry
SRC\ushell.c    16.nov.98   New entry: e' la versione ridotta per consolle
SRC\strt_tcn.c  05.lug.99   3.9 Si agg. printf("... Type '?' for help\n");

----Modifiche interne:
TCN\JDP\MD\tm_m_lay.c   17.mag.99 Per elim. baco 167 si intr. huge in secondo
                                  parametro di tm_define_exe_poll(..)
TCN\ATR\atr_mna.c       17.mag.99 Si elim. 4 static ext_ncdb_*
TCN\JDP\NMA\mamesse.c   17.mag.99 Si agg. strcpy in new_messanger_name, per
                                  ritardare di un colpo ma_decodeString(..)
                                  Si mod. Uchar station in Uchar *station
TCN\JDP\WTB_42\pd_jdp.c 14.giu.99 <ATR:05> do not import frames with a FTF
                                           changed from the previous one
TCN\JDP\LC\lc_layer.c   17.giu.99 <ATR:02> Initialize to 0 the unused fields
                                           in the MCR register
TCN\JDP\MD\nm_layer.c   17.giu.99 <ATR:02> the QNX compiler prefers 960-like
                                           definitions
                                  <ATR:03> initialize the result value
TCN\JDP\MD\nm_stadi.c   17.giu.99 Cambia solo la data (era 09.gen.96)
TCN\ATR\conf_tcn.c      22.giu.99 short nc_mvb_ds_cfg(void): Si agg. in due
                                  punti:   pv_set_p++
STAR\PIL\pi_rtx.c       07.lug.99 macro xs_disable(): si agg. un _nop_()
                                  pil_timer_interrupt(): si usa    level_2_bank
                                                         invece di pil_timer_bank
TCN\JDP\MAP_10\map_ui.c 07.lug.99 <ATR:05> GROUP_LIST definition fixed
TCN\ATR\tcn_mon.c       12.lug.99 Non ho approfondito: modifiche corpose
TCN\JDP\MAP_10\atr_map.c 13.lug.99 Si dichiara short invece che void la routine
                                   bitset_dump(..), anche se non si agg. return
                                   nadi_cmd(..): si elim warnLev con argv=argv


- Le modifiche riguardanti le directory SW\ATR e SW\STAR\HW sono riportare
  nell'history di LBATR in cui sono state trasferite nel corso dello spac-
  chettamento del Monolite.

======================================================
tcns03a.rar             Napoli, Lun.21.feb.2000 09:00
    Rilascio fantasma di Mar.11.mag.1999, ancora con
l'ambiente uVision 1.xx:

----Modifiche esterne:
INC\pi_sys.h    21.apr.99   Si agg. gest. O_COM e O_QNX
INC\pi_rtx.h    26.apr.99   Si agg. define PI_TASK_DEF(name) void(name)
INC\atr_hw.h    04.mag.99   Si agg. define PC_CODE 8086 e gest. led_handler
INC\atr_sys.h   04.mag.99   Si agg. gest. _MRI, __QNX__
SRC\strt_tcn.c  10.mag.99   Si util. PI_TASK_DEF e si mod. Check(nc_node_cfg(..))
LIB\ushell.obj  11.mag.99   New entry fuori da star.lib: vers. rid. per consolle

----Modifiche interne:
ATR\am29f040.c        26.apr.99 Si elim. inc pi_sys.h e chiamate a pi_disable()
                                e pi_enable() intorno agli erase command
ATR\ushell.c          26.apr.99 Si agg. 0 al return intermedio di ushell_register(..)
TCN\ATR\tcn_mon.c     11.mag.99 Non ho approfondito: modifiche corpose
TCN\JDP\MD\lx_param.c 26.apr.96 Si agg. uso __QNX__
TCN\JDP\MD\am_call.c  26.apr.96 Si agg. uso __QNX__
TCN\JDP\WTB_42\mac.c  07.mag.99 Si mod. 5 defin. TimeOut per comaptibilita'
                                TCN, intr. calcolo con BITRATE
TCN\JDP\PDM\atr_pdm.c 10.mag.99 Si intr. Ushort pdm_node_mode_mask(Ushort mode)
TCN\JDP\PDM\atr_pdm.h 10.mag.99 Si intr. prot. Ushort pdm_node_mode_mask(Ushort mode)

======================================================
tcns03.rar              Napoli, Lun.21.feb.2000 08:25
    Ultimo rilascio di Mer.14.apr.1999 di Carnevale con
l'ambiente uVision 1.xx.
(In verita' esisterebbe una versione seguente del 11.mag.1999,
 ma non e' stata utilizzata su nessun progetto applicativo)

- Prima versione con nome unificato TCNS.
- Si eliminano tutti i file ushell.*, ormai trasferiti in \SWISV\LIB.
- I sorgenti sono raccolti in CarF9p14.rar

======================================================
swtcn03.rar             Napoli, Gio.24.feb.2000 18:35, Mungi

- Le seguenti directory sono eliminate dalla libreria e spostate:
\SW\ATR                 ==>             \SWISV\LBATR\SRC
\SW\STAR\HW             ==>             \SWISV\LBATR\A\HW\SRC
\SW\STAR\STARTUP        ==>             \SWISV\LIBMON
\SW\STAR\EMULAT         eliminata
\SW\STAR\FLASH          eliminata
\SW\STAR\PRJS\*.*       eliminate

\SW\TCN\DP_HOST         ==>             \SWISV\LIB\SRC
\SW\TCN\JDP.ORG\*.*     eliminate
\SW\TCN\WTB_41          eliminata

- Si cambia il nome della radice        da  \SW         a  \SWTCN

----Modifiche a \SWTCN\STAR:
- Si sposta     \SW\STAR\LIBS\PRJ\star.prj
  in            \SWTCN\STAR\star1.prj               per uVision 1
- Si creano:    \SWTCN\STAR\star2.uv2, star2.opt    per uVision 2
- Si sposta     \SW\STAR\LIBS\update.bat
  in            \SWTCN\STAR\update.bat              per uVision 1 e 2
    - 2.00    Sab.19.feb.2000     Napoli                  Mungi
        - Si unificano le due gestioni, inserendo un parametro 1 o 2
        - Si adegua a nuova struttura SWTCN
        - Crea direttamente TCNSx con sottodirectory per rarizzazione
        - Si cambia nome della libreria finale
          da   star(1 o 2).lib     a   tcns.lib
    - 2.01    Gio.24.feb.2000     Napoli                  Mungi
        - Sposta i file .obj e .lst nelle nuove directory:
          \SWTCN\STAR\OBJ7.1 e OBJ7.2
        - Mantiene per compatibilita' INC\lib_ver.h, ma ne cambia il contenuto:
          da:   #define LIB_VER "User: Carnevaleg     Date: 14/04/99   Time: 10.31"
          a:    #define LIB_VER " Comando Osoleto: usare [vers] "

- Si sposta     \SW\STAR\LIBS\star_his.txt
  in            \SWTCN\TCNSx\star_his.txt       x=1,2
- In \SWTCN\TCNx\INC    si riducono i file da 41 a 23, eliminando
        10                                      ==>     LBATR\SRC
         5                                      ==>     LBATR\A\HW\SRC
         2 (dp_mbx.h,dp_comm.h)                 ==>     LIB\SRC
         1 (am_util.h)              inutile
- In \SWTCN\TCNx\LIB    si riducono i file da 21 a 5, eliminando
         6 (hw_xxx.obj)                         ==>     LIB\A\XXX\OBJ7
         6 (startXXX.obj,.lst)                  ==>     LIB\A\XXX\OBJ7
         2 (ushell.obj,.lst)                    ==>     LBATR\OBJ7
         4 (dp_mbx.*,dp_comm2.*)                ==>     LIB\OBJ7
  I 5 file rimanenti sono:
        - star.lib           diventa         tcns.lib
        - rtxclk16,20        inalterati
        - La procedura \SWISV\COMANDI\fotcn.bat crea:
          LIB\tcnsvrs.obj,.lst  contenente versione,data,ora della libreria
          che l'applicativo dovra' gestire con il nuovo comando [vers] unificato
- In \SWTCN\TCNx\SRC    si riducono i file da  4 a  1, eliminando
         1 (brdsetup.c)                         ==>     \SWISV\LBATR\A\HW\SRC
         2 (dp_mbx.c, dp_comm2.c)               ==>     \SWISV\LIB\SRC

----Modifiche a \SWTCN\TCN:
TCN\JDP\LC\lc_layer.c   19.feb.00 Si mod. include lc_head.h
TCN\ATR\tcn_mon.c       19.feb.00 Si mod. include bai_ifce.h
TCN\JDP\BA\bao_scfg.c,.h    09.gen.96 eliminati
TCN\JDP\LC\lc_indt.c        09.gen.96 eliminato
TCN\JDP\MD\am_indt.c        09.gen.96 eliminato
TCN\JDP\MD\nm_manag.c,.h    09.gen.96 eliminati
TCN\JDP\PD\lpm_4mpb.c       09.gen.96 eliminato
TCN\JDP\NMA\ma_wtb.c        29.gen.99 eliminato

======================================================
tcn02d.rar
    Versione rilasciata Lun.22.mar.1998,
conservata nell'archivio di D'Andria

======================================================
tcn02c.rar
    Versione rilasciata Ven.08.gen.1999,
conservata nell'archivio di D'Andria

======================================================
tcn02b.rar
    Versione rilasciata Lun.30.nov.1998,
conservata nell'archivio di D'Andria

======================================================
tcn02a.rar
    Versione rilasciata Mar.08.set.1998,
conservata nell'archivio di D'Andria

======================================================
tcn02.rar               Napoli, Lun.21.feb.2000 08:15
    Gloriosa directory STAR utilizzata dai controlli azionamento
                (TLRV, E500P, GSTAF)
fino all'unificazione delle librerie, iniziata a gen.2000
Nome originale:      STAR8U24
Sorgenti:            CARN8U24
rilasciata Mer.24.giu.1998

======================================================
tcn01b.rar
    Versione rilasciata da Carnevale Mar.28.apr.1998, con la
correzione della gestione del bootload
    E' conservata nell'archivio di D'Andria

======================================================
tcn01a.rar              Napoli, Lun.21.feb.2000 08:05
    Versione rilasciata da Carnevale Lun.09.feb.1998, con
l'allungamento dei comandi a rischio:
                r       reset
                rb      reboot
                rr      resetrom
                bl      bootload
ma che contiene un errore nella gestione del bootload poi scoperto
il 15.apr.1998, a dimostrazione che i test di integrazione tra librerie
e applicativi che le usano non possono avvenire troppo lontano dal
rilascio.
Nome originale:      STAR8F09
Sorgenti:            --------

======================================================
tcn01.rar               Napoli, Lun.21.feb.2000 08:00
    Prima versione consolidata delle librerie STAR di Carnevale.
Nome originale:      STAR7D18
Sorgenti:            CARN7D18
rilasciata Gio.18.dic.1997

- Utilizzata per primi colloqui MVB tra CCU e TCU TLRV a Caserta
  durante MiS Tram Birmingham n.01

======================================================
tcn00a.rar
    Seconda versione per TLRV delle librerie STAR di Carnevale.
Nome originale:      STAR7O15
Sorgenti:            --------
rilasciata Mer.15.ott.1997

- Cerca di correggere un errore di acquisizione del bit CHK rilevato
  in laboratorio ISV\STP

======================================================
tcn00.rar
    Prima versione per TLRV delle librerie STAR di Carnevale.
Nome originale:      STAR7S24
Sorgenti:            --------
rilasciata Mer.24.set.1997

- Utilizzata per primi colloqui MVB tra CCU e TCU TLRV in laboratorio

