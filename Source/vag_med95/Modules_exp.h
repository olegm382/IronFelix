#define GetLibVer Lunteg
#define GetKnownECU Vupsen
#define RecogniseECU Pupsen
#define GetChksNum Kuzya
#define FixChkSum Pchelenok
#define GetChkSum Shnuk

	struct ECU_info
		{
			LPVOID EcuBuffer;   // Pointer to buffer with loaded file
			unsigned int EcuFileSize;	// File Size
			char ECU_Descr[100]; // ECU description
			int ChkSumNumber;	// Количество найденых КС в прошивке
			int BadChkSumNumber; // Количество неправильных КС в прошивке
		} ;


#define DLL_SPEC extern "C" __declspec(dllexport)

DLL_SPEC char* GetLibVer(); // Информация об библиотеке. Возвращаем строку с описанием модуля
DLL_SPEC char* GetKnownECU(); // Список извесных ECU в библиотеке. Возвращаем строку со списком ECU
DLL_SPEC int RecogniseECU(ECU_info *); // Опознаем ECU. Если опознали, возвращаем 1
DLL_SPEC int GetChksNum(); // Определяем количество КС в прошивке. Возвращает кол-во найденных КС
DLL_SPEC int FixChkSum(ECU_info *); // Подсчитывает контрольные суммы. Возвращаем кол-во подсчитанных КС
DLL_SPEC int GetChkSum(); // Проверка КС в прошивке. Возвращаем 1 если все КС верны

unsigned int FindHexStr(unsigned int SearchOffset, unsigned int BufferSize, unsigned char *SearchBuffer, unsigned char *HexBytes, unsigned int HexCount);
//Поиск последовательности байт в дампе. Возвращает смещение от начала буфера
//SearchOffset смещение от начала дампа для начала поиска
//BufferSize размер буфера
//SearchBuffer указатель на буфер с дампом
//HexBytes указатель на последовательность искомых байт
//HexCount количество байт для поиска

unsigned int SummInt8(unsigned int StartInt8, unsigned int EndInt8, unsigned char *SearchBuffer);
//Подсчет побайтовой суммы в диапазоне адресов от  StartInt8 до EndInt8
// Возвращает подсчитанную сумму

unsigned int SummInt16Intel(unsigned int StartInt16, unsigned int EndInt16, unsigned char *SearchBuffer);
//Подсчет суммы Word в диапазоне адресов от  StartInt16 до EndInt16 в формате Intel
// Возвращает подсчитанную сумму

unsigned int SummInt16Mot(unsigned int StartInt16, unsigned int EndInt16, unsigned char *SearchBuffer);
//Подсчет суммы Word в диапазоне адресов от  StartInt16 до EndInt16 в формате Motorola
// Возвращает подсчитанную сумму

unsigned int Read4ByteIntel (unsigned char *ReadAddress);
//считывает с адреса ReadAddress 4 байта в формате интел и возвращает их

void Write4ByteIntel (unsigned int WrDword, unsigned char *WriteAddress);
//Записывает по адресу WriteAddress 4 байта в формате интел

unsigned int Read4ByteMot (unsigned char *ReadAddress);
//считывает с адреса ReadAddress 4 байта в формате Motorola и возвращает их

void Write4ByteMot (unsigned int WrDword, unsigned char *WriteAddress);
//Записывает по адресу WriteAddress 4 байта в формате Motorola

unsigned int SummBlock(unsigned int StartBlock, unsigned int EndBlock, unsigned char *SearchBuffer);
//Подсчет поблочной суммы (803C) в диапазоне адресов от  StartBlock до EndBlock в формате Intel
// Возвращает подсчитанную сумму

unsigned int CalcCRC32(unsigned int StartBlock, unsigned int EndBlock, unsigned char *Buffer);
//Подсчет CRC32 начиная со смещения StartBlock от начала буфера до смещения EndBlock

unsigned int CalcCRC32Cont(unsigned int StartBlock, unsigned int EndBlock, unsigned char *Buffer, unsigned int crc_start);
//Подсчет продолжения CRC32 начиная со смещения StartBlock от начала буфера до смещения EndBlock
//С учетом предыдущего значения CRC32 crc_start
