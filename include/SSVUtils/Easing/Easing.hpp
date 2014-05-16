// Copyright (mC) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// Adapted from: http://www.robertpenner.com/easing/

#ifndef SSVU_EASING
#define SSVU_EASING

namespace ssvu
{
	namespace Easing
	{
		namespace Internal
		{
			template<typename T> struct Dispatcher
			{
				using FnType = T(*)(T, T, T, T);
				template<FnType FN> inline static T getMap(const T& mI, const T& mIMin, const T& mIMax, const T& mOMin, const T& mOMax) noexcept
				{
					return FN(mIMin + mI, mOMin, mOMax - mOMin, mIMax - mIMin);
				}
			};
		}

		template<typename T> struct Linear
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return mC * mT / mD + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept { return in(mT, mB, mC, mD); }
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept { return in(mT, mB, mC, mD); }
		};

		template<typename T> struct Sine
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return -mC * std::cos(mT / mD * (ssvu::pi / T(2))) + mC + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return mC * std::sin(mT / mD * (ssvu::pi / T(2))) + mB;
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return -mC / T(2) * (std::cos(ssvu::pi * mT / mD) - T(1)) + mB;
			}
		};

		template<typename T> struct Back
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				T postFix(mT /= mD), s(1.70158);
				return mC * (postFix) * mT * ((s + T(1)) * mT - s) + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				T s(1.70158);
				return mC * ((mT = mT / mD - T(1)) * mT * ((s + T(1)) * mT + s) + T(1)) + mB;
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				T s(1.70158);
				if((mT /= mD / T(2)) < T(1)) return mC / T(2) * (mT * mT * (((s *= T(1.525)) + T(1)) * mT - s)) + mB;
				T postFix(mT -= 2);
				return mC / T(2) * ((postFix) * mT * (((s *= T(1.525)) + 1) * mT + s) + T(2)) + mB;
			}
		};

		template<typename T> struct Cubic
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return mC * (mT /= mD) * mT * mT + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return mC * ((mT = mT / mD - T(1)) * mT * mT + T(1)) + mB;
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if((mT /= mD / T(2)) < T(1)) return mC / T(2) * mT * mT * mT + mB;
				auto p1(mC / T(2) * mT);
				mT -= T(2);
				return p1 * mT * mT + T(2) + mB;
			}
		};

		template<typename T> struct Bounce
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept { return mC - out(mD - mT, T(0), mC, mD) + mB; }
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if((mT /= mD) < (T(1) / T(2.75))) return mC * (T(7.5625) * mT * mT) + mB;
				if(mT < (T(2) / T(2.75)))
				{
					T postFix(mT -= (T(1.5) / T(2.75)));
					return mC * (T(7.5625) * postFix * mT + T(0.75)) + mB;
				}
				if(mT < (T(2.5) / T(2.75)))
				{
					T postFix(mT -= (T(2.25) / T(2.75)));
					return mC * (T(7.5625) * postFix * mT + T(0.9375)) + mB;
				}

				T postFix(mT -= (T(2.625) / T(2.75)));
				return mC * (T(7.5625) * postFix * mT + T(0.984375)) + mB;
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				if(mT < mD / T(2)) return in(mT * T(2), T(0), mC, mD) * T(0.5) + mB;
				return out(mT * T(2) - mD, T(0), mC, mD) * T(0.5) + mC * T(0.5) + mB;
			}
		};

		template<typename T> struct Circ
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return -mC * (std::sqrt(T(1) - (mT /= mD) * mT) - T(1)) + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return mC * std::sqrt(T(1) - (mT = mT / mD - T(1)) * mT) + mB;
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if((mT /= mD / T(2)) < T(1)) return -mC / T(2) * (std::sqrt(T(1) - mT * mT) - T(1)) + mB;
				return mC / T(2) * (std::sqrt(T(1) - mT * (mT -= T(2))) + T(1)) + mB;
			}
		};

		template<typename T> struct Elastic
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if(mT == T(0)) return mB;
				if((mT /= mD) == T(1)) return mB + mC;

				T p(mD * T(0.3)), a(mC), s(p / T(4)), postFix(a * std::pow(T(2), T(10) * (mT -= T(1))));
				SSVU_ASSERT(p != 0);

				return -(postFix * std::sin((mT * mD - s) * (T(2) * pi) / p)) + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if(mT == T(0)) return mB;
				if((mT /= mD) == T(1)) return mB + mC;

				T p(mD * T(0.3)), a(mC), s(p / T(4));
				SSVU_ASSERT(p != 0);

				return (a * std::pow(T(2), T(-10) * mT) * std::sin((mT * mD - s) * (T(2) * pi) / p) + mC + mB);
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if(mT == T(0)) return mB;
				if((mT /= mD / T(2)) == T(2)) return mB + mC;

				T p(mD * T(0.3 * 1.5)), a(mC), s(p / T(4));
				SSVU_ASSERT(p != 0);

				if(mT < 1)
				{
					T postFix(a * std::pow(T(2), T(10) * (mT -= T(1))));
					return -T(0.5) * (postFix * std::sin((mT * mD - s) * (T(2) * pi) / p)) + mB;
				}

				T postFix(a * std::pow(T(2), T(-10) * (mT -= T(1))));
				return postFix * std::sin((mT * mD - s) * (T(2) * pi) / p) * T(0.5) + mC + mB;
			}
		};

		template<typename T> struct Expo
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return (mT == T(0)) ? mB : mC * std::pow(T(2), T(10) * (mT / mD - T(1))) + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return (mT == mD) ? mB + mC : mC * (-std::pow(T(2), T(-10) * mT / mD) + T(1)) + mB;
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if(mT == T(0)) return mB;
				if(mT == mD) return mB + mC;
				if((mT /= mD / T(2)) < T(1)) return mC / T(2) * std::pow(T(2), T(10) * (mT - T(1))) + mB;
				return mC / T(2) * (-std::pow(T(2), T(-10) * --mT) + T(2)) + mB;
			}
		};

		template<typename T> struct Quad
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return mC * (mT /= mD) * mT + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return -mC *(mT /= mD) * (mT - T(2)) + mB;
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if((mT /= mD / T(2)) < T(1)) return ((mC / T(2)) * (mT * mT)) + mB;
				return -mC / T(2) * (((mT - T(2)) * (--mT)) - T(1)) + mB;
			}
		};

		template<typename T> struct Quart
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return mC * (mT /= mD) * mT * mT * mT + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return -mC * ((mT = mT / mD - T(1)) * mT * mT * mT - T(1)) + mB;
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if((mT /= mD / T(2)) < T(1)) return mC / T(2) * mT * mT * mT * mT + mB;
				return -mC / T(2) * ((mT -= T(2)) * mT * mT * mT - T(2)) + mB;
			}
		};

		template<typename T> struct Quint
		{
			inline static T in(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return mC * (mT /= mD) * mT * mT * mT * mT + mB;
			}
			inline static T out(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				return mC * ((mT = mT / mD - T(1)) * mT * mT * mT * mT + T(1)) + mB;
			}
			inline static T inOut(T mT, T mB, T mC, T mD) noexcept
			{
				SSVU_ASSERT(mD != 0);
				if((mT /= mD / T(2)) < T(1)) return mC / T(2) * mT * mT * mT * mT * mT + mB;
				return mC / T(2) * ((mT -= T(2)) * mT * mT * mT * mT + T(2)) + mB;
			}
		};
	}

	template<template<typename> class TEase, typename T1, typename T2, typename T3>
		inline Common<T1, T2, T3> getEasedIn(const T1& mI, const T2& mIMin, const T3& mIMax) noexcept
	{
		using CT = Common<T1, T2, T3>;
		return Easing::Internal::Dispatcher<CT>::template getMap<&TEase<CT>::in>(mI, mIMin, mIMax, mIMin, mIMax);
	}
	template<template<typename> class TEase, typename T1, typename T2, typename T3>
		inline Common<T1, T2, T3> getEasedOut(const T1& mI, const T2& mIMin, const T3& mIMax) noexcept
	{
		using CT = Common<T1, T2, T3>;
		return Easing::Internal::Dispatcher<CT>::template getMap<&TEase<CT>::out>(mI, mIMin, mIMax, mIMin, mIMax);
	}
	template<template<typename> class TEase, typename T1, typename T2, typename T3>
		inline Common<T1, T2, T3> getEasedInOut(const T1& mI, const T2& mIMin, const T3& mIMax) noexcept
	{
		using CT = Common<T1, T2, T3>;
		return Easing::Internal::Dispatcher<CT>::template getMap<&TEase<CT>::inOut>(mI, mIMin, mIMax, mIMin, mIMax);
	}

	template<template<typename> class TEase, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline Common<T1, T2, T3, T4, T5> getMapEasedIn(const T1& mI, const T2& mIMin, const T3& mIMax, const T4& mOMin, const T5& mOMax) noexcept
	{
		using CT = Common<T1, T2, T3, T4, T5>;
		return Easing::Internal::Dispatcher<CT>::template getMap<&TEase<CT>::in>(mI, mIMin, mIMax, mOMin, mOMax);
	}
	template<template<typename> class TEase, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline Common<T1, T2, T3, T4, T5> getMapEasedOut(const T1& mI, const T2& mIMin, const T3& mIMax, const T4& mOMin, const T5& mOMax) noexcept
	{
		using CT = Common<T1, T2, T3, T4, T5>;
		return Easing::Internal::Dispatcher<CT>::template getMap<&TEase<CT>::out>(mI, mIMin, mIMax, mOMin, mOMax);
	}
	template<template<typename> class TEase, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline Common<T1, T2, T3, T4, T5> getMapEasedInOut(const T1& mI, const T2& mIMin, const T3& mIMax, const T4& mOMin, const T5& mOMax) noexcept
	{
		using CT = Common<T1, T2, T3, T4, T5>;
		return Easing::Internal::Dispatcher<CT>::template getMap<&TEase<CT>::inOut>(mI, mIMin, mIMax, mOMin, mOMax);
	}
}

#endif

// TODO: docs, tests
