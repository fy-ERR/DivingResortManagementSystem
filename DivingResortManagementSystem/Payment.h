#ifndef PAYMENT_H
#define PAYMENT_H
#include "DB.h"
#include <iostream>
#include <string>

class Payment {
private:
	DB& db;

public:
	Payment(DB& database);


	void savePayment(int bookingId, double amount);
	string getCurrentDate();
	bool processFullPayment(int bookingId, double totalAmount);
};

#endif
