object FormOptionsPass: TFormOptionsPass
  Left = 419
  Top = 359
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1055#1072#1088#1086#1083#1100' '#1085#1072' '#1085#1072#1089#1090#1088#1086#1081#1082#1080
  ClientHeight = 174
  ClientWidth = 350
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 4
    Top = 4
    Width = 341
    Height = 133
    Shape = bsFrame
  end
  object LabelPassword: TLabel
    Left = 59
    Top = 16
    Width = 41
    Height = 13
    Alignment = taRightJustify
    Caption = #1055#1072#1088#1086#1083#1100':'
  end
  object LabelNew: TLabel
    Left = 24
    Top = 88
    Width = 76
    Height = 13
    Alignment = taRightJustify
    Caption = #1053#1086#1074#1099#1081' '#1087#1072#1088#1086#1083#1100':'
  end
  object LabelConfirm: TLabel
    Left = 60
    Top = 112
    Width = 40
    Height = 13
    Alignment = taRightJustify
    Caption = #1055#1086#1074#1090#1086#1088':'
  end
  object Bevel2: TBevel
    Left = 12
    Top = 40
    Width = 325
    Height = 9
    Shape = bsTopLine
  end
  object Label5: TLabel
    Left = 20
    Top = 48
    Width = 309
    Height = 29
    AutoSize = False
    Caption = 
      #1053#1072#1073#1077#1088#1080#1090#1077' '#1085#1086#1074#1099#1081' '#1087#1072#1088#1086#1083#1100' '#1080' '#1087#1086#1074#1090#1086#1088#1080#1090#1077' '#1077#1075#1086' '#1074#1074#1086#1076'  '#1074#1086' '#1074#1090#1086#1088#1086#1077' '#1087#1086#1083#1077'. '#1053#1086#1074#1099 +
      #1081' '#1087#1072#1088#1086#1083#1100' '#1084#1086#1078#1085#1086' '#1089#1075#1077#1085#1077#1088#1080#1088#1086#1074#1072#1090#1100'.'
    WordWrap = True
  end
  object EditPassword: TEdit
    Left = 108
    Top = 12
    Width = 185
    Height = 21
    TabOrder = 0
    OnKeyPress = EditPasswordKeyPress
  end
  object EditNew: TEdit
    Left = 108
    Top = 84
    Width = 185
    Height = 21
    TabOrder = 1
  end
  object EditConfirm: TEdit
    Left = 108
    Top = 108
    Width = 185
    Height = 21
    TabOrder = 2
  end
  object ButtonGenerate: TButton
    Left = 296
    Top = 84
    Width = 21
    Height = 21
    Caption = '...'
    TabOrder = 3
    OnClick = ButtonGenerateClick
  end
  object ButtonChange: TButton
    Left = 88
    Top = 144
    Width = 85
    Height = 25
    Caption = #1048#1079#1084#1077#1085#1080#1090#1100
    Default = True
    ModalResult = 1
    TabOrder = 4
  end
  object ButtonCancel: TButton
    Left = 176
    Top = 144
    Width = 85
    Height = 25
    Cancel = True
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 5
  end
end
