#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "SparseMatrix.h"

using std::cout;
using std::endl;

bool compareEntriesByCol(MatrixEntry*, MatrixEntry*);
int findFirstIndex(MatrixEntry**, size_t, int x, int(*)(MatrixEntry*, int));
int findLastIndex(MatrixEntry**, size_t, int x, int(*)(MatrixEntry*, int));

int rowComparator(MatrixEntry*, int);
int colComparator(MatrixEntry*, int);

SparseMatrix::SparseMatrix(std::string& str) {

    this->initializeEntries(8);

    std::string input = str;
    std::string split = ", ";

    size_t tokenPos = 0;

    while((tokenPos = input.find(split)) != std::string::npos) {
        std::string token = input.substr(0, tokenPos);

        insertToken(token);

        input.erase(0, tokenPos + split.length());
    }

    insertToken(input);

    int maxRow = -1;
    int maxCol = -1;

    for(size_t c = 0; c < this->numEntries; c++) {
        MatrixEntry* entry = this->entries[c];

        if(entry->getRow() > maxRow) {
            maxRow = entry->getRow();
        }

        if(entry->getCol() > maxCol) {
            maxCol = entry->getCol();
        }
    }

    this->rows = maxRow;
    this->cols = maxCol;
}

SparseMatrix::SparseMatrix(int rows, int cols) {
    this->initializeEntries(8);

    this->rows = rows;
    this->cols = cols;
}


SparseMatrix::~SparseMatrix() {
    for(size_t c = 0; c < this->numEntries; c++) {
        delete this->entries[c];
    }

    delete [] this->entries;
}

int
SparseMatrix::getRows() { return rows; }

int
SparseMatrix::getCols() { return cols; }

void
SparseMatrix::initializeEntries(int initialSize) {
    this->entriesSize = initialSize;
    this->entries = new MatrixEntry*[initialSize];

    this->numEntries = 0;
}

void
SparseMatrix::expandEntries() {

    int newEntriesSize = this->entriesSize * 2;
    MatrixEntry** expandedEntries = new MatrixEntry*[newEntriesSize];

    for(size_t c = 0; c < this->numEntries; c++) {
        expandedEntries[c] = this->entries[c];
    }

    delete [] this->entries;

    this->entriesSize = newEntriesSize;
    this->entries = expandedEntries;
}

void
SparseMatrix::insertToken(std::string& token) {
    size_t rPos = token.find("r");
    size_t cPos = token.find("c");

    std::string valueStr = token.substr(0, rPos);
    std::string rowStr = token.substr(rPos + 1, (cPos - rPos) - 1);
    std::string colStr = token.substr(cPos + 1, token.size() - cPos);

    int value = atoi(valueStr.c_str());
    int row = atoi(rowStr.c_str());
    int col = atoi(colStr.c_str());

    this->insertValue(value, row, col);
}

void
SparseMatrix::insertValue(int value, int row, int col) {

    if(value == 0 || row <= 0 || col <= 0) {
        return;
    }

    MatrixEntry* entry = new MatrixEntry(value, row, col);

    if(this->numEntries > (int)(this->entriesSize * 0.75)) {
        this->expandEntries();
    }

    int j = this->numEntries;

    while(j > 0 && this->entries[j - 1]->getRow() > entry->getRow()) {
        this->entries[j] = this->entries[j - 1];

        j--;
    }

    while(j > 0 && (this->entries[j - 1]->getRow() == entry->getRow() && this->entries[j - 1]->getCol() > entry->getCol())) {
        this->entries[j] = this->entries[j - 1];

        j--;
    }

    this->entries[j] = entry;
    this->numEntries++;
}

void
SparseMatrix::print() {

    size_t index = 0;
    MatrixEntry* entry;

    if(this->numEntries > 0) {
        entry = this->entries[index];
    } else {
        entry = NULL_PTR;
    }

    for(int r = 1; r <= this->rows; r++) {
        for(int c = 1; c <= this->cols; c++) {
            int value;

            if(entry != NULL_PTR && entry->getRow() == r && entry->getCol() == c) {
                value = entry->getValue();

                index++;

                if(index < this->numEntries) {
                    entry = this->entries[index];
                } else {
                    entry = NULL_PTR;
                }

            } else {
                value = 0;
            }

            cout<<std::setw(10)<<value;
        }

        cout<<endl;
    }
}

