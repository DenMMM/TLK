object FormMain: TFormMain
  Left = 209
  Top = 134
  Width = 734
  Height = 521
  Caption = '�������� �����'
  Color = clBtnFace
  Constraints.MinHeight = 226
  Constraints.MinWidth = 657
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 4
    Top = 4
    Width = 718
    Height = 109
    Anchors = [akLeft, akTop, akRight]
    Shape = bsFrame
  end
  object Label1: TLabel
    Left = 287
    Top = 16
    Width = 25
    Height = 13
    Alignment = taRightJustify
    Caption = '���:'
  end
  object Label2: TLabel
    Left = 251
    Top = 40
    Width = 61
    Height = 13
    Alignment = taRightJustify
    Caption = '���������:'
  end
  object Label3: TLabel
    Left = 46
    Top = 12
    Width = 46
    Height = 13
    Alignment = taRightJustify
    Caption = '�������:'
  end
  object LabelOpenTime: TLabel
    Left = 96
    Top = 12
    Width = 133
    Height = 13
    AutoSize = False
    Caption = '????'
  end
  object Label5: TLabel
    Left = 51
    Top = 28
    Width = 41
    Height = 13
    Alignment = taRightJustify
    Caption = '������:'
  end
  object LabelCloseTime: TLabel
    Left = 96
    Top = 28
    Width = 133
    Height = 13
    AutoSize = False
    Caption = '????'
  end
  object LabelDurationTime: TLabel
    Left = 16
    Top = 48
    Width = 76
    Height = 13
    Alignment = taRightJustify
    Caption = '������������:'
  end
  object LabelLengthTime: TLabel
    Left = 96
    Top = 48
    Width = 133
    Height = 13
    AutoSize = False
    Caption = '????'
  end
  object Bevel2: TBevel
    Left = 12
    Top = 68
    Width = 702
    Height = 5
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label9: TLabel
    Left = 13
    Top = 76
    Width = 99
    Height = 13
    Alignment = taRightJustify
    Caption = '���������� �����:'
  end
  object LabelPayTime: TLabel
    Left = 120
    Top = 76
    Width = 76
    Height = 13
    Caption = '00 ���. 00 ���.'
  end
  object Label11: TLabel
    Left = 75
    Top = 92
    Width = 37
    Height = 13
    Alignment = taRightJustify
    Caption = '�����:'
  end
  object LabelPayMany: TLabel
    Left = 120
    Top = 92
    Width = 54
    Height = 13
    Caption = '000 000,00'
  end
  object Label4: TLabel
    Left = 216
    Top = 92
    Width = 96
    Height = 13
    Alignment = taRightJustify
    Caption = '��������� �����:'
  end
  object LabelSelPayMany: TLabel
    Left = 316
    Top = 92
    Width = 54
    Height = 13
    Caption = '000 000,00'
  end
  object EditName: TEdit
    Left = 316
    Top = 12
    Width = 321
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 0
  end
  object EditJob: TEdit
    Left = 316
    Top = 36
    Width = 173
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 1
  end
  object ButtonSelect: TButton
    Left = 432
    Top = 80
    Width = 95
    Height = 25
    Caption = '�������...'
    TabOrder = 2
    OnClick = ButtonSelectClick
  end
  object BitBtnClose: TBitBtn
    Left = 609
    Top = 465
    Width = 105
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = '�������'
    TabOrder = 6
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
  object MemoReport: TMemo
    Left = 68
    Top = 4
    Width = 33
    Height = 29
    BorderStyle = bsNone
    ReadOnly = True
    TabOrder = 7
    Visible = False
    WordWrap = False
  end
  object ButtonSaveReport: TButton
    Left = 536
    Top = 80
    Width = 95
    Height = 25
    Caption = '���������...'
    TabOrder = 3
    OnClick = ButtonSaveReportClick
  end
  object PanelLog: TPanel
    Left = 4
    Top = 136
    Width = 717
    Height = 323
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderWidth = 3
    TabOrder = 5
    object SplitterLog: TSplitter
      Left = 4
      Top = 97
      Width = 709
      Height = 3
      Cursor = crVSplit
      Align = alTop
    end
    object ListViewEvents: TListView
      Left = 4
      Top = 100
      Width = 709
      Height = 219
      Align = alClient
      AllocBy = 250
      Columns = <
        item
          Width = 0
        end
        item
          Alignment = taRightJustify
          Caption = '�����'
          Width = 130
        end
        item
          Alignment = taRightJustify
          Caption = '���������'
          Width = 70
        end
        item
          Caption = '�����/��������'
          Width = 250
        end
        item
          Alignment = taRightJustify
          Caption = '�����'
          Width = 120
        end
        item
          Alignment = taRightJustify
          Caption = '��������'
          Width = 120
        end>
      MultiSelect = True
      ReadOnly = True
      RowSelect = True
      TabOrder = 1
      ViewStyle = vsReport
      OnColumnClick = ListViewEventsColumnClick
      OnDeletion = ListViewEventsDeletion
      OnInsert = ListViewEventsInsert
      OnSelectItem = ListViewEventsSelectItem
    end
    object ListViewFiles: TListView
      Left = 4
      Top = 4
      Width = 709
      Height = 93
      Align = alTop
      AllocBy = 250
      Columns = <
        item
          Width = 0
        end
        item
          Caption = '����'
          Width = 150
        end
        item
          Caption = '�������'
          Width = 170
        end
        item
          Caption = '�����'
          Width = 300
        end>
      HideSelection = False
      MultiSelect = True
      ReadOnly = True
      RowSelect = True
      TabOrder = 0
      ViewStyle = vsReport
      Visible = False
      OnSelectItem = ListViewFilesSelectItem
    end
  end
  object ButtonShowFiles: TButton
    Left = 624
    Top = 118
    Width = 95
    Height = 17
    Anchors = [akTop, akRight]
    Caption = '����� <</>>'
    Enabled = False
    TabOrder = 4
    OnClick = ButtonShowFilesClick
  end
  object OpenDialog: TOpenDialog
    Filter = '����� ����� TimeLocker'#39'� (*.tlg)|*.TLG|��� ����� (*.*)|*.*'
    Left = 4
    Top = 4
  end
  object SaveDialog: TSaveDialog
    DefaultExt = '*.txt'
    Filter = '��������� ����� (*.txt)|*.TXT|��� ����� (*.*)|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 36
    Top = 4
  end
end
