object FormTariffs: TFormTariffs
  Left = 219
  Top = 165
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Тарифы'
  ClientHeight = 382
  ClientWidth = 649
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
    Left = 4
    Top = 4
    Width = 641
    Height = 341
    Style = bsRaised
  end
  object Bevel10: TBevel
    Left = 220
    Top = 52
    Width = 417
    Height = 57
    Shape = bsFrame
  end
  object Bevel7: TBevel
    Left = 220
    Top = 12
    Width = 417
    Height = 33
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 137
    Top = 274
    Width = 25
    Height = 13
    Alignment = taRightJustify
    Caption = 'Имя:'
  end
  object LabelTariffName: TLabel
    Left = 235
    Top = 22
    Width = 53
    Height = 13
    Alignment = taRightJustify
    Caption = 'Название:'
  end
  object LabelTariffApps: TLabel
    Left = 228
    Top = 112
    Width = 94
    Height = 13
    Caption = 'Группы программ:'
  end
  object LabelTariffComputers: TLabel
    Left = 410
    Top = 112
    Width = 96
    Height = 13
    Caption = 'Для компьютеров:'
  end
  object ListViewNames: TListView
    Left = 8
    Top = 8
    Width = 205
    Height = 305
    Columns = <
      item
        AutoSize = True
        Caption = 'Название'
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SortType = stText
    TabOrder = 0
    ViewStyle = vsReport
    OnDeletion = ListViewNamesDeletion
    OnInsert = ListViewNamesInsert
    OnSelectItem = ListViewNamesSelectItem
  end
  object EditName: TEdit
    Left = 292
    Top = 18
    Width = 337
    Height = 21
    TabOrder = 3
    OnExit = EditNameExit
  end
  object ListViewComputers: TListView
    Left = 408
    Top = 128
    Width = 229
    Height = 209
    Checkboxes = True
    Columns = <>
    ColumnClick = False
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    ShowColumnHeaders = False
    SmallImages = FormMain.ImageListIcons
    TabOrder = 7
    ViewStyle = vsList
    OnExit = ListViewComputersExit
    OnKeyPress = ListViewComputersKeyPress
  end
  object ButtonAdd: TButton
    Left = 34
    Top = 318
    Width = 73
    Height = 22
    Caption = 'Добавить'
    TabOrder = 1
    OnClick = ButtonAddClick
  end
  object ButtonDel: TButton
    Left = 110
    Top = 318
    Width = 73
    Height = 22
    Caption = 'Удалить'
    TabOrder = 2
    OnClick = ButtonDelClick
  end
  object ButtonTimes: TButton
    Left = 538
    Top = 70
    Width = 89
    Height = 24
    Caption = 'Время...'
    TabOrder = 5
    OnClick = ButtonTimesClick
  end
  object CheckBoxReboot: TCheckBox
    Left = 232
    Top = 60
    Width = 177
    Height = 17
    Caption = 'Перезагрузка после запуска'
    TabOrder = 4
    OnExit = CheckBoxRebootExit
  end
  object ButtonSave: TButton
    Left = 368
    Top = 352
    Width = 85
    Height = 25
    Caption = 'Принять'
    ModalResult = 1
    TabOrder = 12
    OnClick = ButtonSaveClick
  end
  object ButtonCancel: TButton
    Left = 456
    Top = 352
    Width = 85
    Height = 25
    Caption = 'Отмена'
    ModalResult = 2
    TabOrder = 13
  end
  object ButtonHelp: TButton
    Left = 552
    Top = 352
    Width = 85
    Height = 25
    Caption = 'Справка'
    Enabled = False
    TabOrder = 14
  end
  object CheckListBoxApps: TCheckListBox
    Left = 220
    Top = 128
    Width = 181
    Height = 177
    ItemHeight = 13
    TabOrder = 6
    OnExit = CheckListBoxAppsExit
  end
  object ButtonSetSelComp: TButton
    Left = 290
    Top = 312
    Width = 29
    Height = 25
    Caption = '+'
    TabOrder = 8
    OnClick = ButtonSetSelCompClick
  end
  object ButtonSetAllComp: TButton
    Left = 318
    Top = 312
    Width = 29
    Height = 25
    Caption = '++'
    TabOrder = 9
    OnClick = ButtonSetAllCompClick
  end
  object ButtonResSelComp: TButton
    Left = 349
    Top = 312
    Width = 29
    Height = 25
    Caption = '-'
    TabOrder = 10
    OnClick = ButtonSetSelCompClick
  end
  object ButtonResAllComp: TButton
    Left = 377
    Top = 312
    Width = 29
    Height = 25
    Caption = '--'
    TabOrder = 11
    OnClick = ButtonSetAllCompClick
  end
  object CheckBoxRoute: TCheckBox
    Left = 232
    Top = 80
    Width = 177
    Height = 17
    Caption = 'Выход в Internet'
    TabOrder = 15
    OnExit = CheckBoxRouteExit
  end
  object OpenPictureDialog: TOpenPictureDialog
    DefaultExt = '*.ico'
    Filter = 'Иконки (*.ico)|*.ico'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofNoNetworkButton, ofEnableSizing]
    Title = 'Иконка для тарифа'
    Left = 2
    Top = 4
  end
end
