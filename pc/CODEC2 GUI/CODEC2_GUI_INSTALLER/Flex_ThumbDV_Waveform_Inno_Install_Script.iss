;

#define MyAppName "Flex DSTAR Waveform"
#define MyAppVersion "1.4.0.2"
#define MyAppPublisher "FlexRadio Systems, LLC"
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
DefaultDirName=FlexRadio Systems\ThumbDV DSTAR
DisableDirPage=yes
DefaultGroupName=Flex DSTAR Waveform
DisableProgramGroupPage=yes
OutputBaseFilename=FlexDSTARWaveform
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
Source: "..\..\..\DSP_API\ThumbDV_Release\ThumbDV.ssdr_waveform"; DestDir: {userappdata}\FlexRadio Systems\Waveforms; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

