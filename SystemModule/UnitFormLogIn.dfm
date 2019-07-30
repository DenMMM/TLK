object FormLogIn: TFormLogIn
  Left = 361
  Top = 248
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'Вход в TimeLocker'
  ClientHeight = 185
  ClientWidth = 334
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCloseQuery = FormCloseQuery
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 317
    Height = 145
    Shape = bsFrame
  end
  object LabelInfo: TLabel
    Left = 20
    Top = 24
    Width = 293
    Height = 57
    AutoSize = False
    Caption = 
      'Для входа в программу выберите свой логин из списка и введите па' +
      'роль. Если Ваш логин отсутствует в списке, то Вы удалены из спис' +
      'ка пользователей программы. В таком случае обратитесь к системно' +
      'му администратору.'
    WordWrap = True
  end
  object Label1: TLabel
    Left = 48
    Top = 100
    Width = 34
    Height = 13
    Alignment = taRightJustify
    Caption = 'Логин:'
  end
  object Label2: TLabel
    Left = 41
    Top = 124
    Width = 41
    Height = 13
    Alignment = taRightJustify
    Caption = 'Пароль:'
  end
  object ComboBoxLogin: TComboBox
    Left = 88
    Top = 96
    Width = 173
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
  end
  object EditPassword: TEdit
    Left = 88
    Top = 120
    Width = 173
    Height = 21
    PasswordChar = '*'
    TabOrder = 1
  end
  object BitBtnOk: TBitBtn
    Left = 64
    Top = 158
    Width = 105
    Height = 24
    Caption = 'Принять'
    TabOrder = 2
    OnClick = BitBtnOkClick
    Kind = bkOK
  end
  object BitBtnCancel: TBitBtn
    Left = 172
    Top = 158
    Width = 105
    Height = 24
    Caption = 'Отмена'
    TabOrder = 3
    OnClick = BitBtnCancelClick
    Kind = bkCancel
  end
end
