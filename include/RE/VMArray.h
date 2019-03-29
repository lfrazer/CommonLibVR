#pragma once

#include <cassert>  // assert
#include <iterator>  // random_access_iterator_tag
#include <type_traits>  // enable_if_t, is_integral

#include "RE/BSScriptArray.h"  // BSScriptArray
#include "RE/BSScriptVariable.h"  // BSScriptVariable
#include "RE/BSTSmartPointer.h"  // BSTSmartPointer
#include "RE/VirtualMachine.h"  // Internal::VirtualMachine


namespace RE
{
	namespace BSScript
	{
		template <class T>
		class VMArray
		{
		public:
			using value_type = T;
			using size_type = typename BSScriptArray::size_type;
			using difference_type = BSScriptArray::difference_type;
			using pointer = value_type * ;
			using const_pointer = const value_type*;


			struct const_reference
			{
			public:
				[[nodiscard]] operator value_type() const { return _val.Unpack<value_type>(); }

				[[nodiscard]] bool operator==(const const_reference& a_rhs) const;
				[[nodiscard]] bool operator!=(const const_reference& a_rhs) const;

			protected:
				friend class VMArray<T>;

				const_reference(BSScriptVariable& a_val);

				BSScriptVariable& _val;
			};


			struct reference : const_reference
			{
			public:
				reference& operator=(const value_type& a_val);
				template <typename std::enable_if_t<std::is_integral<T>::value, int> = 0> reference& operator+=(const value_type& a_val);

			protected:
				friend class VMArray<T>;

				reference(BSScriptVariable& a_val);
			};


			template <class U>
			struct iterator_base
			{
			public:
				iterator_base();
				iterator_base(BSScriptArray::iterator_base<U> a_iter);

				[[nodiscard]] constexpr reference operator*() const;
				[[nodiscard]] constexpr pointer operator->() const;

				constexpr iterator_base& operator++();
				constexpr iterator_base operator++(int);

				constexpr iterator_base& operator--();
				constexpr iterator_base operator--(int);

				constexpr iterator_base& operator+=(const difference_type a_offset);
				[[nodiscard]] constexpr iterator_base operator+(const difference_type a_offset) const;

				constexpr iterator_base& operator-=(const difference_type a_offset);
				[[nodiscard]] constexpr iterator_base operator-(const difference_type a_offset) const;
				[[nodiscard]] constexpr difference_type operator-(const iterator_base& a_rhs) const;

				[[nodiscard]] constexpr reference operator[](const difference_type a_offset) const;

				[[nodiscard]] constexpr bool operator==(const iterator_base& a_rhs) const;
				[[nodiscard]] constexpr bool operator!=(const iterator_base& a_rhs) const;
				[[nodiscard]] constexpr bool operator<(const iterator_base& a_rhs) const;
				[[nodiscard]] constexpr bool operator>(const iterator_base& a_rhs) const;
				[[nodiscard]] constexpr bool operator<=(const iterator_base& a_rhs) const;
				[[nodiscard]] constexpr bool operator>=(const iterator_base& a_rhs) const;

			protected:
				BSScriptArray::iterator_base<U> _iter;
			};


			using iterator = iterator_base<BSScriptVariable>;
			using const_iterator = iterator_base<const BSScriptVariable>;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;


			VMArray();
			VMArray(const VMArray& a_rhs);
			VMArray(VMArray&& a_rhs);
			VMArray(BSScriptArray* a_arr);
			VMArray(const BSTSmartPointer<BSScriptArray>& a_arrPtr);
			VMArray(BSTSmartPointer<BSScriptArray>&& a_arrPtr);

			VMArray& operator=(const VMArray& a_rhs);
			VMArray& operator=(VMArray&& a_rhs);
			VMArray& operator=(BSScriptArray* a_arr);
			VMArray& operator=(const BSTSmartPointer<BSScriptArray>& a_arrPtr);
			VMArray& operator=(BSTSmartPointer<BSScriptArray>&& a_arrPtr);

