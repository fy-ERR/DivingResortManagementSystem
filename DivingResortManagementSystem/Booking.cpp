#include "Booking.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <tuple>
#include <chrono>
#include <sstream>
#include <regex>
#include <fstream>
#include <filesystem>
using namespace std;

Booking::Booking(DB& database) :db(database), cus(database), pay(database) {}

void Booking::addBooking() {
	string phone;
	int choice;
	CustomerDetails customerDetails;

	//Step 1: Customer Search or Add
	cout << "\n=========================================\n";
	cout << "        Diving Resort: New Booking         \n";
	cout << "=========================================\n";
	cout << "1. Enter details for new customer.\n";
	cout << "2. Search customer by phone number. \n";
	cout << "Enter your choice: ";

	// Handle input validation safely
	if (!(cin >> choice)) {
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input. Exiting booking.\n";
		return;
	}

	switch (choice) {
	case 1:
		customerDetails.customerID = cus.addCustomer();
		break;
	case 2:
		//phone no input
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		do {
			cout << "Enter phone number (digits only): ";
			getline(cin, phone);

			if (!cus.isValidPhone(phone)) {
				cout << "Invalid phone number. Please try again.\n";
			}
		} while (!cus.isValidPhone(phone));
		customerDetails = cus.getCusIdByPhone(phone);
		break;
	default:
		cout << "Invalid choice. Exiting booking.\n";
		return;
	}

	if (customerDetails.customerID == 0) {
		cout << "Customer not found or creation failed. \n";
		return;
	}
	cout << "Customer ID " << customerDetails.customerID << " selected/created.\n";

	//Step 2: Select Service (Returns ID, Price, and Pax Limit)
	ServiceDetails serviceDetails = selectService();

	if (serviceDetails.serviceID == 0) {
		cout << "Invalid Service ID selected. Exiting booking.\n";
		return;
	}

	if (serviceDetails.prerequisiteLevel > 0 &&
		customerDetails.certificationLevel < serviceDetails.prerequisiteLevel) {
		cout << "Customer's certification level does not meet the service package's prerequisite level.\n ";
		cout << "Booking cannot proceed.\n";

		cout << "Press any key to continue...\n\n";
		cin.ignore();
		cin.get();
		return;
	}

	//Step 3: Get Date of the booking and when service
	string bookingDate = pay.getCurrentDate();
	string serviceDate = getServiceDate();

	if (serviceDate.empty()) {
		cout << "Invalid service date. Exiting booking.\n";
		return;
	}

	// Step 4: Check Instructor Availability and Capacity
	int instructorID = assignInstructor(
		serviceDetails.serviceID,
		serviceDate,
		serviceDetails.paxLimit
	);

	if (instructorID != 0) {
		// Finalize and Save Booking
		int newBookingID = saveBooking(
			customerDetails.customerID,
			serviceDetails.serviceID,
			instructorID,
			bookingDate,
			serviceDate,
			serviceDetails.price
		);

		//check if booking was saved successfully before asking for payment
		if (newBookingID != 0) {
			pay.processFullPayment(newBookingID, serviceDetails.price);
		}
	}
	else {
		cout << "\nBooking process cancelled due to lack of instructor availability/selection.\n";
	}
}

