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

	void Copy(const MListItem *SrcItem_) {};

	MUserUpTime();
	virtual ~MUserUpTime();
};
//---------------------------------------------------------------------------
class MUsersUpTime:
	public MList::Simple <MList, MUsersUpTime, MUserUpTime>
{
public:
    MUsersUpTime() {}
	~MUsersUpTime() {}
};
//---------------------------------------------------------------------------
#endif

