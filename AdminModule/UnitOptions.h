//---------------------------------------------------------------------------
#ifndef UnitOptionsH
#define UnitOptionsH
//---------------------------------------------------------------------------
#include "UnitSLList.h"
#include "UnitLog.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define MAX_OptionsPasswordLength   16
#define MAX_DialogTime              30
#define MAX_FilterFreeTime          (24*60)
//---------------------------------------------------------------------------
// Main Users Rights
#define murPause        1   // ��������� ���������������� ����������
//---------------------------------------------------------------------------
class MOptions:private MSLList
{
private:
    MListItem *operator_new(unsigned Type_) { return NULL; }
    void operator_delete(MListItem *DelItem_) { }
public:
    bool Copy(MOptions *Options_);
private:
    char Password[MAX_OptionsPasswordLength+1]; // ������ �� ��������� ��������
public:
    int LogPeriod;              // ������ ������� ���� � ����� �����
    int CostDialogTime;         // ����� ����������� ���������, ��� ������������ ������ ��� �� ������� (� �������)
    double CostPrecision;       // �������� ������� ���
    int FilterFreeTime;         // ����� �� ��������� ������ ����������, ����� �� ��������� ��� ������ ��������� (� �������)
    unsigned UsersRights;       // ����� ������������� (���������������)

    bool SetPassword(char *Password_);
    bool CheckPassword(char *Password_);

    // ������� ��������� ����������/�������� ������
    unsigned GetDataSize();
    char *SetData(char *Data_);
    char *GetData(char *Data_, char *Limit_);

    // ������������ ��������� ��� ��������/���������� � ������� �������� ������ MSLList
    bool SaveTo(char *File_, unsigned Code_) { return MSLList::SaveTo(File_,Code_); }
    bool LoadFrom(char *File_, unsigned Code_) { return MSLList::LoadFrom(File_,Code_); }
    bool SetDefaultFile(char *File_, unsigned Code_) { return MSLList::SetDefaultFile(File_,Code_); }
    bool Save() { return MSLList::Save(); }
    bool Load() { return MSLList::Load(); }

    MOptions();
    ~MOptions();
};
//---------------------------------------------------------------------------
#endif

