#pragma once

#include <type_traits>
#include <tuple>
#include <cassert>

struct any {
template<typename T>
constexpr operator T() { return {}; };
};

template<int, typename...> struct restart_cases_s;

template<int N, typename...Handlers, typename Case, typename...Cases>
struct restart_cases_s<N,   std::tuple<Handlers...>, std::tuple<Case, Cases...>> :
       restart_cases_s<N+1, std::tuple<Handlers...>, std::tuple<Cases...>>
{
    using next = restart_cases_s <N+1, std::tuple<Handlers...>, std::tuple<Cases...>>;

    const decltype(std::get<N>(*((std::tuple<Handlers...>*)(nullptr))))&& handler;

    template<typename... Args>
    auto operator()(const Case&, Args&&...args)
         -> decltype(handler(std::forward<Args>(args)...))
    {
        return handler(std::forward<Args>(args)...);
    }

    // todo disable instantiation when rest = {}
    // forward call to operator() to next class
    template<typename FwdCase, typename... Args>
    auto operator()(const FwdCase& c, Args&&...args)
         -> decltype(next{std::move(*((std::tuple<Handlers...>*)(nullptr)))}.template operator()(c, std::forward<Args>(args)...))
    {
        return next::template operator()(c, std::forward<Args>(args)...);
    }

    restart_cases_s(std::tuple<Handlers...>&& handlers)
      : next(std::forward<std::tuple<Handlers...>>(handlers))
      , handler(std::get<N>(handlers))
    { }
};

template<int N, typename...Handlers>
struct restart_cases_s<N, std::tuple<Handlers...>, std::tuple<any>>
{
    const decltype(std::get<N>(*((std::tuple<Handlers...>*)(nullptr))))&& handler;

    template<typename AnyCase, typename... Args>
    any operator()(const AnyCase& c, Args&&...args) {
        handler(std::forward<Args>(args)...);
        return {};
    }

    restart_cases_s(std::tuple<Handlers...>&& handlers)
      : handler(std::get<N>(handlers))
    { }
};

template<int N, typename...Handlers>
struct restart_cases_s<N, std::tuple<Handlers...>, std::tuple<>> {

    template<typename UnhandledCase, typename... Args>
    any operator()(const UnhandledCase&, Args&&...) {
        assert(false);
        return {};
    }

    restart_cases_s(std::tuple<Handlers...>&&) { }
};

template<int, typename...> struct extract_handlers;

template<int N, typename Case, typename Handler, typename...Cases, typename ...Handlers, typename...CasesHandlers, typename...Rest>
struct extract_handlers<N,   std::tuple<Handlers...>,          std::tuple<Cases...>,       std::tuple<CasesHandlers...>, std::tuple<std::tuple<Case,Handler>, Rest...>> :
       extract_handlers<N+1, std::tuple<Handlers..., Handler>, std::tuple<Cases..., Case>, std::tuple<CasesHandlers...>, std::tuple<Rest...>>
{
    using next = extract_handlers<N+1, std::tuple<Handlers..., Handler>, std::tuple<Cases..., Case>, std::tuple<CasesHandlers...>, std::tuple<Rest...>>;

    extract_handlers(std::tuple<CasesHandlers...>&& ch, Handlers&&... hs)
      : next(std::forward<std::tuple<CasesHandlers...>>(ch),
             std::forward<Handlers>(hs)...,
             std::get<1>(std::get<N>(std::forward<std::tuple<CasesHandlers...>>(ch))))
    { }
};

template<int N, typename...Cases, typename ...Handlers, typename...CasesHandlers>
struct extract_handlers<N, std::tuple<Handlers...>, std::tuple<Cases...>, std::tuple<CasesHandlers...>, std::tuple<>> {
    std::tuple<Handlers...> handlers;
    using handlers_t = std::tuple<Handlers...>;
    using cases_t = std::tuple<Cases...>;
    extract_handlers(std::tuple<CasesHandlers...>&& ch, Handlers&&... hs)
      : handlers(std::forward<Handlers>(hs)...)
    { }
};

template<typename...CasesHandlers>
struct extract :
       extract_handlers<0, std::tuple<>, std::tuple<>, std::tuple<CasesHandlers...>, std::tuple<CasesHandlers...>>
{
    using e = extract_handlers<0, std::tuple<>, std::tuple<>, std::tuple<CasesHandlers...>, std::tuple<CasesHandlers...>>;
    extract(CasesHandlers&&... ch) : e(std::tuple{std::forward<CasesHandlers>(ch)...}) {};
};

template<typename... CasesHandlers>
struct restart_cases : restart_cases_s<0,
            typename extract<CasesHandlers...>::handlers_t,
            typename extract<CasesHandlers...>::cases_t>
{
    using extractor = extract<CasesHandlers...>;
    using handlers_t = typename extractor::handlers_t;
    using cases_t = typename extractor::cases_t;

    using base = restart_cases_s<0, handlers_t, cases_t>;

    restart_cases(CasesHandlers&&...args)
        : base{extractor{std::forward<CasesHandlers>(args)...}.handlers}
    { }

    template<typename Case, typename... CaseArgs>
    auto operator()(const Case& c, CaseArgs&&...args)
         -> decltype(((base*)(nullptr))->template operator()(c, std::forward<CaseArgs>(args)...))
    {
        return base::template operator()(c, std::forward<CaseArgs>(args)...);
    }
};

template<typename... CasesHandlers>
struct restart_cases<CasesHandlers...>
restart_case(CasesHandlers&&...args)
{
    return restart_cases{std::forward<CasesHandlers>(args)...};
}