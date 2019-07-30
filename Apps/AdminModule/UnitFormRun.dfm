object FormRun: TFormRun
  Left = 338
  Top = 236
  Width = 474
  Height = 316
  BorderIcons = []
  BorderStyle = bsSizeToolWin
  Caption = #1047#1072#1087#1091#1089#1082' '#1082#1086#1084#1087#1100#1102#1090#1077#1088#1086#1074
  Color = clBtnFace
  Constraints.MaxHeight = 500
  Constraints.MaxWidth = 474
  Constraints.MinHeight = 316
  Constraints.MinWidth = 474
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  DesignSize = (
    466
    289)
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 4
    Top = 4
    Width = 457
    Height = 281
    Anchors = [akLeft, akTop, akBottom]
    Style = bsRaised
  end
  object Bevel3: TBevel
    Left = 12
    Top = 168
    Width = 441
    Height = 85
    Anchors = [akLeft, akBottom]
    Shape = bsFrame
  end
  object Bevel6: TBevel
    Left = 356
    Top = 225
    Width = 89
    Height = 19
    Anchors = [akLeft, akBottom]
  end
  object Bevel2: TBevel
    Left = 124
    Top = 259
    Width = 217
    Height = 18
    Anchors = [akLeft, akBottom]
  end
  object Label1: TLabel
    Left = 128
    Top = 261
    Width = 112
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = #1054#1073#1097#1072#1103' '#1089#1090#1086#1080#1084#1086#1089#1090#1100':'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object LabelAllCost: TLabel
    Left = 244
    Top = 261
    Width = 93
    Height = 13
    Alignment = taRightJustify
    Anchors = [akLeft, akBottom]
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object LabelRemaindTimeText: TLabel
    Left = 307
    Top = 228
    Width = 45
    Height = 13
    Alignment = taRightJustify
    Anchors = [akLeft, akBottom]
    Caption = #1054#1089#1090#1072#1090#1086#1082':'
    Enabled = False
  end
  object LabelRemaindTime: TLabel
    Left = 360
    Top = 228
    Width = 81
    Height = 13
    Anchors = [akLeft, akBottom]
    AutoSize = False
    Enabled = False
  end
  object Bevel4: TBevel
    Left = 12
    Top = 260
    Width = 105
    Height = 16
    Anchors = [akLeft, akBottom]
    Style = bsRaised
  end
  object ListViewComputers: TListView
    Left = 8
    Top = 8
    Width = 449
    Height = 125
    Anchors = [akLeft, akTop, akBottom]
    Columns = <
      item
        Width = 0
      end
      item
        Alignment = taRightJustify
        Caption = #8470
        Width = 40
      end
      item
        Caption = #1059#1089#1083#1091#1075#1072
        Width = 180
      end
      item
        Alignment = taRightJustify
        Caption = #1042#1088#1077#1084#1103
        Width = 110
      end
      item
        Alignment = taRightJustify
        Caption = #1057#1090#1086#1080#1084#1086#1089#1090#1100
        Width = 90
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SmallImages = FormMain.ImageListIcons
    TabOrder = 0
    ViewStyle = vsReport
    OnInsert = ListViewComputersInsert
    OnSelectItem = ListViewComputersSelectItem
  end
  object ComboBoxTariff: TComboBox
    Left = 12
    Top = 140
    Width = 181
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akBottom]
    ItemHeight = 13
    TabOrder = 1
    OnClick = ComboBoxTariffClick
  end
  object ComboBoxTime: TComboBox
    Left = 196
    Top = 140
    Width = 145
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akBottom]
    ItemHeight = 13
    MaxLength = 6
    TabOrder = 2
    OnChange = ComboBoxTimeChange
    OnClick = ComboBoxTimeClick
  end
  object BitBtnClose: TBitBtn
    Left = 348
    Top = 256
    Width = 105
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = #1047#1072#1082#1088#1099#1090#1100
    TabOrder = 9
    Kind = bkCancel
  end
  object RadioButtonCash: TRadioButton
    Left = 20
    Top = 180
    Width = 73
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = #1053#1072#1083#1080#1095#1085#1099#1077
    Checked = True
    TabOrder = 3
    TabStop = True
  end
  object RadioButtonDesc: TRadioButton
    Left = 20
    Top = 204
    Width = 77
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = #1055#1086' '#1079#1072#1087#1080#1089#1080
    Enabled = False
    TabOrder = 4
  end
  object ComboBoxDesc: TComboBox
    Left = 108
    Top = 200
    Width = 337
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akBottom]
    Color = clBtnFace
    Enabled = False
    ItemHeight = 13
    TabOrder = 6
  end
  object RadioButtonSubs: TRadioButton
    Left = 20
    Top = 228
    Width = 81
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = #1040#1073#1086#1085#1077#1084#1077#1085#1090
    Enabled = False
    TabOrder = 5
  end
  object ComboBoxSubs: TComboBox
    Left = 108
    Top = 224
    Width = 193
    Height = 21
    Anchors = [akLeft, akBottom]
    Color = clBtnFace
    Enabled = False
    ItemHeight = 13
    TabOrder = 7
  end
  object ProgressBarTime: TProgressBar
    Left = 15
    Top = 263
    Width = 99
    Height = 10
    Anchors = [akLeft, akBottom]
    Min = 0
    Max = 180
    Smooth = True
    TabOrder = 10
  end
  object BitBtnRun: TBitBtn
    Left = 348
    Top = 138
    Width = 105
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = #1047#1072#1087#1091#1089#1090#1080#1090#1100
    TabOrder = 8
    OnClick = BitBtnRunClick
    Glyph.Data = {
      F2010000424DF201000000000000760000002800000024000000130000000100
      0400000000007C01000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333334433333
      3333333333388F3333333333000033334224333333333333338338F333333333
      0000333422224333333333333833338F33333333000033422222243333333333
      83333338F3333333000034222A22224333333338F33F33338F33333300003222
      A2A2224333333338F383F3338F33333300003A2A222A222433333338F8333F33
      38F33333000034A22222A22243333338833333F3338F333300004222A2222A22
      2433338F338F333F3338F3330000222A3A2224A22243338F3838F338F3338F33
      0000A2A333A2224A2224338F83338F338F3338F300003A33333A2224A2224338
      333338F338F3338F000033333333A2224A2243333333338F338F338F00003333
      33333A2224A2233333333338F338F83300003333333333A2224A333333333333
      8F338F33000033333333333A222433333333333338F338F30000333333333333
      A224333333333333338F38F300003333333333333A223333333333333338F8F3
      000033333333333333A3333333333333333383330000}
    NumGlyphs = 2
  end
  object TimerDialogLock: TTimer
    Enabled = False
    OnTimer = TimerDialogLockTimer
    Left = 4
    Top = 4
  end
end
