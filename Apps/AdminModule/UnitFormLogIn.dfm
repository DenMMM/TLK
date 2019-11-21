object FormLogIn: TFormLogIn
  Left = 341
  Top = 287
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1053#1086#1074#1072#1103' '#1089#1084#1077#1085#1072
  ClientHeight = 198
  ClientWidth = 350
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 4
    Top = 4
    Width = 341
    Height = 157
    Shape = bsFrame
  end
  object LabelLogin: TLabel
    Left = 62
    Top = 100
    Width = 34
    Height = 13
    Alignment = taRightJustify
    Caption = #1051#1086#1075#1080#1085':'
  end
  object LabelPassword: TLabel
    Left = 55
    Top = 124
    Width = 41
    Height = 13
    Alignment = taRightJustify
    Caption = #1055#1072#1088#1086#1083#1100':'
  end
  object Bevel2: TBevel
    Left = 12
    Top = 76
    Width = 325
    Height = 9
    Shape = bsTopLine
  end
  object Label5: TLabel
    Left = 20
    Top = 18
    Width = 309
    Height = 47
    AutoSize = False
    Caption = 
      #1042#1099#1073#1077#1088#1080#1090#1077' '#1083#1086#1075#1080#1085' '#1080' '#1074#1074#1077#1076#1080#1090#1077' '#1087#1072#1088#1086#1083#1100'. '#1045#1089#1083#1080' '#1074#1072#1096#1077#1075#1086' '#1083#1086#1075#1080#1085#1072' '#1085#1077#1090' '#1074' '#1089#1087#1080#1089#1082#1077 +
      ', '#1074#1086#1079#1084#1086#1078#1085#1086', '#1086#1085' '#1074#1088#1077#1084#1077#1085#1085#1086' '#1086#1090#1082#1083#1102#1095#1077#1085' '#1089#1080#1089#1072#1076#1084#1080#1085#1086#1084'.'
    Layout = tlCenter
    WordWrap = True
  end
  object ComboBoxLogin: TComboBox
    Left = 100
    Top = 96
    Width = 185
    Height = 21
    Style = csDropDownList
    TabOrder = 0
    OnClick = ComboBoxLoginClick
  end
  object EditPassword: TEdit
    Left = 100
    Top = 120
    Width = 185
    Height = 21
    TabOrder = 1
  end
  object ButtonApply: TButton
    Left = 92
    Top = 168
    Width = 85
    Height = 25
    Caption = #1055#1088#1080#1085#1103#1090#1100
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
  object ButtonCancel: TButton
    Left = 180
    Top = 168
    Width = 85
    Height = 25
    Cancel = True
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 3
  end
end
