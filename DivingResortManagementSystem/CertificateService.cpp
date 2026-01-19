#include <iostream>
#include <iomanip>
#include <fstream>
#include "CertificateService.h"
using namespace std;

CertificateService::CertificateService(DB& database) : db(database), pay(database) {}

void CertificateService::generateCertificate(){
	int bookingID;
	cout << "\n--- Generate Certificate ---\n";
	cout << "Enter Booking ID: ";
	if (!(cin >> bookingID)) {
		cin.clear(); cin.ignore(10000, '\n');
		cout << "Invalid input.\n";
		return;
	}

	// Check booking + service type (only for lessons)
	auto pstmt = db.prepare(R"(
        SELECT 
            B.bookingStatus,
            C.cusName,
            C.cusPhoneNo,
            S.serviceName,
            S.serviceType,
            BS.serviceDate,
            I.instructorName
        FROM booking B
        JOIN customer C ON B.cusID=C.cusID
        JOIN booking_service BS ON B.bookingID=BS.bookingID
        JOIN service S ON BS.serviceID=S.serviceID
        JOIN instructor_assignment IA ON B.bookingID=IA.bookingID
        JOIN instructor I ON IA.instructorID=I.instructorID
        WHERE B.bookingID=?
    )");
	if (!pstmt) return;

	pstmt->setInt(1, bookingID);
	auto res = pstmt->executeQuery();

	if (!res->next()) {
		cout << "Booking not found.\n";
		delete res; delete pstmt;
		return;
	}

	string status = res->getString("bookingStatus");
	string serviceType = res->getString("serviceType");

	if (status == "Cancelled") {
		cout << "Cannot generate certificate for cancelled booking.\n";
		delete res; delete pstmt;
		cout << "Press any key to continue...\n\n";
		cin.ignore();
		cin.get();
		return;
	}

	string cusName = res->getString("cusName");
	string serviceName = res->getString("serviceName");
	string serviceDate = res->getString("serviceDate");
	string instructorName = res->getString("instructorName");

	delete res;
	delete pstmt;

	// Generate a simple text certificate
	string filename = "certificates/certificate_booking_" + to_string(bookingID) + ".txt";
	ofstream out(filename);
	if (!out) {
		cout << "Failed to create certificate file.\n";
		return;
	}
	out << "+-----------------------------------------------------------+\n";
	out << "|                                                           |\n";
	out << "|              **DIVING RESORT OFFICIAL SEAL**              |\n";
	out << "|              ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                |\n";
	out << "|                                                           |\n";
	out << "|   This is to certify that the following individual has    |\n";
	out << "|   successfully participated in our diving program.        |\n";
	out << "|                                                           |\n";
	out << "+-----------------------------------------------------------+\n";
	out << "|                                                           |\n";
	out << "|   CANDIDATE: " << left << setw(44) << cusName << " |\n";
	out << "|   COURSE:    " << left << setw(44) << serviceName << " |\n";
	out << "|   DATE:      " << left << setw(44) << serviceDate << " |\n";
	out << "|   INSTRUCTOR:" << left << setw(44) << instructorName << " |\n";
	out << "|   BOOKING ID:" << left << setw(44) << bookingID << " |\n";
	out << "|                                                           |\n";
	out << "+-----------------------------------------------------------+\n";
	out << "|                                                           |\n";
	out << "|   STATUS: [ COMPLETED ]                                   |\n";
	out << "|   ISSUE DATE: " << left << setw(43) << pay.getCurrentDate() << " |\n";
	out << "|                                                           |\n";
	out << "|   _______________________         _____________________   |\n";
	out << "|    Instructor Signature            Resort Management      |\n";
	out << "|                                                           |\n";
	out << "+-----------------------------------------------------------+\n";

	out.close();

	cout << "\nCertificate generated: " << filename << "\n";
	cout << "(File saved in your program folder)\n";

	cout << "\nPress Enter to continue...";
	cin.ignore(10000, '\n');
	cin.get();
}