//---------------------------------------------------------------------------
#include <string.h>
//#include <mem.h>
#include <stdlib.h>
#include <math.h>
#pragma hdrstop

#include "UnitTariffs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
std::size_t MTariffTimesItem::GetDataSize() const
{
	return
		sizeof(Type)+
		sizeof(BeginTime)+
		sizeof(EndTime)+
		sizeof(SizeTime)+
		sizeof(Cost);
}

void *MTariffTimesItem::SetData(void *Data_) const
{
	Data_=MemSet(Data_,Type);
	Data_=MemSet(Data_,BeginTime);
	Data_=MemSet(Data_,EndTime);
	Data_=MemSet(Data_,SizeTime);
	Data_=MemSet(Data_,Cost);
	return Data_;
}

const void *MTariffTimesItem::GetData(const void *Data_, const void *Limit_)
{
	return
		(Data_=MemGet(Data_,&Type,Limit_)) &&
		(Data_=MemGet(Data_,&BeginTime,Limit_)) &&
		(Data_=MemGet(Data_,&EndTime,Limit_)) &&
		(Data_=MemGet(Data_,&SizeTime,Limit_)) &&
		(Data_=MemGet(Data_,&Cost,Limit_))
		? Data_: nullptr;
}

std::int16_t MTariffTimesItem::MaxWorkTime(std::int16_t Time_) const
{
	std::int16_t
		WorkTime=EndTime - (Time_<BeginTime? Time_+24*60: Time_);
	if ( WorkTime<0 ) WorkTime=0;
	else if ( (Type==mttFlyPacket)&&(WorkTime>SizeTime) ) WorkTime=SizeTime;

	return WorkTime;
}
//---------------------------------------------------------------------------
MTariffsInfo::const_iterator
	MTariffsInfo::Search(std::uint32_t ID_) const
{
	auto iTi=begin();
	auto iEnd=end();

	while ( iTi!=iEnd )
	{
		if ( iTi->ID==ID_ ) break;
		++iTi;
	}

	return iTi;
}
//---------------------------------------------------------------------------
std::size_t MTariffsItem::GetDataSize() const
{
    return
        MIDListItem::GetDataSize()+
        sizeofLine(Name)+
        sizeof(Programs)+
        sizeof(Reboot)+
		sizeof(std::uint8_t)+sizeof(std::int8_t)*Comps.size()+  /// проверить size()
		Times.GetAllDataSize();
}

void *MTariffsItem::SetData(void *Data_) const
{
	Data_=MIDListItem::SetData(Data_);
	Data_=MemSetLine(Data_,Name);
	Data_=MemSet(Data_,Programs);
	Data_=MemSet(Data_,Reboot);
	Data_=MemSet(Data_,static_cast<std::uint8_t>(Comps.size()));
	for ( auto Comp: Comps ) Data_=MemSet(Data_,Comp);
	Data_=Times.SetAllData(Data_);
	return Data_;
}

const void *MTariffsItem::GetData(const void *Data_, const void *Limit_)
{
	std::uint8_t CompsCnt=0;

	if ( !(
		(Data_=MIDListItem::GetData(Data_,Limit_)) &&
		(Data_=MemGetLine(Data_,Name,MAX_TariffNameLen,Limit_)) &&
		(Data_=MemGet(Data_,&Programs,Limit_)) &&
		(Data_=MemGet(Data_,&Reboot,Limit_)) &&
		(Data_=MemGet(Data_,&CompsCnt,Limit_))
		) ) return nullptr;

	if ( (CompsCnt<0) || (CompsCnt>MAX_Comps) ) return nullptr;

	Comps.clear();
	for ( ; CompsCnt>0; --CompsCnt )
	{
		std::int8_t Comp;
		if ( !(Data_=MemGet(Data_,&Comp,Limit_)) ) return nullptr;
		Comps.push_back(Comp);
	}

	if ( !(Data_=Times.GetAllData(Data_,Limit_)) ) return nullptr;

	return Data_;
}

