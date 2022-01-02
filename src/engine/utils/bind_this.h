#ifndef BIND_THIS_H_

#include <functional>

template <class R, class F, class T, typename... Args>
struct binded_this {
    F f;
    T& self;

    R operator()(Args... args) { return (self.*f)(args...); }
};

template <class F>
struct bind_this_helper_type {};

template <class R, class T, typename... Args>
struct bind_this_helper_type<R (T::*)(Args...)> {
    using Self = T;
    using Func = R (T::*)(Args...);
    using Ret = R;
    using binded = binded_this<Ret, Func, Self, Args...>;
};

template <class R, class T, typename... Args>
struct bind_this_helper_type<R (T::*)(Args...) const> {
    using Self = const T;
    using Func = R (T::*)(Args...) const;
    using Ret = R;
    using binded = binded_this<Ret, Func, Self, Args...>;
};

template <typename T>
class binder {
   public:
    using type = T;
    template <typename R, typename... Args>
    using member_fct = R (T::*)(Args...);

    template <typename R, typename... Args>
    using member_fct_c = R (T::*)(Args...) const;

    binder(T& self) : self(self) {}

    binder(binder&) = delete;

    template <typename R, typename... Args>
    auto bind(member_fct<R, Args...> f) {
        return typename bind_this_helper_type<member_fct<R, Args...>>::binded{
            f, self};
    }

    template <typename R, typename... Args>
    auto bind(member_fct_c<R, Args...> f) {
        return typename bind_this_helper_type<member_fct_c<R, Args...>>::binded{
            f, self};
    }

   private:
    T& self;
};

// Care! Does not works with overloaded member functions!
// For overloaded functions, use
// whether it's a ptr to a const or non const member fct
//  binder(self).bind<ret, args...>(&ptr_to_member_fct)
// or if its a ptr to a non-const member fct
//  bind_this<ret (class_name::*)(args...)>(&ptr_to_member_fct, self)
// or if its a ptr to a const member fct
//  bind_this<ret (class_name::*)(args...) const>(&ptr_to_member_fct, self)
template <class F>
auto bind_this(typename bind_this_helper_type<F>::Self& self, F f) {
    return binder(self).bind(f);
};

#endif  // !BIND_THIS_H_
