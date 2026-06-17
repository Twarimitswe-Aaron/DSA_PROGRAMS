/*
 * ============================================================================
 * FILE: 24_airline_reservation.cpp
 * ============================================================================
 * AIRLINE RESERVATION SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: High. Airline reservation systems are a classic DSA
 * practical exam topic that tests graph algorithms (route finding),
 * along with queues (waitlist), BST (passenger records), and hash tables.
 *
 * DATA STRUCTURES USED:
 *   1. Graph (adjacency list) - Flight network
 *      - Airports are vertices, flights are weighted edges (distance/cost)
 *      - BFS for shortest path (unweighted), Dijkstra-like for weighted
 *   2. Queue (FIFO) - Waitlist for fully booked flights
 *      - First passenger to wait gets first available seat
 *   3. Binary Search Tree (BST) - Passenger bookings by PNR number
 *      - O(log n) lookup for booking management
 *   4. Hash Table (unordered_map) - Flight lookup by flight number
 *      - O(1) access to flight details, schedule, capacity
 *   5. Stack - Cancellation history / undo last booking
 *      - LIFO: most recent cancellation can be reversed first
 *
 * FUNCTIONALITIES:
 *   - Add airports and flights to the route network
 *   - Find shortest path between airports (BFS)
 *   - Book/cancel flights with undo capability
 *   - Waitlist management when flights are full
 *   - Search flight by number (instant hash table lookup)
 *   - Display all passengers on a flight
 *   - Show flight network topology
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
#include <algorithm>
#include <climits>
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// PASSENGER STRUCTURE
// Represents a booked passenger with PNR (unique booking reference).
// Stored in a BST keyed by PNR for O(log n) search.
// -------------------------------------------------------------------
struct Passenger {
    string pnr;            // Unique booking reference (e.g., "PNR001")
    string name;           // Passenger name
    int age;               // Age of passenger
    string flightNumber;   // Flight they are booked on
    string seatNumber;     // Assigned seat
    string bookingDate;    // Date of booking
    bool isCheckedIn;      // Check-in status

    Passenger(string p, string n, int a, string f, string s)
        : pnr(p), name(n), age(a), flightNumber(f), seatNumber(s),
          isCheckedIn(false) {
        time_t now = time(0);
        bookingDate = ctime(&now);
        if (!bookingDate.empty() && bookingDate.back() == '\n')
            bookingDate.pop_back();
    }
};

// -------------------------------------------------------------------
// FLIGHT STRUCTURE
// Stored in hash table by flight number for O(1) lookup.
// -------------------------------------------------------------------
struct Flight {
    string flightNumber;     // Unique flight code (e.g., "AI202")
    string origin;           // Departure airport code
    string destination;      // Arrival airport code
    int capacity;            // Total seats
    int bookedCount;         // Currently booked seats
    double fare;             // Base fare
    queue<string> waitlist;  // FIFO queue of PNRs waiting for seats

    Flight() : capacity(0), bookedCount(0), fare(0.0) {}

    Flight(string fn, string from, string to, int cap, double f)
        : flightNumber(fn), origin(from), destination(to),
          capacity(cap), bookedCount(0), fare(f) {}

    bool isFull() const { return bookedCount >= capacity; }
    int availableSeats() const { return capacity - bookedCount; }

    void display() const {
        cout << "  " << left << setw(10) << flightNumber
             << setw(10) << origin
             << setw(10) << destination
             << setw(8) << capacity
             << setw(8) << bookedCount
             << setw(8) << availableSeats()
             << "$" << fixed << setprecision(2) << fare
             << (isFull() ? " FULL" : "")
             << " | Wait: " << waitlist.size() << "\n";
    }
};

// ============================================================================
// SECTION 2: GRAPH FOR FLIGHT NETWORK
//
// Why Graph? Airlines operate route networks where airports connect via
// flights. A graph naturally models this:
//   - Vertices = airports (identified by IATA codes)
//   - Edges = flights (with weight = distance or travel time)
// We use an adjacency list (vector of edges per vertex) for memory
// efficiency in sparse flight networks.
// ============================================================================

struct Edge {
    string destination;  // Destination airport code
    int distance;        // Distance in km
    double cost;         // Typical cost

    Edge(string dest, int dist, double c)
        : destination(dest), distance(dist), cost(c) {}
};

class FlightGraph {
private:
    unordered_map<string, vector<Edge> > adjList;
    unordered_map<string, string> airportNames;  // Code -> full name

public:
    // -------------------------------------------------------------------
    // ADD AIRPORT: Add a vertex to the graph. O(1).
    // -------------------------------------------------------------------
    void addAirport(string code, string name) {
        if (airportNames.find(code) == airportNames.end()) {
            airportNames[code] = name;
            adjList[code] = vector<Edge>();
            cout << "  [OK] Airport '" << name << "' (" << code << ") added.\n";
        } else {
            cout << "  [ERROR] Airport " << code << " already exists.\n";
        }
    }

    // -------------------------------------------------------------------
    // ADD FLIGHT ROUTE: Add a directed edge between airports. O(1).
    // -------------------------------------------------------------------
    void addFlight(string from, string to, int distance, double cost) {
        if (airportNames.find(from) == airportNames.end()) {
            cout << "  [ERROR] Origin airport " << from << " not found.\n";
            return;
        }
        if (airportNames.find(to) == airportNames.end()) {
            cout << "  [ERROR] Destination airport " << to << " not found.\n";
            return;
        }
        adjList[from].push_back(Edge(to, distance, cost));
        cout << "  [OK] Flight added: " << from << " -> " << to
             << " (" << distance << " km, $" << cost << ")\n";
    }

    // -------------------------------------------------------------------
    // BFS SHORTEST PATH: Find minimum number of connections (hops).
    // BFS guarantees the shortest path in an UNWEIGHTED graph (where
    // each edge has equal weight = 1 connection). For a weighted graph
    // where edges have different distances, use Dijkstra (not BFS).
    //
    // We use BFS here for path-with-fewest-stops queries.
    // O(V + E) time, O(V) space.
    // -------------------------------------------------------------------
    void findShortestPath(string from, string to) {
        if (airportNames.find(from) == airportNames.end() ||
            airportNames.find(to) == airportNames.end()) {
            cout << "  [ERROR] Airport not found.\n";
            return;
        }

        // BFS setup
        unordered_map<string, bool> visited;
        unordered_map<string, string> parent;  // For path reconstruction
        queue<string> q;

        visited[from] = true;
        q.push(from);
        parent[from] = "";

        while (!q.empty()) {
            string current = q.front();
            q.pop();

            if (current == to) break;

            for (size_t i = 0; i < adjList[current].size(); i++) {
                string next = adjList[current][i].destination;
                if (!visited[next]) {
                    visited[next] = true;
                    parent[next] = current;
                    q.push(next);
                }
            }
        }

        if (!visited[to]) {
            cout << "  [INFO] No path found between "
                 << from << " and " << to << ".\n";
            return;
        }

        // Reconstruct path from parent pointers
        stack<string> path;
        string current = to;
        while (current != "") {
            path.push(current);
            current = parent[current];
        }

        cout << "  Shortest path (fewest stops): ";
        int hops = 0;
        while (!path.empty()) {
            string airport = path.top();
            path.pop();
            cout << airportNames[airport] << " (" << airport << ")";
            if (!path.empty()) cout << " -> ";
            hops++;
        }
        cout << "\n  Stops: " << (hops - 1) << "\n";
    }

    // -------------------------------------------------------------------
    // DISPLAY AIRPORT LIST
    // -------------------------------------------------------------------
    void displayAirports() {
        if (airportNames.empty()) {
            cout << "  [INFO] No airports in network.\n";
            return;
        }
        cout << "  " << left << setw(8) << "Code"
             << "Airport Name\n";
        cout << "  " << string(30, '-') << "\n";
        for (auto& pair : airportNames) {
            cout << "  " << left << setw(8) << pair.first
                 << pair.second << "\n";
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY FLIGHT NETWORK (all routes from each airport)
    // -------------------------------------------------------------------
    void displayNetwork() {
        if (adjList.empty()) {
            cout << "  [INFO] No flights in network.\n";
            return;
        }
        for (auto& pair : adjList) {
            string from = pair.first;
            cout << "  " << airportNames[from] << " (" << from << ") ->\n";
            for (size_t i = 0; i < pair.second.size(); i++) {
                Edge& e = pair.second[i];
                cout << "      " << airportNames[e.destination]
                     << " (" << e.destination << ")  "
                     << e.distance << " km, $" << e.cost << "\n";
            }
        }
    }

    bool airportExists(string code) {
        return airportNames.find(code) != airportNames.end();
    }
};

// ============================================================================
// SECTION 3: BST FOR PASSENGER BOOKINGS
//
// Why BST? Passengers are searched by PNR number. BST gives O(log n)
// search/insert/delete and can traverse in sorted order (handy for
// generating passenger manifests sorted by PNR).
// ============================================================================

struct BookingNode {
    Passenger passenger;
    BookingNode* left;
    BookingNode* right;

    BookingNode(Passenger p) : passenger(p), left(nullptr), right(nullptr) {}
};

class BookingBST {
private:
    BookingNode* root;

    BookingNode* insert(BookingNode* node, Passenger p) {
        if (!node) return new BookingNode(p);
        if (p.pnr < node->passenger.pnr)
            node->left = insert(node->left, p);
        else if (p.pnr > node->passenger.pnr)
            node->right = insert(node->right, p);
        return node;
    }

    BookingNode* search(BookingNode* node, string pnr) {
        if (!node || node->passenger.pnr == pnr) return node;
        if (pnr < node->passenger.pnr)
            return search(node->left, pnr);
        return search(node->right, pnr);
    }

    BookingNode* findMin(BookingNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    BookingNode* deleteNode(BookingNode* node, string pnr) {
        if (!node) return nullptr;
        if (pnr < node->passenger.pnr)
            node->left = deleteNode(node->left, pnr);
        else if (pnr > node->passenger.pnr)
            node->right = deleteNode(node->right, pnr);
        else {
            if (!node->left) {
                BookingNode* temp = node->right;
                delete node;
                return temp;
            }
            if (!node->right) {
                BookingNode* temp = node->left;
                delete node;
                return temp;
            }
            BookingNode* successor = findMin(node->right);
            node->passenger = successor->passenger;
            node->right = deleteNode(node->right, successor->passenger.pnr);
        }
        return node;
    }

    void inorderDisplay(BookingNode* node) {
        if (!node) return;
        inorderDisplay(node->left);
        cout << "    " << left << setw(12) << node->passenger.pnr
             << setw(25) << node->passenger.name
             << setw(6) << node->passenger.age
             << setw(10) << node->passenger.flightNumber
             << setw(8) << node->passenger.seatNumber
             << (node->passenger.isCheckedIn ? "Checked In" : "Not Checked")
             << "\n";
        inorderDisplay(node->right);
    }

    int countByFlight(BookingNode* node, string flightNum) {
        if (!node) return 0;
        int count = countByFlight(node->left, flightNum);
        if (node->passenger.flightNumber == flightNum) count++;
        count += countByFlight(node->right, flightNum);
        return count;
    }

    void destroy(BookingNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    BookingBST() : root(nullptr) {}
    ~BookingBST() { destroy(root); }

    bool addBooking(Passenger p) {
        if (search(root, p.pnr)) {
            cout << "  [ERROR] Booking with PNR " << p.pnr
                 << " already exists.\n";
            return false;
        }
        root = insert(root, p);
        return true;
    }

    Passenger* findBooking(string pnr) {
        BookingNode* node = search(root, pnr);
        if (node) return &node->passenger;
        return nullptr;
    }

    bool removeBooking(string pnr) {
        if (!search(root, pnr)) return false;
        root = deleteNode(root, pnr);
        return true;
    }

    void displayAllBookings() {
        if (!root) {
            cout << "  [INFO] No bookings.\n";
            return;
        }
        cout << "    " << left << setw(12) << "PNR"
             << setw(25) << "Name"
             << setw(6) << "Age"
             << setw(10) << "Flight"
             << setw(8) << "Seat"
             << "Status\n";
        cout << "    " << string(70, '-') << "\n";
        inorderDisplay(root);
    }

    void displayByFlight(string flightNum) {
        if (!root) {
            cout << "  [INFO] No bookings for flight " << flightNum << ".\n";
            return;
        }
        cout << "  Bookings for flight " << flightNum << ":\n";
        // We traverse and filter manually
        stack<BookingNode*> stk;
        BookingNode* curr = root;
        bool found = false;
        while (curr || !stk.empty()) {
            while (curr) {
                stk.push(curr);
                curr = curr->left;
            }
            curr = stk.top();
            stk.pop();
            if (curr->passenger.flightNumber == flightNum) {
                if (!found) {
                    cout << "    " << left << setw(12) << "PNR"
                         << setw(25) << "Name"
                         << setw(6) << "Age"
                         << setw(8) << "Seat" << "\n";
                    found = true;
                }
                cout << "    " << left << setw(12) << curr->passenger.pnr
                     << setw(25) << curr->passenger.name
                     << setw(6) << curr->passenger.age
                     << setw(8) << curr->passenger.seatNumber << "\n";
            }
            curr = curr->right;
        }
        if (!found) cout << "    No passengers found.\n";
    }

    int passengerCountOnFlight(string flightNum) {
        return countByFlight(root, flightNum);
    }
};

// ============================================================================
// SECTION 4: MAIN SYSTEM
// ============================================================================

class AirlineSystem {
private:
    FlightGraph routeNetwork;       // Graph: airport -> flight connections
    BookingBST bookings;            // BST: PNR -> passenger booking
    unordered_map<string, Flight> flights;  // Hash: flight# -> Flight details
    stack<string> cancellationStack;       // Stack: recent cancellations (PNR)
    int pnrCounter;

    string generatePNR() {
        stringstream ss;
        ss << "PNR" << setw(3) << setfill('0') << pnrCounter++;
        return ss.str();
    }

public:
    AirlineSystem() : pnrCounter(1) {
        // Seed some airports
        routeNetwork.addAirport("JFK", "New York JFK");
        routeNetwork.addAirport("LAX", "Los Angeles");
        routeNetwork.addAirport("ORD", "Chicago O'Hare");
        routeNetwork.addAirport("ATL", "Atlanta Hartsfield");
        routeNetwork.addAirport("DFW", "Dallas/Fort Worth");

        // Seed some flights
        addFlightToSystem("AA100", "JFK", "LAX", 3970, 350.00, 200);
        addFlightToSystem("AA200", "JFK", "ORD", 1190, 150.00, 150);
        addFlightToSystem("UA300", "LAX", "ORD", 2800, 250.00, 180);
        addFlightToSystem("DL400", "ATL", "JFK", 1220, 140.00, 160);
        addFlightToSystem("AA500", "DFW", "LAX", 2000, 200.00, 175);
        addFlightToSystem("UA600", "ORD", "ATL", 980, 120.00, 140);
    }

    // -------------------------------------------------------------------
    // ADD FLIGHT: Insert into hash table + graph. O(1) avg.
    // -------------------------------------------------------------------
    void addFlightToSystem(string fn, string from, string to, int dist,
                           double fare, int capacity) {
        if (flights.find(fn) != flights.end()) {
            cout << "  [ERROR] Flight " << fn << " already exists.\n";
            return;
        }
        if (!routeNetwork.airportExists(from)) {
            cout << "  [ERROR] Airport " << from << " not found.\n";
            return;
        }
        if (!routeNetwork.airportExists(to)) {
            cout << "  [ERROR] Airport " << to << " not found.\n";
            return;
        }
        flights[fn] = Flight(fn, from, to, capacity, fare);
        routeNetwork.addFlight(from, to, dist, fare);
    }

    // -------------------------------------------------------------------
    // BOOK FLIGHT: Find flight -> check capacity -> add to BST. O(log n).
    // If full, add to waitlist queue.
    // -------------------------------------------------------------------
    void bookFlight(string flightNum, string name, int age) {
        auto it = flights.find(flightNum);
        if (it == flights.end()) {
            cout << "  [ERROR] Flight " << flightNum << " not found.\n";
            return;
        }

        Flight& flight = it->second;
        string pnr = generatePNR();
        string seat = "S" + to_string(flight.bookedCount + 1);

        if (flight.isFull()) {
            flight.waitlist.push(pnr);
            cout << "  [WAITLIST] Flight " << flightNum << " is full. "
                 << "PNR " << pnr << " added to waitlist (position "
                 << flight.waitlist.size() << ").\n";

            Passenger p(pnr, name, age, flightNum, "WAITLIST");
            bookings.addBooking(p);
            return;
        }

        Passenger p(pnr, name, age, flightNum, seat);
        if (bookings.addBooking(p)) {
            flight.bookedCount++;
            cout << "  [OK] Booked! PNR: " << pnr
                 << " | Flight: " << flightNum
                 << " | Seat: " << seat
                 << " | Passenger: " << name << "\n";
        }
    }

    // -------------------------------------------------------------------
    // CANCEL BOOKING: Remove from BST -> free seat -> process waitlist. O(log n).
    // Push to stack for potential undo.
    // -------------------------------------------------------------------
    void cancelBooking(string pnr) {
        Passenger* p = bookings.findBooking(pnr);
        if (!p) {
            cout << "  [ERROR] Booking not found with PNR " << pnr << ".\n";
            return;
        }

        string flightNum = p->flightNumber;
        auto it = flights.find(flightNum);

        bookings.removeBooking(pnr);
        cancellationStack.push(pnr);

        if (it != flights.end()) {
            Flight& flight = it->second;
            if (p->seatNumber != "WAITLIST") {
                flight.bookedCount--;

                // Process waitlist (FIFO)
                if (!flight.waitlist.empty()) {
                    string nextPnr = flight.waitlist.front();
                    flight.waitlist.pop();
                    Passenger* nextP = bookings.findBooking(nextPnr);
                    if (nextP) {
                        nextP->seatNumber = "S" + to_string(flight.bookedCount + 1);
                        flight.bookedCount++;
                        cout << "  [WAITLIST] PNR " << nextPnr
                             << " auto-booked from waitlist.\n";
                    }
                }
            }
        }

        cout << "  [OK] Booking " << pnr << " cancelled.\n";
    }

    // -------------------------------------------------------------------
    // UNDO CANCELLATION: Pop from stack, re-book. O(1) for stack + O(log n) for BST.
    // -------------------------------------------------------------------
    void undoCancel() {
        if (cancellationStack.empty()) {
            cout << "  [INFO] No cancellations to undo.\n";
            return;
        }
        string pnr = cancellationStack.top();
        cancellationStack.pop();

        Passenger* p = bookings.findBooking(pnr);
        if (!p) {
            cout << "  [ERROR] Cannot undo: booking data for " << pnr
                 << " no longer available.\n";
            return;
        }

        auto it = flights.find(p->flightNumber);
        if (it != flights.end()) {
            Flight& flight = it->second;
            if (!flight.isFull()) {
                p->seatNumber = "S" + to_string(flight.bookedCount + 1);
                flight.bookedCount++;
                cout << "  [UNDO] Reinstated booking " << pnr
                     << " on flight " << p->flightNumber
                     << " seat " << p->seatNumber << ".\n";
            } else {
                p->seatNumber = "WAITLIST";
                flight.waitlist.push(pnr);
                cout << "  [UNDO] Reinstated to waitlist for flight "
                     << p->flightNumber << ".\n";
            }
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY FLIGHTS: Iterate hash table. O(n).
    // -------------------------------------------------------------------
    void displayFlights() {
        if (flights.empty()) {
            cout << "  [INFO] No flights in system.\n";
            return;
        }
        cout << "  " << left << setw(10) << "Flight"
             << setw(10) << "From"
             << setw(10) << "To"
             << setw(8) << "Cap."
             << setw(8) << "Booked"
             << setw(8) << "Avail."
             << "Fare\n";
        cout << "  " << string(70, '-') << "\n";
        for (auto& pair : flights) {
            pair.second.display();
        }
    }

    // -------------------------------------------------------------------
    // SEARCH FLIGHT BY NUMBER: O(1) hash table lookup.
    // -------------------------------------------------------------------
    void searchFlight(string flightNum) {
        auto it = flights.find(flightNum);
        if (it == flights.end()) {
            cout << "  [ERROR] Flight " << flightNum << " not found.\n";
            return;
        }
        Flight& f = it->second;
        cout << "  Flight " << f.flightNumber << ":\n";
        cout << "    " << f.origin << " -> " << f.destination << "\n";
        cout << "    Capacity: " << f.capacity
             << " | Booked: " << f.bookedCount
             << " | Available: " << f.availableSeats() << "\n";
        cout << "    Fare: $" << f.fare << "\n";
        cout << "    Waitlist: " << f.waitlist.size() << "\n";
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   AIRLINE RESERVATION SYSTEM\n";
        cout << "   Data Structures: Graph, BST, Queue,\n";
        cout << "   Hash Table, Stack\n";
        cout << "=============================================\n\n";

        int choice, age, dist, capacity;
        string flightNum, from, to, name, pnr, airportCode, airportName;
        double fare;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [FLIGHT NETWORK - Graph]\n";
            cout << "    1. Add Airport\n";
            cout << "    2. Add Flight Route\n";
            cout << "    3. Display Airports\n";
            cout << "    4. Display Flight Network\n";
            cout << "    5. Find Shortest Path (BFS)\n";
            cout << "  [BOOKINGS - BST]\n";
            cout << "    6. Add Flight to System\n";
            cout << "    7. Display All Flights\n";
            cout << "    8. Search Flight (Hash Table)\n";
            cout << "    9. Book Flight\n";
            cout << "   10. Cancel Booking\n";
            cout << "   11. Display All Bookings (BST In-Order)\n";
            cout << "   12. Display Bookings by Flight\n";
            cout << "  [UNDO - Stack]\n";
            cout << "   13. Undo Last Cancellation\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Airport Code: "; getline(cin, airportCode);
                    cout << "  Airport Name: "; getline(cin, airportName);
                    routeNetwork.addAirport(airportCode, airportName);
                    break;
                case 2:
                    cout << "  From: "; getline(cin, from);
                    cout << "  To: "; getline(cin, to);
                    cout << "  Distance (km): "; cin >> dist; cin.ignore();
                    cout << "  Cost ($): "; cin >> fare; cin.ignore();
                    routeNetwork.addFlight(from, to, dist, fare);
                    break;
                case 3: routeNetwork.displayAirports(); break;
                case 4: routeNetwork.displayNetwork(); break;
                case 5:
                    cout << "  From: "; getline(cin, from);
                    cout << "  To: "; getline(cin, to);
                    routeNetwork.findShortestPath(from, to);
                    break;
                case 6:
                    cout << "  Flight Number: "; getline(cin, flightNum);
                    cout << "  From: "; getline(cin, from);
                    cout << "  To: "; getline(cin, to);
                    cout << "  Distance (km): "; cin >> dist; cin.ignore();
                    cout << "  Fare ($): "; cin >> fare; cin.ignore();
                    cout << "  Capacity: "; cin >> capacity; cin.ignore();
                    addFlightToSystem(flightNum, from, to, dist, fare, capacity);
                    break;
                case 7: displayFlights(); break;
                case 8:
                    cout << "  Flight Number: "; getline(cin, flightNum);
                    searchFlight(flightNum);
                    break;
                case 9:
                    cout << "  Flight Number: "; getline(cin, flightNum);
                    cout << "  Passenger Name: "; getline(cin, name);
                    cout << "  Age: "; cin >> age; cin.ignore();
                    bookFlight(flightNum, name, age);
                    break;
                case 10:
                    cout << "  PNR Number: "; getline(cin, pnr);
                    cancelBooking(pnr);
                    break;
                case 11: bookings.displayAllBookings(); break;
                case 12:
                    cout << "  Flight Number: "; getline(cin, flightNum);
                    bookings.displayByFlight(flightNum);
                    break;
                case 13: undoCancel(); break;
                case 0: cout << "  Exiting. Goodbye!\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    AirlineSystem system;
    system.run();
    return 0;
}
