#include <iostream>
#include <algorithm>

#include "SparseMatrix.h"

using std::cout;
using std::endl;

int main() {

    std::string matrixAStr = "-5r4c4, 5r1c4, 2r2c2, 5r3c1, -3r3c2, -7r2c3, 3r1c1";
    std::string matrixBStr = "-5r4c4, 5r1c4, 2r2c2, 5r3c1, -3r3c2, -7r2c3, 10r1c1";


    SparseMatrix* matrixA = new SparseMatrix(matrixAStr);
    SparseMatrix* matrixB = new SparseMatrix(matrixBStr);

    matrixA->print();

    cout<<endl;

    matrixB->print();

    SparseMatrix* result = matrixA->add(matrixA);

    cout<<endl<<endl;

    if(result != NULL_PTR) {
        result->print();
    }


    delete matrixA;
    delete matrixB;

    delete result;
}
