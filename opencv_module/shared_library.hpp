#pragma once

#ifndef __OPENCV_UTILITY_HPP__
#define __OPENCV_UTILITY_HPP__

#include <stdexcept>
#include <string>
#include <vector>

#include "platform.h"

#if (OPENCV_OS == OPENCV_OS_WINDOWS_NT)
    #ifdef OPENCV_API_EXPORT
        #define OPENCV_API __declspec(dllexport)
    #else
        #define OPENCV_API __declspec(dllimport)
    #endif
#else
    #define OPENCV_API
#endif

namespace cv
{
    template <class M>
    class ScopedLock
    {
    public:
        inline explicit ScopedLock(M& mutex) : mutex_(mutex)
        {
            mutex_.lock();
        }

        inline ~ScopedLock()
        {
            mutex_.unlock();
        }

    private:
        M& mutex_;

        ScopedLock();
        ScopedLock(const ScopedLock&);
        ScopedLock& operator = (const ScopedLock&);
    };

    class OPENCV_API Mutex
    {
    public:
        typedef cv::ScopedLock<Mutex> ScopedLock;

        Mutex();
        ~Mutex();

        void lock();
        bool tryLock();
        void unlock();

    private:
        Mutex(const Mutex&);
        Mutex& operator = (const Mutex&);

        class Impl;
        Impl* impl_;
    };

    class OPENCV_API AtomicCounter
    {
    public:
        typedef int ValueType; /// The underlying integer type.

        AtomicCounter();
        explicit AtomicCounter(ValueType initialValue);
        AtomicCounter(const AtomicCounter& counter);
        ~AtomicCounter();

        AtomicCounter& operator = (const AtomicCounter& counter);
        AtomicCounter& operator = (ValueType value);

        ValueType value() const;

        ValueType operator ++ ();    // prefix
        ValueType operator ++ (int); // postfix

        ValueType operator -- ();    // prefix
        ValueType operator -- (int); // postfix

        bool operator ! () const;

    private:
    #if defined(OPENCV_OS_FAMILY_WINDOWS)
        typedef volatile long ImplType;
    #elif (OPENCV_OS == OPENCV_OS_MAC_OS_X)
        typedef int32_t ImplType;
    #elif defined(OPENCV_HAVE_GCC_ATOMICS)
        typedef int ImplType;
    #else
        // generic implementation based on Mutex
        struct ImplType
        {
            mutable Mutex mutex;
            volatile int  value;
        };
    #endif

        ImplType counter_;
    };

    class OPENCV_API RefCountedObject
    {
    public:
        RefCountedObject();

        void duplicate();
        void release();

        int referenceCount() const;

    protected:
        virtual ~RefCountedObject();
        virtual void deleteImpl();

    private:
        RefCountedObject(const RefCountedObject&);
        RefCountedObject& operator = (const RefCountedObject&);

        mutable AtomicCounter counter_;
    };

    template <class C>
    class AutoPtr
    {
    public:
        inline AutoPtr() : ptr_(0)
        {
        }

        inline explicit AutoPtr(C* ptr) : ptr_(ptr)
        {
        }

        inline AutoPtr(C* ptr, bool shared) : ptr_(ptr)
        {
            if (shared && ptr_) ptr_->duplicate();
        }

        inline AutoPtr(const AutoPtr& ptr) : ptr_(ptr.ptr_)
        {
            if (ptr_) ptr_->duplicate();
        }

        template <class Other>
        inline AutoPtr(const AutoPtr<Other>& ptr) : ptr_(const_cast<Other*>(ptr.get()))
        {
            if (ptr_) ptr_->duplicate();
        }

        inline ~AutoPtr()
        {
            if (ptr_) ptr_->release();
        }

        inline AutoPtr& assign(C* ptr)
        {
            if (ptr_ != ptr)
            {
                if (ptr_) ptr_->release();
                ptr_ = ptr;
            }
            return *this;
        }

        inline AutoPtr& assign(C* ptr, bool shared)
        {
            if (ptr_ != ptr)
            {
                if (ptr_) ptr_->release();
                ptr_ = ptr;
                if (shared && ptr_) ptr_->duplicate();
            }
            return *this;
        }

