#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <string>
using namespace std;

const int ROWS = 4;
const int COLS = 5;
const double HOURLY_RATE = 50.0;

class Ticket {
public:
    int row, col;
    time_t entryTime;
    time_t exitTime;
    bool active;
    string userID;

    Ticket(int r = -1, int c = -1, string id = "") {
        row = r;
        col = c;
        entryTime = time(0);
        exitTime = 0;
        active = (r != -1 && c != -1);
        userID = id;
    }

    void saveToFile() {
        ofstream file("tickets.txt", ios::app);
        if (file.is_open()) {
            file << "User: " << userID 
                 << " | Slot: Row " << row + 1 << ", Col " << col + 1
                 << " | Entry Time: " << ctime(&entryTime);
            file.close();
        }
    }

    void saveExitToFile() {
        ofstream file("tickets.txt", ios::app);
        if (file.is_open()) {
            double hours = difftime(exitTime, entryTime) / 3600.0;
            if (hours < 1.0) hours = 1.0;
            double fee = hours * HOURLY_RATE;
            file << "User: " << userID 
                 << " | Slot: Row " << row + 1 << ", Col " << col + 1
                 << " | Exit Time: " << ctime(&exitTime)
                 << " | Fee: Rs. " << fixed << setprecision(2) << fee << "\n\n";
            file.close();
        }
    }

    double calculateFee() {
        double hours = difftime(exitTime, entryTime) / 3600.0;
        if (hours < 1.0) hours = 1.0;
        return hours * HOURLY_RATE;
    }
};

class ParkingSystem {
private:
    bool parkingSlots[ROWS][COLS];
    Ticket activeTickets[ROWS][COLS];
    bool isAdminView;
    string currentUserID;

public:
    ParkingSystem() {
        for (int i = 0; i < ROWS; i++)
            for (int j = 0; j < COLS; j++)
                parkingSlots[i][j] = false;
        isAdminView = false;
        currentUserID = "";
    }

    void displayUserSlot() {
        if (currentUserID.empty()) {
            cout << "[!] No user logged in.\n";
            return;
        }

        bool found = false;
        cout << "\n===== YOUR PARKING INFORMATION =====\n";
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (parkingSlots[i][j] && activeTickets[i][j].userID == currentUserID) {
                    cout << "Slot: Row " << i+1 << ", Column " << j+1 << "\n";
                    cout << "Entry Time: " << ctime(&activeTickets[i][j].entryTime);
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        
        if (!found) {
            cout << "No active parking found for user " << currentUserID << "\n";
        }
        cout << "=================================\n";
    }

    void displayMap() {
        cout << "\n========== SMART PARKING MAP ==========\n\n";
        cout << "          ";
        for (int col = 0; col < COLS; ++col)
            cout << "Col " << col + 1 << "  ";
        cout << "\n";

        for (int row = 0; row < ROWS; ++row) {
            cout << "Row " << row + 1 << "    ";
            for (int col = 0; col < COLS; ++col) {
                if (isAdminView) {
                    cout << (parkingSlots[row][col] ? "[B]   " : "[A]   ");
                } else {
                    cout << (parkingSlots[row][col] ? "[X]   " : "[ ]   ");
                }
            }
            cout << "\n";
        }

        cout << "\nLegend: ";
        if (isAdminView)
            cout << "[A] = Available, [B] = Booked";
        else
            cout << "[ ] = Available, [X] = Occupied";
        cout << "\n=======================================\n";
    }

    bool findNearestSlot(int &r, int &c) {
        for (int i = 0; i < ROWS; i++)
            for (int j = 0; j < COLS; j++)
                if (!parkingSlots[i][j]) {
                    r = i;
                    c = j;
                    return true;
                }
        return false;
    }

    void bookSlotManually(string userID) {
        int row, col;
        displayMap();
        cout << "Admin: Enter Row (1-4) for user " << userID << ": ";
        cin >> row;
        cout << "Admin: Enter Column (1-5) for user " << userID << ": ";
        cin >> col;
        row--; col--;

        if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
            cout << "[!] Invalid slot selection.\n";
        } else if (parkingSlots[row][col]) {
            cout << "[!] Slot already booked!\n";
        } else {
            parkingSlots[row][col] = true;
            activeTickets[row][col] = Ticket(row, col, userID);
            activeTickets[row][col].saveToFile();
            cout << "[?] Slot booked for user " << userID << " at Row " << row + 1 << ", Column " << col + 1 << "\n";
        }
    }

    void bookNearestSlot(string userID) {
        int row, col;
        if (findNearestSlot(row, col)) {
            parkingSlots[row][col] = true;
            activeTickets[row][col] = Ticket(row, col, userID);
            activeTickets[row][col].saveToFile();
            cout << "[?] Nearest Slot Booked for user " << userID << " at Row " << row + 1 << ", Column " << col + 1 << "\n";
        } else {
            cout << "[!] No available slots!\n";
        }
    }

