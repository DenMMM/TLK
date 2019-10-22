//---------------------------------------------------------------------------
#ifndef UnitClOptionsH
#define UnitClOptionsH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
class MClOptions;
//---------------------------------------------------------------------------
#define mcoTransp       0x00000001      // Полупрозрачный shell
#define mcoAddRoute     0x00000002      // Управлять маршрутом по-умолчанию
#define mcoAutoRun      0x00000004      // Обрабатывать раздел автозапуска при входе пользователя
//---------------------------------------------------------------------------
#define MAX_ClUNameLen   104            // Длина логина пользователя, под которым будет запускаться TLK shell
//---------------------------------------------------------------------------
class MClOptionsStub:
	public MSLListItem <MClOptions, MClOptionsStub> {};

class MClOptions:
	public MSLList <MClOptions, MClOptionsStub> 	/// private ?
{
public:
    // Функции механизма сохранения/загрузки данных
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

public:
	std::wstring ShellUser;   			// Логин пользователя для блокировки TLK shell
	short ToEndTime;                    // За сколько минут предупреждать об окончании времени (0-no show)
	short MessageTime;                  // Сколько секунд показывать предупреждение
	short MsgEndTime;                   // Время показа сообщения об окончании времени, секунд (0-no show)
	short RebootWait;                   // Секунд до перезагрузки после окончании времени (0-no reboot)
	short AutoLockTime;                 // Минут без связи с сервером до блокировки (0-no lock)
	unsigned Flags;                     // Остальные настройки - флаги

	void SetShellUser(const std::wstring &Name_);

	MClOptions():
		ToEndTime(2),
		MessageTime(15),
		MsgEndTime(10),
		RebootWait(20),
		AutoLockTime(15),
		Flags(0)
	{
	}
};
//---------------------------------------------------------------------------
#endif
