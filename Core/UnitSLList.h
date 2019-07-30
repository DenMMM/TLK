//---------------------------------------------------------------------------
#ifndef UnitSLListH
#define UnitSLListH
//---------------------------------------------------------------------------
class MSLListItem;
class MSLList;
//---------------------------------------------------------------------------
#include <string>
#include <winsock2.h>
//#include <windows.h>
#include "UnitList.h"
//---------------------------------------------------------------------------
#define MAX_SLFileSize  0x02000000  // ����������� �� ������ ������ - 32�����
#define MAX_SLRegSize   0x00000800  // ����������� �� ������ ������ ������� - 2�����
//---------------------------------------------------------------------------
class MSLListItem:public MListItem
{
private:
    // ������� ����������� ������� ������ ������������ �� MSLListItem ������,
    // ���������� � �������������� �� �/�� ������.
    virtual unsigned GetDataSize() const=0;
    virtual char *SetData(char *Data_) const=0;
    virtual const char *GetData(const char *Data_, const char *Limit_)=0;
    // ������ � ��� ����� � MSLList
    friend class MSLList;
};
//---------------------------------------------------------------------------
class MSLList:public MList
{
public:
    HKEY DefaultKey;            // HKCR/HKCU/HKLM/HKU
    std::string DefaultFile;    // ���� � ����� �� ����� / ������ �������
    std::string DefaultValue;   // ��� ��������� � �������
    unsigned DefaultCode;       // �������� ���� ���������� ������
    mutable DWORD LastError;    // ��� ������ ��������� �������� � ������/��������

    // ������� ����������� ������� ������ ������������ �� MSLList ������,
    // ���������� � �������������� �� �/�� ������.
    // ��������. MList �� ������������ ���������� ����������, � MSLList-��.
    virtual unsigned GetDataSize() const { return 0; }
    virtual char *SetData(char *Data_) const { return Data_; }
    virtual const char *GetData(const char *Data_, const char *Limit_) { return Data_; }

public:
    // ������� ����������� ������� ������ ���� ��������� ������,
    // ���������� � �������������� �� �/�� ������.
    unsigned GetAllDataSize(bool Header_=true) const;
    char *SetAllData(char *Data_, bool Header_=true) const;
    const char *GetAllData(const char *Data_, const char *Limit_);

    bool SaveTo(const std::string &File_, unsigned Code_, bool Always_=true, bool Safe_=false) const;
    bool AttachTo(const std::string &File_, unsigned Code_, bool Safe_=false) const;
    bool LoadFrom(const std::string &File_, unsigned Code_);
    bool StoreTo(HKEY Key_, const std::string &SubKey_, const std::string &Value_, unsigned Code_) const;
    bool QueryFrom(HKEY Key_, const std::string &SubKey_, const std::string &Value_, unsigned Code_);
    bool SaveAsReg(const char *File_, HKEY Key_, const char *SubKey_, const char *Value_, unsigned Code_) const;
    void SetDefaultFile(const std::string &File_, unsigned Code_);
    void SetDefaultKey(HKEY Key_, const std::string &SubKey_, const std::string &Value_, unsigned Code_);
    bool Save(bool Always_=true, bool Safe_=false) const;
    bool Attach(bool Safe_=false) const;
    bool Load();
    DWORD gLastErr() const { return LastError; }

    MSLList()
    {
        DefaultCode=0;
        LastError=0;
    }
};
//---------------------------------------------------------------------------
#endif

