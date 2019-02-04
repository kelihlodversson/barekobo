#pragma once

namespace hfh3
{
    /** A partial replacement of std::function<...> as we don't have access to the STL 
      * on a barebone environment.
      * This implementation is a simplified version of the GCC standard library's 
      * implementation of std::function.
      * (https://gcc.gnu.org/svn/gcc/trunk/libstdc++-v3/include/std/functional?p=268513)
      */
    template<typename>
    class Callback;

    /** The actual implementation is specified as a partial template specialization, so
      * callbacks can be declared as Callback<return_type(arg1_type, arg2_type, ... etc)>
      */
    template<typename Ret, typename ... Args>
    class Callback<Ret(Args...)>
    {
        public:

        Callback() 
            : wrapped(nullptr)
        {
        }

        template<typename Functor>
        Callback(Functor functor) 
            : wrapped(new ConcreteWrapper<Functor>(functor))
        {
        }

        ~Callback()
        {
            if(wrapped)
            {
                delete wrapped;
            }
        }

        template<typename Functor>
        Callback& operator=(Functor functor)
        {
            if(wrapped)
            {
                delete wrapped;
            }
            wrapped = new ConcreteWrapper<Functor>(functor);
            return *this;
        }

        Ret operator()(Args... args)
        {
            return (*wrapped)(args...);
        }

        operator bool() const 
        {
            return wrapped != nullptr;
        }

        private:

        struct AbstractWrapper 
        {
            virtual Ret operator()(Args... args);
            virtual ~AbstractWrapper() {}
        };

        template<typename Functor>
        struct ConcreteWrapper : AbstractWrapper
        {
            ConcreteWrapper(Functor inFunctor) 
                : functor(inFunctor)
            {}

            virtual Ret operator()(Args... args) override
            {
                return functor(args...);
            }

            Functor functor;
        };

        AbstractWrapper* wrapped;
    };
}