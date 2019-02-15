#include "dbhandle.h"

/*CopyLeft Techtonic Software 2018 - http://www.techtonicsoftware.com/
 * This Program Is Lisenced Under GNU V3 and comes with ABSOLUTELY NO WARRANTY. 
 * You may distribute, modify and run it however you must not claim it as your own nor sublisence it.
 * Any distribution must include the readme file.
*/

using namespace TechtonicSoftware;
using namespace SQLHelper;

SQLError::SQLError(SQLCHAR sQLState[], long errorNum, long nativeError, SQLCHAR messageText[])
{
    this->SQLState = sQLState;
    this->ErrorNum = errorNum;
    this->NativeError = nativeError;
    this->MessageText = messageText;
}

void SQLQueryData::CharArrToLower(char *charArr)
{
    unsigned charArrSize;

    charArrSize = sizeof(charArr);

    for (unsigned i = 0; i < charArrSize; i++)
        charArr[i] = tolower(charArr[i]);
}

int SQLQueryData::GetColIndexFromName(const char *name)
{
    unsigned numCols;
    char *name_Lower, *currentColName;
    unsigned nameLen;

    //Put Name to lower
    nameLen = strlen(name) + 1;
    name_Lower = (char *)malloc(sizeof(char *) * nameLen);
    strcpy(name_Lower, name);
    CharArrToLower(name_Lower);

    numCols = this->columnNames.size();
    for (unsigned i = 0; i < numCols; i++)
    {
        currentColName = const_cast<char *>(this->columnNames[i]);
        if (strcmp(name_Lower, currentColName) == 0)
        {
            free(name_Lower);
            return i;
        }
    }

    free(name_Lower);
    return -1;
}

SQLQueryData::SQLQueryData()
{
}

SQLQueryData::~SQLQueryData()
{
    //Remove Data
    for (unsigned i = 0; i < this->data.size(); i++)
    {
        for (unsigned j = 0; j < this->data[i].size(); j++)
        {
            if (this->data[i][j] != nullptr)
                free(this->data[i][j]);
        }
        this->data[i].clear();
    }
    this->data.clear();

    //Remove Columns
    for (unsigned i = 0; i < columnNames.size(); i++)
        free((void *)columnNames[i]);
}

void SQLQueryData::SetColumnNames(std::vector<SQLCHAR *> columnNames)
{
    unsigned numCols;
    std::vector<const char *> newColumnNames;
    char *newColName;

    //Init
    numCols = columnNames.size();

    for (unsigned i = 0; i < numCols; i++)
    {
        //Make to Lower
        newColName = reinterpret_cast<char *>(columnNames[i]);
        CharArrToLower(newColName);

        newColumnNames.push_back(const_cast<const char *>(newColName));
    }

    this->columnNames = newColumnNames;
}

void SQLQueryData::AddRow(std::vector<SQLCHAR *> row)
{
    unsigned rowSize, valueSize;
    std::vector<char *> newRow;
    char *newValue, *oldVal;

    //Init
    rowSize = row.size();

    //Create local pointers
    for (unsigned i = 0; i < rowSize; i++)
    {
        //Work out val size
        valueSize = (strlen(oldVal) + 1) * sizeof(char *);

        //Cast and assign
        oldVal = reinterpret_cast<char *>(row[i]);
        newValue = (char *)malloc(valueSize);

        //Copy into new variable
        strcpy(newValue, oldVal);
        newRow.push_back(newValue);
    }

    this->data.push_back(newRow);
}

char *SQLQueryData::GetCellData(unsigned rowIndex, unsigned colIndex)
{
    return this->data[rowIndex][colIndex];
}

char *SQLQueryData::GetCellData(unsigned rowIndex, const char *colName)
{
    int colIndex;

    colIndex = GetColIndexFromName(colName);

    if (colIndex == -1)
        return nullptr;
    else
        return this->data[rowIndex][colIndex];
}

unsigned SQLQueryData::NumRows()
{
    return this->data.size();
}

SQLParam::SQLParam(const char *argument)
{
    char *nonConst;

    nonConst = const_cast<char *>(argument);

    this->Argument = reinterpret_cast<unsigned char *>(nonConst);
    this->ArgumentSize = strlen(argument);
    this->SQLArgumentTypeCode = SQL_VARCHAR;
}

