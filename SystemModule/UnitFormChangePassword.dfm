object FormChangePassword: TFormChangePassword
  Left = 308
  Top = 245
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Смена пароля'
  ClientHeight = 219
  ClientWidth = 410
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
    Width = 393
    Height = 177
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 70
    Top = 84
    Width = 34
    Height = 13
    Alignment = taRightJustify
    Caption = 'Логин:'
  end
  object Label2: TLabel
    Left = 17
    Top = 108
    Width = 87
    Height = 13
    Alignment = taRightJustify
    Caption = 'Текущий пароль:'
  end
  object Label4: TLabel
    Left = 28
    Top = 136
    Width = 76
    Height = 13
    Alignment = taRightJustify
    Caption = 'Новый пароль:'
  end
  object Label5: TLabel
    Left = 20
    Top = 160
    Width = 84
    Height = 13
    Alignment = taRightJustify
    Caption = 'Подтверждение:'
  end
  object Bevel2: TBevel
    Left = 16
    Top = 68
    Width = 377
    Height = 5
    Shape = bsTopLine
  end
  object LabelInfo: TLabel
    Left = 24
    Top = 20
    Width = 361
    Height = 45
    AutoSize = False
    Caption = 
      'Чтобы изменить пароль для входа в TimeLocker выберите свой логин' +
      ' в списке пользователей и введите свой текущий пароль. Затем зад' +
      'айте новый пароль и повторите его в поле '#39'Подтверждение'#39'.'
    WordWrap = True
  end
  object ComboBoxLogin: TComboBox
    Left = 108
    Top = 80
    Width = 169
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
  end
  object EditPassword: TEdit
    Left = 108
    Top = 104
    Width = 169
    Height = 21
    MaxLength = 16
    PasswordChar = '*'
    TabOrder = 1
  end
  object BitBtnOk: TBitBtn
    Left = 100
    Top = 191
    Width = 105
    Height = 24
    Caption = 'Изменить'
    TabOrder = 6
    OnClick = BitBtnOkClick
    Kind = bkOK
  end
  object BitBtnCancel: TBitBtn
    Left = 208
    Top = 191
    Width = 105
    Height = 24
    Caption = 'Отмена'
    TabOrder = 7
    OnClick = BitBtnCancelClick
    Kind = bkCancel
  end
  object EditNewPassword: TEdit
    Left = 108
    Top = 132
    Width = 169
    Height = 21
    MaxLength = 16
    PasswordChar = '*'
    TabOrder = 2
  end
  object EditConfirmPassword: TEdit
    Left = 108
    Top = 156
    Width = 169
    Height = 21
    MaxLength = 16
    PasswordChar = '*'
    TabOrder = 5
  end
  object ComboBoxPasswordLength: TComboBox
    Left = 282
    Top = 132
    Width = 47
    Height = 21
    Style = csDropDownList
    Enabled = False
    ItemHeight = 13
    TabOrder = 3
  end
  object Button1: TButton
    Left = 332
    Top = 132
    Width = 57
    Height = 21
    Caption = 'Новый'
    Enabled = False
    TabOrder = 4
  end
end
