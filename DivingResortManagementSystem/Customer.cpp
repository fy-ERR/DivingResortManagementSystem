#include "Customer.h"
#include "DB.h"
#include <iostream>
#include <string>
#include <cctype>
using namespace std;

Customer::Customer(DB& database) : db(database) {}

int Customer::addCustomer() {
	string name;
	string email;
	string phoneNo;
	int certificationLevel;

	cout << "Enter customer name: ";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	getline(cin, name);
	//email input
	do {
		cout << "Enter email address: ";
		getline(cin, email);

		if (!isValidEmail(email)) {
			cout << "Invalid email format. Example: user@example.com\n";
		}
	} while (!isValidEmail(email));
	//phone no input
	do {
		cout << "Enter phone number (digits only): ";
		getline(cin, phoneNo);

		if (!isValidPhone(phoneNo)) {
			cout << "Invalid phone number. Please try again.\n";
		}
	} while (!isValidPhone(phoneNo));

	cout << "\n1.Preliminary diver or swimmer. \n2.Open Water Diver. \n3.Advanced Open Water Diver. \n4.Rescue Diver. \n";
	cout << "Enter number to choose customer's certification level: ";
	cin >> certificationLevel;

	auto pstmt = db.prepare("INSERT INTO customer (cusName, cusEmail, cusPhoneNo, certificationLevel) VALUES (?, ?, ?, ?)");
	
	pstmt->setString(1, name);
	pstmt->setString(2, email);
	pstmt->setString(3, phoneNo);
	pstmt->setInt(4, certificationLevel);

	pstmt->executeUpdate();

	delete pstmt;

	//fetch the lastest generated cusId
	QueryResult qr = db.select("SELECT LAST_INSERT_ID()");

	int customerID = 0;
	if (qr.res && qr.res->next())
		customerID = qr.res->getInt(1); //1 column is cusID

	delete qr.res;
	delete qr.stmt;

	return customerID;
}

CustomerDetails Customer::getCusIdByPhone(string phone) {
	CustomerDetails details;
	
	sql::PreparedStatement* pstmt = db.prepare("SELECT cusID, certificationLevel+0 AS certLevel FROM customer WHERE cusPhoneNo = ?");
	if (!pstmt) return details;

	pstmt->setString(1, phone);

	sql::ResultSet* res = pstmt->executeQuery();

	if (res->next()) {
		details.customerID = res->getInt("cusID");
		details.certificationLevel = res->getInt("certLevel");

		std::cout << "\nCustomer Found!\n";
		std::cout << "Customer ID: " << details.customerID << "\n";
		std::cout << "Certification Level: " << details.certificationLevel << endl;
	}

	delete pstmt;
	delete res;

	return details;
}

bool Customer::isValidPhone(const string& phone) {
	if (phone.length() < 9 || phone.length() > 11)
		return false;

	for (char c : phone) {
		if (!isdigit(c))
			return false;
	}
	return true;
}

bool Customer::isValidEmail(const string& email) {
	size_t atPos = email.find('@');
	size_t dotPos = email.find('.', atPos);

	if (atPos == string::npos) return false;         // no @
	if (dotPos == string::npos) return false;        // no dot after @
	if (atPos == 0) return false;                    // starts with @
	if (dotPos <= atPos + 1) return false;           // nothing between @ and .
	if (dotPos == email.length() - 1) return false;  // ends with .

	return true;
}
