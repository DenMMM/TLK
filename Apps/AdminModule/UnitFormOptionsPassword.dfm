object FormOptionsPassword: TFormOptionsPassword
  Left = 419
  Top = 359
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Пароль на настройки'
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
    Caption = 'Пароль:'
  end
  object LabelNew: TLabel
    Left = 24
    Top = 88
    Width = 76
    Height = 13
    Alignment = taRightJustify
    Caption = 'Новый пароль:'
  end
  object LabelConfirm: TLabel
    Left = 60
    Top = 112
    Width = 40
    Height = 13
    Alignment = taRightJustify
    Caption = 'Повтор:'
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
      'Наберите новый пароль и повторите его ввод  во второе поле. Новы' +
      'й пароль можно сгенерировать.'
    WordWrap = True
  end
  object EditPassword: TEdit
    Left = 108
    Top = 12
    Width = 185
    Height = 21
    TabOrder = 0
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
    Caption = 'Изменить'
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
    Caption = 'Отмена'
    ModalResult = 2
    TabOrder = 5
  end
end
