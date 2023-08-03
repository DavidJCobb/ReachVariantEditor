; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
                  
#include "vc-version.iss"
#include "rvt-version.iss"

#define MyAppName "ReachVariantTool"
//#define MyAppVersion "{#rvt_version_major}.{#rvt_version_minor}.{#rvt_version_patch}.{#rvt_version_build}"
#define MyAppPublisher "DavidJCobb"
#define MyAppURL "https://www.nexusmods.com/halothemasterchiefcollection/mods/192"
#define MyAppExeName "ReachVariantTool.exe"

#define CppProjectPath SourcePath + "\..\ReachVariantTool\"
#define CppBuildResultPath SourcePath + "\..\x64\Release\ReachVariantTool\"

#define VCInstallMessage "Installing Microsoft Visual C++ Redistributable...."

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{E8991692-EB10-4EB2-965C-92DB5A468561}
AppName={#MyAppName}
AppVersion="{#rvt_version_major}.{#rvt_version_minor}.{#rvt_version_patch}.{#rvt_version_build}"
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
CreateAppDir=yes
DefaultDirName={autopf}\ReachVariantTool
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputDir={#SourcePath}\inno-output
OutputBaseFilename=ReachVariantToolSetup
SetupIconFile={#SourcePath}\ReachVariantTool.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#CppBuildResultPath}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs; Excludes: "\themes\*.qss" 
Source: "{#CppProjectPath}LICENSES\*"; DestDir: "{app}\LICENSES\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#CppProjectPath}help\*"; DestDir: "{app}\help\"; Flags: ignoreversion recursesubdirs createallsubdirs; Permissions: users-modify
Source: "{#CppProjectPath}themes\*"; DestDir: "{app}\themes\"; Flags: ignoreversion; Permissions: users-modify
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[UninstallDelete]
Type: files; Name: "{app}\ReachVariantTool.ini"

; Install VC redist
[Run]
Filename: "{app}\vc_redist.x64.exe"; Parameters: "/install /passive"; StatusMsg: "{#VCInstallMessage}"; Check: IsWin64 and not VCInstalled

[Code]
function VCInstalled: Boolean;
var
  major: Cardinal;
  minor: Cardinal;
  bld:   Cardinal;
  rbld:  Cardinal;
  key:   String;
begin
  Result := False;
  key    := 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64';
  if RegQueryDWordValue(HKEY_LOCAL_MACHINE, key, 'Major', major) then begin
    if RegQueryDWordValue(HKEY_LOCAL_MACHINE, key, 'Minor', minor) then begin
      if RegQueryDWordValue(HKEY_LOCAL_MACHINE, key, 'Bld', bld) then begin
        if RegQueryDWordValue(HKEY_LOCAL_MACHINE, key, 'RBld', rbld) then begin
            Log('Existing VC redist version: ' + IntToStr(major) + '.' + IntToStr(minor) + '.' + IntToStr(bld) + '.' + IntToStr(rbld));
            Result := (major >= {#vc_version_major}) and (minor >= {#vc_version_minor}) and (bld >= {#vc_version_bld}) and (rbld >= {#vc_version_rbld})
        end;
      end;
    end;
  end;
end;