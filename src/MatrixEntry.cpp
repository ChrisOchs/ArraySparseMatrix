#include "MatrixEntry.h"

MatrixEntry::MatrixEntry(int value, int row, int col) :
    value(value), row(row), col(col){

}

MatrixEntry::~MatrixEntry() {

}

int
MatrixEntry::getValue() {
    return value;
}

int
MatrixEntry::getRow() {
    return row;
}

int
MatrixEntry::getCol() {
    return col;
}

bool
MatrixEntry::sameCell(MatrixEntry* other) {
    return this->row == other->row &&
            this->col == other->col;
}

bool
MatrixEntry::equals(MatrixEntry* other) {
    return  sameCell(other) && this->value == other->value;
}


