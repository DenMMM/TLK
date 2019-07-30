object FormTariffTimes: TFormTariffTimes
  Left = 269
  Top = 195
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  ClientHeight = 345
  ClientWidth = 585
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
  object Bevel2: TBevel
    Left = 4
    Top = 4
    Width = 577
    Height = 305
    Style = bsRaised
  end
  object LabelTimeTypeText: TLabel
    Left = 22
    Top = 222
    Width = 22
    Height = 13
    Alignment = taRightJustify
    Caption = 'Тип:'
  end
  object LabelBeginText: TLabel
    Left = 34
    Top = 250
    Width = 10
    Height = 13
    Alignment = taRightJustify
    Caption = 'С:'
  end
  object LabelEndText: TLabel
    Left = 146
    Top = 250
    Width = 18
    Height = 13
    Alignment = taRightJustify
    Caption = 'До:'
  end
  object LabelSizeText: TLabel
    Left = 266
    Top = 250
    Width = 17
    Height = 13
    Alignment = taRightJustify
    Caption = 'На:'
  end
  object LabelCostText: TLabel
    Left = 386
    Top = 250
    Width = 58
    Height = 13
    Alignment = taRightJustify
    Caption = 'Стоимость:'
  end
  object ListViewTimes: TListView
    Left = 8
    Top = 8
    Width = 569
    Height = 205
    Columns = <
      item
        Width = 0
      end
      item
        Caption = 'С'
        Width = 90
      end
      item
        Caption = 'До'
        Width = 90
      end
      item
        Caption = 'Время'
        Width = 140
      end
      item
        Caption = 'Стоимость'
        Width = 140
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnCompare = ListViewTimesCompare
    OnDeletion = ListViewTimesDeletion
    OnInsert = ListViewTimesInsert
    OnSelectItem = ListViewTimesSelectItem
  end
  object ComboBoxType: TComboBox
    Left = 48
    Top = 218
    Width = 149
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
    OnClick = ComboBoxTypeClick
    Items.Strings = (
      'За каждый час'
      '"Плавающий" пакет'
      'Пакет в интервале времени')
  end
  object EditCost: TEdit
    Left = 448
    Top = 246
    Width = 125
    Height = 21
    TabOrder = 8
    OnExit = EditCostExit
  end
  object ComboBoxBeginH: TComboBox
    Left = 48
    Top = 246
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 2
    OnClick = ComboBoxBeginHClick
  end
  object ComboBoxBeginM: TComboBox
    Left = 91
    Top = 246
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 3
    OnClick = ComboBoxBeginHClick
  end
  object ComboBoxEndH: TComboBox
    Left = 168
    Top = 246
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 4
    OnClick = ComboBoxEndHClick
  end
  object ComboBoxEndM: TComboBox
    Left = 215
    Top = 246
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 5
    OnClick = ComboBoxEndHClick
  end
  object ComboBoxSizeH: TComboBox
    Left = 287
    Top = 246
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
    OnClick = ComboBoxSizeHClick
  end
  object ComboBoxSizeM: TComboBox
    Left = 330
    Top = 246
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 7
    OnClick = ComboBoxSizeHClick
  end
  object ButtonSave: TButton
    Left = 304
    Top = 316
    Width = 85
    Height = 25
    Caption = 'Принять'
    ModalResult = 1
    TabOrder = 11
    OnClick = ButtonSaveClick
  end
  object ButtonCancel: TButton
    Left = 392
    Top = 316
    Width = 85
    Height = 25
    Caption = 'Отмена'
    ModalResult = 2
    TabOrder = 12
  end
  object ButtonHelp: TButton
    Left = 488
    Top = 316
    Width = 85
    Height = 25
    Caption = 'Справка'
    Enabled = False
    TabOrder = 13
  end
  object ButtonAdd: TButton
    Left = 422
    Top = 278
    Width = 73
    Height = 22
    Caption = 'Добавить'
    TabOrder = 9
    OnClick = ButtonAddClick
  end
  object ButtonDel: TButton
    Left = 498
    Top = 278
    Width = 73
    Height = 22
    Caption = 'Удалить'
    TabOrder = 10
    OnClick = ButtonDelClick
  end
end