        inline AutoPtr& assign(const AutoPtr& ptr)
        {
            if (&ptr != this)
            {
                if (ptr_) ptr_->release();
                ptr_ = ptr.ptr_;
                if (ptr_) ptr_->duplicate();
            }
            return *this;
        }

        template <class Other>
        inline AutoPtr& assign(const AutoPtr<Other>& ptr)
        {
            if (ptr.get() != ptr_)
            {
                if (ptr_) ptr_->release();
                ptr_ = const_cast<Other*>(ptr.get());
                if (ptr_) ptr_->duplicate();
            }
            return *this;
        }

        inline AutoPtr& operator = (const AutoPtr& ptr)
        {
            return assign(ptr);
        }

        template <class Other>
        inline AutoPtr& operator = (const AutoPtr<Other>& ptr)
        {
            return assign<Other>(ptr);
        }

        inline void swap(AutoPtr& ptr)
        {
            std::swap(ptr_, ptr.ptr_);
        }

        template <class Other>
        inline AutoPtr<Other> cast() const
        {
            Other* pOther = dynamic_cast<Other*>(ptr_);
            return AutoPtr<Other>(pOther, true);
        }

        template <class Other>
        inline AutoPtr<Other> unsafeCast() const
        {
            Other* pOther = static_cast<Other*>(ptr_);
            return AutoPtr<Other>(pOther, true);
        }

        inline C* operator -> ()
        {
            if (ptr_)
                return ptr_;
            else
                throw std::runtime_error("Null Pointer");
        }

        inline const C* operator -> () const
        {
            if (ptr_)
                return ptr_;
            else
                throw std::runtime_error("Null Pointer");
        }

        inline C& operator * ()
        {
            if (ptr_)
                return *ptr_;
            else
                throw std::runtime_error("Null Pointer");
        }

        inline const C& operator * () const
        {
            if (ptr_)
                return *ptr_;
            else
                throw std::runtime_error("Null Pointer");
        }

        inline C* get()
        {
            return ptr_;
        }

        inline const C* get() const
        {
            return ptr_;
        }

        inline bool operator ! () const
        {
            return ptr_ == 0;
        }

        inline bool isNull() const
        {
            return ptr_ == 0;
        }

        inline C* duplicate()
        {
            if (ptr_) ptr_->duplicate();
            return ptr_;
        }

    private:
        C* ptr_;
    };

    template <class C>
    inline void swap(AutoPtr<C>& p1, AutoPtr<C>& p2)
    {
        p1.swap(p2);
    }

    template <class S>
    class SingletonHolder
    {
    public:
        inline SingletonHolder() : pS_(0)
        {
        }

        inline ~SingletonHolder()
        {
            delete pS_;
        }

        inline S* get()
        {
            Mutex::ScopedLock lock(m_);
            if (!pS_)
                pS_ = new S;
            return pS_;
        }

    private:
        S* pS_;
        Mutex m_;
    };

    class OPENCV_API SharedLibrary
    {
    public:
        SharedLibrary();
        SharedLibrary(const std::string& path);
        ~SharedLibrary();

        void load(const std::string& path);
        void unload();
        bool isLoaded() const;

        bool hasSymbol(const std::string& name);
        void* getSymbol(const std::string& name);

        static std::string suffix();

    private:
        SharedLibrary(const SharedLibrary&);
        SharedLibrary& operator = (const SharedLibrary&);

        class Impl;
        Impl* impl_;
    };

    class OPENCV_API RegExpr
    {
    public:
        static bool compare(const std::string& str, const std::string& pattern);
    };

    class OPENCV_API Path
    {
    public:
         static void glob(const std::string& pattern, std::vector<std::string>& result, bool recursive = false);
    };

    class OPENCV_API Environment
    {
    public:
        static bool has(const std::string& name);
        static std::string get(const std::string& name);
        static std::string get(const std::string& name, const std::string& defaultValue);
    };
}

#endif // __OPENCV_UTILITY_HPP__