ServiceDetails Booking::selectService() {
	ServiceDetails details;
	int serviceId;

	cout << "\n--- Available Services ---\n";
	auto qr = db.select("SELECT serviceID, serviceName, serviceType, duration, paxPerSession, price FROM service");

	if (!qr.res) {
		cout << "Error fetching services.\n";
		return details;
	}

	cout << left << setw(4) << "ID" << setw(30) << "Service Name" << setw(18) << "Service Type";
	cout << setw(13) << "Duration" << setw(18) << "Pax/Session" << setw(10) << "Price(RM)\n";
	cout << string(95, '-') << "\n";

	while (qr.res->next()) {
		cout << left << setw(4) << qr.res->getInt("serviceID");
		cout << setw(30) << qr.res->getString("serviceName");
		cout << setw(18) << qr.res->getString("serviceType");
		cout << setw(13) << to_string(qr.res->getInt("duration")) + " day(s)";
		cout << setw(18) << qr.res->getInt("paxPerSession");
		cout << setw(10) << fixed << setprecision(2) << qr.res->getDouble("price") << endl;
	}

	delete qr.res;
	delete qr.stmt;

	cout << "\nChoose service ID: ";
	if (!(cin >> serviceId)) {
		cin.clear();
		cin.ignore(10000, '\n');
		return details;
	}

	// Fetch PRICE, PAX LIMIT, and DURATION
	auto pstmt = db.prepare("SELECT price, paxPerSession, duration, COALESCE(prerequisiteLevel,0) AS prerequisiteLevel FROM service WHERE serviceID = ?");
	if (!pstmt) return details;

	pstmt->setInt(1, serviceId);
	sql::ResultSet* res = pstmt->executeQuery();

	if (res->next()) {
		details.serviceID = serviceId;
		details.price = res->getDouble("price");
		details.paxLimit = res->getInt("paxPerSession");
		details.durationDays = res->getInt("duration");
		details.prerequisiteLevel = res->getInt("prerequisiteLevel");
	}

	delete res;
	delete pstmt;
	return details;
}

bool Booking::isValidDate(const string& date) {
	// Check format YYYY-MM-DD using regex
	regex datePattern(R"(\d{4}-\d{2}-\d{2})");
	if (!regex_match(date, datePattern)) {
		return false;
	}

	// Extract year, month, day
	int year = stoi(date.substr(0, 4));
	int month = stoi(date.substr(5, 2));
	int day = stoi(date.substr(8, 2));

	// Basic validation
	if (month < 1 || month > 12) return false;
	if (day < 1 || day > 31) return false;

	// Days in each month
	int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	// Leap year check
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
		daysInMonth[1] = 29;
	}

	if (day > daysInMonth[month - 1]) return false;

	return true;
}

string Booking::getServiceDate() {
	string date;
	cout << "\nEnter service date (YYYY-MM-DD): ";
	cin >> date;

	if (!isValidDate(date)) {
		cout << "Invalid date format. Please use YYYY-MM-DD.\n";
		return "";
	}

	// Check if date is not in the past
	string currentDate = pay.getCurrentDate();
	if (date < currentDate) {
		cout << "Service date cannot be in the past.\n";
		return "";
	}

	return date;
}

