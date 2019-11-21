object FormAuth: TFormAuth
  Left = 317
  Top = 165
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1057#1077#1090#1077#1074#1086#1081' '#1082#1083#1102#1095
  ClientHeight = 329
  ClientWidth = 549
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object BevelBorder: TBevel
    Left = 4
    Top = 4
    Width = 541
    Height = 289
    Style = bsRaised
  end
  object Bevel12: TBevel
    Left = 12
    Top = 228
    Width = 441
    Height = 57
    Shape = bsFrame
  end
  object LabelTime: TLabel
    Left = 18
    Top = 234
    Width = 427
    Height = 43
    AutoSize = False
    Caption = 
      #1057#1077#1090#1077#1074#1086#1081' '#1082#1083#1102#1095' '#1080#1089#1087#1086#1083#1100#1079#1091#1077#1090#1089#1103' '#1076#1083#1103' '#1074#1079#1072#1080#1084#1085#1086#1081' '#1072#1091#1090#1077#1085#1090#1080#1092#1080#1082#1072#1094#1080#1080' '#1084#1086#1076#1091#1083#1103' '#1091#1087#1088 +
      #1072#1074#1083#1077#1085#1080#1103' TLK '#1080' '#1082#1083#1080#1077#1085#1090#1072'. '#1057#1086#1079#1076#1072#1081#1090#1077' '#1085#1086#1074#1099#1081' '#1082#1083#1102#1095', '#1089#1086#1093#1088#1072#1085#1080#1090#1077' '#1074' '#1092#1072#1081#1083' '#1080' '#1087 +
      #1088#1080#1084#1077#1085#1080#1090#1077' '#1085#1072' '#1082#1072#1078#1076#1086#1084' '#1082#1086#1084#1087#1100#1102#1090#1077#1088#1077'.'
    WordWrap = True
  end
  object ButtonOK: TButton
    Left = 268
    Top = 300
    Width = 85
    Height = 25
    Caption = #1055#1088#1080#1085#1103#1090#1100
    ModalResult = 1
    TabOrder = 0
    OnClick = ButtonOKClick
  end
  object ButtonCancel: TButton
    Left = 356
    Top = 300
    Width = 85
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 1
  end
  object ButtonHelp: TButton
    Left = 452
    Top = 300
    Width = 85
    Height = 25
    Caption = #1057#1087#1088#1072#1074#1082#1072
    Enabled = False
    TabOrder = 2
  end
  object ButtonNew: TButton
    Left = 462
    Top = 234
    Width = 73
    Height = 22
    Caption = #1053#1086#1074#1099#1081
    TabOrder = 3
    OnClick = ButtonNewClick
  end
  object ButtonSave: TButton
    Left = 462
    Top = 258
    Width = 73
    Height = 22
    Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100'...'
    TabOrder = 4
    OnClick = ButtonSaveClick
  end
  object MemoHEX: TMemo
    Left = 8
    Top = 8
    Width = 533
    Height = 213
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'System'
    Font.Pitch = fpFixed
    Font.Style = []
    ParentFont = False
    PopupMenu = PopupMenuHEX
    TabOrder = 5
    OnExit = MemoHEXExit
    OnKeyDown = MemoHEXKeyDown
    OnKeyPress = MemoHEXKeyPress
  end
  object SaveDialog: TSaveDialog
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Left = 20
    Top = 16
  end
  object PopupMenuHEX: TPopupMenu
    Left = 48
    Top = 16
  end
end
