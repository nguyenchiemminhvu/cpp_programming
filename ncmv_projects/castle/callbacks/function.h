#pragma once

#include <tuple>
#include <utility>

namespace castle
{
namespace callbacks
{

// Common interface. All callback variants below derive from this so the caller
// can hold a single i_function<Args...>* regardless of how the target is bound.
template <typename... Args>
class i_function
{
public:
    using param_types = std::tuple<Args...>;

    virtual ~i_function() = default;
    virtual void operator()(Args... args) = 0;
};

// -----------------------------------------------------------------------------
// Runtime-bound free / static function
// Free / static function pointer are stored as members => one level
// of indirection per call.
// -----------------------------------------------------------------------------
template <typename... Args>
class function : public i_function<Args...>
{
public:
    using param_types = std::tuple<Args...>;

    function(void (*func)(Args...)) : func_(func) {}

    void operator()(Args... args) override
    {
        (*func_)(std::forward<Args>(args)...);
    }

private:
    void (*func_)(Args...);
};

// -----------------------------------------------------------------------------
// Runtime-bound functor / lambda (owns the callable by value).
// The callable (functor object or lambda closure) is stored as a member,
// so this variant works with lambdas that capture state, std::bind results,
// or any object that provides operator()(Args...).
//
//   int x = 10;
//   auto lam = [x](int a, float b) { /* use x */ };
//   castle::events::function_f<decltype(lam), int, float> cb(std::move(lam));
//   cb(1, 2.0f);
//
// Because the closure type of a lambda is anonymous, prefer the make_
// factory below to deduce it:
//
//   auto cb = castle::events::make_function_f<int, float>(
//       [x](int a, float b) { /* ... */ });
// -----------------------------------------------------------------------------
template <typename Callable, typename... Args>
class function_f : public i_function<Args...>
{
public:
    using callable_type = Callable;
    using param_types   = std::tuple<Args...>;

    // Perfect-forward construction so both lvalue functors and rvalue
    // lambda closures can be stored efficiently.
    template <typename C, typename = std::enable_if_t<!std::is_same_v<std::decay_t<C>, function_f>>>
    explicit function_f(C&& c)
        : callable_(std::forward<C>(c)) {}

    void operator()(Args... args) override
    {
        (void)callable_(std::forward<Args>(args)...);
    }

private:
    Callable callable_;
};

// Deduction helper: caller only needs to spell out the signature (Args...),
// the closure/functor type is deduced.
template <typename... Args, typename Callable>
auto make_function_f(Callable&& c)
{
    return function_f<std::decay_t<Callable>, Args...>(std::forward<Callable>(c));
}

// -----------------------------------------------------------------------------
// Runtime-bound functor by reference (does NOT own the callable).
// Use when the functor is large, non-copyable, or you explicitly want shared
// state. Caller must ensure the referenced functor outlives this callback.
//
//   struct BigFunctor { void operator()(int); /* heavy state */ };
//   BigFunctor f;
//   castle::events::function_fr<BigFunctor, int> cb(f);
//   cb(7);
// -----------------------------------------------------------------------------
template <typename Callable, typename... Args>
class function_fr : public i_function<Args...>
{
public:
    using callable_type = Callable;
    using param_types   = std::tuple<Args...>;

    explicit function_fr(Callable& c) : callable_(&c) {}

    void operator()(Args... args) override
    {
        (void)(*callable_)(std::forward<Args>(args)...);
    }

private:
    Callable* callable_;
};

// -----------------------------------------------------------------------------
// Runtime-bound member function.
// Object pointer and member function pointer are stored as members => one level
// of indirection per call.
//
//   struct Handler { void on_tick(int, float); };
//   Handler h;
//   castle::events::function_m<Handler, int, float> cb(h, &Handler::on_tick);
//   cb(1, 2.0f);
// -----------------------------------------------------------------------------
template <typename ObjType, typename... Args>
class function_m : public i_function<Args...>
{
public:
    using obj_type = ObjType;
    using param_types = std::tuple<Args...>;

    function_m(obj_type& obj, void (obj_type::*func)(Args...))
        : obj_(&obj), func_(func) {}