int Booking::assignInstructor(int serviceID, const string& serviceDate, int paxLimit) {
	int instructorID = 0;

	// 1) Get selected service duration
	int duration = 1;
	{
		auto durStmt = db.prepare("SELECT duration FROM service WHERE serviceID=?");
		if (!durStmt) return 0;

		durStmt->setInt(1, serviceID);
		auto durRes = durStmt->executeQuery();
		if (durRes->next()) duration = durRes->getInt("duration");

		delete durRes;
		delete durStmt;
	}

	// 2) Fetch instructors (remove instructorStatus filter unless you REALLY have that column)
	QueryResult qr = db.select("SELECT instructorID, instructorName, specialization FROM instructor");
	if (!qr.res) {
		cout << "Error fetching instructors.\n";
		return 0;
	}

	vector<tuple<int, string, string>> availableInstructors;

	while (qr.res->next()) {
		int currID = qr.res->getInt("instructorID");
		string currName = qr.res->getString("instructorName");
		string specialization = qr.res->getString("specialization");

		// 3) Capacity check for same service session (same service + same date)
		int assignedPax = 0;
		{
			auto capStmt = db.prepare(R"(
                SELECT COALESCE(SUM(BS.quantity),0) AS AssignedPax
                FROM booking B
                JOIN instructor_assignment IA ON B.bookingID = IA.bookingID
                JOIN booking_service BS ON B.bookingID = BS.bookingID
                WHERE IA.instructorID = ?
                  AND BS.serviceID = ?
                  AND BS.serviceDate = ?
                  AND B.bookingStatus = 'Confirmed'
            )");
			if (!capStmt) continue;

			capStmt->setInt(1, currID);
			capStmt->setInt(2, serviceID);
			capStmt->setString(3, serviceDate);

			auto capRes = capStmt->executeQuery();
			if (capRes->next()) assignedPax = capRes->getInt("AssignedPax");

			delete capRes;
			delete capStmt;
		}

		if (assignedPax >= paxLimit) {
			continue; // no remaining capacity
		}

		// 4) Overlap check (multi-day conflicts)
		int overlapCount = 0;
		{
			auto overlapStmt = db.prepare(R"(
	SELECT COUNT(*) AS OverlapCount
    FROM booking B
    JOIN instructor_assignment IA ON B.bookingID = IA.bookingID
    JOIN booking_service BS ON B.bookingID = BS.bookingID
    JOIN service S ON BS.serviceID = S.serviceID
    WHERE IA.instructorID = ?
      AND B.bookingStatus = 'Confirmed'
      -- allow sharing same service session
      AND NOT (BS.serviceID = ? AND BS.serviceDate = ?)
      -- check overlap with other sessions
      AND (
            BS.serviceDate < DATE_ADD(?, INTERVAL ? DAY)
        AND DATE_ADD(BS.serviceDate, INTERVAL S.duration DAY) > ?
      )
)");

			if (!overlapStmt) continue;

			overlapStmt->setInt(1, currID);

			// exclude same session
			overlapStmt->setInt(2, serviceID);
			overlapStmt->setString(3, serviceDate);

			// overlap window for NEW booking
			overlapStmt->setString(4, serviceDate);
			overlapStmt->setInt(5, duration);
			overlapStmt->setString(6, serviceDate);

			auto overlapRes = overlapStmt->executeQuery();
			if (overlapRes->next()) overlapCount = overlapRes->getInt("OverlapCount");

			delete overlapRes;
			delete overlapStmt;
		}

		if (overlapCount == 0) {
			availableInstructors.push_back({ currID, currName, specialization });
		}
	}

	delete qr.res;
	delete qr.stmt;

	// 5) Display list
	if (availableInstructors.empty()) {
		cout << "No instructors available for this service date: " << serviceDate << "\n";
		return 0;
	}

	cout << "\n--- Available Instructors ---\n";
	cout << left << setw(6) << "ID" << setw(25) << "Name" << setw(20) << "Specialization" << "\n";
	cout << string(55, '-') << "\n";
	for (auto& inst : availableInstructors) {
		cout << left << setw(6) << get<0>(inst)
			<< setw(25) << get<1>(inst)
			<< setw(20) << get<2>(inst) << "\n";
	}

	cout << "\nEnter Instructor ID to assign (0 to cancel): ";
	if (!(cin >> instructorID)) {
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input.\n";
		return 0;
	}
	if (instructorID == 0) return 0;

	// validate chosen ID is in list
	bool ok = false;
	for (auto& inst : availableInstructors) {
		if (get<0>(inst) == instructorID) { ok = true; break; }
	}
	if (!ok) {
		cout << "Invalid instructor selection.\n";
		return 0;
	}

	return instructorID;
}

