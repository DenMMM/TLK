//---------------------------------------------------------------------------
#ifndef UnitListH
#define UnitListH
//---------------------------------------------------------------------------
#include <stddef.h>
#include <cstddef>
#include <vector>
#include <iterator>
#include <stdexcept>
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
class MListItem;

template <
	typename parent_item, typename base_type,
	typename item_type, unsigned char type_id>
class MListItemTyped;

template <typename parent_item,	typename item_type>
class MListItemSimple;

template <typename list_type, typename base_type>
class MList;

template <typename parent_list, typename item_type>
class MListSimple;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Базовые интерфейсы элемента списка
template <
	typename list_type,			// Имя будущего класса-списка
	typename base_type>			// Базовый (родительский) тип его элементов
class MListItem
{
	friend MList <list_type, base_type>;

private:
	base_type* Prev;			// Указатель на предыдущий элемент списка
	base_type* Next;			// Указатель на следующий элемент списка

public:
	// Доступ к элементам класса
	base_type* gPrev() const { return Prev; }
	base_type* gNext() const { return Next; }

	// Возвращает [двоичный] ID, ассоциированный с типом элемента
	virtual unsigned char gTypeID() const noexcept = 0;
	// Объявим виртуальное копирование через базовый тип
	virtual base_type& operator=(base_type&) = 0;
	virtual base_type& operator=(base_type&&) noexcept = 0;

	// Занулим указатели нового объекта
	MListItem(): Prev(nullptr), Next(nullptr) {}
	MListItem(MListItem&): MListItem() {}
	MListItem(MListItem&&): MListItem() {}
	// А при копировании сохраним их не тронутыми
	MListItem& operator=(MListItem&) { return *this; }
	MListItem& operator=(MListItem&&) noexcept { return *this; }

	virtual ~MListItem() = default;
};
//---------------------------------------------------------------------------
// Завершает элемент типизированного списка
template <
	typename parent_item,		// Родительский класс
	typename base_type,			// Базовый тип его элементов
	typename item_type,     	// Создаваемый тип элемента
	unsigned char type_id>  	// Его двоичный ID
class MListItemTyped: public parent_item
{
public:
	static const unsigned char TypeID = type_id;

	virtual unsigned char gTypeID() const override final
	{
		return type_id;			// TypeID
	}

	static item_type* New()
	{
		return new item_type;
	}

	virtual base_type& operator=(base_type& Src_) override ///final
	{
		item_type& Left=static_cast<item_type&>(*this);
		item_type& Right=dynamic_cast<item_type&>(Src_);
		Left=Right;
		return Left;
	}

	virtual base_type& operator=(base_type&& Src_) noexcept override ///final
	{
		item_type& Left=static_cast<item_type&>(*this);
		item_type& Right=dynamic_cast<item_type&>(Src_);
		Left=std::move(Right);
		return Left;
	}
};
//---------------------------------------------------------------------------
// Завершает элемент простого (однотипного) списка
template <
	typename parent_item,		// Родительский класс
	typename item_type>			// Тип элемента списка
class MListItemSimple: public MListItemTyped <parent_item, item_type, item_type, 0>
{
protected:
//	using MListItemTyped <parent_item, item_type, item_type, 0> ::TypeID;	/// = delete ?
//	using MListItemTyped <parent_item, item_type, item_type, 0> ::gTypeID;
	using MListItemTyped <parent_item, item_type, item_type, 0> ::operator=;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Реализация типизированного списка
template <
	typename list_type,			// Создаваемый класс списка (имя)
	typename base_type>			// Базовый тип элемента списка
class MList
{
private:
	base_type *First;			// Указатель на первый элемент списка
	base_type *Last;			// Указатель на последний элемент списка
	size_t Count;				// Количество элементов в списке
	std::vector <base_type*(*)()> NewForType;
	base_type* (*NewForTypeDef)();

protected:
	// Заполнение таблицы, используемой для создания элементов по TypeID
	void TypesIDPrepare(size_t Size_)
	{
		NewForTypeDef=nullptr;
		NewForType.clear();
		NewForType.resize(Size_, nullptr);
	}

