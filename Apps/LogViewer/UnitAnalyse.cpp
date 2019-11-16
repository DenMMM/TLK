//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "UnitAnalyse.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool ProcessComputersState(
	const MLogRecords &Log_,
	const MLogRecords::const_iterator iPoint_,
	MStates &States_, MTariffs &Tariffs_)
{
	States_.Clear();
	Tariffs_.Clear();
	// Ищем назад по логу ближайшие данные по состоянию компьютеров
	MLogRecords::const_iterator
		iPos=iPoint_,
		iBegin=Log_.begin();
	while(
		(iPos!=iBegin) &&    	/// Не совсем чисто, т.к. итератор "прямой"
		(iPos->gTypeID()!=MLogRecords::DataStates::TypeID)
		) --iPos;
	if ( iPos==iBegin ) goto error;

	// Заполняем таблицу состояний начальными данными
	{
		auto &rcdds=static_cast<const MLogRecords::DataStates&>(*iPos);
		for ( const auto &Ld: rcdds.Items )
		{
			States_.Add().sFromLog(Ld);
		}
	}
	// Начинаем сбор данных за прошедшее время
	while(iPos!=iPoint_)
	{
		++iPos;

		switch(iPos->gTypeID())
		{
			case MLogRecords::CompRun::TypeID:
			{
				auto &rcdr=static_cast<const MLogRecords::CompRun&>(*iPos);
				auto iState=States_.Search(rcdr.Number);
				if ( iState==States_.end() ) goto error;
				auto iTariff=Tariffs_.SrchUUID(rcdr.Tariff);
				if ( iTariff==Tariffs_.end() ) goto error;

				MTariffRunTimesItem runtime;
///				memset(&runtime,0,sizeof(runtime));
				runtime.TariffID=0;
				runtime.Number=0;
				runtime.StartTime=0;
				runtime.Type=0;
				runtime.BeginTime=0;
				runtime.EndTime=0;
				runtime.SizeTime=0;
				runtime.WorkTime=0;
				runtime.MaxTime=0;
				runtime.Cost=0.;

				runtime.WorkTime=rcdr.WorkTime;

				iState->Timer(rcdr.SystemTime);
				if ( !iState->CmdRun(&*iTariff,&runtime,false) ) goto error;
			}
				break;

			case MLogRecords::CompFine::TypeID:
			{
				auto &rcdf=static_cast<const MLogRecords::CompFine&>(*iPos);
				auto iState=States_.Search(rcdf.Number);
				if ( iState==States_.end() ) goto error;
//                if ( (fine=Fines->Search(rcdf.Fine))==nullptr ) goto error;
				iState->Timer(rcdf.SystemTime);
				if ( !iState->CmdFine(rcdf.Time,false) ) goto error;
			}
				break;

			case MLogRecords::CompExchange::TypeID:
			{
				auto &rcde=static_cast<const MLogRecords::CompExchange&>(*iPos);
				auto iState=States_.Search(rcde.From);
				if ( iState==States_.end() ) goto error;
				auto iState2=States_.Search(rcde.To);
				if ( iState2==States_.end() ) goto error;

				iState->Timer(rcde.SystemTime);
				iState2->Timer(rcde.SystemTime);
				if ( !iState->CmdExchange(&*iState2,false) ) goto error;
			}
				break;

			case MLogRecords::ModeLock::TypeID:
			{
				auto &rcdl=static_cast<const MLogRecords::ModeLock&>(*iPos);
				auto iState=States_.Search(rcdl.Number);
				if ( iState==States_.end() ) goto error;

				iState->Timer(rcdl.SystemTime);
				iState->CmdLock(rcdl.Apply,false);
			}
				break;

			case MLogRecords::ModePause::TypeID:
			{
				auto &rcdp=static_cast<const MLogRecords::ModePause&>(*iPos);
				auto iState=States_.Search(rcdp.Number);
				if ( iState==States_.end() ) goto error;

				iState->Timer(rcdp.SystemTime);
				iState->CmdPause(rcdp.Apply,false);
			}
				break;

			case MLogRecords::ModeOpen::TypeID:
			{
				auto &rcdo=static_cast<const MLogRecords::ModeOpen&>(*iPos);
				auto iState=States_.Search(rcdo.Number);
				if ( iState==States_.end() ) goto error;

				iState->Timer(rcdo.SystemTime);
				iState->CmdOpen(rcdo.Apply,false);
			}
				break;

			case MLogRecords::DataTariffs::TypeID:
			{
				auto &rcddtrf=static_cast<const MLogRecords::DataTariffs&>(*iPos);

				Tariffs_.Clear();
				for ( const auto &Ld: rcddtrf.Items )
				{
					Tariffs_.Add().sFromLog(Ld);
				}
			}
				break;

			default: break;
		}
	}
	// Обновляем состояния
	States_.Timer(iPos->SystemTime);

	return true;
error:
	return false;
}
//---------------------------------------------------------------------------
bool ProcessUsersUpTime(
//	const MLogRecords &Log_,
	MLogRecords::const_iterator iPosBegin_,
	const MLogRecords::const_iterator iPosEnd_,
	MUsers &Users_, MUsersUpTime &UpTimes_)
{
	Users_.Clear();
	UpTimes_.Clear();

	for ( MUserUpTime *uptime=nullptr;
		iPosBegin_!=iPosEnd_; ++iPosBegin_ )
	{
		switch(iPosBegin_->gTypeID())
		{
			case MLogRecords::AppConfig::TypeID:
				break;

			case MLogRecords::AppLogIn::TypeID:
				if ( uptime!=nullptr ) goto error;      /// throw ?
				uptime=&UpTimes_.Add();
				uptime->User=static_cast<const MLogRecords::AppLogIn&>(*iPosBegin_).User;
				uptime->BeginTime=static_cast<const MLogRecords::AppLogIn&>(*iPosBegin_).SystemTime;
				break;

			case MLogRecords::AppLogOut::TypeID:
				if ( uptime==nullptr ) goto error;
				uptime->EndTime=static_cast<const MLogRecords::AppLogOut&>(*iPosBegin_).SystemTime;
				uptime=nullptr;
				break;

			case MLogRecords::CompRun::TypeID:
				if ( uptime==nullptr ) continue;
				uptime->Gains+=static_cast<const MLogRecords::CompRun&>(*iPosBegin_).Cost;
				break;

			case MLogRecords::CompFine::TypeID:
				break;

			case MLogRecords::CompExchange::TypeID:
				break;

			case MLogRecords::DataUsers::TypeID:
			{
				auto &rcddusr=static_cast<const MLogRecords::DataUsers&>(*iPosBegin_);

				// Добавляем новых пользователей в список
				for ( const auto &Ld: rcddusr.Items )
				{
					auto iUsr=Users_.SrchUUID(Ld.UUID);

					if ( iUsr!=Users_.end() )
						iUsr->Name=Ld.Name;
					else
						Users_.Add().sFromLog(Ld);
				}
				break;
			}

			default:
				break;
		}
	}

	return true;
error:
	return false;
}
//---------------------------------------------------------------------------

