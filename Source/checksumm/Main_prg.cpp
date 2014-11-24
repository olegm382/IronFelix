//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include <strsafe.h>
#pragma hdrstop
#include <Modules_dll.h>

#include "Main_prg.h"
#define BUFSIZE MAX_PATH
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

void PrintErrorMessage();

TForm1 *Form1;

	char KnownECU=0;	// Флаг ECU опознано
	TCHAR GoodModules[BUFSIZE];	// Full name module for check file
	struct ECU_info ECUinfo;
	TCHAR CurrDir_Buff[BUFSIZE];
	DWORD dwRet;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenFileClick(TObject *Sender)
{

	int OriFileHandle;   // Handle файла прошивки
	int OriFileSize;	 // Размер прошивки
	LPVOID OriBuffer = NULL;  // Указатель на буфер чтения прошивки


	 WIN32_FIND_DATA FindFileData;
	 HANDLE hFind = INVALID_HANDLE_VALUE;
	 DWORD dwError;
	 LPSTR DirSpec;

	   DirSpec = (LPSTR) malloc (BUFSIZE);
//	   TCHAR CurrDir_Buff[BUFSIZE];
	   TCHAR Modules_Buff[BUFSIZE];
//	   DWORD dwRet;

	if (KnownECU==1)     //
		{
			VirtualFree(ECUinfo.EcuBuffer, 0, MEM_RELEASE);
		}
	KnownECU=0;
	FixECU->Enabled=False;
	SaveFile->Enabled=False;
	ECUType->Text="";
	ChksAvail->Text="";
	ChksBad->Text="";
	ChksFix->Text="";

//	   dwRet = GetCurrentDirectory(BUFSIZE, CurrDir_Buff);
//	   if (dwRet!=0) Logs->Lines->Add(CurrDir_Buff);
//	   if (dwRet==0) throw (Exception("GetCurrentDir Error"));

	Logs->Lines->Add("Select file to open...");
	if (OpenDialog1->Execute())   //Вызываем диалог выбора файла
	{
    // First, check if the file exists.
		if (FileExists(OpenDialog1->FileName))
		{
		  FName->Text=OpenDialog1->FileName;
        }
		else
		{
		  Logs->Lines->Add("File does not exist");
		  throw(Exception("File does not exist"));
		}
	}
	else
	{
        Logs->Lines->Add("File not selected");
		throw(Exception("File not selected"));
    }
	Logs->Lines->Add (OpenDialog1->FileName);
	OriFileHandle=FileOpen (OpenDialog1->FileName,fmOpenRead );
	OriFileSize=FileSeek(OriFileHandle, 0, 2);  //Определяем размер файла
	FileSeek(OriFileHandle, 0, 0);	//Позиционируемся на начало файла

	Logs->Lines->Add ("File size is: " + IntToStr (OriFileSize) + " bytes");

	// Проверяем допустимые размеры прошивки

	if (OriFileSize !=32768 && OriFileSize != 65536 && OriFileSize != 0x20000 && OriFileSize != 0x40000 && OriFileSize != 0x80000 && OriFileSize !=0xC0000 && OriFileSize !=0xD0000 && OriFileSize != 0x100000 && OriFileSize != 0x200000 )
		{
			FileClose(OriFileHandle);
			Logs->Lines->Add("Wrong file size");
            throw(Exception("Wrong file size"));
		}
		OriBuffer = VirtualAlloc(NULL, OriFileSize, MEM_COMMIT, PAGE_READWRITE);
		Logs->Lines->Add ("Reading file...");
	if (FileRead (OriFileHandle, OriBuffer, OriFileSize) !=OriFileSize)
		{
			VirtualFree(OriBuffer, 0, MEM_RELEASE);
			OriBuffer=NULL;
			FileClose(OriFileHandle);
			Logs->Lines->Add("File read error.");
			throw(Exception("File read error."));
		}
		  Logs->Lines->Add ("Readed " + IntToStr (OriFileSize) + " bytes");
		  FileClose(OriFileHandle);

			ECUinfo.EcuBuffer=OriBuffer;
			ECUinfo.EcuFileSize=OriFileSize;


// Распознаем тип блока ECU и количество контрольных сумм


	   StringCbCopyN (DirSpec, BUFSIZE, CurrDir_Buff, dwRet);
	   StringCbCatN (DirSpec, BUFSIZE, "\\Modules\\*.dll", 16);

//		   Logs->Lines->Add(DirSpec);

   // Find the first file in the directory.
	   hFind = FindFirstFile(DirSpec, &FindFileData);

   if (hFind == INVALID_HANDLE_VALUE)
   {
/*	  Logs->Lines->Add ("Invalid file handle. Error is ");
	  Logs->Lines->Add (GetLastError());
*/

	  throw(Exception("Invalid file handle"));
   }
   else
   {
	  do
	  {
//	   Logs->Lines->Add ( FindFileData.cFileName);
	   StringCbCopyN (Modules_Buff, BUFSIZE, CurrDir_Buff, dwRet);
	   StringCbCatN (Modules_Buff, BUFSIZE, "\\Modules\\", 11);
	   StringCbCatN (Modules_Buff, BUFSIZE, FindFileData.cFileName, StrLen(FindFileData.cFileName)+1);
//	   Logs->Lines->Add(Modules_Buff);
	   HMODULE hDll= LoadLibrary (Modules_Buff);
	   if (hDll)
			{
				RecogniseECU_proc =(RecogniseECU_Imp) GetProcAddress(hDll, RecogniseECU);
				if (RecogniseECU_proc)
					{
						 KnownECU=RecogniseECU_proc(&ECUinfo);
							if (KnownECU)
								{
									 Logs->Lines->Add("ECU type:");
									 Logs->Lines->Add(ECUinfo.ECU_Descr);
									 ECUType->Text=ECUinfo.ECU_Descr;
									 Logs->Lines->Add("Avaiable checksumm="+ IntToStr(ECUinfo.ChkSumNumber));
									 ChksAvail->Text=IntToStr(ECUinfo.ChkSumNumber);
									 Logs->Lines->Add("Bad checksumm="+ IntToStr(ECUinfo.BadChkSumNumber));
									 ChksBad->Text=IntToStr(ECUinfo.BadChkSumNumber);
									 if (ECUinfo.BadChkSumNumber)
										{
                                         	Logs->Lines->Add("Need to fix dump");
										}
									 else
										{
                                            Logs->Lines->Add("Not need to fix dump");
                                        }
									 FreeLibrary (hDll);
									 break;
								}
					}
				FreeLibrary (hDll);
			}
	  }
	  while (FindNextFile(hFind, &FindFileData) != 0 && KnownECU!=1);

	  dwError = GetLastError();
	  FindClose(hFind);
/*	  if (dwError != ERROR_NO_MORE_FILES)
	  {
	  throw(Exception("FindNextFile error"));
	  }
*/
	  if (KnownECU==1)
		{
			StringCbCopyN (GoodModules, BUFSIZE, Modules_Buff, BUFSIZE);
			FixECU->Enabled=True;
		}
	  else
		{
			Logs->Lines->Add ("I dont know this dump");
			VirtualFree(OriBuffer, 0, MEM_RELEASE);
			OriBuffer=NULL;

        }
	}
}
//---------------------------------------------------------------------------

