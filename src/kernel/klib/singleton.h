#ifndef SINGLETON_H
#define SINGLETON_H

namespace xenon
{
    template <typename T>
    class singleton
    {
    public:
        static T &instance()
        {
            static T instance;
            return instance;
        }

        ~singleton() = delete;
        singleton(const singleton&) = delete;
        singleton(singleton &&) = delete;
        singleton &operator=(const singleton&) = delete;
        singleton &&operator=(singleton&&) = delete;

    private:
        singleton()
        {
        }
    };
}

#endif // SINGLETON_H
