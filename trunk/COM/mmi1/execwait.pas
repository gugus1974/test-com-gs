unit execwait;

interface

uses Windows, Forms, ShellAPI; // Include SHELLAPI in this "uses" clause

function ExecAppWait(AppName, Params: string): Boolean ;

implementation

{ Execute an external application APPNAME.
  Pass optional parameters in PARAMS, separated by spaces.
  Wait for completion of the application
  Returns FALSE if application failed.                     }
function ExecAppWait(AppName, Params: string): Boolean;
var
    // Structure containing and receiving info about application to start
    ShellExInfo: TShellExecuteInfo;
begin
    FillChar(ShellExInfo, SizeOf(ShellExInfo), 0);

    with ShellExInfo do
    begin
        cbSize       := SizeOf(ShellExInfo);
        fMask        := see_Mask_NoCloseProcess;
        Wnd          := Application.Handle;
        lpFile       := PChar(AppName);
        lpParameters := PChar(Params);
        nShow        := sw_ShowNormal;
    end;

    Result := ShellExecuteEx(@ShellExInfo);

    if Result then
    begin
        while WaitForSingleObject(ShellExInfo.HProcess, 100) = WAIT_TIMEOUT do
        begin
            Application.ProcessMessages;
            if Application.Terminated then
            begin
                Break;
            end;
        end;
    end;
end;

end.