void PrintErrorMessage()
{
		LPVOID lpMsgBuf;

	FormatMessage
		(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
		);

// Display the string.
	MessageBox( NULL, (char*)lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );

// Free the buffer.
	LocalFree( lpMsgBuf );

}

void __fastcall TForm1::SaveFileClick(TObject *Sender)
{
		int OriFileHandle, writedbytes;

		Logs->Lines->Add("Select file to write...");
		if (SaveDialog1->Execute())
		Logs->Lines->Add("Writing to file:");
		Logs->Lines->Add(SaveDialog1->FileName);
		if (FileExists(SaveDialog1->FileName))
			{
				OriFileHandle=FileOpen (SaveDialog1->FileName,fmOpenWrite);
				Logs->Lines->Add("Target file exist. File will be overwritten");

			}
		else
			{
                OriFileHandle=FileCreate (SaveDialog1->FileName,fmOpenWrite);
            }

		FileSeek(OriFileHandle, 0, 0);	//Позиционируемся на начало файла

		writedbytes=FileWrite (OriFileHandle, ECUinfo.EcuBuffer, ECUinfo.EcuFileSize);
		if (writedbytes!=ECUinfo.EcuFileSize)
			{
				FileClose(OriFileHandle);
				Logs->Lines->Add("File write error.");
				throw(Exception("File write error."));
			}
		  Logs->Lines->Add ("Writed " + IntToStr (writedbytes) + " bytes");
		  FileClose(OriFileHandle);

}
//---------------------------------------------------------------------------