	template <typename new_item_type>
	void TypesIDSet()
	{
		const size_t ID_=new_item_type::TypeID;

#ifdef _DEBUG
		// Проверим, что функция для этого ID_ еще не задана
		if ( NewForType.at(ID_)!=nullptr )
			throw std::runtime_error (
				"MList::TypesIDSet()\n"
				"Повторное использование одного и того же TypeID."
				);
#endif
		NewForType[ID_]=
			reinterpret_cast<base_type*(*)()>(&new_item_type::New);
	}

	void TypesIDDef(base_type*(*Ptr_)())
	{
		NewForType.clear();
		NewForTypeDef=Ptr_;
	}

	base_type *DefTypeNew() const
	{
		return NewForTypeDef();
	}

public:
	class const_iterator:
		public std::iterator <std::bidirectional_iterator_tag, base_type>
	{
	protected:
		typedef typename std::iterator <std::bidirectional_iterator_tag, base_type> ::pointer pointer;
		typedef typename std::iterator <std::bidirectional_iterator_tag, base_type> ::reference reference;

		pointer MyPtr;		// Адрес элемента, на который указывает итератор

	public:
		const_iterator(): MyPtr(nullptr) {}
		const_iterator(pointer Init_): MyPtr(Init_) {}

		const reference operator*() const { return *MyPtr; }
		const pointer operator->() const { return MyPtr; }

		const_iterator& operator++()
		{
			MyPtr=MyPtr->Next;
			return *this;
		}

		const_iterator operator++(int)
		{
			const_iterator Before=(*this);
			++(*this);
			return Before;
		}

		const_iterator& operator--()
		{
			MyPtr=MyPtr->Prev;
			return *this;
		}

		const_iterator operator--(int)
		{
			const_iterator Before=(*this);
			--(*this);
			return Before;
		}

		bool operator==(const const_iterator& Right_) const
		{
			return MyPtr==Right_.MyPtr;
		}

		bool operator!=(const const_iterator& Right_) const
		{
			return !((*this)==Right_);
		}
	};

	class iterator:
		public const_iterator
	{
	protected:
		typedef typename const_iterator::pointer pointer;
		typedef typename const_iterator::reference reference;
		using const_iterator::MyPtr;

	public:
		iterator() = default;
		iterator(pointer Init_): const_iterator(Init_) {}

		reference operator*() const { return *MyPtr; }
		pointer operator->() const { return MyPtr; }

		iterator& operator++()
		{
			++(*static_cast<const_iterator*>(this));
			return *this;
		}

		iterator operator++(int)
		{
			iterator Before=(*this);
			++(*this);
			return Before;
		}

		iterator& operator--()
		{
			--(*static_cast<const_iterator*>(this));
			return *this;
		}

		iterator operator--(int)
		{
			iterator Before=(*this);
			--(*this);
			return Before;
		}
	};

	iterator begin() noexcept { return iterator(First); }
	const_iterator begin() const noexcept { return const_iterator(First); }
	const_iterator cbegin() const noexcept { return const_iterator(First); }

	iterator end() noexcept { return iterator(nullptr); }
	const_iterator end() const noexcept { return const_iterator(nullptr); }
	const_iterator cend() const noexcept { return const_iterator(nullptr); }

protected:
	// Присоединяет к списку уэе созданный 'item::New()' элемент
	base_type *Add(base_type *NewItem_);

public:
	// Доступ к атрибутам списка
	base_type *gFirst() const { return First; }
	base_type *gLast() const { return Last; }
	size_t gCount() const { return Count; }

	// Проверяет можно ли добавлять элементы по ID
	bool isTyped() const { return NewForTypeDef==nullptr; }

	// Операции над отдельными элементами _одного_ списка
	template <typename new_item_type>
	base_type *Add()
	{
		return Add(new_item_type::New());
	}

	base_type *Add(unsigned char TypeID_);
	base_type *GetItem(size_t Index_) const;
	void Swap(base_type *Item1_, base_type *Item2_);
	void Del(base_type *DelItem_);

	// Операции над списком целиком (не трогая атрибуты наследника MList)
	void Clear();                       		// Удалить все элементы списка
	void Move(list_type *SrcList_) noexcept;    // Заместить элементы списка исходными
	void Splice(list_type *AtchList_);      	// Присоединить элементы исходного списка

