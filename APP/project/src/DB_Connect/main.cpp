#include "maria.hpp"

int main( int argc, char *argv[] )
{
    CDBConnector dbConn;

    dbConn.SetDBInfo({
            "localhost",
            "test",
            "1234",
            "mydatabase"
    });

    if(dbConn.DB_Connect() == false)
        return -1;

    dbConn.SetQueryStr(
        "SELECT * FROM TABLE1"
    );

    if(dbConn.ExecQuery() == false)
        return -1;

    dbConn.PrintResult();

    return 0;
}