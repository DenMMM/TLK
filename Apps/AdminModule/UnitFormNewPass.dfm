object FormNewPass: TFormNewPass
  Left = 376
  Top = 332
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = #1053#1086#1074#1099#1081' '#1087#1072#1088#1086#1083#1100
  ClientHeight = 165
  ClientWidth = 325
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object BevelBorder: TBevel
    Left = 4
    Top = 4
    Width = 317
    Height = 125
    Shape = bsFrame
  end
  object Bevel1: TBevel
    Left = 28
    Top = 16
    Width = 213
    Height = 21
    Style = bsRaised
  end
  object LabelPassword: TLabel
    Left = 32
    Top = 20
    Width = 205
    Height = 13
    Alignment = taCenter
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object LabelLength: TLabel
    Left = 23
    Top = 64
    Width = 36
    Height = 13
    Alignment = taRightJustify
    Caption = #1044#1083#1080#1085#1072':'
  end
  object Bevel2: TBevel
    Left = 12
    Top = 48
    Width = 301
    Height = 9
    Shape = bsTopLine
  end
  object ButtonNew: TButton
    Left = 244
    Top = 16
    Width = 53
    Height = 21
    Caption = #1053#1086#1074#1099#1081
    TabOrder = 0
    OnClick = ButtonNewClick
  end
  object ComboBoxLength: TComboBox
    Left = 64
    Top = 60
    Width = 73
    Height = 21
    Style = csDropDownList
    TabOrder = 1
  end
  object CheckBoxCapital: TCheckBox
    Left = 152
    Top = 60
    Width = 145
    Height = 17
    Caption = #1047#1040#1043#1051#1040#1042#1053#1067#1045' '#1073#1091#1082#1074#1099
    TabOrder = 2
  end
  object CheckBoxSmall: TCheckBox
    Left = 152
    Top = 80
    Width = 145
    Height = 17
    Caption = #1089#1090#1088#1086#1095#1085#1099#1077' '#1073#1091#1082#1074#1099
    TabOrder = 3
  end
  object CheckBoxNum: TCheckBox
    Left = 152
    Top = 100
    Width = 145
    Height = 17
    Caption = #1062#1080#1092#1088#1099
    TabOrder = 4
  end
  object ButtonUse: TButton
    Left = 76
    Top = 136
    Width = 85
    Height = 25
    Caption = #1055#1088#1080#1085#1103#1090#1100
    Default = True
    ModalResult = 1
    TabOrder = 5
  end
  object ButtonCancel: TButton
    Left = 164
    Top = 136
    Width = 85
    Height = 25
    Cancel = True
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 6
  end
end
