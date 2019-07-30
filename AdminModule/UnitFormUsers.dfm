object FormUsers: TFormUsers
  Left = 336
  Top = 168
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Пользователи'
  ClientHeight = 320
  ClientWidth = 459
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel4: TBevel
    Left = 6
    Top = 6
    Width = 447
    Height = 283
    Style = bsRaised
  end
  object Bevel2: TBevel
    Left = 12
    Top = 200
    Width = 436
    Height = 57
    Shape = bsFrame
  end
  object Bevel1: TBevel
    Left = 12
    Top = 140
    Width = 333
    Height = 57
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 22
    Top = 234
    Width = 64
    Height = 13
    Alignment = taRightJustify
    Caption = 'Полное имя:'
  end
  object Label2: TLabel
    Left = 25
    Top = 210
    Width = 61
    Height = 13
    Alignment = taRightJustify
    Caption = 'Должность:'
  end
  object Label3: TLabel
    Left = 52
    Top = 150
    Width = 34
    Height = 13
    Alignment = taRightJustify
    Caption = 'Логин:'
  end
  object Label4: TLabel
    Left = 45
    Top = 174
    Width = 41
    Height = 13
    Alignment = taRightJustify
    Caption = 'Пароль:'
  end
  object ListViewUsers: TListView
    Left = 10
    Top = 10
    Width = 439
    Height = 125
    Columns = <
      item
        Width = 0
      end
      item
        Caption = 'Логин'
        Width = 100
      end
      item
        Caption = 'Должность'
        Width = 130
      end
      item
        Caption = 'Полное имя'
        Width = 180
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SmallImages = FormMain.ImageListIcons
    TabOrder = 0
    ViewStyle = vsReport
    OnDeletion = ListViewUsersDeletion
    OnInsert = ListViewUsersInsert
    OnSelectItem = ListViewUsersSelectItem
  end
  object EditFullName: TEdit
    Left = 92
    Top = 230
    Width = 349
    Height = 21
    TabOrder = 8
    OnExit = EditFullNameExit
  end
  object EditLogin: TEdit
    Left = 92
    Top = 146
    Width = 133
    Height = 21
    MaxLength = 16
    TabOrder = 1
    OnExit = EditLoginExit
  end
  object ComboBoxJobTitle: TComboBox
    Left = 92
    Top = 206
    Width = 245
    Height = 21
    ItemHeight = 13
    TabOrder = 7
    OnExit = ComboBoxJobTitleExit
    Items.Strings = (
      'Помощник сисадмина'
      'Менеджер (бухгалтер и т.п.)'
      'Админ')
  end
  object ButtonAdd: TButton
    Left = 218
    Top = 261
    Width = 73
    Height = 22
    Caption = 'Добавить'
    TabOrder = 9
    OnClick = ButtonAddClick
  end
  object ButtonDelete: TButton
    Left = 290
    Top = 261
    Width = 73
    Height = 22
    Caption = 'Удалить'
    TabOrder = 10
    OnClick = ButtonDeleteClick
  end
  object ButtonChange: TButton
    Left = 366
    Top = 261
    Width = 73
    Height = 22
    Caption = 'Изменить'
    TabOrder = 11
    OnClick = ButtonChangeClick
  end
  object BitBtnSave: TBitBtn
    Left = 68
    Top = 293
    Width = 105
    Height = 24
    Caption = 'Сохранить'
    Default = True
    ModalResult = 1
    TabOrder = 12
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
    Left = 176
    Top = 293
    Width = 105
    Height = 24
    Caption = 'Отмена'
    TabOrder = 13
    Kind = bkCancel
  end
  object BitBtnHelp: TBitBtn
    Left = 284
    Top = 293
    Width = 105
    Height = 24
    Caption = 'Справка'
    TabOrder = 14
    Kind = bkHelp
  end
  object EditPassword: TEdit
    Left = 92
    Top = 170
    Width = 133
    Height = 21
    MaxLength = 16
    TabOrder = 3
    OnExit = EditPasswordExit
  end
  object ComboBoxPasswordLength: TComboBox
    Left = 230
    Top = 170
    Width = 47
    Height = 21
    Style = csDropDownList
    Enabled = False
    ItemHeight = 13
    TabOrder = 4
  end
  object CheckBoxActive: TCheckBox
    Left = 230
    Top = 148
    Width = 69
    Height = 17
    Caption = 'Активен'
    TabOrder = 2
    OnClick = CheckBoxActiveClick
  end
  object ButtonAccessRights: TButton
    Left = 354
    Top = 158
    Width = 89
    Height = 24
    Caption = 'Права...'
    TabOrder = 6
    OnClick = ButtonAccessRightsClick
  end
  object ButtonNewPassword: TButton
    Left = 280
    Top = 170
    Width = 57
    Height = 21
    Caption = 'Новый'
    Enabled = False
    TabOrder = 5
  end
end
