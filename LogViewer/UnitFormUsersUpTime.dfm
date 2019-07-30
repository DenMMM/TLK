object FormUsersUpTime: TFormUsersUpTime
  Left = 214
  Top = 187
  Width = 743
  Height = 472
  Caption = 'Смены'
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
        Caption = 'Пользователь'
        Width = 250
      end
      item
        Alignment = taRightJustify
        Caption = 'Начало смены'
        Width = 120
      end
      item
        Alignment = taRightJustify
        Caption = 'Окончание'
        Width = 120
      end
      item
        Alignment = taRightJustify
        Caption = 'Длительность'
        Width = 100
      end
      item
        Alignment = taRightJustify
        Caption = 'Выручка'
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