	MList():
		First(nullptr),
		Last(nullptr),
		Count(0),
		NewForTypeDef(nullptr)
	{
	}

	MList& operator=(const MList& SrcList_);

	MList(const MList& SrcList_):
		First(nullptr),
		Last(nullptr),
		Count(0),
		NewForType(SrcList_.NewForType),
		NewForTypeDef(SrcList_.NewForTypeDef)
	{
		*this=SrcList_;
	}

	MList& operator=(MList&& SrcList_) noexcept
	{
		Move(static_cast<list_type*>(&SrcList_));   /// cast - временный хак
		return *this;
	}

	MList(MList&& SrcList_) noexcept:
		First(nullptr),
		Last(nullptr),
		Count(0),
		NewForType(std::move(SrcList_.NewForType)),
		NewForTypeDef(std::move(SrcList_.NewForTypeDef))
	{
		Move(&SrcList_);
	}

	~MList()
	{
		Clear();
	}
};
//---------------------------------------------------------------------------
// Заглушка для эмуляции простых списков
template <
	typename parent_list,       // Родительский класс списка
//	typename list_type,			// Создаваемый класс (имя)
	typename item_type>			// Тип элемента
class MListSimple: public parent_list
{
private:
	using parent_list::TypesIDPrepare;
	using parent_list::TypesIDSet;
	using parent_list::TypesIDDef;

protected:
	using parent_list::isTyped;
	using parent_list::Add;

public:
//	typedef typename Iterators<item_type>::const_iterator const_iterator;
//	typedef typename Iterators<item_type>::iterator iterator;

	item_type* Add() {
		return static_cast<item_type*>(parent_list::template Add<item_type>()); }

