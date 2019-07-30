object FormTariffs: TFormTariffs
  Left = 281
  Top = 196
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Тарифы'
  ClientHeight = 298
  ClientWidth = 483
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object BevelBorder: TBevel
    Left = 6
    Top = 6
    Width = 471
    Height = 262
    Style = bsRaised
  end
  object Bevel2: TBevel
    Left = 12
    Top = 177
    Width = 229
    Height = 48
    Shape = bsFrame
  end
  object Bevel1: TBevel
    Left = 12
    Top = 228
    Width = 229
    Height = 34
    Shape = bsFrame
  end
  object LabelPages: TLabel
    Left = 22
    Top = 239
    Width = 46
    Height = 13
    Alignment = taRightJustify
    Caption = 'Вкладки:'
  end
  object BevelBorderIcon: TBevel
    Left = 201
    Top = 152
    Width = 22
    Height = 22
    Style = bsRaised
  end
  object Label1: TLabel
    Left = 15
    Top = 156
    Width = 25
    Height = 13
    Alignment = taRightJustify
    Caption = 'Имя:'
  end
  object ImageIcon: TImage
    Left = 204
    Top = 155
    Width = 16
    Height = 16
    Stretch = True
    Transparent = True
  end
  object ListViewNames: TListView
    Left = 10
    Top = 10
    Width = 231
    Height = 139
    Columns = <
      item
        Width = 0
      end
      item
        Caption = 'Имя тарифа'
        Width = 200
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SmallImages = FormMain.ImageListIcons
    TabOrder = 0
    ViewStyle = vsReport
    OnChange = ListViewNamesChange
  end
  object BitBtnResetSelComp: TBitBtn
    Left = 418
    Top = 178
    Width = 27
    Height = 23
    Caption = '-'
    TabOrder = 13
    OnClick = BitBtnSetSelCompClick
  end
  object BitBtnResetAllComp: TBitBtn
    Left = 444
    Top = 178
    Width = 27
    Height = 23
    Caption = '--'
    TabOrder = 14
    OnClick = BitBtnSetAllCompClick
  end
  object EditName: TEdit
    Left = 44
    Top = 152
    Width = 153
    Height = 21
    MaxLength = 30
    TabOrder = 1
    OnExit = EditNameExit
  end
  object CheckBoxPage1: TCheckBox
    Left = 74
    Top = 237
    Width = 33
    Height = 17
    Caption = '1'
    TabOrder = 5
    OnClick = CheckBoxPage1Click
  end
  object CheckBoxPage2: TCheckBox
    Left = 106
    Top = 237
    Width = 33
    Height = 17
    Caption = '2'
    TabOrder = 6
    OnClick = CheckBoxPage1Click
  end
  object CheckBoxPage3: TCheckBox
    Left = 138
    Top = 237
    Width = 33
    Height = 17
    Caption = '3'
    TabOrder = 7
    OnClick = CheckBoxPage1Click
  end
  object BitBtnClose: TBitBtn
    Left = 188
    Top = 272
    Width = 105
    Height = 24
    Caption = 'Отмена'
    TabOrder = 21
    Kind = bkCancel
  end
  object BitBtnSelectIcon: TBitBtn
    Left = 224
    Top = 152
    Width = 17
    Height = 22
    Caption = '...'
    TabOrder = 2
    OnClick = BitBtnSelectIconClick
    Spacing = 0
  end
  object CheckBoxPage4: TCheckBox
    Left = 170
    Top = 237
    Width = 33
    Height = 17
    Caption = '4'
    TabOrder = 8
    OnClick = CheckBoxPage1Click
  end
  object CheckBoxPage5: TCheckBox
    Left = 202
    Top = 237
    Width = 33
    Height = 17
    Caption = '5'
    TabOrder = 9
    OnClick = CheckBoxPage1Click
  end
  object CheckBoxRoute: TCheckBox
    Left = 20
    Top = 201
    Width = 177
    Height = 17
    Caption = 'Internet'
    TabOrder = 4
    OnClick = CheckBoxPage1Click
  end
  object BitBtnSetSelComp: TBitBtn
    Left = 362
    Top = 178
    Width = 27
    Height = 23
    Caption = '+'
    TabOrder = 11
    OnClick = BitBtnSetSelCompClick
  end
  object BitBtnSetAllComp: TBitBtn
    Left = 388
    Top = 178
    Width = 27
    Height = 23
    Caption = '++'
    TabOrder = 12
    OnClick = BitBtnSetAllCompClick
  end
  object BitBtnSave: TBitBtn
    Left = 80
    Top = 272
    Width = 105
    Height = 24
    Caption = 'Сохранить'
    TabOrder = 20
    OnClick = BitBtnSaveClick
    Kind = bkOK
  end
  object ListViewComputers: TListView
    Left = 244
    Top = 10
    Width = 229
    Height = 166
    Checkboxes = True
    Columns = <>
    ColumnClick = False
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    ShowColumnHeaders = False
    SmallImages = FormMain.ImageListIcons
    TabOrder = 10
    ViewStyle = vsList
    OnKeyPress = ListViewComputersKeyPress
  end
  object BitBtnHelp: TBitBtn
    Left = 296
    Top = 272
    Width = 105
    Height = 24
    Caption = 'Справка'
    TabOrder = 22
    Kind = bkHelp
  end
  object ButtonAdd: TButton
    Left = 248
    Top = 240
    Width = 73
    Height = 22
    Caption = 'Добавить'
    TabOrder = 17
    OnClick = ButtonAddClick
  end
  object ButtonDelete: TButton
    Left = 320
    Top = 240
    Width = 73
    Height = 22
    Caption = 'Удалить'
    TabOrder = 18
    OnClick = ButtonDeleteClick
  end
  object ButtonChange: TButton
    Left = 396
    Top = 240
    Width = 73
    Height = 22
    Caption = 'Изменить'
    TabOrder = 19
    OnClick = ButtonChangeClick
  end
  object ButtonPorts: TButton
    Left = 268
    Top = 208
    Width = 89
    Height = 24
    Caption = 'Порты...'
    Enabled = False
    TabOrder = 15
  end
  object ButtonTimes: TButton
    Left = 364
    Top = 208
    Width = 89
    Height = 24
    Caption = 'Время...'
    TabOrder = 16
    OnClick = ButtonTimesClick
  end
  object CheckBoxReboot: TCheckBox
    Left = 20
    Top = 184
    Width = 177
    Height = 17
    Caption = 'Перезагрузка после запуска'
    TabOrder = 3
    OnClick = CheckBoxRebootClick
  end
  object OpenPictureDialog: TOpenPictureDialog
    DefaultExt = '*.ico'
    Filter = 'Иконки (*.ico)|*.ico'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofNoNetworkButton, ofEnableSizing]
    Title = 'Иконка для тарифа'
    Left = 452
    Top = 2
  end
end
