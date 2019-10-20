//---------------------------------------------------------------------------
#ifndef UnitShellStateH
#define UnitShellStateH
//---------------------------------------------------------------------------
// Main Shell State
#define mssConfig       1       // Открыты настройки
#define mssErrorConfig  2       // Ошибка загрузки настроек
#define mssErrorState   4       // Ошибка работы с файлом состояний
#define mssErrorLog     8       // Ошибка работы с файлом лога
//---------------------------------------------------------------------------
class MShellState
{
public:
    unsigned State;
    unsigned User;

	MShellState():
		State(0),
		User(0)
	{
	}
};
//---------------------------------------------------------------------------
#endif

