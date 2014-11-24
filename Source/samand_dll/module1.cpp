//---------------------------------------------------------------------------

#include <windows.h>
//---------------------------------------------------------------------------
//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you
//---------------------------------------------------------------------------

#pragma argsused
#include <intsafe.h>
#include <strsafe.h>

#include <Modules_exp.h>

int FindFixCRC(ECU_info* dllECUinfo, int FixCRC);
//Ищет или исправляет КС
//Если FixCRC=1 то исправляет
//Возвращает 1 если успешно, 0 если ошибка

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

char* GetLibVer()
{
	char* Ver_str="Sagem Iran Khodro ECU v.1.0.0.1";
	return Ver_str;
}

int RecogniseECU (ECU_info* dllECUinfo)
{
	unsigned int i, s_crc, FindOffset, HexCount ;
	char* FileDescr="Sagem Iran Khodro";
//	char cf_crc[30];
	unsigned char HexBytes[]={0xDC, 0x06, 0xA8, 0x41, 0x08, 0x12, 0x18, 0x60, 0xC0, 0x85, 0x00, 0x35, 0xC0, 0x95, 0x00, 0x35};
	HexCount=16;
	DWORD Buffer= (DWORD)dllECUinfo->EcuBuffer;

	//Распознаем ECU
	if (dllECUinfo->EcuFileSize!=0xD0000)
		{
			return 0;
		}

	FindOffset=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , HexBytes, HexCount);
	if (FindOffset==0xFFFFFFFF)
		{
			return 0;
		}
	if (!FindFixCRC(dllECUinfo, 0))
	{
		return 0;
	}

/*	for (i = 0; i < dllECUinfo->EcuFileSize; i++)
		{
			f_crc+= *(unsigned char*)(Buffer+i);
		}
*/
	StringCbLength(FileDescr, 50, &s_crc);
	StringCbCopyN (dllECUinfo->ECU_Descr, 100, FileDescr, s_crc);
/*	IntToChar (f_crc, cf_crc);
	StringCbLength(cf_crc, 50, &s_crc);
	StringCbCatN (dllECUinfo->ECU_Descr, 100, cf_crc, s_crc+1 );
*/
/*	FindOffset=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , HexBytes, HexCount);
	f_crc=SummInt8(0, dllECUinfo->EcuFileSize-1, (unsigned char*)Buffer );
	s_crc=SummInt16Intel(0, dllECUinfo->EcuFileSize-1, (unsigned char*)Buffer );
	s_crc=SummInt16Mot(0, dllECUinfo->EcuFileSize-1, (unsigned char*)Buffer );
*/


	return 1;

}

int FixChkSum(ECU_info* dllECUinfo)
{
	unsigned int FindOffset, HexCount ;
	unsigned char HexBytes[]={0xDC, 0x06, 0xA8, 0x41, 0x08, 0x12, 0x18, 0x60, 0xC0, 0x85, 0x00, 0x35, 0xC0, 0x95, 0x00, 0x35};
	HexCount=16;
	DWORD Buffer= (DWORD)dllECUinfo->EcuBuffer;

	//Распознаем ECU
	if (dllECUinfo->EcuFileSize!=0xD0000)
		{
			return 0;
		}
	FindOffset=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , HexBytes, HexCount);
	if (FindOffset==0xFFFFFFFF)
		{
			return 0;
		}
	if (!FindFixCRC(dllECUinfo, 1))
	{
		return 0;
	}
	return dllECUinfo->BadChkSumNumber;

}

