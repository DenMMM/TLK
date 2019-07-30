object FormClient: TFormClient
  Left = 238
  Top = 174
  Width = 690
  Height = 437
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1072' '#1082#1086#1084#1087#1100#1102#1090#1077#1088#1086#1074
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
  DesignSize = (
    682
    410)
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
        Caption = #8470
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
    TabIndex = 0
    TabOrder = 1
    OnChange = PageControlChange
    object TabSheetGames: TTabSheet
      Caption = #1055#1088#1086#1075#1088#1072#1084#1084#1099
      DesignSize = (
        557
        341)
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
        Caption = #1053#1072#1079#1074#1072#1085#1080#1077':'
      end
      object LabelCmd: TLabel
        Left = 18
        Top = 288
        Width = 98
        Height = 13
        Alignment = taRightJustify
        Anchors = [akLeft, akBottom]
        Caption = #1050#1086#1084#1072#1085#1076#1085#1072#1103' '#1089#1090#1088#1086#1082#1072':'
      end
      object LabelIcon: TLabel
        Left = 30
        Top = 312
        Width = 86
        Height = 13
        Alignment = taRightJustify
        Anchors = [akLeft, akBottom]
        Caption = #1060#1072#1081#1083' '#1089' '#1080#1082#1086#1085#1082#1086#1081':'
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
        Caption = #1044#1086#1073#1072#1074#1080#1090#1100
        TabOrder = 4
        OnClick = ButtonAddClick
      end
      object ButtonDel: TButton
        Left = 478
        Top = 282
        Width = 73
        Height = 22
        Anchors = [akRight, akBottom]
        Caption = #1059#1076#1072#1083#1080#1090#1100
        TabOrder = 5
        OnClick = ButtonDelClick
      end
      object ButtonAddChild: TButton
        Left = 478
        Top = 311
        Width = 73
        Height = 22
        Anchors = [akRight, akBottom]
        Caption = #1050#1086#1088#1077#1085#1100
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
      Caption = #1055#1086#1074#1077#1076#1077#1085#1080#1077' '#1082#1083#1080#1077#1085#1090#1072
      ImageIndex = 1
      DesignSize = (
        557
        341)
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
        Caption = #1055#1088#1077#1076#1091#1087#1088#1077#1078#1076#1077#1085#1080#1077' '#1086#1073' '#1086#1082#1086#1085#1095#1072#1085#1080#1080' '#1074#1088#1077#1084#1077#1085#1080' ('#1084#1080#1085#1091#1090'):'
      end
      object Label4: TLabel
        Left = 179
        Top = 44
        Width = 110
        Height = 13
        Alignment = taRightJustify
        Caption = #1055#1086#1082#1072#1079#1099#1074#1072#1090#1100' ('#1089#1077#1082#1091#1085#1076'):'
      end
      object Label5: TLabel
        Left = 106
        Top = 88
        Width = 186
        Height = 13
        Alignment = taRightJustify
        Caption = #1047#1072#1076#1077#1088#1078#1082#1072' '#1076#1086' '#1087#1077#1088#1077#1079#1072#1075#1088#1091#1079#1082#1080' ('#1089#1077#1082#1091#1085#1076'):'
      end
      object Label6: TLabel
        Left = 76
        Top = 132
        Width = 216
        Height = 13
        Alignment = taRightJustify
        Caption = #1041#1083#1086#1082#1080#1088#1086#1074#1082#1072' '#1087#1088#1080' '#1086#1090#1089#1091#1090#1089#1090#1074#1080#1080' '#1089#1074#1103#1079#1080' ('#1084#1080#1085#1091#1090'):'
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
        ItemHeight = 0
        TabOrder = 0
        OnClick = ComboBoxToEndTimeClick
      end
      object ComboBoxMessageTime: TComboBox
        Left = 296
        Top = 40
        Width = 125
        Height = 21
        Style = csDropDownList
        ItemHeight = 0
        TabOrder = 1
      end
      object ComboBoxRebootWait: TComboBox
        Left = 296
        Top = 84
        Width = 125
        Height = 21
        Style = csDropDownList
        ItemHeight = 0
        TabOrder = 2
      end
      object ComboBoxAutoLockTime: TComboBox
        Left = 296
        Top = 128
        Width = 125
        Height = 21
        Style = csDropDownList
        ItemHeight = 0
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
    Caption = #1054#1087#1077#1088#1072#1094#1080#1080'...'
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
    Caption = #1047#1072#1082#1088#1099#1090#1100
    ModalResult = 2
    TabOrder = 3
  end
  object ButtonHelp: TButton
    Left = 584
    Top = 380
    Width = 85
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #1057#1087#1088#1072#1074#1082#1072
    Enabled = False
    TabOrder = 4
  end
  object ButtonCancel: TButton
    Left = 19
    Top = 379
    Width = 73
    Height = 22
    Anchors = [akRight, akBottom]
    Caption = #1055#1088#1077#1088#1074#1072#1090#1100
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
      Caption = #1056#1072#1079#1086#1089#1083#1072#1090#1100
      OnClick = NSendClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object NOpen: TMenuItem
      Caption = #1054#1082#1088#1099#1090#1100'...'
      OnClick = NOpenClick
    end
    object NSave: TMenuItem
      Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100'...'
      OnClick = NSaveClick
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object NLoad: TMenuItem
      Caption = #1047#1072#1075#1088#1091#1079#1080#1090#1100
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
