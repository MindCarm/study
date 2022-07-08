#include "maria.hpp"

constexpr unsigned int mysqlPort = 3066;

CDBConnector::CDBConnector()
    : m_conn(mysql_init(nullptr))
{
}

CDBConnector::~CDBConnector()
{
    if(m_conn)
        mysql_close(m_conn);
}

bool CDBConnector::DB_Connect()
{
    if(!mysql_real_connect(m_conn, 
        m_connDatas.server.c_str(), 
        m_connDatas.user.c_str(), 
        m_connDatas.passwd.c_str(), 
        m_connDatas.database.c_str(),
        mysqlPort, nullptr, 0)) {
        std::cout << "Connection error : " << mysql_error(m_conn) << std::endl;
        return false;
    }

    return true;
}

bool CDBConnector::ExecQuery()
{
    if(mysql_query(m_conn, m_strQuery.c_str())){
        std::cout << "MySQL query error : " << mysql_error(m_conn) << std::endl;
        return false;
    }

    m_res = mysql_use_result(m_conn);

    MYSQL_ROW rows;
    
    //auto fieldCount = mysql_field_count(m_conn); //해당 테이블의 컬럼 개수 반환

    while (rows = mysql_fetch_row(m_res))
    {
        //auto length = mysql_fetch_lengths(m_res); //row의 해당 컬럼의 데이터의 길이 반환 ex) Column1컬럼의 Data가 'abcd'라면 값은 : 4
        

        table1_datas datas;
        datas.column1 = rows[0];
        datas.column2 = rows[1];
        datas.column3 = rows[2];
        datas.column4 = rows[3];
        m_resData.push_back(datas);
    }
    mysql_free_result(m_res);

    return true;
}

const void CDBConnector::PrintResult() const
{
    for(const auto& var : m_resData)
    {
        std::cout 
        << "column1 : ["<< var.column1 << "]\t"
        << "column2 : ["<< var.column2 << "]\t"
        << "column3 : ["<< var.column3 << "]\t"
        << "column4 : ["<< var.column4 << "]\t"
        << std::endl;
    }

}