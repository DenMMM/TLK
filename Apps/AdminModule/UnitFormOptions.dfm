object FormOptions: TFormOptions
  Left = 305
  Top = 240
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1054#1073#1097#1080#1077' '#1085#1072#1089#1090#1088#1086#1081#1082#1080
  ClientHeight = 278
  ClientWidth = 461
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object BevelBorder: TBevel
    Left = 4
    Top = 4
    Width = 453
    Height = 237
    Style = bsRaised
  end
  object Label1: TLabel
    Left = 65
    Top = 52
    Width = 270
    Height = 13
    Alignment = taRightJustify
    Caption = #1042#1088#1077#1084#1103' '#1080#1089#1087#1086#1083#1100#1079#1086#1074#1072#1085#1080#1103' '#1076#1080#1072#1083#1086#1075#1086#1074' '#1088#1072#1089#1095#1077#1090#1072' '#1094#1077#1085' ('#1084#1080#1085#1091#1090'):'
  end
  object Label2: TLabel
    Left = 221
    Top = 80
    Width = 114
    Height = 13
    Alignment = taRightJustify
    Caption = #1058#1086#1095#1085#1086#1089#1090#1100' '#1088#1072#1089#1095#1077#1090#1072' '#1094#1077#1085':'
  end
  object Label3: TLabel
    Left = 51
    Top = 116
    Width = 284
    Height = 13
    Alignment = taRightJustify
    Caption = #1042#1088#1077#1084#1103' '#1076#1083#1103' '#1087#1086#1087#1072#1076#1072#1085#1080#1103' '#1087#1086#1076' '#1092#1080#1083#1100#1090#1088' "'#1057#1074#1086#1073#1086#1076#1085#1099#1077'" ('#1084#1080#1085#1091#1090'):'
  end
  object Bevel1: TBevel
    Left = 12
    Top = 104
    Width = 437
    Height = 9
    Shape = bsTopLine
  end
  object Bevel2: TBevel
    Left = 12
    Top = 140
    Width = 437
    Height = 9
    Shape = bsTopLine
  end
  object Label4: TLabel
    Left = 132
    Top = 16
    Width = 147
    Height = 13
    Alignment = taRightJustify
    Caption = #1055#1077#1088#1080#1086#1076' '#1074#1077#1076#1077#1085#1080#1103' '#1092#1072#1081#1083#1072' '#1083#1086#1075#1072':'
  end
  object Bevel3: TBevel
    Left = 12
    Top = 40
    Width = 437
    Height = 9
    Shape = bsTopLine
  end
  object ButtonSave: TButton
    Left = 180
    Top = 248
    Width = 85
    Height = 25
    Caption = #1055#1088#1080#1085#1103#1090#1100
    ModalResult = 1
    TabOrder = 5
    OnClick = ButtonSaveClick
  end
  object ButtonCancel: TButton
    Left = 268
    Top = 248
    Width = 85
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 6
  end
  object ButtonHelp: TButton
    Left = 364
    Top = 248
    Width = 85
    Height = 25
    Caption = #1057#1087#1088#1072#1074#1082#1072
    Enabled = False
    TabOrder = 7
  end
  object EditCostDialogTime: TEdit
    Left = 340
    Top = 48
    Width = 85
    Height = 21
    TabOrder = 1
    OnExit = EditCostDialogTimeExit
  end
  object EditCostPrecision: TEdit
    Left = 340
    Top = 76
    Width = 85
    Height = 21
    TabOrder = 2
    OnExit = EditCostPrecisionExit
  end
  object EditFilterFreeTime: TEdit
    Left = 340
    Top = 112
    Width = 85
    Height = 21
    TabOrder = 3
    OnExit = EditFilterFreeTimeExit
  end
  object CheckBoxRightPause: TCheckBox
    Left = 20
    Top = 152
    Width = 385
    Height = 17
    Caption = #1056#1072#1079#1088#1077#1096#1080#1090#1100' '#1072#1076#1084#1080#1085#1080#1089#1090#1088#1072#1090#1086#1088#1072#1084' '#1087#1088#1080#1086#1089#1090#1072#1085#1072#1074#1083#1080#1074#1072#1090#1100' '#1082#1086#1084#1087#1100#1102#1090#1077#1088#1099
    TabOrder = 4
    OnExit = CheckBoxRightPauseExit
  end
  object ComboBoxLogPeriod: TComboBox
    Left = 284
    Top = 12
    Width = 141
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnExit = ComboBoxLogPeriodExit
    Items.Strings = (
      #1057#1091#1090#1082#1080
      #1053#1077#1076#1077#1083#1103
      #1052#1077#1089#1103#1094)
  end
end
