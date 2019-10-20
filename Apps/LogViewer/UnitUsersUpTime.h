//---------------------------------------------------------------------------
#ifndef UnitUsersUpTimeH
#define UnitUsersUpTimeH
//---------------------------------------------------------------------------
#include "UnitList.h"
//---------------------------------------------------------------------------
class MUserUpTime:
	public MListItem::Simple <
		MListItem::Proxy <MListItem, MUserUpTime>,
		MUserUpTime>
{
public:
	unsigned User;
	__int64 BeginTime;
	__int64 EndTime;
	double Gains;

	MUserUpTime():
		User(0),
		BeginTime(0),
		EndTime(0),
		Gains(0.)
	{
	}
};
//---------------------------------------------------------------------------
class MUsersUpTime:
	public MList::Simple <MList, MUsersUpTime, MUserUpTime>
{
};
//---------------------------------------------------------------------------
#endif

