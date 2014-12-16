#define EIGEN2_SUPPORT
#include <Eigen/Core>
#include <iostream>

using namespace Eigen;
using namespace std;

int main()
{
  Matrix3f m = Matrix3f::Random();
  cout << "Here is the matrix m:" << endl << m << endl;
  Matrix3f n = Matrix3f::Random();
  cout << "And here is the matrix n:" << endl << n << endl;
  cout << "The coefficient-wise product of m and n is:" << endl;
  cout << m.cwise() * n << endl;
  cout << "The product of m and n is:" << endl;
  cout << m * n << endl;
  cout << "Taking the cube of the coefficients of m yields:" << endl;
  cout << m.cwise().pow(3) << endl;
}
