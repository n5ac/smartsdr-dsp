;
; INNO Setup Compiler Script
; for Flex DSTAR Waveform and GUI
; Author: Mark Hanson, AA3RK
;

#define MyAppName "FlexRadio DSTAR Waveform"
#define MyAppVersion "0.0.4"
#define MyAppVersionWithV "v0.0.4"
#define MyAppPublisher "FlexRadio Systems"
#define MyAppURL "http://www.flexradio.com/"
#define MyAppExeName "ThumbDV_DSTAR_GUI.exe"

[Setup]
AppId={{2AA4AC17-A170-4825-9BE9-D9974CCC9444}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
VersionInfoVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppPublisher}\{#MyAppName} {#MyAppVersionWithV}
DisableDirPage=yes
DefaultGroupName=FlexRadio DSTAR Waveform
DisableProgramGroupPage=yes
OutputBaseFilename=FlexRadioDSTARWaveform_Installer
SetupIconFile=..\CODEC2 GUI\Images\dstar.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\CODEC2 GUI\bin\x86\Release\ThumbDV_DSTAR_GUI.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\ThumbDV_DSTAR_GUI.exe.config"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\Flex.UiWpfFramework.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\FlexLib.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\Ionic.Zip.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\Util.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\Vita.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\..\DSP_API\Waveform\ThumbDV.ssdr_waveform"; DestDir: "{userappdata}\FlexRadio Systems\Waveforms\"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