int FindFixCRC(ECU_info* dllECUinfo, int FixCRC)
{
	 unsigned int StartAdr1, EndAdr1, StartAdr2, EndAdr2, StartAdr3, EndAdr3, ChkSumAdr1, ChkSumAdr2;
	 unsigned int ChkSumm1=0, ChkSumm2=0, CalcSumm1, CalcSumm2, BadSumm=0;

	 StartAdr1=0x02000; 	// Start and end address of block for checksumm
	 EndAdr1=0x03FFF;
	 StartAdr2=0x10000;
	 EndAdr2=0x6FFFD;
	 StartAdr3=0x72806;
	 EndAdr3=0x7FFFF;
	 ChkSumAdr1=0x6FFFE;
	 ChkSumAdr2=0x72804;

	 dllECUinfo->ChkSumNumber=2;  //CheckSumm number in dump

	 DWORD Buffer1 = (DWORD)dllECUinfo->EcuBuffer;
	 unsigned char* Buffer= (unsigned char*)(Buffer1);
	 ChkSumm1= *(Buffer+ChkSumAdr1) + (*(Buffer+ChkSumAdr1+1)<<8);
	 ChkSumm2= *(Buffer+ChkSumAdr2) + (*(Buffer+ChkSumAdr2+1)<<8);

	 CalcSumm1=SummInt8(StartAdr1, EndAdr1, Buffer);
	 CalcSumm1+=SummInt8(StartAdr2, EndAdr2, Buffer);
	 CalcSumm1&=0xFFFF;
	 CalcSumm2=SummInt8(StartAdr3, EndAdr3, Buffer);
	 CalcSumm2&=0xFFFF;

	 if (ChkSumm1!=CalcSumm1)
		{
			if (FixCRC)
				{
					*(Buffer+ChkSumAdr1)=CalcSumm1;
					*(Buffer+ChkSumAdr1+1)=CalcSumm1>>8;
				}
			BadSumm++;
		}
	 if (ChkSumm2!=CalcSumm2)
		{
			if (FixCRC)
				{
					*(Buffer+ChkSumAdr2)=CalcSumm2;
					*(Buffer+ChkSumAdr2+1)=CalcSumm2>>8;
				}

			BadSumm++;
		}

	 dllECUinfo->BadChkSumNumber=BadSumm;  //Bad CheckSumm number in dump
     return 1;
}

unsigned int FindHexStr(unsigned int SearchOffset, unsigned int BufferSize, unsigned char *SearchBuffer, unsigned char *HexBytes, unsigned int HexCount)
{
	unsigned int i, h_bytes, s_count=0;
	for (i = SearchOffset; i < BufferSize-HexCount; i++)
		{
			if (*(SearchBuffer+i)==*HexBytes)
				{
					s_count=1;
					if (HexCount==1)
						{
                        	return i;
						}
					for (h_bytes = 1; h_bytes < HexCount; h_bytes++)
						{
							if (*(SearchBuffer+i+h_bytes)==*(HexBytes+h_bytes))
								{
								   s_count++;
								}
							else
								{
									s_count=0;
									break;
								}
						}
					 if (s_count==HexCount)
						{
							return i;
						}
				}
		}
	return 0xFFFFFFFF;
}

unsigned int SummInt8(unsigned int StartInt8, unsigned int EndInt8, unsigned char *SearchBuffer)
{
	unsigned int summ=0, i;
	for (i = StartInt8; i <= EndInt8; i++)
		{
			summ+= *(SearchBuffer+i);
		}
	return summ;
}

unsigned int SummInt16Intel(unsigned int StartInt16, unsigned int EndInt16, unsigned char *SearchBuffer)
{
	unsigned int summ=0, i;
	for (i = StartInt16; i < EndInt16; i=i+2)
		{
			summ+= *(SearchBuffer+i);
			summ+= *(SearchBuffer+i+1)<<8;
		}
	return summ;
}

unsigned int SummInt16Mot(unsigned int StartInt16, unsigned int EndInt16, unsigned char *SearchBuffer)
{
	unsigned int summ=0, i;
	for (i = StartInt16; i < EndInt16; i=i+2)
		{
			summ+= *(SearchBuffer+i+1);
			summ+= *(SearchBuffer+i)<<8;
		}
	return summ;
}

unsigned int Read4ByteIntel (unsigned char *ReadAddress)
{
	unsigned int  ReadedDword;
	ReadedDword= *(ReadAddress)+(*(ReadAddress+1)<<8)+(*(ReadAddress+2)<<16)+(*(ReadAddress+3)<<24);
	return ReadedDword;
}

void Write4ByteIntel (unsigned int WrDword, unsigned char *WriteAddress)
{
	*WriteAddress= WrDword;
	*(WriteAddress+1)= WrDword>>8;
	*(WriteAddress+2)= WrDword>>16;
	*(WriteAddress+3)= WrDword>>24;
}