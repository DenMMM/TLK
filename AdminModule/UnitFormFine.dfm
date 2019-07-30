object FormFine: TFormFine
  Left = 314
  Top = 220
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Штрафование'
  ClientHeight = 299
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
    Height = 287
    Style = bsRaised
  end
  object Bevel6: TBevel
    Left = 141
    Top = 124
    Width = 240
    Height = 18
  end
  object Bevel3: TBevel
    Left = 108
    Top = 168
    Width = 273
    Height = 50
    Shape = bsFrame
  end
  object Bevel5: TBevel
    Left = 108
    Top = 244
    Width = 273
    Height = 29
    Shape = bsFrame
  end
  object LabelAllReturnManyText: TLabel
    Left = 145
    Top = 126
    Width = 91
    Height = 13
    Caption = 'Вернуть всего:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object LabelAllReturnMany: TLabel
    Left = 240
    Top = 126
    Width = 138
    Height = 13
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object BevelBaseButtonsOut: TBevel
    Left = 390
    Top = 136
    Width = 109
    Height = 119
    Style = bsRaised
  end
  object BevelBaseButtonsIn: TBevel
    Left = 392
    Top = 138
    Width = 105
    Height = 115
  end
  object CheckBoxEnableWaiting: TCheckBox
    Left = 114
    Top = 198
    Width = 99
    Height = 17
    Caption = 'С ожиданием'
    TabOrder = 4
    OnClick = CheckBoxEnableWaitingClick
  end
  object BitBtnClose: TBitBtn
    Left = 392
    Top = 260
    Width = 105
    Height = 25
    Cancel = True
    Caption = 'Закрыть'
    ModalResult = 2
    TabOrder = 11
    OnClick = BitBtnCloseClick
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      333333333333333333333333000033338833333333333333333F333333333333
      0000333911833333983333333388F333333F3333000033391118333911833333
      38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
      911118111118333338F3338F833338F3000033333911111111833333338F3338
      3333F8330000333333911111183333333338F333333F83330000333333311111
      8333333333338F3333383333000033333339111183333333333338F333833333
      00003333339111118333333333333833338F3333000033333911181118333333
      33338333338F333300003333911183911183333333383338F338F33300003333
      9118333911183333338F33838F338F33000033333913333391113333338FF833
      38F338F300003333333333333919333333388333338FFF830000333333333333
      3333333333333333333888330000333333333333333333333333333333333333
      0000}
    NumGlyphs = 2
  end
  object ComboBoxFineTime: TComboBox
    Left = 112
    Top = 172
    Width = 265
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 3
    OnChange = ComboBoxFineTimeChange
  end
  object RadioButtonCertainTime: TRadioButton
    Left = 96
    Top = 148
    Width = 169
    Height = 17
    Caption = 'Снять только часть времени'
    TabOrder = 1
    OnClick = RadioButtonCertainTimeClick
  end
  object RadioButtonAllTime: TRadioButton
    Left = 96
    Top = 224
    Width = 225
    Height = 17
    Caption = 'Снять все время и закрыть компьютер'
    TabOrder = 2
    OnClick = RadioButtonCertainTimeClick
  end
  object CheckBoxReturnMany: TCheckBox
    Left = 114
    Top = 250
    Width = 239
    Height = 17
    Caption = 'Вернуть деньги (приблизительную сумму)'
    Enabled = False
    TabOrder = 5
    OnClick = CheckBoxReturnManyClick
  end
  object ListViewFineComputers: TListView
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
        Caption = 'Штраф'
        Width = 90
      end
      item
        Caption = 'Ожидание'
      end
      item
        Caption = 'Вернуть'
        Width = 130
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SmallImages = FormMain.ImageListIcons
    TabOrder = 10
    ViewStyle = vsReport
    OnDeletion = ListViewFineComputersDeletion
    OnEnter = ListViewFineComputersEnter
    OnInsert = ListViewFineComputersInsert
  end
  object ListViewComputersForFine: TListView
    Left = 10
    Top = 124
    Width = 81
    Height = 165
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
    OnEnter = ListViewComputersForFineEnter
  end
  object ProgressBarDialogUseTime: TProgressBar
    Left = 96
    Top = 277
    Width = 285
    Height = 12
    Min = 0
    Max = 180
    TabOrder = 12
  end
  object ButtonAdd: TButton
    Left = 396
    Top = 142
    Width = 97
    Height = 25
    Caption = 'Добавить'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
    OnClick = ButtonAddClick
  end
  object ButtonChange: TButton
    Left = 396
    Top = 166
    Width = 97
    Height = 25
    Caption = 'Изменить'
    TabOrder = 7
    OnClick = ButtonChangeClick
  end
  object ButtonDelete: TButton
    Left = 396
    Top = 194
    Width = 97
    Height = 25
    Caption = 'Удалить'
    TabOrder = 8
    OnClick = ButtonDeleteClick
  end
  object ButtonFine: TButton
    Left = 396
    Top = 224
    Width = 97
    Height = 25
    Caption = 'Штрафовать'
    TabOrder = 9
    OnClick = ButtonFineClick
  end
  object TimerDialogLock: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = TimerDialogLockTimer
    Left = 472
    Top = 4
  end
end
