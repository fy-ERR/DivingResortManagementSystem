#include <iostream>
#include <iomanip>
#include <fstream>
#include "CertificateService.h"
using namespace std;

CertificateService::CertificateService(DB& database) : db(database), pay(database) {}

void CertificateService::generateCertificate() {
    int bookingID;

    cout << "\n--- Generate Certificate ---\n";
    cout << "Enter Booking ID: ";
    if (!(cin >> bookingID)) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "Invalid input.\n";
        return;
    }

    // 1) Check if certificate already exists
    {
        auto checkStmt = db.prepare("SELECT certID FROM certificate WHERE bookingID=?");
        if (!checkStmt) return;

        checkStmt->setInt(1, bookingID);
        auto checkRes = checkStmt->executeQuery();

        if (checkRes->next()) {
            cout << "Certificate already generated. Cert ID: "
                << checkRes->getInt("certID") << endl;
            delete checkRes;
            delete checkStmt;

            cout << "\nPress Enter to continue...";
            cin.ignore(10000, '\n');
            cin.get();
            return;
        }

        delete checkRes;
        delete checkStmt;
    }

    // 2) Validate booking first (booking exists, not cancelled)
    string cusName, serviceName, serviceDate, instructorName, status;

    auto pstmt = db.prepare(R"(
        SELECT 
            B.bookingStatus,
            C.cusName,
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
        LIMIT 1
    )");
    if (!pstmt) return;

    pstmt->setInt(1, bookingID);
    auto res = pstmt->executeQuery();

    if (!res->next()) {
        cout << "Booking not found.\n";
        delete res; delete pstmt;
        cout << "\nPress Enter to continue...";
        cin.ignore(10000, '\n');
        cin.get();
        return;
    }

    status = res->getString("bookingStatus");
    if (status == "Cancelled") {
        cout << "Cannot generate certificate for cancelled booking.\n";
        delete res; delete pstmt;
        cout << "\nPress Enter to continue...";
        cin.ignore(10000, '\n');
        cin.get();
        return;
    }

    cusName = res->getString("cusName");
    serviceName = res->getString("serviceName");
    serviceDate = res->getString("serviceDate");
    instructorName = res->getString("instructorName");

    delete res;
    delete pstmt;

    // 3) Prepare certificate file path
    string filePath = "certificates/certificate_booking_" + to_string(bookingID) + ".txt";

    // 4) Insert certificate record NOW (safe)
    int certID = 0;
    {
        auto insertStmt = db.prepare(
            "INSERT INTO certificate (bookingID, issueDate, filePath) "
            "VALUES (?, CURDATE(), ?)"
        );
        if (!insertStmt) return;

        insertStmt->setInt(1, bookingID);
        insertStmt->setString(2, filePath);
        insertStmt->execute();
        delete insertStmt;

        auto idStmt = db.prepare("SELECT LAST_INSERT_ID() AS certID");
        if (!idStmt) return;

        auto idRes = idStmt->executeQuery();
        if (idRes->next()) certID = idRes->getInt("certID");

        delete idRes;
        delete idStmt;
    }

    // 5) Generate certificate file
    ofstream certFile(filePath);
    if (!certFile) {
        cout << "Failed to create certificate file.\n";
        auto delStmt = db.prepare("DELETE FROM certificate WHERE certID=?");
        delStmt->setInt(1, certID);
        delStmt->execute();
        delete delStmt;

        return;
    }

    certFile << "+-----------------------------------------------------------+\n";
    certFile << "|                                                           |\n";
    certFile << "|              **DIVING RESORT OFFICIAL SEAL**              |\n";
    certFile << "|              ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                |\n";
    certFile << "|                                                           |\n";
    certFile << "|   This is to certify that the following individual has    |\n";
    certFile << "|   successfully participated in our diving program.        |\n";
    certFile << "|                                                           |\n";
    certFile << "+-----------------------------------------------------------+\n";
    certFile << "|                                                           |\n";
    certFile << "|   CANDIDATE: " << left << setw(44) << cusName << " |\n";
    certFile << "|   COURSE:    " << left << setw(44) << serviceName << " |\n";
    certFile << "|   DATE:      " << left << setw(44) << serviceDate << " |\n";
    certFile << "|   INSTRUCTOR:" << left << setw(44) << instructorName << " |\n";
    certFile << "|   BOOKING ID:" << left << setw(44) << bookingID << " |\n";
    certFile << "|                                                           |\n";
    certFile << "+-----------------------------------------------------------+\n";
    certFile << "|                                                           |\n";
    certFile << "|   CERT ID:   " << left << setw(44) << certID << " |\n";
    certFile << "|   STATUS: [ COMPLETED ]                                   |\n";
    certFile << "|   ISSUE DATE:" << left << setw(44) << pay.getCurrentDate() << " |\n";
    certFile << "|                                                           |\n";
    certFile << "|   _______________________         _____________________   |\n";
    certFile << "|    Instructor Signature            Resort Management      |\n";
    certFile << "|                                                           |\n";
    certFile << "+-----------------------------------------------------------+\n";

    certFile.close();

    cout << "\nCertificate generated successfully.\n";
    cout << "Certificate ID: " << certID << endl;
    cout << "Saved at: " << filePath << endl;

    cout << "\nPress Enter to continue...";
    cin.ignore(10000, '\n');
    cin.get();
}
