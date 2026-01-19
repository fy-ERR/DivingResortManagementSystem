#ifndef BOOKING_H
#define BOOKING_H
#include "DB.h"
#include "Customer.h"
#include "Payment.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include "mysql_connection.h"
#include <vector>
using namespace std;

struct ServiceDetails {
	int serviceID = 0;       // The unique identifier of the service
	double price = 0.0;      // The unit price of the service
	int paxLimit = 0;        // The maximum capacity (paxPerSession)
	int durationDays = 0;    // The duration of the service (duration)
	int prerequisiteLevel = 0; 
};

struct FullBookingDetails {
	//Booking table fields
	int bookingID = 0;
	string bookingDate;
	string bookingStatus;
	double totalAmount = 0.0;
	double refundAmount = 0.0;

	string cusName;
	string cusPhoneNo;

	string serviceName;
	string sericeType;

	string instructorName;

	double totalPaid = 0.0;
};

class Booking {
private:
	DB& db;
	Customer cus;
	Payment pay;

public:
	Booking(DB& database);

	void addBooking();
	void searchBooking();
	void cancelBooking();
	void viewInstructorSchedule();

	ServiceDetails selectService(); //fetch service list and return serviceId
	string getServiceDate();
	bool isValidDate(const string& date);
	int assignInstructor(int serviceID, const std::string& date, int paxLimit);
	int saveBooking(int cusID, int serviceID, int instructorID, string bookingDate, string serviceDate, double price);
};

#endif
