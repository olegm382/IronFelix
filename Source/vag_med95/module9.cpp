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
#include <md5.c>
#include <gmp/gmp.h>

#define ECUFILESIZE 0x200000

int FindFixCRC(ECU_info* dllECUinfo, int FixCRC);
//Search or correct checksum
//If FixCRC=1 then correct
//Return 1 if OK, 0 if error



int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

char* GetLibVer()
{
	char* Ver_str="VAG Bosch MED9.5.10 ECU v.1.0.0.1";
	return Ver_str;
}

int RecogniseECU (ECU_info* dllECUinfo)
{
	unsigned int i, s_crc, FindOffset, HexCount1, HexCount2, HexCount3 ;
	char* FileDescr="VAG Bosch MED9.5";

	unsigned char HexBytes1[]={0xA1, 0x86, 0x00, 0x00, 0x7C, 0xA5, 0x62, 0x14, 0xA1, 0x66, 0x00, 0x02, 0x7C, 0xA5, 0x5A, 0x14, 0x38, 0xC6, 0x00, 0x04, 0x7C, 0x06, 0x20, 0x40, 0x41, 0x80, 0xFF, 0xE8, 0x7C, 0xA3, 0x2B, 0x78 };
	unsigned char HexBytes2[]={0x7D, 0x84, 0x28, 0xAE, 0x7D, 0x63, 0x28, 0xAE, 0x7C, 0x0C, 0x58, 0x00, 0x41, 0x82, 0x00, 0x18 };
	HexCount1=32;
	HexCount2=16;

	DWORD Buffer= (DWORD)dllECUinfo->EcuBuffer;


	//Recognise ECU
	if (dllECUinfo->EcuFileSize!=ECUFILESIZE)
		{
			return 0;
		}

	FindOffset=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , HexBytes1, HexCount1);
	if (FindOffset==0xFFFFFFFF)
		{
			return 0;
		}
	FindOffset=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , HexBytes2, HexCount2);
	if (FindOffset==0xFFFFFFFF)
		{
			return 0;
		}

	if (!FindFixCRC(dllECUinfo, 0))
	{
		return 0;
	}

	StringCbLength(FileDescr, 50, &s_crc);
	StringCbCopyN (dllECUinfo->ECU_Descr, 100, FileDescr, s_crc);

	return 1;

}

