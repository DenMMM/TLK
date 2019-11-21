object FormUsersUpTime: TFormUsersUpTime
  Left = 214
  Top = 187
  Caption = #1057#1084#1077#1085#1099
  ClientHeight = 436
  ClientWidth = 727
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsMDIChild
  OldCreateOrder = False
  Visible = True
  OnClose = FormClose
  DesignSize = (
    727
    436)
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel2: TBevel
    Left = 2
    Top = 2
    Width = 731
    Height = 42
    Anchors = [akLeft, akTop, akRight]
    Shape = bsFrame
  end
  object Bevel1: TBevel
    Left = 0
    Top = 47
    Width = 735
    Height = 398
    Anchors = [akLeft, akTop, akRight, akBottom]
    Style = bsRaised
  end
  object Label1: TLabel
    Left = 17
    Top = 8
    Width = 95
    Height = 13
    Alignment = taRightJustify
    Caption = #1042#1099#1073#1088#1072#1085#1085#1086#1077' '#1074#1088#1077#1084#1103':'
  end
  object LabelSelTime: TLabel
    Left = 116
    Top = 8
    Width = 97
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = '0,00'
  end
  object LabelSelMoney: TLabel
    Left = 116
    Top = 24
    Width = 97
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = '0,00'
  end
  object Label2: TLabel
    Left = 75
    Top = 24
    Width = 37
    Height = 13
    Alignment = taRightJustify
    Caption = #1057#1091#1084#1084#1072':'
  end
  object ListViewUpTimes: TListView
    Left = 4
    Top = 51
    Width = 727
    Height = 390
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <
      item
        Width = 0
      end
      item
        Caption = #1055#1086#1083#1100#1079#1086#1074#1072#1090#1077#1083#1100
        Width = 250
      end
      item
        Alignment = taRightJustify
        Caption = #1053#1072#1095#1072#1083#1086' '#1089#1084#1077#1085#1099
        Width = 120
      end
      item
        Alignment = taRightJustify
        Caption = #1054#1082#1086#1085#1095#1072#1085#1080#1077
        Width = 120
      end
      item
        Alignment = taRightJustify
        Caption = #1044#1083#1080#1090#1077#1083#1100#1085#1086#1089#1090#1100
        Width = 100
      end
      item
        Alignment = taRightJustify
        Caption = #1042#1099#1088#1091#1095#1082#1072
        Width = 100
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnColumnClick = ListViewUpTimesColumnClick
    OnCompare = ListViewUpTimesCompare
    OnSelectItem = ListViewUpTimesSelectItem
  end
end
