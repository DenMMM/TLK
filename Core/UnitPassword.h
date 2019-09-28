//---------------------------------------------------------------------------
#ifndef UnitPasswordH
#define UnitPasswordH
//---------------------------------------------------------------------------
#include <string>

#define SHA_256                             // SHA_512
#include "sha1.h"
#include "sha2.h"
#include "hmac.h"

#include "UnitSLList.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
class MPassword;
//---------------------------------------------------------------------------
#define PASS_Char       '#'                 // Отображаемый при вооде пароля символ
#define PASS_Alg        HMAC_SHA256         // HMAC_SHA512
#define PASS_HashSize   SHA256_DIGEST_SIZE  // SHA512_DIGEST_SIZE
#define PASS_SaltSize   SHA256_DIGEST_SIZE  // SHA256_BLOCK_SIZE, SHA512_BLOCK_SIZE ?
//---------------------------------------------------------------------------
class MPassword: private MSLList		/// MSLListItem ?
{
private:
    unsigned char Salt[PASS_SaltSize];
    unsigned char Hash[PASS_HashSize];

public:
	// Функции механизма сохранения/загрузки данных
	virtual unsigned GetDataSize() const override;
	virtual void *SetData(void *Data_) const override;
	virtual const void *GetData(const void *Data_, const void *Limit_) override;
//	void Copy(const MList *SrcItem_) override {}

	void Copy(const MPassword *SrcPass_);

	void Set(const std::wstring &Pass_);
	bool Check(const std::wstring &Pass_) const;

	MPassword()
	{
		memset(Salt,0,sizeof(Salt));
		memset(Hash,0,sizeof(Hash));
	}

	~MPassword() = default;
};
//---------------------------------------------------------------------------
#endif
