object FormOpenConfig: TFormOpenConfig
  Left = 377
  Top = 326
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Открыть настройки'
  ClientHeight = 86
  ClientWidth = 294
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
    Width = 285
    Height = 45
    Shape = bsFrame
  end
  object LabelPassword: TLabel
    Left = 31
    Top = 20
    Width = 41
    Height = 13
    Alignment = taRightJustify
    Caption = 'Пароль:'
  end
  object EditPassword: TEdit
    Left = 76
    Top = 16
    Width = 185
    Height = 21
    TabOrder = 0
  end
  object ButtonApply: TButton
    Left = 60
    Top = 56
    Width = 85
    Height = 25
    Caption = 'Принять'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
  object ButtonCancel: TButton
    Left = 148
    Top = 56
    Width = 85
    Height = 25
    Cancel = True
    Caption = 'Отмена'
    ModalResult = 2
    TabOrder = 2
  end
end
