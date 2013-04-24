program Sdiapp;

uses
  Forms,
  SDIMAIN in 'SDIMAIN.PAS' {SDIAppForm},
  About in 'ABOUT.PAS' {AboutBox},
  execwait in 'execwait.pas',
  CPDrv in 'CPDrv.pas',
  consolle in 'consolle.pas' {ConsolleForm};

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TSDIAppForm, SDIAppForm);
  Application.CreateForm(TAboutBox, AboutBox);
  Application.CreateForm(TConsolleForm, ConsolleForm);
  Application.Run;
end.
 
