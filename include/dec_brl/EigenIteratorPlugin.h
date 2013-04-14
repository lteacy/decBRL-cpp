/**
 * @file Plugin code to provide standard library style iterators for Eigen
 * arrays and matrix types.
 * @author Luke Teacy
 */
#ifndef EIGEN_ITERATOR_PLUGIN_H
#define EIGEN_ITERATOR_PLUGIN_H

typedef Scalar value_type;

dec_brl::ConstEigenIterator<DenseBase<Derived> > begin() const
{
    return dec_brl::ConstEigenIterator<DenseBase<Derived> >(0,*this);
}

dec_brl::ConstEigenIterator<DenseBase<Derived> > end() const
{
    return dec_brl::ConstEigenIterator<DenseBase<Derived> >(size(),*this);
}

#endif // EIGEN_ITERATOR_PLUGIN_H
