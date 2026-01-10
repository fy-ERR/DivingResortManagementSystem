#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <iostream>
#include "DB.h"


struct CustomerDetails {
	int customerID = 0;
	int certificationLevel = 0;
};

class Customer {
private: 
	DB& db;

public:
	Customer(DB& database);

	int addCustomer(); //add new cus details and return the new generated Id
	//void viewCustomer();
	void updateCustomerDetails();
	CustomerDetails getCusIdByPhone(string phone); //retrieve cusId by phone no. as parameter

	bool isValidPhone(const string& phone);
	bool isValidEmail(const string& email);
};

#endif
