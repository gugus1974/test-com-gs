object ConsolleForm: TConsolleForm
  Left = 551
  Top = 116
  Width = 751
  Height = 576
  Caption = 'CONSOLLE'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object ConsolleMemo: TMemo
    Left = 0
    Top = 0
    Width = 743
    Height = 523
    Align = alClient
    Lines.Strings = (
      'ConsolleMemo')
    TabOrder = 0
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 523
    Width = 743
    Height = 19
    Panels = <
      item
        Width = 50
      end
      item
        Width = 50
      end
      item
        Width = 50
      end>
    SimplePanel = False
  end
end
