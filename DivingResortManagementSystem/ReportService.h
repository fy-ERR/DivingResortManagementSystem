#ifndef REPORTSERVICE_H
#define REPORTSERVICE_H
#include <iostream>
#include "DB.h"
#include "Payment.h"


class ReportService {
private:
	DB& db;
	Payment pay;

public:
	ReportService(DB& db);
	void monthlyRevenueReport();
	void printBar(
		const std::string& label,
		double gross,
		double refund,
		double lastMonthNet,
		double scale
	);

};

#endif