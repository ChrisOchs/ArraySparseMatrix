#ifndef MATRIXENTRY_H
#define MATRIXENTRY_H


class MatrixEntry
{

    private:
        int row;
        int col;
        int value;

    public:
        MatrixEntry(int, int, int);
        ~MatrixEntry();

        int getRow();
        int getCol();

        int getValue();

        bool equals(MatrixEntry*);
        bool sameCell(MatrixEntry*);
};

#endif // MATRIXENTRY_H
