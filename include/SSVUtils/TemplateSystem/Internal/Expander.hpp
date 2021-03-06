
// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVU_TEMPLATESYSTEM_EXPANDER
#define SSVU_TEMPLATESYSTEM_EXPANDER

#include "SSVUtils/Core/Core.hpp"
#include "SSVUtils/Container/Container.hpp"

#include <string>

namespace ssvu
{
namespace TemplateSystem
{
enum class Settings
{
    EraseUnexisting,          // Do not write empty/unexisting
                              // replacements/sections
    MaintainUnexisting,       // Write empty/unexisting replacements/sections
                              // (both symbols and contents)
    MaintainUnexistingOnlyCnt // Write only empty/unexisting sections
                              // contents once (not symbols)
};

class Dictionary;

class Expander
{
private:
    const Dictionary& dict;
    const std::string& src;
    std::string& bufResult;
    std::string& bufKey;
    const std::size_t idxBegin, idxEnd;
    std::size_t idx, sectIdxCntStart, sectIdxCntEnd, sectIdxStart, sectIdxEnd;
    bool separate;
    Settings settings;

    inline auto getC(std::size_t mIdx) const noexcept
    {
        SSVU_ASSERT(mIdx < src.size());
        return src[mIdx];
    }
    inline auto getC() const noexcept
    {
        return getC(idx);
    }

    bool replace();
    bool replaceSection();

public:
    inline Expander(const Dictionary& mDict, const std::string& mSrc,
        std::string& mBufResult, std::string& mBufKey, std::size_t mIdxBegin,
        std::size_t mIdxEnd, bool mSeparate, Settings mSettings)
        : dict{mDict}, src{mSrc}, bufResult{mBufResult}, bufKey{mBufKey},
          idxBegin{mIdxBegin}, idxEnd{mIdxEnd}, idx{idxBegin},
          separate{mSeparate}, settings{mSettings}
    {
    }

    bool expand();
};
} // namespace TemplateSystem
} // namespace ssvu

#endif