void MTariffsItem::CostPacket(MTariffRunTimesItem &RunTime_) const
{
	int StartTime, WorkTime;
	double Cost;

	// Выделяем количество минут с начала суток
	StartTime=ExtractHoursMin(RunTime_.StartTime);
	//
	WorkTime=0; Cost=0.;

	for ( const auto &tt: Times)
	{
		if ( (tt.Type!=mttPacket)||
			(tt.BeginTime!=RunTime_.BeginTime)||
			(tt.EndTime!=RunTime_.EndTime) ) continue;
		WorkTime=tt.MaxWorkTime(StartTime);
		// Корректируем время работы в соответствии с заданным ограничением
		if ( WorkTime>RunTime_.MaxTime ) WorkTime=RunTime_.MaxTime;
		if ( WorkTime!=0 ) Cost=tt.Cost;
		break;
	}

	RunTime_.WorkTime=WorkTime;
	RunTime_.Cost=Cost;
}

void MTariffsItem::CostFlyPacket(MTariffRunTimesItem &RunTime_) const
{
	int StartTime, SizeTime, WorkTime, MaxTime, NeedTime, RealTime;
	double Cost, LastCost;

	// Выделяем количество минут с начала суток
	StartTime=ExtractHoursMin(RunTime_.StartTime);
	//
	SizeTime=RunTime_.SizeTime;
	WorkTime=0; Cost=LastCost=0.;

	while(true)
	{
		// Ищем подходящую запись для пакета с ненулевым временем работы
		auto iTt=Times.cbegin();
		auto iEnd=Times.end();

		for ( ; iTt!=iEnd; ++iTt )
		{
			if ( (iTt->Type!=mttFlyPacket)||
				(iTt->SizeTime!=SizeTime) ) continue;
			if ( (MaxTime=iTt->MaxWorkTime(StartTime))!=0 ) break;
		}
		// Если подходящих записей больше нету, то заканчиваем накопление времени и стоимости
		if ( iTt==iEnd )
		{
			// Досчитываем стоимость "невлезшего" в тарифную сетку времени
			// по последней встреченной стоимости пакета
			Cost+=(SizeTime-WorkTime)*(LastCost/SizeTime);
			break;
		}
		LastCost=iTt->Cost;
		// Вычисляем сколько еще времени нужно для формирования полного пакета
		NeedTime=SizeTime-WorkTime;
		// Проверяем сколько реально времени можно добавить к пакету
		RealTime=MaxTime<NeedTime? MaxTime: NeedTime;
		// Добавляем это время к пакету и увеличиваем стоимость пакета
		WorkTime+=RealTime; Cost+=RealTime*(LastCost/SizeTime);
		// Проверяем не полное ли время пакета уже
		if ( WorkTime==SizeTime ) break;
		// Увеличиваем время начала отсчета и следим, чтобы оно не превысило суточное время
		StartTime+=RealTime; if ( StartTime>=(24*60) ) StartTime-=24*60;
	}
	// Корректируем время работы в соответствии с заданным ограничением
	if ( WorkTime>RunTime_.MaxTime ) WorkTime=RunTime_.MaxTime;

	RunTime_.WorkTime=WorkTime;
	RunTime_.Cost=Cost;
}

void MTariffsItem::CostHours(MTariffRunTimesItem &RunTime_) const
{
	int StartTime, SizeTime, WorkTime, MaxTime, NeedTime, RealTime;
	double Cost;

	// Выделяем количество минут с начала суток
	StartTime=ExtractHoursMin(RunTime_.StartTime);
	// Корректируем желаемое время работы в соответствии с заданным ограничением
	SizeTime=RunTime_.SizeTime;
	if ( SizeTime>RunTime_.MaxTime ) SizeTime=RunTime_.MaxTime;
	//
	WorkTime=0; Cost=0.;

	while(true)
	{
		// Ищем подходящую запись с ненулевым временем работы
		auto iTt=Times.cbegin();
		auto iEnd=Times.cend();

		for ( ; iTt!=iEnd; ++iTt )
		{
			if ( (iTt->Type==mttHours)&&
				((MaxTime=iTt->MaxWorkTime(StartTime))!=0) ) break;
		}
		// Если подходящих записей больше нету, то заканчиваем накопление времени и стоимости
		if ( iTt==iEnd ) break;
		// Вычисляем сколько еще времени нужно добавить
		NeedTime=SizeTime-WorkTime;
		// Проверяем сколько реально времени можно добавить
		RealTime=MaxTime<NeedTime? MaxTime: NeedTime;
		// Добавляем это время и увеличиваем стоимость
		WorkTime+=RealTime;
		Cost+=RealTime*iTt->Cost/60.;
		// Проверяем не набрано ли уже все время
		if ( WorkTime==SizeTime ) break;
		// Увеличиваем время начала отсчета и следим, чтобы оно не превысило суточное время
		StartTime+=RealTime; if ( StartTime>=(24*60) ) StartTime-=24*60;
	}

	RunTime_.WorkTime=WorkTime;
	RunTime_.Cost=Cost;
}

