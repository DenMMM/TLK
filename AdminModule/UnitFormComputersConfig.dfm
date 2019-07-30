object FormComputersConfig: TFormComputersConfig
  Left = 265
  Top = 146
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Настройка компьютеров'
  ClientHeight = 372
  ClientWidth = 611
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
    Width = 599
    Height = 335
    Style = bsRaised
  end
  object ListViewComputers: TListView
    Left = 10
    Top = 10
    Width = 103
    Height = 271
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
        Caption = 'Net'
        Width = 30
      end>
    HideSelection = False
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    SmallImages = FormMain.ImageListIcons
    TabOrder = 0
    ViewStyle = vsReport
    OnInsert = ListViewComputersInsert
  end
  object PageControlOptions: TPageControl
    Left = 120
    Top = 10
    Width = 481
    Height = 327
    ActivePage = TabSheetGames
    TabOrder = 3
    object TabSheetGames: TTabSheet
      Caption = 'Программы'
      object Label1: TLabel
        Left = 63
        Top = 204
        Width = 53
        Height = 13
        Alignment = taRightJustify
        Caption = 'Название:'
      end
      object Label2: TLabel
        Left = 18
        Top = 228
        Width = 98
        Height = 13
        Alignment = taRightJustify
        Caption = 'Командная строка:'
      end
      object Label3: TLabel
        Left = 30
        Top = 252
        Width = 86
        Height = 13
        Alignment = taRightJustify
        Caption = 'Файл с иконкой:'
      end
      object TreeViewGames: TTreeView
        Left = 4
        Top = 4
        Width = 465
        Height = 189
        HideSelection = False
        Indent = 19
        ReadOnly = True
        TabOrder = 0
        OnChange = TreeViewGamesChange
        OnDeletion = TreeViewGamesDeletion
      end
      object EditGameName: TEdit
        Left = 120
        Top = 200
        Width = 321
        Height = 21
        MaxLength = 63
        TabOrder = 1
        OnExit = EditGameNameExit
      end
      object EditGameCommandLine: TEdit
        Left = 120
        Top = 224
        Width = 321
        Height = 21
        MaxLength = 255
        TabOrder = 2
        OnExit = EditGameCommandLineExit
      end
      object EditGameIconFile: TEdit
        Left = 120
        Top = 248
        Width = 321
        Height = 21
        MaxLength = 255
        TabOrder = 4
        OnExit = EditGameIconFileExit
      end
      object ButtonBrowseGameFile: TButton
        Left = 444
        Top = 224
        Width = 21
        Height = 21
        Caption = '...'
        TabOrder = 3
        OnClick = ButtonBrowseGameFileClick
      end
      object ButtonBrowseGameIcon: TButton
        Left = 444
        Top = 248
        Width = 21
        Height = 21
        Caption = '...'
        TabOrder = 5
        OnClick = ButtonBrowseGameIconClick
      end
      object ButtonAddGame: TButton
        Left = 246
        Top = 274
        Width = 73
        Height = 22
        Caption = 'Добавить'
        TabOrder = 6
        OnClick = ButtonAddGameClick
      end
      object ButtonDeleteGame: TButton
        Left = 318
        Top = 274
        Width = 73
        Height = 22
        Caption = 'Удалить'
        TabOrder = 7
        OnClick = ButtonDeleteGameClick
      end
      object ButtonChangeGame: TButton
        Left = 394
        Top = 274
        Width = 73
        Height = 22
        Caption = 'Изменить'
        TabOrder = 8
        OnClick = ButtonChangeGameClick
      end
    end
  end
  object BitBtnClose: TBitBtn
    Left = 388
    Top = 345
    Width = 105
    Height = 24
    Caption = 'Закрыть'
    TabOrder = 4
    Kind = bkCancel
  end
  object BitBtnHelp: TBitBtn
    Left = 496
    Top = 345
    Width = 105
    Height = 24
    Caption = 'Справка'
    TabOrder = 5
    Kind = bkHelp
  end
  object ButtonSetComputerParam: TButton
    Left = 13
    Top = 288
    Width = 100
    Height = 24
    Caption = '<< Разослать'
    TabOrder = 1
    OnClick = ButtonSetComputerParamClick
  end
  object ButtonGetComputerParam: TButton
    Left = 13
    Top = 311
    Width = 100
    Height = 24
    Caption = 'Загрузить >>'
    TabOrder = 2
    OnClick = ButtonGetComputerParamClick
  end
  object OpenDialog: TOpenDialog
    Left = 572
  end
end
