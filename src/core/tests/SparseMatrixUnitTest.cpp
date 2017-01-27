/*
 * Copyright (C) 2015 Fondazione Istituto Italiano di Tecnologia
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <iDynTree/Core/SparseMatrix.h>
#include <iDynTree/Core/Triplets.h>
#include <iDynTree/Core/TestUtils.h>
#include <iostream>


using namespace iDynTree;
using namespace std;

void testCreateMatrixFromAccessorOperator()
{
    SparseMatrix matrix(5,5);
    matrix(2,0) = 7;
    std::cout << matrix.description() << "\n";
    matrix(0,1) = 3;
    std::cout << matrix.description() << "\n";
    matrix(1,0) = 22;
    std::cout << matrix.description() << "\n";
    matrix(1,4) = 17;
    std::cout << matrix.description() << "\n";
    matrix(2,1) = 5;
    std::cout << matrix.description() << "\n";
    matrix(2,3) = 1;
    std::cout << matrix.description() << "\n";
    matrix(4,2) = 14;
    std::cout << matrix.description() << "\n";
    matrix(4,4) = 8;
    std::cout << matrix.description() << "\n";

    std::cout << "Matrix:\n" << matrix.internalDescription() << "\n";
    std::cout << "Matrix:\n" << matrix.description(true) << "\n";
}

void testCreateMatrixFromTriplets()
{
    SparseMatrix matrix(5,5);
    Triplets triplets;
    triplets.pushTriplet(iDynTree::Triplet(2, 0, 7));
    triplets.pushTriplet(iDynTree::Triplet(0, 1, 3));
    triplets.pushTriplet(iDynTree::Triplet(1, 0, 22));
    triplets.pushTriplet(iDynTree::Triplet(1, 4, 17));
    triplets.pushTriplet(iDynTree::Triplet(2, 1, 5));
    triplets.pushTriplet(iDynTree::Triplet(2, 3, 1));
    triplets.pushTriplet(iDynTree::Triplet(4, 2, 14));
    triplets.pushTriplet(iDynTree::Triplet(4, 4, 8));

    matrix.setFromTriplets(triplets);

    std::cout << "Matrix:\n" << matrix.description(true) << "\n";
}

void testCreateMatrixFromDuplicateTriplets()
{
    SparseMatrix matrix(5,5);
    Triplets triplets;
    triplets.pushTriplet(iDynTree::Triplet(2, 0, 7));
    triplets.pushTriplet(iDynTree::Triplet(0, 1, 3));
    triplets.pushTriplet(iDynTree::Triplet(1, 0, 22));
    triplets.pushTriplet(iDynTree::Triplet(1, 4, 17));
    triplets.pushTriplet(iDynTree::Triplet(2, 1, 5));
    triplets.pushTriplet(iDynTree::Triplet(2, 3, 1));
    triplets.pushTriplet(iDynTree::Triplet(1, 4, -5));
    triplets.pushTriplet(iDynTree::Triplet(4, 2, 14));
    triplets.pushTriplet(iDynTree::Triplet(4, 4, 8));
    triplets.pushTriplet(iDynTree::Triplet(0, 1, 4));

    matrix.setFromTriplets(triplets);

    std::cout << "Matrix:\n" << matrix.description(true) << "\n";
}

void testMatrixIterator()
{
    SparseMatrix matrix(5,5);
    Triplets triplets;
    triplets.pushTriplet(iDynTree::Triplet(2, 0, 7));
    triplets.pushTriplet(iDynTree::Triplet(0, 1, 3));
    triplets.pushTriplet(iDynTree::Triplet(1, 0, 22));
    triplets.pushTriplet(iDynTree::Triplet(1, 4, 17));
    triplets.pushTriplet(iDynTree::Triplet(2, 1, 5));
    triplets.pushTriplet(iDynTree::Triplet(2, 3, 1));
    triplets.pushTriplet(iDynTree::Triplet(1, 4, -5));
    triplets.pushTriplet(iDynTree::Triplet(4, 2, 14));
    triplets.pushTriplet(iDynTree::Triplet(4, 4, 8));
    triplets.pushTriplet(iDynTree::Triplet(0, 1, 4));

    matrix.setFromTriplets(triplets);
    std::cout << "Matrix:\n" << matrix.description(true) << "\n";

    //iterator can modify values
    for (SparseMatrix::iterator it(matrix.begin()); it != matrix.end(); ++it) {
        it->value()++;
    }

    //const iterator cannot
    for (SparseMatrix::const_iterator it(matrix.begin()); it!= matrix.end() ; ++it) {
        std::cout << it->value << "(" << it->row << "," << it->column << ")\n";
    }
}

int main()
{
    testCreateMatrixFromAccessorOperator();
    testCreateMatrixFromTriplets();
    testCreateMatrixFromDuplicateTriplets();
    testMatrixIterator();
}
