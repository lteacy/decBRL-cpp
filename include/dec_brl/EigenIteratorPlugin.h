/**
 * @file Plugin code to provide standard library style iterators for Eigen
 * arrays and matrix types.
 * @author Luke Teacy
 */
#ifndef EIGEN_ITERATOR_PLUGIN_H
#define EIGEN_ITERATOR_PLUGIN_H

dec_brl::ConstEigenIterator<DenseBase<Derived> > begin()
{
    return dec_brl::ConstEigenIterator<DenseBase<Derived> >(0,*this);
}

dec_brl::ConstEigenIterator<DenseBase<Derived> > end()
{
    return dec_brl::ConstEigenIterator<DenseBase<Derived> >(size(),*this);
}

#endif // EIGEN_ITERATOR_PLUGIN_H
