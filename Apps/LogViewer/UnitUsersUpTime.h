//---------------------------------------------------------------------------
#ifndef UnitUsersUpTimeH
#define UnitUsersUpTimeH
//---------------------------------------------------------------------------
#include "UnitList.h"
//---------------------------------------------------------------------------
class MUserUpTime;
class MUsersUpTime;
//---------------------------------------------------------------------------
class MUserUpTime:
	public MListItemSimple <
		MListItem <MUsersUpTime, MUserUpTime>,
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
	public MListSimple <
		MList <MUsersUpTime, MUserUpTime>,
		MUserUpTime>
{
};
//---------------------------------------------------------------------------
#endif

