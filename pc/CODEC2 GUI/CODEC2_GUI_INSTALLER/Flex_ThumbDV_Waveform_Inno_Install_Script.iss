;
; INNO Setup Compiler Script
; for Flex DSTAR Waveform and GUI
; Author: Mark Hanson, AA3RK
;

; The App Versions can be passed in and will match the AssemblyInfo for the application
#ifndef MyAppVersion
  #define MyAppVersion "1.2.0.0"
#endif

#ifndef MyAppVersionWithV
  #define MyAppVersionWithV "v1.2.0.0"
#endif

#define MyAppName "SmartSDR D-STAR Waveform"
#define MyAppPublisher "FlexRadio Systems"
#define MyAppURL "http://www.flexradio.com/"
#define MyAppExeName "SmartSDR_D-STAR_Waveform.exe"

[Setup]
AppId={{6A900659-244F-467B-94D6-F0D0B1F72B81}
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
DefaultGroupName=DSTAR Waveform
DisableProgramGroupPage=yes
OutputBaseFilename=SmartSDR_D-STAR_Waveform_Installer_{#MyAppVersionWithV}
SetupIconFile=..\CODEC2 GUI\Images\dstar.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
Source: "..\CODEC2 GUI\bin\x86\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\{#MyAppExeName}.config"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\Flex.UiWpfFramework.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\FlexLib.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\Ionic.Zip.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\Util.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\x86\Release\Vita.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\Licenses\gpl-3.0.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\Licenses\FlexLib License.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\..\DSP_API\Waveform\ThumbDV.ssdr_waveform"; DestDir: "{userappdata}\FlexRadio Systems\Waveforms\"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName} {#MyAppVersionWithV}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]

const
// constants for Firewall access exception
  NET_FW_ACTION_ALLOW = 1;
  NET_FW_IP_PROTOCOL_TCP = 6;
  NET_FW_IP_PROTOCOL_UDP = 17;
  
  NET_FW_SCOPE_ALL = 0;
  NET_FW_IP_VERSION_ANY = 2;
  NET_FW_PROFILE_DOMAIN = 0;
	NET_FW_PROFILE_STANDARD = 1;


//===========================================================================
procedure SetFirewallExceptionVista(AppName,FileName:string);
//===========================================================================
//create SmartSDR wirewall in-bound exception for Vista and greater

var
  FirewallRule: Variant;
  FirewallPolicy: Variant;

begin
    try
    FirewallRule := CreateOleObject('HNetCfg.FWRule');
    FirewallRule.Name := AppName;
    FirewallRule.Description := 'UDP In-bound Firewall rule for SmartSDR DSTAR Waveform';
    FirewallRule.ApplicationName := FileName;
    FirewallRule.Protocol := NET_FW_IP_PROTOCOL_UDP;
    FirewallRule.EdgeTraversal := True;
    FirewallRule.Action := NET_FW_ACTION_ALLOW;
    FirewallRule.Enabled := True;
    // FirewallRule.InterfaceTypes := 'All';
    FirewallPolicy := CreateOleObject('HNetCfg.FwPolicy2');
    FirewallPolicy.Rules.Add(FirewallRule);  
  except
  end;
  
  try
    FirewallRule := CreateOleObject('HNetCfg.FWRule');
    FirewallRule.Name := AppName;
    FirewallRule.Description := 'TCP In-bound Firewall rule for SmartSDR DSTAR Waveform';
    FirewallRule.ApplicationName := FileName;
    FirewallRule.Protocol := NET_FW_IP_PROTOCOL_TCP;
    FirewallRule.EdgeTraversal := True;
    FirewallRule.Action := NET_FW_ACTION_ALLOW;
    FirewallRule.Enabled := True;
    // FirewallRule.InterfaceTypes := 'All';
    FirewallPolicy := CreateOleObject('HNetCfg.FwPolicy2');
    FirewallPolicy.Rules.Add(FirewallRule); 
  except
  end;  
end;


//===========================================================================
procedure CurStepChanged(CurStep: TSetupStep);
//===========================================================================
// runs after setup completes DSTAR Waveform

begin   
  if (CurStep=ssInstall) then
  begin   
      
    // Add Vista and greater Firewall rules
    SetFirewallExceptionVista('{#SetupSetting("AppVerName")}', ExpandConstant('{app}')+'\{#MyAppExeName}'); 
  
  end;
end;


