/*
 * Copyright (C) 2016 Fondazione Istituto Italiano di Tecnologia
 * Authors: Silvio Traversaro, Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "Triplets.h"
#include "MatrixDynSize.h"
#include "MatrixFixSize.h"
#include "SparseMatrix.h"

#include <algorithm>

namespace iDynTree {

    Triplet::Triplet(unsigned row, unsigned column, double value)
    : row(row)
    , column(column)
    , value(value) {}

    bool Triplet::operator<(const iDynTree::Triplet &other) const
    {
        return compare(*this, other);
    }

    bool Triplet::compare(const iDynTree::Triplet &a, const iDynTree::Triplet &b)
    {
        return a.row < b.row
        || (a.row == b.row && a.column < b.column);
    }

    void Triplets::reserve(unsigned size)
    {
        m_triplets.reserve(size);
    }

    template <unsigned rows, unsigned cols>
    void Triplets::setSubMatrix(unsigned startingRow,
                                unsigned startingColumn,
                                const MatrixFixSize<rows, cols>& matrix)
    {
        m_triplets.reserve(m_triplets.size() + (rows * cols));
        for (unsigned row = 0; row < rows; ++row) {
            for (unsigned col = 0; col < cols; ++col) {
                m_triplets.push_back(Triplet(startingRow + row, startingColumn + col, matrix(row, col)));
            }
        }
    }

    void Triplets::setSubMatrix(unsigned startingRow,
                                unsigned startingColumn,
                                const MatrixDynSize &matrix)
    {
        unsigned rows = matrix.rows();
        unsigned cols = matrix.cols();

        m_triplets.reserve(m_triplets.size() + (rows * cols));
        for (unsigned row = 0; row < rows; ++row) {
            for (unsigned col = 0; col < cols; ++col) {
                m_triplets.push_back(Triplet(startingRow + row, startingColumn + col, matrix(row, col)));
            }
        }
    }

    void Triplets::setSubMatrix(unsigned startingRow,
                                unsigned startingColumn,
                                const SparseMatrix &matrix)
    {
        m_triplets.reserve(m_triplets.size() + matrix.numberOfNonZeros());
        //TODO: missing iterator in sparsematrix
    }

    unsigned Triplets::size() const { return m_triplets.size(); }

    std::vector<iDynTree::Triplet>::const_iterator Triplets::begin() const { return m_triplets.begin(); }
    std::vector<iDynTree::Triplet>::iterator Triplets::begin() { return m_triplets.begin(); }
    std::vector<iDynTree::Triplet>::const_iterator Triplets::end() const { return m_triplets.end(); }
    std::vector<iDynTree::Triplet>::iterator Triplets::end() { return m_triplets.end(); }

    void Triplets::pushTriplet(const iDynTree::Triplet &triplet)
    {
        m_triplets.push_back(triplet);
    }

}
