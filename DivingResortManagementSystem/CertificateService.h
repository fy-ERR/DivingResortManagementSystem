#ifndef CERTIFICATESERVICE_H
#define CERTIFICATESERVICE_H
#include <iostream>
#include "DB.h"
#include "Payment.h"


class CertificateService {
private:
	DB& db;
	Payment pay;

public:
	CertificateService(DB& db);
	void generateCertificate();

};

#endif