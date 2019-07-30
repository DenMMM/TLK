object FormClient: TFormClient
  Left = 238
  Top = 174
  Width = 690
  Height = 437
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'Настройка компьютеров'
  Color = clBtnFace
  Constraints.MinHeight = 437
  Constraints.MinWidth = 690
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ListViewComputers: TListView
    Left = 4
    Top = 4
    Width = 101
    Height = 369
    Anchors = [akLeft, akTop, akBottom]
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
  end
  object PageControl: TPageControl
    Left = 112
    Top = 4
    Width = 565
    Height = 369
    ActivePage = TabSheetGames
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 1
    OnChange = PageControlChange
    object TabSheetGames: TTabSheet
      Caption = 'Программы'
      object Bevel1: TBevel
        Left = 4
        Top = 252
        Width = 469
        Height = 85
        Anchors = [akLeft, akRight, akBottom]
        Shape = bsFrame
      end
      object LabelName: TLabel
        Left = 63
        Top = 264
        Width = 53
        Height = 13
        Alignment = taRightJustify
        Anchors = [akLeft, akBottom]
        Caption = 'Название:'
      end
      object LabelCmd: TLabel
        Left = 18
        Top = 288
        Width = 98
        Height = 13
        Alignment = taRightJustify
        Anchors = [akLeft, akBottom]
        Caption = 'Командная строка:'
      end
      object LabelIcon: TLabel
        Left = 30
        Top = 312
        Width = 86
        Height = 13
        Alignment = taRightJustify
        Anchors = [akLeft, akBottom]
        Caption = 'Файл с иконкой:'
      end
      object TreeViewGames: TTreeView
        Left = 4
        Top = 4
        Width = 549
        Height = 241
        Anchors = [akLeft, akTop, akRight, akBottom]
        HideSelection = False
        Indent = 19
        ReadOnly = True
        TabOrder = 0
        OnChange = TreeViewGamesChange
        OnDeletion = TreeViewGamesDeletion
      end
      object EditName: TEdit
        Left = 120
        Top = 260
        Width = 193
        Height = 21
        Anchors = [akLeft, akRight, akBottom]
        TabOrder = 1
        OnChange = EditNameChange
      end
      object EditCmd: TEdit
        Left = 120
        Top = 284
        Width = 345
        Height = 21
        Anchors = [akLeft, akRight, akBottom]
        TabOrder = 2
        OnChange = EditCmdChange
      end
      object EditIcon: TEdit
        Left = 120
        Top = 308
        Width = 345
        Height = 21
        Anchors = [akLeft, akRight, akBottom]
        TabOrder = 3
        OnChange = EditIconChange
      end
      object ButtonAdd: TButton
        Left = 478
        Top = 258
        Width = 73
        Height = 22
        Anchors = [akRight, akBottom]
        Caption = 'Добавить'
        TabOrder = 4
        OnClick = ButtonAddClick
      end
      object ButtonDel: TButton
        Left = 478
        Top = 282
        Width = 73
        Height = 22
        Anchors = [akRight, akBottom]
        Caption = 'Удалить'
        TabOrder = 5
        OnClick = ButtonDelClick
      end
      object ButtonAddChild: TButton
        Left = 478
        Top = 311
        Width = 73
        Height = 22
        Anchors = [akRight, akBottom]
        Caption = 'Корень'
        TabOrder = 6
        OnClick = ButtonAddChildClick
      end
      object BitBtnUp: TBitBtn
        Left = 400
        Top = 256
        Width = 29
        Height = 25
        Anchors = [akRight, akBottom]
        Caption = 'Up'
        TabOrder = 7
        OnClick = BitBtnUpClick
      end
      object BitBtnDown: TBitBtn
        Left = 428
        Top = 256
        Width = 29
        Height = 25
        Anchors = [akRight, akBottom]
        Caption = 'Dwn'
        TabOrder = 8
        OnClick = BitBtnDownClick
      end
    end
    object TabSheetOptions: TTabSheet
      Caption = 'Поведение клиента'
      ImageIndex = 1
      object Bevel2: TBevel
        Left = 4
        Top = 4
        Width = 549
        Height = 333
        Anchors = [akLeft, akTop, akRight, akBottom]
        Shape = bsFrame
      end
      object Label1: TLabel
        Left = 42
        Top = 20
        Width = 247
        Height = 13
        Alignment = taRightJustify
        Caption = 'Предупреждение об окончании времени (минут):'
      end
      object Label4: TLabel
        Left = 179
        Top = 44
        Width = 110
        Height = 13
        Alignment = taRightJustify
        Caption = 'Показывать (секунд):'
      end
      object Label5: TLabel
        Left = 106
        Top = 88
        Width = 186
        Height = 13
        Alignment = taRightJustify
        Caption = 'Задержка до перезагрузки (секунд):'
      end
      object Label6: TLabel
        Left = 76
        Top = 132
        Width = 216
        Height = 13
        Alignment = taRightJustify
        Caption = 'Блокировка при отсутствии связи (минут):'
      end
      object Bevel3: TBevel
        Left = 12
        Top = 72
        Width = 533
        Height = 9
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object Bevel4: TBevel
        Left = 12
        Top = 116
        Width = 533
        Height = 9
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object Bevel5: TBevel
        Left = 12
        Top = 160
        Width = 533
        Height = 9
        Anchors = [akLeft, akTop, akRight]
        Shape = bsTopLine
      end
      object ComboBoxToEndTime: TComboBox
        Left = 296
        Top = 16
        Width = 125
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        OnClick = ComboBoxToEndTimeClick
      end
      object ComboBoxMessageTime: TComboBox
        Left = 296
        Top = 40
        Width = 125
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 1
      end
      object ComboBoxRebootWait: TComboBox
        Left = 296
        Top = 84
        Width = 125
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 2
      end
      object ComboBoxAutoLockTime: TComboBox
        Left = 296
        Top = 128
        Width = 125
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 3
      end
    end
  end
  object ButtonOperations: TButton
    Left = 400
    Top = 380
    Width = 85
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Операции...'
    PopupMenu = PopupMenuOperations
    TabOrder = 2
    OnClick = ButtonOperationsClick
  end
  object ButtonClose: TButton
    Left = 488
    Top = 380
    Width = 85
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Закрыть'
    ModalResult = 2
    TabOrder = 3
  end
  object ButtonHelp: TButton
    Left = 584
    Top = 380
    Width = 85
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Справка'
    Enabled = False
    TabOrder = 4
  end
  object ButtonCancel: TButton
    Left = 19
    Top = 379
    Width = 73
    Height = 22
    Anchors = [akRight, akBottom]
    Caption = 'Прервать'
    TabOrder = 5
    OnClick = ButtonCancelClick
  end
  object PanelProcess: TPanel
    Left = 112
    Top = 380
    Width = 209
    Height = 21
    Alignment = taLeftJustify
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 6
  end
  object PopupMenuOperations: TPopupMenu
    Alignment = paRight
    AutoPopup = False
    Left = 16
    Top = 28
    object NSend: TMenuItem
      Caption = 'Разослать'
      OnClick = NSendClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object NOpen: TMenuItem
      Caption = 'Окрыть...'
      OnClick = NOpenClick
    end
    object NSave: TMenuItem
      Caption = 'Сохранить...'
      OnClick = NSaveClick
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object NLoad: TMenuItem
      Caption = 'Загрузить'
      OnClick = NLoadClick
    end
  end
  object OpenDialog: TOpenDialog
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 44
    Top = 28
  end
  object SaveDialog: TSaveDialog
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Left = 72
    Top = 28
  end
end
