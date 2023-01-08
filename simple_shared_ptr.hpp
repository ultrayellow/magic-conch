// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#pragma once

#include <algorithm>

namespace uy
{
    template <typename T>
    class shared_ptr
    {
    private:
        T* ptr;
        long* ctrl;

        template <typename U>
        friend class shared_ptr;

    public:
        shared_ptr() : ptr(), ctrl() {}

        shared_ptr(T* ptr)
            : ptr(ptr), ctrl(new long(0))
        {
            if (this->ctrl)
            {
                ++(*this->ctrl);
            }
        }

        shared_ptr(const shared_ptr& that) throw()
            : ptr(that.ptr), ctrl(that.ctrl)
        {
            if (this->ctrl)
            {
                ++(*this->ctrl);
            }
        }

        template <typename U>
        shared_ptr(const shared_ptr<U>& that) throw()
            : ptr(that.ptr), ctrl(that.ctrl)
        {
            if (this->ctrl)
            {
                ++(*this->ctrl);
            }
        }

        template <typename U>
        shared_ptr(const shared_ptr<U>& that, T* ptr) throw()
            : ptr(ptr), ctrl(that.ctrl)
        {
            if (this->ctrl)
            {
                ++(*this->ctrl);
            }
        }

        ~shared_ptr()
        {
            if (this->ctrl)
            {
                if (--(*this->ctrl) == 0)
                {
                    delete this->ptr;
                    delete this->ctrl;
                }
            }
        }

        shared_ptr& operator=(shared_ptr temp) throw()
        {
            std::swap(this->ptr, temp.ptr);
            std::swap(this->ctrl, temp.ctrl);
            return *this;
        }

        T& operator*() const throw()
        {
            return *this->ptr;
        }

        T* get() const throw()
        {
            return this->ptr;
        }

        T* operator->() const throw()
        {
            return this->ptr;
        }

        operator bool() const throw()
        {
            return this->ptr != NULL;
        }
    };

    template <typename T, typename U>
    bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() == rhs.get();
    }

    template <typename T, typename U>
    bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() != rhs.get();
    }

    template <typename T, typename TSource>
    shared_ptr<T> static_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        return shared_ptr<T>(that, static_cast<typename shared_ptr<T>::element_type*>(that.get()));
    }

    template <typename T, typename TSource>
    shared_ptr<T> const_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        return shared_ptr<T>(that, const_cast<typename shared_ptr<T>::element_type*>(that.get()));
    }

    template <typename T, typename TSource>
    shared_ptr<T> dynamic_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        typename shared_ptr<T>::element_type* p = dynamic_cast<typename shared_ptr<T>::element_type*>(that.get());
        return p ? shared_ptr<T>(that, p) : shared_ptr<T>();
    }

    template <typename T, typename TSource>
    shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        return shared_ptr<T>(that, reinterpret_cast<typename shared_ptr<T>::element_type*>(that.get()));
    }

    template <typename T>
    shared_ptr<T> make_shared()
    {
        return shared_ptr<T>(new T);
    }

    template <typename T, typename A1>
    shared_ptr<T> make_shared(const A1& a1)
    {
        return shared_ptr<T>(new T(a1));
    }

    template <typename T, typename A1, typename A2>
    shared_ptr<T> make_shared(const A1& a1, const A2& a2)
    {
        return shared_ptr<T>(new T(a1, a2));
    }

    template <typename T, typename A1, typename A2, typename A3>
    shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3)
    {
        return shared_ptr<T>(new T(a1, a2, a3));
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4>
    shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
    {
        return shared_ptr<T>(new T(a1, a2, a3, a4));
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
    shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
    {
        return shared_ptr<T>(new T(a1, a2, a3, a4, a5));
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
    {
        return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6));
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
    {
        return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7));
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
    {
        return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8));
    }

    template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
    {
        return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9));
    }
}
