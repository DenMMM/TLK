//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "UnitAnalyse.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool ProcessComputersState(
	MLogRecords::const_iterator Position_,
	MStates *States_, MTariffs *Tariffs_)
{
	States_->Clear();
	Tariffs_->Clear();
	// Ищем назад по логу ближайшие данные по состоянию компьютеров
	MLogRecords::const_iterator Record=Position_;
	while(
		Record &&
		(Record->gTypeID()!=MLogRecords::DataStates::TypeID)
		) Record=Record->gPrev();
	if ( Record==nullptr ) goto error;

	// Заполняем таблицу состояний начальными данными
	{
		auto *rcdds=dynamic_cast<MLogRecords::DataStates*>(Record);
		for ( auto &Ld: rcdds->Items )
		{
			States_->Add().sFromLog(Ld);
		}
	}
	// Начинаем сбор данных за прошедшее время
	while(Record!=Position_)
	{
		Record=Record->gNext();

		switch(Record->gTypeID())
		{
			case MLogRecords::CompRun::TypeID:
			{
				auto *rcdr=static_cast<MLogRecords::CompRun*>(Record);
				auto iState=States_->Search(rcdr->Number);
				if ( iState==States_->end() ) goto error;
				auto iTariff=Tariffs_->SrchUUID(rcdr->Tariff);
				if ( iTariff==Tariffs_->end() ) goto error;

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

				runtime.WorkTime=rcdr->WorkTime;

				iState->Timer(rcdr->SystemTime);
				if ( !iState->CmdRun(&*iTariff,&runtime,false) ) goto error;
			}
				break;

			case MLogRecords::CompFine::TypeID:
			{
				auto *rcdf=static_cast<MLogRecords::CompFine*>(Record);
				auto iState=States_->Search(rcdf->Number);
				if ( iState==States_->end() ) goto error;
//                if ( (fine=Fines->Search(rcdf->Fine))==nullptr ) goto error;
				iState->Timer(rcdf->SystemTime);
				if ( !iState->CmdFine(rcdf->Time,false) ) goto error;
			}
				break;

			case MLogRecords::CompExchange::TypeID:
			{
				auto *rcde=static_cast<MLogRecords::CompExchange*>(Record);
				auto iState=States_->Search(rcde->From);
				if ( iState==States_->end() ) goto error;
				auto iState2=States_->Search(rcde->To);
				if ( iState2==States_->end() ) goto error;

				iState->Timer(rcde->SystemTime);
				iState2->Timer(rcde->SystemTime);
				if ( !iState->CmdExchange(&*iState2,false) ) goto error;
			}
				break;

			case MLogRecords::ModeLock::TypeID:
			{
				auto *rcdl=static_cast<MLogRecords::ModeLock*>(Record);
				auto iState=States_->Search(rcdl->Number);
				if ( iState==States_->end() ) goto error;

				iState->Timer(rcdl->SystemTime);
				iState->CmdLock(rcdl->Apply,false);
			}
				break;

			case MLogRecords::ModePause::TypeID:
			{
				auto *rcdp=static_cast<MLogRecords::ModePause*>(Record);
				auto iState=States_->Search(rcdp->Number);
				if ( iState==States_->end() ) goto error;

				iState->Timer(rcdp->SystemTime);
				iState->CmdPause(rcdp->Apply,false);
			}
				break;

			case MLogRecords::ModeOpen::TypeID:
			{
				auto *rcdo=static_cast<MLogRecords::ModeOpen*>(Record);
				auto iState=States_->Search(rcdo->Number);
				if ( iState==States_->end() ) goto error;

				iState->Timer(rcdo->SystemTime);
				iState->CmdOpen(rcdo->Apply,false);
			}
				break;

			case MLogRecords::DataTariffs::TypeID:
			{
				auto *rcddtrf=static_cast<MLogRecords::DataTariffs*>(Record);

				Tariffs_->Clear();
				for ( auto &Ld: rcddtrf->Items )
				{
					Tariffs_->Add().sFromLog(Ld);
				}
			}
				break;

			default: break;
		}
	}
	// Обновляем состояния
	States_->Timer(Record->SystemTime);

	return true;
error:
	return false;
}
//---------------------------------------------------------------------------
bool ProcessUsersUpTime(
	MLogRecords::const_iterator Begin_,
	MLogRecords::const_iterator End_,
	MUsers *Users_, MUsersUpTime *UpTimes_)
{
	Users_->Clear();
	UpTimes_->Clear();
	MUserUpTime *uptime=nullptr;

	if ( Begin_==nullptr ) goto exit;
	for ( ; Begin_!=End_->gNext(); Begin_=Begin_->gNext() )
    {
        switch(Begin_->gTypeID())
        {
			case MLogRecords::AppConfig::TypeID:
				break;

			case MLogRecords::AppLogIn::TypeID:
				if ( uptime!=nullptr ) goto error;
				uptime=&UpTimes_->Add();
				uptime->User=static_cast<MLogRecords::AppLogIn*>(Begin_)->User;
				uptime->BeginTime=static_cast<MLogRecords::AppLogIn*>(Begin_)->SystemTime;
				break;

			case MLogRecords::AppLogOut::TypeID:
				if ( uptime==nullptr ) goto error;
				uptime->EndTime=static_cast<MLogRecords::AppLogOut*>(Begin_)->SystemTime;
				uptime=nullptr;
				break;

			case MLogRecords::CompRun::TypeID:
				if ( uptime==nullptr ) continue;
				uptime->Gains+=static_cast<MLogRecords::CompRun*>(Begin_)->Cost;
				break;

			case MLogRecords::CompFine::TypeID:
				break;

			case MLogRecords::CompExchange::TypeID:
				break;

			case MLogRecords::DataUsers::TypeID:
			{
				auto *rcddusr=static_cast<MLogRecords::DataUsers*>(Begin_);

				// Добавляем новых пользователей в список
				for ( auto &Ld: rcddusr->Items )
				{
					auto iUsr=Users_->SrchUUID(Ld.UUID);

					if ( iUsr!=Users_->end() )
						iUsr->Name=Ld.Name;
					else
						Users_->Add().sFromLog(Ld);
				}
				break;
			}

			default:
				break;
		}
	}

exit:
	return true;
error:
	return false;
}
//---------------------------------------------------------------------------

