#ifndef DBHANDLE_H
#define DBHANDLE_H

/*CopyLeft Techtonic Software 2018 - http://www.techtonicsoftware.com/
 * This Program Is Lisenced Under GNU V3 and comes with ABSOLUTELY NO WARRANTY. 
 * You may distribute, modify and run it however you must not claim it as your own nor sublisence it.
 * Any distribution must include the readme file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <vector>

#define OUTSTR_LEN 1024
#define RESULT_LEN 256

namespace TechtonicSoftware
{
namespace SQLHelper
{

class SQLError
{
public:
  SQLCHAR *SQLState;
  long ErrorNum, NativeError;
  SQLCHAR *MessageText;

  SQLError(SQLCHAR sQLState[], long errorNum, long nativeError, SQLCHAR messageText[]);
};

class SQLQueryData
{
private:
  std::vector<std::vector<char *>> data;
  std::vector<const char *> columnNames;

  void CharArrToLower(char *charArr);
  int GetColIndexFromName(const char *name);

public:
  SQLQueryData();
  ~SQLQueryData();

  void SetColumnNames(std::vector<SQLCHAR *> columnNames);
  void AddRow(std::vector<SQLCHAR *> row);
  char *GetCellData(unsigned rowIndex, unsigned colIndex);
  char *GetCellData(unsigned rowIndex, const char *colName);
  unsigned NumRows();
};

class SQLParam
{

public:
  unsigned char *Argument;
  unsigned ArgumentSize;
  int SQLArgumentTypeCode;

  SQLParam(const char *argument);
  SQLParam(unsigned argument);
  SQLParam(int argument);
  SQLParam(long int argument);
  SQLParam(double argument);
  SQLParam(bool argument);

  ~SQLParam();
};

class DBHandle
{
private:
  const char *connectionStr;
  bool isConnected, errorOccured;
  SQLHENV env;
  SQLHDBC connectionHandle;
  SQLHSTMT statementHandle;
  unsigned timeout_Login_Seconds = 5;

  bool ErrorOccured(unsigned e, char *s, SQLHANDLE h, SQLSMALLINT t, bool dcOnError);
  void Extract_Log_Error(char *fn, SQLHANDLE handle, SQLSMALLINT type);

  void InitAll();
  void FreeAll();
  void InitStatement();
  void FreeStatement(std::vector<SQLCHAR*> *resultRow);

public:
  static void DeleteParams(std::vector<SQLParam *> params);

  std::vector<SQLError *> errors;

  DBHandle(const char *connectionStr);
  ~DBHandle();

  bool SendQuery(const char *query, SQLQueryData *callback);
  bool SendQuery(const char *query, std::vector<SQLParam *> sqlParams, SQLQueryData *callback);

  bool ErrorOccured();
};

} // namespace SQLHelper
} // namespace TechtonicSoftware

#endif // DBHANDLE_H
