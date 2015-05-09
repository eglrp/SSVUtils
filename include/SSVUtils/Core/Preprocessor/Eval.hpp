// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVU_CORE_PREPROCESSOR_EVAL
#define SSVU_CORE_PREPROCESSOR_EVAL

// TODO: generate, docs
#define SSVPP_IMPL_EVAL_5(...) __VA_ARGS__
#define SSVPP_IMPL_EVAL_4(...) SSVPP_IMPL_EVAL_5(SSVPP_IMPL_EVAL_5(SSVPP_IMPL_EVAL_5(__VA_ARGS__)))
#define SSVPP_IMPL_EVAL_3(...) SSVPP_IMPL_EVAL_4(SSVPP_IMPL_EVAL_4(SSVPP_IMPL_EVAL_4(__VA_ARGS__)))
#define SSVPP_IMPL_EVAL_2(...) SSVPP_IMPL_EVAL_3(SSVPP_IMPL_EVAL_3(SSVPP_IMPL_EVAL_3(__VA_ARGS__)))
#define SSVPP_IMPL_EVAL_1(...) SSVPP_IMPL_EVAL_2(SSVPP_IMPL_EVAL_2(SSVPP_IMPL_EVAL_2(__VA_ARGS__)))
#define SSVPP_IMPL_EVAL_0(...) SSVPP_IMPL_EVAL_1(SSVPP_IMPL_EVAL_1(SSVPP_IMPL_EVAL_1(__VA_ARGS__)))

#define SSVPP_IMPL_EVAL(...) SSVPP_IMPL_EVAL_0(__VA_ARGS__)
#define SSVPP_EVAL(...) SSVPP_IMPL_EVAL(__VA_ARGS__)

#endif