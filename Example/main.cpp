#include "dbhandle.h"
#include <iostream>

using namespace TechtonicSoftware;
using namespace SQLHelper;

const char *CONNECTIONSTR = "Driver=ODBC Driver 17 for SQL Server;Server=127.0.0.1;Uid=sa;Pwd=Password123;database=TestDB";
const long int MAXCOUNT = 25;

int main()
{
    DBHandle *dbHandle;
    std::vector<SQLParam *> params;
    SQLQueryData *callback;

    //Init
    dbHandle = new DBHandle(CONNECTIONSTR);

    //Test Insert Loop (Test Table contains TestTableID (identity id) and TestVal (bigint))
    for (long int i = 0; i < MAXCOUNT; i++)
    {
        params.clear();
        params.push_back(new SQLParam(i));

        std::cout << "Inserting " << i << "\n";

        //Insert Data
        dbHandle->SendQuery("INSERT INTO TestTable(TestVal) Values(?)", params, nullptr);

        if (dbHandle->ErrorOccured())
            std::cout << dbHandle->errors[dbHandle->errors.size() - 1]->MessageText;
    }

    //Get Data
    callback = new SQLQueryData();
    dbHandle->SendQuery("SELECT * FROM TestTable", callback);
    for (unsigned i = 0; i < callback->NumRows(); i++)
        std::cout << "ID: " << callback->GetCellData(i, "TestTableID") << ", Value: " << callback->GetCellData(i, "TestVal") << "\n";

    //Clean Up and close
    delete callback;
    delete dbHandle;
    return 0;
}