    void operator()(Args... args) override
    {
        (obj_->*func_)(std::forward<Args>(args)...);
    }

private:
    obj_type* obj_;
    void (obj_type::*func_)(Args...);
};

// -----------------------------------------------------------------------------
// Compile-time-bound free / static function
// Empty object: the call is a direct function call.
//
//   void on_event(int, float);
//   castle::events::function_ct<&on_event> cb;
//   cb(1, 2.0f);
// -----------------------------------------------------------------------------
template <auto Func>
class function_ct;

template <typename... Args, void (*Func)(Args...)>
class function_ct<Func> : public i_function<Args...>
{
public:
    using param_types = std::tuple<Args...>;

    void operator()(Args... args) override
    {
        (void)(*Func)(std::forward<Args>(args)...);
    }
};

// -----------------------------------------------------------------------------
// Compile-time-bound functor (zero storage).
// Useful for stateless lambdas wrapped in a named type, or functors that are
// default-constructible and pure. The callable is instantiated on each call;
// for stateless closures the compiler collapses this to a direct call.
//
//   struct Add { void operator()(int a, int b) { /* ... */ } };
//   castle::events::function_ct_f<Add, int, int> cb;
//   cb(1, 2);
// -----------------------------------------------------------------------------
template <typename Callable, typename... Args>
class function_ct_f : public i_function<Args...>
{
    static_assert(std::is_default_constructible_v<Callable>,
                  "function_ct_f requires a default-constructible callable "
                  "(stateless functor or captureless lambda wrapped in a type).");
public:
    using callable_type = Callable;
    using param_types   = std::tuple<Args...>;

    void operator()(Args... args) override
    {
        Callable{}(std::forward<Args>(args)...);
    }
};

// -----------------------------------------------------------------------------
// Compile-time-bound member function, runtime-bound instance
// The member function pointer is a non-type template parameter so the call is
// devirtualisable/inlinable. Only the object pointer is stored.
//
//   struct Handler { void on_tick(int, float); };
//   Handler h;
//   castle::events::function_ct_m<&Handler::on_tick> cb(h);
//   cb(1, 2.0f);
// -----------------------------------------------------------------------------
template <auto mem_func_>
class function_ct_m;

template <typename ObjType, typename... Args, void (ObjType::*mem_func_)(Args...)>
class function_ct_m<mem_func_> : public i_function<Args...>
{
public:
    using obj_type = ObjType;
    using param_types = std::tuple<Args...>;

    explicit function_ct_m(obj_type& obj) : obj_(&obj) {}

    void operator()(Args... args) override
    {
        (void)(obj_->*mem_func_)(std::forward<Args>(args)...);
    }

private:
    obj_type* obj_;
};

template <typename ObjType, typename... Args, void (ObjType::*mem_func_)(Args...) const>
class function_ct_m<mem_func_> : public i_function<Args...>
{
public:
    using obj_type = ObjType;
    using param_types = std::tuple<Args...>;

    explicit function_ct_m(const obj_type& obj) : obj_(&obj) {}

    void operator()(Args... args) override
    {
        (void)(obj_->*mem_func_)(std::forward<Args>(args)...);
    }

private:
    const obj_type* obj_;
};

// -----------------------------------------------------------------------------
// Compile-time-bound instance AND member function
// Nothing is stored per-callback: the call site is a direct member call.
// Instance must have static storage duration (namespace scope, static, etc.).
//
//   struct Handler { void on_tick(int); };
//   Handler g_handler;
//   castle::events::function_ct_im<g_handler, &Handler::on_tick> cb;
//   cb(42);
// -----------------------------------------------------------------------------
template <auto& Instance, auto mem_func_>
class function_ct_im;

template <typename ObjType, ObjType& Instance,
          typename... Args, void (ObjType::*mem_func_)(Args...)>
class function_ct_im<Instance, mem_func_> : public i_function<Args...>
{
public:
    using obj_type = ObjType;
    using param_types = std::tuple<Args...>;

    void operator()(Args... args) override
    {
        (void)(Instance.*mem_func_)(std::forward<Args>(args)...);
    }
};

template <typename ObjType, ObjType& Instance,
          typename... Args, void (ObjType::*mem_func_)(Args...) const>
class function_ct_im<Instance, mem_func_> : public i_function<Args...>
{
public:
    using obj_type = ObjType;
    using param_types = std::tuple<Args...>;

    void operator()(Args... args) override
    {
        (void)(Instance.*mem_func_)(std::forward<Args>(args)...);
    }
};

} // namespace callbacks
} // namespace castle