SparseMatrix*
SparseMatrix::scalarMultiply(int constant) {
    SparseMatrix* result = new SparseMatrix(this->rows, this->cols);

    if(constant == 0) {
        return result;
    }

    for(size_t c = 0; c < this->numEntries; c++) {
        MatrixEntry* entry = this->entries[c];

        result->insertValue(entry->getValue() * constant, entry->getRow(), entry->getCol());
    }

    return result;
}

bool
SparseMatrix::equals(SparseMatrix* other) {
    if(this->rows != other->rows || this->cols != other->cols) {
        return false;
    }

    if(this->numEntries != other->numEntries) {
        return false;
    }

    for(size_t c = 0; c < this->numEntries; c++) {
        MatrixEntry* thisEntry = this->entries[c];
        MatrixEntry* otherEntry = other->entries[c];

        if(!thisEntry->equals(otherEntry)) {
            return false;
        }
    }

    return true;
}

SparseMatrix*
SparseMatrix::add(SparseMatrix* other) {
    return addSub(other, 1);
}

SparseMatrix*
SparseMatrix::subtract(SparseMatrix* other) {
    return addSub(other, -1);
}

SparseMatrix*
SparseMatrix::addSub(SparseMatrix* other, int sign) {

    if(this->rows != other->rows || this->cols != other->cols) {
        return NULL_PTR;
    }

    SparseMatrix* result = new SparseMatrix(this->rows, this->cols);

    for(int r = 1; r <= this->rows; r++) {

        int myRowStart = findFirstIndex(this->entries, this->numEntries, r, rowComparator);
        int otherRowStart = findFirstIndex(other->entries, other->numEntries, r, rowComparator);

        if(myRowStart == -1 && otherRowStart == -1) {
            continue;
        }

        if(myRowStart == -1) {
            int otherRowEnd = findLastIndex(other->entries, other->numEntries, r, rowComparator);

            for(int i = otherRowStart; i <= otherRowEnd; i++) {
                MatrixEntry* entry = other->entries[i];

                result->insertValue(
                            sign * entry->getValue(),
                            entry->getRow(),
                            entry->getCol());
            }

            continue;
        }

        if(otherRowStart == -1) {
            int myRowEnd = findLastIndex(this->entries, this->numEntries, r, rowComparator);

            for(int i = myRowStart; i <= myRowEnd; i++) {
                MatrixEntry* entry = this->entries[i];

                result->insertValue(
                            entry->getValue(),
                            entry->getRow(),
                            entry->getCol());
            }

            continue;
        }

        int myRowEnd = findLastIndex(this->entries, this->numEntries, r, rowComparator);
        int otherRowEnd = findLastIndex(other->entries, other->numEntries, r, rowComparator);

        int i = myRowStart;
        int j = otherRowStart;

        while(i <= myRowEnd && j <= otherRowEnd) {

            MatrixEntry* myEntry = this->entries[i];
            MatrixEntry* otherEntry = other->entries[j];

            if(myEntry->getCol() == otherEntry->getCol()) {
                result->insertValue(
                            myEntry->getValue() + sign * otherEntry->getValue(),
                            myEntry->getRow(),
                            myEntry->getCol());

                i++;
                j++;
            } else if(myEntry ->getCol() < otherEntry->getCol()) {

                result->insertValue(
                            myEntry->getValue(),
                            myEntry->getRow(),
                            myEntry->getCol());

                i++;

            } else {

                result->insertValue(
                            sign * otherEntry->getValue(),
                            otherEntry->getRow(),
                            otherEntry->getCol());

                j++;

            }

        }

        while(i <= myRowEnd) {
             MatrixEntry* myEntry = this->entries[i];

            result->insertValue(
                            myEntry->getValue(),
                            myEntry->getRow(),
                            myEntry->getCol());

            i++;
        }

        while(j <= otherRowEnd) {

            MatrixEntry* otherEntry = other->entries[j];

            result->insertValue(
                            sign * otherEntry->getValue(),
                            otherEntry->getRow(),
                            otherEntry->getCol());

            j++;
        }
    }

    return result;
}