SQLParam::SQLParam(unsigned argument)
{
    char *newVar, *newVar2;

    newVar = const_cast<char *>(std::to_string(argument).c_str());
    newVar2 = (char *)malloc((strlen(newVar) + 1) * sizeof(char *));
    strcpy(newVar2, newVar);

    this->Argument = reinterpret_cast<unsigned char *>(newVar2);
    this->ArgumentSize = strlen(newVar2);
    this->SQLArgumentTypeCode = SQL_BIGINT;
}

SQLParam::SQLParam(int argument)
{
    char *newVar, *newVar2;

    newVar = const_cast<char *>(std::to_string(argument).c_str());
    newVar2 = (char *)malloc((strlen(newVar) + 1) * sizeof(char *));
    strcpy(newVar2, newVar);

    this->Argument = reinterpret_cast<unsigned char *>(newVar2);
    this->ArgumentSize = strlen(newVar2);
    this->SQLArgumentTypeCode = SQL_INTEGER;
}

SQLParam::SQLParam(long int argument)
{
    char *newVar, *newVar2;

    newVar = const_cast<char *>(std::to_string(argument).c_str());
    newVar2 = (char *)malloc((strlen(newVar) + 1) * sizeof(char *));
    strcpy(newVar2, newVar);

    this->Argument = reinterpret_cast<unsigned char *>(newVar2);
    this->ArgumentSize = strlen(newVar2);
    this->SQLArgumentTypeCode = SQL_BIGINT;
}

SQLParam::SQLParam(double argument)
{
    char *newVar, *newVar2;

    newVar = const_cast<char *>(std::to_string(argument).c_str());
    newVar2 = (char *)malloc((strlen(newVar) + 1) * sizeof(char *));
    strcpy(newVar2, newVar);

    this->Argument = reinterpret_cast<unsigned char *>(newVar2);
    this->ArgumentSize = strlen(newVar2);
    this->SQLArgumentTypeCode = SQL_REAL;
}

SQLParam::SQLParam(bool argument)
{
    char *newVar, *newVar2;

    newVar = const_cast<char *>(std::to_string((unsigned)argument).c_str());
    newVar2 = (char *)malloc((strlen(newVar) + 1) * sizeof(char *));
    strcpy(newVar2, newVar);

    this->Argument = reinterpret_cast<unsigned char *>(newVar2);
    this->ArgumentSize = strlen(newVar2);
    this->SQLArgumentTypeCode = SQL_BIT;
}

SQLParam::~SQLParam()
{
    if (this->Argument != nullptr)
    {
        free(this->Argument);
        this->Argument = nullptr;
    }
}

bool DBHandle::ErrorOccured(unsigned e, char *s, SQLHANDLE h, SQLSMALLINT t, bool dcOnError)
{
    if (e != SQL_SUCCESS && e != SQL_SUCCESS_WITH_INFO)
    {
        this->errorOccured = true;
        Extract_Log_Error(s, h, t);

        if (dcOnError)
            FreeAll();

        return true;
    }
    else
        return false;
}

void DBHandle::Extract_Log_Error(char *fn, SQLHANDLE handle, SQLSMALLINT type)
{
    SQLINTEGER i;
    SQLINTEGER NativeError;
    SQLCHAR SQLState[7];
    SQLCHAR MessageText[256];
    SQLSMALLINT TextLength;
    SQLRETURN ret;

    //Init
    i = 0;

    //Show Error
    fprintf(stderr, "\nThe driver reported the following error %s\n", fn);
    do
    {
        ret = SQLGetDiagRec(type, handle, ++i, SQLState, &NativeError, MessageText, sizeof(MessageText), &TextLength);
        if (SQL_SUCCEEDED(ret))
        {
            this->errors.push_back(new SQLError(SQLState, i, NativeError, MessageText));
            printf("%s:%ld:%ld:%s\n", SQLState, (long)i, (long)NativeError, MessageText);
        }
    } while (ret == SQL_SUCCESS);
}

