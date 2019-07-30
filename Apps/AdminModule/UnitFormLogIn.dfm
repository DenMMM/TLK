object FormLogIn: TFormLogIn
  Left = 341
  Top = 287
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Новая смена'
  ClientHeight = 198
  ClientWidth = 350
  Color = clBtnFace
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
    Caption = 'Логин:'
  end
  object LabelPassword: TLabel
    Left = 55
    Top = 124
    Width = 41
    Height = 13
    Alignment = taRightJustify
    Caption = 'Пароль:'
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
      'Выберите логин и введите пароль. Если вашего логина нет в списке' +
      ', возможно, он временно отключен сисадмином.'
    Layout = tlCenter
    WordWrap = True
  end
  object ComboBoxLogin: TComboBox
    Left = 100
    Top = 96
    Width = 185
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
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
    Caption = 'Принять'
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
    Caption = 'Отмена'
    ModalResult = 2
    TabOrder = 3
  end
end
