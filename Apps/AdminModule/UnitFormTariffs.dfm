object FormTariffs: TFormTariffs
  Left = 219
  Top = 165
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1058#1072#1088#1080#1092#1099
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
    Height = 73
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
    Caption = #1048#1084#1103':'
  end
  object LabelTariffName: TLabel
    Left = 235
    Top = 22
    Width = 53
    Height = 13
    Alignment = taRightJustify
    Caption = #1053#1072#1079#1074#1072#1085#1080#1077':'
  end
  object LabelTariffApps: TLabel
    Left = 228
    Top = 128
    Width = 94
    Height = 13
    Caption = #1043#1088#1091#1087#1087#1099' '#1087#1088#1086#1075#1088#1072#1084#1084':'
  end
  object LabelTariffComputers: TLabel
    Left = 410
    Top = 128
    Width = 96
    Height = 13
    Caption = #1044#1083#1103' '#1082#1086#1084#1087#1100#1102#1090#1077#1088#1086#1074':'
  end
  object ListViewNames: TListView
    Left = 8
    Top = 8
    Width = 205
    Height = 305
    Columns = <
      item
        AutoSize = True
        Caption = #1053#1072#1079#1074#1072#1085#1080#1077
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SortType = stText
    TabOrder = 0
    ViewStyle = vsReport
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
    Top = 144
    Width = 229
    Height = 193
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
    Caption = #1044#1086#1073#1072#1074#1080#1090#1100
    TabOrder = 1
    OnClick = ButtonAddClick
  end
  object ButtonDel: TButton
    Left = 110
    Top = 318
    Width = 73
    Height = 22
    Caption = #1059#1076#1072#1083#1080#1090#1100
    TabOrder = 2
    OnClick = ButtonDelClick
  end
  object ButtonTimes: TButton
    Left = 538
    Top = 76
    Width = 89
    Height = 24
    Caption = #1042#1088#1077#1084#1103'...'
    TabOrder = 5
    OnClick = ButtonTimesClick
  end
  object CheckBoxReboot: TCheckBox
    Left = 232
    Top = 60
    Width = 177
    Height = 17
    Caption = #1055#1077#1088#1077#1079#1072#1075#1088#1091#1079#1082#1072' '#1087#1086#1089#1083#1077' '#1079#1072#1087#1091#1089#1082#1072
    TabOrder = 4
    OnExit = CheckBoxRebootExit
  end
  object ButtonSave: TButton
    Left = 368
    Top = 352
    Width = 85
    Height = 25
    Caption = #1055#1088#1080#1085#1103#1090#1100
    ModalResult = 1
    TabOrder = 12
    OnClick = ButtonSaveClick
  end
  object ButtonCancel: TButton
    Left = 456
    Top = 352
    Width = 85
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 13
  end
  object ButtonHelp: TButton
    Left = 552
    Top = 352
    Width = 85
    Height = 25
    Caption = #1057#1087#1088#1072#1074#1082#1072
    Enabled = False
    TabOrder = 14
  end
  object CheckListBoxApps: TCheckListBox
    Left = 220
    Top = 144
    Width = 181
    Height = 161
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
    Caption = #1042#1099#1093#1086#1076' '#1074' Internet'
    TabOrder = 15
    OnExit = CheckBoxRouteExit
  end
  object CheckBoxDesktop: TCheckBox
    Left = 232
    Top = 100
    Width = 177
    Height = 17
    Caption = #1054#1090#1082#1088#1099#1090#1100' '#1088#1072#1073#1086#1095#1080#1081' '#1089#1090#1086#1083
    TabOrder = 16
    Visible = False
    OnExit = CheckBoxDesktopExit
  end
  object OpenPictureDialog: TOpenPictureDialog
    DefaultExt = '*.ico'
    Filter = #1048#1082#1086#1085#1082#1080' (*.ico)|*.ico'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofNoNetworkButton, ofEnableSizing]
    Title = #1048#1082#1086#1085#1082#1072' '#1076#1083#1103' '#1090#1072#1088#1080#1092#1072
    Left = 2
    Top = 4
  end
end
