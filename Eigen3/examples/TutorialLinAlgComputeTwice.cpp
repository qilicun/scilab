#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main()
{
   Matrix2f A, b;
   LLT<Matrix2f> llt;
   A << 2, -1, -1, 3;
   b << 1, 2, 3, 1;
   cout << "Here is the matrix A:\n" << A << endl;
   cout << "Here is the right hand side b:\n" << b << endl;
   cout << "Computing LLT decomposition..." << endl;
   llt.compute(A);
   cout << "The solution is:\n" << llt.solve(b) << "\nState: " << llt.info() << endl;
   if (llt.info() != Success) {
        cout << "solve failue!\n";
        exit(1);
   } else {
        cout << "Succeccfully solved!\n";
   }
   Matrix2f L = llt.matrixL();
   Matrix2f U = llt.matrixU();
   Matrix2f l = Matrix2f::Identity();
   cout << "Lower part is:\n" << L << endl;
   cout << "Upperer part is:\n" << U << endl;
//   l.block<2,2>(0,0).triangularView<Upper>()=llt.matrixLLT();
   l.triangularView<Upper>()=llt.matrixLLT();
   cout << "LLT part is:\n" << l << endl;
   A(1,1)++;
   cout << "The matrix A is now:\n" << A << endl;
   cout << "Computing LLT decomposition..." << endl;
   llt.compute(A);
   cout << "The solution is now:\n" << llt.solve(b) << endl;
}
