//---------------------------------------------------------------------------
#ifndef UnitWinAPIH
#define UnitWinAPIH
//---------------------------------------------------------------------------
#include <winsock2.h>
//#include <windows.h>
//---------------------------------------------------------------------------
namespace MWAPI
{
    class CRITICAL_SECTION
    {
    private:
        ::CRITICAL_SECTION Section;

    public:
        // Авто-блок одной критической секции
        class Lock
        {
        private:
            MWAPI::CRITICAL_SECTION &Obj;

        public:
            Lock(
                MWAPI::CRITICAL_SECTION &Obj_):
                Obj(Obj_)
            {
                Obj.Enter();
            }
            ~Lock()
            {
                Obj.Leafe();
            }
        };

        // Авто-блок двух критических секций
        // с защитой от взаимной блокировки потоков
        class DualLock
        {
        private:
            MWAPI::CRITICAL_SECTION &Obj1;
            MWAPI::CRITICAL_SECTION &Obj2;

        public:
            DualLock(
                MWAPI::CRITICAL_SECTION &Obj1_,
                MWAPI::CRITICAL_SECTION &Obj2_):
                Obj1(Obj1_), Obj2(Obj2_)
            {
                if ( &Obj1<&Obj2 )
                {
                    Obj1.Enter();
                    Obj2.Enter();
                } else
                {
                    Obj2.Enter();
                    Obj1.Enter();
                }
            }
            ~DualLock()
            {
                Obj1.Leafe();
                Obj2.Leafe();
            }
        };


        // Синхронизация доступа к одиночному объекту
        void Enter() { ::EnterCriticalSection(&Section); }
        void Leafe() { ::LeaveCriticalSection(&Section); }

        CRITICAL_SECTION() { ::InitializeCriticalSection(&Section); }
        ~CRITICAL_SECTION() { ::DeleteCriticalSection(&Section); }
    };

};
//---------------------------------------------------------------------------
#endif