			[[nodiscard]] constexpr reference at(size_type a_pos);
			[[nodiscard]] constexpr const_reference at(size_type a_pos) const;

			[[nodiscard]] reference operator[](size_type a_pos);
			[[nodiscard]] const_reference operator[](size_type a_pos) const;

			[[nodiscard]] constexpr reference front();
			[[nodiscard]] constexpr const_reference front() const;

			[[nodiscard]] constexpr reference back();
			[[nodiscard]] constexpr const_reference back() const;

			[[nodiscard]] constexpr BSScriptArray* data() noexcept;
			[[nodiscard]] constexpr const BSScriptArray* data() const noexcept;

			[[nodiscard]] constexpr iterator begin() noexcept;
			[[nodiscard]] constexpr const_iterator begin() const noexcept;
			[[nodiscard]] constexpr const_iterator cbegin() const noexcept;

			[[nodiscard]] constexpr iterator end() noexcept;
			[[nodiscard]] constexpr const_iterator end() const noexcept;
			[[nodiscard]] constexpr const_iterator cend() const noexcept;

			[[nodiscard]] constexpr reverse_iterator rbegin() noexcept;
			[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept;
			[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept;

			[[nodiscard]] constexpr reverse_iterator rend() noexcept;
			[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept;
			[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept;

			[[nodiscard]] constexpr bool empty() const noexcept;

			[[nodiscard]] constexpr size_type size() const noexcept;

			void resize(size_type a_count);

		protected:
			BSTSmartPointer<BSScriptArray> alloc(size_type a_count);

			// members
			BSTSmartPointer<BSScriptArray> _data;
		};


		template <class T>
		VMArray<T>::const_reference::const_reference(BSScriptVariable& a_val) :
			_val(a_val)
		{}


		template <class T>
		[[nodiscard]] bool VMArray<T>::const_reference::operator==(const const_reference& a_rhs) const
		{
			return _val == a_rhs._val;
		}


		template <class T>
		[[nodiscard]] bool VMArray<T>::const_reference::operator!=(const const_reference& a_rhs) const
		{
			return !operator==(a_rhs);
		}


		template <class T>
		VMArray<T>::reference::reference(BSScriptVariable& a_val) :
			const_reference(a_val)
		{}


		template <class T>
		auto VMArray<T>::reference::operator=(const value_type& a_val)
			-> reference&
		{
			_val.Pack(a_val);
			return *this;
		}


		template <class T>
		template <typename std::enable_if_t<std::is_integral<T>::value, int>>
		auto VMArray<T>::reference::operator+=(const value_type& a_val)
			-> reference&
		{
			_val.Pack(_val.Unpack<value_type>() + a_val);
			return *this;
		}


		template <class T>
		template <class U>
		VMArray<T>::iterator_base<U>::iterator_base() :
			_iter()
		{}



		template <class T>
		template <class U>
		VMArray<T>::iterator_base<U>::iterator_base(BSScriptArray::iterator_base<U> a_iter) :
			_iter(a_iter)
		{}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr auto VMArray<T>::iterator_base<U>::operator*() const
			-> reference
		{
			return _iter.operator*();
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr auto VMArray<T>::iterator_base<U>::operator->() const
			-> pointer
		{
			return _iter.operator->();
		}


		template <class T>
		template <class U>
		constexpr auto VMArray<T>::iterator_base<U>::operator++()
			-> iterator_base&
		{
			_iter.operator++();
			return *this;
		}


		template <class T>
		template <class U>
		constexpr auto VMArray<T>::iterator_base<U>::operator++(int)
			-> iterator_base
		{
			return _iter.operator++(0);
		}


		template <class T>
		template <class U>
		constexpr auto VMArray<T>::iterator_base<U>::operator--()
			-> iterator_base&
		{
			_iter.operator--();
			return *this;
		}


		template <class T>
		template <class U>
		constexpr auto VMArray<T>::iterator_base<U>::operator--(int)
			-> iterator_base
		{
			return _iter.operator--(0);
		}


		template <class T>
		template <class U>
		constexpr auto VMArray<T>::iterator_base<U>::operator+=(const difference_type a_offset)
			-> iterator_base&
		{
			_iter.operator+=(a_offset);
			return *this;
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr auto VMArray<T>::iterator_base<U>::operator+(const difference_type a_offset) const
			-> iterator_base
		{
			return _iter.operator+(a_offset);
		}


		template <class T>
		template <class U>
		constexpr auto VMArray<T>::iterator_base<U>::operator-=(const difference_type a_offset)
			-> iterator_base&
		{
			_iter.operator-=(a_offset);
			return *this;
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr auto VMArray<T>::iterator_base<U>::operator-(const difference_type a_offset) const
			-> iterator_base
		{
			return _iter.operator-(a_offset);
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr auto VMArray<T>::iterator_base<U>::operator-(const iterator_base& a_rhs) const
			-> difference_type
		{
			return _iter.operator-(a_rhs._iter);
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr auto VMArray<T>::iterator_base<U>::operator[](const difference_type a_offset) const
			-> reference
		{
			return _iter.operator[](a_offset);
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr bool VMArray<T>::iterator_base<U>::operator==(const iterator_base& a_rhs) const
		{
			return _iter.operator==(a_rhs._iter);
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr bool VMArray<T>::iterator_base<U>::operator!=(const iterator_base& a_rhs) const
		{
			return _iter.operator!=(a_rhs._iter);
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr bool VMArray<T>::iterator_base<U>::operator<(const iterator_base& a_rhs) const
		{
			return _iter.operator<(a_rhs._iter);
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr bool VMArray<T>::iterator_base<U>::operator>(const iterator_base& a_rhs) const
		{
			return _iter.operator>(a_rhs._iter);
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr bool VMArray<T>::iterator_base<U>::operator<=(const iterator_base& a_rhs) const
		{
			return _iter.operator<=(a_rhs._iter);
		}


		template <class T>
		template <class U>
		[[nodiscard]] constexpr bool VMArray<T>::iterator_base<U>::operator>=(const iterator_base& a_rhs) const
		{
			return _iter.operator>=(a_rhs._iter);
		}


		template <class T>
		VMArray<T>::VMArray() :
			_data(nullptr)
		{}


		template <class T>
		VMArray<T>::VMArray(const VMArray& a_rhs) :
			_data(nullptr)
		{
			if (a_rhs._data) {
				_data = alloc(a_rhs.size());

				for (std::size_t i = 0; i < size(); ++i) {
					operator[](i) = a_rhs[i];
				}
			}
		}


		template <class T>
		VMArray<T>::VMArray(VMArray&& a_rhs) :
			_data(std::move(a_rhs._data))
		{}


		template <class T>
		VMArray<T>::VMArray(BSScriptArray* a_arr) :
			_data(a_arr)
		{}


		template <class T>
		VMArray<T>::VMArray(const BSTSmartPointer<BSScriptArray>& a_arrPtr) :
			_data(a_arrPtr)
		{}


		template <class T>
		VMArray<T>::VMArray(BSTSmartPointer<BSScriptArray>&& a_arrPtr) :
			_data(std::move(a_arrPtr))
		{}


		template <class T>
		auto VMArray<T>::operator=(const VMArray& a_rhs)
			-> VMArray&
		{
			if (!a_rhs._data) {
				_data = nullptr;
			} else if (size() != a_rhs.size()) {
				_data = alloc(a_rhs.size());
			}

			if (_data && a_rhs._data) {
				for (std::size_t i = 0; i < size(); ++i) {
					operator[](i) = a_rhs[i];
				}
			}

			return *this;
		}


		template <class T>
		auto VMArray<T>::operator=(VMArray&& a_rhs)
			-> VMArray&
		{
			_data = std::move(a_rhs._data);
			return *this;
		}


		template <class T>
		auto VMArray<T>::operator=(BSScriptArray* a_arr)
			-> VMArray&
		{
			_data.reset(a_arr);
		}


		template <class T>
		auto VMArray<T>::operator=(const BSTSmartPointer<BSScriptArray>& a_arrPtr)
			-> VMArray&
		{
			_data = a_arrPtr;
		}


		template <class T>
		auto VMArray<T>::operator=(BSTSmartPointer<BSScriptArray>&& a_arrPtr)
			-> VMArray&
		{
			_data = std::move(a_arrPtr);
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::at(size_type a_pos)
			-> reference
		{
			assert(_data.get() != 0);
			return _data->at(a_pos);
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::at(size_type a_pos) const
			-> const_reference
		{
			assert(_data.get() != 0);
			return _data->at(a_pos);
		}


		template <class T>
		[[nodiscard]] auto VMArray<T>::operator[](size_type a_pos)
			-> reference
		{
			assert(_data.get() != 0);
			return reference(_data->operator[](a_pos));
		}


		template <class T>
		[[nodiscard]] auto VMArray<T>::operator[](size_type a_pos) const
			-> const_reference
		{}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::front()
			-> reference
		{
			return operator[](0);
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::front() const
			-> const_reference
		{
			return operator[](0);
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::back()
			-> reference
		{
			return operator[](size() - 1);
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::back() const
			-> const_reference
		{
			return operator[](size() - 1);
		}


		template <class T>
		[[nodiscard]] constexpr BSScriptArray* VMArray<T>::data() noexcept
		{
			return _data.get();
		}


		template <class T>
		[[nodiscard]] constexpr const BSScriptArray* VMArray<T>::data() const noexcept
		{
			return _data.get();
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::begin() noexcept
			-> iterator
		{
			assert(_data.get() != 0);
			return iterator(_data->begin());
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::begin() const noexcept
			-> const_iterator
		{
			assert(_data.get() != 0);
			return const_iterator(_data->cbegin());
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::cbegin() const noexcept
			-> const_iterator
		{
			return begin();
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::end() noexcept
			-> iterator
		{
			assert(_data.get() != 0);
			return iterator(_data->end());
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::end() const noexcept
			-> const_iterator
		{
			assert(_data.get() != 0);
			return const_iterator(_data->cend());
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::cend() const noexcept
			-> const_iterator
		{
			return end();
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::rbegin() noexcept
			-> reverse_iterator
		{
			assert(_data.get() != 0);
			return reverse_iterator(_data->rbegin());
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::rbegin() const noexcept
			-> const_reverse_iterator
		{
			assert(_data.get() != 0);
			return const_reverse_iterator(_data->crbegin());
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::crbegin() const noexcept
			-> const_reverse_iterator
		{
			return rbegin();
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::rend() noexcept
			-> reverse_iterator
		{
			assert(_data.get() != 0);
			return reverse_iterator(_data->rend());
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::rend() const noexcept
			-> const_reverse_iterator
		{
			assert(_data.get() != 0);
			return const_reverse_iterator(_data->crend());
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::crend() const noexcept
			-> const_reverse_iterator
		{
			return rend();
		}


		template <class T>
		[[nodiscard]] constexpr bool VMArray<T>::empty() const noexcept
		{
			return _data ? true : _data->empty();
		}


		template <class T>
		[[nodiscard]] constexpr auto VMArray<T>::size() const noexcept
			-> size_type
		{
			return _data ? 0 : _data->size();
		}


		template <class T>
		void VMArray<T>::resize(size_type a_count)
		{
			auto newArrPtr = alloc(a_count);
			if (_data) {
				auto& oldArr = *_data.get();
				auto& newArr = *newArrPtr.get();
				for (std::size_t i = 0; i < oldArr.size(); ++i) {
					newArr[i] = std::move(oldArr[i]);
				}
			}
			_data = newArrPtr;
		}


		template <class T>
		BSTSmartPointer<BSScriptArray> VMArray<T>::alloc(size_type a_count)
		{
			auto vm = Internal::VirtualMachine::GetSingleton();
			BSTSmartPointer<BSScriptArray> arrPtr;
			auto typeID = GetTypeID<value_type>();
			bool allocSuccess = vm->AllocateArray(typeID, a_count, arrPtr);
			assert(allocSuccess);	// alloc failed
			return arrPtr;
		}
	}
}