int Booking::saveBooking(int cusID, int serviceID, int instructorID,
	string bookingDate, string serviceDate, double price) {
	int bookingID = 0;

	try {
		// INSERT INTO BOOKING
		auto pstmt = db.prepare("INSERT INTO booking (cusID, bookingDate, bookingStatus, totalAmount) VALUES(?,?,'Confirmed',?)");
		if (!pstmt) return 0;

		pstmt->setInt(1, cusID);
		pstmt->setString(2, bookingDate);
		pstmt->setDouble(3, price);
		pstmt->executeUpdate();
		delete pstmt;

		// Get the generated bookingID
		QueryResult qr = db.select("SELECT LAST_INSERT_ID() AS lastID");
		if (qr.res && qr.res->next()) {
			bookingID = qr.res->getInt("lastID");
		}
		delete qr.res;
		delete qr.stmt;

		if (bookingID == 0) {
			cout << "Error: Failed to retrieve booking ID.\n";
			return 0;
		}

		// INSERT INTO BOOKING_SERVICE
		pstmt = db.prepare("INSERT INTO booking_service (bookingID, serviceID, serviceDate, subtotal, quantity) VALUES (?, ?, ?, ?, 1)");
		if (!pstmt) return 0;

		pstmt->setInt(1, bookingID);
		pstmt->setInt(2, serviceID);
		pstmt->setString(3, serviceDate);
		pstmt->setDouble(4, price);
		pstmt->executeUpdate();
		delete pstmt;

		// INSERT INTO INSTRUCTOR_ASSIGNMENT
		pstmt = db.prepare("INSERT INTO instructor_assignment (bookingID, instructorID) VALUES (?, ?)");
		if (!pstmt) return 0;

		pstmt->setInt(1, bookingID);
		pstmt->setInt(2, instructorID);
		pstmt->executeUpdate();
		delete pstmt;

		cout << "\nBooking created SUCCESSFULLY! Your new booking ID is: " << bookingID <<endl;
	}
	catch (sql::SQLException& e) {
		cerr << "Error saving booking: " << e.what() << endl;
		return 0;
	}

	return bookingID;
}

void Booking::searchBooking() {
	int bookingID;
	cout << "\n=========================================\n";
	cout << "        Diving Resort: Search Booking        \n";
	cout << "=========================================\n";
	cout << "Enter Booking ID to search: ";

	if (!(cin >> bookingID)) {
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input. Returning to main menu. \n";
		return;
	}

	auto pstmt = db.prepare(R"(
		SELECT 
			B.bookingID, B.bookingDate, B.bookingStatus, B.totalAmount, B.refundAmount,
			C.cusName, C.cusPhoneNo,
			S.serviceName, S.serviceType,
			BS.serviceDate,
			I.instructorName,
			COALESCE(SUM(P.paymentAmount), 0) AS totalPaid
		FROM BOOKING B
		JOIN CUSTOMER C ON B.cusID = C.cusID
		JOIN BOOKING_SERVICE BS ON B.bookingID = BS.bookingID
		JOIN SERVICE S ON BS.serviceID = S.serviceID
		JOIN INSTRUCTOR_ASSIGNMENT IA ON B.bookingID = IA.bookingID
		JOIN INSTRUCTOR I ON IA.instructorID = I.instructorID
		LEFT JOIN PAYMENT P ON B.bookingID = P.bookingID
		WHERE B.bookingID = ?
		GROUP BY B.bookingID
	)");

	if (!pstmt) {
		cerr << "Error preparing search statement." << endl;
		return;
	}

	pstmt->setInt(1, bookingID);
	sql::ResultSet* res = pstmt->executeQuery();

	if (res->next()) {
		cout << "\n" << string(50, '=') << "\n";
		cout << "       BOOKING DETAILS - ID " << bookingID << "\n";
		cout << string(50, '=') << "\n\n";

		cout << left << setw(20) << "Status:" << res->getString("bookingStatus") << "\n";
		cout << setw(20) << "Booking Date:" << res->getString("bookingDate") << "\n";
		cout << setw(20) << "Service Date:" << res->getString("serviceDate") << "\n";
		cout << setw(20) << "Total Amount:" << "RM " << fixed << setprecision(2) << res->getDouble("totalAmount") << "\n\n";

		cout << left << setw(20) << "Customer Name:" << res->getString("cusName") << "\n";
		cout << setw(20) << "Phone No.:" << res->getString("cusPhoneNo") << "\n\n";

		cout << setw(20) << "Service:" << res->getString("serviceName") << " (" << res->getString("serviceType") << ")\n";
		cout << setw(20) << "Instructor:" << res->getString("instructorName") << "\n\n";

		double totalPaid = res->getDouble("totalPaid");
		double balance = res->getDouble("totalAmount") - totalPaid;

		cout << setw(20) << "Total Paid:" << "RM " << fixed << setprecision(2) << totalPaid << "\n";
		cout << setw(20) << "Balance Due:" << "RM " << fixed << setprecision(2) << balance << "\n";

		// Show refund if applicable
		if (!res->isNull("refundAmount")) {
			double refundAmount = res->getDouble("refundAmount");
			if (refundAmount > 0.0) {
				cout << setw(20) << "Refund Processed:" << "RM " << fixed << setprecision(2) << refundAmount << "\n";
			}
		}
	}
	else {
		cout << "Error: Booking ID " << bookingID << " not found.\n";
	}

	delete res;
	delete pstmt;

	cout << "\nPress Enter to continue...";
	cin.ignore(10000, '\n');
	cin.get();
}

