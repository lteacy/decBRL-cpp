/**
 * @file Plugin code to provide standard library style iterators for Eigen
 * arrays and matrix types.
 * @author Luke Teacy
 */
#ifndef EIGEN_ITERATOR_PLUGIN_H
#define EIGEN_ITERATOR_PLUGIN_H

typedef Scalar value_type;

typedef dec_brl::ConstEigenIterator<DenseBase<Derived> > const_iterator;

const_iterator begin() const
{
    return const_iterator(0,*this);
}

const_iterator end() const
{
    return const_iterator(size(),*this);
}

#endif // EIGEN_ITERATOR_PLUGIN_H
