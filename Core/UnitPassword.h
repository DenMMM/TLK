//---------------------------------------------------------------------------
#ifndef UnitPasswordH
#define UnitPasswordH
//---------------------------------------------------------------------------
class MPassword;
//---------------------------------------------------------------------------
#define SHA_256                             // SHA_512
#include "sha1.h"
#include "sha2.h"
#include "hmac.h"
#include "UnitCommon.h"
//---------------------------------------------------------------------------
#define PASS_Char       '#'                 // ������������ ��� ����� ������ ������ 
#define PASS_Alg        HMAC_SHA256         // HMAC_SHA512
#define PASS_HashSize   SHA256_DIGEST_SIZE  // SHA512_DIGEST_SIZE
#define PASS_SaltSize   SHA256_DIGEST_SIZE  // SHA256_BLOCK_SIZE, SHA512_BLOCK_SIZE ?
//---------------------------------------------------------------------------
class MPassword
{
private:
    unsigned char Salt[PASS_SaltSize];
    unsigned char Hash[PASS_HashSize];

public:
    unsigned GetDataSize() const;
    char *SetData(char *Data_) const;
    const char *GetData(const char *Data_, const char *Limit_);
    void Copy(MPassword *SrcPass_);

    void Set(char *Pass_);
    bool Check(const char *Pass_) const;

    MPassword();
    ~MPassword();
};

//---------------------------------------------------------------------------
#endif
