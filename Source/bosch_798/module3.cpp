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
	char* Ver_str="HYUNDAY Bosch M7.9.8 ECU v.1.0.0.1";
	return Ver_str;
}

int RecogniseECU (ECU_info* dllECUinfo)
{
	unsigned int i, s_crc, FindOffset, HexCount ;
	char* FileDescr="HYUNDAY Bosch M7.9.8";
//	char cf_crc[30];
	unsigned char HexBytes[]={0xF0, 0xEA, 0xF0, 0xFB, 0xE6, 0xFC, 0xF5, 0xFF, 0xE0, 0x9D, 0x0D, 0x13, 0xDC, 0x09};
	HexCount=14;
	DWORD Buffer= (DWORD)dllECUinfo->EcuBuffer;

	//Распознаем ECU
	if (dllECUinfo->EcuFileSize!=0xD0000 && dllECUinfo->EcuFileSize!=0xC0000)
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
	unsigned char HexBytes[]={0xF0, 0xEA, 0xF0, 0xFB, 0xE6, 0xFC, 0xF5, 0xFF, 0xE0, 0x9D, 0x0D, 0x13, 0xDC, 0x09};
	HexCount=14;
	DWORD Buffer= (DWORD)dllECUinfo->EcuBuffer;

	//Распознаем ECU
	if (dllECUinfo->EcuFileSize!=0xD0000 && dllECUinfo->EcuFileSize!=0xC0000)
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
	 unsigned int StartAdr1, EndAdr1, StartAdr2, EndAdr2, StartAdr3, EndAdr3, ChkSumAdr1, ChkSumAdr2, ChkSumAdr3, CS_off;
	 unsigned int ChkSumm1=0, ChkSumm2=0, ChkSumm3=0, CalcSumm1, CalcSumm2, CalcSumm3, TotalSumm=0, BadSumm=0, MultCSAdr;
	 //Signature Block CheckSumm
	 unsigned char CSummSig[]={0xDC, 0x0D, 0xA8, 0x8C, 0xE0, 0x09, 0x00, 0xE8, 0x10, 0xF9, 0xF0, 0x8C, 0xF0, 0x1D, 0x06, 0xF8};
	 unsigned int HexCount=16;

	 DWORD Buffer1 = (DWORD)dllECUinfo->EcuBuffer;
	 unsigned char* Buffer= (unsigned char*)(Buffer1);

	 //Calc and check Block Checksumm (803c)
	 StartAdr1=0;
	 CS_off=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , CSummSig, HexCount);
	 if (CS_off!=0xFFFFFFFF)
		{
		 StartAdr1=*(Buffer+CS_off-0x08)+(*(Buffer+CS_off-0x07)<<8)+(*(Buffer+CS_off-0x04)<<16)+(*(Buffer+CS_off-0x03)<<24);
		 EndAdr1=*(Buffer+CS_off+0x26)+(*(Buffer+CS_off+0x27)<<8)+(*(Buffer+CS_off+0x2A)<<16)+(*(Buffer+CS_off+0x2B)<<24);
			 if (dllECUinfo->EcuFileSize==0xC0000 && StartAdr1>=0x18000 && StartAdr1<0xA0000)
			 {
				 StartAdr1-=0x10000;
				 EndAdr1-=0x10000;
			 }
			 if (dllECUinfo->EcuFileSize==0xC0000 && StartAdr1>=0xC0000 && StartAdr1<0xCFFFF)
			 {
				 StartAdr1-=0x30000;
				 EndAdr1-=0x30000;
			 }
		}

	 CS_off=FindHexStr(CS_off+16, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , CSummSig, HexCount);
	 if (CS_off!=0xFFFFFFFF)
		{
		 StartAdr2=*(Buffer+CS_off-0x08)+(*(Buffer+CS_off-0x07)<<8)+(*(Buffer+CS_off-0x04)<<16)+(*(Buffer+CS_off-0x03)<<24);
		 EndAdr2=*(Buffer+CS_off+0x26)+(*(Buffer+CS_off+0x27)<<8)+(*(Buffer+CS_off+0x2A)<<16)+(*(Buffer+CS_off+0x2B)<<24);
		 ChkSumAdr1=*(Buffer+CS_off+0x3C)+(*(Buffer+CS_off+0x3D)<<8)+(*(Buffer+CS_off+0x38)<<14)+(*(Buffer+CS_off+0x39)<<22);
			 if (dllECUinfo->EcuFileSize==0xC0000 && StartAdr2>=0x18000 && StartAdr2<0xA0000)
			 {
				 StartAdr1-=0x10000;
				 EndAdr1-=0x10000;
			 }
			 if (dllECUinfo->EcuFileSize==0xC0000 && StartAdr2>=0xC0000 && StartAdr2<0xCFFFF)
			 {
				 StartAdr2-=0x30000;
				 EndAdr2-=0x30000;
			 }
			 if (dllECUinfo->EcuFileSize==0xC0000 && ChkSumAdr1>=0x18000 && ChkSumAdr1<0xA0000)
			 {
				 ChkSumAdr1-=0x10000;
			 }
			 if (dllECUinfo->EcuFileSize==0xC0000 && ChkSumAdr1>=0xC0000 && ChkSumAdr1<0xCFFFF)
			 {
				 ChkSumAdr1-=0x30000;
			 }
		}
	 if(StartAdr1!=0x00)
	 {
		TotalSumm++;
		ChkSumm1=Read4ByteIntel(Buffer+ChkSumAdr1);
		CalcSumm1=SummBlock(StartAdr1, EndAdr1, Buffer);
		CalcSumm1+=SummBlock(StartAdr2, EndAdr2, Buffer);
			 if (ChkSumm1!=CalcSumm1)
				{
					if (FixCRC)
						{
							Write4ByteIntel(CalcSumm1, Buffer+ChkSumAdr1);
						}
					BadSumm++;
				}
	 }

	//Cacl and check full block checksumm
	 StartAdr1=0x18000; 	// Start and end address of block for checksumm
	 EndAdr1=0x9FFF5;
	 StartAdr2=0xA0000;
	 EndAdr2=0xBFFF5;
	 StartAdr3=0xC0000;
	 EndAdr3=0xCFFF5;
	 ChkSumAdr1=0x9FFF6;
	 ChkSumAdr2=0xBFFF6;
	 ChkSumAdr3=0xCFFF6;



	 if (dllECUinfo->EcuFileSize==0xC0000)
	 {
		 StartAdr1-=0x10000; 	// Start and end address of block for checksumm for 786K file
		 EndAdr1-=0x10000;
//		 StartAdr2-=0x10000;
//		 EndAdr2-=0x10000;
		 StartAdr3-=0x30000;
		 EndAdr3-=0x30000;
		 ChkSumAdr1-=0x10000;
//		 ChkSumAdr2-=0x10000;
		 ChkSumAdr3-=0x30000;
	 }


	 ChkSumm1= Read4ByteIntel(Buffer+ChkSumAdr1);
	 ChkSumm2= Read4ByteIntel(Buffer+ChkSumAdr2);
	 ChkSumm3= Read4ByteIntel(Buffer+ChkSumAdr3);
	 TotalSumm+=3;

	 CalcSumm1=SummInt16Intel(StartAdr1, EndAdr1, Buffer);
	 CalcSumm2=SummInt16Intel(StartAdr2, EndAdr2, Buffer);
	 CalcSumm3=SummInt16Intel(StartAdr3, EndAdr3, Buffer);


	 if (ChkSumm1==(Read4ByteIntel(Buffer+ChkSumAdr1+4)^0xFFFFFFFF))
	 {
		 if (ChkSumm1!=CalcSumm1)
			{
				if (FixCRC)
					{
						Write4ByteIntel(CalcSumm1, Buffer+ChkSumAdr1);
						Write4ByteIntel(CalcSumm1^0xFFFFFFFF, Buffer+ChkSumAdr1+4);
					}
				BadSumm++;
			}
	 }
	 else
	 {
		if (FixCRC)
			{
				Write4ByteIntel(CalcSumm1, Buffer+ChkSumAdr1);
				Write4ByteIntel(CalcSumm1^0xFFFFFFFF, Buffer+ChkSumAdr1+4);
			}
		BadSumm++;
	 }

	 if (ChkSumm2==(Read4ByteIntel(Buffer+ChkSumAdr2+4)^0xFFFFFFFF))
	 {
		 if (ChkSumm2!=CalcSumm2)
			{
				if (FixCRC)
					{
						Write4ByteIntel(CalcSumm2, Buffer+ChkSumAdr2);
						Write4ByteIntel(CalcSumm2^0xFFFFFFFF, Buffer+ChkSumAdr2+4);
					}
				BadSumm++;
			}
	 }
	 else
	 {
		if (FixCRC)
			{
				Write4ByteIntel(CalcSumm2, Buffer+ChkSumAdr2);
				Write4ByteIntel(CalcSumm2^0xFFFFFFFF, Buffer+ChkSumAdr2+4);
			}

		BadSumm++;
     }

	 if (ChkSumm3==(Read4ByteIntel(Buffer+ChkSumAdr3+4)^0xFFFFFFFF))
		{
		 if (ChkSumm3!=CalcSumm3)
			{
				if (FixCRC)
					{
						Write4ByteIntel(CalcSumm3, Buffer+ChkSumAdr3);
						Write4ByteIntel(CalcSumm3^0xFFFFFFFF, Buffer+ChkSumAdr3+4);
					}
				BadSumm++;
			}
		 }
	 else
	 {
		if (FixCRC)
			{
				Write4ByteIntel(CalcSumm3, Buffer+ChkSumAdr3);
				Write4ByteIntel(CalcSumm3^0xFFFFFFFF, Buffer+ChkSumAdr3+4);
			}
		BadSumm++;
	 }


     //Cacl and check multipoint checksumm
	 for (MultCSAdr = 0xBBBDE; MultCSAdr < 0xBBEDE; MultCSAdr+=0x10)
	 {
		TotalSumm++;
		StartAdr1=Read4ByteIntel(Buffer+MultCSAdr);
		EndAdr1=Read4ByteIntel(Buffer+MultCSAdr+4);
	 if (dllECUinfo->EcuFileSize==0xC0000 && StartAdr1>=0x18000 && StartAdr1<0xA0000)
	 {
		 StartAdr1-=0x10000;
		 EndAdr1-=0x10000;
	 }
	 if (dllECUinfo->EcuFileSize==0xC0000 && StartAdr1>=0xC0000 && StartAdr1<0xCFFFF)
	 {
		 StartAdr1-=0x30000;
		 EndAdr1-=0x30000;
	 }


	 if (EndAdr1<=dllECUinfo->EcuFileSize)
	 {
		ChkSumm1= Read4ByteIntel(Buffer+MultCSAdr+8);
		CalcSumm1=SummInt16Intel(StartAdr1, EndAdr1, Buffer);
		 if (ChkSumm1==(Read4ByteIntel(Buffer+MultCSAdr+12)^0xFFFFFFFF))
			{
			 if (ChkSumm1!=CalcSumm1)
				{
					if (FixCRC)
						{
							Write4ByteIntel(CalcSumm1, Buffer+MultCSAdr+8);
							Write4ByteIntel(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
						}
					BadSumm++;
				}
			 }
		 else
		 {
			if (FixCRC)
				{
					Write4ByteIntel(CalcSumm1, Buffer+MultCSAdr+8);
					Write4ByteIntel(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
				}
			BadSumm++;
	     }
	 }
	 else break;
	 }


	 dllECUinfo->BadChkSumNumber=BadSumm;  //Bad CheckSumm number in dump
	 dllECUinfo->ChkSumNumber=TotalSumm;  //CheckSumm number in dump
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

unsigned int SummBlock(unsigned int StartBlock, unsigned int EndBlock, unsigned char *SearchBuffer)
{
	unsigned int summ=0, i;
	for (i = StartBlock; i < EndBlock; i=i+0x2000)
		{
			summ+= *(SearchBuffer+i);
			summ+= *(SearchBuffer+i+1)<<8;
			summ+= *(SearchBuffer+i+0x1FFE);
			summ+= *(SearchBuffer+i+0x1FFF)<<8;
		}
	return summ;

}