void Booking::cancelBooking() {
	int bookingID;
	cout << "\n=========================================\n";
	cout << "        Diving Resort: Cancel Booking        \n";
	cout << "=========================================\n";
	cout << "Enter booking ID to cancel: ";

	if (!(cin >> bookingID)) {
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input.\n";
		return;
	}

	try {
		// First check if booking exists and get payment info
		auto pstmt = db.prepare(R"(
		  SELECT 
		   B.bookingStatus,
			BS.serviceDate,
			COALESCE(SUM(P.paymentAmount), 0) AS totalPaid
		FROM booking B
		JOIN booking_service BS ON B.bookingID = BS.bookingID
		LEFT JOIN payment P ON B.bookingID = P.bookingID
		WHERE B.bookingID = ?
		GROUP BY B.bookingID, BS.serviceDate
	)");

		if (!pstmt) return;

		pstmt->setInt(1, bookingID);
		sql::ResultSet* res = pstmt->executeQuery();

		if (res->next()) {
			string status = res->getString("bookingStatus");
			double paid = res->getDouble("totalPaid");
			string cancelDate = pay.getCurrentDate();
			string serviceDate = res->getString("serviceDate");

			if (status == "Cancelled") {
				cout << "This booking is already cancelled.\n";
				delete res;
				delete pstmt;
				return;
			}
			auto diffStmt = db.prepare("SELECT DATEDIFF(?, ?) AS daysBefore");
			diffStmt->setString(1, serviceDate);
			diffStmt->setString(2, cancelDate);
			auto diffRes = diffStmt->executeQuery();

			int daysBefore = 0;
			if (diffRes->next()) daysBefore = diffRes->getInt("daysBefore");
			delete diffRes;
			delete diffStmt;

			double refundRate = 0.0;
			if (daysBefore >= 14) refundRate = 1.0;
			else if (daysBefore >= 7) refundRate = 0.5;
			else refundRate = 0.0;

			double refundAmount = paid * refundRate;

			// Update booking status
			auto updateStmt = db.prepare("UPDATE booking SET bookingStatus='Cancelled', refundAmount=?, cancellationDate=? WHERE bookingID=?");
			if (!updateStmt) {
				delete res;
				delete pstmt;
				return;
			}

			updateStmt->setDouble(1, refundAmount);
			updateStmt->setString(2, cancelDate);
			updateStmt->setInt(3, bookingID);
			updateStmt->executeUpdate();
			delete updateStmt;

			cout << "\nSUCCESS: Booking ID " << bookingID << " has been cancelled.\n";
			if (paid > 0) {
				cout << "\nCancelled before " << daysBefore << " day(s) of service => " << "Refund rate : " << (refundRate * 100) << " % \n";
				cout << "Refund recorded: RM " << fixed << setprecision(2) << refundAmount << "\n";
			}
		}
		else {
			cout << "Error: Booking ID " << bookingID << " not found.\n";
		}
		delete res;
		delete pstmt;
	}
	catch (sql::SQLException& e) {
		cerr << "Cancellation Error: " << e.what() << endl;
	}

	cout << "\nPress Enter to continue...";
	cin.ignore(10000, '\n');
	cin.get();
}

