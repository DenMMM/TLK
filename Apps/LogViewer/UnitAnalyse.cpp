//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "UnitAnalyse.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool ProcessComputersState(MLogRecord *Position_,
    MStates *States_, MTariffs *Tariffs_)
{
    MLogRecord *Record;
    MLogRecordDataStates *rcdds;
    MLogRecordRun *rcdr;
    MLogRecordFine *rcdf;
    MLogRecordExchange *rcde;
    MLogRecordLock *rcdl;
    MLogRecordPause *rcdp;
    MLogRecordOpen *rcdo;
    MLogRecordDataTariffs *rcddtrf;
    MLogRecordDataFines *rcddfn;
    MLogRecordDataUsers *rcddusr;
    MState *state, *state2;
    MTariff *tariff;
    MFine *fine;
    MRunTime runtime;

    States_->Clear();
    Tariffs_->Clear();
    // Ищем назад по логу ближайшие данные по состоянию компьютеров
    Record=Position_;
    while(Record&&(Record->gTypeID()!=mlrDataStates))
        Record=(MLogRecord*)Record->gPrev();
    if ( Record==NULL ) goto error;
    // Заполняем таблицу состояний начальными данными
    rcdds=(MLogRecordDataStates*)Record;
    for ( unsigned i=0; i<rcdds->States.Count(); i++ )
    {
        if ( (state=(MState*)States_->Add())==NULL ) goto error;
        state->SetStateData(&rcdds->States[i]);
    }
    // Начинаем сбор данных за прошедшее время
    while(Record!=Position_)
    {
        Record=(MLogRecord*)Record->gNext();

        switch(Record->gTypeID())
        {
            case mlrRun:
                rcdr=(MLogRecordRun*)Record;
                if ( (state=States_->Search(rcdr->Number))==NULL ) goto error;
                if ( (tariff=(MTariff*)Tariffs_->SrchID(rcdr->Tariff))==NULL ) goto error;
                memset(&runtime,0,sizeof(runtime));
                runtime.WorkTime=rcdr->WorkTime;
                state->Timer(rcdr->SystemTime);
                if ( !state->CmdRun(tariff,&runtime,false) ) goto error;
                break;
            case mlrFine:
                rcdf=(MLogRecordFine*)Record;
                if ( (state=States_->Search(rcdf->Number))==NULL ) goto error;
//                if ( (fine=Fines->Search(rcdf->Fine))==NULL ) goto error;
                state->Timer(rcdf->SystemTime);
                if ( !state->CmdFine(rcdf->Time,false) ) goto error;
                break;
            case mlrExchange:
                rcde=(MLogRecordExchange*)Record;
                if ( (state=States_->Search(rcde->From))==NULL ) goto error;
                if ( (state2=States_->Search(rcde->To))==NULL ) goto error;
                state->Timer(rcde->SystemTime);
                state2->Timer(rcde->SystemTime);
                if ( !state->CmdExchange(state2,false) ) goto error;
                break;
            case mlrLock:
                rcdl=(MLogRecordLock*)Record;
                if ( (state=States_->Search(rcdl->Number))==NULL ) goto error;
                state->Timer(rcdl->SystemTime);
                state->CmdLock(rcdl->Apply,false);
                break;
            case mlrPause:
                rcdp=(MLogRecordPause*)Record;
                if ( (state=States_->Search(rcdp->Number))==NULL ) goto error;
                state->Timer(rcdp->SystemTime);
                state->CmdPause(rcdp->Apply,false);
                break;
            case mlrOpen:
                rcdo=(MLogRecordOpen*)Record;
                if ( (state=States_->Search(rcdo->Number))==NULL ) goto error;
                state->Timer(rcdo->SystemTime);
                state->CmdOpen(rcdo->Apply,false);
                break;
            case mlrDataTariffs:
                rcddtrf=(MLogRecordDataTariffs*)Record;
                Tariffs_->Clear();
                for ( unsigned i=0; i<rcddtrf->Tariffs.Count(); i++ )
                {
                    tariff=(MTariff*)Tariffs_->Add();
                    tariff->SetTariffData(&rcddtrf->Tariffs[i]);
                }
                break;
            default: break;
        }
    }
    // Обновляем состояния
    States_->Timer(((MLogRecordEvent*)Record)->SystemTime);

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------
bool ProcessUsersUpTime(MLogRecord *Begin_, MLogRecord *End_,
    MUsers *Users_, MUsersUpTime *UpTimes_)
{
    MLogRecordDataUsers *rcddusr;
    MUser *usr;
    MUserUpTime *uptime;

    Users_->Clear();
    UpTimes_->Clear();
    if ( Begin_==NULL ) goto exit;
    uptime=NULL;
    for ( ; Begin_!=End_->gNext(); Begin_=(MLogRecord*)Begin_->gNext() )
    {
        switch(Begin_->gTypeID())
        {
            case mlrConfig: break;
            case mlrLogIn:
                if ( uptime!=NULL ) goto error;
                uptime=(MUserUpTime*)UpTimes_->Add();
                uptime->User=((MLogRecordLogIn*)Begin_)->User;
                uptime->BeginTime=((MLogRecordLogIn*)Begin_)->SystemTime;
                break;
            case mlrLogOut:
                if ( uptime==NULL ) goto error;
                uptime->EndTime=((MLogRecordLogIn*)Begin_)->SystemTime;
                uptime=NULL;
                break;
            case mlrRun:
                if ( uptime==NULL ) continue;
                uptime->Gains+=((MLogRecordRun*)Begin_)->Cost;
                break;
            case mlrFine: break;
            case mlrExchange: break;
            case mlrDataUsers:
                // Добавляем новых пользователей в список
                rcddusr=(MLogRecordDataUsers*)Begin_;
                for ( unsigned i=0; i<rcddusr->Users.Count(); i++ )
                {
                    if ( (usr=(MUser*)Users_->SrchID(rcddusr->Users[i].ID))!=NULL )
                        strcpy(usr->Name,rcddusr->Users[i].Name);
                    else
                    {
                        usr=(MUser*)Users_->Add();
                        usr->SetUserData(&rcddusr->Users[i]);
                    }
                }
                break;
            default: break;
        }
    }

exit:
    return true;
error:
    return false;
}
//---------------------------------------------------------------------------

