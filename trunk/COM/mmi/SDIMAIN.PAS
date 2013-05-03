unit SDIMAIN;

interface

uses Windows, Forms, Controls, Dialogs,
     SysUtils,ExecWait,CPDrv,StrUtils,DateUtils, StdCtrls, Classes, ExtCtrls,
     ToolWin, ComCtrls;

type
    TSDIAppForm = class(TForm)
        Memo1: TMemo;
    Panel2: TPanel;
    Panel1: TPanel;
    txtOperator: TLabeledEdit;
    txtBoardID: TLabeledEdit;
    Panel3: TPanel;
    Panel4: TPanel;
    btnStart: TButton;
    btnExit: TButton;
    lblMessage: TLabel;
    btnOK: TButton;
    btnSI: TButton;
    btnNO: TButton;
    btnAnnulla: TButton;
    btnRiprova: TButton;
    ToolBar1: TPanel;
    btnStop: TButton;
    ConsoRichEdit: TRichEdit;
        procedure FileExit1Execute(Sender: TObject);
        procedure HelpAbout1Execute(Sender: TObject);
        procedure FormCreate(Sender: TObject);
        procedure btnStartClick(Sender: TObject);
        procedure FormDestroy(Sender: TObject);
        procedure btnExitClick(Sender: TObject);
        procedure txtBoardIDEnter(Sender: TObject);
        procedure txtBoardIDKeyPress(Sender: TObject; var Key: Char);
        procedure txtOperatorExit(Sender: TObject);
        procedure txtOperatorKeyPress(Sender: TObject; var Key: Char);
    procedure btnOKClick(Sender: TObject);
    procedure btnSIClick(Sender: TObject);
    procedure btnNOClick(Sender: TObject);
    procedure btnRiprovaClick(Sender: TObject);
    procedure btnAnnullaClick(Sender: TObject);
    procedure btnStopClick(Sender: TObject);
    procedure Button1Click(Sender: TObject);

    private

        procedure StartTest;
        function  TestCommand(cmd,pass,fail: string; timeout: integer): Boolean;
        procedure Wait(ms:Integer);
        function  ScanFile(FileName: TFileName; Str: String): Boolean;
        procedure DisplayTestStatus(TestIndex: Integer; TestStatus: Integer; Descr: String);
        function  CheckResultFile(FileName: TFileName): Boolean;
        function  CheckResultFileNew(FileName: TFileName; out ErrorStr: String ): Boolean;
        procedure ClearTestList;
        function  TestReset: Boolean;
        function  TestClock: Boolean;
        function  TestBDM: Boolean;
        function  TestRam: Boolean;
        function  TestLoad: Boolean;
        function  TestSerial: Boolean;
        function  TestFlash: Boolean;
        function  TestEEProm: Boolean;
        function  TestLed: Boolean;
        function  TestSW1: Boolean;
        function  TestTS: Boolean;
        function  TestMVBC: Boolean;
        function  TestIsol: Boolean;
        function  TestIO: Boolean;
        function  GetValue(cmd,varname: string; timeout: integer; ncar:integer): String;
        procedure LogTestResult(Result: String);
        function  MessagePanel(const Msg: string; DlgType: TMsgDlgType; Buttons: TMsgDlgButtons): Word;

    end;

var
    SDIAppForm:       TSDIAppForm;
    ComPortDriver:    TCommPortDriver;
    BDMPort:          String;
    COMPort:          String;
    testnames:        Array[1..14] of string;
    NamesMaxLen:      Integer;
    ResetTestIndex:   Integer;
    ClockTestIndex:   Integer;
    BDMTestIndex:     Integer;
    RAMTestIndex:     Integer;
    LOADTestIndex:    Integer;
    SERIALTestIndex:  Integer;
    FLASHTestIndex:   Integer;
    EEPROMTestIndex:  Integer;
    TSTestIndex:      Integer;
    MVBCTestIndex:    Integer;
    IsolTestIndex:    Integer;
    LEDTestIndex:     Integer;
    SW1TestIndex:     Integer;
    IOTestIndex:      Integer;
    ToolBarSelect:    Word;
    UserStopTest:     Boolean;

const
    Version = '1.00';
    PassedLogFileName = 'COMTEST.LOG';
    FailedLogFileName = 'COMTESTF.LOG';


implementation

uses about, consolle;

{$R *.dfm}

procedure TSDIAppForm.FileExit1Execute(Sender: TObject);
begin
  Close;
end;

procedure TSDIAppForm.HelpAbout1Execute(Sender: TObject);
begin
  AboutBox.ShowModal;
end;

procedure TSDIAppForm.Wait(ms: Integer);
var
    tStart: TDateTime;
    tStop:  TDateTime;
begin
    (* attende 2 secondi *)
    tStart := Now;
    tStop  := IncMillisecond(tStart,ms);

    while (Now < tStop) do
    begin
        Application.ProcessMessages;
    end;
end;

