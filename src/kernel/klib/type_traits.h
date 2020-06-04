#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

template <typename T>
struct remove_reference
{
    typedef T type;
};

template <typename T>
struct remove_reference<T&>
{
    typedef T type;
};

template <typename T>
struct remove_reference<T&&>
{
    typedef T type;
};

template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T>
{
    typedef T type;
};

template<bool B, class T = void>
using enable_if_t = typename enable_if<B, T>::type;

template <class T, T v>
struct integral_constant
{
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant;
    constexpr operator value_type() const noexcept
    {
        return value;
    }

    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};

using false_type = integral_constant<bool, false>;
using true_type = integral_constant<bool, true>;

template<class T, class U>
struct is_same : false_type {};

template<class T>
struct is_same<T, T> : true_type {};

// explanation: https://en.wikibooks.org/wiki/More_C++_Idioms/Member_Detector
#define reflection_has_member(member)                                                               \
    template <class K>                                                                              \
    class __HAS_MEMBER__##member                                                                    \
    {                                                                                               \
    private:                                                                                        \
        using yes = char[2];                                                                        \
        using no  = char[1];                                                                        \
                                                                                                    \
        struct fallback { int member; };                                                            \
        struct derived : K, fallback {};                                                            \
                                                                                                    \
        template <class U>                                                                          \
        static no &check(decltype(U::member)*);                                                     \
                                                                                                    \
        template <class U>                                                                          \
        static yes &check(U*);                                                                      \
                                                                                                    \
    public:                                                                                         \
        static constexpr bool RESULT = (sizeof(check<derived>(nullptr)) == sizeof(yes));            \
    };                                                                                              \
                                                                                                    \
    template <class K>                                                                              \
    struct has_member_##member : public integral_constant<bool, __HAS_MEMBER__##member<K>::RESULT>  \
    {                                                                                               \
    };




#endif // TYPE_TRAITS_H