void Booking::viewInstructorSchedule() {
	int instructorID;
	string startDate, endDate;

	//retrieve all instructors on list
	try {
		sql::PreparedStatement* stmt = db.prepare(
			"SELECT instructorID, instructorName, specialization "
			"FROM instructor"
		);

		sql::ResultSet* res = stmt->executeQuery();

		cout << "\n========================= INSTRUCTORS SCHEDULE =============\n";
		cout << left
			<< setw(4) << "ID"
			<< setw(25) << "Name"
			<< setw(25) << "Specialization\n";
		cout << "--------------------------------------------------------\n";

		while (res->next()) {
			cout << left
				<< setw(4) << res->getInt("instructorID")
				<< setw(25) << res->getString("instructorName")
				<< setw(25) << res->getString("specialization")
				<< endl;
		}

		delete res;
		delete stmt;
	}
	catch (sql::SQLException& e) {
		cerr << "Error retrieving instructors: " << e.what() << endl;
	}

	cout << "Enter Instructor ID: ";
	if (!(cin >> instructorID)) {
		cin.clear(); cin.ignore(10000, '\n');
		cout << "Invalid input.\n";
		return;
	}

	cout << "Enter start date (YYYY-MM-DD): ";
	cin >> startDate;
	cout << "Enter end date (YYYY-MM-DD): ";
	cin >> endDate;

	if (!isValidDate(startDate) || !isValidDate(endDate) || endDate < startDate) {
		cout << "Invalid date range.\n";
		return;
	}

	auto pstmt = db.prepare(R"(
        SELECT 
            BS.serviceDate,
            S.serviceName,
            S.serviceType,
            C.cusName,
            B.bookingStatus
        FROM instructor_assignment IA
        JOIN booking B ON IA.bookingID = B.bookingID
        JOIN booking_service BS ON B.bookingID = BS.bookingID
        JOIN service S ON BS.serviceID = S.serviceID
        JOIN customer C ON B.cusID = C.cusID
        WHERE IA.instructorID = ?
          AND BS.serviceDate BETWEEN ? AND ?
        ORDER BY BS.serviceDate
    )");

	if (!pstmt) return;

	pstmt->setInt(1, instructorID);
	pstmt->setString(2, startDate);
	pstmt->setString(3, endDate);

	auto res = pstmt->executeQuery();

	cout << "\nDate         | Service                       | Customer         | Status\n";
	cout << "---------------------------------------------------------------------\n";

	bool any = false;
	while (res->next()) {
		any = true;
		cout << left << setw(12) << res->getString("serviceDate") << " | "
			<< setw(29) << res->getString("serviceName") << " | "
			<< setw(16) << res->getString("cusName") << " | "
			<< res->getString("bookingStatus") << "\n";
	}

	if (!any) cout << "No schedule found in this range.\n";

	delete res;
	delete pstmt;

	cout << "\nPress Enter to continue...";
	cin.ignore(10000, '\n');
	cin.get();
}