void MTariffsItem::Cost(MTariffRunTimesItem &RunTime_, double Prec_) const
{
	if ( !CheckForComp(RunTime_.Number) )
	{
        RunTime_.WorkTime=0; RunTime_.Cost=0.;
        return;
    }

    switch(RunTime_.Type)
    {
        case mttHours: CostHours(RunTime_); break;
        case mttFlyPacket: CostFlyPacket(RunTime_); break;
        case mttPacket: CostPacket(RunTime_); break;
        default: RunTime_.WorkTime=0; RunTime_.Cost=0.; break;
    }
    // Округляем стоимость до желаемой точности
	RunTime_.Cost=ceil(RunTime_.Cost/Prec_)*Prec_;
}

bool MTariffsItem::CheckForTime(std::int64_t Time_) const
{
    int Time;

    // Выделяем количество минут с начала суток
    Time=ExtractHoursMin(Time_);

	for ( const auto &tt: Times )
	{
		if ( tt.MaxWorkTime(Time)>0 ) return true;
	}

	return false;
}

bool MTariffsItem::CheckForComp(std::int8_t Num_) const
{
	for ( auto Comp: Comps )
	{
		if ( Comp==Num_ ) return true;
	}

	return false;
}

MTariffsInfoItem MTariffsItem::GetInfo() const
{
	MTariffsInfoItem Info;

	Info.ID=gUUID();
	Info.Name=Name;

	return Info;
}

MTariffRunTimes MTariffsItem::GetRunTimes(std::int64_t Time_) const
{
	// Выделяем количество минут с начала суток
	int SysTime=ExtractHoursMin(Time_);

	MTariffRunTimes ResTimes;
	// Проверяем возможность запуска на почасовой основе
	for ( const auto &tt: Times )
	{
		if ( (tt.Type!=mttHours)||
			(tt.MaxWorkTime(SysTime)==0) ) continue;

		MTariffRunTimesItem& rt=ResTimes.Add();
		rt.Type=mttHours;
		break;
	}
	// Ищем подходящие "плавающие" пакеты
	for ( const auto &tt: Times )
	{
		if ( (tt.Type!=mttFlyPacket)||
			(tt.MaxWorkTime(SysTime)==0) ) continue;

		MTariffRunTimesItem& rt=ResTimes.Add();
		rt.Type=mttFlyPacket;
		rt.SizeTime=tt.SizeTime;
	}
	// Ищем подходящие пакеты
	for ( const auto &tt: Times )
	{
		if ( (tt.Type!=mttPacket)||
			(tt.MaxWorkTime(SysTime)==0) ) continue;

		MTariffRunTimesItem& rt=ResTimes.Add();
		rt.Type=mttPacket;
		rt.BeginTime=tt.BeginTime;
		rt.EndTime=tt.EndTime;
	}

	return ResTimes;
}
//---------------------------------------------------------------------------
MTariffsInfo MTariffs::GetForTime(std::int64_t Time_) const
{
	MTariffsInfo ResInfo;

	for ( const auto &trf: *this )
	{
		if ( !trf.CheckForTime(Time_) ) continue;
		ResInfo.Add()=trf.GetInfo();
	}

	return ResInfo;
}
//---------------------------------------------------------------------------