(* Aggiorna il display con l'esito di un test *)
procedure TSDIAppForm.DisplayTestStatus(TestIndex: Integer; TestStatus: Integer; Descr: String);
var
    temp: String;
begin
    temp := LeftStr(memo1.Lines[TestIndex],NamesMaxLen);
    if (length(temp) < NamesMaxLen) then
    begin
        temp := temp + StringOfChar('.',NamesMaxLen-length(temp));
    end;
    temp := temp + '...';

    case TestStatus of
        0: temp := temp + 'FAILED <-';
        1: temp := temp + 'PASSED';
        else
            temp := temp + Descr;
    end;
    memo1.Lines[TestIndex] := temp;

end;

(* Cerca una stringa in un file, Restituisce true se e' stata trovata *)
function TSDIAppForm.ScanFile(FileName: TFileName; Str: String): Boolean;
var
    RetVal: Boolean;
    SFile: TextFile;
    TempStr: String;

begin
    RetVal := false;
    if (fileexists(FileName)) then
    begin
        AssignFile(SFile,FileName);
        Reset(SFile);
        while (Not eof(SFile) and not RetVal) do
        begin
            Readln(SFile,TempStr);
            if (pos(Str,TempStr) > 0) then
            begin
                RetVal := true;
            end;
        end;
        CloseFile(SFile);
    end;
    ScanFile := RetVal;
end;

(* Controlla che nel file non siano presenti i messaggi di errore *)
function TSDIAppForm.CheckResultFile(FileName: TFileName): Boolean;
var
    RetVal: Boolean;
    i: Integer;

const
    errorstr: Array [1..5] of string =
    ('BERR',
     'error',
     'Aborted',
     'WARNING - No power on ICD cable',
     'Can not enter background mode');

begin
    RetVal := true;
    if (fileexists(FileName)) then
    begin
        for i := 1 to length(errorstr) do
        begin
            RetVal := RetVal and not ScanFile(FileName,errorstr[i]);
        end;
    end
    else
    begin
        RetVal := false;
    end;
    CheckResultFile := RetVal;
end;

(* Controlla che nel file non siano presenti i messaggi di errore, restituisce FALSE se ne trova *)
function TSDIAppForm.CheckResultFileNew(FileName: TFileName; out ErrorStr: String ): Boolean;
var
    RetVal: Boolean;
    RetScan: Boolean;
    i: Integer;
(*
const
    errorstr: Array [1..5] of string =
    ('BERR',
     'error',
     'Aborted',
     'WARNING - No power on ICD cable',
     'Can not enter background mode');
*)
begin
    ErrorStr :='';
    RetVal := true;
    if (fileexists(FileName)) then
    begin
        for i := 1 to length(errorstr) do
        begin
            RetScan := ScanFile(FileName,errorstr[i]);
            if (RetScan)
                 then ErrorStr := errorstr[i];
            RetVal := RetVal and not RetScan;

        end;
    end
    else
    begin
        RetVal := false;
    end;
    CheckResultFileNew := RetVal;
end;

function TSDIAppForm.TestCommand(cmd,pass,fail: string; timeout: integer): Boolean;
var
    resstr: string;
    s: string;
    RxData: array[0..200] of char;
    DataPtr: pchar;
    Passed: boolean;
    Failed: boolean;
    cntTimeOut : Integer;
   Datasize : Integer;
    p: pchar;

begin
    resstr := '';
    cntTimeOut := 0;
    ComPortDriver.InputTimeout := 1000;
    DataPtr := @RxData;
    ComPortDriver.FlushBuffers(true,true);
    Passed := false;
    Failed := false;
    ComPortDriver.SendString(cmd+#13);

    while (Not Passed and Not Failed) do
    begin
        ComPortDriver.ReadData(DataPtr,180);
        SetString(s,DataPtr,strlen(DataPtr));
        Datasize := strlen(DataPtr);

        resstr := resstr+s;

        // now for the Ascii reception
        While ConsoRichEdit.Lines.Count > 1000 Do ConsoRichEdit.Lines.Delete(0);//azzera la lista

        // Get current line
        If ConsoRichEdit.Lines.Count = 0 Then ConsoRichEdit.Lines.Add('');

        if ConsoRichEdit.Lines.Count <> 0 then
                s := ConsoRichEdit.Lines[ConsoRichEdit.Lines.Count-1]
        else
                s := '';
        // Parse incoming text
        p := DataPtr;

        while Datasize > 0 do
        begin
        case p^ of
        #10:; // LF
        #13: // CR - cursor to next line
                begin
                if ConsoRichEdit.Lines.Count <> 0 then
                        ConsoRichEdit.Lines[ConsoRichEdit.Lines.Count-1] := s
                else
                        ConsoRichEdit.Lines.Add( s );
                ConsoRichEdit.Lines.Add('');
                s := '';
                end;
        #8: // Backspace - delete last char
                delete( s, length(s), 1 );
                else // Any other char - add it to the current line
                s := s + p^;
                end;
        dec( DataSize );
        inc( p );
        end;
        // If current line isn't empty
        if (s<>'') then
        if ConsoRichEdit.Lines.Count <> 0 then
                // Update current line
                ConsoRichEdit.Lines[ConsoRichEdit.Lines.Count-1] := s
        else
                // New line - add it
                ConsoRichEdit.Lines.Add( s );
        ConsoRichEdit.Update;

        (* Cerca l'esito positivo *)
        Passed := (pos(pass,resstr) > 0);

        (* Cerca l'esito negativo *)
        Failed := ((pos(fail,resstr) > 0) or (cntTimeOut > TimeOut));

        Inc(cntTimeOut);
    end;
    ComPortDriver.FlushBuffers(true,true);

    TestCommand := Passed and Not Failed;
end;


(* Invia un comando ed attende una risposta restituendo la stringa *)
function TSDIAppForm.GetValue(cmd,varname: string; timeout: integer; ncar:integer): String;
var
    retval : string;
    resstr: string;
    s: string;
    RxData: array[0..200] of char;
    DataPtr: pchar;
    Passed: boolean;
    Failed: boolean;
    cntTimeOut : Integer;

begin
    retval := '';
    resstr := '';
    cntTimeOut := 0;
    ComPortDriver.InputTimeout := 1000;
    DataPtr := @RxData;
    ComPortDriver.FlushBuffers(true,true);
    Passed := false;
    Failed := false;
    ComPortDriver.SendString(cmd+#13);

    while (Not Passed and Not Failed) do
    begin
        ComPortDriver.ReadData(DataPtr,180);
        SetString(s,DataPtr,strlen(DataPtr));
        resstr := resstr+s;

        (* Cerca la stringa di risposta *)
        Passed := (pos(varname,resstr) > 0);

        Failed := (cntTimeOut > TimeOut);

        Inc(cntTimeOut);
    end;
    ComPortDriver.FlushBuffers(true,true);

    if Passed then
    begin
        retval := MidStr(resstr,pos(varname,resstr)+length(varname),ncar);
        if (pos(#13,retval) > 0) then
        begin
            retval := LeftStr(retval,pos(#13,retval)-1);
        end;
    end;
    GetValue := retval;
end;

function TSDIAppForm.MessagePanel(const Msg: string; DlgType: TMsgDlgType; Buttons: TMsgDlgButtons): Word;
begin
    lblMessage.Caption := Msg;

    btnOK.Visible := (mbOK in Buttons);
    btnNO.Visible := (mbNO in Buttons);
    btnSI.Visible := (mbYES in Buttons);
    btnAnnulla.Visible := (mbCancel in Buttons);
    btnRiprova.Visible := (mbRetry in Buttons);
    ToolBar1.Visible := True;

    if (btnOK.Visible) then
    begin
        btnOK.SetFocus;
    end;

    if (btnSI.Visible) then
    begin
        btnSI.SetFocus;
    end;

    ToolBar1.Refresh;
    panel2.Refresh;

    ToolBarSelect := 0;

    while (ToolBarSelect = 0) do
    begin
        Application.ProcessMessages;
    end;

    lblMessage.Caption := '';
    ToolBar1.Visible := False;
    ToolBar1.Refresh;
    panel2.Refresh;

    MessagePanel := ToolBarSelect;
end;


(**************************************************************************)
(* Routine per la gestione dei singoli test                               *)
(**************************************************************************)

(* verifica accensione led alla pressione del tasto reset *)
function TSDIAppForm.TestReset: Boolean;
var
    Passed: boolean;
    bt:     Word;

begin
    Passed := false;

    DisplayTestStatus(ResetTestIndex,-1,'IN ESECUZIONE');

    bt := MessagePanel('Premere il pulsante PS1 posto sul frontalino della scheda'#13#10+
                       'e verificare che risultino accesi solo i seguenti LED:'#13#10#13#10+
                       'LD1a, LD1b, LD4a, LD4b, LD5a, LD5b, LD6a e LD6b.'#13#10#13#10+
                       'Rilasciare il pulsante PS1 e verificare che si spenga'#13#10+
                       'solo il LED: LD1b.'#13#10#13#10+
                       'Verifica OK?',
                       mtConfirmation,[mbYES, mbNO]);

    Passed := (bt = mrYes);

    TestReset := Passed;
    if Passed then
    begin
        DisplayTestStatus(ResetTestIndex,1,'');
    end
    else
    begin
        DisplayTestStatus(ResetTestIndex,0,'');
    end;
end;

(* oscillazione clock *)
function TSDIAppForm.TestClock: Boolean;
var
    Passed: boolean;
    bt:     Word;

begin
    Passed := false;

    DisplayTestStatus(ClockTestIndex,-1,'IN ESECUZIONE');

    bt := MessagePanel('Verificare con l''oscilloscopio che sul pin 5 di IC3'#13#10+
                       'sia presente un segnale a frequenza 24MHz +/- 1%.'#13#10#13#10+
                       'Verifica OK?',
                       mtConfirmation,[mbYES, mbNO]);

    Passed := (bt = mrYes);

    TestClock := Passed;
    if Passed then
    begin
        DisplayTestStatus(ClockTestIndex,1,'');
    end
    else
    begin
        DisplayTestStatus(ClockTestIndex,0,'');
    end;
end;

function TSDIAppForm.TestBDM: Boolean;
var
        TestBDMError : String;
begin
    (* Controlla cle il file RESET.RES esista e non contenga errori *)
//    if (CheckResultFile('RESET.RES') or (BDMPort='NONE')) then
    if (CheckResultFileNew('RESET.RES', TestBDMError) or (BDMPort='NONE')) then
    begin
        DisplayTestStatus(BDMTestIndex,1,'');
        TestBDM := True;

    end
    else
    begin
        DisplayTestStatus(BDMTestIndex,0,'');
        TestBDM := False;
        Beep;
    end;
end;

function TSDIAppForm.TestRam: Boolean;
var
        testWE0: boolean;
        testWE1: boolean;
        testWE2: boolean;
        testWE3: boolean;
        bt:      Word;

begin
    (* Controlla che il file TESTRAM.RES esista, non contenga errori, e contenga l'esito positivo del test *)
    if (CheckResultFile('TRAM_WE0.RES') and ScanFile('TRAM_WE0.RES','00000000 00000000')
                                         and ScanFile('TRAM_WE0.RES','00000000 55000000')
                                         and ScanFile('TRAM_WE0.RES','00000000 AA000000'))  then
    begin
        testWE0 := True;
        lblMessage.Caption := 'test WE0 OK'+#13;
    end
    else
    begin
        testWE0 := False;
        lblMessage.Caption := 'ERRORE test WE0'+#13;
    end;

    if (CheckResultFile('TRAM_WE1.RES') and ScanFile('TRAM_WE1.RES','00000000 00000000')
                                         and ScanFile('TRAM_WE1.RES','00000000 00AA0000')
                                         and ScanFile('TRAM_WE1.RES','00000000 00550000'))  then
    begin
        testWE1 := True;
        lblMessage.Caption := 'test WE1 OK'+#13;
    end
    else
    begin
        testWE1 := False;
        lblMessage.Caption := 'ERRORE test WE1'+#13;
    end;

    if (CheckResultFile('TRAM_WE2.RES') and ScanFile('TRAM_WE2.RES','00000000 00000000')
                                         and ScanFile('TRAM_WE2.RES','00000000 0000AA00')
                                         and ScanFile('TRAM_WE2.RES','00000000 00005500'))  then
   begin
        testWE2 := True;
        lblMessage.Caption := 'test WE2 OK'+#13;
    end
    else
    begin
        testWE2 := False;
        lblMessage.Caption := 'ERRORE test WE2'+#13;
    end;


    if (CheckResultFile('TRAM_WE3.RES') and ScanFile('TRAM_WE3.RES','00000000 00000000')
                                         and ScanFile('TRAM_WE3.RES','00000000 000000AA')
                                         and ScanFile('TRAM_WE3.RES','00000000 00000055'))  then
   begin
        testWE2 := True;
        lblMessage.Caption := 'test WE3 OK'+#13;
    end
    else
    begin
        testWE2 := False;
        lblMessage.Caption := 'ERRORE test WE3'+#13;
    end;


    if ((CheckResultFile('TESTRAM.RES') and ScanFile('TESTRAM.RES','00000050 000000A5'))
        or (BDMPort='NONE')) then
    begin
        DisplayTestStatus(RAMTestIndex,1,'');
        if (testWE0 and testWE1 and testWE2 and testWE3) then
        TestRAM := True
        else
        TestRAM := True;
    end
    else
    begin  //test fallito
        DisplayTestStatus(RAMTestIndex,0,'');
        TestRAM := False;
        Beep;
    end;
end;

function TSDIAppForm.TestLoad: Boolean;
begin
    (* Controlla che il file LOADAPP.RES esista, non contenga errori *)
    if (CheckResultFile('LOADAPP.RES') or (BDMPort='NONE')) then
    begin
        DisplayTestStatus(LOADTestIndex,1,'');
        TestLOAD := True;
    end
    else
    begin
        DisplayTestStatus(LOADTestIndex,0,'');
        TestLOAD := False;
        Beep;
    end;
end;

function TSDIAppForm.TestSerial: Boolean;
var
    Passed: boolean;
    Retry:  boolean;
    bt:     word;
begin
    Retry  := true;

    while Retry do
    begin
        DisplayTestStatus(SERIALTestIndex,-1, 'IN ESECUZIONE');
        (*********************************************)
        (* Invia un comando errato ed attende il ??? *)
        (*********************************************)
        if (BDMPort<>'NONE') then
        begin
            Passed := TestCommand('wrongcommand','???','ERROR',2);
        end
        else
        begin
            Passed := True;
        end;

        TestSERIAL := Passed;
        if Passed then
        begin
            DisplayTestStatus(SERIALTestIndex,1,'');
            Retry := False;
        end
        else
        begin
            DisplayTestStatus(SERIALTestIndex,0,'');
            Beep;
            bt := MessagePanel('Ripetere il test ?',mtConfirmation,[mbYES, mbNO]);
            Retry := (bt = mrYES);
        end;
    end;
end;


function TSDIAppForm.TestFlash: Boolean;
var
    Passed:boolean;
    Retry:boolean;
    bt:     word;
begin
    Retry  := true;

    while Retry do
    begin
        (*******************************************)
        (* Invia il comando per resettare la flash *)
        (*******************************************)
//        DisplayTestStatus(FLASHTestIndex,-1, 'CANCELLAZIONE');

        if (BDMPort<>'NONE') then
        begin
            Passed := TestCommand('tstFLASH','FLASHERASEOK','ERROR',10);
//            Passed := TestCommand('resflash','FLASHERASEOK','ERROR',10);
        end
        else
        begin
            Passed := True;
        end;

        if (Passed) then
        begin
            (************************************)
            (* Invia il comando per verifica AA *)
            (************************************)
            DisplayTestStatus(FLASHTestIndex,-1, 'VERIFICA AAAA');

            if (BDMPort<>'NONE') then
            begin
                Passed := TestCommand('verflashaa','FLASHAAPASSED','ERROR',20);
            end
            else
            begin
                Passed := True;
            end;
        end;

        if (Passed) then
        begin
            (*******************************************)
            (* Invia il comando per resettare la flash *)
            (*******************************************)
            DisplayTestStatus(FLASHTestIndex,-1, 'CANCELLAZIONE');

            if (BDMPort<>'NONE') then
            begin
                Passed := TestCommand('resflash','FLASHERASEOK','ERROR',10);
            end
            else
            begin
                Passed := True;
            end;
        end;

        if (Passed) then
        begin
            (************************************)
            (* Invia il comando per verifica 55 *)
            (************************************)
            DisplayTestStatus(FLASHTestIndex,-1, 'VERIFICA 5555');

            if (BDMPort<>'NONE') then
            begin
                Passed := TestCommand('verflash55','FLASH55PASSED','ERROR',20);
            end
            else
            begin
                Passed := True;
            end;
        end;

        TestFlash := Passed;
        if Passed then
        begin
            DisplayTestStatus(FLASHTestIndex,1,'');
            Retry := False;
        end
        else
        begin
            DisplayTestStatus(FLASHTestIndex,0,'');
            Beep;
            bt := MessagePanel('Ripetere il test ?',mtConfirmation,[mbYES, mbNO]);
            Retry := (bt = mrYES);
        end;
    end;
end;

function TSDIAppForm.TestEEProm: Boolean;
var
    Passed:boolean;
    Retry:boolean;
    bt:     word;
begin
    Retry  := true;

    while Retry do
    begin
        (********************************************************)
        (* Invia il comando per riempire la EEPROM di dati noti *)
        (********************************************************)
        DisplayTestStatus(EEPROMTestIndex,-1, 'SCRITTURA');

        if (BDMPort<>'NONE') then
        begin
            Passed := TestCommand('filleeprom','EEPROMFILLOK','ERROR',25);
        end
        else
        begin
            Passed := True;
        end;

        if (Passed) then
        begin
            (*****************************************************)
            (* Invia il comando per la verifica dei dati scritti *)
            (*****************************************************)
            DisplayTestStatus(EEPROMTestIndex,-1, 'VERIFICA');

            if (BDMPort<>'NONE') then
            begin
                Passed := TestCommand('vereeprom','EEPROMVERIFYOK','ERROR',10);
            end
            else
            begin
                Passed := True;
            end;
        end;

        TestEEPROM := Passed;
        if Passed then
        begin
            DisplayTestStatus(EEPROMTestIndex,1,'');
            Retry := False;
        end
        else
        begin
            DisplayTestStatus(EEPROMTestIndex,0,'');
            Beep;
            bt := MessagePanel('Ripetere il test ?',mtConfirmation,[mbYES, mbNO]);
            Retry := (bt = mrYES);
        end;
    end;
end;

function TSDIAppForm.TestLed: Boolean;
var
    Passed: boolean;
    Retry:  boolean;
    bt:     Word;

begin
    Passed := false;
    Retry  := true;

    DisplayTestStatus(LEDTestIndex,-1,'IN ESECUZIONE');

    MessagePanel('Verificare che i LED LD1a,LD2b,LD3a e LD3b siano accesi'#13#10+
                 'e tutti gli altri siano spenti.'#13#10+
                 'Verificare che alla pressione del tasto SI si accendano'#13#10+
                 'uno alla volta in sequenza i led:'#13#10+
                 'LD4a, LD4b, LD5a, LD5b, LD6a e LD6b'#13#10#13#10+
                 'Pronti per osservare la sequenza ?',
               mtInformation,[mbYES]);

    while Retry do
    begin
        TestCommand('ledcycle','LEDOK','ERROR',10);

        bt := MessagePanel('La sequenza e'' corretta ?',mtConfirmation,[mbYES, mbNO, mbRETRY]);

        Retry  := (bt = mrRetry);
        Passed := (bt = mrYes);
    end;

    TestLed := Passed;
    if Passed then
    begin
        DisplayTestStatus(LEDTestIndex,1,'');
    end
    else
    begin
        DisplayTestStatus(LEDTestIndex,0,'');
    end;
end;

function TSDIAppForm.TestSW1: Boolean;
var
    Passed: boolean;
    swpos:  Integer;
    tempstr: string;
    Retry:  boolean;
    mr:  Word;

begin
    Passed := true;

    DisplayTestStatus(SW1TestIndex,-1,'IN ESECUZIONE');

    Retry := True;
    while(Retry) do
    begin
        (* Test SW1 in posizione 1 *)
        MessagePanel('Posizionare lo switch SW1 della scheda in prova'#13#10+
                     'in posizione ->1<-',mtInformation,[mbOK]);
        tempstr := GetValue('readsw','SW1 = 0x',2,2);

        swpos := 0;
        if (tempstr <> '') then
        begin
            swpos := strtointdef('$'+tempstr,-1);
        end;

        Retry := False;
        if (swpos <> $11) then
        begin
            mr := MessagePanel('Errore: Lo switch e'' stato rilevato in posizione:'+#13#10+
                               '->'+IntToStr(swpos and $0F)+'<-'+#13#10
                               ,mtInformation,[mbOK, mbRetry]);
            Retry := (mr = mrRetry);
        end;
    end;
    Passed := Passed and (swpos = $11);


    Retry := True;
    while(Passed And Retry) do
    begin
        (* Test SW1 in posizione 2 *)
        MessagePanel('Posizionare lo switch SW1 della scheda in prova'#13#10+
                     'in posizione ->2<-',mtInformation,[mbOK]);
        tempstr := GetValue('readsw','SW1 = 0x',2,2);
        if (tempstr <> '') then
        begin
            swpos := strtointdef('$'+tempstr,-1);
        end;

        Retry := False;
        if (swpos <> $12) then
        begin
            mr := MessagePanel('Errore: Lo switch e'' stato rilevato in posizione:'+#13#10+
                               '->'+IntToStr(swpos and $0F)+'<-'+#13#10
                               ,mtInformation,[mbOK, mbRetry]);
            Retry := (mr = mrRetry);
        end;
    end;
    Passed := Passed and (swpos = $12);


    Retry := True;
    while(Passed And Retry) do
    begin
        (* Test SW1 in posizione 4 *)
        MessagePanel('Posizionare lo switch SW1 della scheda in prova'#13#10+
                     'in posizione ->4<-',mtInformation,[mbOK]);
        tempstr := GetValue('readsw','SW1 = 0x',2,2);
        if (tempstr <> '') then
        begin
            swpos := strtointdef('$'+tempstr,-1);
        end;

        Retry := False;
        if (swpos <> $14) then
        begin
            mr := MessagePanel('Errore: Lo switch e'' stato rilevato in posizione:'+#13#10+
                               '->'+IntToStr(swpos and $0F)+'<-'+#13#10
                               ,mtInformation,[mbOK, mbRetry]);
            Retry := (mr = mrRetry);
        end;
    end;
    Passed := Passed and (swpos = $14);


    Retry := True;
    while(Passed And Retry) do
    begin
        (* Test SW1 in posizione 8 *)
        MessagePanel('Posizionare lo switch SW1 della scheda in prova'#13#10+
                     'in posizione ->8<-',mtInformation,[mbOK]);
        tempstr := GetValue('readsw','SW1 = 0x',2,2);
        if (tempstr <> '') then
        begin
            swpos := strtointdef('$'+tempstr,-1);
        end;

        Retry := False;
        if (swpos <> $18) then
        begin
            mr := MessagePanel('Errore: Lo switch e'' stato rilevato in posizione:'+#13#10+
                               '->'+IntToStr(swpos and $0F)+'<-'+#13#10
                               ,mtInformation,[mbOK, mbRetry]);
            Retry := (mr = mrRetry);
        end;
    end;
    Passed := Passed and (swpos = $18);

    TestSW1 := Passed;
    if Passed then
    begin
        DisplayTestStatus(SW1TestIndex,1,'');
    end
    else
    begin
        DisplayTestStatus(SW1TestIndex,0,'');
    end;
end;

function TSDIAppForm.TestTS: Boolean;
var
    Passed:boolean;
    Retry:  boolean;
    bt:     Word;
begin
    Retry  := true;

    while Retry do
    begin
        (*****************************************)
        (* Invia il comando per verificare la TS *)
        (*****************************************)
        DisplayTestStatus(TSTestIndex,-1, 'IN ESECUZIONE');

        if (BDMPort<>'NONE') then
        begin
            Passed := TestCommand('verts','TSPASSED','ERROR',10);
        end
        else
        begin
            Passed := True;
        end;

        TestTS := Passed;
        if Passed then
        begin
            DisplayTestStatus(TSTestIndex,1,'');
            Retry := False;
        end
        else
        begin
            DisplayTestStatus(TSTestIndex,0,'');
            Beep;
            bt := MessagePanel('Ripetere il test ?',mtConfirmation,[mbYES, mbNO]);
            Retry := (bt = mrYES);
        end;
    end;
end;

function TSDIAppForm.TestMVBC: Boolean;
var
    Passed: boolean;
    bt:     Word;

begin
    Passed := false;

    DisplayTestStatus(MVBCTestIndex,-1,'IN ESECUZIONE');
    Beep;

    bt := MessagePanel('Collegare l''oscilloscopio con sonde in differenziale'#13#10+
                       'tra i test point TP1 e TP2 del box di collaudo.'#13#10+
                       'Posizionare il commutatore S1 del box di collaudo'#13#10+
                       'in posizione 1 e verificare sull''oscilloscopio la presenza'#13#10+
                       'di un segnale con bit rate di 1 usec ed ampiezza compresa'#13#10+
                       'tra 2 e 12 Volts.'#13#10#13#10+
                       'Ripetere la verifica con il commutatore in posizione 2.'#13#10#13#10+
                       'Verifica OK?',
                       mtConfirmation,[mbYES, mbNO]);

    Passed := (bt = mrYes);

    TestMVBC := Passed;
    if Passed then
    begin
        DisplayTestStatus(MVBCTestIndex,1,'');
    end
    else
    begin
        DisplayTestStatus(MVBCTestIndex,0,'');
    end;
end;

function TSDIAppForm.TestIsol: Boolean;
var
    Passed: boolean;
    Retry:  boolean;
    bt:     Word;

begin
    Passed := false;
    Retry  := true;

    DisplayTestStatus(IsolTestIndex,-1,'IN ESECUZIONE');

    bt := MessagePanel('Collegare l''oscilloscopio con sonde in differenziale'#13#10+
                       'tra i test point TP1 e TP2 del box di collaudo.'#13#10+
                       'Posizionare il commutatore S1 del box di collaudo'#13#10+
                       'in posizione 1 e verificare sull''oscilloscopio'#13#10+
                       'che il segnale si interrompa alla pressione del tasto OK.'#13#10#13#10+
                       'Pronti?',
                       mtConfirmation,[mbOK]);

    while Retry do
    begin
        TestCommand('sendmf 1','','',1);

        bt := MessagePanel('Verificata l''interruzione del segnale ?',mtConfirmation,[mbYES, mbNO, mbRETRY]);

        Retry  := (bt = mrRetry);
        Passed := (bt = mrYes);
    end;

    if (Passed) then
    begin
        Retry  := true;
        bt := MessagePanel('Collegare l''oscilloscopio con sonde in differenziale'#13#10+
                           'tra i test point TP1 e TP2 del box di collaudo.'#13#10+
                           'Posizionare il commutatore S1 del box di collaudo'#13#10+
                           'in posizione ->2<- e verificare sull''oscilloscopio'#13#10+
                           'che il segnale si interrompa alla pressione del tasto OK.'#13#10#13#10+
                           'Pronti?',
                           mtConfirmation,[mbOK]);

        while Retry do
        begin
            TestCommand('sendmf 1','','',1);

            bt := MessagePanel('Verificata l''interruzione del segnale ?',mtConfirmation,[mbYES, mbNO, mbRETRY]);

            Retry  := (bt = mrRetry);
            Passed := (bt = mrYes);
        end;
    end;


    TestIsol := Passed;
    if Passed then
    begin
        DisplayTestStatus(IsolTestIndex,1,'');
    end
    else
    begin
        DisplayTestStatus(IsolTestIndex,0,'');
    end;
end;

function TSDIAppForm.TestIO: Boolean;
var
    Passed:boolean;
    IOFailed: String;
    Retry: boolean;
    bt: word;
begin
    (*************************************************************)
    (* Invia il comando per verificare gli Input/Output digitali *)
    (*************************************************************)
    Retry := true;

    while Retry do
    begin
        DisplayTestStatus(IOTestIndex,-1, 'IN ESECUZIONE');
        if (BDMPort<>'NONE') then
        begin
            Passed := TestCommand('testio','TESTIOOK','TESTIOFAIL',10);

            //--------------------------------------
            if (Not Passed) then
            begin
                IOFailed := GetValue('testio','TESTIOFAIL',10,200);
            end;
            //--------------------------------------

        end
        else
        begin
            Passed := True;
        end;

        TestIO := Passed;
        if Passed then
        begin
            DisplayTestStatus(IOTestIndex,1,'');
//            bt := MessagePanel('Ripetere il test ?',mtConfirmation,[mbYES, mbNO]);
//            Retry  := (bt = mrYes);
            Retry  := False;
        end
        else
        begin
            DisplayTestStatus(IOTestIndex,0,'');
            Beep;
            bt := MessagePanel('Riscontrati errori sui segnali di I/O alle'#13#10+
                               'seguenti righe della tabella 2 del doc. 238EE60368B:'#13#10#13#10+
                               IOFailed,mtConfirmation,[mbOK, mbRetry]);
            Retry  := (bt = mrRetry);
        end;

    end;
end;

procedure TSDIAppForm.ClearTestList;
var
    i: integer;
begin
    memo1.Clear;
    memo1.Lines.Add('LISTA DEI TEST DA ESEGUIRE');
    for i:=1 to Length(testnames) do
    begin
        memo1.Lines.Add(format('%2d) %s',[i,testnames[i]]));
    end;
end;

(***************************************************************)
(* Programma principale                                        *)
(***************************************************************)
procedure TSDIAppForm.FormCreate(Sender: TObject);
var
    i: integer;

begin
    i := 1;

    testnames[i] := 'Circuito di Reset';
    ResetTestIndex := i;
    inc(i);
    testnames[i] := 'Circuito di Clock';
    ClockTestIndex := i;
    inc(i);
    testnames[i] := 'Interfaccia BDM';
    BDMTestIndex := i;
    inc(i);
    testnames[i] := 'Memoria RAM';
    RAMTestIndex := i;
    inc(i);
    testnames[i] := 'Caricamento applicativo';
    LOADTestIndex := i;
    inc(i);
    testnames[i] := 'Interfaccia Seriale';
    SERIALTestIndex := i;
    inc(i);
    testnames[i] := 'Memoria Flash';
    FLASHTestIndex := i;
    inc(i);
    testnames[i] := 'Input/Output digitali';
    IOTestIndex := i;
    inc(i);
    testnames[i] := 'Led';
    LEDTestIndex := i;
    inc(i);
    testnames[i] := 'Memoria EEProm';
    EEPROMTestIndex := i;
    inc(i);
    testnames[i] := 'Memoria MVB (TS)';
    TSTestIndex := i;
    inc(i);
    testnames[i] := 'Commutatore SW1';
    SW1TestIndex := i;
    inc(i);
    testnames[i] := 'TX/RX su bus MVB';
    MVBCTestIndex := i;
    inc(i);
    testnames[i] := 'Isolamento bus MVB';
    IsolTestIndex := i;
    inc(i);

    NamesMaxLen := 0;
    for i:= 1 to length(testnames) do
    begin
        if (length(testnames[i]) > NamesMaxLen) then
        begin
            NamesMaxLen := length(testnames[i]);
        end;
    end;
    NamesMaxLen := NamesMaxLen+4;


    ComPortDriver := TCommPortDriver.Create;

    (* Preleva i parametri dalla linea di comando: *)
    (* [bdmport [comport]] *)
    if (ParamCount > 0) then
    begin
        BDMPort := ParamStr(1);

        if (ParamCount > 1) then
        begin
            COMPort := ParamStr(2);
        end
        else
        begin
            COMPort := 'com1';
        end;
    end
    else
    begin
        BDMPort := 'lpt1';
        COMPort := 'com1';
    end;

    Caption := 'COM Board Tester v.'+Version;
    Caption := Caption+'    (BDMPort:'+UpperCase(BDMPort)+', COMPort:'+UpperCase(COMPort)+')';
 
    ClearTestList; //procedura che presetta il TMEMO1

    ComPortDriver.BaudRateValue := 38400;
    ComPortDriver.PortName := COMPort;


    if (Not ComPortDriver.Connect) then
    begin
        MessageDlg('Impossibile aprire la porta seriale: '+COMPort,mtError,[mbOK],0);
        Application.Terminate;
        ConsolleForm.StatusBar1.Panels[1].Text := COMPort +' 38400 8N1 '+ 'CLOSED';
    end
    else
    begin
//        ConsolleForm.StatusBar1.Panels[0].Text :=  ComPortDriver.PortName;
//        ConsolleForm.StatusBar1.Panels[1].Text :=  'OPEN';
//        ConsolleForm.StatusBar1.Panels[2].Text := IntToStr(ComPortDriver.BaudRateValue);
    end;

    (* Lancia icd32 per sbloccare il reset della scheda COM *)
//    if (BDMPort <> 'NONE') then
    begin
        copyfile('startup.1','startup.icd',False);
        ExecAppWait('BDMEXIT.bat','');
        copyfile('startup.cpy','startup.icd',False);
    end;
end;

procedure TSDIAppForm.btnStartClick(Sender: TObject);
begin
    UserStopTest := False;
    if (txtOperator.Text = '') then
    begin
        beep;
        // MessageDlg('Inserire il nome dell''operatore per proseguire',
        //           mtError,[mbOK],0);
        txtOperator.SetFocus;
        txtOperator.Text := 'TESTER1'
    end
    else
    begin
        if (txtBoardID.Text = '') then
        begin
            beep;
//            MessageDlg('Inserire un Identificativo scheda per proseguire',
//                       mtError,[mbOK],0);
            txtBoardID.SetFocus;
            txtBoardID.Text := 'COM-01';
        end
        else
        begin
            StartTest;
            txtBoardID.SetFocus;
        end;
    end;
end;

procedure TSDIAppForm.StartTest;
var
    BoardOK:    Boolean;
    StopTest:   Boolean;
begin
    btnStart.Enabled := False;
    btnExit.Enabled := False;
    txtOperator.TabStop := False;
    txtBoardID.TabStop := False;

    BDMPort := 'lpt1';

    (* Cancella i risultati di un eventuale test precedente *)
    BoardOK  := true;
    StopTest := false;
    ClearTestList;
    if (fileexists('reset.res')) then
    begin
        deletefile('reset.res');
    end;
    if (fileexists('testram.res')) then
    begin
        deletefile('testram.res');
    end;
    if (fileexists('loadapp.res')) then
    begin
        deletefile('loadapp.res');
    end;

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestReset) then
        begin
            StopTest := true;
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(ResetTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestClock) then
        begin
            StopTest := true;
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(ClockTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        DisplayTestStatus(BDMTestIndex,-1, 'IN ESECUZIONE');

        (* lancia ICD32 *)
        if (BDMPort <> 'NONE') then
        begin
//            ExecAppWait('icd32.exe','quiet '+BDMPort);
            ExecAppWait('LOAD_COM_USB.BAT','');
        end;

        (* attende 2 secondi *)
        Wait(2000);

        if (Not TestBDM) then
        begin
            StopTest := true;
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(BDMTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestRam) then
        begin
            StopTest := true;
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(RAMTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestLoad) then
        begin
            StopTest := true;
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(LOADTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestSerial) then
        begin
            StopTest := true;
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(SERIALTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestFlash) then
        begin
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(FLASHTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestEEProm) then
        begin
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(EEPROMTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestTS) then
        begin
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(TSTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestIO) then
        begin
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(IOTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestMVBC) then
        begin
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(MVBCTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestIsol) then
        begin
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(IsolTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestLED) then
        begin
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(LEDTestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (Not StopTest and Not UserStopTest) then
    begin
        if (Not TestSW1) then
        begin
            BoardOK := false;
        end;
    end
    else
    begin
        DisplayTestStatus(SW1TestIndex,-1, 'NON ESEGUITO');
    end;

    Wait(10);

    if (UserStopTest) then
    begin
        LogTestResult('STOP');
    end
    else
    begin
        if BoardOK then
        begin
            LogTestResult('PASSED');
        end
        else
        begin
            LogTestResult('FAILED');
        end;
    end;

    btnStart.Enabled := True;
    btnExit.Enabled := True;
    txtOperator.TabStop := True;
    txtBoardID.TabStop := True;
end;

procedure TSDIAppForm.LogTestResult(Result: String);
var
    LogFile: TextFile;
    LogLine: String;
    LogFileName: String;

begin
    (* Visualizza e memorizza l'esito complessivo del test *)

    LogFileName := '';
    if (Result = 'PASSED') then
    begin
        LogFileName := PassedLogFileName;
    end;

    if (Result = 'FAILED') then
    begin
        LogFileName := FailedLogFileName;
    end;


    (* Apre il Log File ed appende se gia' presente *)
    if (LogFileName <> '') then
    begin
        AssignFile(LogFile,LogFileName);
        if FileExists(LogFileName) then
        begin
            Append(LogFile);
        end
        else
        begin
            Rewrite(LogFile);
        end;

        LogLine := format('"%s","%s","%s","%s","%s"',
                          [DateToStr(Now),
                           TimeToStr(Now),
                           txtOperator.Text,
                           txtBoardID.Text,
                           Result]);

        Writeln(LogFile,LogLine);
        CloseFile(LogFile);
    end;

    if (Result = 'PASSED') then
    begin
        memo1.Lines.Add('');
        memo1.Lines.Add('    - SCHEDA COLLAUDATA CON SUCCESSO -');
    end;

    if (Result = 'FAILED') then
    begin
        memo1.Lines.Add('');
        memo1.Lines.Add('    ********************************');
        memo1.Lines.Add('    **** SCHEDA NON FUNZIONANTE ****');
        memo1.Lines.Add('    ********************************');
    end;

    if (Result = 'STOP') then
    begin
        memo1.Lines.Add('');
        memo1.Lines.Add('  ********************************************');
        memo1.Lines.Add('  **** COLLAUDO INTERROTTO DALL''OPERATORE ****');
        memo1.Lines.Add('  ********************************************');
    end;

end;


procedure TSDIAppForm.FormDestroy(Sender: TObject);
begin
    ComPortDriver.Free;
end;

procedure TSDIAppForm.btnExitClick(Sender: TObject);
begin
    Close;
end;

procedure TSDIAppForm.txtBoardIDEnter(Sender: TObject);
begin
    (* Controlla che l'utente abbia immesso un Nome Operatore *)
    if (txtOperator.Text = '') then
    begin
        beep;
        MessageDlg('Inserire il nome dell''operatore per proseguire',
                   mtError,[mbOK],0);
        txtOperator.SetFocus;
    end
end;
procedure TSDIAppForm.txtBoardIDKeyPress(Sender: TObject; var Key: Char);
begin
    if (Key = #13) then
    begin
        txtBoardID.Text := trim(txtBoardID.Text);
        if (txtBoardID.Text = '') then
        begin
            beep;
            MessageDlg('Inserire un Identificativo scheda per proseguire',
                       mtError,[mbOK],0);
            txtBoardID.SetFocus;
        end
        else
        begin
            StartTest;
            txtBoardID.SetFocus;
            txtBoardID.SelectAll;
        end;
    end;
end;

procedure TSDIAppForm.txtOperatorExit(Sender: TObject);
begin
    txtOperator.Text := trim(txtOperator.Text);
end;

procedure TSDIAppForm.txtOperatorKeyPress(Sender: TObject; var Key: Char);
begin
    if (Key = #13) then
    begin
        txtBoardID.SetFocus;
    end;
end;

procedure TSDIAppForm.btnOKClick(Sender: TObject);
begin
    ToolBarSelect := mrOK;
end;

procedure TSDIAppForm.btnSIClick(Sender: TObject);
begin
    ToolBarSelect := mrYES;
end;

procedure TSDIAppForm.btnNOClick(Sender: TObject);
begin
    ToolBarSelect := mrNO;
end;

procedure TSDIAppForm.btnRiprovaClick(Sender: TObject);
begin
    ToolBarSelect := mrRETRY;
end;

procedure TSDIAppForm.btnAnnullaClick(Sender: TObject);
begin
    ToolBarSelect := mrCANCEL;
end;

procedure TSDIAppForm.btnStopClick(Sender: TObject);
begin
    UserStopTest := True;
end;

procedure TSDIAppForm.Button1Click(Sender: TObject);
begin
//  ConsolleForm.Show;
//  ConsolleForm.StatusBar1.Panels[0].Text :=  COMPort;

  end;

end.


