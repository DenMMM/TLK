object FormAccessRights: TFormAccessRights
  Left = 290
  Top = 156
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  ClientHeight = 271
  ClientWidth = 431
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel4: TBevel
    Left = 6
    Top = 6
    Width = 419
    Height = 235
    Style = bsRaised
  end
  object Bevel1: TBevel
    Left = 31
    Top = 32
    Width = 386
    Height = 53
    Shape = bsFrame
  end
  object Bevel5: TBevel
    Left = 31
    Top = 112
    Width = 386
    Height = 73
    Shape = bsFrame
  end
  object BitBtnSave: TBitBtn
    Left = 56
    Top = 245
    Width = 105
    Height = 24
    Caption = 'Сохранить'
    Default = True
    ModalResult = 1
    TabOrder = 11
    OnClick = BitBtnSaveClick
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    NumGlyphs = 2
  end
  object BitBtnCancel: TBitBtn
    Left = 164
    Top = 245
    Width = 105
    Height = 24
    Caption = 'Отмена'
    TabOrder = 12
    Kind = bkCancel
  end
  object BitBtnHelp: TBitBtn
    Left = 272
    Top = 245
    Width = 105
    Height = 24
    Caption = 'Справка'
    TabOrder = 13
    Kind = bkHelp
  end
  object CheckBox1: TCheckBox
    Left = 39
    Top = 120
    Width = 177
    Height = 17
    Caption = 'Изменение списка программ'
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 6
  end
  object CheckBoxSetAutoMode: TCheckBox
    Left = 39
    Top = 140
    Width = 245
    Height = 17
    Caption = 'Перевод компьютеров в режим настройки'
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 7
  end
  object CheckBoxSetUseMode: TCheckBox
    Left = 39
    Top = 160
    Width = 269
    Height = 17
    Caption = 'Перевод компьютеров в режим использования'
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 8
  end
  object CheckBox4: TCheckBox
    Left = 15
    Top = 216
    Width = 241
    Height = 17
    Caption = 'Удаленное наблюдение за компьютерами'
    Enabled = False
    TabOrder = 10
  end
  object CheckBox5: TCheckBox
    Left = 39
    Top = 40
    Width = 134
    Height = 17
    Caption = 'Выдача абонементов'
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 1
  end
  object CheckBox6: TCheckBox
    Left = 39
    Top = 60
    Width = 113
    Height = 17
    Caption = 'Запись на время'
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 2
  end
  object CheckBoxRemoveAllTime: TCheckBox
    Left = 203
    Top = 40
    Width = 149
    Height = 17
    Caption = 'Полное снятие времени'
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 3
  end
  object CheckBox9: TCheckBox
    Left = 219
    Top = 60
    Width = 122
    Height = 17
    Caption = 'С возвратом денег'
    Enabled = False
    TabOrder = 4
  end
  object CheckBoxClientsServices: TCheckBox
    Left = 15
    Top = 12
    Width = 173
    Height = 17
    Caption = 'Обслуживание посетителей'
    TabOrder = 0
    OnClick = CheckBoxClientsServicesClick
  end
  object CheckBoxComputersServices: TCheckBox
    Left = 15
    Top = 92
    Width = 177
    Height = 17
    Caption = 'Обслуживание компьютеров'
    TabOrder = 5
    OnClick = CheckBoxComputersServicesClick
  end
  object CheckBoxWorkWithLogs: TCheckBox
    Left = 15
    Top = 192
    Width = 186
    Height = 17
    Caption = 'Работа с финансовыми логами'
    Enabled = False
    TabOrder = 9
  end
end
