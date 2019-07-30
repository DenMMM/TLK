object FormFine: TFormFine
  Left = 282
  Top = 227
  BorderIcons = []
  BorderStyle = bsSizeToolWin
  Caption = #1064#1090#1088#1072#1092#1086#1074#1072#1085#1080#1077
  ClientHeight = 280
  ClientWidth = 457
  Color = clBtnFace
  Constraints.MaxHeight = 500
  Constraints.MaxWidth = 473
  Constraints.MinHeight = 316
  Constraints.MinWidth = 473
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  DesignSize = (
    457
    280)
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel2: TBevel
    Left = 4
    Top = 4
    Width = 457
    Height = 281
    Anchors = [akLeft, akTop, akBottom]
    Style = bsRaised
  end
  object Bevel4: TBevel
    Left = 12
    Top = 168
    Width = 441
    Height = 85
    Anchors = [akLeft, akBottom]
    Shape = bsFrame
  end
  object Bevel8: TBevel
    Left = 124
    Top = 259
    Width = 217
    Height = 18
    Anchors = [akLeft, akBottom]
  end
  object Label1: TLabel
    Left = 128
    Top = 261
    Width = 91
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = #1042#1077#1088#1085#1091#1090#1100' '#1074#1089#1077#1075#1086':'
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object LabelAllReturn: TLabel
    Left = 224
    Top = 261
    Width = 113
    Height = 13
    Alignment = taRightJustify
    Anchors = [akLeft, akBottom]
    AutoSize = False
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 27
    Top = 180
    Width = 37
    Height = 13
    Alignment = taRightJustify
    Anchors = [akLeft, akBottom]
    Caption = #1064#1090#1088#1072#1092':'
  end
  object CheckBoxWarn: TCheckBox
    Left = 76
    Top = 202
    Width = 151
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = #1058#1086#1083#1100#1082#1086' '#1087#1088#1077#1076#1091#1087#1088#1077#1078#1076#1077#1085#1080#1077
    TabOrder = 3
    OnClick = ComboBoxTimeClick
  end
  object ListViewFines: TListView
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
        Caption = #1064#1090#1088#1072#1092
        Width = 170
      end
      item
        Alignment = taRightJustify
        Caption = #1042#1088#1077#1084#1103
        Width = 70
      end
      item
        Caption = #1054#1078#1080#1076#1072#1085#1080#1077
      end
      item
        Caption = #1042#1077#1088#1085#1091#1090#1100
        Width = 90
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnInsert = ListViewFinesInsert
  end
  object BitBtn1: TBitBtn
    Left = 348
    Top = 256
    Width = 105
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = #1047#1072#1082#1088#1099#1090#1100
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 5
  end
  object BitBtnFine: TBitBtn
    Left = 348
    Top = 138
    Width = 105
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = #1064#1090#1088#1072#1092#1086#1074#1072#1090#1100
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
    TabOrder = 4
    OnClick = BitBtnFineClick
  end
  object CheckBoxWait: TCheckBox
    Left = 344
    Top = 178
    Width = 95
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = #1057' '#1086#1078#1080#1076#1072#1085#1080#1077#1084
    TabOrder = 2
    OnClick = ComboBoxTimeClick
  end
  object ComboBoxTime: TComboBox
    Left = 68
    Top = 176
    Width = 265
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akBottom]
    TabOrder = 1
    OnClick = ComboBoxTimeClick
  end
end
