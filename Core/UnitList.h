//---------------------------------------------------------------------------
#ifndef UnitListH
#define UnitListH
//---------------------------------------------------------------------------
#include <cstddef>
#include <vector>
//---------------------------------------------------------------------------
class MListItem;
class MList;
//---------------------------------------------------------------------------
// Элемент списка с бызовыми интерфейсами
class MListItem
{
	friend class MList;

private:
	MListItem* Prev;		// Указатель на предыдущий элемент списка
	MListItem* Next;    	// Указатель на следующий элемент списка

public:
	// Доступ к элементам класса
	MListItem* gPrev() const { return Prev; }
	MListItem* gNext() const { return Next; }

	// Возвращает [двоичный] ID, ассоциированный с типом элемента
	virtual unsigned char gTypeID() const noexcept = 0;
	// Объявим копирование через базовый тип
	// и защитим указатели от изменения
	virtual MListItem& operator=(const MListItem&) { return *this; }
	virtual MListItem& operator=(MListItem&&) noexcept { return *this; }

	// Занулим указатели нового объекта
	MListItem(): Prev(nullptr), Next(nullptr) {}
	MListItem(MListItem&): MListItem() {}
	MListItem(MListItem&&): MListItem() {}
	virtual ~MListItem() = default;


public:
	// Приводит типы в интерфейсах к желаемому виду (замена шаблонизации MListItem)
	template <
		typename parent_item,	// Класс элемента, который будем расширять
		typename base_type>     // Базовый класс элемента списка (например, MListItem)
	class Proxy: public parent_item
	{
	protected:
		using parent_item::gPrev;
		using parent_item::gNext;

	public:
		base_type* gPrev() const {
			return static_cast<base_type*>(parent_item::gPrev()); }
		base_type* gNext() const {
			return static_cast<base_type*>(parent_item::gNext()); }
	};

	// Завершает формироание элемента типизированного списка
	template <
		typename parent_item,	// Класс элемента, который будем расширять
		typename item_type,     // Создаваемый класс (фактический тип элемента)
		unsigned char type_id>  // Двоичный ID этого класса
	class Typed: public parent_item
	{
	public:
		static const unsigned char TypeID = type_id;

		virtual unsigned char gTypeID() const override final
		{
			return type_id; 	// TypeID
		}

		static item_type* New()
		{
			return new item_type;
		}

		virtual MListItem& operator=(const MListItem& Src_) override //final
		{
			item_type& Left=static_cast<item_type&>(*this);
			const item_type& Right=dynamic_cast<const item_type&>(Src_);
			Left=Right;
			return *this;
		}

		virtual MListItem& operator=(MListItem&& Src_) noexcept override //final
		{
			item_type& Left=static_cast<item_type&>(*this);
			item_type& Right=dynamic_cast<item_type&>(Src_);
			Left=std::move(Right);
			return *this;
		}
	};

	// Завершает формирование элемента простого списка
	template <
		typename parent_item,
		typename item_type>
	class Simple: public Typed <parent_item, item_type, 0>
	{
	protected:
		using Typed <parent_item, item_type, 0> ::TypeID;
		using parent_item::gTypeID;
	};
};
//---------------------------------------------------------------------------
class MList
{
private:
	MListItem *First;    	// Указатель на первый элемент списка
	MListItem *Last;     	// Указатель на последний элемент списка
	size_t Count;     		// Количество элементов в списке
	std::vector <MListItem*(*)()> NewForType;
	MListItem* (*NewForTypeDef)();

protected:
	// Заполнение таблицы, используемой для создания элементов по TypeID
	void TypesIDPrepare(size_t Size_)
	{
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
			reinterpret_cast<MListItem*(*)()>(&new_item_type::New);
	}

	// Присоединяет к списку уэе созданный 'item::New()' элемент
	MListItem *Add(MListItem *NewItem_);

public:
	// Доступ к атрибутам списка
	MListItem *gFirst() const { return First; }
	MListItem *gLast() const { return Last; }
	size_t gCount() const { return Count; }

	// Проверяет можно ли добавлять элементы по ID
	bool isTyped() const { return NewForTypeDef==nullptr; }

	// Операции над отдельными элементами _одного_ списка
	template <typename new_item_type>
	MListItem *Add()
	{
		return Add(new_item_type::New());
	}

	MListItem *Add(unsigned char TypeID_);
	MListItem *GetItem(size_t Index_) const;
	void Swap(MListItem *Item1_, MListItem *Item2_);
	void Del(MListItem *DelItem_);

	// Операции над списком целиком (не трогая атрибуты наследника MList)
	void Clear() noexcept;                  // Удалить все элементы списка
	void Move(MList *SrcList_) noexcept;    // Заместить элементы списка исходными
	void Splice(MList *AtchList_);			// Присоединить элементы исходного списка

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
		Move(&SrcList_);
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


public:
	// Приводит типы в интерфейсах к желаемому виду (замена шаблонизации MList)
	template <
		typename parent_list,	// Список, которй будем проксировать
		typename list_type,     // Создаваемый класс списка
		typename base_type>		// Базовый класс элемента списка
	class Typed: public parent_list
	{
	protected:
		base_type* Add(base_type* NewItem_) {
			return static_cast<base_type*>(parent_list::Add(NewItem_)); }

	public:
		base_type* gFirst() const {
			return static_cast<base_type*>(parent_list::gFirst()); }
		base_type* gLast() const {
			return static_cast<base_type*>(parent_list::gLast()); }

		template <typename new_item_type>
		base_type* Add() {
			return static_cast<base_type*>(parent_list::template Add<new_item_type>()); }

		base_type* Add(unsigned char TypeID_) {
			return static_cast<base_type*>(parent_list::Add(TypeID_)); }
		base_type* GetItem(size_t Index_) const {
			return static_cast<base_type*>(parent_list::GetItem(Index_)); }
		void Swap(base_type* Item1_, base_type* Item2_) {
			parent_list::Swap(Item1_,Item2_); }
		void Del(base_type* DelItem_) {
			parent_list::Del(DelItem_); }

		void Move(list_type* SrcList_) { parent_list::Move(SrcList_); }
		void Splice(list_type* AtchList_) { parent_list::Splice(AtchList_); }
	};

	// То же самое, но добавляет 'Add()' с типом по-умолчанию
	template <
		typename parent_list,
		typename list_type,
		typename item_type>
	class Simple: public Typed <parent_list, list_type, item_type>
	{
	protected:
		using parent_list::isTyped;
		using parent_list::Add;

	public:
		item_type* Add() {
			return static_cast<item_type*>(parent_list::template Add<item_type>()); }

		Simple()
		{
			NewForTypeDef=
				reinterpret_cast<MListItem*(*)()>   /// грубовато, в стиле 'C'
				(&item_type::New);
		}
	};
};
//---------------------------------------------------------------------------
#endif

