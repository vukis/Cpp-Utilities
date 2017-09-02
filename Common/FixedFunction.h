#pragma once

#include <utility>

/** The FixedFunction<RetValueType(Args...), StorageSize> class implements functional object.
* This function is analog of 'std::function' with limited capabilities:
* - It supports only move semantics.
* - The size of functional objects is limited to storage size (can be perferctly fitted in CPU cache line).
* Due to limitations above it is much faster on creation and copying than std::function.
*/
template <typename FuncSignarture, size_t StorageSize = 64>
class FixedFunction;

template <typename RetValueType, typename... Args, size_t StorageSize>
class FixedFunction<RetValueType(Args...), StorageSize> {

    using FuncPtrType = RetValueType(*)(Args...);

public:
    FixedFunction()
        : m_fuctionPtr{ nullptr }
        , m_executor{ nullptr }
        , m_allocator{ nullptr }
    {}

    /**
    * FixedFunction Constructor from functional object.
    * Parameter 'object' functor object will be stored in the internal storage
    * Using move constructor. Unmovable objects are prohibited explicitly.
    */
    template <typename FuncObjT>
    FixedFunction(FuncObjT&& object) : FixedFunction()
    {
        using UnrefType = std::remove_reference_t<FuncObjT>;

        static_assert(sizeof(UnrefType) < StorageSize,         "Functional object doesn't fit into internal storage");
        static_assert(std::is_move_constructible_v<UnrefType>, "Functional object should be of movable type");

        m_executor = [](void* functionObj, FuncPtrType, Args... args) -> RetValueType {
            return (*static_cast<UnrefType*>(functionObj))(std::forward<Args>(args)...);
        };

        m_allocator = [](void* functionObStorage, void* functionObjSrc) {
            if (functionObjSrc)
            {
                UnrefType* object = static_cast<UnrefType*>(functionObjSrc);
                new (functionObStorage) UnrefType(std::move(*object));
            }
            else 
            {
                static_cast<UnrefType*>(functionObStorage)->~UnrefType();
            }
        };

        m_allocator(&m_functionObjStorage, &object);
    }

    /**
    * FixedFunction constructor from free function or static member.
    */
    template <typename FreeFuncRetValueType, typename... FreeFuncArgs>
    FixedFunction(FreeFuncRetValueType(*functionPtr)(FreeFuncArgs...)) : FixedFunction()
    {
        m_fuctionPtr = functionPtr;
        m_executor = [](void*, FuncPtrType functionPtr, Args... args) -> RetValueType {
            return static_cast<FreeFuncRetValueType(*)(FreeFuncArgs...)>(functionPtr)(std::forward<Args>(args)...);
        };
    }

    FixedFunction(FixedFunction&& other) : FixedFunction()
    {
        MoveFrom(other);
    }

    FixedFunction& operator=(FixedFunction&& other)
    {
        MoveFrom(other);
        return *this;
    }

    ~FixedFunction()
    {
        if (m_allocator)
            m_allocator(&m_functionObjStorage, nullptr);
    }

    /**
    * operator () Execute stored functional object.
    * std::runtime_error if no functional object is stored.
    */
    RetValueType operator()(Args... args)
    {
        if (!m_executor)
            throw std::runtime_error("Call of empty functor");

        return m_executor(&m_functionObjStorage, m_fuctionPtr, std::forward<Args>(args)...);
    }

    explicit operator bool() const
    {
        return m_executor != nullptr;
    }

private:
    FixedFunction& operator=(const FixedFunction &) = delete;
    FixedFunction(const FixedFunction &) = delete;

    void MoveFrom(FixedFunction& other)
    {
        if (this == &other)
            return;

        if (m_allocator) 
        {
            m_allocator(&m_functionObjStorage, nullptr);
            m_allocator = nullptr;
        }
        else 
        {
            m_fuctionPtr = nullptr;
        }

        m_executor = other.m_executor;
        other.m_executor = nullptr;

        if (other.m_allocator)
        {
            m_allocator = other.m_allocator;
            m_allocator(&m_functionObjStorage, &other.m_functionObjStorage);
        }
        else 
        {
            m_fuctionPtr = other.m_fuctionPtr;
        }
    }

    using StorageType = std::aligned_storage_t<StorageSize, sizeof(size_t)>;
    union {
        StorageType m_functionObjStorage;
        FuncPtrType m_fuctionPtr;
    };

    using ExecutorType = RetValueType(*)(void* functionObj, FuncPtrType functionPtr, Args... args);
    ExecutorType m_executor;

    using AllocType = void(*)(void* functionObjStorage, void* functionObjSrc);
    AllocType m_allocator;
};
