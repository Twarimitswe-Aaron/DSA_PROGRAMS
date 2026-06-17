/*
 * ============================================================================
 * FILE: 30_cinema_booking.cpp
 * ============================================================================
 * CINEMA/MOVIE TICKET BOOKING SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Very High. Movie ticket booking is a classic DSA practical
 * exam problem. Tests 2D arrays (seat grid), queues (waitlist), hash tables
 * (movie/screening lookup), stacks (booking undo), and linked lists (bookings).
 * DataFlair and multiple GitHub repos confirm this is a top DSA project.
 *
 * DATA STRUCTURES USED:
 *   1. 2D Array (vector of vectors) - Seat Layout Grid
 *      - Rows x Columns seat matrix
 *      - O(1) seat status check by [row][col]
 *      - Perfect for grid-based seat visualization
 *   2. Queue - Booking Waitlist (FIFO)
 *      - If seats unavailable, customers join waitlist
 *      - Processed in order when seats free up
 *   3. Hash Table (unordered_map) - Movie & Screening Lookup
 *      - O(1) movie search by ID
 *      - O(1) screening lookup by code
 *   4. Stack - Booking History / Undo (LIFO)
 *      - Cancel most recent booking
 *   5. Linked List - Customer Booking Records
 *      - Each booking node stores customer + seat info
 *
 * FUNCTIONALITIES:
 *   - Add movies & screenings (hash table)
 *   - Display seat availability (2D array grid)
 *   - Book tickets (seat allocation + booking linked list)
 *   - Cancel booking (stack: undo recent, linked list: remove record)
 *   - Join waitlist (queue) / Process waitlist (FIFO)
 *   - View booking history (stack: most recent first)
 *   - Search movies by ID (hash table O(1))
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <iomanip>
#include <sstream>
#include <cstdlib>
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTS
// ============================================================================
struct Movie {
    string movieId;
    string title;
    string genre;
    int duration;       // minutes
    double rating;

    Movie() : duration(0), rating(0.0) {}
    Movie(string id, string t, string g, int d, double r)
        : movieId(id), title(t), genre(g), duration(d), rating(r) {}
};

struct Screening {
    string screeningId;   // e.g. "SCR001"
    string movieId;
    string time;
    int screenNum;
    int rows;
    int cols;

    Screening() : screenNum(0), rows(0), cols(0) {}
    Screening(string sid, string mid, string t, int sn, int r, int c)
        : screeningId(sid), movieId(mid), time(t),
          screenNum(sn), rows(r), cols(c) {}
};

struct Seat {
    int row;
    int col;
    string label;         // e.g. "A1", "B12"
    bool booked;

    Seat() : row(0), col(0), booked(false) {}
    Seat(int r, int c, string l) : row(r), col(c), label(l), booked(false) {}
};

struct Booking {
    string bookingId;
    string customerName;
    string screeningId;
    string movieTitle;
    string seatLabel;
    string time;

    Booking() {}
    Booking(string bid, string cn, string sid, string mt, string sl, string t)
        : bookingId(bid), customerName(cn), screeningId(sid),
          movieTitle(mt), seatLabel(sl), time(t) {}
};

// Waitlist entry
struct WaitlistEntry {
    string customerName;
    string screeningId;
    int requestedSeats;   // How many seats they want

    WaitlistEntry() : requestedSeats(1) {}
    WaitlistEntry(string cn, string sid, int rs)
        : customerName(cn), screeningId(sid), requestedSeats(rs) {}
};

// ============================================================================
// SECTION 2: BOOKING LINKED LIST
//
// Why Linked List?
//   - Dynamic growth as bookings made
//   - O(1) insertion/removal at head
//   - Easy traversal for reporting
// ============================================================================
struct BookingNode {
    Booking booking;
    BookingNode* next;

    BookingNode(Booking b) : booking(b), next(nullptr) {}
};

class BookingList {
private:
    BookingNode* head;
    int count;

public:
    BookingList() : head(nullptr), count(0) {}

    ~BookingList() {
        BookingNode* curr = head;
        while (curr) {
            BookingNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    void addBooking(Booking b) {
        BookingNode* node = new BookingNode(b);
        node->next = head;
        head = node;
        count++;
    }

    bool removeBooking(string bookingId) {
        BookingNode* curr = head;
        BookingNode* prev = nullptr;
        while (curr) {
            if (curr->booking.bookingId == bookingId) {
                if (prev) prev->next = curr->next;
                else head = curr->next;
                delete curr;
                count--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    BookingNode* find(string bookingId) {
        BookingNode* curr = head;
        while (curr) {
            if (curr->booking.bookingId == bookingId) return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    void displayAll() {
        if (!head) {
            cout << "    [No bookings]\n";
            return;
        }
        BookingNode* curr = head;
        int idx = 1;
        while (curr) {
            cout << "    " << idx++ << ". " << curr->booking.bookingId
                 << " | " << curr->booking.customerName
                 << " | " << curr->booking.movieTitle
                 << " | Seat " << curr->booking.seatLabel
                 << " | " << curr->booking.time << "\n";
            curr = curr->next;
        }
    }

    int getCount() { return count; }
    bool isEmpty() { return head == nullptr; }
};

// ============================================================================
// SECTION 3: MAIN CINEMA SYSTEM
// ============================================================================
class CinemaSystem {
private:
    unordered_map<string, Movie> movies;            // Hash: movieId -> Movie
    unordered_map<string, Screening> screenings;    // Hash: scrId -> Screening
    unordered_map<string, vector<vector<Seat>>> seatMaps; // scrId -> 2D seat grid
    BookingList bookings;                           // Linked list of Bookings
    stack<Booking> bookingHistory;                  // Stack: undo/cancel recent
    queue<WaitlistEntry> waitlist;                  // Queue: FIFO waitlist
    int nextBookingNum;

public:
    CinemaSystem() : nextBookingNum(1001) {
        seedData();
    }

    void seedData() {
        // Movies
        movies["MOV01"] = Movie("MOV01", "Inception", "Sci-Fi", 148, 8.8);
        movies["MOV02"] = Movie("MOV02", "The Dark Knight", "Action", 152, 9.0);
        movies["MOV03"] = Movie("MOV03", "Interstellar", "Sci-Fi", 169, 8.7);
        movies["MOV04"] = Movie("MOV04", "Parasite", "Thriller", 132, 8.6);
        movies["MOV05"] = Movie("MOV05", "Toy Story 3", "Animation", 103, 8.3);

        // Screenings
        screenings["SCR01"] = Screening("SCR01", "MOV01", "10:30 AM", 1, 5, 8);
        screenings["SCR02"] = Screening("SCR02", "MOV02", "1:00 PM", 2, 5, 8);
        screenings["SCR03"] = Screening("SCR03", "MOV03", "4:30 PM", 1, 5, 8);
        screenings["SCR04"] = Screening("SCR04", "MOV04", "7:00 PM", 3, 5, 8);
        screenings["SCR05"] = Screening("SCR05", "MOV05", "9:30 PM", 2, 5, 8);

        // Initialize seat maps (2D grid) for each screening
        for (auto& pair : screenings) {
            string sid = pair.first;
            Screening& s = pair.second;
            vector<vector<Seat>> grid(s.rows, vector<Seat>(s.cols));
            for (int r = 0; r < s.rows; r++) {
                for (int c = 0; c < s.cols; c++) {
                    string label = string(1, 'A' + r) + to_string(c + 1);
                    grid[r][c] = Seat(r, c, label);
                }
            }
            seatMaps[sid] = grid;
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY SCREENINGS: Hash table lookup O(n)
    // -------------------------------------------------------------------
    void showScreenings() {
        cout << "  Available Screenings:\n";
        cout << "  " << left << setw(8) << "Code"
             << setw(25) << "Movie"
             << setw(12) << "Time"
             << setw(8) << "Screen"
             << "Seats\n";
        cout << "  " << string(60, '-') << "\n";
        for (auto& pair : screenings) {
            Screening& s = pair.second;
            auto mit = movies.find(s.movieId);
            string title = (mit != movies.end()) ? mit->second.title : "Unknown";
            int avail = countAvailable(s.screeningId);
            cout << "  " << left << setw(8) << s.screeningId
                 << setw(25) << title
                 << setw(12) << s.time
                 << setw(8) << ("Screen " + to_string(s.screenNum))
                 << avail << "/" << (s.rows * s.cols) << "\n";
        }
    }

    // -------------------------------------------------------------------
    // COUNT AVAILABLE: O(r*c) scan of 2D seat grid
    // -------------------------------------------------------------------
    int countAvailable(string screeningId) {
        auto it = seatMaps.find(screeningId);
        if (it == seatMaps.end()) return 0;
        int count = 0;
        for (auto& row : it->second) {
            for (auto& seat : row) {
                if (!seat.booked) count++;
            }
        }
        return count;
    }

    // -------------------------------------------------------------------
    // DISPLAY SEAT GRID: 2D Array visualization
    // -------------------------------------------------------------------
    void showSeats(string screeningId) {
        auto sit = screenings.find(screeningId);
        if (sit == screenings.end()) {
            cout << "  [ERROR] Screening not found.\n";
            return;
        }
        auto mit = movies.find(sit->second.movieId);
        string title = (mit != movies.end()) ? mit->second.title : "Unknown";

        cout << "  Screen " << sit->second.screenNum
             << " | " << title
             << " | " << sit->second.time << "\n";
        cout << "  Seat Layout (2D Grid):\n  ";
        // Column headers
        for (int c = 0; c < sit->second.cols; c++)
            cout << "  " << (c + 1) << " ";
        cout << "\n";

        auto& grid = seatMaps[screeningId];
        for (int r = 0; r < sit->second.rows; r++) {
            cout << "  " << (char)('A' + r) << " ";
            for (int c = 0; c < sit->second.cols; c++) {
                cout << "[" << (grid[r][c].booked ? "X" : "O") << "]";
            }
            cout << "\n";
        }
        cout << "  Legend: [O] Available  [X] Booked\n";
    }

    // -------------------------------------------------------------------
    // BOOK TICKET: Allocate seat, add to linked list + history stack
    // -------------------------------------------------------------------
    bool bookTicket(string screeningId, string customerName, int row, int col) {
        auto sit = screenings.find(screeningId);
        if (sit == screenings.end()) {
            cout << "  [ERROR] Screening not found.\n";
            return false;
        }
        if (row < 0 || row >= sit->second.rows ||
            col < 0 || col >= sit->second.cols) {
            cout << "  [ERROR] Invalid seat position.\n";
            return false;
        }
        auto& grid = seatMaps[screeningId];
        if (grid[row][col].booked) {
            cout << "  [INFO] Seat already booked. Adding to waitlist...\n";
            waitlist.push(WaitlistEntry(customerName, screeningId, 1));
            cout << "  [OK] " << customerName << " added to waitlist.\n";
            return false;
        }

        grid[row][col].booked = true;
        string seatLabel = grid[row][col].label;
        auto mit = movies.find(sit->second.movieId);
        string movieTitle = (mit != movies.end()) ? mit->second.title : "Unknown";
        string bookingId = "BK" + to_string(nextBookingNum++);

        Booking b(bookingId, customerName, screeningId, movieTitle, seatLabel, sit->second.time);
        bookings.addBooking(b);          // Linked list
        bookingHistory.push(b);          // Stack: for undo

        cout << "  [BOOKED] " << bookingId << " | " << customerName
             << " | " << movieTitle << " | Seat " << seatLabel
             << " | " << sit->second.time << "\n";
        return true;
    }

    // -------------------------------------------------------------------
    // CANCEL BOOKING: Undo via stack + linked list removal
    // -------------------------------------------------------------------
    void cancelLastBooking() {
        if (bookingHistory.empty()) {
            cout << "  [INFO] No bookings to cancel.\n";
            return;
        }
        Booking last = bookingHistory.top();
        bookingHistory.pop();

        // Remove from linked list
        bookings.removeBooking(last.bookingId);

        // Free the seat in 2D grid
        auto sit = screenings.find(last.screeningId);
        if (sit != screenings.end()) {
            auto& grid = seatMaps[last.screeningId];
            for (int r = 0; r < sit->second.rows; r++) {
                for (int c = 0; c < sit->second.cols; c++) {
                    if (grid[r][c].label == last.seatLabel) {
                        grid[r][c].booked = false;
                        break;
                    }
                }
            }
        }

        cout << "  [CANCELLED] " << last.bookingId
             << " (" << last.customerName << " - " << last.movieTitle
             << " Seat " << last.seatLabel << ")\n";

        // Process waitlist (FIFO)
        processWaitlist(last.screeningId);
    }

    // -------------------------------------------------------------------
    // PROCESS WAITLIST: Queue - serve FIFO when seats free up
    // -------------------------------------------------------------------
    void processWaitlist(string screeningId) {
        if (waitlist.empty()) return;
        queue<WaitlistEntry> temp;
        bool assigned = false;
        while (!waitlist.empty()) {
            WaitlistEntry w = waitlist.front();
            waitlist.pop();
            if (w.screeningId == screeningId && !assigned) {
                // Try to auto-assign first available seat
                auto& grid = seatMaps[screeningId];
                for (size_t r = 0; r < grid.size(); r++) {
                    for (size_t c = 0; c < grid[r].size(); c++) {
                        if (!grid[r][c].booked) {
                            cout << "  [WAITLIST] Assigning " << w.customerName
                                 << " to " << grid[r][c].label << "\n";
                            bookTicket(screeningId, w.customerName, r, c);
                            assigned = true;
                            break;
                        }
                    }
                    if (assigned) break;
                }
                if (!assigned) temp.push(w);
            } else {
                temp.push(w);
            }
        }
        // Restore unprocessed waitlist
        while (!temp.empty()) {
            waitlist.push(temp.front());
            temp.pop();
        }
    }

    // -------------------------------------------------------------------
    // SHOW WAITLIST: Queue display
    // -------------------------------------------------------------------
    void showWaitlist() {
        if (waitlist.empty()) {
            cout << "  [INFO] Waitlist is empty.\n";
            return;
        }
        queue<WaitlistEntry> temp = waitlist;
        cout << "  Waitlist (FIFO):\n";
        int i = 1;
        while (!temp.empty()) {
            WaitlistEntry w = temp.front();
            temp.pop();
            cout << "  " << i++ << ". " << w.customerName
                 << " (" << w.screeningId << ")\n";
        }
    }

    // -------------------------------------------------------------------
    // BOOKING HISTORY: Stack display (most recent first)
    // -------------------------------------------------------------------
    void showHistory() {
        if (bookingHistory.empty()) {
            cout << "  [INFO] No booking history.\n";
            return;
        }
        stack<Booking> temp = bookingHistory;
        cout << "  Recent Bookings (most recent first):\n";
        while (!temp.empty()) {
            Booking b = temp.top();
            temp.pop();
            cout << "  " << b.bookingId << " | " << b.customerName
                 << " | " << b.movieTitle << " | Seat " << b.seatLabel << "\n";
        }
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   CINEMA TICKET BOOKING SYSTEM\n";
        cout << "   Data Structures: 2D Array (Seats),\n";
        cout << "   Hash Table (Movies), Linked List (Bookings),\n";
        cout << "   Stack (History/Undo), Queue (Waitlist)\n";
        cout << "=============================================\n\n";

        int choice, row, col;
        string sid, custName;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [SCREENINGS - Hash Table]\n";
            cout << "    1. Show All Screenings\n";
            cout << "  [SEATS - 2D Array]\n";
            cout << "    2. Show Seat Layout (Grid)\n";
            cout << "    3. Book Ticket (2D + LL + Stack)\n";
            cout << "    4. Cancel Last Booking (Stack Undo)\n";
            cout << "  [BOOKINGS - Linked List]\n";
            cout << "    5. All Booking Records\n";
            cout << "  [HISTORY - Stack]\n";
            cout << "    6. Booking History (Most Recent)\n";
            cout << "  [WAITLIST - Queue]\n";
            cout << "    7. Show Waitlist\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: showScreenings(); break;
                case 2:
                    cout << "  Screening Code: "; getline(cin, sid);
                    showSeats(sid);
                    break;
                case 3:
                    cout << "  Screening Code: "; getline(cin, sid);
                    cout << "  Customer Name: "; getline(cin, custName);
                    cout << "  Row (0-" << (seatMaps[sid].empty() ? 0 : (int)seatMaps[sid].size() - 1) << "): ";
                    cin >> row;
                    cout << "  Col (0-" << (seatMaps[sid].empty() ? 0 : (int)seatMaps[sid][0].size() - 1) << "): ";
                    cin >> col; cin.ignore();
                    bookTicket(sid, custName, row, col);
                    break;
                case 4: cancelLastBooking(); break;
                case 5:
                    cout << "  All Booking Records (Linked List):\n";
                    bookings.displayAll();
                    break;
                case 6: showHistory(); break;
                case 7: showWaitlist(); break;
                case 0: cout << "  Enjoy the movie! Goodbye.\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    CinemaSystem cinema;
    cinema.run();
    return 0;
}
