#pragma once

#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include <vector>

using string = std::string;

struct connection_datas 
{
    string server;
    string user;
    string passwd;
    string database;
};

struct table1_datas
{
    string column1;
    string column2;
    string column3;
    string column4;
};

class CDBConnector
{
    public:
        CDBConnector();
        ~CDBConnector();

        bool DB_Connect();
        bool ExecQuery();
        const void PrintResult() const;
        
        void SetQueryStr(const string& query) { m_strQuery = query; }
        void SetDBInfo(const connection_datas& conn_datas) { m_connDatas = conn_datas; }

    private:
        MYSQL* m_conn;
        MYSQL_RES* m_res;
        std::vector<table1_datas> m_resData;
        connection_datas m_connDatas;
        string m_strQuery;
};