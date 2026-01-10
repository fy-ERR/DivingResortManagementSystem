#ifndef DB_H
#define DB_H
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include "mysql_connection.h"
#include <string>
#include <vector>

using namespace std;

struct QueryResult {
	sql::Statement* stmt;
	sql::ResultSet* res;
};

class DB {
private:
	sql::Driver* driver;
	sql::Connection* con;

public:
	DB();
	~DB();
	QueryResult select(string sql);
	int execute(string sql);

	sql::PreparedStatement* prepare(string sql);
	sql::Connection* getConnection();

};

#endif