	MListSimple()
	{
		TypesIDDef(&item_type::New);		/// грубовато, в стиле 'C'
	}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
base_type *MList<list_type,base_type>::Add(base_type *NewItem_)
{
	// Проверим, что объект еще не привязан к какому-либо списку
	if (
		NewItem_->Prev!=nullptr ||
		NewItem_->Next!=nullptr )
	{
		throw std::runtime_error (
			"MList::Add()\n"
			"Объект уже добавлен в список."
			);
	}
	// Инициализируем объект
	NewItem_->Prev=Last;
	NewItem_->Next=nullptr;
	// Изменяем атрибуты самого списка и предыдущего объекта
	if ( First==nullptr ) First=NewItem_;
	else Last->Next=NewItem_;
	Last=NewItem_;
	// Корректируем счетчик
	Count++;

	return NewItem_;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
base_type *MList<list_type,base_type>::Add(unsigned char TypeID_)
{
	base_type* (*NewF)()=NewForTypeDef;

	if ( NewF==nullptr )
	{
		if ( TypeID_>=NewForType.size() ) return nullptr;
		// Возьмем из массива адрес функции, создающей элемент списка
		NewF=NewForType[TypeID_];
		if ( NewF==nullptr ) return nullptr;
	}

	// Создадим новый элемент и добавим его к списку
	return Add(NewF());
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
base_type *MList<list_type,base_type>::GetItem(size_t Index_) const
{
	if ( Index_>=Count )
	{
		throw std::out_of_range (
			"MList<list_type,base_type>::GetItem()\n"
			"Попытка выйти за границы списка."
			);
	}

	base_type *SearchItem=First;
	// Ищем по цепочке указатель на нужный элемент
	for ( size_t i=0; (i!=Index_)&&(SearchItem!=nullptr); i++ )
		SearchItem=SearchItem->Next;

	return SearchItem;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void MList<list_type,base_type>::Swap(base_type *Item1_, base_type *Item2_)
{
	if ( Count==0 )
	{
		throw std::runtime_error (
			"MList<list_type,base_type>::Swap()\n"
			"Список пуст."
			);
	}

	if (
		Item1_==nullptr ||
		Item2_==nullptr )
	{
		throw std::runtime_error (
			"MList<list_type,base_type>::Swap()\n"
			"Объект не существует (nullptr)."
			);
	}

	if ( Item1_==Item2_ ) return;

	base_type *Item;

	if ( Item1_->Prev ) Item1_->Prev->Next=Item2_;
	if ( Item2_->Prev ) Item2_->Prev->Next=Item1_;
	Item=Item1_->Prev;
	Item1_->Prev=Item2_->Prev;
	Item2_->Prev=Item;

	if ( Item1_->Next ) Item1_->Next->Prev=Item2_;
	if ( Item2_->Next ) Item2_->Next->Prev=Item1_;
	Item=Item1_->Next;
	Item1_->Next=Item2_->Next;
	Item2_->Next=Item;

	if ( First==Item1_ ) First=Item2_;
	else if ( First==Item2_ ) First=Item1_;

	if ( Last==Item1_ ) Last=Item2_;
	else if ( Last==Item2_ ) Last=Item1_;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void MList<list_type,base_type>::Del(base_type *DelItem_)
{
	if ( Count==0 )
	{
		throw std::runtime_error (
			"MList<list_type,base_type>::Del()\n"
			"Список пуст."
			);
	}

	if ( DelItem_==nullptr )
	{
		throw std::runtime_error (
			"MList<list_type,base_type>::Del()\n"
			"Объект не существует (nullptr)."
			);
	}

	if (
		DelItem_->Prev==nullptr &&
		DelItem_->Next==nullptr &&
		DelItem_!=First )
	{
		throw std::runtime_error (
			"MList<list_type,base_type>::Del()\n"
			"Объект не принадлежит списку."
			);
	}

	// Разбираемся с предыдущим элементом списка и его началом
	if ( DelItem_->Prev!=nullptr ) DelItem_->Prev->Next=DelItem_->Next;
	else First=DelItem_->Next;
	// Разбираемся со следующим элементом списка и его концом
	if ( DelItem_->Next!=nullptr ) DelItem_->Next->Prev=DelItem_->Prev;
	else Last=DelItem_->Prev;
	// Корректируем счетчик
	Count--;

	// Удаляем объект из памяти
	delete DelItem_;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void MList<list_type,base_type>::Clear()
{
	base_type *del=First, *next;

	while(del!=nullptr)
	{
		next=del->Next;
		delete del;
		del=next;
	}

	First=Last=nullptr;
	Count=0;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
MList<list_type,base_type>& MList<list_type,base_type>::operator=(const MList& SrcList_)
{
	if ( (&SrcList_)==this ) return *this;

	// Очищаем список-приемник
	Clear();
	try
	{
		// Копируем в него элементы исходного
		base_type *SrcItem=SrcList_.First, *NewItem;
		while(SrcItem)
		{
			NewItem=Add(SrcItem->gTypeID());
			*NewItem=*SrcItem;
			SrcItem=SrcItem->Next;
		}
	} catch(...)
	{
		// Раз список не заполнился, очистим его
		Clear();
		// И передадим исключение выше
		throw;
	}

	return *this;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void MList<list_type,base_type>::Move(list_type *SrcList_) noexcept
{
	if ( SrcList_==this ) return;

	// Очищаем список-приемник
	Clear();
	// Копируем ссылки на элементы и их количество
	First=SrcList_->First;
	Last=SrcList_->Last;
	Count=SrcList_->Count;
	// Обнуляем эти поля в исходном списке
	SrcList_->First=nullptr;
	SrcList_->Last=nullptr;
	SrcList_->Count=0;
}
//---------------------------------------------------------------------------
template <typename list_type, typename base_type>
void MList<list_type,base_type>::Splice(list_type *AtchList_)
{
	if ( AtchList_==this ) return;

	// Проверяем есть ли что присоединять
	if ( AtchList_->First==nullptr ) return;
	// Связываем списки
	if ( First==nullptr ) First=AtchList_->First;
	else
	{
		Last->Next=AtchList_->First;
		Last->Next->Prev=Last;
	}
	Last=AtchList_->Last;
	// Увеличиваем счетчик
	Count+=AtchList_->Count;
	// Отсоединяем элементы от исходного списка
	AtchList_->First=nullptr;
	AtchList_->Last=nullptr;
	AtchList_->Count=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif

