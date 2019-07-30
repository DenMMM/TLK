//---------------------------------------------------------------------------
#ifndef UnitUsersUpTimeH
#define UnitUsersUpTimeH
//---------------------------------------------------------------------------
#include "UnitList.h"
//---------------------------------------------------------------------------
class MUserUpTime:public MListItem
{
private:
    unsigned char gTypeID() const { return 0; }

public:
    unsigned User;
    __int64 BeginTime;
    __int64 EndTime;
    double Gains;

    void Copy(const MListItem *SrcItem_) {};

    MUserUpTime();
    ~MUserUpTime();
};
//---------------------------------------------------------------------------
class MUsersUpTime:public MList
{
private:
    bool Typed() const { return false; }
    MListItem *item_new(unsigned char TypeID_) const { return (MListItem*)new MUserUpTime; }
    void item_del(MListItem *Item_) const { delete (MUserUpTime*)Item_; }
    
public:
    MUsersUpTime() {}
    ~MUsersUpTime() { Clear(); }
};
//---------------------------------------------------------------------------
#endif

