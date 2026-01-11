#include "Customer.h"
#include <iostream>
#include <string>
#include <cctype>
using namespace std;

Customer::Customer(DB& database) : db(database) {}

CustomerDetails Customer::addCustomer() {
	CustomerDetails details;
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

	details.customerID = customerID;
	details.certificationLevel = certificationLevel; // <-- you already asked user input

	return details;
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

void Customer::updateCustomerDetails() {
	int customerID;
	cout << "\nEnter Customer ID to update: ";
	cin >> customerID;

	auto pstmt = db.prepare(
		"SELECT cusName, cusPhoneNo, cusEmail, certificationLevel+0 AS certLevel FROM customer WHERE cusID=?"
	);
	pstmt->setInt(1, customerID);
	auto res = pstmt->executeQuery();

	if (!res->next()) {
		cout << "Customer not found.\n";
		delete res;
		delete pstmt;
		return;
	}

	string phone = res->getString("cusPhoneNo");
	string email = res->getString("cusEmail");
	int certificationLevel = res->getInt("certLevel");

	string certName;
	if (certificationLevel == 1) certName = "Preliminary diver or swimmer";
	else if (certificationLevel == 2) certName = "Open Water Diver";
	else if (certificationLevel == 3) certName = "Advanced Open Water Diver";
	else certName = "Rescue Diver";

	cout << "\nCurrent Details:\n";
	cout << "1. Phone   : " << phone << "\n";
	cout << "2. Email   : " << email << "\n";
	cout << "3. certificationLevel : " << certName << "\n";

	delete res;
	delete pstmt;

	int choice;
	cout << "\nSelect field to update (1-3): ";
	cin >> choice;
	cin.ignore();

	if (choice == 3) {
		cout << "\n1.Preliminary diver or swimmer. \n2.Open Water Diver. \n3.Advanced Open Water Diver. \n4.Rescue Diver. \n";
		cout << "Enter number to choose customer's new certification level: ";
	}
	string newValue;
	getline(cin, newValue);

	string sql;
	if (choice == 1) sql = "UPDATE customer SET cusPhoneNo=? WHERE customerID=?";
	else if (choice == 2) sql = "UPDATE customer SET cusEmail=? WHERE customerID=?";
	else if (choice == 3) sql = "UPDATE customer SET certificationLevel=? WHERE cusID=?";
	else {
		cout << "Invalid choice.\n";
		return;
	}

	auto updateStmt = db.prepare(sql);
	updateStmt->setString(1, newValue);
	updateStmt->setInt(2, customerID);
	updateStmt->execute();

	delete updateStmt;

	cout << "Customer details updated successfully.";
	cout << "\nPress Enter to continue...";
	cin.ignore(10000, '\n');
	cin.get();
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
