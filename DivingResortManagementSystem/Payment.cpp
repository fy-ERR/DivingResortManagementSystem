#include "Payment.h"
#include <iomanip>
#include <iostream>
#include <ctime>
#include <sstream>
using namespace std;

Payment::Payment(DB& database) : db(database) {}

string Payment::getCurrentDate() {
	auto t = time(nullptr);
	struct tm tm;

	//localtime_s (&target_struct, &source_time)
	localtime_s(&tm, &t);

	std::ostringstream oss;
	oss << put_time(&tm, "%Y-%m-%d");
	return oss.str();
}

bool Payment::processFullPayment(int bookingID, double totalAmount) {
	double paidAmount;
	cout << "\n=========================================\n";
	cout << "             Final Payment Section         \n";
	cout << "=========================================\n";
	cout << "Booking Total: RM " << std::fixed << std::setprecision(2) << totalAmount << "\n";

	cout << "Enter payment amount: RM";
	while (!(cin >> paidAmount) || paidAmount < totalAmount) {
		//Allow overpayment and treat it as the full amount, giving change outside the system
		if (paidAmount > totalAmount) {
			cout << "Payment accepted. Change of RM" << fixed << setprecision(2) << (paidAmount - totalAmount) << "is due to the customer.\n";
			paidAmount = totalAmount;
			break;
		}
		cout << "Invalid amount. Payment must be at least RM" << totalAmount << "(Full payment).\n";
		cout << "Enter payment amount: RM";
		cin.clear();
		cin.ignore(1000, '\n');
	}

	savePayment(bookingID, paidAmount);
	cout << "\nPayment successful! \n";

	cout << "Press any key to continue...\n\n";
	cin.ignore();
	cin.get(); //wait user to press for continue 
	return true;
}


void Payment::savePayment(int bookingID, double amount) {
	try {
		string date = getCurrentDate();

		auto pstmt = db.prepare("INSERT INTO payment(bookingID, paymentDate, paymentAmount) VALUES(? , ? , ? )");
		pstmt->setInt(1, bookingID);
		pstmt->setString(2, date);
		pstmt->setDouble(3, amount);

		pstmt->executeUpdate();
		delete pstmt;
	}
	catch (sql::SQLException& e) {
		std::cerr << "Payment Database Error: " << e.what() << std::endl;
	}
}