void Booking::generateCertificate() {
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

	// You can decide your own rule here:
	// Example: Only allow certificate if status == "Confirmed"
	// or add a new status "Completed"
	if (status == "Cancelled") {
		cout << "Cannot generate certificate for cancelled booking.\n";
		delete res; delete pstmt;
		cout << "Press any key to continue...\n\n";
		cin.ignore();
		cin.get();
		return;
	}
	// Optional: only "Lesson" gets certificate
	// if (serviceType != "Lesson") { ... }

	string cusName = res->getString("cusName");
	string serviceName = res->getString("serviceName");
	string serviceDate = res->getString("serviceDate");
	string instructorName = res->getString("instructorName");

	delete res;
	delete pstmt;

	// Generate a simple text certificate
	std::filesystem::create_directories("certificates");
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

void Booking::monthlyRevenueReport() {
	int year;
	cout << "\n--- Monthly Revenue Report ---\n";
	cout << "Enter year (e.g. 2026): ";
	if (!(cin >> year)) {
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Invalid input.\n";
		return;
	}

	auto pstmt = db.prepare(R"(
        SELECT 
            DATE_FORMAT(P.paymentDate, '%Y-%m') AS month,
            COUNT(DISTINCT P.bookingID) AS totalBookingsPaid,
            SUM(P.paymentAmount) AS grossRevenue,
            COALESCE(SUM(B.refundAmount), 0) AS totalRefund,
            (SUM(P.paymentAmount) - COALESCE(SUM(B.refundAmount), 0)) AS netRevenue
        FROM payment P
        LEFT JOIN booking B ON P.bookingID = B.bookingID
        WHERE YEAR(P.paymentDate) = ?
        GROUP BY DATE_FORMAT(P.paymentDate, '%Y-%m')
        ORDER BY month
    )");

	if (!pstmt) return;

	pstmt->setInt(1, year);
	auto res = pstmt->executeQuery();

	cout << "\n+=============================================================+\n";
	cout << "|                 DIVING RESORT REVENUE REPORT                |\n";
	cout << "+=============================================================+\n\n";
	cout << left
		<< setw(10) << "Month" << "| "
		<< setw(9) << "Bookings" << "| "
		<< setw(12) << "Gross (RM)" << "| "
		<< setw(12) << "Refund (RM)" << "| "
		<< setw(12) << "Net (RM)" << "\n";
	cout << string(62, '-') << "\n";

	bool any = false;
	double totalGross = 0.0;
	double totalRefund = 0.0;
	double totalNet = 0.0;
	vector<pair<string, double>> chartData;

	while (res->next()) {
		any = true;

		string month = res->getString("month");
		int bookings = res->getInt("totalBookingsPaid");
		double gross = res->getDouble("grossRevenue");
		double refund = res->getDouble("totalRefund");
		double net = res->getDouble("netRevenue");

		totalGross += gross;
		totalRefund += refund;
		totalNet += net;

		cout << left << setw(10) << month << "| "
			<< right << setw(8) << bookings << " | "
			<< setw(11) << fixed << setprecision(2) << gross << " | "
			<< setw(11) << refund << " | "
			<< setw(11) << net << "\n";

		chartData.push_back({ month, net });
	}

	if (!any) {
		cout << "No revenue records found for this year.\n";
	}
	else {
		cout << string(62, '-') << "\n";
		cout << left << setw(10) << "TOTAL" << "| "
			<< setw(8) << " " << " | "
			<< setw(11) << fixed << setprecision(2) << totalGross << " | "
			<< setw(11) << totalRefund << " | "
			<< setw(11) << totalNet << "\n\n";
		cout << string(62, '-') << "\n";
	}

	delete res;
	delete pstmt;

	cout << "+============ Monthly Revenue Chart (Net Revenue) ============+\n";
	cout << "(Scale: # = RM 500)\n\n";

	const double SCALE = 500.0;
	for (const auto& item : chartData) {
		printBar(item.first, item.second, SCALE);
	}

	cout << "\n+=============================================================+\n";
	cout << "| Report generated on: " << pay.getCurrentDate() << "  \n";
	cout << "+=============================================================+\n";

	cout << "\nPress Enter to continue...";
	cin.ignore(10000, '\n');
	cin.get();
}

void Booking::printBar(const string& label, double value, double scale) {
	int hashtag = static_cast<int>(value / scale);
	cout << left << setw(10) << label << ": ";

	for (int i = 0; i < hashtag; ++i)
		cout << "#";

	cout << " (RM " << fixed << setprecision(2) << value << ")\n";
}
