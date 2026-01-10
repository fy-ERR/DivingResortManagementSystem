#include "DB.h"
#include <iostream>
#include <vector>
using namespace std;

DB::DB() {
    try {
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "");
        con->setSchema("divingresortdb");
        cout << "Database connected successfully!" << endl;
    }
    catch (sql::SQLException& e) {
        cerr << "Connection error: " << e.what() << endl;
        con = nullptr;
    }
}

DB::~DB() {
    if (con != nullptr) {
        delete con;
        con = nullptr;
    }
}

QueryResult DB::select(string sql) {
    QueryResult qr = { nullptr, nullptr };

    if (!con) return qr;

    try {
        qr.stmt = con->createStatement();
        qr.res = qr.stmt->executeQuery(sql);
    }
    catch (sql::SQLException& e) {
        cerr << "Connection error: " << e.what() << endl;
        con = nullptr;
    }
    return qr;
}

int DB::execute(string sql) {         
    if (!con) return -1;

    try {
        sql::Statement* stmt = con->createStatement();
        int affectedRows = stmt->executeUpdate(sql);
        delete stmt;
        return affectedRows;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Execution error: " << e.what() << endl;
        return -1;
    }
}

sql::PreparedStatement* DB::prepare(std::string sql) {
    if (!con) return nullptr;

    try {
        return con->prepareStatement(sql);
    }
    catch (sql::SQLException& e) {
        std::cerr << "Prepare error: " << e.what() << std::endl;
        return nullptr;
    }
}

sql::Connection* DB::getConnection() {
    return con;
}

