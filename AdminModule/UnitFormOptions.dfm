object FormOptions: TFormOptions
  Left = 305
  Top = 240
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Общие настройки'
  ClientHeight = 278
  ClientWidth = 461
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
    Width = 453
    Height = 237
    Style = bsRaised
  end
  object Label1: TLabel
    Left = 65
    Top = 52
    Width = 270
    Height = 13
    Alignment = taRightJustify
    Caption = 'Время использования диалогов расчета цен (минут):'
  end
  object Label2: TLabel
    Left = 221
    Top = 80
    Width = 114
    Height = 13
    Alignment = taRightJustify
    Caption = 'Точность расчета цен:'
  end
  object Label3: TLabel
    Left = 51
    Top = 116
    Width = 284
    Height = 13
    Alignment = taRightJustify
    Caption = 'Время для попадания под фильтр "Свободные" (минут):'
  end
  object Bevel1: TBevel
    Left = 12
    Top = 104
    Width = 437
    Height = 9
    Shape = bsTopLine
  end
  object Bevel2: TBevel
    Left = 12
    Top = 140
    Width = 437
    Height = 9
    Shape = bsTopLine
  end
  object Label4: TLabel
    Left = 132
    Top = 16
    Width = 147
    Height = 13
    Alignment = taRightJustify
    Caption = 'Период ведения файла лога:'
  end
  object Bevel3: TBevel
    Left = 12
    Top = 40
    Width = 437
    Height = 9
    Shape = bsTopLine
  end
  object ButtonSave: TButton
    Left = 180
    Top = 248
    Width = 85
    Height = 25
    Caption = 'Принять'
    ModalResult = 1
    TabOrder = 5
    OnClick = ButtonSaveClick
  end
  object ButtonCancel: TButton
    Left = 268
    Top = 248
    Width = 85
    Height = 25
    Caption = 'Отмена'
    ModalResult = 2
    TabOrder = 6
  end
  object ButtonHelp: TButton
    Left = 364
    Top = 248
    Width = 85
    Height = 25
    Caption = 'Справка'
    Enabled = False
    TabOrder = 7
  end
  object EditCostDialogTime: TEdit
    Left = 340
    Top = 48
    Width = 85
    Height = 21
    TabOrder = 1
    OnExit = EditCostDialogTimeExit
  end
  object EditCostPrecision: TEdit
    Left = 340
    Top = 76
    Width = 85
    Height = 21
    TabOrder = 2
    OnExit = EditCostPrecisionExit
  end
  object EditFilterFreeTime: TEdit
    Left = 340
    Top = 112
    Width = 85
    Height = 21
    TabOrder = 3
    OnExit = EditFilterFreeTimeExit
  end
  object CheckBoxRightPause: TCheckBox
    Left = 20
    Top = 152
    Width = 385
    Height = 17
    Caption = 'Разрешить администраторам приостанавливать компьютеры'
    TabOrder = 4
    OnExit = CheckBoxRightPauseExit
  end
  object ComboBoxLogPeriod: TComboBox
    Left = 284
    Top = 12
    Width = 141
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnExit = ComboBoxLogPeriodExit
    Items.Strings = (
      'Сутки'
      'Неделя'
      'Месяц'
      'Квартал'
      'Год')
  end
end
