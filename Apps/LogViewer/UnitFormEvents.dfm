object FormEvents: TFormEvents
  Left = 178
  Top = 217
  Width = 741
  Height = 491
  Caption = #1057#1086#1073#1099#1090#1080#1103
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsMDIChild
  OldCreateOrder = False
  Position = poDefaultPosOnly
  Visible = True
  OnClose = FormClose
  OnShow = FormShow
  DesignSize = (
    733
    464)
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel2: TBevel
    Left = 2
    Top = 2
    Width = 729
    Height = 42
    Anchors = [akLeft, akTop, akRight]
    Shape = bsFrame
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
  object Panel: TPanel
    Left = 0
    Top = 47
    Width = 733
    Height = 417
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderWidth = 3
    TabOrder = 0
    object Splitter: TSplitter
      Left = 4
      Top = 149
      Width = 725
      Height = 3
      Cursor = crVSplit
      Align = alTop
      MinSize = 120
    end
    object PanelState: TPanel
      Left = 4
      Top = 4
      Width = 725
      Height = 145
      Align = alTop
      BorderWidth = 2
      TabOrder = 0
      DesignSize = (
        725
        145)
      object ListViewComputers: TListView
        Left = 3
        Top = 3
        Width = 719
        Height = 118
        Align = alTop
        AllocBy = 100
        Anchors = [akLeft, akTop, akRight, akBottom]
        BiDiMode = bdLeftToRight
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
            Caption = #1057#1086#1089#1090#1086#1103#1085#1080#1077
            Width = 130
          end
          item
            Caption = #1053#1072#1079#1074#1072#1085#1080#1077' '#1090#1072#1088#1080#1092#1072
            Width = 130
          end
          item
            Caption = #1042#1088#1077#1084#1103' '#1088#1072#1073#1086#1090#1099
            Width = 90
          end
          item
            Caption = #1054#1089#1090#1072#1083#1086#1089#1100
            Width = 70
          end
          item
            Caption = #1044#1086
            Width = 70
          end
          item
            Caption = #1042#1088#1077#1084#1103' '#1096#1090#1088#1072#1092#1072
            Width = 90
          end
          item
            Caption = #1054#1089#1090#1072#1083#1086#1089#1100
            Width = 70
          end>
        HideSelection = False
        ReadOnly = True
        RowSelect = True
        ParentBiDiMode = False
        SmallImages = FormMain.ImageList
        SortType = stData
        TabOrder = 0
        ViewStyle = vsReport
        OnInsert = ListViewComputersInsert
      end
      object CheckBoxCompAUpd: TCheckBox
        Left = 12
        Top = 123
        Width = 169
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = #1054#1073#1085#1086#1074#1083#1103#1090#1100' '#1072#1074#1090#1086#1084#1072#1090#1080#1095#1077#1089#1082#1080
        TabOrder = 1
        OnClick = CheckBoxCompAUpdClick
      end
      object ButtonCompUpd: TButton
        Left = 188
        Top = 123
        Width = 75
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = #1054#1073#1085#1086#1074#1080#1090#1100
        TabOrder = 2
        OnClick = ButtonCompUpdClick
      end
    end
    object ListViewEvents: TListView
      Left = 4
      Top = 152
      Width = 725
      Height = 261
      Align = alClient
      Columns = <
        item
          Width = 0
        end
        item
          Alignment = taRightJustify
          Caption = #1050#1086#1075#1076#1072
          Width = 120
        end
        item
          Alignment = taRightJustify
          Caption = #8470
        end
        item
          Caption = #1058#1072#1088#1080#1092'/'#1086#1087#1077#1088#1072#1094#1080#1103
          Width = 280
        end
        item
          Alignment = taRightJustify
          Caption = #1042#1088#1077#1084#1103
          Width = 140
        end
        item
          Alignment = taRightJustify
          Caption = #1054#1087#1083#1072#1095#1077#1085#1086
          Width = 100
        end>
      HideSelection = False
      MultiSelect = True
      ReadOnly = True
      RowSelect = True
      TabOrder = 1
      ViewStyle = vsReport
      OnColumnClick = ListViewEventsColumnClick
      OnCompare = ListViewEventsCompare
      OnSelectItem = ListViewEventsSelectItem
    end
  end
end