void __fastcall TForm1::AboutClick(TObject *Sender)
{
	 WIN32_FIND_DATA FindFileData;
	 HANDLE hFind = INVALID_HANDLE_VALUE;
	 DWORD dwError;
	 LPSTR DirSpec;

	   DirSpec = (LPSTR) malloc (BUFSIZE);
//	   TCHAR CurrDir_Buff[BUFSIZE];
	   TCHAR Modules_Buff[BUFSIZE];
//	   DWORD dwRet;

	   Logs->Lines->Clear();
	   Logs->Lines->Add("Iron Felix ver:1.0.0.1");
	   Logs->Lines->Add("by Pirat aka OzZY_RP");
	   Logs->Lines->Add("motronic383@yandex.ru");
	   Logs->Lines->Add("(c) 2011-2014");
	   Logs->Lines->Add("Available modules:");

//	   dwRet = GetCurrentDirectory(BUFSIZE, CurrDir_Buff);
//	   if (dwRet!=0) Logs->Lines->Add(CurrDir_Buff);
//	   if (dwRet==0) throw (Exception("GetCurrentDir Error"));

	   StringCbCopyN (DirSpec, BUFSIZE, CurrDir_Buff, dwRet);
	   StringCbCatN (DirSpec, BUFSIZE, "\\Modules\\*.dll", 16);

//	   	   Logs->Lines->Add(DirSpec);

   // Find the first file in the directory.
	   hFind = FindFirstFile(DirSpec, &FindFileData);

   if (hFind == INVALID_HANDLE_VALUE)
   {
//      dwError=GetLastError();
//	  Logs->Lines->Add (GetLastError());
	  throw(Exception("Invalid file handle"));
   }
   else
   {
//	   Logs->Lines->Add ( FindFileData.cFileName);
	   StringCbCopyN (Modules_Buff, BUFSIZE, CurrDir_Buff, dwRet);
	   StringCbCatN (Modules_Buff, BUFSIZE, "\\Modules\\", 11);
	   StringCbCatN (Modules_Buff, BUFSIZE, FindFileData.cFileName, StrLen(FindFileData.cFileName)+1);
//	   Logs->Lines->Add(Modules_Buff);
	   HMODULE hDll= LoadLibrary (Modules_Buff);
	   if (hDll)
			{
				GetLibVer_proc =(GetLibVer_Imp) GetProcAddress(hDll, GetLibVer);
				if (GetLibVer_proc)
					{
						Logs->Lines->Add(GetLibVer_proc());
					}
				FreeLibrary (hDll);
			}
		// List all the other files in the directory.
	  while (FindNextFile(hFind, &FindFileData) != 0)
      {
//		 Logs->Lines->Add (FindFileData.cFileName);
	   StringCbCopyN (Modules_Buff, BUFSIZE, CurrDir_Buff, dwRet);
	   StringCbCatN (Modules_Buff, BUFSIZE, "\\Modules\\", 11);
	   StringCbCatN (Modules_Buff, BUFSIZE, FindFileData.cFileName, StrLen(FindFileData.cFileName)+1);
	   HMODULE hDll= LoadLibrary (Modules_Buff);
	   if (hDll)
			{
				GetLibVer_proc =(GetLibVer_Imp) GetProcAddress(hDll, GetLibVer);
				if (GetLibVer_proc)
					{
						Logs->Lines->Add(GetLibVer_proc());
					}
				FreeLibrary (hDll);
			}
	  }

	  dwError = GetLastError();
	  FindClose(hFind);
	  if (dwError != ERROR_NO_MORE_FILES)
	  {
	  throw(Exception("FindNextFile error"));
	  }
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormCreate(TObject *Sender)
{
	ECUinfo.EcuBuffer=NULL;
	ECUinfo.EcuFileSize=0;
	ECUinfo.ChkSumNumber=0;
	ECUinfo.BadChkSumNumber=0;
//	DWORD dwRet;

	   dwRet = GetCurrentDirectory(BUFSIZE, CurrDir_Buff);
	   if (dwRet==0) throw (Exception("GetCurrentDir Error"));

}
//---------------------------------------------------------------------------


void __fastcall TForm1::FixECUClick(TObject *Sender)
{
	   int FixResult;

	   HMODULE hDll= LoadLibrary (GoodModules);
	   if (hDll)
			{
				FixChkSum_proc =(FixChkSum_Imp) GetProcAddress(hDll, FixChkSum);
				if (FixChkSum_proc)
					{
						FixResult=FixChkSum_proc(&ECUinfo);
						SaveFile->Enabled=False;

							if (FixResult)
								{
									Logs->Lines->Add("Fixed " + IntToStr(FixResult) + " from " + IntToStr(ECUinfo.ChkSumNumber) + " checksumm");
									ChksBad->Text=IntToStr(ECUinfo.BadChkSumNumber);
									ChksFix->Text=IntToStr(FixResult);
								}
							else
								{
									Logs->Lines->Add("Zero checksumm fixed");
									ChksBad->Text=IntToStr(ECUinfo.BadChkSumNumber);
									ChksFix->Text="0";
                            	}
					}
				else
					{
						FreeLibrary (hDll);
						FixECU->Enabled=False;
                        throw(Exception("Error loading module!"));
                    }
			   FreeLibrary (hDll);
            }
		 else
			{
				FixECU->Enabled=False;
				throw(Exception("Error loading module!"));
			}

       SaveFile->Enabled=True;

}
//---------------------------------------------------------------------------





