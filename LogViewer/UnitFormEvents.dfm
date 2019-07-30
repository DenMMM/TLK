object FormEvents: TFormEvents
  Left = 178
  Top = 217
  Width = 741
  Height = 491
  Caption = 'События'
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
    Caption = 'Выбранное время:'
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
    Caption = 'Сумма:'
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
            Caption = '№'
            Width = 40
          end
          item
            Caption = 'Состояние'
            Width = 130
          end
          item
            Caption = 'Название тарифа'
            Width = 130
          end
          item
            Caption = 'Время работы'
            Width = 90
          end
          item
            Caption = 'Осталось'
            Width = 70
          end
          item
            Caption = 'До'
            Width = 70
          end
          item
            Caption = 'Время штрафа'
            Width = 90
          end
          item
            Caption = 'Осталось'
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
        Caption = 'Обновлять автоматически'
        TabOrder = 1
        OnClick = CheckBoxCompAUpdClick
      end
      object ButtonCompUpd: TButton
        Left = 188
        Top = 123
        Width = 75
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Обновить'
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
          Caption = 'Когда'
          Width = 120
        end
        item
          Alignment = taRightJustify
          Caption = '№'
        end
        item
          Caption = 'Тариф/операция'
          Width = 280
        end
        item
          Alignment = taRightJustify
          Caption = 'Время'
          Width = 140
        end
        item
          Alignment = taRightJustify
          Caption = 'Оплачено'
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
