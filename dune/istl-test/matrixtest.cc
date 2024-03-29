// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
/** \file
    \brief Unit tests for the different dynamic matrices provided by ISTL
 */
#include "config.h"

#include <fenv.h>

#include <dune/common/fmatrix.hh>
#include <dune/common/diagonalmatrix.hh>
#include <dune/istl/bcrsmatrix.hh>
#include <dune/istl/matrix.hh>
#include <dune/istl/bdmatrix.hh>
#include <dune/istl/btdmatrix.hh>
#include <dune/istl/scaledidmatrix.hh>


using namespace Dune;

// forward decls

template <class MatrixType>
void testSuperMatrix(MatrixType& matrix);
template <class MatrixType, class X, class Y>
void testMatrix(MatrixType& matrix, X& x, Y& y);
template <class MatrixType, class VectorType>
void testSolve(const MatrixType& matrix);

template <class MatrixType>
void testSuperMatrix(MatrixType& matrix)
{
  // ////////////////////////////////////////////////////////
  //   Check the types which are exported by the matrix
  // ////////////////////////////////////////////////////////

  typedef typename MatrixType::field_type field_type;

  typedef typename MatrixType::block_type block_type;

  typedef typename MatrixType::size_type size_type;

  typedef typename MatrixType::allocator_type allocator_type;

  size_type n = matrix.N();
  size_type m = matrix.M();
  BlockVector<FieldVector<field_type, block_type::cols> > x(m);
  BlockVector<FieldVector<field_type, block_type::rows> > y(n);

  testMatrix(matrix, x, y);
}