    void userBooking() {
        if (currentUserID.empty()) {
            cout << "Enter your user ID: ";
            cin >> currentUserID;
        }

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (parkingSlots[i][j] && activeTickets[i][j].userID == currentUserID) {
                    cout << "[!] You already have an active booking at Row " << i+1 << ", Column " << j+1 << "\n";
                    return;
                }
            }
        }

        int choice;
        cout << "\nBooking Options:\n1. Book Slot Manually\n2. Book Nearest Slot Automatically\nChoose option: ";
        cin >> choice;

        if (choice == 1) {
            cout << "[?] Your manual booking request has been sent to admin.\n";
            cout << "Admin is processing your manual booking request...\n";
            bookSlotManually(currentUserID);
        } else if (choice == 2) {
            cout << "[?] Processing automatic booking...\n";
            bookNearestSlot(currentUserID);
        } else {
            cout << "[!] Invalid choice.\n";
        }
    }

    void userExitAndBill() {
        if (currentUserID.empty()) {
            cout << "[!] No user logged in.\n";
            return;
        }

        bool found = false;
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (parkingSlots[i][j] && activeTickets[i][j].userID == currentUserID) {
                    parkingSlots[i][j] = false;
                    activeTickets[i][j].exitTime = time(0);

                    double fee = activeTickets[i][j].calculateFee();
                    activeTickets[i][j].saveExitToFile();

                    cout << fixed << setprecision(2);
                    cout << "[?] Your slot at Row " << i + 1 << ", Column " << j + 1 << " is now free.\n";
                    cout << "Total parking fee: Rs. " << fee << "\n";
                    found = true;
                    break;
                }
            }
            if (found) break;
        }

        if (!found) {
            cout << "[!] No active parking found for user " << currentUserID << "\n";
        }
    }

    void cancelSlot() {
        int row, col;
        cout << "Enter Row (1-4) to cancel: ";
        cin >> row;
        cout << "Enter Column (1-5) to cancel: ";
        cin >> col;
        row--; col--;

        if (row < 0 || row >= ROWS || col < 0 || col >= COLS || !parkingSlots[row][col]) {
            cout << "[!] Invalid or unoccupied slot.\n";
        } else {
            cout << "Cancelling slot for user " << activeTickets[row][col].userID << "\n";
            parkingSlots[row][col] = false;
            activeTickets[row][col].exitTime = time(0);
            activeTickets[row][col].saveExitToFile();
            cout << "[?] Slot at Row " << row + 1 << ", Column " << col + 1 << " cancelled by Admin.\n";
        }
    }

    void resetTicketsFile() {
        ofstream file("tickets.txt", ios::trunc);
        if (file.is_open()) {
            file.close();
            cout << "[?] Ticket history cleared.\n";
        }
    }

    void searchTicketsByDate() {
        string date;
        cout << "Enter date (e.g., Jun 06): ";
        cin.ignore();
        getline(cin, date);

        ifstream file("tickets.txt");
        string line;
        bool found = false;

        cout << "\n=== Tickets on " << date << " ===\n";

        while (getline(file, line)) {
            if (line.find(date) != string::npos) {
                cout << line << endl;
                found = true;
            }
        }

        if (!found)
            cout << "[!] No tickets found on this date.\n";
        file.close();
    }

    void adminPanel() {
        isAdminView = true;
        char continueOption;
        do {
            int choice;
            displayMap();
            cout << "\nAdmin Options:\n1. Cancel Slot\n2. Clear Ticket History\n3. Search Tickets by Date\n4. Exit Admin Panel\nEnter choice: ";
            cin >> choice;

            switch (choice) {
                case 1: cancelSlot(); break;
                case 2: resetTicketsFile(); break;
                case 3: searchTicketsByDate(); break;
                case 4: {
                    isAdminView = false;
                    return;
                }
                default: cout << "[!] Invalid choice.\n";
            }

            cout << "\nDo you want to perform another admin operation? (y/n): ";
            cin >> continueOption;
        } while (continueOption == 'y' || continueOption == 'Y');
        
        isAdminView = false;
    }
};

int main() {
    string password;
    int role;
    ParkingSystem system;

    while (true) {
        cout << "\n===== SMART PARKING ASSISTANT =====\n";
        cout << "1. Login as Admin\n2. Login as User\n3. Exit\nEnter choice: ";
        cin >> role;

        if (role == 1) {
            cout << "Enter Admin Password: ";
            cin >> password;
            if (password == "admin123") {
                system.adminPanel();
            } else {
                cout << "[!] Incorrect password.\n";
            }
        } else if (role == 2) {
            int choice;
            do {
                cout << "\nUser Menu:\n1. Book Slot\n2. Exit & Generate Bill\n3. View My Booking\n4. Back to Main Menu\nEnter choice: ";
                cin >> choice;
                switch (choice) {
                    case 1: system.userBooking(); break;
                    case 2: system.userExitAndBill(); break;
                    case 3: system.displayUserSlot(); break;
                    case 4: break;
                    default: cout << "[!] Invalid choice.\n";
                }
            } while (choice != 4);
        } else if (role == 3) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "[!] Invalid option. Try again.\n";
        }
    }

    return 0;
}

