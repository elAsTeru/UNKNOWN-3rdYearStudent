#pragma once
#include <type_traits>

namespace System::Helper
{
	template <typename T, T beginVal, T endVal>
	class EnumIterator
	{
	public:
		// イテレーターの始端を示すコンストラクタ
		EnumIterator() :id(static_cast<element>(beginVal)) {}
		// イテレーターの終端を示すコンストラクタ
		EnumIterator(const T& _End) :id(static_cast<element>(_End)) {}
		// イテレーターが次の要素を示すように演算子++をオーバーロード
		EnumIterator operator++()
		{
			id++;
			return *this;
		}
		// イテレーターが現在指している要素を返すように間接演算子をオーバーロード
		T operator*()const { return static_cast<T>(id); }
		// 始端の要素
		EnumIterator begin()const { return *this; }
		// 終端の要素
		EnumIterator end()
		{
			static const EnumIterator endIte = ++EnumIterator(endVal);
			return endIte;
		}
		// イテレーターの終端を検知するために演算子!=をオーバーロード
		bool operator!= (const EnumIterator _End)const { return id != _End.id; }

		EnumIterator Incriment(const uint8_t& _IncrimentSize)
		{
			id += _IncrimentSize;
			return *this;
		}

		const int& GetId() { return this->id; }
	private:
		// Private Variable

		// 列挙型を安全にキャストできるように列挙型Tの基底型を取得
		// NOTE:underlying_type<T>::typeで列挙型Tの基底型を取得できる
		typedef typename std::underlying_type<T>::type element;
		// 現在指している要素の番号
		int id;
	};
}