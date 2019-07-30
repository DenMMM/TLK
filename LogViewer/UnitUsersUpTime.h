//---------------------------------------------------------------------------
#ifndef UnitUsersUpTimeH
#define UnitUsersUpTimeH
//---------------------------------------------------------------------------
#include "UnitList.h"
//---------------------------------------------------------------------------
class MUserUpTime:public MListItem
{
public:
    bool Copy(MListItem *SrcItem_);
public:
    unsigned User;
    __int64 BeginTime;
    __int64 EndTime;
    double Gains;

    MUserUpTime();
    ~MUserUpTime();
};
//---------------------------------------------------------------------------
class MUsersUpTime:public MList
{
private:
    MListItem *operator_new(unsigned Type_) { return (MListItem*)new MUserUpTime; }
    void operator_delete(MListItem *DelItem_) { delete (MUserUpTime*)DelItem_; }
public:
    MUsersUpTime() { constructor(); }
    ~MUsersUpTime() { destructor(); }
};
//---------------------------------------------------------------------------
#endif

