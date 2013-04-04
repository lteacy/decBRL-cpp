/**
 * @file BoostCompatibleArray.h
 * Defines Wrapper class for Eigen3 Array, to make it compatible with
 * the Boost.Math concept of a scalar. This should enable vectorisation of
 * special math function calculations provided by boost.
 */
#ifndef DECBRL_CPP_BOOSTCOMPATIBLEARRAY_H
#define DECBRL_CPP_BOOSTCOMPATIBLEARRAY_H

#include <Eigen/Dense>
#include <boost/utility/enable_if.hpp>
#include <limits>

/**
 * Namespace all public functions and types defined in the DecBRL library.
 */
namespace dec_brl {
    
    /**
     * Wrapper class for Eigen3 Array, to make it compatible with
     * the Boost.Math concept of a scalar. This should enable vectorisation of
     * special math function calculations provided by boost.
     */
    template<typename Scalar> class BoostCompatibleArray
    : public Eigen::Array<Scalar,1,Eigen::Dynamic>
    {
    public:
        
        /**
         * The Base class for this type.
         */
        typedef Eigen::Array<Scalar,1,Eigen::Dynamic> Base;
        
        /**
         * Default constructor.
         */
        BoostCompatibleArray() : Base() {}
        
        /**
         * Constructor required by Boost library for construction from scalar.
         */
        BoostCompatibleArray(Scalar s) : Base(1)
        {
            (*this)[0] = s;
        }
        
        /**
         * This constructor allows you to construct BoostCompatibleArray from
         * Eigen expressions.
         */
        template<typename OtherDerived>
        BoostCompatibleArray(const Eigen::ArrayBase<OtherDerived>& other)
        : Base(other)
        { }
        
        /**
         * This method allows you to assign Eigen expressions to
         * BoostCompatibleArray
         */
        template<typename OtherDerived>
        BoostCompatibleArray & operator=
            (const Eigen::ArrayBase <OtherDerived>& other)
        {
            this->Base::operator=(other);
            return *this;
        }
        
        /**
         * This method allows you to assign Eigen expressions to
         * BoostCompatibleArray
         */
        template<typename OtherDerived>
        BoostCompatibleArray & operator*=
        (const Eigen::ArrayBase <OtherDerived>& other)
        {
            // allow rhs to be wrapped scalar
            if(other.size()==1)
            {
                this->Base::operator*=(other(0));
                return *this;
            }
            
            // allow scalars to resize
            if(this->size()==1)
            {
                this->setConstant(other.size(),(*this)[0]);
            }
            
            this->Base::operator*=(other);
            return *this;
        }
        
        /**
         * This method allows you to assign Eigen expressions to
         * BoostCompatibleArray
         */
        template<typename OtherDerived>
        BoostCompatibleArray & operator+=
        (const Eigen::ArrayBase <OtherDerived>& other)
        {
            // allow rhs to be wrapped scalar
            if(other.size()==1)
            {
                this->Base::operator+=(other(0));
                return *this;
            }
            
            // allow scalars to resize
            if(this->size()==1)
            {
                this->setConstant(other.size(),(*this)[0]);
            }
            
            this->Base::operator+=(other);
            return *this;
        }
        
        /**
         * This method allows you to assign Eigen expressions to
         * BoostCompatibleArray
         */
        template<typename OtherDerived>
        BoostCompatibleArray & operator-=
        (const Eigen::ArrayBase <OtherDerived>& other)
        {
            // allow rhs to be wrapped scalar
            if(other.size()==1)
            {
                this->Base::operator-=(other(0));
                return *this;
            }
            
            // allow scalars to resize
            if(this->size()==1)
            {
                this->setConstant(other.size(),(*this)[0]);
            }
            
            this->Base::operator-=(other);
            return *this;
        }
        
        /**
         * This method allows you to assign Eigen expressions to
         * BoostCompatibleArray
         */
        template<typename OtherDerived>
        BoostCompatibleArray & operator/=
        (const Eigen::ArrayBase <OtherDerived>& other)
        {
            // allow rhs to be wrapped scalar
            if(other.size()==1)
            {
                this->Base::operator/=(other(0));
                return *this;
            }
            
            // allow scalars to resize
            if(this->size()==1)
            {
                this->setConstant(other.size(),(*this)[0]);
            }
            
            this->Base::operator/=(other);
            return *this;
        }
        
        /**
         * This method allows you to assign Eigen expressions to
         * BoostCompatibleArray
         */
        BoostCompatibleArray & operator*=(const Scalar other)
        {            
            this->Base::operator*=(other);
            return *this;
        }
        
        /**
         * This method allows you to assign Eigen expressions to
         * BoostCompatibleArray
         */
        BoostCompatibleArray & operator/=(const Scalar other)
        {
            this->Base::operator/=(other);
            return *this;
        }
        
        /**
         * This method allows you to assign Eigen expressions to
         * BoostCompatibleArray
         */
        BoostCompatibleArray & operator+=(const Scalar other)
        {
            this->Base::operator+=(other);
            return *this;
        }
        
        /**
         * This method allows you to assign Eigen expressions to
         * BoostCompatibleArray
         */
        BoostCompatibleArray & operator-=(const Scalar other)
        {
            this->Base::operator-=(other);
            return *this;
        }
        
    };
    
} // namespace dec_brl
    
namespace std {

    template <typename Scalar>
    class numeric_limits<dec_brl::BoostCompatibleArray<Scalar> >
    : public numeric_limits<Scalar> {};
    
}


#endif // DECBRL_CPP_BOOSTCOMPATIBLEARRAY_H
