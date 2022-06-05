//---------------------------------------------------------------------------
#ifndef UnitAuthH
#define UnitAuthH
//---------------------------------------------------------------------------
#define SHA_256                             // SHA_512
#include "..\Ext\BrianGladman\SHA\sha1.h"
#include "..\Ext\BrianGladman\SHA\sha2.h"
#include "..\Ext\BrianGladman\SHA\hmac.h"

#include "UnitSLList.h"
//---------------------------------------------------------------------------
class MAuth;
//---------------------------------------------------------------------------
#define MAC_Alg         HMAC_SHA256         // HMAC_SHA512
#define MAC_Size        SHA256_DIGEST_SIZE  // SHA512_DIGEST_SIZE
#define MAC_KeySize     SHA256_BLOCK_SIZE   // SHA512_BLOCK_SIZE
//---------------------------------------------------------------------------
class MAuthStub:
	public MSLListItem <MAuth, MAuthStub> {};

class MAuth:
	public MSLList <MAuth, MAuthStub>       /// private ?
{
public:
    // Функции механизма сохранения/загрузки данных
	virtual std::size_t GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;

private:
	unsigned char Key[MAC_KeySize];

public:
	void SetKey(const void *Key_, std::size_t Size_);
    void GetKey(void *Buff_, std::size_t Size_) const;
	void Calc(const void *Data_, std::size_t DataSize_, void *MAC_, std::size_t MACSize_) const;
    bool Check(const void *Data_, std::size_t DataSize_, const void *MAC_, std::size_t MACSize_) const;

	MAuth()
	{
		memset(Key,0x00,sizeof(Key));
	}
};
//---------------------------------------------------------------------------
#endif
