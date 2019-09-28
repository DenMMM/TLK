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
	virtual unsigned char gTypeID() const = 0;
	// Оператор 'delete' для производного от MListItem класса
//	virtual MListItem* Destroy() const = 0;
	// В производном классе реализует 'operator='
	virtual void Copy(const MListItem* SrcItem_) = 0;
	// Создает копию объекта без лишних 'dynamic_cast' (оптимизация)
//	virtual MListItem* Clone() const = 0;

	MListItem():
		Prev(nullptr),
		Next(nullptr)
	{
	}

	virtual ~MListItem() = default;


public:
	// Прокси-класс, чтобы реже использовать 'dynamic_cast'
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

		Proxy() = default;
		virtual ~Proxy() override = default;
	};

	// Расширение базовых интерфейсов, зависящее от типа элемента списка
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
/*
		virtual void Destroy() const override final
		{
			delete static_cast<item_type*>(this);	/// Заменен виртуальным деструктором
		}

		virtual MListItem* Clone() const override final
		{
			item_type* new_obj=item_type::New();
			*new_obj = *(static_cast<item_type*>(this));

			return static_cast<MListItem*>(new_obj);
		}
*/
		Typed() = default;
		virtual ~Typed() override = default;
	};

	// Формирует элемент простого (однотипного) списка
	template <
		typename parent_item,
		typename item_type>
	class Simple: public Typed <parent_item, item_type, 0>
	{
	protected:
		using parent_item::gTypeID;

	public:
		Simple() = default;
		virtual ~Simple() override = default;
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

	void TypesIDSet(size_t ID_, MListItem*(*Ptr_)())
	{
#ifdef _DEBUG
		// Проверим, что функция для этого ID_ еще не задана
		if ( NewForType.at(ID_)!=nullptr )
			throw std::runtime_error (
				"MList::TypesIDSet()\n"
				"Повторное использование одного и того же TypeID."
				);
#endif
		NewForType[ID_]=Ptr_;
	}

public:
	// Доступ к атрибутам списка
	MListItem *gFirst() const { return First; }
	MListItem *gLast() const { return Last; }
	size_t gCount() const { return Count; }

	// Проверяет можно ли добавлять элементы по ID
	bool isTyped() const { return NewForTypeDef==nullptr; }

	// Операции над отдельными элементами _одного_ списка
	MListItem *Add(MListItem *NewItem_);
	MListItem *Add(unsigned char TypeID_);
	MListItem *GetItem(size_t Index_) const;
	void Swap(MListItem *Item1_, MListItem *Item2_);
	void Del(MListItem *DelItem_);

	// Операции над списком целиком (не трогая атрибуты наследника MList)
	void Clear();                       // Удалить все элементы списка
	void Copy(const MList *SrcList_);   // Создать копию элементов исходного списка
	void Move(MList *SrcList_);         // Заместить элементы списка исходными      /// заменить на swap ?
	void Splice(MList *AtchList_);      // Присоединить элементы исходного списка

	MList():
		First(nullptr),
		Last(nullptr),
		Count(0),
		NewForTypeDef(nullptr)
	{
	}

	~MList()
	{
		Clear();
	}


public:
	// Прокси-класс, чтобы не использовать лишний раз 'dynamic_cast'
	template <
		typename parent_list,	// Список, которй будем проксировать
		typename list_type,     // Создаваемый класс списка
		typename base_type>		// Базовый класс элемента списка
	class Typed: public parent_list
	{
	public:
		base_type* gFirst() const {
			return static_cast<base_type*>(parent_list::gFirst()); }
		base_type* gLast() const {
			return static_cast<base_type*>(parent_list::gLast()); }

		base_type* Add(base_type* NewItem_) {
			return static_cast<base_type*>(parent_list::Add(NewItem_)); }
		base_type* Add(unsigned char TypeID_) {
			return static_cast<base_type*>(parent_list::Add(TypeID_)); }
		base_type* GetItem(size_t Index_) const {
			return static_cast<base_type*>(parent_list::GetItem(Index_)); }
		void Swap(base_type* Item1_, base_type* Item2_) {
			parent_list::Swap(Item1_,Item2_); }
		void Del(base_type* DelItem_) {
			parent_list::Del(DelItem_); }

		void Copy(const list_type* SrcList_) { parent_list::Copy(SrcList_); }
		void Move(list_type* SrcList_) { parent_list::Move(SrcList_); }
		void Splice(list_type* AtchList_) { parent_list::Splice(AtchList_); }

		Typed() = default;
		~Typed() = default;
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

	public:
		item_type* Add(item_type* NewItem_) {
			return static_cast<item_type*>(parent_list::Add(NewItem_)); }
		item_type* Add() {
			return static_cast<item_type*>(parent_list::Add(item_type::New())); }

		Simple()
		{
			NewForTypeDef=
				reinterpret_cast<MListItem*(*)()>   /// грубовато, в стиле 'C'
				(&item_type::New);
		}

		~Simple() = default;
	};
};
//---------------------------------------------------------------------------
#endif

/*
	// Prohibited operations
	MPassword();
	MDirNotify (const MDirNotify &Src_);
	MDirNotify (MDirNotify &&Src_);

	MDirNotify &operator=(const MDirNotify &Src_);
	MDirNotify &operator=(MDirNotify &&Src_);
*/

