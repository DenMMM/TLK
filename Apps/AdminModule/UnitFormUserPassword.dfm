object FormUserPassword: TFormUserPassword
  Left = 339
  Top = 307
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Изменение пароля'
  ClientHeight = 198
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
    Height = 157
    Shape = bsFrame
  end
  object LabelLogin: TLabel
    Left = 66
    Top = 16
    Width = 34
    Height = 13
    Alignment = taRightJustify
    Caption = 'Логин:'
  end
  object LabelPassword: TLabel
    Left = 59
    Top = 40
    Width = 41
    Height = 13
    Alignment = taRightJustify
    Caption = 'Пароль:'
  end
  object LabelNew: TLabel
    Left = 24
    Top = 112
    Width = 76
    Height = 13
    Alignment = taRightJustify
    Caption = 'Новый пароль:'
  end
  object LabelConfirm: TLabel
    Left = 60
    Top = 136
    Width = 40
    Height = 13
    Alignment = taRightJustify
    Caption = 'Повтор:'
  end
  object Bevel2: TBevel
    Left = 12
    Top = 64
    Width = 325
    Height = 9
    Shape = bsTopLine
  end
  object Label5: TLabel
    Left = 20
    Top = 72
    Width = 309
    Height = 29
    AutoSize = False
    Caption = 
      'Наберите новый пароль и повторите его ввод  во второе поле. Новы' +
      'й пароль можно сгенерировать.'
    WordWrap = True
  end
  object ComboBoxLogin: TComboBox
    Left = 108
    Top = 12
    Width = 185
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnClick = ComboBoxLoginClick
  end
  object EditPassword: TEdit
    Left = 108
    Top = 36
    Width = 185
    Height = 21
    TabOrder = 1
  end
  object EditNew: TEdit
    Left = 108
    Top = 108
    Width = 185
    Height = 21
    TabOrder = 2
  end
  object EditConfirm: TEdit
    Left = 108
    Top = 132
    Width = 185
    Height = 21
    TabOrder = 4
  end
  object ButtonGenerate: TButton
    Left = 296
    Top = 108
    Width = 21
    Height = 21
    Caption = '...'
    TabOrder = 3
    OnClick = ButtonGenerateClick
  end
  object ButtonChange: TButton
    Left = 88
    Top = 168
    Width = 85
    Height = 25
    Caption = 'Изменить'
    Default = True
    ModalResult = 1
    TabOrder = 5
  end
  object ButtonCancel: TButton
    Left = 176
    Top = 168
    Width = 85
    Height = 25
    Cancel = True
    Caption = 'Отмена'
    ModalResult = 2
    TabOrder = 6
  end
end
