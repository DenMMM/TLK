//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "UnitAnalyse.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool ProcessComputersState(MLogRecordsItem *Position_,
	MStates *States_, MTariffs *Tariffs_)
{
	MLogRecordsItem *Record;
	MLogRecords::DataStates *rcdds;
	MLogRecords::CompRun *rcdr;
	MLogRecords::CompFine *rcdf;
	MLogRecords::CompExchange *rcde;
	MLogRecords::ModeLock *rcdl;
	MLogRecords::ModePause *rcdp;
	MLogRecords::ModeOpen *rcdo;
	MLogRecords::DataTariffs *rcddtrf;
	MLogRecords::DataFines *rcddfn;
	MLogRecords::DataUsers *rcddusr;
	MStatesItem *state, *state2;
	MTariffsItem *tariff;
	MFinesItem *fine;
    MTariffRunTimesItem runtime;

    States_->Clear();
    Tariffs_->Clear();
    // Ищем назад по логу ближайшие данные по состоянию компьютеров
    Record=Position_;
	while(
		Record &&
		(Record->gTypeID()!=MLogRecords::DataStates::TypeID)
		) Record=Record->gPrev();
    if ( Record==nullptr ) goto error;
    // Заполняем таблицу состояний начальными данными
	rcdds=dynamic_cast<MLogRecords::DataStates*>(Record);
	for ( auto &Ld: rcdds->Items )
	{
		state=States_->Add();
		state->sFromLog(Ld);
	}
	// Начинаем сбор данных за прошедшее время
	while(Record!=Position_)
	{
		Record=Record->gNext();

		switch(Record->gTypeID())
		{
			case MLogRecords::CompRun::TypeID:
				rcdr=static_cast<MLogRecords::CompRun*>(Record);

				state=States_->Search(rcdr->Number); if ( state==nullptr ) goto error;
				tariff=Tariffs_->SrchUUID(rcdr->Tariff); if ( tariff==nullptr ) goto error;

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

				state->Timer(rcdr->SystemTime);
				if ( !state->CmdRun(tariff,&runtime,false) ) goto error;
				break;

			case MLogRecords::CompFine::TypeID:
				rcdf=static_cast<MLogRecords::CompFine*>(Record);

				state=States_->Search(rcdf->Number); if ( state==nullptr ) goto error;
//                if ( (fine=Fines->Search(rcdf->Fine))==nullptr ) goto error;
				state->Timer(rcdf->SystemTime);
				if ( !state->CmdFine(rcdf->Time,false) ) goto error;
				break;

			case MLogRecords::CompExchange::TypeID:
				rcde=static_cast<MLogRecords::CompExchange*>(Record);

				state=States_->Search(rcde->From); if ( state==nullptr ) goto error;
				state2=States_->Search(rcde->To); if ( state2==nullptr ) goto error;

				state->Timer(rcde->SystemTime);
				state2->Timer(rcde->SystemTime);
				if ( !state->CmdExchange(state2,false) ) goto error;
				break;

			case MLogRecords::ModeLock::TypeID:
				rcdl=static_cast<MLogRecords::ModeLock*>(Record);

				state=States_->Search(rcdl->Number); if ( state==nullptr ) goto error;

				state->Timer(rcdl->SystemTime);
				state->CmdLock(rcdl->Apply,false);
				break;

			case MLogRecords::ModePause::TypeID:
				rcdp=static_cast<MLogRecords::ModePause*>(Record);

				state=States_->Search(rcdp->Number); if ( state==nullptr ) goto error;

				state->Timer(rcdp->SystemTime);
				state->CmdPause(rcdp->Apply,false);
				break;

			case MLogRecords::ModeOpen::TypeID:
				rcdo=static_cast<MLogRecords::ModeOpen*>(Record);

				state=States_->Search(rcdo->Number); if ( state==nullptr ) goto error;

				state->Timer(rcdo->SystemTime);
				state->CmdOpen(rcdo->Apply,false);
				break;

			case MLogRecords::DataTariffs::TypeID:
				rcddtrf=static_cast<MLogRecords::DataTariffs*>(Record);

				Tariffs_->Clear();
				for ( auto &Ld: rcddtrf->Items )
				{
					tariff=Tariffs_->Add();
					tariff->sFromLog(Ld);
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
bool ProcessUsersUpTime(MLogRecordsItem *Begin_, MLogRecordsItem *End_,
    MUsers *Users_, MUsersUpTime *UpTimes_)
{
    MLogRecords::DataUsers *rcddusr;
    MUsersItem *usr;
    MUserUpTime *uptime;

    Users_->Clear();
    UpTimes_->Clear();
    if ( Begin_==nullptr ) goto exit;
    uptime=nullptr;
    for ( ; Begin_!=End_->gNext(); Begin_=Begin_->gNext() )
    {
        switch(Begin_->gTypeID())
        {
			case MLogRecords::AppConfig::TypeID:
				break;

			case MLogRecords::AppLogIn::TypeID:
				if ( uptime!=nullptr ) goto error;
				uptime=UpTimes_->Add();
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
				rcddusr=static_cast<MLogRecords::DataUsers*>(Begin_);

				// Добавляем новых пользователей в список
				for ( auto &Ld: rcddusr->Items )
				{
					usr=Users_->SrchUUID(Ld.UUID);
					if ( usr!=nullptr )
					{
						usr->Name=Ld.Name;
					} else
					{
						usr=Users_->Add();
						usr->sFromLog(Ld);
					}
				}
				break;

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

