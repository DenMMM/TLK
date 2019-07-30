object FormComputers: TFormComputers
  Left = 329
  Top = 197
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Компьютеры'
  ClientHeight = 319
  ClientWidth = 468
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
  object BevelBorder: TBevel
    Left = 6
    Top = 6
    Width = 455
    Height = 283
    Style = bsRaised
  end
  object Bevel1: TBevel
    Left = 204
    Top = 224
    Width = 253
    Height = 33
    Shape = bsFrame
  end
  object BevelNet: TBevel
    Left = 204
    Top = 192
    Width = 253
    Height = 29
    Shape = bsFrame
  end
  object BevelPattern: TBevel
    Left = 204
    Top = 112
    Width = 253
    Height = 53
    Shape = bsFrame
  end
  object BevelJust: TBevel
    Left = 204
    Top = 32
    Width = 253
    Height = 53
    Shape = bsFrame
  end
  object LabelAddress: TLabel
    Left = 212
    Top = 40
    Width = 112
    Height = 13
    Alignment = taRightJustify
    Caption = 'IP-адрес компьютера:'
    Color = clBtnFace
    ParentColor = False
  end
  object LabelNumber: TLabel
    Left = 268
    Top = 64
    Width = 56
    Height = 13
    Alignment = taRightJustify
    Caption = 'Его номер:'
  end
  object LabelMarkerColor: TLabel
    Left = 231
    Top = 235
    Width = 67
    Height = 13
    Alignment = taRightJustify
    Caption = 'Цвет группы:'
  end
  object ShapeMarkerColor: TShape
    Left = 302
    Top = 230
    Width = 22
    Height = 22
    Brush.Color = clBlack
    Enabled = False
  end
  object LabelPattern: TLabel
    Left = 282
    Top = 120
    Width = 42
    Height = 13
    Alignment = taRightJustify
    Caption = 'Шаблон:'
  end
  object LabelBeginNumber: TLabel
    Left = 221
    Top = 144
    Width = 51
    Height = 13
    Alignment = taRightJustify
    Caption = 'С номера:'
  end
  object LabelEndNumber: TLabel
    Left = 332
    Top = 144
    Width = 52
    Height = 13
    Alignment = taRightJustify
    Caption = 'По номер:'
  end
  object LabelGlobalPattern: TLabel
    Left = 282
    Top = 200
    Width = 42
    Height = 13
    Alignment = taRightJustify
    Caption = 'Шаблон:'
  end
  object EditAddress: TEdit
    Left = 328
    Top = 36
    Width = 125
    Height = 21
    MaxLength = 20
    TabOrder = 4
    OnExit = EditAddressExit
  end
  object EditNumber: TEdit
    Tag = 1
    Left = 328
    Top = 60
    Width = 45
    Height = 21
    TabOrder = 5
    OnExit = EditNumberExit
  end
  object BitBtnMarkerRed: TBitBtn
    Left = 357
    Top = 230
    Width = 22
    Height = 22
    TabOrder = 11
    OnClick = BitBtnMarkerNoneClick
    Glyph.Data = {
      46050000424D4605000000000000360400002800000010000000110000000100
      08000000000010010000120B0000120B00000200000000000000000000000000
      FF00000000000000000000000000000000000101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      010101010101010101010101010101010101}
    Spacing = 0
  end
  object BitBtnMarkerLime: TBitBtn
    Left = 381
    Top = 230
    Width = 22
    Height = 22
    TabOrder = 12
    OnClick = BitBtnMarkerNoneClick
    Glyph.Data = {
      46050000424D4605000000000000360400002800000010000000110000000100
      08000000000010010000120B0000120B000002000000000000000000000000FF
      0000000000000000000000000000000000000101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      010101010101010101010101010101010101}
  end
  object BitBtnMarkerAqua: TBitBtn
    Left = 405
    Top = 230
    Width = 22
    Height = 22
    TabOrder = 13
    OnClick = BitBtnMarkerNoneClick
    Glyph.Data = {
      46050000424D4605000000000000360400002800000010000000110000000100
      08000000000010010000120B0000120B0000020000000000000000000000FFFF
      0000000000000000000000000000000000000101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      010101010101010101010101010101010101}
  end
  object BitBtnMarkerYellow: TBitBtn
    Left = 429
    Top = 230
    Width = 22
    Height = 22
    TabOrder = 14
    OnClick = BitBtnMarkerNoneClick
    Glyph.Data = {
      46050000424D4605000000000000360400002800000010000000110000000100
      08000000000010010000120B0000120B000002000000000000000000000000FF
      FF00000000000000000000000000000000000101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      010101010101010101010101010101010101}
  end
  object BitBtnSave: TBitBtn
    Left = 76
    Top = 293
    Width = 105
    Height = 24
    Caption = 'Сохранить'
    Default = True
    ModalResult = 1
    TabOrder = 18
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
  object ListViewComputers: TListView
    Left = 10
    Top = 10
    Width = 173
    Height = 275
    Columns = <
      item
        Width = 0
      end
      item
        Alignment = taRightJustify
        Caption = '№'
        Width = 40
      end
      item
        Caption = 'IP-адрес'
        Width = 100
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SmallImages = FormMain.ImageListIcons
    SortType = stText
    TabOrder = 0
    ViewStyle = vsReport
    OnDeletion = ListViewComputersDeletion
    OnInsert = ListViewComputersInsert
    OnSelectItem = ListViewComputersSelectItem
  end
  object RadioButtonJust: TRadioButton
    Left = 192
    Top = 12
    Width = 153
    Height = 17
    Caption = 'Задать непосредственно'
    TabOrder = 1
    OnClick = RadioButtonJustClick
  end
  object RadioButtonPattern: TRadioButton
    Left = 192
    Top = 92
    Width = 121
    Height = 17
    Caption = 'Задать по шаблону'
    Enabled = False
    TabOrder = 2
    OnClick = RadioButtonJustClick
  end
  object RadioButtonNet: TRadioButton
    Left = 192
    Top = 172
    Width = 101
    Height = 17
    Caption = 'Опросить сеть'
    Enabled = False
    TabOrder = 3
    OnClick = RadioButtonJustClick
  end
  object EditPattern: TEdit
    Left = 328
    Top = 116
    Width = 125
    Height = 21
    MaxLength = 20
    TabOrder = 6
  end
  object EditBeginNumber: TEdit
    Left = 276
    Top = 140
    Width = 45
    Height = 21
    TabOrder = 7
  end
  object EditEndNumber: TEdit
    Left = 388
    Top = 140
    Width = 45
    Height = 21
    TabOrder = 8
  end
  object EditGlobalPattern: TEdit
    Left = 328
    Top = 196
    Width = 125
    Height = 21
    MaxLength = 20
    TabOrder = 9
  end
  object BitBtnCancel: TBitBtn
    Left = 184
    Top = 293
    Width = 105
    Height = 24
    Caption = 'Отмена'
    TabOrder = 19
    Kind = bkCancel
  end
  object BitBtnHelp: TBitBtn
    Left = 292
    Top = 293
    Width = 105
    Height = 24
    Caption = 'Справка'
    TabOrder = 20
    Kind = bkHelp
  end
  object ButtonAdd: TButton
    Left = 214
    Top = 262
    Width = 73
    Height = 22
    Caption = 'Добавить'
    TabOrder = 15
    OnClick = ButtonAddClick
  end
  object ButtonDelete: TButton
    Left = 286
    Top = 262
    Width = 73
    Height = 22
    Caption = 'Удалить'
    TabOrder = 16
    OnClick = ButtonDeleteClick
  end
  object ButtonChange: TButton
    Left = 362
    Top = 262
    Width = 73
    Height = 22
    Caption = 'Изменить'
    TabOrder = 17
    OnClick = ButtonChangeClick
  end
  object BitBtnMarkerNone: TBitBtn
    Left = 333
    Top = 230
    Width = 22
    Height = 22
    TabOrder = 10
    OnClick = BitBtnMarkerNoneClick
    Spacing = 0
  end
end
