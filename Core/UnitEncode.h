﻿//---------------------------------------------------------------------------
#ifndef UnitEncodeH
#define UnitEncodeH
//---------------------------------------------------------------------------
#include <cstddef>
#include <cstdint>
//---------------------------------------------------------------------------
// Побитовый циклический сдвиг влево
template <typename type>
inline type BitsLeft(type Bits_) noexcept
{
	return (Bits_<<1)|(Bits_>>(sizeof(type)*8-1));
}

// Побитовый циклический сдвиг вправо
template <typename type>
inline type BitsRight(type Bits_) noexcept
{
	return (Bits_>>1)|(Bits_<<(sizeof(type)*8-1));
}

// Основной раунд шифрования
template <typename type>
inline type BasicEncodeRound(type Blk_, type Code_) noexcept
{
	Blk_+=Code_;                             // складываем с циклическим переносом
	Blk_=BitsRight(Blk_);                    // побитовый циклический сдвиг вправо
	Blk_^=Code_;                             // побитовое исключающее "или"
	return Blk_;
}

// Основной раунд дешифрования
template <typename type>
inline type BasicDecodeRound(type Blk_, type Code_) noexcept
{
	Blk_^=Code_;                             // побитовое исключающее "или"
	Blk_=BitsLeft(Blk_);                     // побитовый циклический сдвиг вправо
	Blk_-=Code_;                             // складываем с циклическим переносом
	return Blk_;
}

// Простейшее блочно-потоковое шифрование/дешифрование
void BasicEncode(void *Data__, std::size_t DataSize_, std::uint32_t Code_, int Round_=8);
void BasicDecode(void *Data__, std::size_t DataSize_, std::uint32_t Code_, int Round_=8);
//---------------------------------------------------------------------------
#endif
