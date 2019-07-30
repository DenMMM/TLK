object FormDesk: TFormDesk
  Left = 261
  Top = 187
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  AlphaBlendValue = 150
  AutoSize = True
  BorderIcons = []
  BorderStyle = bsDialog
  ClientHeight = 32
  ClientWidth = 115
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel2: TBevel
    Left = 1
    Top = 1
    Width = 113
    Height = 30
  end
  object LabelWorkTime: TLabel
    Left = 10
    Top = 11
    Width = 45
    Height = 13
    AutoSize = False
    Caption = '00:00'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Lucida Console'
    Font.Pitch = fpFixed
    Font.Style = [fsBold]
    ParentFont = False
    Layout = tlCenter
  end
  object LabelCompNum: TLabel
    Left = 71
    Top = 3
    Width = 38
    Height = 25
    Alignment = taCenter
    AutoSize = False
    Caption = '99'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'BankGothic Md BT'
    Font.Style = []
    ParentFont = False
    Layout = tlCenter
  end
  object Bevel3: TBevel
    Left = 63
    Top = 5
    Width = 9
    Height = 21
    Shape = bsLeftLine
  end
  object LabelHide: TLabel
    Left = 0
    Top = 0
    Width = 115
    Height = 32
    AutoSize = False
    ShowAccelChar = False
    Transparent = True
    OnMouseEnter = LabelHideMouseEnter
  end
  object TimerHide: TTimer
    Enabled = False
    OnTimer = TimerHideTimer
  end
end
