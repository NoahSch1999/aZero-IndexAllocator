#pragma once
#include <type_traits>
#include <concepts>
#include <stack>
#include <vector>

namespace aZero
{
	namespace DS
	{
		template<typename T>
		concept UnsignedInteger =
			std::is_same_v<T, uint64_t> ||
			std::is_same_v<T, uint32_t> ||
			std::is_same_v<T, uint16_t> ||
			std::is_same_v<T, uint8_t>;

		/*
		An allocator that allocates indices incrementally from 0 and enables reuse of freed indices.
		IndexType has to be one of the following types:
			- unsigned 64-bit integer
			- unsigned 32-bit integer
			- unsigned 16-bit integer
			- unsigned 8-bit integer
		*/
		template<UnsignedInteger IndexType>
		class IndexAllocator
		{
		public:
			// The value of the index that signifies that it's invalid.
			constexpr static IndexType InvalidIndex = std::numeric_limits<IndexType>::max();

			/*
			A wrapper around IndexType.
			The wrapper is movable-only to avoid unintentional double-frees.
			*/
			class Index
			{
				friend IndexAllocator;
			public:
				Index() = default;
				Index(const Index&) = delete;
				Index& operator=(const Index&) = delete;

				Index(Index&& other) noexcept { this->Move(other); }

				Index& operator=(Index&& other) noexcept
				{
					if (*this != other)
					{
						this->Move(other);
					}

					return *this;
				}

				// Conversion function to make the class be implicitly or explicitly converted to the type of IndexType.
				operator IndexType() const { return m_Index; }

				// Returns whether or not the stored value is valid. TRUE: The stored value is valid.
				[[nodiscard]] bool IsValid() const { return m_Index != InvalidIndex; }

			private:
				Index(IndexType index)
					:m_Index(index) { }

				void Move(Index& other)
				{
					m_Index = other.m_Index;
					other.m_Index = InvalidIndex;
				}

				Index& operator=(const IndexType& index) { m_Index = index; return *this; };

				IndexType m_Index = InvalidIndex;
			};

			IndexAllocator() :m_NextFreeIndex(0) {}

			/*
			Returns a free index.
			Reuses previously freed indices LIFO-style or increments the current max allocated index if the list of freed indices is empty.
			Throws std::out_of_range if the list of free indices is empty and the max value of IndexType has been reached.
			*/
			[[nodiscard]] Index Allocate()
			{
				if (!m_FreeIndices.empty())
				{
					const IndexType index = m_FreeIndices.top();
					m_FreeIndices.pop();
					return index;
				}

				if (m_NextFreeIndex == InvalidIndex)
				{
					throw std::out_of_range("Max number of indices allocated.");
				}

				return Index(m_NextFreeIndex++);
			}

			/*
			Frees the index for reuse and invalidates the input.
			Return early if the index isn't valid.
			*/
			void Free(Index& index)
			{
				if (!index.IsValid())
					return;

				m_FreeIndices.push(std::move(index));
				index = InvalidIndex;
			}

			// Returns the current max index that has been allocated.
			[[nodiscard]] IndexType GetCurrentMax() const { return m_NextFreeIndex; }

			// Returns the number of reusable indices.
			[[nodiscard]] std::size_t GetFreeCount() const { return m_FreeIndices.size(); }

		private:
			std::stack<Index, std::vector<Index>> m_FreeIndices;
			IndexType m_NextFreeIndex;
		};
	}
}