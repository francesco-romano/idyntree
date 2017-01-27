/*
 * Copyright (C) 2016 Fondazione Istituto Italiano di Tecnologia
 * Authors: Silvio Traversaro, Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef IDYNTREE_TRIPLETS_H
#define IDYNTREE_TRIPLETS_H

#include <vector>
#include <iterator>

namespace iDynTree {
    class Triplet;
    class Triplets;

    class MatrixDynSize;
    template <unsigned rows, unsigned cols>
    class MatrixFixSize;
    class SparseMatrix;
}

class iDynTree::Triplet
{
public:

    static bool compare(const iDynTree::Triplet& a, const iDynTree::Triplet &b);

    Triplet(unsigned row, unsigned column, double value);

    bool operator<(const iDynTree::Triplet&) const;

    unsigned row;
    unsigned column;
    double value;
};

class iDynTree::Triplets
{
    std::vector<iDynTree::Triplet> m_triplets;

public:

    void reserve(unsigned size);

    template <unsigned rows, unsigned cols>
    void setSubMatrix(unsigned startingRow,
                      unsigned startingColumn,
                      const MatrixFixSize<rows, cols>&);
    void setSubMatrix(unsigned startingRow,
                      unsigned startingColumn,
                      const MatrixDynSize&);
    void setSubMatrix(unsigned startingRow,
                      unsigned startingColumn,
                      const SparseMatrix&);

    void pushTriplet(const Triplet& triplet);

    unsigned size() const;

    //for now simply return the vector iterator and do not implement our own iterator
    std::vector<iDynTree::Triplet>::const_iterator begin() const;
    std::vector<iDynTree::Triplet>::iterator begin();
    std::vector<iDynTree::Triplet>::const_iterator end() const;
    std::vector<iDynTree::Triplet>::iterator end();

};


#endif /* end of include guard: IDYNTREE_TRIPLETS_H */
