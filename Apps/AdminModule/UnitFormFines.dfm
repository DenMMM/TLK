object FormFines: TFormFines
  Left = 285
  Top = 216
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Штрафы'
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
    Caption = 'Описание:'
  end
  object LabelTime: TLabel
    Left = 22
    Top = 262
    Width = 117
    Height = 13
    Alignment = taRightJustify
    Caption = 'Время штрафа (минут):'
  end
  object ButtonSave: TButton
    Left = 268
    Top = 300
    Width = 85
    Height = 25
    Caption = 'Принять'
    ModalResult = 1
    TabOrder = 5
    OnClick = ButtonSaveClick
  end
  object ButtonCancel: TButton
    Left = 356
    Top = 300
    Width = 85
    Height = 25
    Caption = 'Отмена'
    ModalResult = 2
    TabOrder = 6
  end
  object ButtonHelp: TButton
    Left = 452
    Top = 300
    Width = 85
    Height = 25
    Caption = 'Справка'
    Enabled = False
    TabOrder = 7
  end
  object ButtonAdd: TButton
    Left = 462
    Top = 234
    Width = 73
    Height = 22
    Caption = 'Добавить'
    TabOrder = 3
    OnClick = ButtonAddClick
  end
  object ButtonDel: TButton
    Left = 462
    Top = 258
    Width = 73
    Height = 22
    Caption = 'Удалить'
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
        Caption = 'Описание'
        Width = 300
      end
      item
        Alignment = taRightJustify
        Caption = 'Время'
        Width = 150
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SortType = stText
    TabOrder = 0
    ViewStyle = vsReport
    OnDeletion = ListViewFinesDeletion
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
    ItemHeight = 13
    TabOrder = 2
    OnClick = ComboBoxTimeClick
  end
end
