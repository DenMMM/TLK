object FormOrder: TFormOrder
  Left = 363
  Top = 263
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Оформление заказа'
  ClientHeight = 339
  ClientWidth = 513
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
  object Bevel1: TBevel
    Left = 6
    Top = 6
    Width = 501
    Height = 327
    Style = bsRaised
  end
  object BevelBaseButtonsOut: TBevel
    Left = 390
    Top = 176
    Width = 109
    Height = 119
    Style = bsRaised
  end
  object Bevel10: TBevel
    Left = 108
    Top = 280
    Width = 273
    Height = 29
    Shape = bsFrame
  end
  object Bevel8: TBevel
    Left = 108
    Top = 176
    Width = 273
    Height = 29
    Shape = bsFrame
  end
  object Bevel2: TBevel
    Left = 240
    Top = 151
    Width = 260
    Height = 18
  end
  object LabelCostText: TLabel
    Left = 235
    Top = 184
    Width = 58
    Height = 13
    Alignment = taRightJustify
    Caption = 'Стоимость:'
  end
  object LabelRemaindTimeText: TLabel
    Left = 239
    Top = 288
    Width = 45
    Height = 13
    Alignment = taRightJustify
    Caption = 'Остаток:'
  end
  object BevelBaseButtonsIn: TBevel
    Left = 392
    Top = 178
    Width = 105
    Height = 115
  end
  object LabelOrderCost: TLabel
    Left = 360
    Top = 153
    Width = 137
    Height = 13
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Bevel5: TBevel
    Left = 298
    Top = 181
    Width = 78
    Height = 19
  end
  object LabelCost: TLabel
    Left = 300
    Top = 184
    Width = 73
    Height = 13
    AutoSize = False
  end
  object Bevel6: TBevel
    Left = 288
    Top = 285
    Width = 89
    Height = 19
  end
  object LabelRemaindTime: TLabel
    Left = 292
    Top = 288
    Width = 81
    Height = 13
    AutoSize = False
    Caption = '00 час. 00 мин.'
  end
  object Bevel7: TBevel
    Left = 384
    Top = 127
    Width = 116
    Height = 19
  end
  object LabelRealSizeTime: TLabel
    Left = 388
    Top = 130
    Width = 107
    Height = 13
    AutoSize = False
  end
  object Bevel9: TBevel
    Left = 108
    Top = 228
    Width = 273
    Height = 29
    Shape = bsFrame
  end
  object LabelSubscriptionNumberText: TLabel
    Left = 115
    Top = 288
    Width = 37
    Height = 13
    Alignment = taRightJustify
    Caption = 'Номер:'
  end
  object Label1: TLabel
    Left = 244
    Top = 153
    Width = 112
    Height = 13
    Caption = 'Общая стоимость:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object ComboBoxTariffTime: TComboBox
    Left = 240
    Top = 126
    Width = 141
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 2
    OnChange = ComboBoxTariffTimeChange
  end
  object EditSubscriptionNumber: TEdit
    Left = 156
    Top = 284
    Width = 77
    Height = 21
    TabOrder = 7
    Text = '000000000'
  end
  object ListViewOrderComputers: TListView
    Left = 10
    Top = 10
    Width = 493
    Height = 111
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
        Caption = 'Тип тарифа'
        Width = 130
      end
      item
        Caption = 'Время'
        Width = 90
      end
      item
        Caption = 'Скидка (%)'
        Width = 70
      end
      item
        Caption = 'Стоимость'
        Width = 130
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SmallImages = FormMain.ImageListIcons
    TabOrder = 12
    ViewStyle = vsReport
    OnDeletion = ListViewOrderComputersDeletion
    OnEnter = ListViewOrderComputersEnter
    OnInsert = ListViewOrderComputersInsert
    OnSelectItem = ListViewOrderComputersSelectItem
  end
  object ListViewComputersForOrder: TListView
    Left = 10
    Top = 124
    Width = 81
    Height = 205
    Columns = <
      item
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    ShowColumnHeaders = False
    SmallImages = FormMain.ImageListIcons
    TabOrder = 0
    ViewStyle = vsReport
    OnEnter = ListViewComputersForOrderEnter
    OnSelectItem = ListViewComputersForOrderSelectItem
  end
  object BitBtnClose: TBitBtn
    Left = 392
    Top = 300
    Width = 105
    Height = 25
    Caption = 'Закрыть'
    TabOrder = 13
    Kind = bkCancel
  end
  object ComboBoxTariffName: TComboBox
    Left = 96
    Top = 126
    Width = 141
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
    OnChange = ComboBoxTariffNameChange
  end
  object ProgressBarDialogUseTime: TProgressBar
    Left = 96
    Top = 316
    Width = 285
    Height = 12
    Min = 0
    Max = 180
    TabOrder = 14
  end
  object RadioButtonCash: TRadioButton
    Left = 96
    Top = 156
    Width = 121
    Height = 17
    Caption = 'Оплата наличными'
    TabOrder = 3
    OnClick = RadioButtonCashClick
  end
  object RadioButtonDesc: TRadioButton
    Left = 96
    Top = 208
    Width = 149
    Height = 17
    Caption = 'По записи на это время'
    Enabled = False
    TabOrder = 4
    OnClick = RadioButtonCashClick
  end
  object RadioButtonSubs: TRadioButton
    Left = 96
    Top = 260
    Width = 81
    Height = 17
    Caption = 'Абонемент'
    Enabled = False
    TabOrder = 5
    OnClick = RadioButtonCashClick
  end
  object ComboBoxDescNumber: TComboBox
    Left = 112
    Top = 232
    Width = 265
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
  end
  object ButtonAdd: TButton
    Left = 396
    Top = 182
    Width = 97
    Height = 25
    Caption = 'Добавить'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 8
    OnClick = ButtonAddClick
  end
  object ButtonDelete: TButton
    Left = 396
    Top = 234
    Width = 97
    Height = 25
    Caption = 'Удалить'
    TabOrder = 10
    OnClick = ButtonDeleteClick
  end
  object ButtonChange: TButton
    Left = 396
    Top = 206
    Width = 97
    Height = 25
    Caption = 'Изменить'
    TabOrder = 9
    OnClick = ButtonChangeClick
  end
  object ButtonRun: TButton
    Left = 396
    Top = 264
    Width = 97
    Height = 25
    Caption = 'Запустить'
    TabOrder = 11
    OnClick = ButtonRunClick
  end
  object TimerDialogLock: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = TimerDialogLockTimer
    Left = 472
    Top = 4
  end
end
