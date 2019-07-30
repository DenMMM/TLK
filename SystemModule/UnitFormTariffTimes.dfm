object FormTariffTimes: TFormTariffTimes
  Left = 303
  Top = 190
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  ClientHeight = 289
  ClientWidth = 511
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
    Left = 6
    Top = 6
    Width = 499
    Height = 253
    Style = bsRaised
  end
  object LabelTimeTypeText: TLabel
    Left = 14
    Top = 182
    Width = 22
    Height = 13
    Alignment = taRightJustify
    Caption = 'Тип:'
  end
  object Bevel1: TBevel
    Left = 204
    Top = 178
    Width = 289
    Height = 21
    Style = bsRaised
  end
  object LabelDaysText: TLabel
    Left = 176
    Top = 181
    Width = 24
    Height = 13
    Alignment = taRightJustify
    Caption = 'Дни:'
  end
  object LabelBeginTimeText: TLabel
    Left = 14
    Top = 210
    Width = 10
    Height = 13
    Alignment = taRightJustify
    Caption = 'С:'
  end
  object LabelEndTimeText: TLabel
    Left = 118
    Top = 210
    Width = 18
    Height = 13
    Alignment = taRightJustify
    Caption = 'До:'
  end
  object LabelSizeTimeText: TLabel
    Left = 230
    Top = 210
    Width = 17
    Height = 13
    Alignment = taRightJustify
    Caption = 'На:'
  end
  object LabelCostText: TLabel
    Left = 344
    Top = 210
    Width = 58
    Height = 13
    Alignment = taRightJustify
    Caption = 'Стоимость:'
  end
  object ListViewTimesAndCosts: TListView
    Left = 10
    Top = 10
    Width = 491
    Height = 161
    Columns = <
      item
        Width = 0
      end
      item
        Caption = 'С'
      end
      item
        Caption = 'До'
      end
      item
        Caption = 'Время'
        Width = 90
      end
      item
        Caption = 'Дни'
        Width = 150
      end
      item
        Caption = 'Стоимость'
        Width = 120
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnChange = ListViewTimesAndCostsChange
  end
  object ComboBoxTimeType: TComboBox
    Left = 40
    Top = 178
    Width = 129
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
    OnChange = ComboBoxTimeTypeChange
    Items.Strings = (
      'За каждый час'
      'На ЧЧ час. ММ мин.'
      'C ЧЧ:ММ до ЧЧ:ММ')
  end
  object CheckBoxDayMo: TCheckBox
    Left = 208
    Top = 180
    Width = 37
    Height = 17
    Caption = 'Пн.'
    TabOrder = 9
    OnClick = CheckBoxDayMoClick
  end
  object CheckBoxDayTu: TCheckBox
    Left = 248
    Top = 180
    Width = 37
    Height = 17
    Caption = 'Вт.'
    TabOrder = 10
    OnClick = CheckBoxDayMoClick
  end
  object CheckBoxDayWe: TCheckBox
    Left = 288
    Top = 180
    Width = 37
    Height = 17
    Caption = 'Ср.'
    TabOrder = 11
    OnClick = CheckBoxDayMoClick
  end
  object CheckBoxDayTh: TCheckBox
    Left = 328
    Top = 180
    Width = 37
    Height = 17
    Caption = 'Чт.'
    TabOrder = 12
    OnClick = CheckBoxDayMoClick
  end
  object CheckBoxDayFr: TCheckBox
    Left = 368
    Top = 180
    Width = 37
    Height = 17
    Caption = 'Пт.'
    TabOrder = 13
    OnClick = CheckBoxDayMoClick
  end
  object CheckBoxDaySa: TCheckBox
    Left = 408
    Top = 180
    Width = 37
    Height = 17
    Caption = 'Сб.'
    TabOrder = 14
    OnClick = CheckBoxDayMoClick
  end
  object CheckBoxDaySu: TCheckBox
    Left = 448
    Top = 180
    Width = 37
    Height = 17
    Caption = 'Вс.'
    TabOrder = 15
    OnClick = CheckBoxDayMoClick
  end
  object EditCost: TEdit
    Left = 404
    Top = 206
    Width = 97
    Height = 21
    TabOrder = 8
    OnExit = EditCostExit
  end
  object BitBtnSave: TBitBtn
    Left = 96
    Top = 263
    Width = 105
    Height = 24
    Caption = 'Сохранить'
    TabOrder = 20
    Kind = bkOK
  end
  object BitBtnCancel: TBitBtn
    Left = 204
    Top = 263
    Width = 105
    Height = 24
    Caption = 'Отмена'
    TabOrder = 21
    Kind = bkCancel
  end
  object ComboBoxBeginTimeH: TComboBox
    Left = 28
    Top = 206
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 2
    OnChange = ComboBoxBeginTimeHChange
  end
  object ComboBoxBeginTimeM: TComboBox
    Left = 71
    Top = 206
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 3
    OnChange = ComboBoxBeginTimeHChange
  end
  object ComboBoxEndTimeH: TComboBox
    Left = 140
    Top = 206
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 4
    OnChange = ComboBoxEndTimeHChange
  end
  object ComboBoxEndTimeM: TComboBox
    Left = 183
    Top = 206
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 5
    OnChange = ComboBoxEndTimeHChange
  end
  object ComboBoxSizeTimeH: TComboBox
    Left = 251
    Top = 206
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
    OnChange = ComboBoxSizeTimeHChange
  end
  object ComboBoxSizeTimeM: TComboBox
    Left = 294
    Top = 206
    Width = 41
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 7
    OnChange = ComboBoxSizeTimeHChange
  end
  object BitBtnHelp: TBitBtn
    Left = 312
    Top = 263
    Width = 105
    Height = 24
    Caption = 'Справка'
    TabOrder = 22
    Kind = bkHelp
  end
  object CheckBoxEnableDesc: TCheckBox
    Left = 28
    Top = 232
    Width = 189
    Height = 17
    Caption = 'Разрешить запись на это время'
    TabOrder = 16
    OnClick = CheckBoxEnableDescClick
  end
  object ButtonAdd: TButton
    Left = 275
    Top = 232
    Width = 73
    Height = 22
    Caption = 'Добавить'
    TabOrder = 17
    OnClick = ButtonAddClick
  end
  object ButtonDelete: TButton
    Left = 347
    Top = 232
    Width = 73
    Height = 22
    Caption = 'Удалить'
    TabOrder = 18
    OnClick = ButtonDeleteClick
  end
  object ButtonChange: TButton
    Left = 423
    Top = 232
    Width = 73
    Height = 22
    Caption = 'Изменить'
    TabOrder = 19
    OnClick = ButtonChangeClick
  end
end
