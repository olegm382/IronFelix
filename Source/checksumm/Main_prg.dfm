object Form1: TForm1
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Iron Felix'
  ClientHeight = 311
  ClientWidth = 611
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 505
    Height = 113
    Caption = 'ECU Info'
    Ctl3D = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentCtl3D = False
    ParentFont = False
    TabOrder = 3
    object Label2: TLabel
      Left = 8
      Top = 26
      Width = 57
      Height = 13
      Caption = 'File name:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = False
    end
    object Label1: TLabel
      Left = 8
      Top = 52
      Width = 55
      Height = 13
      Caption = 'ECU Type:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label3: TLabel
      Left = 8
      Top = 80
      Width = 90
      Height = 13
      Caption = 'CheckSumm All:'
    end
    object Label4: TLabel
      Left = 176
      Top = 80
      Width = 97
      Height = 13
      Caption = 'CheckSumm Bad:'
    end
    object Label5: TLabel
      Left = 343
      Top = 80
      Width = 106
      Height = 13
      Caption = 'CheckSumm Fixed:'
    end
    object FName: TEdit
      Left = 79
      Top = 26
      Width = 418
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 0
    end
    object ECUType: TEdit
      Left = 79
      Top = 53
      Width = 418
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 1
    end
    object ChksAvail: TEdit
      Left = 104
      Top = 80
      Width = 41
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 2
    end
    object ChksBad: TEdit
      Left = 279
      Top = 80
      Width = 42
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 3
    end
    object ChksFix: TEdit
      Left = 455
      Top = 80
      Width = 42
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 4
    end
  end
  object OpenFile: TButton
    Left = 519
    Top = 8
    Width = 84
    Height = 25
    Caption = 'Open file'
    TabOrder = 0
    OnClick = OpenFileClick
  end
  object Logs: TMemo
    Left = 8
    Top = 136
    Width = 505
    Height = 153
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object FixECU: TButton
    Left = 519
    Top = 48
    Width = 84
    Height = 25
    Caption = 'FixECU'
    Enabled = False
    TabOrder = 2
    OnClick = FixECUClick
  end
  object About: TButton
    Left = 519
    Top = 127
    Width = 84
    Height = 25
    Caption = 'About'
    TabOrder = 4
    OnClick = AboutClick
  end
  object SaveFile: TButton
    Left = 519
    Top = 88
    Width = 84
    Height = 25
    Caption = 'Save file'
    Enabled = False
    TabOrder = 5
    OnClick = SaveFileClick
  end
  object OpenDialog1: TOpenDialog
    Filter = 'bin|*.bin|ori|*.ori|mod|*.mod|all|*.*'
    FilterIndex = 4
    Left = 576
    Top = 272
  end
  object SaveDialog1: TSaveDialog
    Filter = 'bin|*.bin|ori|*.ori|mod|*.mod|all|*.*'
    FilterIndex = 4
    Left = 528
    Top = 267
  end
end
