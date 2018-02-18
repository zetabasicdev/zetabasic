#pragma once

class Range
{
public:
    Range()
        :
        mStartRow(0),
        mStartCol(0),
        mEndRow(0),
        mEndCol(0)
    {
        // intentionally left blank
    }

    Range(int row, int col)
        :
        mStartRow(row),
        mStartCol(col),
        mEndRow(row),
        mEndCol(col)
    {
        // intentionally left blank
    }

    Range(int startRow, int startCol, int endRow, int endCol)
        :
        mStartRow(startRow),
        mStartCol(startCol),
        mEndRow(endRow),
        mEndCol(endCol)
    {
        // intentionally left blank
    }

    Range(const Range& start, const Range& end)
        :
        mStartRow(start.mStartRow),
        mStartCol(start.mStartCol),
        mEndRow(end.mEndRow),
        mEndCol(end.mEndCol)
    {
        // intentionally left blank
    }

    ~Range()
    {
        // intentionally left blank
    }

    void reset()
    {
        mStartRow = mStartCol = 0;
        mEndRow = mEndCol = 0;
    }

    void set(const Range& start, const Range& end)
    {
        mStartRow = start.mStartRow;
        mStartCol = start.mStartCol;
        mEndRow = start.mEndRow;
        mEndCol = start.mEndCol;
    }

    int getStartRow() const
    {
        return mStartRow;
    }

    int getStartCol() const
    {
        return mStartCol;
    }

    int getEndRow() const
    {
        return mEndRow;
    }

    int getEndCol() const
    {
        return mEndCol;
    }

private:
    int mStartRow;
    int mStartCol;
    int mEndRow;
    int mEndCol;
};
