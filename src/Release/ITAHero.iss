; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "ITA Hero"
#define MyAppVersion "1.5"
#define MyAppPublisher "Victor Elias/Lucas Smaira"
#define MyAppURL "http://www.ita.br"
#define MyAppExeName "ITA Hero.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{B936F7E9-6182-4530-9E8A-6A30AA5C044B}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={src}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir=..\Release
OutputBaseFilename=ITA Hero
SetupIconFile=..\ITA Hero.ico
Compression=lzma
SolidCompression=yes

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Types]
Name: "fulls"; Description: "Full installation"
Name: "full"; Description: "Full, without source code"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "program"; Description: "Program Files"; Types: full fulls custom; Flags: fixed
Name: "source"; Description: "Source Files"; Types: fulls

[Dirs]
Name: "{app}\Chart"
Name: "{app}\Font"
Name: "{app}\FX"
Name: "{app}\Image"
Name: "{app}\Sound"
Name: "{app}\SAVEDATA"
Name: "{app}\src"; Components: source

[Files]
Source: "..\..\ITA Hero.exe"; DestDir: "{app}"; Flags: ignoreversion; Components: program
Source: "..\..\Chart\*"; DestDir: "{app}\Chart"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: program; Excludes: "Chart Maker,.svn"
Source: "..\..\Font\*"; DestDir: "{app}\Font"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: program; Excludes: ".svn"
Source: "..\..\FX\*"; DestDir: "{app}\FX"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: program; Excludes: ".svn"
Source: "..\..\Image\*"; DestDir: "{app}\Image"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: program; Excludes: "originals,\Sprite\*.rar,.svn"   
Source: "..\..\Sound\*"; DestDir: "{app}\Sound"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: program; Excludes: ".svn"
Source: "..\..\src\*"; DestDir: "{app}\src"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: program; Excludes: "*.o,ITA_Hero_private*,ITAHero.iss,Release,*.win,*.layout,Libraries dev packs,object,.svn"
Source: "..\..\*.dll"; DestDir: "{sys}"; Flags: sharedfile; Components: program; Excludes: "jpeg.dll,libtiff-3.dll,smpeg.dll"
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent