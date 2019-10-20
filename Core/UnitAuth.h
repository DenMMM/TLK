//---------------------------------------------------------------------------
#ifndef UnitAuthH
#define UnitAuthH
//---------------------------------------------------------------------------
#define SHA_256                             // SHA_512
#include "sha1.h"
#include "sha2.h"
#include "hmac.h"

#include "UnitSLList.h"
//---------------------------------------------------------------------------
class MAuth;
//---------------------------------------------------------------------------
#define MAC_Alg         HMAC_SHA256         // HMAC_SHA512
#define MAC_Size        SHA256_DIGEST_SIZE  // SHA512_DIGEST_SIZE
#define MAC_KeySize     SHA256_BLOCK_SIZE   // SHA512_BLOCK_SIZE
//---------------------------------------------------------------------------
class MAuth: public MSLList
{
private:
    // Функции механизма сохранения/загрузки данных
	unsigned GetDataSize() const override ;
	void *SetData(void *Data_) const override ;
	const void *GetData(const void *Data_, const void *Limit_) override ;

    unsigned char Key[MAC_KeySize];

public:
    void Copy(MList *SrcList_) {}
    void SetKey(const void *Key_, size_t Size_);
    void GetKey(void *Buff_, size_t Size_) const;
    void Calc(const void *Data_, size_t DataSize_, void *MAC_, size_t MACSize_) const;
    bool Check(const void *Data_, size_t DataSize_, const void *MAC_, size_t MACSize_) const;

    MAuth();
    ~MAuth();
};
//---------------------------------------------------------------------------
#endif