int FixChkSum(ECU_info* dllECUinfo)
{
	unsigned int FindOffset, HexCount1, HexCount2, HexCount3;
	unsigned char HexBytes1[]={0xA1, 0x86, 0x00, 0x00, 0x7C, 0xA5, 0x62, 0x14, 0xA1, 0x66, 0x00, 0x02, 0x7C, 0xA5, 0x5A, 0x14, 0x38, 0xC6, 0x00, 0x04, 0x7C, 0x06, 0x20, 0x40, 0x41, 0x80, 0xFF, 0xE8, 0x7C, 0xA3, 0x2B, 0x78 };
	unsigned char HexBytes2[]={0x7D, 0x84, 0x28, 0xAE, 0x7D, 0x63, 0x28, 0xAE, 0x7C, 0x0C, 0x58, 0x00, 0x41, 0x82, 0x00, 0x18 };
	HexCount1=32;
	HexCount2=16;

	DWORD Buffer= (DWORD)dllECUinfo->EcuBuffer;


	//Recognise ECU
	if (dllECUinfo->EcuFileSize!=ECUFILESIZE)
		{
			return 0;
		}

	FindOffset=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , HexBytes1, HexCount1);
	if (FindOffset==0xFFFFFFFF)
		{
			return 0;
		}
	FindOffset=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , HexBytes2, HexCount2);
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
	 DWORD Buffer1 = (DWORD)dllECUinfo->EcuBuffer;
	 unsigned char* Buffer= (unsigned char*)(Buffer1);
	 unsigned int FindOffset, EcuSubType, StartAdr1, EndAdr1, ChkSumAdr1, CS_off, CS_count;
	 unsigned int r2_base=0x5C9FF0;
	 unsigned int ChkSumm1=0, CalcSumm1, TotalSumm=0, BadSumm=0, MultCSAdr;
	 //Signature Block CheckSumm
	 unsigned char EcuBlockSig1[]={0x81, 0x43, 0x00, 0x00, 0x81, 0x23, 0x00, 0x04, 0x7C, 0x0A, 0x48, 0x40, 0x41, 0x81, 0x00, 0x18};
	 unsigned int EcuHexCount1=16;

	 md5_t	md5;
	 unsigned char	MD5CalcSig[MD5_SIZE], MD5DecodedSig[MD5_SIZE];
	 unsigned int MD5_StartAdr, MD5_EndAdr, MD5BlkStart, MD5BlkEnd, md5sum, RSAModulus, RSASignature, RSAExponent, RSAFix;
	 //Signature for MD5
	 unsigned char MD5Sig1[]={0x7D, 0x4A, 0x5B, 0x78, 0x7D, 0x9D, 0xF8, 0xAE, 0x7D, 0x4A, 0x63, 0x78, 0x7D, 0x5E, 0xF9, 0x2E};
	 unsigned char MD5Sig2[]={0x39, 0x00, 0x00, 0x00, 0x3B, 0xE0, 0x00, 0x7F, 0x2C, 0x1F, 0x00, 0x00, 0x41, 0x80, 0x00, 0x64};
	 unsigned int MD5HexCount1=16;
	 unsigned int MD5HexCount2=16;
	 unsigned int RSAPubExp=0x03;
	 unsigned char RSABuf[0x80];
	 unsigned char RSAPubModulus[]={
									0x79, 0xD3, 0xD0, 0x1A, 0xB7, 0xE3, 0x6C, 0x98,
									0x4B, 0xBB, 0x54, 0xF5, 0x1C, 0x69, 0xA4, 0x91,
									0xF7, 0xD6, 0xD6, 0x82, 0xA4, 0xC6, 0x8B, 0x77,
									0x0D, 0x2F, 0xEE, 0xFA, 0x3D, 0x43, 0xFF, 0x4C,
									0x6B, 0xED, 0xF6, 0xA7, 0x1F, 0xE1, 0xF8, 0xB3,
									0x54, 0x66, 0x1E, 0x5D, 0x0E, 0x54, 0x87, 0xB6,
									0x66, 0x2B, 0x60, 0x9A, 0x25, 0x07, 0xA9, 0xBC,
									0x8F, 0x3C, 0xFA, 0x56, 0xDE, 0xAE, 0xD7, 0x83,
									0x9E, 0xBF, 0x74, 0xBB, 0x94, 0x22, 0x5C, 0x7D,
									0xB7, 0x2C, 0x3D, 0x1E, 0x6A, 0xDE, 0xC9, 0xB7,
									0x87, 0xA6, 0xF2, 0x2E, 0xFD, 0x87, 0x12, 0x46,
									0x4E, 0xDE, 0xA5, 0x46, 0x2C, 0x22, 0x45, 0x61,
									0x97, 0x07, 0x08, 0x99, 0x21, 0x44, 0x64, 0x36,
									0xDF, 0x53, 0xA1, 0x84, 0xA6, 0x34, 0xC4, 0xF5,
									0x98, 0x6F, 0xE6, 0x23, 0x5C, 0xF2, 0x1D, 0x7E,
									0x7A, 0x25, 0xA0, 0x52, 0xFB, 0xDA, 0x48, 0xB3
									};
	 unsigned char RSAPrivExp[]={
								0x51, 0x37, 0xE0, 0x11, 0xCF, 0xEC, 0xF3, 0x10,
								0x32, 0x7C, 0xE3, 0x4E, 0x12, 0xF1, 0x18, 0x61,
								0x4F, 0xE4, 0x8F, 0x01, 0xC3, 0x2F, 0x07, 0xA4,
								0xB3, 0x75, 0x49, 0xFC, 0x28, 0xD7, 0xFF, 0x88,
								0x47, 0xF3, 0xF9, 0xC4, 0xBF, 0xEB, 0xFB, 0x22,
								0x38, 0x44, 0x14, 0x3E, 0x09, 0x8D, 0xAF, 0xCE,
								0xEE, 0xC7, 0x95, 0xBC, 0x18, 0xAF, 0xC6, 0x7D,
								0xB4, 0xD3, 0x51, 0x8F, 0x3F, 0x1F, 0x3A, 0x56,
								0xCF, 0x8E, 0xE4, 0x2D, 0x8A, 0x89, 0x30, 0x51,
								0x9F, 0x4B, 0x89, 0xF9, 0x87, 0xAF, 0x55, 0xA1,
								0x3A, 0x86, 0x61, 0x4A, 0xC2, 0xA9, 0x9B, 0x74,
								0x24, 0xA3, 0xC1, 0x8C, 0x4D, 0x22, 0xDE, 0xCF,
								0x0C, 0xB6, 0xA9, 0x0E, 0x30, 0xDB, 0xA1, 0x22,
								0x4C, 0xFD, 0xC3, 0xDC, 0x5E, 0x41, 0xC2, 0x58,
								0xE2, 0x61, 0xCE, 0xEF, 0xDE, 0x44, 0x0E, 0x36,
								0xC8, 0x5B, 0x34, 0x38, 0xBB, 0x43, 0x41, 0x2B
								};
	 mpz_t PubExp, PrivExp, Modulus, CalcSig, DecodeSig, Sig;

		//CRC32 and RSA
		 CS_off=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , MD5Sig1, MD5HexCount1);
		 if (CS_off!=0xFFFFFFFF)
			{
			 MD5BlkEnd=*(Buffer+CS_off+0x47)+(*(Buffer+CS_off+0x46)<<8)+(*(Buffer+CS_off+0x43)<<16)+(*(Buffer+CS_off+0x42)<<24);
			 if (*(Buffer+CS_off+0x46)& 0x80) { MD5BlkEnd-=0x10000;}
			 MD5BlkStart=*(Buffer+CS_off+0x4F)+(*(Buffer+CS_off+0x4E)<<8)+(*(Buffer+CS_off+0x4B)<<16)+(*(Buffer+CS_off+0x4A)<<24);
			 if (*(Buffer+CS_off+0x4E)& 0x80) { MD5BlkStart-=0x10000;}
			}
		 else
			{
				 return 0;
			}
		 //Calc MD5
		 md5_init(&md5);
		 MD5_StartAdr=Read4ByteMot(Buffer+MD5BlkStart);
		 MD5_EndAdr=Read4ByteMot(Buffer+MD5BlkEnd);
		 md5_process(&md5, Buffer+MD5_StartAdr, MD5_EndAdr-MD5_StartAdr+1);
		 md5_finish (&md5, MD5CalcSig);

		 CS_off=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , MD5Sig2, MD5HexCount2);
		 if (CS_off!=0xFFFFFFFF)
			{
			 RSASignature=*(Buffer+CS_off-0x09)+(*(Buffer+CS_off-0x0A)<<8)+(*(Buffer+CS_off-0x0D)<<16)+(*(Buffer+CS_off-0x0E)<<24);
			 if (*(Buffer+CS_off-0x0A)& 0x80) { RSASignature-=0x10000;}
			 RSAModulus=*(Buffer+CS_off+0x97)+(*(Buffer+CS_off+0x96)<<8)+r2_base-0x400000;
			 if (*(Buffer+CS_off+0x96)& 0x80) { RSAModulus-=0x10000;}
			 RSAExponent=*(Buffer+CS_off+0x137)+(*(Buffer+CS_off+0x136)<<8)+r2_base-0x400000;
			 if (*(Buffer+CS_off+0x136)& 0x80) { RSAExponent-=0x10000;}
			}
		 else
			{
				 return 0;
			}
		 //Decode RSA Signature
		 RSAFix=0;
		 mpz_init (PubExp);
		 mpz_init (Modulus);
		 mpz_init (DecodeSig);
		 mpz_init (Sig);
		 mpz_import (Sig, 0x80, 1, 1, 0, 0, Buffer+RSASignature);
		 mpz_import (Modulus, 0x80, 1, 1, 0, 0, Buffer+RSAModulus);
		 mpz_import (PubExp, 4, 1, 1, 0, 0, Buffer+RSAExponent);
		 mpz_powm (DecodeSig, Sig, PubExp, Modulus);
		 size_t rsa_size;
		 rsa_size=mpz_sizeinbase(DecodeSig, 2);
		 rsa_size=rsa_size/8+1;
		 memset(RSABuf, 0x00, 0x80);
		 mpz_export(RSABuf+0x80-rsa_size, &rsa_size, 1, 1, 0, 0, DecodeSig);
		 memcpy(MD5DecodedSig, RSABuf+0x70, 0x10);
		 mpz_clear (PubExp);
		 mpz_clear (Modulus);
		 mpz_clear (DecodeSig);
		 mpz_clear (Sig);

		 TotalSumm++;
			 if (memcmp(MD5CalcSig, MD5DecodedSig, 0x10))
				{
					if (FixCRC)
						{
							 //Update Modulus, Exponent, recalc MD5 and RSA
						 *(Buffer+RSAExponent)=0;
						 *(Buffer+RSAExponent+1)=0;
						 *(Buffer+RSAExponent+2)=0;
						 *(Buffer+RSAExponent+3)=RSAPubExp;
						 memcpy(Buffer+RSAModulus, RSAPubModulus, 0x80);
						 RSAFix=1;//Flag for recalc RSA
						 md5_init(&md5);
						 md5_process(&md5, Buffer+MD5_StartAdr, MD5_EndAdr-MD5_StartAdr+1);
						 md5_finish (&md5, MD5CalcSig);
						 //Encode new RSA Signature
						 mpz_init (PrivExp);
						 mpz_init (Modulus);
						 mpz_init (CalcSig);
						 mpz_init (Sig);
						 RSABuf[0]=0x00;
						 RSABuf[1]=0x01;
						 memset (RSABuf+2, 0xFF, 0x6D);
						 RSABuf[0x6F]=0x00;
						 memcpy(RSABuf+0x70, MD5CalcSig, 0x10);
						 mpz_import (Sig, 0x80, 1, 1, 0, 0, RSABuf);
						 mpz_import (Modulus, 0x80, 1, 1, 0, 0, RSAPubModulus);
						 mpz_import (PrivExp, 0x80, 1, 1, 0, 0, RSAPrivExp);
						 mpz_powm (CalcSig, Sig, PrivExp, Modulus);
						 rsa_size=mpz_sizeinbase(CalcSig, 2);
						 rsa_size=rsa_size/8+1;
						 mpz_export(RSABuf, &rsa_size, 1, 1, 0, 0, CalcSig);
						 mpz_clear (PrivExp);
						 mpz_clear (Modulus);
						 mpz_clear (CalcSig);
						 mpz_clear (Sig);
						 memcpy(Buffer+RSASignature, RSABuf, 0x80);
						}
					BadSumm++;
				}

		 //First CS Block
		 CS_off=FindHexStr(0, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , EcuBlockSig1, EcuHexCount1);
		 if (CS_off!=0xFFFFFFFF)
			{
			 MultCSAdr=*(Buffer+CS_off-0x0D)+(*(Buffer+CS_off-0x0E)<<8)+(*(Buffer+CS_off-0x11)<<16)+(*(Buffer+CS_off-0x12)<<24);
			 if (*(Buffer+CS_off-0x0E)& 0x80) { MultCSAdr-=0x10000;}
			}
		 else
			{
				 return 0;
			}

			TotalSumm++;

				StartAdr1=Read4ByteMot(Buffer+MultCSAdr);
				EndAdr1=Read4ByteMot(Buffer+MultCSAdr+4);
				ChkSumm1= Read4ByteMot(Buffer+MultCSAdr+8);
				CalcSumm1=SummInt16Mot(StartAdr1, EndAdr1, Buffer);
				 if (ChkSumm1==(Read4ByteMot(Buffer+MultCSAdr+12)^0xFFFFFFFF))
					{
					 if (ChkSumm1!=CalcSumm1)
						{
							if (FixCRC)
								{
									Write4ByteMot(CalcSumm1, Buffer+MultCSAdr+8);
									Write4ByteMot(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
								}
							BadSumm++;
						}
					 }
				 else
				 {
					if (FixCRC)
						{
							Write4ByteMot(CalcSumm1, Buffer+MultCSAdr+8);
							Write4ByteMot(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
						}
					BadSumm++;
				 }

		 //Second CS Block
		 CS_off=FindHexStr(CS_off+16, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , EcuBlockSig1, EcuHexCount1);
		 if (CS_off!=0xFFFFFFFF)
			{
			 MultCSAdr=*(Buffer+CS_off-0x0D)+(*(Buffer+CS_off-0x0E)<<8)+(*(Buffer+CS_off-0x11)<<16)+(*(Buffer+CS_off-0x12)<<24);
			 if (*(Buffer+CS_off-0x0E)& 0x80) { MultCSAdr-=0x10000;}
			}
		 else
			{
				 return 0;
			}

			MultCSAdr+=0x10; //Correct Adress with CS number :)

		  for (CS_count = 0; CS_count < 4; CS_count++)
		  {
				TotalSumm++;

				StartAdr1=Read4ByteMot(Buffer+MultCSAdr);
				EndAdr1=Read4ByteMot(Buffer+MultCSAdr+4);
				ChkSumm1= Read4ByteMot(Buffer+MultCSAdr+8);
				CalcSumm1=SummInt16Mot(StartAdr1, EndAdr1, Buffer);
				 if (ChkSumm1==(Read4ByteMot(Buffer+MultCSAdr+12)^0xFFFFFFFF))
					{
					 if (ChkSumm1!=CalcSumm1)
						{
							if (FixCRC)
								{
									Write4ByteMot(CalcSumm1, Buffer+MultCSAdr+8);
									Write4ByteMot(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
								}
							BadSumm++;
						}
					 }
				 else
				 {
					if (FixCRC)
						{
							Write4ByteMot(CalcSumm1, Buffer+MultCSAdr+8);
							Write4ByteMot(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
						}
					BadSumm++;
				 }
            MultCSAdr+=0x10;
		  }

		 //Third CS Block
		 CS_off=FindHexStr(CS_off+16, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , EcuBlockSig1, EcuHexCount1);
		 if (CS_off!=0xFFFFFFFF)
			{
			 MultCSAdr=*(Buffer+CS_off-0x0D)+(*(Buffer+CS_off-0x0E)<<8)+r2_base-0x400000;
			 if (*(Buffer+CS_off-0x0E)& 0x80) { MultCSAdr-=0x10000;}
			}
		 else
			{
				 return 0;
			}

			MultCSAdr+=0x40; //Correct Adress with CS number :)

		  for (CS_count = 0; CS_count < 6; CS_count++)
		  {
				TotalSumm++;

				StartAdr1=Read4ByteMot(Buffer+MultCSAdr)-0x400000;
				EndAdr1=Read4ByteMot(Buffer+MultCSAdr+4)-0x400000;
				ChkSumm1= Read4ByteMot(Buffer+MultCSAdr+8);
				CalcSumm1=SummInt16Mot(StartAdr1, EndAdr1, Buffer);
				 if (ChkSumm1==(Read4ByteMot(Buffer+MultCSAdr+12)^0xFFFFFFFF))
					{
					 if (ChkSumm1!=CalcSumm1)
						{
							if (FixCRC)
								{
									Write4ByteMot(CalcSumm1, Buffer+MultCSAdr+8);
									Write4ByteMot(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
								}
							BadSumm++;
						}
					 }
				 else
				 {
					if (FixCRC)
						{
							Write4ByteMot(CalcSumm1, Buffer+MultCSAdr+8);
							Write4ByteMot(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
						}
					BadSumm++;
				 }
			MultCSAdr+=0x10;
		  }

			if (RSAFix) //Recalc RSA after CS block fix
				{
				 md5_init(&md5);
				 md5_process(&md5, Buffer+MD5_StartAdr, MD5_EndAdr-MD5_StartAdr+1);
				 md5_finish (&md5, MD5CalcSig);
				 //Encode new RSA Signature
				 mpz_init (PrivExp);
				 mpz_init (Modulus);
				 mpz_init (CalcSig);
				 mpz_init (Sig);
				 RSABuf[0]=0x00;
				 RSABuf[1]=0x01;
				 memset (RSABuf+2, 0xFF, 0x6D);
				 RSABuf[0x6F]=0x00;
				 memcpy(RSABuf+0x70, MD5CalcSig, 0x10);
				 mpz_import (Sig, 0x80, 1, 1, 0, 0, RSABuf);
				 mpz_import (Modulus, 0x80, 1, 1, 0, 0, RSAPubModulus);
				 mpz_import (PrivExp, 0x80, 1, 1, 0, 0, RSAPrivExp);
				 mpz_powm (CalcSig, Sig, PrivExp, Modulus);
				 rsa_size=mpz_sizeinbase(CalcSig, 2);
				 rsa_size=rsa_size/8+1;
				 mpz_export(RSABuf, &rsa_size, 1, 1, 0, 0, CalcSig);
				 mpz_clear (PrivExp);
				 mpz_clear (Modulus);
				 mpz_clear (CalcSig);
				 mpz_clear (Sig);
				 memcpy(Buffer+RSASignature, RSABuf, 0x80);
				}


		 //Fourth CS Block
		 CS_off=FindHexStr(CS_off+16, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , EcuBlockSig1, EcuHexCount1);
		 CS_off=FindHexStr(CS_off+16, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , EcuBlockSig1, EcuHexCount1);
		 CS_off=FindHexStr(CS_off+16, dllECUinfo->EcuFileSize, (unsigned char*)Buffer , EcuBlockSig1, EcuHexCount1);
		 if (CS_off!=0xFFFFFFFF)
			{
			 MultCSAdr=*(Buffer+CS_off-0x0D)+(*(Buffer+CS_off-0x0E)<<8)+(*(Buffer+CS_off-0x11)<<16)+(*(Buffer+CS_off-0x12)<<24);
			 if (*(Buffer+CS_off-0x0E)& 0x80) { MultCSAdr-=0x10000;}
			}
		 else
			{
				 return 0;
			}

			MultCSAdr+=0x90; //Correct Adress with CS number :)

		  for (CS_count = 0; CS_count < 55; CS_count++)
		  {
				TotalSumm++;

				StartAdr1=Read4ByteMot(Buffer+MultCSAdr);
				EndAdr1=Read4ByteMot(Buffer+MultCSAdr+4);
				ChkSumm1= Read4ByteMot(Buffer+MultCSAdr+8);
				CalcSumm1=SummInt16Mot(StartAdr1, EndAdr1, Buffer);
				 if (ChkSumm1==(Read4ByteMot(Buffer+MultCSAdr+12)^0xFFFFFFFF))
					{
					 if (ChkSumm1!=CalcSumm1)
						{
							if (FixCRC)
								{
									Write4ByteMot(CalcSumm1, Buffer+MultCSAdr+8);
									Write4ByteMot(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
								}
							BadSumm++;
						}
					 }
				 else
				 {
					if (FixCRC)
						{
							Write4ByteMot(CalcSumm1, Buffer+MultCSAdr+8);
							Write4ByteMot(CalcSumm1^0xFFFFFFFF, Buffer+MultCSAdr+12);
						}
					BadSumm++;
				 }
			MultCSAdr+=0x10;
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

unsigned int Read4ByteMot (unsigned char *ReadAddress)
{
	unsigned int  ReadedDword;
	ReadedDword= *(ReadAddress+3)+(*(ReadAddress+2)<<8)+(*(ReadAddress+1)<<16)+(*(ReadAddress)<<24);
	return ReadedDword;
}

void Write4ByteIntel (unsigned int WrDword, unsigned char *WriteAddress)
{
	*WriteAddress= WrDword;
	*(WriteAddress+1)= WrDword>>8;
	*(WriteAddress+2)= WrDword>>16;
	*(WriteAddress+3)= WrDword>>24;
}

void Write4ByteMot (unsigned int WrDword, unsigned char *WriteAddress)
{
	*(WriteAddress+3)= WrDword;
	*(WriteAddress+2)= WrDword>>8;
	*(WriteAddress+1)= WrDword>>16;
	*(WriteAddress)= WrDword>>24;
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

