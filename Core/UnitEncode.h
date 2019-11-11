//---------------------------------------------------------------------------
#ifndef UnitEncodeH
#define UnitEncodeH
//---------------------------------------------------------------------------
#include <cstddef>
//---------------------------------------------------------------------------
// ��������� ����������� ����� �����
template <typename type>
inline type BitsLeft(type Bits_)
{
	return (Bits_<<1)|(Bits_>>(sizeof(type)*8-1));
}

// ��������� ����������� ����� ������
template <typename type>
inline type BitsRight(type Bits_)
{
    return (Bits_>>1)|(Bits_<<(sizeof(type)*8-1));
}

// �������� ����� ����������
template <typename type>
inline type BasicEncodeRound(type Blk_, type Code_)
{
    Blk_+=Code_;                             // ���������� � ����������� ���������
    Blk_=BitsRight(Blk_);                    // ��������� ����������� ����� ������
    Blk_^=Code_;                             // ��������� ����������� "���"
    return Blk_;
}

// �������� ����� ������������
template <typename type>
inline type BasicDecodeRound(type Blk_, type Code_)
{
    Blk_^=Code_;                             // ��������� ����������� "���"
    Blk_=BitsLeft(Blk_);                     // ��������� ����������� ����� ������
    Blk_-=Code_;                             // ���������� � ����������� ���������
    return Blk_;
}

// ���������� ������-��������� ����������/������������
void BasicEncode(void *Data__, size_t DataSize_, unsigned Code_, int Round_=8);
void BasicDecode(void *Data__, size_t DataSize_, unsigned Code_, int Round_=8);
//---------------------------------------------------------------------------
#endif
