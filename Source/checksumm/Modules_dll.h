#define GetLibVer "Lunteg"
#define GetKnownECU "Vupsen"
#define RecogniseECU "Pupsen"
#define GetChksNum "Kuzya"
#define FixChkSum "Pchelenok"
#define GetChkSum "Shnuk"
	struct ECU_info
		{
			LPVOID EcuBuffer;   // Указатель на буфер с загруженным файлом
			unsigned int EcuFileSize;	// Размер файла
			char ECU_Descr[100]; // Описание текущей прошивки
			int ChkSumNumber;	// Количество найденых КС в прошивке
			int BadChkSumNumber; //Количество неправильных КС в прошивке
		} ;


typedef char* (*GetLibVer_Imp)();
GetLibVer_Imp GetLibVer_proc; // Информация об библиотеке. Возвращаем строку с описанием модуля
typedef char* (*GetKnownECU_Imp)();
GetKnownECU_Imp GetKnownECU_proc; // Список извесных ECU в библиотеке. Возвращаем строку со списком ECU
typedef int (*RecogniseECU_Imp)(ECU_info *);
RecogniseECU_Imp RecogniseECU_proc; // Опознаем ECU. Если опознали, возвращаем 1
typedef int (*GetChksNum_Imp)();
GetChksNum_Imp GetChksNum_proc; // Определяем количество КС в прошивке. Возвращает кол-во найденных КС
typedef int (*FixChkSum_Imp)(ECU_info *);
FixChkSum_Imp FixChkSum_proc; // Подсчитывает контрольные суммы. Возвращаем кол-во подсчитанных КС
typedef int (*GetChkSum_Imp)();
GetChkSum_Imp GetChkSum_proc; // Проверка КС в прошивке. Возвращаем 1 если все КС верны