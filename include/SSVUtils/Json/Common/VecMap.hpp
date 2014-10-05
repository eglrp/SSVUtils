// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVU_JSON_COMMON_VECMAP
#define SSVU_JSON_COMMON_VECMAP

namespace ssvu
{
	namespace Json
	{
		namespace Internal
		{
			template<typename TDerived> class VecBase
			{
				private:
					inline auto& getTD() noexcept				{ return castUp<TDerived>(*this); }
					inline const auto& getTD() const noexcept	{ return castUp<TDerived>(*this); }

				public:
					template<typename T> inline bool has(const T& mValue) const noexcept { return getTD().is(getTD().lookup(mValue), mValue); }

					inline auto& getData() noexcept { return getTD().data; }
					inline const auto& getData() const noexcept { return getTD().data; }

					template<typename TC> inline auto operator==(const TC& mC) const noexcept { return getData() == mC.getData(); }
					template<typename TC> inline auto operator!=(const TC& mC) const noexcept { return !(operator==(mC)); }

					inline void reserve(std::size_t mV) { getData().reserve(mV); }

					inline void clear()		noexcept		{ getData().clear(); }

					inline auto size()		const noexcept	{ return getData().size(); }
					inline auto empty()		const noexcept	{ return getData().empty(); }

					inline auto begin()		noexcept		{ return std::begin(getData()); }
					inline auto end()		noexcept		{ return std::end(getData()); }
					inline auto begin()		const noexcept	{ return std::begin(getData()); }
					inline auto end()		const noexcept	{ return std::end(getData()); }
					inline auto cbegin()	const noexcept	{ return std::cbegin(getData()); }
					inline auto cend()		const noexcept	{ return std::cend(getData()); }
					inline auto rbegin()	noexcept		{ return std::rbegin(getData()); }
					inline auto rend()		noexcept		{ return std::rend(getData()); }
					inline auto crbegin()	const noexcept	{ return std::crbegin(getData()); }
					inline auto crend()		const noexcept	{ return std::crend(getData()); }
			};

			template<typename TK, typename TV> class VecMap : public VecBase<VecMap<TK, TV>>
			{
				template<typename TDerived> friend class VecBase;

				public:
					using Item = std::pair<TK, TV>;

				private:
					std::vector<Item> data;

					template<typename T> inline static auto lookupHelper(T& mVecMap, const TK& mKey) noexcept
					{
						return lowerBound(mVecMap.data, mKey, [](const auto& mA, const auto& mB){ return mA.first < mB; });
					}

					inline auto lookup(const TK& mKey) noexcept			{ return lookupHelper(*this, mKey); }
					inline auto lookup(const TK& mKey) const noexcept	{ return lookupHelper(*this, mKey); }

					template<typename T> inline bool is(const T& mItr, const TK& mKey) const noexcept
					{
						return mItr != std::end(data) && mItr->first == mKey;
					}

				public:
					inline VecMap() = default;
					inline VecMap(const VecMap& mVM) : data{mVM.data} { }
					inline VecMap(VecMap&& mVM) : data{std::move(mVM.data)} { }
					inline VecMap(std::initializer_list<Item>&& mIL) : data{std::move(mIL)}
					{
						sort(data, [](const auto& mA, const auto& mB){ return mA.first < mB.first; });
					}

					template<typename TTK> inline auto& operator[](TTK&& mKey)
					{
						auto itr(lookup(mKey));
						return is(itr, mKey) ? itr->second : data.emplace(itr, fwd<TTK>(mKey), TV{})->second;
					}

					inline const auto& at(const TK& mKey) const
					{
						auto itr(lookup(mKey));
						if(is(itr, mKey)) return itr->second;

						throw std::out_of_range{""};
					}

					inline const auto& atOrDefault(const TK& mKey) const
					{
						auto itr(lookup(mKey));
						if(is(itr, mKey)) return itr->second;
						return TV{};
					}
			};
		}
	}
}

#endif
