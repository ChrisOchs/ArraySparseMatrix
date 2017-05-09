#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include <string>
#include "MatrixEntry.h"

#define NULL_PTR 0

class SparseMatrix
{
    private:
        int rows;
        int cols;

        MatrixEntry** entries;
        size_t entriesSize;
        size_t numEntries;

        void insertToken(std::string&);

        void insertValue(int, int, int);

        void initializeEntries(int);
        void expandEntries();

        SparseMatrix* addSub(SparseMatrix*, int);

        MatrixEntry** getEntriesByCol();

    public:
        SparseMatrix(std::string&);
        SparseMatrix(int, int);

        ~SparseMatrix();

        int getRows();
        int getCols();

        void print();

        bool equals(SparseMatrix*);

        SparseMatrix* scalarMultiply(int);
        SparseMatrix* add(SparseMatrix*);
        SparseMatrix* subtract(SparseMatrix*);
        SparseMatrix* multiply(SparseMatrix*);
        SparseMatrix* power(int);

        SparseMatrix* transpose();
};

#endif // SPARSEMATRIX_H
