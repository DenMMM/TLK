object FormFines: TFormFines
  Left = 359
  Top = 213
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Штрафы'
  ClientHeight = 227
  ClientWidth = 371
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
  object Bevel1: TBevel
    Left = 6
    Top = 6
    Width = 359
    Height = 191
    Style = bsRaised
  end
  object LabelTimeText: TLabel
    Left = 174
    Top = 122
    Width = 117
    Height = 13
    Alignment = taRightJustify
    Caption = 'Время штрафа (минут):'
  end
  object LabelCommentText: TLabel
    Left = 59
    Top = 146
    Width = 73
    Height = 13
    Alignment = taRightJustify
    Caption = 'Комментарий:'
  end
  object ListViewFines: TListView
    Left = 10
    Top = 10
    Width = 351
    Height = 103
    Columns = <
      item
        Width = 0
      end
      item
        Alignment = taRightJustify
        Caption = 'Время штрафа'
        Width = 90
      end
      item
        Caption = 'Комментарий'
        Width = 230
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnChange = ListViewFinesChange
  end
  object BitBtnAdd: TBitBtn
    Left = 135
    Top = 170
    Width = 73
    Height = 22
    Caption = 'Добавить'
    TabOrder = 3
    OnClick = BitBtnAddClick
    NumGlyphs = 2
  end
  object BitBtnDelete: TBitBtn
    Left = 207
    Top = 170
    Width = 73
    Height = 22
    Caption = 'Удалить'
    TabOrder = 4
    OnClick = BitBtnDeleteClick
    NumGlyphs = 2
  end
  object BitBtnChange: TBitBtn
    Left = 284
    Top = 170
    Width = 73
    Height = 22
    Caption = 'Изменить'
    TabOrder = 5
    OnClick = BitBtnChangeClick
    NumGlyphs = 2
  end
  object ComboBoxTime: TComboBox
    Left = 296
    Top = 118
    Width = 65
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
    OnChange = ComboBoxTimeChange
  end
  object EditComment: TEdit
    Left = 136
    Top = 142
    Width = 225
    Height = 21
    MaxLength = 30
    TabOrder = 2
    OnExit = EditCommentExit
  end
  object BitBtnSave: TBitBtn
    Left = 25
    Top = 201
    Width = 105
    Height = 24
    Caption = 'Сохранить'
    TabOrder = 6
    OnClick = BitBtnSaveClick
    Kind = bkOK
  end
  object BitBtnCancel: TBitBtn
    Left = 133
    Top = 201
    Width = 105
    Height = 24
    Caption = 'Отмена'
    TabOrder = 7
    Kind = bkCancel
  end
  object BitBtnHelp: TBitBtn
    Left = 241
    Top = 201
    Width = 105
    Height = 24
    Caption = 'Справка'
    TabOrder = 8
    Kind = bkHelp
  end
end
