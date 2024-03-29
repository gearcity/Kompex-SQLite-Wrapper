/*
	This file is part of Kompex SQLite Wrapper.
	Copyright (c) 2008-2021 Sven Broeske

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include <iostream>

#include "KompexSQLiteBlob.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteException.h"

namespace Kompex
{

SQLiteBlob::SQLiteBlob():
	mBlobHandle(0)
{
}

SQLiteBlob::SQLiteBlob(SQLiteDatabase *db, std::string symbolicDatabaseName, std::string tableName, std::string columnName, int64 rowId, BLOB_ACCESS_MODE accessMode):
	mBlobHandle(0)
{
	OpenBlob(db, symbolicDatabaseName, tableName, columnName, rowId, accessMode);
}

SQLiteBlob::~SQLiteBlob()
{
	if(mBlobHandle != 0)
		CloseBlob();
}

void SQLiteBlob::OpenBlob(SQLiteDatabase *db, std::string symbolicDatabaseName, std::string tableName, std::string columnName, int64 rowId, BLOB_ACCESS_MODE accessMode)
{
	if(mBlobHandle != 0)
		CloseBlob();

	mDatabase = db;
	if(sqlite3_blob_open(mDatabase->GetDatabaseHandle(), symbolicDatabaseName.c_str(), tableName.c_str(), columnName.c_str(), rowId, accessMode, &mBlobHandle) != SQLITE_OK)
		KOMPEX_EXCEPT(sqlite3_errmsg(mDatabase->GetDatabaseHandle()), sqlite3_errcode(mDatabase->GetDatabaseHandle()));
}

void SQLiteBlob::CloseBlob()
{
	if(sqlite3_blob_close(mBlobHandle) != SQLITE_OK)
		KOMPEX_EXCEPT(sqlite3_errmsg(mDatabase->GetDatabaseHandle()), sqlite3_errcode(mDatabase->GetDatabaseHandle()));

	mBlobHandle = 0;
}

int SQLiteBlob::GetBlobSize() const
{
	if(mBlobHandle == 0)
		KOMPEX_EXCEPT("GetBlobSize() no open BLOB handle", -1);

	return sqlite3_blob_bytes(mBlobHandle);
}

void SQLiteBlob::ReadBlob(void *buffer, int numberOfBytes, int offset)
{
	if(mBlobHandle == 0)
		KOMPEX_EXCEPT("ReadBlob() no open BLOB handle", -1);
	if((offset + numberOfBytes) > GetBlobSize())
		KOMPEX_EXCEPT("ReadBlob() offset and numberOfBytes exceed the BLOB size", -1);
		
	switch(sqlite3_blob_read(mBlobHandle, buffer, numberOfBytes, offset))
	{
		case SQLITE_OK:
			break;
		case SQLITE_ABORT:
			KOMPEX_EXCEPT("ReadBlob() BLOB handle expired - can not read BLOB", -1);
		default:
			KOMPEX_EXCEPT(sqlite3_errmsg(mDatabase->GetDatabaseHandle()), sqlite3_errcode(mDatabase->GetDatabaseHandle()));
	}
}

void SQLiteBlob::WriteBlob(const void *buffer, int numberOfBytes, int offset)
{
	if(mBlobHandle == 0)
		KOMPEX_EXCEPT("WriteBlob() no open BLOB handle", -1);
	if((offset + numberOfBytes) > GetBlobSize())
		KOMPEX_EXCEPT("WriteBlob() offset and numberOfBytes exceed the BLOB size", -1);

	switch(sqlite3_blob_write(mBlobHandle, buffer, numberOfBytes, offset))
	{
		case SQLITE_OK:
			break;
		case SQLITE_ABORT:
			KOMPEX_EXCEPT("WriteBlob() BLOB handle expired - can not write BLOB", -1);
		default:
			KOMPEX_EXCEPT(sqlite3_errmsg(mDatabase->GetDatabaseHandle()), sqlite3_errcode(mDatabase->GetDatabaseHandle()));
	}
}

}	// namespace Kompex