template <class MatrixType, class X, class Y>
void testMatrix(MatrixType& matrix, X& x, Y& y)
{
  // ////////////////////////////////////////////////////////
  //   Check the types which are exported by the matrix
  // ////////////////////////////////////////////////////////

  typedef typename MatrixType::field_type field_type;

  typedef typename FieldTraits<field_type>::real_type real_type;

  typedef typename MatrixType::block_type block_type;

  typedef typename MatrixType::row_type row_type;

  typedef typename MatrixType::size_type size_type;

  typedef typename MatrixType::RowIterator RowIterator;

  typedef typename MatrixType::ConstRowIterator ConstRowIterator;

  typedef typename MatrixType::ColIterator ColIterator;

  typedef typename MatrixType::ConstColIterator ConstColIterator;

  assert(MatrixType::blocklevel >= 0);

  // ////////////////////////////////////////////////////////
  //   Count number of rows, columns, and nonzero entries
  // ////////////////////////////////////////////////////////

  typename MatrixType::RowIterator rowIt    = matrix.begin();
  typename MatrixType::RowIterator rowEndIt = matrix.end();

  typename MatrixType::size_type numRows = 0, numEntries = 0;

  for (; rowIt!=rowEndIt; ++rowIt) {

    typename MatrixType::ColIterator colIt    = rowIt->begin();
    typename MatrixType::ColIterator colEndIt = rowIt->end();

    for (; colIt!=colEndIt; ++colIt) {
      assert(matrix.exists(rowIt.index(), colIt.index()));
      numEntries++;
    }

    numRows++;

  }

  assert (numRows == matrix.N());

  // ///////////////////////////////////////////////////////////////
  //   Count number of rows, columns, and nonzero entries again.
  //   This time use the const iterators
  // ///////////////////////////////////////////////////////////////

  typename MatrixType::ConstRowIterator constRowIt    = matrix.begin();
  typename MatrixType::ConstRowIterator constRowEndIt = matrix.end();

  numRows    = 0;
  numEntries = 0;

  for (; constRowIt!=constRowEndIt; ++constRowIt) {

    typename MatrixType::ConstColIterator constColIt    = constRowIt->begin();
    typename MatrixType::ConstColIterator constColEndIt = constRowIt->end();

    for (; constColIt!=constColEndIt; ++constColIt)
      numEntries++;

    numRows++;

  }

  assert (numRows == matrix.N());

  // ////////////////////////////////////////////////////////
  //   Count number of rows, columns, and nonzero entries
  //   This time we're counting backwards
  // ////////////////////////////////////////////////////////

  rowIt    = matrix.beforeEnd();
  rowEndIt = matrix.beforeBegin();

  numRows    = 0;
  numEntries = 0;

  for (; rowIt!=rowEndIt; --rowIt) {

    typename MatrixType::ColIterator colIt    = rowIt->beforeEnd();
    typename MatrixType::ColIterator colEndIt = rowIt->beforeBegin();

    for (; colIt!=colEndIt; --colIt) {
      assert(matrix.exists(rowIt.index(), colIt.index()));
      numEntries++;
    }

    numRows++;

  }

  assert (numRows == matrix.N());

  // ///////////////////////////////////////////////////////////////
  //   Count number of rows, columns, and nonzero entries again.
  //   This time use the const iterators and count backwards.
  // ///////////////////////////////////////////////////////////////

  constRowIt    = matrix.beforeEnd();
  constRowEndIt = matrix.beforeBegin();

  numRows    = 0;
  numEntries = 0;

  for (; constRowIt!=constRowEndIt; --constRowIt) {

    typename MatrixType::ConstColIterator constColIt    = constRowIt->beforeEnd();
    typename MatrixType::ConstColIterator constColEndIt = constRowIt->beforeBegin();

    for (; constColIt!=constColEndIt; --constColIt)
      numEntries++;

    numRows++;

  }

  assert (numRows == matrix.N());

  // ///////////////////////////////////////////////////////
  //   More dimension stuff
  // ///////////////////////////////////////////////////////

  size_type n = matrix.N(); ++n;
  size_type m = matrix.M(); ++m;

  // ///////////////////////////////////////////////////////
  //   Test assignment operators and the copy constructor
  // ///////////////////////////////////////////////////////

  // assignment from other matrix
  MatrixType secondMatrix;
  secondMatrix = matrix;

  // assignment from scalar
  matrix = 0;

  // The copy constructor
  DUNE_UNUSED MatrixType thirdMatrix(matrix);

  // ///////////////////////////////////////////////////////
  //   Test component-wise operations
  // ///////////////////////////////////////////////////////

  matrix *= M_PI;
  matrix /= M_PI;

  matrix += secondMatrix;
  matrix -= secondMatrix;

  // ///////////////////////////////////////////////////////////
  //   Test the various matrix-vector multiplications
  // ///////////////////////////////////////////////////////////

  Y yy=y;

  matrix.mv(x,yy);

  matrix.mtv(x,yy);

  matrix.umv(x,y);

  matrix.umtv(x,y);

  matrix.umhv(x,y);

  matrix.mmv(x,y);

  matrix.mmtv(x,y);

  matrix.mmhv(x,y);

  matrix.usmv(M_PI,x,y);

  matrix.usmtv(M_PI,x,y);

  matrix.usmhv(M_PI,x,y);

  // //////////////////////////////////////////////////////////////
  //   Test the matrix norms
  // //////////////////////////////////////////////////////////////

  real_type frobenius_norm = matrix.frobenius_norm();

  frobenius_norm += matrix.frobenius_norm2();

  frobenius_norm += matrix.infinity_norm();

  frobenius_norm += matrix.infinity_norm_real();

}

// ///////////////////////////////////////////////////////////////////
//   Test the solve()-method for those matrix classes that have it
// ///////////////////////////////////////////////////////////////////
template <class MatrixType, class VectorType>
void testSolve(const MatrixType& matrix)
{
  typedef typename VectorType::size_type size_type;

  // create some right hand side
  VectorType b(matrix.N());
  for (size_type i=0; i<b.size(); i++)
    b[i] = i;

  // solution vector
  VectorType x(matrix.M());

  // Solve the system
  matrix.solve(x,b);

  // compute residual
  matrix.mmv(x,b);

  if (b.two_norm() > 1e-10)
    DUNE_THROW(ISTLError, "Solve() method doesn't appear to produce the solution!");
}

