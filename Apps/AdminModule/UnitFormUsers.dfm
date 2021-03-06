object FormUsers: TFormUsers
  Left = 260
  Top = 188
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1055#1086#1083#1100#1079#1086#1074#1072#1090#1077#1083#1080
  ClientHeight = 342
  ClientWidth = 541
  Color = clBtnFace
  DoubleBuffered = True
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
  object Bevel4: TBevel
    Left = 4
    Top = 4
    Width = 533
    Height = 301
    Style = bsRaised
  end
  object Bevel1: TBevel
    Left = 12
    Top = 216
    Width = 517
    Height = 57
    Shape = bsFrame
  end
  object LabelFullName: TLabel
    Left = 26
    Top = 250
    Width = 64
    Height = 13
    Alignment = taRightJustify
    Caption = #1055#1086#1083#1085#1086#1077' '#1080#1084#1103':'
  end
  object LabelLogin: TLabel
    Left = 56
    Top = 226
    Width = 34
    Height = 13
    Alignment = taRightJustify
    Caption = #1051#1086#1075#1080#1085':'
  end
  object LabelActive: TLabel
    Left = 247
    Top = 227
    Width = 45
    Height = 13
    Alignment = taRightJustify
    Caption = #1040#1082#1090#1080#1074#1077#1085':'
  end
  object ListViewUsers: TListView
    Left = 8
    Top = 8
    Width = 525
    Height = 201
    Columns = <
      item
        Caption = #1051#1086#1075#1080#1085
        Width = 150
      end
      item
        Caption = #1055#1086#1083#1085#1086#1077' '#1080#1084#1103
        Width = 300
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SmallImages = FormMain.ImageListIcons
    SortType = stText
    TabOrder = 0
    ViewStyle = vsReport
    OnInsert = ListViewUsersInsert
    OnSelectItem = ListViewUsersSelectItem
  end
  object EditName: TEdit
    Left = 96
    Top = 246
    Width = 321
    Height = 21
    TabOrder = 4
    OnExit = EditNameExit
  end
  object EditLogin: TEdit
    Left = 96
    Top = 222
    Width = 133
    Height = 21
    TabOrder = 1
    OnExit = EditLoginExit
  end
  object ButtonSave: TButton
    Left = 260
    Top = 312
    Width = 85
    Height = 25
    Caption = #1055#1088#1080#1085#1103#1090#1100
    ModalResult = 1
    TabOrder = 8
    OnClick = ButtonSaveClick
  end
  object ButtonCancel: TButton
    Left = 348
    Top = 312
    Width = 85
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 9
  end
  object ButtonHelp: TButton
    Left = 444
    Top = 312
    Width = 85
    Height = 25
    Caption = #1057#1087#1088#1072#1074#1082#1072
    Enabled = False
    TabOrder = 10
  end
  object ButtonAdd: TButton
    Left = 378
    Top = 278
    Width = 73
    Height = 22
    Caption = #1044#1086#1073#1072#1074#1080#1090#1100
    TabOrder = 6
    OnClick = ButtonAddClick
  end
  object ButtonDel: TButton
    Left = 454
    Top = 278
    Width = 73
    Height = 22
    Caption = #1059#1076#1072#1083#1080#1090#1100
    TabOrder = 7
    OnClick = ButtonDelClick
  end
  object BitBtnActive: TBitBtn
    Left = 296
    Top = 222
    Width = 57
    Height = 21
    Caption = #1044#1072
    Glyph.Data = {
      42020000424D4202000000000000420000002800000010000000100000000100
      1000030000000002000000000000000000000000000000000000007C0000E003
      00001F0000001F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C
      1F7C1F7C1F7C1F7C1F7CA215A20CA10CA10CA10C810C81088108810861086108
      61081F7C1F7C1F7C1F7C8215C210C20CA20CA10CA10C810C810C810881088108
      61081F7C1F7C1F7C1F7CA215C210C210C210C20C8211A215A10C810C81088108
      81081F7C1F7C1F7C1F7CC319E214E210E210C2108215C215A10CA10C810C8108
      81081F7C1F7C1F7C1F7C031E231502150215E210A215E215C20CA20CA10C810C
      81081F7C1F7C1F7C1F7C24224319231923150215E210E210C210C20CA20CA10C
      810C1F7C1F7C1F7C1F7C6426641D43194319231503150215E210C210C20CA20C
      A10C1F7C1F7C1F7C1F7CE52AE52A841D84266422431E231E031AE219E215E215
      C2151F7C1F7C1F7C1F7C1F7C2633A5211F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C
      1F7C1F7C1F7C1F7C1F7C883B062A1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C
      1F7C1F7C1F7C1F7C1F7CE94347321F7C1F7C1F7C1F7C1F7C1F7C1F7C83221F7C
      1F7C1F7C1F7C1F7C1F7CEB4B883A1F7C1F7C1F7C1F7C1F7C1F7CC42643191F7C
      1F7C1F7C1F7C1F7C1F7C1F7CEC4F883A1F7C1F7C1F7C1F7CE52A631D1F7C1F7C
      1F7C1F7C1F7C1F7C1F7C1F7C1F7CEC4F883A473267372633A4211F7C1F7C1F7C
      1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7CEB4BE943262EE5251F7C1F7C1F7C1F7C
      1F7C1F7C1F7C}
    ParentDoubleBuffered = True
    TabOrder = 2
    OnClick = BitBtnActiveClick
  end
  object BitBtnInactive: TBitBtn
    Left = 358
    Top = 222
    Width = 57
    Height = 21
    Caption = #1053#1077#1090
    Glyph.Data = {
      42020000424D4202000000000000420000002800000010000000100000000100
      1000030000000002000000000000000000000000000000000000007C0000E003
      00001F0000001F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C
      1F7C1F7C1F7C1F7C1F7CA6304314431443144210421042104210420C220C220C
      220C1F7C1F7C1F7C1F7CA62C63186314431443144314421042104210420C220C
      220C1F7C1F7C1F7C1F7CC6306418631863186314A52CA6304210421042104210
      220C1F7C1F7C1F7C1F7CC734841C641C641C6318A62CC6344314421442104210
      42101F7C1F7C1F7C1F7CE83C85208420841C641CA630C7386314431443144210
      42101F7C1F7C1F7C1F7C0941A528A52485208420841C641C6318631443144214
      42101F7C1F7C1F7C1F7C2A49C62CA628A52485248420841C641C631863144314
      42101F7C1F7C1F7C1F7C6C596B59C7302A4D29490945E840E83CC738A62C6314
      A6301F7C1F7C1F7C1F7C1F7C8D65E8381F7C1F7C1F7C1F7C1F7C1F7CA6306318
      1F7C1F7C1F7C1F7C1F7C1F7CCF7129451F7C1F7C1F7C1F7C1F7C1F7CC6346318
      1F7C1F7C1F7C1F7C1F7C1F7C327E6B511F7C1F7C1F7C1F7C1F7C1F7CC734641C
      1F7C1F7C1F7C1F7C1F7C1F7C1F7C327E6B511F7C1F7C1F7C1F7CE83C84201F7C
      1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C327E4A4D09414A510949A5281F7C1F7C
      1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C117EAE6D0839C7301F7C1F7C1F7C
      1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C
      1F7C1F7C1F7C}
    ParentDoubleBuffered = True
    TabOrder = 3
    OnClick = BitBtnActiveClick
  end
  object ButtonPassword: TButton
    Left = 430
    Top = 234
    Width = 89
    Height = 24
    Caption = #1055#1072#1088#1086#1083#1100'...'
    TabOrder = 5
    OnClick = ButtonPasswordClick
  end
end
