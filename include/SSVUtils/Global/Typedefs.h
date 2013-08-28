// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVU_GLOBAL_TYPEDEFS
#define SSVU_GLOBAL_TYPEDEFS

#include <memory>
#include <functional>

namespace ssvu
{
	template<typename T, typename TDeleter = std::default_delete<T>> using Uptr = std::unique_ptr<T, TDeleter>;
	using Action = std::function<void()>;
	using Predicate = std::function<bool()>;
}

#endif
