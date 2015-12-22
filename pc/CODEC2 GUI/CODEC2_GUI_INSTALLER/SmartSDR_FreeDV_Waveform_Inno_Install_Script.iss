;
; INNO Setup Compiler Script
; for SmartSDR FreeDV Waveform and GUI
; Author: Mark Hanson, AA3RK
;

; The App Versions can be passed in and will match the AssemblyInfo for the application
#ifndef MyAppVersion
  #define MyAppVersion "1.0.5.0"
#endif

#ifndef MyAppVersionWithV
  #define MyAppVersionWithV "v1.0.5.0"
#endif

#define MyAppName "SmartSDR FreeDV Waveform"
#define MyAppPublisher "FlexRadio Systems"
#define MyAppURL "http://www.flexradio.com/"
#define MyAppExeName "SmartSDR_FreeDV_Waveform.exe"



[Setup]
AppId={{69CD74EA-0824-438C-A3DF-1759AEF3F7D2}
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
DefaultGroupName=FreeDV Waveform
DisableProgramGroupPage=yes
OutputBaseFilename=SmartSDR_FreeDV_Waveform_Installer_{#MyAppVersionWithV}
SetupIconFile=..\CODEC2 GUI\Images\FDVLogo.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
Source: "..\CODEC2 GUI\bin\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\Release\Flex.UiWpfFramework.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\Release\FlexLib.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\Release\Ionic.Zip.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\Release\Util.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\bin\Release\Vita.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\Images\FDVLogo.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\Licenses\gpl-3.0.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\CODEC2 GUI\Licenses\FlexLib License.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\..\DSP_API\Waveform\FreeDV.ssdr_waveform"; DestDir: "{userappdata}\FlexRadio Systems\Waveforms\"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\FDVLogo.ico"
Name: "{commondesktop}\{#MyAppName} {#MyAppVersionWithV}"; Filename: "{app}\{#MyAppExeName}";  Tasks: desktopicon; IconFilename: "{app}\FDVLogo.ico"

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
    FirewallRule.Description := 'UDP In-bound Firewall rule for SmartSDR FreeDV Waveform';
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
    FirewallRule.Description := 'TCP In-bound Firewall rule for SmartSDR FreeDV Waveform';
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
// runs after setup completes FreeDV Waveform

begin   
  if (CurStep=ssInstall) then
  begin   
      
    // Add Vista and greater Firewall rules
    SetFirewallExceptionVista('{#SetupSetting("AppVerName")}', ExpandConstant('{app}')+'\{#MyAppExeName}'); 
  
  end;
end;