void DBHandle::InitAll()
{
    SQLRETURN returnCode;
    SQLCHAR outstr[OUTSTR_LEN];
    SQLSMALLINT outstrlen;

    //Init
    this->errorOccured = false;
    this->env = SQL_NULL_HENV;
    this->connectionHandle = SQL_NULL_HDBC;
    this->statementHandle = SQL_NULL_HSTMT;
    outstrlen = OUTSTR_LEN;

    // Allocate environment handle
    returnCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &this->env);
    if (ErrorOccured(returnCode, (char *)"SQLAllocHandle(SQL_HANDLE_ENV)", this->env, SQL_HANDLE_ENV, false))
    {
        printf("%s\n", this->errors.back()->MessageText);
        return;
    }

    // Set the ODBC version environment attribute
    returnCode = SQLSetEnvAttr(this->env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER *)SQL_OV_ODBC3, 0);
    if (ErrorOccured(returnCode, (char *)"SQLSetEnvAttr(SQL_ATTR_ODBC_VERSION)", this->env, SQL_HANDLE_ENV, false))
    {
        printf("%s\n", this->errors.back()->MessageText);
        return;
    }

    // Allocate connection handle
    returnCode = SQLAllocHandle(SQL_HANDLE_DBC, env, &this->connectionHandle);
    if (ErrorOccured(returnCode, (char *)"SQLAllocHandle(SQL_HANDLE_DBC)", this->connectionHandle, SQL_HANDLE_DBC, false))
    {
        printf("%s\n", this->errors.back()->MessageText);
        return;
    }

    // Set login timeout to specified amount
    SQLSetConnectAttr(this->connectionHandle, SQL_LOGIN_TIMEOUT, (SQLPOINTER) & this->timeout_Login_Seconds, 0);
    if (ErrorOccured(returnCode, (char *)"SQLSetConnectAttr(SQL_LOGIN_TIMEOUT)", this->connectionHandle, SQL_HANDLE_DBC, true))
    {
        printf("%s\n", this->errors.back()->MessageText);
        return;
    }

    // Connect to data source
    returnCode = SQLDriverConnect(this->connectionHandle, NULL, (SQLCHAR *)connectionStr, SQL_NTS, outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_NOPROMPT);
    //printf("%s\n", outstr);
    if (ErrorOccured(returnCode, (char *)"SQLDriverConnect()", statementHandle, SQL_HANDLE_STMT, true))
        return;

    isConnected = true;
}

void DBHandle::FreeAll()
{
    // Free handles

    // Connection
    if (this->connectionHandle != SQL_NULL_HDBC)
    {
        SQLDisconnect(this->connectionHandle);
        SQLFreeHandle(SQL_HANDLE_DBC, this->connectionHandle);
    }

    // Environment
    if (this->env != SQL_NULL_HENV)
        SQLFreeHandle(SQL_HANDLE_ENV, this->env);

    isConnected = false;

    this->env = SQL_NULL_HENV;
    this->connectionHandle = SQL_NULL_HDBC;
    this->statementHandle = SQL_NULL_HSTMT;
}

void DBHandle::InitStatement()
{
    RETCODE returnCode;

    // Allocate statement handle
    if (statementHandle == SQL_NULL_HSTMT)
    {
        returnCode = SQLAllocHandle(SQL_HANDLE_STMT, this->connectionHandle, &statementHandle);
        if (ErrorOccured(returnCode, (char *)"SQLAllocHandle(SQL_HANDLE_STMT)", statementHandle, SQL_HANDLE_STMT, true))
        {
            printf("%s", this->errors.back()->MessageText);
            return;
        }
    }
}

void DBHandle::FreeStatement()
{
    // Statement
    if (statementHandle != SQL_NULL_HSTMT)
    {
        SQLFreeHandle(SQL_HANDLE_STMT, statementHandle);
        this->statementHandle = SQL_NULL_HSTMT;
    }
}

DBHandle::DBHandle(const char *connectionStr)
{
    this->connectionStr = connectionStr;
    InitAll();
}

DBHandle::~DBHandle()
{
    FreeAll();
}

bool DBHandle::SendQuery(const char *query, SQLQueryData *callback)
{
    return SendQuery(query, std::vector<SQLParam *>(), callback);
}

