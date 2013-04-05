/**
 * Wrapper for Eigen3 library, which provides standard library style iterators
 * for eigen library arrays and matrices.
 */
#ifndef EIGEN_WITH_PLUGIN_H
#define EIGEN_WITH_PLUGIN_H

#ifdef EIGEN_DENSEBASE_H // don't let Eigen compile without plugin
YOU_MUST_INCLUDE_PLUGIN_BEFORE_EIGEN_HEADERS
#endif

#include "EigenIterator.h"
#define EIGEN_DENSEBASE_PLUGIN "dec_brl/EigenIteratorPlugin.h"
#include <Eigen/Dense>

#endif // EIGEN_WITH_PLUGIN_H
