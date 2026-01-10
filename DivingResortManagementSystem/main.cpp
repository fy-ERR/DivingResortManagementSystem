#include <iostream>
#include "DB.h"
#include "Booking.h"

using namespace std;

int main() {
    DB db;
    Booking booking(db);

    int choice;
    do {
        cout << "\n+=======================================+\n";
        cout << "|        DIVING RESORT ADMIN SYSTEM     |\n";
        cout << "+=======================================+\n\n";

        cout << "  [1]  Add new booking\n";
        cout << "  [2]  Search booking record\n";
        cout << "  [3]  Cancel booking\n";
        cout << "  [4]  View instructor schedule\n";
        cout << "  [5]  Generate certificate\n";
        cout << "  [6]  Monthly revenue report\n";
        cout << "  [0]  Exit\n";

        cout << "\n-----------------------------------------\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            continue;
        }

        switch (choice) {
        case 1: booking.addBooking(); break;
        case 2: booking.searchBooking(); break;
        case 3: booking.cancelBooking(); break;
        case 4: booking.viewInstructorSchedule(); break;
        case 5: booking.generateCertificate(); break;
        case 6: booking.monthlyRevenueReport(); break;
        case 0: cout << "Bye!\n"; break;
        default: cout << "Invalid choice.\n"; break;
        }
    } while (choice != 0);

    return 0;
}