int main()
{

  // feenableexcept does not exist on OS X
#ifndef __APPLE__
  feenableexcept(FE_INVALID);
#endif


  // ////////////////////////////////////////////////////////////
  //   Test the Matrix class -- a scalar dense dynamic matrix
  // ////////////////////////////////////////////////////////////

  Matrix<FieldMatrix<double,1,1> > matrixScalar(10,10);
  for (int i=0; i<10; i++)
    for (int j=0; j<10; j++)
      matrixScalar[i][j] = (i+j)/((double)(i*j+1));        // just anything

  testSuperMatrix(matrixScalar);

  // ////////////////////////////////////////////////////////////
  //   Test the Matrix class -- a block-valued dense dynamic matrix
  // ////////////////////////////////////////////////////////////

  Matrix<FieldMatrix<double,3,3> > matrix(10,10);
  for (int i=0; i<10; i++)
    for (int j=0; j<10; j++)
      for (int k=0; k<3; k++)
        for (int l=0; l<3; l++)
          matrix[i][j][k][l] = (i+j)/((double)(k*l+1));            // just anything

  testSuperMatrix(matrix);

  // ////////////////////////////////////////////////////////////
  //   Test the BCRSMatrix class -- a sparse dynamic matrix
  // ////////////////////////////////////////////////////////////

  BCRSMatrix<FieldMatrix<double,2,2> > bcrsMatrix(4,4, BCRSMatrix<FieldMatrix<double,2,2> >::random);

  bcrsMatrix.setrowsize(0,2);
  bcrsMatrix.setrowsize(1,3);
  bcrsMatrix.setrowsize(2,3);
  bcrsMatrix.setrowsize(3,2);

  bcrsMatrix.endrowsizes();

  bcrsMatrix.addindex(0, 0);
  bcrsMatrix.addindex(0, 1);

  bcrsMatrix.addindex(1, 0);
  bcrsMatrix.addindex(1, 1);
  bcrsMatrix.addindex(1, 2);

  bcrsMatrix.addindex(2, 1);
  bcrsMatrix.addindex(2, 2);
  bcrsMatrix.addindex(2, 3);

  bcrsMatrix.addindex(3, 2);
  bcrsMatrix.addindex(3, 3);

  bcrsMatrix.endindices();

  typedef BCRSMatrix<FieldMatrix<double,2,2> >::RowIterator RowIterator;
  typedef BCRSMatrix<FieldMatrix<double,2,2> >::ColIterator ColIterator;

  for(RowIterator row = bcrsMatrix.begin(); row != bcrsMatrix.end(); ++row)
    for(ColIterator col = row->begin(); col != row->end(); ++col)
      *col = 1.0 + (double) row.index() * (double) col.index();

  testSuperMatrix(bcrsMatrix);

  // ////////////////////////////////////////////////////////////////////////
  //   Test the BDMatrix class -- a dynamic block-diagonal matrix
  // ////////////////////////////////////////////////////////////////////////

  BDMatrix<FieldMatrix<double,4,4> > bdMatrix(2);
  bdMatrix = 4.0;

  testSuperMatrix(bdMatrix);

  // ////////////////////////////////////////////////////////////////////////
  //   Test the BTDMatrix class -- a dynamic block-tridiagonal matrix
  //   a) the scalar case
  // ////////////////////////////////////////////////////////////////////////

  BTDMatrix<FieldMatrix<double,1,1> > btdMatrixScalar(4);
  typedef BTDMatrix<FieldMatrix<double,1,1> >::size_type size_type;

  btdMatrixScalar = 4.0;

  testSuperMatrix(btdMatrixScalar);

  btdMatrixScalar = 0.0;
  for (size_type i=0; i<btdMatrixScalar.N(); i++)    // diagonal
    btdMatrixScalar[i][i] = 1+i;

  for (size_type i=0; i<btdMatrixScalar.N()-1; i++)
    btdMatrixScalar[i][i+1] = 2+i;               // first off-diagonal

  testSolve<BTDMatrix<FieldMatrix<double,1,1> >, BlockVector<FieldVector<double,1> > >(btdMatrixScalar);

  // test a 1x1 BTDMatrix, because that is a special case
  BTDMatrix<FieldMatrix<double,1,1> > btdMatrixScalar_1x1(1);
  btdMatrixScalar_1x1 = 1.0;
  testSuperMatrix(btdMatrixScalar_1x1);

  // ////////////////////////////////////////////////////////////////////////
  //   Test the BTDMatrix class -- a dynamic block-tridiagonal matrix
  //   b) the block-valued case
  // ////////////////////////////////////////////////////////////////////////

  BTDMatrix<FieldMatrix<double,2,2> > btdMatrix(4);
  typedef BTDMatrix<FieldMatrix<double,2,2> >::size_type size_type;

  btdMatrix = 0.0;
  for (size_type i=0; i<btdMatrix.N(); i++)    // diagonal
    btdMatrix[i][i] = ScaledIdentityMatrix<double,2>(1+i);

  for (size_type i=0; i<btdMatrix.N()-1; i++)
    btdMatrix[i][i+1] = ScaledIdentityMatrix<double,2>(2+i);               // upper off-diagonal
  for (size_type i=1; i<btdMatrix.N(); i++)
    btdMatrix[i-1][i] = ScaledIdentityMatrix<double,2>(2+i);               // lower off-diagonal

  // add some off diagonal stuff to the blocks in the matrix
  // diagonals
  btdMatrix[0][0][0][1] = 2;
  btdMatrix[0][0][1][0] = -1;

  btdMatrix[1][1][0][1] = 2;
  btdMatrix[1][1][1][0] = 3;

  btdMatrix[2][2][0][1] = 2;
  btdMatrix[2][2][0][0] += sqrt(2.);
  btdMatrix[2][2][1][0] = 3;

  btdMatrix[3][3][0][1] = -1;
  btdMatrix[3][3][0][0] -= 0.5;
  btdMatrix[3][3][1][0] = 2;

  // off diagonals
  btdMatrix[0][1][0][1] = std::sqrt(2);
  btdMatrix[1][0][0][1] = std::sqrt(2);

  btdMatrix[1][0][1][0] = -13./17.;
  btdMatrix[1][2][0][1] = -1./std::sqrt(2);
  btdMatrix[1][2][1][0] = -13./17.;

  btdMatrix[2][1][0][1] = -13./17.;
  btdMatrix[2][1][1][0] = -13./17.;
  btdMatrix[2][3][0][1] = -1./std::sqrt(2);
  btdMatrix[2][3][1][0] = -17.;

  btdMatrix[3][2][0][1] = 1.;
  btdMatrix[3][2][1][0] = 1.;


  BTDMatrix<FieldMatrix<double,2,2> > btdMatrixThrowAway = btdMatrix;    // the test method overwrites the matrix
  testSuperMatrix(btdMatrixThrowAway);

  testSolve<BTDMatrix<FieldMatrix<double,2,2> >, BlockVector<FieldVector<double,2> > >(btdMatrix);

  // test a 1x1 BTDMatrix, because that is a special case
  BTDMatrix<FieldMatrix<double,2,2> > btdMatrix_1x1(1);
  btdMatrix_1x1 = 1.0;
  testSuperMatrix(btdMatrix_1x1);

  // ////////////////////////////////////////////////////////////////////////
  //   Test the FieldMatrix class
  // ////////////////////////////////////////////////////////////////////////
  typedef FieldMatrix<double,4,4>::size_type size_type;
  FieldMatrix<double,4,4> fMatrix;

  for (size_type i=0; i<fMatrix.N(); i++)
    for (size_type j=0; j<fMatrix.M(); j++)
      fMatrix[i][j] = (i+j)/3;        // just anything
  FieldVector<double,4> fvX;
  FieldVector<double,4> fvY;

  testMatrix(fMatrix, fvX, fvY);

  // ////////////////////////////////////////////////////////////////////////
  //   Test the 1x1 specialization of the FieldMatrix class
  // ////////////////////////////////////////////////////////////////////////

  FieldMatrix<double,1,1> fMatrix1x1;
  fMatrix1x1[0][0] = 2.3;    // just anything

  FieldVector<double,1> fvX1;
  FieldVector<double,1> fvY1;

  testMatrix(fMatrix, fvX, fvY);

  // ////////////////////////////////////////////////////////////////////////
  //   Test the DiagonalMatrix class
  // ////////////////////////////////////////////////////////////////////////

  FieldVector<double,1> dMatrixConstructFrom;
  dMatrixConstructFrom = 3.1459;

  DiagonalMatrix<double,4> dMatrix1;
  dMatrix1 = 3.1459;
  testMatrix(dMatrix1, fvX, fvY);

  DiagonalMatrix<double,4> dMatrix2(3.1459);
  testMatrix(dMatrix2, fvX, fvY);

  DiagonalMatrix<double,4> dMatrix3(dMatrixConstructFrom);
  testMatrix(dMatrix3, fvX, fvY);

  // ////////////////////////////////////////////////////////////////////////
  //   Test the ScaledIdentityMatrix class
  // ////////////////////////////////////////////////////////////////////////

  ScaledIdentityMatrix<double,4> sIdMatrix;
  sIdMatrix = 3.1459;

  testMatrix(sIdMatrix, fvX, fvY);
}