bool DBHandle::SendQuery(const char *query, std::vector<SQLParam *> sqlParams, SQLQueryData *callback)
{
    SQLRETURN returnCode;
    unsigned numColumns_LeftOver, sqlparams_Size, statementLength, currentColSize, currentColNameSize;
    std::vector<SQLCHAR *> resultRow, colNames;
    std::vector<unsigned> colSizes;
    SQLCHAR *currentResult;
    bool execOngoing;
    SQLPOINTER *currentColName;
    SQLSMALLINT bufLenUsed;
    SQLParam *currentParam;
    SQLLEN cbValue;
    SQLSMALLINT numColumns;

    if (!isConnected)
        InitAll();

    //Init Statement
    InitStatement();

    //If no params just exec normally
    cbValue = SQL_NTS;
    statementLength = strlen(query);
    sqlparams_Size = sqlParams.size();

    for (unsigned i = 0; i < sqlparams_Size; i++)
    {
        currentParam = sqlParams[i];
        returnCode = SQLBindParameter(statementHandle, (i + 1), SQL_PARAM_INPUT, SQL_C_CHAR, currentParam->SQLArgumentTypeCode, currentParam->ArgumentSize, 0, currentParam->Argument, 0, &cbValue);

        if (ErrorOccured(returnCode, (char *)"SQLBindParameter()", statementHandle, SQL_HANDLE_STMT, true))
        {
            FreeStatement();
            return false;
        }
    }

    //Prepare
    returnCode = SQLPrepare(statementHandle, (SQLCHAR *)query, statementLength);
    if (ErrorOccured(returnCode, (char *)"SQLPrepare()", statementHandle, SQL_HANDLE_STMT, true))
    {
        FreeStatement();
        return false;
    }

    //Exec
    SQLExecDirect(this->statementHandle, (SQLCHAR *)query, statementLength);
    if (ErrorOccured(returnCode, (char *)"SQLExecDirect()", statementHandle, SQL_HANDLE_STMT, true))
    {
        FreeStatement();
        return false;
    }

    if (callback != nullptr)
    {
        //Get Num Columns
        SQLNumResultCols(statementHandle, &numColumns);

        //Bind All Columns
        for (unsigned i = 1; i <= numColumns; i++)
        {
            numColumns_LeftOver = numColumns;
            bufLenUsed = 0;

            currentResult = new SQLCHAR[RESULT_LEN]();
            currentColNameSize = sizeof(SQLPOINTER *) * RESULT_LEN;
            currentColName = (SQLPOINTER *)malloc(currentColNameSize);

            //Bind vars to data output
            returnCode = SQLBindCol(statementHandle, i, SQL_C_CHAR, currentResult, RESULT_LEN, (SQLLEN *)&numColumns_LeftOver);
            if (ErrorOccured(returnCode, (char *)"SQLBindCol()", statementHandle, SQL_HANDLE_STMT, true))
            {
                FreeStatement();
                return false;
            }
            else
                resultRow.push_back(currentResult);

            //Get col name/s
            returnCode = SQLColAttribute(statementHandle, (SQLUSMALLINT)i, SQL_DESC_LABEL, currentColName, currentColNameSize, &bufLenUsed, NULL);
            if (ErrorOccured(returnCode, (char *)"SQLColAttribute(SQL_DESC_LABEL)", statementHandle, SQL_HANDLE_STMT, true))
                colNames.push_back((SQLCHAR *)"");
            else
                colNames.push_back((SQLCHAR *)currentColName);
        }

        //Put in Column Names
        callback->SetColumnNames(colNames);

        //Now Fetch All Data
        execOngoing = true;
        while (execOngoing)
        {
            //Fetch data
            returnCode = SQLFetch(statementHandle);

            //If Success create vars
            if (returnCode == SQL_SUCCESS || returnCode == SQL_SUCCESS_WITH_INFO)
            {
                //Add To Main Result
                callback->AddRow(resultRow);
            }
            else //If Problem check any potential errors and stop loop
            {
                if (returnCode != SQL_NO_DATA)
                {
                    ErrorOccured(returnCode, (char *)"SQLFetch()", statementHandle, SQL_HANDLE_STMT, true);
                    FreeStatement();
                    return false;
                }
                execOngoing = false;
            }
        }
    }

    FreeStatement();
    return true;
}

bool DBHandle::ErrorOccured()
{
    return this->errorOccured;
}
