//
//  Object.h
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef Object_h
#define Object_h

#include <typeinfo>
#include <type_traits>
#include <stdexcept>
#include <iostream>
#include <ostream>
#include <map>
#include <vector>

namespace Plist
{
    class Object;
    
    typedef std::vector<Object> Array;
    typedef std::map<std::string, Object> Dictionary;
    typedef std::vector<char> Data;
    
    class Object
    {
    public:
        Object() : m_pContent(0) {}
        
        template <typename ValueType>
        Object(const ValueType& value)
        : m_pContent(new holder<typename std::decay<const ValueType>::type>(value))
        {
        }
        
        Object(const Object& other) : m_pContent(other.m_pContent ? other.m_pContent->clone() : 0) {}
        
        virtual ~Object() { destory(); }
        
    public:
        void destory()
        {
            if (m_pContent != NULL) {
                delete m_pContent;
            }
            m_pContent = NULL;
        }
        
        Object& swap(Object& rhs)
        {
            std::swap(m_pContent, rhs.m_pContent);
            return *this;
        }
        
        template <typename ValueType>
        Object& operator=(const ValueType& rhs)
        {
            Object(rhs).swap(*this);
            return *this;
        }
        
        Object& operator=(const Object& rhs)
        {
            Object(rhs).swap(*this);
            return *this;
        }
        
        template <typename T>
        bool isType() {
            return getType() == typeid(T);
        }
    public:
        bool isEmpth() const { return !m_pContent; }
        
        const std::type_info& getType() const { return m_pContent ? m_pContent->getType() : typeid(void); }
        
    protected:
        class placeholder
        {
        public:
            virtual ~placeholder() {}
            
        public:
            virtual const std::type_info& getType() const = 0;
            virtual placeholder* clone() const = 0;
        };
        
        template <typename ValueType>
        class holder : public placeholder
        {
        public:
            ValueType held;
            
        public:
            holder(const ValueType& value) : held(value) {}
            
        public:
            virtual const std::type_info& getType() const { return typeid(ValueType); }
            
            virtual placeholder* clone() const { return new holder(held); }
            
        };
        
    protected:
        placeholder* m_pContent;
        template <typename ValueType>
        friend ValueType* obj_cast(Object*);
        
    public:
        template <typename ValueType>
        ValueType operator()() const
        {
            if (!m_pContent) {
                
            } else if (getType() == typeid(ValueType)) {
                return static_cast<Object::holder<ValueType>*>(m_pContent)->held;
            }
        }
        
        template <typename ValueType>
        ValueType get() const
        {
            if (!m_pContent) {
                
            } else if (getType() == typeid(ValueType)) {
                return static_cast<Object::holder<ValueType>*>(m_pContent)->held;
            }
        }
    };
    
    //cost
    template <typename ValueType>
    ValueType* obj_cast(Object* operand)
    {
        return operand && (operand->getType() == typeid(ValueType)) ?
        &static_cast<Object::holder<ValueType>*>(operand->m_pContent)->held :
        0;
    }
    
    
    template <typename ValueType>
    inline const ValueType* obj_cast(const Object* operand)
    {
        return obj_cast<ValueType>(const_cast<Object*>(operand));
    }
    
    
    
    template <typename ValueType>
    ValueType obj_cast(Object& operand)
    {
        typedef typename std::remove_reference<ValueType>::type nonref;
        nonref* result = obj_cast<nonref>(&operand);
        typedef typename std::conditional<std::is_reference<ValueType>::value, ValueType, typename std::add_lvalue_reference<ValueType>::type>::type ref_type;
        return static_cast<ref_type>(*result);;
    }
    
    template<typename ValueType>
    inline ValueType obj_cast(const Object & operand)
    {
        typedef typename std::remove_reference<ValueType>::type nonref;
        return obj_cast<const nonref &>(const_cast<Object &>(operand));
    }
    
    class  bad_obj_cast : public std::bad_cast
    {
    public:
        virtual const char * what() const noexcept
        {
            return "boost::bad_any_cast: "
            "failed conversion using boost::any_cast";
        }
    };
}

#endif /* Object_h */
