object FormMain: TFormMain
  Left = 287
  Top = 209
  ActiveControl = TrackBarVolume
  Caption = 'VPlay'
  ClientHeight = 375
  ClientWidth = 504
  Color = clBlack
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnResize = FormResize
  OnShow = FormShow
  DesignSize = (
    504
    375)
  PixelsPerInch = 96
  TextHeight = 13
  object PanelControl: TPanel
    Left = 0
    Top = 328
    Width = 512
    Height = 56
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 0
    DesignSize = (
      512
      56)
    object BevelState: TBevel
      Left = 2
      Top = 32
      Width = 508
      Height = 22
      Anchors = [akLeft, akTop, akRight]
    end
    object SpeedButtonPlay: TSpeedButton
      Left = 6
      Top = 5
      Width = 23
      Height = 22
      GroupIndex = 1
      Down = True
      Flat = True
      Glyph.Data = {
        42020000424D4202000000000000420000002800000010000000100000000100
        1000030000000002000000000000000000000000000000000000007C0000E003
        00001F0000001F7C1F7C1F7C1F7C1F7C00021F7C1F7C1F7C1F7C1F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003000200021F7C1F7C1F7C1F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003000200021F7C1F7C1F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003000200021F7C1F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003E003000200021F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003E003E003000200021F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003E003E003E003000200021F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003E003E003E003E00300020002
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003E003E003E003E003E0031F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003E003E003E003E0031F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003E003E003E0031F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003E003E0031F7C1F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E003E0031F7C1F7C1F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E003E0031F7C1F7C1F7C1F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE003E0031F7C1F7C1F7C1F7C1F7C1F7C1F7C
        1F7C1F7C1F7C1F7C1F7C1F7C1F7CE0031F7C1F7C1F7C1F7C1F7C1F7C1F7C1F7C
        1F7C1F7C1F7C}
      Spacing = 0
      Transparent = False
      OnClick = SpeedButtonPlayClick
    end
    object SpeedButtonPause: TSpeedButton
      Left = 30
      Top = 5
      Width = 23
      Height = 22
      GroupIndex = 1
      Flat = True
      Glyph.Data = {
        36030000424D3603000000000000360000002800000010000000100000000100
        18000000000000030000120B0000120B00000000000000000000F800F8F800F8
        F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F8
        00F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8
        F800F8F800F8F800F8008080008080F800F8F800F8F800F8F800F80080800080
        80F800F8F800F8F800F8F800F8F800F8F800F800F8F800F8F800F8F8008080F8
        00F8F800F800F8F800F8F800F8F8008080F800F8F800F8F800F8F800F8F800F8
        F800F800F8F800F8F800F8F8008080F800F8F800F800F8F800F8F800F8F80080
        80F800F8F800F8F800F8F800F8F800F8F800F800F8F800F8F800F8F8008080F8
        00F8F800F800F8F800F8F800F8F8008080F800F8F800F8F800F8F800F8F800F8
        F800F800F8F800F8F800F8F8008080F800F8F800F800F8F800F8F800F8F80080
        80F800F8F800F8F800F8F800F8F800F8F800F800F8F800F8F800F8F8008080F8
        00F8F800F800F8F800F8F800F8F8008080F800F8F800F8F800F8F800F8F800F8
        F800F800F8F800F8F800F8F8008080F800F8F800F800F8F800F8F800F8F80080
        80F800F8F800F8F800F8F800F8F800F8F800F800F8F800F8F800F8F8008080F8
        00F8F800F800F8F800F8F800F8F8008080F800F8F800F8F800F8F800F8F800F8
        F800F800F8F800F8F800F8F8008080F800F8F800F800F8F800F8F800F8F80080
        80F800F8F800F8F800F8F800F8F800F8F800F800F8F800F8F800F8F8008080F8
        00F8F800F800F8F800F8F800F8F8008080F800F8F800F8F800F8F800F8F800F8
        F800F800F8F800F8F800F8F8F800F8F800F8F800F800F8F800F8F800F8F8F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F8
        00F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8
        F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F8
        00F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8}
      Spacing = 0
      Transparent = False
      OnClick = SpeedButtonPauseClick
    end
    object SpeedButtonStop: TSpeedButton
      Left = 54
      Top = 5
      Width = 23
      Height = 22
      GroupIndex = 1
      Flat = True
      Glyph.Data = {
        36030000424D3603000000000000360000002800000010000000100000000100
        18000000000000030000120B0000120B00000000000000000000F800F8F800F8
        F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F8
        00F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8
        F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F880808080
        8080808080808080808080808080F800F8F800F8F800F8F800F8F800F8F800F8
        F800F8F800F8000000000000000000000000000000000000000000808080F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F800000000000000000000
        0000000000000000000000808080F800F8F800F8F800F8F800F8F800F8F800F8
        F800F8F800F8000000000000000000000000000000000000000000808080F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F800000000000000000000
        0000000000000000000000808080F800F8F800F8F800F8F800F8F800F8F800F8
        F800F8F800F8000000000000000000000000000000000000000000808080F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F800000000000000000000
        0000000000000000000000808080F800F8F800F8F800F8F800F8F800F8F800F8
        F800F8F800F8000000000000000000000000000000000000000000F800F8F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F8
        00F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8
        F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F8
        00F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8
        F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800
        F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F8
        00F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8F800F8}
      Spacing = 0
      Transparent = False
      OnClick = SpeedButtonStopClick
    end
    object LabelState: TLabel
      Left = 3
      Top = 33
      Width = 405
      Height = 20
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Color = clBlack
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWhite
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Layout = tlCenter
    end
    object LabelTime: TLabel
      Left = 408
      Top = 33
      Width = 46
      Height = 20
      Alignment = taCenter
      Anchors = [akTop, akRight]
      AutoSize = False
      Caption = '00:00:00'
      Color = clBlack
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWhite
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Layout = tlCenter
    end
    object LabelSeparator: TLabel
      Left = 454
      Top = 33
      Width = 9
      Height = 20
      Alignment = taCenter
      Anchors = [akTop, akRight]
      AutoSize = False
      Caption = '/'
      Color = clBlack
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWhite
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Layout = tlCenter
    end
    object LabelLength: TLabel
      Left = 463
      Top = 33
      Width = 46
      Height = 20
      Alignment = taCenter
      Anchors = [akTop, akRight]
      AutoSize = False
      Caption = '00:00:00'
      Color = clBlack
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWhite
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Layout = tlCenter
    end
    object BevelPos: TBevel
      Left = 96
      Top = 12
      Width = 325
      Height = 9
      Anchors = [akLeft, akTop, akRight]
    end
    object TrackBarVolume: TTrackBar
      Left = 436
      Top = 10
      Width = 71
      Height = 12
      Anchors = [akTop, akRight]
      Max = 100
      PageSize = 0
      TabOrder = 0
      ThumbLength = 9
      TickMarks = tmBoth
      TickStyle = tsNone
      OnChange = TrackBarVolumeChange
    end
    object PanelPos: TPanel
      Left = 94
      Top = 10
      Width = 13
      Height = 13
      BevelInner = bvLowered
      BevelWidth = 2
      TabOrder = 1
      OnMouseDown = PanelPosMouseDown
      OnMouseMove = PanelPosMouseMove
      OnMouseUp = PanelPosMouseUp
    end
  end
  object Timer: TTimer
    Interval = 500
    OnTimer = TimerTimer
    Left = 8
    Top = 8
  end
end
