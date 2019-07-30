object FormFines: TFormFines
  Left = 285
  Top = 216
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1064#1090#1088#1072#1092#1099
  ClientHeight = 329
  ClientWidth = 549
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
    Width = 541
    Height = 289
    Style = bsRaised
  end
  object Bevel12: TBevel
    Left = 12
    Top = 228
    Width = 441
    Height = 57
    Shape = bsFrame
  end
  object LabelDescription: TLabel
    Left = 46
    Top = 238
    Width = 53
    Height = 13
    Alignment = taRightJustify
    Caption = #1054#1087#1080#1089#1072#1085#1080#1077':'
  end
  object LabelTime: TLabel
    Left = 22
    Top = 262
    Width = 117
    Height = 13
    Alignment = taRightJustify
    Caption = #1042#1088#1077#1084#1103' '#1096#1090#1088#1072#1092#1072' ('#1084#1080#1085#1091#1090'):'
  end
  object ButtonSave: TButton
    Left = 268
    Top = 300
    Width = 85
    Height = 25
    Caption = #1055#1088#1080#1085#1103#1090#1100
    ModalResult = 1
    TabOrder = 5
    OnClick = ButtonSaveClick
  end
  object ButtonCancel: TButton
    Left = 356
    Top = 300
    Width = 85
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 6
  end
  object ButtonHelp: TButton
    Left = 452
    Top = 300
    Width = 85
    Height = 25
    Caption = #1057#1087#1088#1072#1074#1082#1072
    Enabled = False
    TabOrder = 7
  end
  object ButtonAdd: TButton
    Left = 462
    Top = 234
    Width = 73
    Height = 22
    Caption = #1044#1086#1073#1072#1074#1080#1090#1100
    TabOrder = 3
    OnClick = ButtonAddClick
  end
  object ButtonDel: TButton
    Left = 462
    Top = 258
    Width = 73
    Height = 22
    Caption = #1059#1076#1072#1083#1080#1090#1100
    TabOrder = 4
    OnClick = ButtonDelClick
  end
  object ListViewFines: TListView
    Left = 8
    Top = 8
    Width = 533
    Height = 213
    AllocBy = 20
    Columns = <
      item
        Caption = #1054#1087#1080#1089#1072#1085#1080#1077
        Width = 300
      end
      item
        Alignment = taRightJustify
        Caption = #1042#1088#1077#1084#1103
        Width = 150
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SortType = stText
    TabOrder = 0
    ViewStyle = vsReport
    OnInsert = ListViewFinesInsert
    OnSelectItem = ListViewFinesSelectItem
  end
  object EditDescription: TEdit
    Left = 104
    Top = 234
    Width = 341
    Height = 21
    TabOrder = 1
    OnExit = EditDescriptionExit
  end
  object ComboBoxTime: TComboBox
    Left = 144
    Top = 258
    Width = 153
    Height = 21
    Style = csDropDownList
    TabOrder = 2
    OnClick = ComboBoxTimeClick
  end
end