SparseMatrix*
SparseMatrix::transpose() {
    SparseMatrix* result = new SparseMatrix(this->cols, this->rows);

    for(int c = 0; c < this->numEntries; c++) {
        MatrixEntry* entry = this->entries[c];

        result->insertValue(entry->getValue(), entry->getCol(), entry->getRow());
    }

    return result;
}

SparseMatrix*
SparseMatrix::multiply(SparseMatrix* other) {

    if(this->getCols() != other->getRows()) {
        return NULL_PTR;
    }

    SparseMatrix* result = new SparseMatrix(this->getRows(), other->getCols());

    MatrixEntry** otherEntriesByCol = other->getEntriesByCol();

    for(int r = 1; r <= this->getRows(); r++) {

        int startOfRow = findFirstIndex(this->entries, this->numEntries, r, rowComparator);

        if(startOfRow == -1) {
            continue;
        }

        int endOfRow = findLastIndex(this->entries, this->numEntries, r, rowComparator);

        for(int c = 1; c <= other->getCols(); c++) {
            int startOfCol = findFirstIndex(otherEntriesByCol, other->numEntries, c, colComparator);

            if(startOfCol == -1) {
                continue;
            }

            int endOfCol = findLastIndex(otherEntriesByCol, other->numEntries, c, colComparator);

            int i = startOfRow;
            int j = startOfCol;

            int total = 0;

            while(i <= endOfRow && j <= endOfCol) {

                while(this->entries[i]->getCol() < otherEntriesByCol[j]->getRow()) {
                    i++;
                }

                while(otherEntriesByCol[j]->getRow() < this->entries[i]->getCol()) {
                    j++;
                }

                total += this->entries[i]->getValue() * otherEntriesByCol[j]->getValue();

                i++;
                j++;
            }

            result->insertValue(total, r, c);
        }
    }

    delete [] otherEntriesByCol;

    return result;
}

SparseMatrix*
SparseMatrix::power(int pow) {

    if(this->getRows() != this->getCols()) {
        return NULL_PTR;
    }

    if(pow == 0) {
        return NULL_PTR;
    }

    SparseMatrix* base = this;
    SparseMatrix* result = base;

    while(pow > 0) {
        if(pow % 2 != 0) {
            pow -= 1;

            SparseMatrix* oldResult = result;

            result = result->multiply(base);

            delete oldResult;
        }

        pow /= 2;

        SparseMatrix* oldBase = base;
        base = base->multiply(base);

        delete oldBase;
    }

    return result;
}

MatrixEntry**
SparseMatrix::getEntriesByCol() {

    MatrixEntry** sortedEntries = new MatrixEntry*[this->numEntries];

    for(size_t c = 0; c < this->numEntries; c++) {
        sortedEntries[c] = this->entries[c];
    }

    std::sort(sortedEntries, sortedEntries + this->numEntries, compareEntriesByCol);

    return sortedEntries;
}

bool compareEntriesByCol(MatrixEntry* a, MatrixEntry* b) {
    if(a->getCol() == b->getCol()) {
        return a->getRow() < b->getRow();
    }

    return a->getCol() < b->getCol();
}

int findFirstIndex(MatrixEntry** entries, size_t n, int x, int(*comp)(MatrixEntry*, int)) {
    int low = 0;
    int high = n - 1;

    while (low < high) {
        int mid = low + ((high - low) / 2);

        if(comp(entries[mid], x) <= 0) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    if(comp(entries[low], x) == 0) {
        return low;
    } else {
        return -1;
    }
}

int findLastIndex(MatrixEntry** entries, size_t n, int x, int(*comp)(MatrixEntry*, int)) {
    int low = 0;
    int high = n;

    while (low < high - 1) {
        int mid = low + ((high - low) / 2);

        if(comp(entries[mid], x) < 0) {
            high = mid;
        } else {
            low = mid;
        }
    }

    if(comp(entries[low], x) == 0) {
        return low;
    } else {
        return -1;
    }
}

int rowComparator(MatrixEntry* entry, int row) {
    return row - entry->getRow();
}

int colComparator(MatrixEntry* entry, int col) {
    return col - entry->getCol();
}


