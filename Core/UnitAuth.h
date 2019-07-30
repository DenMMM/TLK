//---------------------------------------------------------------------------
#ifndef UnitAuthH
#define UnitAuthH
//---------------------------------------------------------------------------
class MAuth;
//---------------------------------------------------------------------------
#define SHA_256                             // SHA_512
#include "sha1.h"
#include "sha2.h"
#include "hmac.h"
#include "UnitSLList.h"
//---------------------------------------------------------------------------
#define MAC_Alg         HMAC_SHA256         // HMAC_SHA512
#define MAC_Size        SHA256_DIGEST_SIZE  // SHA512_DIGEST_SIZE
#define MAC_KeySize     SHA256_BLOCK_SIZE   // SHA512_BLOCK_SIZE
//---------------------------------------------------------------------------
class MAuth:public MSLList
{
private:
    // Заглушки, т.к. списка на самом деле нет - только "заголовок"
    MListItem *item_new(unsigned char TypeID_) const { return NULL; }
    void item_del(MListItem *Item_) const {}

    // Функции механизма сохранения/загрузки данных
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);

    unsigned char Key[MAC_KeySize];

public:
    void Copy(MList *SrcList_) {}
    void SetKey(const char *Key_, int Size_);
    void GetKey(char *Buff_, int Size_) const;
    void Calc(const char *Data_, int DataSize_, char *MAC_, int MACSize_) const;
    bool Check(const char *Data_, int DataSize_, const char *MAC_, int MACSize_) const;

    MAuth();
    ~MAuth();
};
//---------------------------------------------------------------------------
#endif
