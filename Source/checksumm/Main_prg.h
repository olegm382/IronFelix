//---------------------------------------------------------------------------

#ifndef Main_prgH
#define Main_prgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TOpenDialog *OpenDialog1;
	TButton *OpenFile;
	TSaveDialog *SaveDialog1;
	TLabel *Label1;
	TButton *SaveFile;
	TLabel *Label2;
	TMemo *Logs;
	TButton *About;
	TButton *FixECU;
	TEdit *FName;
	TGroupBox *GroupBox1;
	TEdit *ECUType;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *ChksAvail;
	TEdit *ChksBad;
	TLabel *Label5;
	TEdit *ChksFix;
	void __fastcall OpenFileClick(TObject *Sender);
	void __fastcall SaveFileClick(TObject *Sender);
	void __fastcall AboutClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FixECUClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
