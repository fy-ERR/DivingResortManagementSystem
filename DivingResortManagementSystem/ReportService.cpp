#include "ReportService.h"
#include <iostream>
#include <iomanip>
#pragma execution_character_set("utf-8")
#ifdef _WIN32
#include <windows.h>
#endif
namespace {
	void enableAnsiColors() {
#ifdef _WIN32
		// Enable ANSI escape sequences in Windows console (Windows 10+)
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE) return;

		DWORD dwMode = 0;
		if (!GetConsoleMode(hOut, &dwMode)) return;

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
#endif
	}

	constexpr const char* CLR_RESET = "\033[0m";
	constexpr const char* CLR_GREEN = "\033[32m";
	constexpr const char* CLR_RED = "\033[31m";
	constexpr const char* CLR_CYAN = "\033[36m";
	constexpr const char* CLR_BOLD = "\033[1m";
}
using namespace std;

ReportService::ReportService(DB& database) : db(database), pay(database) {}

void ReportService::monthlyRevenueReport() {
    const double SCALE = 500.0;
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
    double totalGross = 0.0, totalRefund = 0.0, totalNet = 0.0;

    struct ChartRow { string month; double gross; double refund; };
    vector<ChartRow> chartData;

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

        chartData.push_back({ month, gross, refund });
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
    }

    delete res;
    delete pstmt;

    // ===== Chart (Gross vs Refunds) =====
    enableAnsiColors();

    cout << CLR_BOLD << "+========== Monthly Revenue Chart (Gross vs Refunds) =========+\n" << CLR_RESET;
    cout << "(Scale: 1 symbol = RM 500)\n";
    cout << "Legend: " << CLR_GREEN << "# Gross" << CLR_RESET
        << "  " << CLR_RED << "- Refunds" << CLR_RESET << "\n\n";

    double lastMonthNet = -1;  // sentinel (means no previous month)
    for (const auto& item : chartData) {
        double currentNet = item.gross - item.refund;

        printBar(
            item.month,
            item.gross,
            item.refund,
            lastMonthNet,
            SCALE
        );

        lastMonthNet = currentNet;  // update for next iteration
    }

    cout << "\n+=============================================================+\n";
    cout << "| Report generated on: " << pay.getCurrentDate() << "  \n";
    cout << "+=============================================================+\n";

    cout << "\nPress Enter to continue...";
    cin.ignore(10000, '\n');
    cin.get();
}


void ReportService::printBar(
    const string& label,
    double gross,
    double refund,
    double lastMonthNet,
    double scale
) {
    int grossSymbols = static_cast<int>((gross / scale) + 0.5);
    int refundSymbols = static_cast<int>((refund / scale) + 0.5);

    if (grossSymbols < 0) grossSymbols = 0;
    if (refundSymbols < 0) refundSymbols = 0;

    cout << left << setw(10) << label << ": ";

    cout << CLR_GREEN;
    for (int i = 0; i < grossSymbols; ++i) cout << "#";
    cout << CLR_RESET;

    cout << CLR_RED;
    for (int i = 0; i < refundSymbols; ++i) cout << "-";
    cout << CLR_RESET;

    double net = gross - refund;

    cout << "  (Net RM " << fixed << setprecision(2) << net;

    SetConsoleOutputCP(CP_UTF8); //to print unicode
    if (lastMonthNet > 0) {
        double diffPercent = ((net - lastMonthNet) / lastMonthNet) * 100.0;

        if (diffPercent >= 0) {
            cout << ", " << CLR_GREEN
                << "+" << fixed << setprecision(1) << diffPercent
                << "%" << CLR_RESET << " ↑";
        }
        else {
            cout << ", " << CLR_RED
                << fixed << setprecision(1) << diffPercent
                << "%" << CLR_RESET << " ↓";
        }
    }
    else {
        cout << ", N/A";
    }

    cout << ")\n";
}

