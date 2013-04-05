/**
 * @file EigenIterator.h
 * Defines an iterator type for iterating over data contained in an eigen3
 * matrix or array.
 * @author Luke Teacy
 */
#ifndef DEC_BRL_EIGEN_ITERATOR_H
#define DEC_BRL_EIGEN_ITERATOR_H

namespace dec_brl {
    
    /**
     * Class for Iterating over the data contained in an eigen3 matrix or array.
     */
    template<class ArrayType> class ConstEigenIterator
    {
    private:
        
        /**
         * Current index into array.
         */
        int index_i;
        
        /**
         * Object that we are iterating over.
         */
        const ArrayType* pArray_i;
        
    public:
        
        /**
         * Copy constructor
         */
        ConstEigenIterator(const ConstEigenIterator& it)
        : index_i(it.index_i), pArray_i(it.pArray_i) {}
        
        /**
         * Construct from index and object
         */
        ConstEigenIterator(const int ind, const ArrayType& array)
        : index_i(ind), pArray_i(&array) {}
        
        /**
         * Assign index
         */
        ConstEigenIterator& operator=(int ind)
        {
            index_i = ind;
            return *this;
        }
        
        /**
         * Copy assignment
         */
        ConstEigenIterator& operator=(const ConstEigenIterator& rhs)
        {
            index_i = rhs.index_i;
            pArray_i = rhs.pArray_i;
            return *this;
        }
        
        /**
         * prefix increment operator
         */
        ConstEigenIterator& operator++()
        {
            ++index_i;
            return *this;
        }
        
        /**
         * postfix increment operator
         */
        ConstEigenIterator operator++(int)
        {
            return ConstEigenIterator(index_i++,*pArray_i);
        }
        
        /**
         * Subtract constant from iterator
         */
        ConstEigenIterator operator-(int val)
        {
            return ConstEigenIterator(index_i-val,*pArray_i);
        }
        
        /**
         * Add constant to iterator
         */
        ConstEigenIterator operator+(int val)
        {
            return ConstEigenIterator(index_i+val,*pArray_i);
        }
        
        /**
         * Find the distance between to iterators.
         */
        int operator-(const ConstEigenIterator& rhs)
        {
            return index_i-rhs.index_i;
        }
        
        /**
         * Add constant to this iterator.
         */
        ConstEigenIterator& operator+=(int val)
        {
            index_i += val;
            return *this;
        }
        
        /**
         * Subtract constant from this iterator.
         */
        ConstEigenIterator& operator-=(int val)
        {
            index_i -= val;
            return *this;
        }
        
        /**
         * Dereference operator
         */
        const typename ArrayType::Scalar& operator*()
        {
            return (*pArray_i)[index_i];
        }
        
        /**
         * Check for iterator equality.
         */
        bool operator==(const ConstEigenIterator& rhs)
        {
            return (pArray_i==rhs.pArray_i) && (index_i==rhs.index_i);
        }
        
        /**
         * Check for iterator equality.
         */
        bool operator!=(const ConstEigenIterator& rhs)
        {
            return (index_i!=rhs.index_i) || (pArray_i!=rhs.pArray_i);
        }
        
    }; // class ConstEigenIterator
    
} // namespace dec_brl

#endif // DEC_BRL_EIGEN_ITERATOR_H
