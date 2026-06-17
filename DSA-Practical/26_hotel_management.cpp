/*
 * ============================================================================
 * FILE: 26_hotel_management.cpp
 * ============================================================================
 * HOTEL MANAGEMENT SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: High. Hotel management is a classic DSA practical exam
 * topic. Tests queues (reservations), BST (room management), hash tables
 * (guest lookup), and stacks (checkout/billing history).
 *
 * DATA STRUCTURES USED:
 *   1. Queue (FIFO) - Reservation requests
 *      - Booking requests processed in arrival order
 *   2. Binary Search Tree (BST) - Room inventory by room number
 *      - O(log n) search for room assignment and status checks
 *   3. Hash Table (unordered_map) - Guest lookup by booking ID
 *      - O(1) retrieval of guest details and billing info
 *   4. Stack - Checkout history / billing (LIFO)
 *      - Most recent checkout viewed first for audit
 *   5. Singly Linked List - Bill breakdown per guest/room
 *      - Itemized charges (room, meals, services)
 *
 * FUNCTIONALITIES:
 *   - Add/manage rooms (BST by room number)
 *   - Process reservation requests (queue - FIFO)
 *   - Check-in guests (assign room from BST)
 *   - Check-out with bill generation (linked list of charges)
 *   - View checkout history (stack)
 *   - Search guest by booking ID (hash table)
 *   - Display room availability
 *   - Room service ordering
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <sstream>
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// BILL ITEM (Linked List Node)
// Each billed item is a node in a singly linked list per stay.
// -------------------------------------------------------------------
struct BillItem {
    string description;     // e.g., "Room Charge", "Room Service", "Mini Bar"
    double amount;          // Cost of this item
    BillItem* next;         // Next bill item

    BillItem(string desc, double amt)
        : description(desc), amount(amt), next(nullptr) {}
};

class BillList {
private:
    BillItem* head;
    int count;

public:
    BillList() : head(nullptr), count(0) {}
    ~BillList() {
        BillItem* current = head;
        while (current) {
            BillItem* temp = current;
            current = current->next;
            delete temp;
        }
    }

    void addItem(string desc, double amount) {
        BillItem* item = new BillItem(desc, amount);
        item->next = head;
        head = item;
        count++;
    }

    double getTotal() {
        double total = 0;
        BillItem* current = head;
        while (current) {
            total += current->amount;
            current = current->next;
        }
        return total;
    }

    void display() {
        if (!head) {
            cout << "    No charges.\n";
            return;
        }
        cout << "    " << left << setw(35) << "Description"
             << "Amount\n";
        cout << "    " << string(50, '-') << "\n";
        BillItem* current = head;
        while (current) {
            cout << "    " << left << setw(35) << current->description
                 << "$" << fixed << setprecision(2) << current->amount << "\n";
            current = current->next;
        }
        cout << "    " << string(50, '-') << "\n";
        cout << "    " << left << setw(35) << "TOTAL"
             << "$" << getTotal() << "\n";
    }
};

// -------------------------------------------------------------------
// GUEST STRUCTURE
// Stored in hash table by booking ID for O(1) lookup.
// -------------------------------------------------------------------
struct Guest {
    string bookingId;       // Unique booking reference
    string name;            // Guest name
    string phone;           // Contact number
    string email;           // Email address
    int roomNumber;         // Assigned room (-1 if not checked in)
    string checkIn;         // Check-in date
    string checkOut;        // Check-out date (empty if still checked in)
    BillList charges;       // Linked list of bill items

    Guest() : roomNumber(-1) {}
    Guest(string bid, string n, string p, string e)
        : bookingId(bid), name(n), phone(p), email(e),
          roomNumber(-1) {
        time_t now = time(0);
        checkIn = ctime(&now);
        if (!checkIn.empty() && checkIn.back() == '\n')
            checkIn.pop_back();
    }
};

// -------------------------------------------------------------------
// ROOM STRUCTURE
// Stored in BST by room number for O(log n) availability checks.
// -------------------------------------------------------------------
struct Room {
    int roomNumber;         // Room number (key)
    string type;            // Single, Double, Suite, Penthouse
    double rate;            // Nightly rate
    int capacity;           // Max guests
    bool isAvailable;       // Available for booking
    string currentGuest;    // Current guest name (empty if vacant)

    Room() : roomNumber(0), rate(0.0), capacity(0), isAvailable(true) {}
    Room(int num, string t, double r, int cap)
        : roomNumber(num), type(t), rate(r),
          capacity(cap), isAvailable(true) {}

    void display() {
        cout << "  " << left << setw(8) << roomNumber
             << setw(15) << type
             << setw(6) << capacity
             << "$" << fixed << setprecision(2) << setw(8) << rate
             << (isAvailable ? "Available" : "Occupied")
             << (isAvailable ? "" : (" by " + currentGuest))
             << "\n";
    }
};

// ============================================================================
// SECTION 2: BST FOR ROOM INVENTORY
//
// Why BST? Rooms are searched by room number and we need to display
// them in sorted order. BST provides O(log n) search and O(n) in-order
// traversal for sorted room listing.
// ============================================================================

struct RoomNode {
    Room room;
    RoomNode* left;
    RoomNode* right;

    RoomNode(Room r) : room(r), left(nullptr), right(nullptr) {}
};

class RoomBST {
private:
    RoomNode* root;

    RoomNode* insert(RoomNode* node, Room r) {
        if (!node) return new RoomNode(r);
        if (r.roomNumber < node->room.roomNumber)
            node->left = insert(node->left, r);
        else if (r.roomNumber > node->room.roomNumber)
            node->right = insert(node->right, r);
        return node;
    }

    RoomNode* search(RoomNode* node, int roomNum) {
        if (!node || node->room.roomNumber == roomNum) return node;
        if (roomNum < node->room.roomNumber)
            return search(node->left, roomNum);
        return search(node->right, roomNum);
    }

    void inorderDisplay(RoomNode* node) {
        if (!node) return;
        inorderDisplay(node->left);
        node->room.display();
        inorderDisplay(node->right);
    }

    void destroy(RoomNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    RoomBST() : root(nullptr) {}
    ~RoomBST() { destroy(root); }

    void addRoom(Room r) {
        if (search(root, r.roomNumber)) {
            cout << "  [ERROR] Room " << r.roomNumber << " already exists.\n";
            return;
        }
        root = insert(root, r);
        cout << "  [OK] Room " << r.roomNumber
             << " (" << r.type << ") added. Rate: $" << r.rate << "/night\n";
    }

    Room* findRoom(int roomNum) {
        RoomNode* node = search(root, roomNum);
        if (node) return &node->room;
        return nullptr;
    }

    bool assignRoom(int roomNum, string guestName) {
        Room* r = findRoom(roomNum);
        if (!r) {
            cout << "  [ERROR] Room " << roomNum << " not found.\n";
            return false;
        }
        if (!r->isAvailable) {
            cout << "  [ERROR] Room " << roomNum << " is already occupied.\n";
            return false;
        }
        r->isAvailable = false;
        r->currentGuest = guestName;
        cout << "  [OK] Room " << roomNum << " assigned to " << guestName << ".\n";
        return true;
    }

    void vacateRoom(int roomNum) {
        Room* r = findRoom(roomNum);
        if (!r) {
            cout << "  [ERROR] Room " << roomNum << " not found.\n";
            return;
        }
        r->isAvailable = true;
        r->currentGuest = "";
        cout << "  [OK] Room " << roomNum << " is now available.\n";
    }

    void displayAvailable() {
        cout << "  Available Rooms:\n";
        cout << "  " << left << setw(8) << "Room"
             << setw(15) << "Type"
             << setw(6) << "Cap."
             << "Rate\n";
        cout << "  " << string(42, '-') << "\n";
        displayAvailableHelper(root);
    }

    void displayAvailableHelper(RoomNode* node) {
        if (!node) return;
        displayAvailableHelper(node->left);
        if (node->room.isAvailable) node->room.display();
        displayAvailableHelper(node->right);
    }

    void displayAll() {
        if (!root) {
            cout << "  [INFO] No rooms in system.\n";
            return;
        }
        cout << "  " << left << setw(8) << "Room"
             << setw(15) << "Type"
             << setw(6) << "Cap."
             << setw(10) << "Rate"
             << "Status\n";
        cout << "  " << string(65, '-') << "\n";
        inorderDisplay(root);
    }
};

// ============================================================================
// SECTION 3: MAIN HOTEL SYSTEM
// ============================================================================

class HotelSystem {
private:
    RoomBST rooms;                              // BST: room# -> Room
    unordered_map<string, Guest> guests;        // Hash: bookingId -> Guest
    queue<string> reservationQueue;             // Queue: booking IDs waiting
    stack<string> checkoutHistory;              // Stack: recent checkouts
    int bookingCounter;

public:
    HotelSystem() : bookingCounter(100) {
        // Seed rooms
        rooms.addRoom(Room(101, "Single", 100.00, 1));
        rooms.addRoom(Room(102, "Single", 100.00, 1));
        rooms.addRoom(Room(201, "Double", 150.00, 2));
        rooms.addRoom(Room(202, "Double", 150.00, 2));
        rooms.addRoom(Room(301, "Suite", 250.00, 3));
        rooms.addRoom(Room(302, "Suite", 250.00, 3));
        rooms.addRoom(Room(401, "Penthouse", 500.00, 4));
    }

    string generateBookingId() {
        stringstream ss;
        ss << "BK" << bookingCounter++;
        return ss.str();
    }

    // -------------------------------------------------------------------
    // RESERVATION: Create booking, add to queue. O(1).
    // -------------------------------------------------------------------
    void makeReservation(string name, string phone, string email) {
        string bid = generateBookingId();
        Guest g(bid, name, phone, email);
        guests[bid] = g;
        reservationQueue.push(bid);
        cout << "  [OK] Reservation created. Booking ID: " << bid
             << " | Guest: " << name << "\n";
        cout << "  [INFO] Added to processing queue at position "
             << reservationQueue.size() << ".\n";
    }

    // -------------------------------------------------------------------
    // PROCESS RESERVATION: Dequeue -> assign room. FIFO order. O(log n).
    // -------------------------------------------------------------------
    void processReservation() {
        if (reservationQueue.empty()) {
            cout << "  [INFO] No pending reservations.\n";
            return;
        }

        string bid = reservationQueue.front();
        reservationQueue.pop();

        auto it = guests.find(bid);
        if (it == guests.end()) {
            cout << "  [ERROR] Booking " << bid << " not found.\n";
            return;
        }

        Guest& guest = it->second;
        cout << "  Processing reservation for " << guest.name
             << " (Booking: " << bid << ")...\n";

        // Ask which room
        int roomNum;
        rooms.displayAvailable();
        cout << "  Enter room number: ";
        cin >> roomNum;
        cin.ignore();

        if (rooms.assignRoom(roomNum, guest.name)) {
            guest.roomNumber = roomNum;
        }
    }

    // -------------------------------------------------------------------
    // CHECK-OUT: Vacate room, generate bill, push to stack. O(log n).
    // -------------------------------------------------------------------
    void checkOut(string bookingId) {
        auto it = guests.find(bookingId);
        if (it == guests.end()) {
            cout << "  [ERROR] Booking " << bookingId << " not found.\n";
            return;
        }

        Guest& guest = it->second;
        if (guest.roomNumber == -1) {
            cout << "  [ERROR] Guest has not checked in.\n";
            return;
        }

        // Set checkout time
        time_t now = time(0);
        guest.checkOut = ctime(&now);
        if (!guest.checkOut.empty() && guest.checkOut.back() == '\n')
            guest.checkOut.pop_back();

        // Add room charge to bill
        Room* r = rooms.findRoom(guest.roomNumber);
        if (r) {
            guest.charges.addItem("Room Charge (" + r->type + ")", r->rate);
        }

        cout << "\n=========================================\n";
        cout << "   CHECK-OUT BILL\n";
        cout << "   Booking: " << bookingId << "\n";
        cout << "   Guest: " << guest.name << "\n";
        cout << "   Room: " << guest.roomNumber << "\n";
        cout << "   Check-in: " << guest.checkIn << "\n";
        cout << "   Check-out: " << guest.checkOut << "\n";
        cout << "=========================================\n";
        guest.charges.display();
        cout << "=========================================\n";

        rooms.vacateRoom(guest.roomNumber);
        guest.roomNumber = -1;
        checkoutHistory.push(bookingId);
    }

    // -------------------------------------------------------------------
    // ADD CHARGE TO GUEST BILL: O(1) hash table + O(1) linked list.
    // -------------------------------------------------------------------
    void addCharge(string bookingId, string desc, double amount) {
        auto it = guests.find(bookingId);
        if (it == guests.end()) {
            cout << "  [ERROR] Booking " << bookingId << " not found.\n";
            return;
        }
        it->second.charges.addItem(desc, amount);
        cout << "  [OK] Charge added: " << desc << " ($" << amount << ")\n";
    }

    // -------------------------------------------------------------------
    // VIEW CHECKOUT HISTORY: Stack - most recent first. O(n).
    // -------------------------------------------------------------------
    void viewCheckoutHistory() {
        if (checkoutHistory.empty()) {
            cout << "  [INFO] No checkout history.\n";
            return;
        }
        stack<string> temp = checkoutHistory;
        cout << "  Recent checkouts (most recent first):\n";
        int i = 1;
        while (!temp.empty()) {
            string bid = temp.top();
            temp.pop();
            auto it = guests.find(bid);
            if (it != guests.end()) {
                cout << "  " << i++ << ". " << bid
                     << " - " << it->second.name << "\n";
            }
        }
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   HOTEL MANAGEMENT SYSTEM\n";
        cout << "   Data Structures: BST, Queue, Hash Table,\n";
        cout << "   Stack, Linked List\n";
        cout << "=============================================\n\n";

        int choice, roomNum, cap;
        string name, phone, email, bid, type, desc;
        double rate, amount;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [ROOMS - BST]\n";
            cout << "    1. Add Room\n";
            cout << "    2. Display All Rooms\n";
            cout << "    3. Display Available Rooms\n";
            cout << "  [RESERVATIONS - Queue]\n";
            cout << "    4. Make Reservation\n";
            cout << "    5. Process Next Reservation (FIFO)\n";
            cout << "  [GUESTS - Hash Table]\n";
            cout << "    6. Search Guest by Booking ID\n";
            cout << "    7. Check-In Guest\n";
            cout << "    8. Check-Out Guest\n";
            cout << "    9. Add Charge to Bill\n";
            cout << "  [HISTORY - Stack]\n";
            cout << "   10. View Checkout History (Stack)\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Room Number: "; cin >> roomNum; cin.ignore();
                    cout << "  Type (Single/Double/Suite): "; getline(cin, type);
                    cout << "  Nightly Rate ($): "; cin >> rate; cin.ignore();
                    cout << "  Capacity: "; cin >> cap; cin.ignore();
                    rooms.addRoom(Room(roomNum, type, rate, cap));
                    break;
                case 2: rooms.displayAll(); break;
                case 3: rooms.displayAvailable(); break;
                case 4:
                    cout << "  Guest Name: "; getline(cin, name);
                    cout << "  Phone: "; getline(cin, phone);
                    cout << "  Email: "; getline(cin, email);
                    makeReservation(name, phone, email);
                    break;
                case 5: processReservation(); break;
                case 6:
                    cout << "  Booking ID: "; getline(cin, bid);
                    {
                        auto it = guests.find(bid);
                        if (it != guests.end()) {
                            Guest& g = it->second;
                            cout << "  Guest: " << g.name
                                 << " | Phone: " << g.phone
                                 << " | Room: "
                                 << (g.roomNumber > 0 ? to_string(g.roomNumber) : "Not checked in")
                                 << " | Email: " << g.email << "\n";
                        } else {
                            cout << "  [ERROR] Booking not found.\n";
                        }
                    }
                    break;
                case 7:
                    cout << "  Booking ID: "; getline(cin, bid);
                    {
                        auto it = guests.find(bid);
                        if (it == guests.end()) {
                            cout << "  [ERROR] Booking not found.\n";
                        } else if (it->second.roomNumber != -1) {
                            cout << "  [ERROR] Guest already checked in.\n";
                        } else {
                            Guest& g = it->second;
                            rooms.displayAvailable();
                            cout << "  Enter room number: "; cin >> roomNum; cin.ignore();
                            if (rooms.assignRoom(roomNum, g.name)) {
                                g.roomNumber = roomNum;
                            }
                        }
                    }
                    break;
                case 8:
                    cout << "  Booking ID: "; getline(cin, bid);
                    checkOut(bid);
                    break;
                case 9:
                    cout << "  Booking ID: "; getline(cin, bid);
                    cout << "  Description: "; getline(cin, desc);
                    cout << "  Amount ($): "; cin >> amount; cin.ignore();
                    addCharge(bid, desc, amount);
                    break;
                case 10: viewCheckoutHistory(); break;
                case 0: cout << "  Exiting. Goodbye!\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    HotelSystem system;
    system.run();
    return 0;
}
