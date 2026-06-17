/*
 * ============================================================================
 * FILE: 18_train_reservation.cpp
 * ============================================================================
 * TRAIN RESERVATION SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: High. Railway reservation systems test understanding of
 * multiple data structures working together in a single application, and
 * appear frequently in DSA practical exams.
 *
 * DATA STRUCTURES USED:
 *   1. Graph (Adjacency List)       - Railway network: stations are vertices,
 *                                     tracks are weighted edges (distance in km)
 *   2. Queue (FIFO)                - Waitlist for fully booked trains
 *                                     (first-come-first-served when seats open)
 *   3. Stack (LIFO)               - Cancellation history for undo/review
 *                                     (most recent cancellation on top)
 *   4. Circular Linked List         - Seat management: rotating seat allocation
 *                                     in a circular layout (no wasted space)
 *   5. Hash Table (unordered_map)   - Train lookup by train number (O(1)),
 *                                     Passenger lookup by PNR (O(1))
 *
 * FUNCTIONALITIES:
 *   - Add/display train routes with stations (graph with edge weights)
 *   - Find shortest path between stations (Dijkstra's algorithm)
 *   - Book tickets (check availability, if full add to waitlist queue)
 *   - Cancel tickets (push to cancellation stack for undo)
 *   - Automatic waitlist processing (FIFO: next in queue gets freed seat)
 *   - View seat availability with circular seat layout
 *   - Find all trains between two stations
 *   - View passenger details by PNR (hash table lookup)
 *   - View cancellation history (stack - most recent first)
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <vector>
#include <list>           // For adjacency list in graph
#include <queue>          // For waitlist (FIFO) and Dijkstra min-heap
#include <stack>          // For cancellation history (LIFO)
#include <unordered_map>  // For hash table: train number -> Train, PNR -> Passenger
#include <climits>        // For INT_MAX (infinity in Dijkstra)
#include <iomanip>        // For formatted output (setw)
#include <ctime>          // For timestamps in cancellation records
using namespace std;

// ============================================================================
// SECTION 1: DATA STRUCTURES USED BY THE SYSTEM
// ============================================================================

// -------------------------------------------------------------------
// SeatNode - Circular Linked List node for seat management.
//
// Why Circular Linked List? Seats are arranged in a circular layout
// (e.g., around compartment tables). Circular list lets us rotate
// through seats to find the next available one without dead ends.
// -------------------------------------------------------------------
struct SeatNode {
    int seatNumber;        // Seat number (1 to totalSeats)
    string passengerName;  // Name of passenger (empty if available)
    bool isBooked;         // true = occupied, false = available
    SeatNode* next;        // Pointer to next seat (circular, last points to first)

    SeatNode(int num) : seatNumber(num), passengerName(""),
                        isBooked(false), next(nullptr) {}
};

// -------------------------------------------------------------------
// Passenger - Booking record stored in hash table for O(1) PNR lookup.
// -------------------------------------------------------------------
struct Passenger {
    string pnr;            // Unique PNR (e.g., "PNR1001")
    string name;           // Passenger name
    int trainNumber;       // Train they are booked on
    int seatNumber;        // Allocated seat number (-1 if waitlisted)
    string status;         // "CONFIRMED", "WAITLISTED", or "CANCELLED"

    Passenger() : trainNumber(-1), seatNumber(-1), status("") {}

    Passenger(string p, string n, int t, int s, string st)
        : pnr(p), name(n), trainNumber(t), seatNumber(s), status(st) {}
};

// -------------------------------------------------------------------
// WaitlistRequest - Queue element for waitlisted bookings (FIFO).
// -------------------------------------------------------------------
struct WaitlistRequest {
    string pnr;           // PNR of the waitlisted passenger
    int trainNumber;      // Which train they want

    WaitlistRequest(string p, int t) : pnr(p), trainNumber(t) {}
};

// -------------------------------------------------------------------
// CancellationRecord - Stack element for cancellation history (LIFO).
//
// Why Stack? Recent cancellations are reviewed most-recent-first.
// Stack supports the undo/review pattern naturally.
// -------------------------------------------------------------------
struct CancellationRecord {
    string pnr;            // PNR of cancelled booking
    string passengerName;  // Name of passenger who cancelled
    int trainNumber;       // Train number
    int seatNumber;        // Seat that was freed
    string timestamp;      // When the cancellation happened

    CancellationRecord(string p, string n, int t, int s)
        : pnr(p), passengerName(n), trainNumber(t), seatNumber(s) {
        time_t now = time(nullptr);
        timestamp = ctime(&now);
        // Remove trailing newline from ctime
        if (!timestamp.empty() && timestamp.back() == '\n')
            timestamp.pop_back();
    }
};

// ============================================================================
// SECTION 2: GRAPH - RAILWAY NETWORK
//
// Why Graph? Railway network is a natural graph (stations = vertices,
// tracks = weighted edges). We use adjacency list (vector of lists)
// because railway networks are SPARSE - O(V+E) space vs O(V^2) matrix.
// ============================================================================

class RailwayNetwork {
private:
    vector<string> stations;                    // Station names (index = vertex ID)
    unordered_map<string, int> stationIndex;    // Hash: station name -> vertex ID
    vector<list<pair<int, int>>> adjList;       // Adjacency list: {neighbor, distance}

    // -------------------------------------------------------------------
    // GET OR CREATE STATION: Returns vertex ID for a station name.
    // If station doesn't exist, creates it. O(1) average using hash table.
    // -------------------------------------------------------------------
    int getOrCreateStation(string name) {
        if (stationIndex.find(name) == stationIndex.end()) {
            stationIndex[name] = stations.size();
            stations.push_back(name);
            adjList.resize(stations.size());
        }
        return stationIndex[name];
    }

public:
    // -------------------------------------------------------------------
    // ADD TRACK: Add a bidirectional track between two stations.
    // Railway tracks are undirected (you can travel both ways).
    // Edge weight = distance in kilometers.
    // -------------------------------------------------------------------
    void addTrack(string station1, string station2, int distance) {
        int u = getOrCreateStation(station1);
        int v = getOrCreateStation(station2);
        adjList[u].push_back({v, distance});  // station1 -> station2
        adjList[v].push_back({u, distance});  // station2 -> station1 (undirected)
    }

    // -------------------------------------------------------------------
    // DISPLAY NETWORK: Show all stations and their connections. O(V+E).
    // -------------------------------------------------------------------
    void displayNetwork() {
        if (stations.empty()) {
            cout << "  [INFO] No stations in the network.\n";
            return;
        }
        cout << "  Railway Network (stations and track distances):\n";
        for (size_t i = 0; i < stations.size(); i++) {
            cout << "  " << left << setw(15) << stations[i] << " -> ";
            for (auto& edge : adjList[i]) {
                cout << stations[edge.first] << "(" << edge.second << "km) ";
            }
            cout << "\n";
        }
    }

    // -------------------------------------------------------------------
    // SHORTEST DISTANCE: Dijkstra's algorithm for weighted shortest path.
    //
    // Why Dijkstra? We need the shortest path in terms of track distance
    // (positive edge weights). Dijkstra with a min-heap (priority queue)
    // gives O((V+E) log V) time complexity.
    //
    // Returns distance in km, or -1 if no path exists.
    // Also prints the path via the parent array.
    // -------------------------------------------------------------------
    int shortestDistance(string source, string dest) {
        if (stationIndex.find(source) == stationIndex.end()) {
            cout << "  [ERROR] Station '" << source << "' not found.\n";
            return -1;
        }
        if (stationIndex.find(dest) == stationIndex.end()) {
            cout << "  [ERROR] Station '" << dest << "' not found.\n";
            return -1;
        }

        int src = stationIndex[source];
        int dst = stationIndex[dest];
        int V = stations.size();

        vector<int> dist(V, INT_MAX);   // Initialize all distances to infinity
        vector<int> parent(V, -1);      // For reconstructing the path
        // Min-heap: {distance, vertex}. greater<pair> makes it min-heap.
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            // Skip stale entries (if we already found a shorter path)
            if (d > dist[u]) continue;

            // Relax all edges from u
            for (auto& edge : adjList[u]) {
                int v = edge.first;
                int w = edge.second;
                if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        if (dist[dst] == INT_MAX) {
            cout << "  [INFO] No path exists between "
                 << source << " and " << dest << ".\n";
            return -1;
        }

        // Reconstruct and print the path
        vector<string> path;
        for (int v = dst; v != -1; v = parent[v]) {
            path.push_back(stations[v]);
        }
        cout << "  Shortest path (" << dist[dst] << " km): ";
        for (int i = path.size() - 1; i >= 0; i--) {
            cout << path[i];
            if (i > 0) cout << " -> ";
        }
        cout << "\n";

        return dist[dst];
    }

    // -------------------------------------------------------------------
    // HAS STATION: Check if a station exists in the network. O(1) avg.
    // -------------------------------------------------------------------
    bool hasStation(string name) {
        return stationIndex.find(name) != stationIndex.end();
    }

};

// ============================================================================
// SECTION 3: TRAIN CLASS WITH CIRCULAR LINKED LIST SEAT MANAGEMENT
//
// Why Circular Linked List for seats?
//   - Seats are arranged in a circular pattern (e.g., around compartment tables)
//   - Allocating the "next available" seat means rotating through the circle
//   - No wasted space: we never reach a null pointer, just cycle through
//   - Easy to display the circular seat layout to the user
// ============================================================================

class Train {
private:
    int trainNumber;        // Unique train number (e.g., 101)
    string trainName;       // Train name (e.g., "Mumbai Express")
    string source;          // Source station
    string destination;     // Destination station
    vector<string> routeStations; // All stations this train stops at (for route queries)
    int totalSeats;         // Total number of seats on this train
    int availableSeats;     // Currently available seats
    SeatNode* seatHead;     // Head of circular linked list of seats
    SeatNode* lastAllocated; // Pointer to last allocated seat (rotating allocation)

    // -------------------------------------------------------------------
    // CREATE SEATS: Build the circular linked list of seats.
    // Each seat is a node; the last node's next points back to head.
    // O(n) time for n seats.
    // -------------------------------------------------------------------
    void createSeats() {
        if (seatHead) return;  // Already created
        seatHead = new SeatNode(1);
        SeatNode* current = seatHead;
        for (int i = 2; i <= totalSeats; i++) {
            current->next = new SeatNode(i);
            current = current->next;
        }
        current->next = seatHead;  // Point last node back to head -> circular!
        lastAllocated = seatHead;  // Start allocation from head
    }

public:
    Train() : trainNumber(0), trainName(""), source(""), destination(""),
              totalSeats(0), availableSeats(0), seatHead(nullptr),
              lastAllocated(nullptr) {}

    Train(int num, string name, string src, string dest, int seats)
        : trainNumber(num), trainName(name), source(src), destination(dest),
          totalSeats(seats), availableSeats(seats),
          seatHead(nullptr), lastAllocated(nullptr) {
        createSeats();
        // By default, route includes source and destination
        routeStations.push_back(src);
        routeStations.push_back(dest);
    }

    // -------------------------------------------------------------------
    // DESTRUCTOR: Delete all seat nodes in the circular list.
    // Must carefully traverse the circle and delete each node.
    // -------------------------------------------------------------------
    ~Train() {
        if (seatHead) {
            SeatNode* current = seatHead;
            do {
                SeatNode* temp = current;
                current = current->next;
                delete temp;
            } while (current != seatHead);
        }
    }

    // -------------------------------------------------------------------
    // ADD ROUTE STATION: Add an intermediate station to this train's route.
    // Used by findTrainsBetweenStations to match trains that stop at
    // both the user's source and destination.
    // -------------------------------------------------------------------
    void addRouteStation(string station) {
        routeStations.push_back(station);
    }

    // -------------------------------------------------------------------
    // GETTERS
    // -------------------------------------------------------------------
    int getNumber()              { return trainNumber; }
    string getName()             { return trainName; }
    string getSource()           { return source; }
    string getDestination()      { return destination; }
    int getAvailableSeats()      { return availableSeats; }
    int getTotalSeats()          { return totalSeats; }
    vector<string> getRoute()    { return routeStations; }

    bool isFull()                { return availableSeats == 0; }

    // -------------------------------------------------------------------
    // BOOK SEAT: Find the next available seat using rotating allocation.
    //
    // Algorithm: Starting from lastAllocated->next, traverse the circular
    // list until we find an unbooked seat. This ensures fair rotating
    // allocation (not always starting from seat 1).
    //
    // Returns seat number on success, -1 if train is full.
    // -------------------------------------------------------------------
    int bookSeat(string passengerName) {
        if (isFull()) return -1;

        SeatNode* current = lastAllocated->next;
        // Traverse the circle until we come back to lastAllocated
        while (current != lastAllocated) {
            if (!current->isBooked) {
                current->isBooked = true;
                current->passengerName = passengerName;
                availableSeats--;
                lastAllocated = current;
                return current->seatNumber;
            }
            current = current->next;
        }
        // Check lastAllocated itself (one last chance)
        if (!lastAllocated->isBooked) {
            lastAllocated->isBooked = true;
            lastAllocated->passengerName = passengerName;
            availableSeats--;
            return lastAllocated->seatNumber;
        }
        return -1;  // Should not reach here if isFull() check passed
    }

    // -------------------------------------------------------------------
    // CANCEL SEAT: Mark a seat as available by seat number.
    // Traverses the circular list to find the seat. O(n) worst case.
    // -------------------------------------------------------------------
    bool cancelSeat(int seatNumber) {
        if (!seatHead) return false;
        SeatNode* current = seatHead;
        do {
            if (current->seatNumber == seatNumber && current->isBooked) {
                current->isBooked = false;
                current->passengerName = "";
                availableSeats++;
                return true;
            }
            current = current->next;
        } while (current != seatHead);
        return false;
    }

    // -------------------------------------------------------------------
    // DISPLAY SEATS: Show the circular seat layout.
    // Available seats are shown as [num] and booked as [num*].
    // -------------------------------------------------------------------
    void displaySeats() {
        if (!seatHead) {
            cout << "  [INFO] No seat data available.\n";
            return;
        }
        cout << "  Seat Layout (Circular) - "
             << (totalSeats - availableSeats) << "/" << totalSeats << " booked:\n    ";
        SeatNode* current = seatHead;
        int count = 0;
        do {
            cout << "[" << setw(2) << current->seatNumber
                 << (current->isBooked ? "*" : " ") << "] ";
            count++;
            // Add a line break every 10 seats for readability
            if (count % 10 == 0) cout << "\n    ";
            current = current->next;
        } while (current != seatHead);
        cout << "\n    (* = booked, space = available)\n";
    }

    // -------------------------------------------------------------------
    // DISPLAY INFO: Show train summary information.
    // -------------------------------------------------------------------
    void displayInfo() {
        cout << "  Train #" << trainNumber << ": " << trainName << "\n";
        cout << "  Route: " << source << " -> " << destination << "\n";
        cout << "  Seats: " << (totalSeats - availableSeats) << "/"
             << totalSeats << " booked";
        if (!isFull()) {
            cout << " (" << availableSeats << " available)";
        } else {
            cout << " (FULL)";
        }
        cout << "\n";
    }

    // -------------------------------------------------------------------
    // STOPS AT: Check if this train stops at a given station.
    // -------------------------------------------------------------------
    bool stopsAt(string station) {
        for (string& s : routeStations) {
            if (s == station) return true;
        }
        return false;
    }
};

// ============================================================================
// SECTION 4: MAIN TRAIN RESERVATION SYSTEM
//
// Integrates ALL data structures into one cohesive application:
//   - unordered_map<int, Train>   : Hash table for O(1) train lookup
//   - unordered_map<string, Passenger> : Hash table for O(1) PNR lookup
//   - queue<WaitlistRequest>      : FIFO waitlist for full trains
//   - stack<CancellationRecord>   : LIFO cancellation history
//   - RailwayNetwork              : Graph of stations and tracks
// ============================================================================

class TrainReservationSystem {
private:
    unordered_map<int, Train> trains;           // Hash: train number -> Train object
    unordered_map<string, Passenger> passengers; // Hash: PNR -> Passenger details
    queue<WaitlistRequest> waitlist;             // Queue: waitlisted booking requests (FIFO)
    stack<CancellationRecord> cancelStack;       // Stack: cancellation history (LIFO)
    RailwayNetwork network;                      // Graph: railway network
    int nextPNR;                                 // Auto-incrementing PNR counter

    // -------------------------------------------------------------------
    // GENERATE PNR: Creates a unique Passenger Name Record identifier.
    // Format: "PNR" + sequential number (e.g., PNR1001, PNR1002).
    // -------------------------------------------------------------------
    string generatePNR() {
        return "PNR" + to_string(nextPNR++);
    }

    // -------------------------------------------------------------------
    // PROCESS WAITLIST: When a seat opens up on a train, check the
    // waitlist queue for that train. The FIRST person in the queue
    // (FIFO order) gets the newly freed seat automatically.
    //
    // Why FIFO? Fairness: the earliest requester gets priority.
    // This is identical to real railway waiting list systems.
    // -------------------------------------------------------------------
    void processWaitlist(int trainNumber) {
        if (waitlist.empty()) return;

        int processed = 0;
        int qSize = waitlist.size();
        queue<WaitlistRequest> temp;

        // Dequeue all requests, process matching ones
        for (int i = 0; i < qSize; i++) {
            WaitlistRequest req = waitlist.front();
            waitlist.pop();

            if (processed == 0 && req.trainNumber == trainNumber &&
                trains.find(trainNumber) != trains.end() &&
                !trains[trainNumber].isFull()) {

                // Found a waitlisted passenger for this train with seats available
                if (passengers.find(req.pnr) != passengers.end()) {
                    Passenger& p = passengers[req.pnr];
                    int seat = trains[trainNumber].bookSeat(p.name);
                    if (seat != -1) {
                        p.seatNumber = seat;
                        p.status = "CONFIRMED";
                        cout << "  [AUTO] Waitlist processed: " << req.pnr
                             << " (" << p.name << ") got seat " << seat
                             << " on train " << trainNumber << "\n";
                        processed++;
                    } else {
                        temp.push(req);  // Could not book, keep in waitlist
                    }
                }
            } else {
                temp.push(req);  // Not matching, keep in waitlist
            }
        }
        waitlist = temp;  // Replace with remaining waitlisted requests
    }

public:
    // -------------------------------------------------------------------
    // CONSTRUCTOR: Initialize the system with sample data.
    // Populates the railway network with major Indian railway routes
    // and creates some initial trains.
    // -------------------------------------------------------------------
    TrainReservationSystem() : nextPNR(1001) {
        // --- Build the Railway Network (Graph) ---
        network.addTrack("Mumbai", "Delhi", 1400);
        network.addTrack("Delhi", "Chennai", 2200);
        network.addTrack("Mumbai", "Chennai", 1300);
        network.addTrack("Delhi", "Kolkata", 1500);
        network.addTrack("Chennai", "Kolkata", 1700);
        network.addTrack("Mumbai", "Bangalore", 1000);
        network.addTrack("Bangalore", "Chennai", 350);
        network.addTrack("Delhi", "Bangalore", 2100);
        network.addTrack("Mumbai", "Kolkata", 2000);
        network.addTrack("Chennai", "Hyderabad", 650);
        network.addTrack("Hyderabad", "Mumbai", 710);
        network.addTrack("Delhi", "Hyderabad", 1600);

        // --- Create Trains (with limited seats to demonstrate waitlist) ---
        Train t101(101, "Mumbai Express", "Mumbai", "Delhi", 5);
        t101.addRouteStation("Surat");
        t101.addRouteStation("Jaipur");
        trains.insert({101, t101});

        Train t102(102, "Coromandel Express", "Mumbai", "Chennai", 5);
        t102.addRouteStation("Pune");
        t102.addRouteStation("Bangalore");
        trains.insert({102, t102});

        Train t103(103, "Grand Trunk Express", "Delhi", "Chennai", 5);
        t103.addRouteStation("Agra");
        t103.addRouteStation("Hyderabad");
        trains.insert({103, t103});

        Train t104(104, "Howrah Express", "Mumbai", "Kolkata", 5);
        t104.addRouteStation("Nagpur");
        t104.addRouteStation("Bhubaneswar");
        trains.insert({104, t104});

        Train t105(105, "Bangalore Rajdhani", "Delhi", "Bangalore", 5);
        t105.addRouteStation("Jaipur");
        t105.addRouteStation("Chennai");
        trains.insert({105, t105});
    }

    // -------------------------------------------------------------------
    // ADD TRAIN: Add a new train with a given route and seat count.
    // -------------------------------------------------------------------
    void addTrain() {
        int num, seats;
        string name, src, dest;
        cout << "  Train Number: "; cin >> num;
        cin.ignore();
        if (trains.find(num) != trains.end()) {
            cout << "  [ERROR] Train " << num << " already exists!\n";
            return;
        }
        cout << "  Train Name: "; getline(cin, name);
        cout << "  Source Station: "; getline(cin, src);
        cout << "  Destination Station: "; getline(cin, dest);
        cout << "  Total Seats: "; cin >> seats;
        cin.ignore();

        // Ensure stations exist in the network graph
        if (!network.hasStation(src) || !network.hasStation(dest)) {
            cout << "  [WARN] One or both stations not in railway network.\n";
            cout << "  Adding stations to network.\n";
            network.addTrack(src, dest, 0);  // Add with 0 distance placeholder
        }

        Train newTrain(num, name, src, dest, seats);
        trains.insert({num, newTrain});
        cout << "  [OK] Train " << num << " (" << name << ") added successfully.\n";
    }

    // -------------------------------------------------------------------
    // DISPLAY ALL TRAINS: Show all registered trains. O(n).
    // -------------------------------------------------------------------
    void displayAllTrains() {
        if (trains.empty()) {
            cout << "  [INFO] No trains in the system.\n";
            return;
        }
        cout << "  " << left << setw(8) << "Train#" << setw(30) << "Name"
             << setw(20) << "Route"
             << "Seats\n";
        cout << "  " << string(75, '-') << "\n";
        for (auto& pair : trains) {
            Train& t = pair.second;
            cout << "  " << left << setw(8) << t.getNumber()
                 << setw(30) << t.getName()
                 << setw(10) << t.getSource() + " -> " + t.getDestination()
                 << (t.getTotalSeats() - t.getAvailableSeats()) << "/"
                 << t.getTotalSeats() << "\n";
        }
    }

    // -------------------------------------------------------------------
    // FIND SHORTEST PATH: Use Dijkstra on the railway network graph
    // to find the shortest track distance between two stations.
    // -------------------------------------------------------------------
    void findShortestPath() {
        string src, dest;
        cout << "  Source Station: "; cin >> src;
        cout << "  Destination Station: "; cin >> dest;
        network.shortestDistance(src, dest);
    }

    // -------------------------------------------------------------------
    // DISPLAY RAILWAY NETWORK: Show the graph of all stations and tracks.
    // -------------------------------------------------------------------
    void displayNetwork() {
        network.displayNetwork();
    }

    // -------------------------------------------------------------------
    // BOOK TICKET: Main booking flow.
    // 1. Show available trains for the route
    // 2. Select a train
    // 3. If seats available -> book seat (circular linked list), create PNR
    // 4. If train is full -> add to waitlist queue (FIFO)
    // 5. Store passenger details in hash table (PNR -> Passenger)
    // -------------------------------------------------------------------
    void bookTicket() {
        if (trains.empty()) {
            cout << "  [ERROR] No trains available.\n";
            return;
        }

        string src, dest, name;
        cout << "  Source Station: "; cin >> src;
        cout << "  Destination Station: "; cin >> dest;
        cin.ignore();

        // Find trains that run on this route
        vector<int> matchingTrains;
        for (auto& pair : trains) {
            Train& t = pair.second;
            if (t.getSource() == src && t.getDestination() == dest) {
                matchingTrains.push_back(t.getNumber());
            }
        }

        if (matchingTrains.empty()) {
            cout << "  [INFO] No direct trains found between "
                 << src << " and " << dest << ".\n";
            cout << "  Try finding trains between stations using the network.\n";
            return;
        }

        cout << "  Available trains:\n";
        for (int tnum : matchingTrains) {
            Train& t = trains[tnum];
            cout << "    " << tnum << ". " << t.getName()
                 << " (" << t.getAvailableSeats() << "/" << t.getTotalSeats() << " seats)\n";
        }

        int choice;
        cout << "  Select train number: "; cin >> choice;
        cin.ignore();
        if (trains.find(choice) == trains.end()) {
            cout << "  [ERROR] Invalid train number.\n";
            return;
        }

        Train& selectedTrain = trains[choice];

        cout << "  Passenger Name: "; getline(cin, name);

        string pnr = generatePNR();

        if (!selectedTrain.isFull()) {
            // Seat available - book it using circular linked list
            int seat = selectedTrain.bookSeat(name);
            if (seat != -1) {
                Passenger p(pnr, name, choice, seat, "CONFIRMED");
                passengers[pnr] = p;  // Store in hash table for O(1) lookup
                cout << "  [OK] Ticket booked! PNR: " << pnr
                     << ", Seat: " << seat << "\n";
            } else {
                cout << "  [ERROR] Unexpected error booking seat.\n";
            }
        } else {
            // Train is full - add to waitlist queue (FIFO)
            Passenger p(pnr, name, choice, -1, "WAITLISTED");
            passengers[pnr] = p;
            waitlist.push(WaitlistRequest(pnr, choice));
            cout << "  [INFO] Train is full. Added to waitlist. PNR: " << pnr << "\n";
            cout << "  You are #" << waitlist.size() << " in the waitlist queue.\n";
        }
    }

    // -------------------------------------------------------------------
    // CANCEL TICKET: Cancel a booking by PNR.
    // 1. Look up passenger in hash table (O(1))
    // 2. Free their seat (circular linked list)
    // 3. Push cancellation to stack (LIFO for undo)
    // 4. Trigger automatic waitlist processing
    // -------------------------------------------------------------------
    void cancelTicket() {
        if (passengers.empty()) {
            cout << "  [INFO] No bookings to cancel.\n";
            return;
        }

        string pnr;
        cout << "  Enter PNR to cancel: "; cin >> pnr;

        if (passengers.find(pnr) == passengers.end()) {
            cout << "  [ERROR] PNR " << pnr << " not found.\n";
            return;
        }

        Passenger& p = passengers[pnr];
        if (p.status == "CANCELLED") {
            cout << "  [INFO] This ticket is already cancelled.\n";
            return;
        }

        int trainNum = p.trainNumber;
        int seatNum = p.seatNumber;

        if (p.status == "CONFIRMED" && seatNum != -1) {
            // Free the seat in the circular linked list
            if (trains.find(trainNum) != trains.end()) {
                Train& t = trains[trainNum];
                if (t.cancelSeat(seatNum)) {
                    // Push cancellation to stack (LIFO for undo)
                    cancelStack.push(CancellationRecord(pnr, p.name, trainNum, seatNum));
                    cout << "  [OK] Ticket " << pnr << " cancelled. Seat "
                         << seatNum << " on train " << trainNum << " freed.\n";

                    // Update passenger status
                    p.status = "CANCELLED";
                    p.seatNumber = -1;

                    // Auto-process waitlist: give the freed seat to next in queue
                    processWaitlist(trainNum);
                } else {
                    cout << "  [ERROR] Could not cancel seat " << seatNum << ".\n";
                }
            }
        } else if (p.status == "WAITLISTED") {
            // Remove from waitlist (requires rebuilding queue)
            p.status = "CANCELLED";
            cout << "  [OK] Waitlisted ticket " << pnr << " removed from queue.\n";
            // Push cancellation to stack for record-keeping
            cancelStack.push(CancellationRecord(pnr, p.name, trainNum, -1));
        } else {
            cout << "  [ERROR] Cannot cancel ticket with status: " << p.status << "\n";
        }
    }

    // -------------------------------------------------------------------
    // VIEW SEAT LAYOUT: Display circular seating for a selected train.
    // Uses the circular linked list traversal to show each seat.
    // -------------------------------------------------------------------
    void viewSeatLayout() {
        if (trains.empty()) {
            cout << "  [INFO] No trains in system.\n";
            return;
        }
        int tnum;
        cout << "  Enter train number: "; cin >> tnum;
        if (trains.find(tnum) == trains.end()) {
            cout << "  [ERROR] Train " << tnum << " not found.\n";
            return;
        }
        trains[tnum].displayInfo();
        trains[tnum].displaySeats();
    }

    // -------------------------------------------------------------------
    // FIND TRAINS BETWEEN STATIONS: Search all trains that stop at both
    // the given source and destination stations (not necessarily direct).
    // Useful for multi-route planning.
    // -------------------------------------------------------------------
    void findTrainsBetweenStations() {
        string src, dest;
        cout << "  Source Station: "; cin >> src;
        cout << "  Destination Station: "; cin >> dest;

        cout << "  Trains stopping at both " << src << " and " << dest << ":\n";
        bool found = false;
        for (auto& pair : trains) {
            Train& t = pair.second;
            if (t.stopsAt(src) && t.stopsAt(dest)) {
                t.displayInfo();
                found = true;
            }
        }
        if (!found) {
            cout << "  [INFO] No trains found that stop at both stations.\n";
        }
    }

    // -------------------------------------------------------------------
    // VIEW PASSENGER BY PNR: O(1) lookup using hash table.
    // Retrieves full passenger details from the PNR.
    // -------------------------------------------------------------------
    void viewPassengerByPNR() {
        string pnr;
        cout << "  Enter PNR: "; cin >> pnr;

        if (passengers.find(pnr) == passengers.end()) {
            cout << "  [ERROR] PNR " << pnr << " not found.\n";
            return;
        }

        Passenger& p = passengers[pnr];
        cout << "  Passenger Details:\n";
        cout << "    PNR: " << p.pnr << "\n";
        cout << "    Name: " << p.name << "\n";
        cout << "    Status: " << p.status << "\n";
        if (p.trainNumber != -1) {
            cout << "    Train: " << p.trainNumber;
            if (trains.find(p.trainNumber) != trains.end()) {
                cout << " (" << trains[p.trainNumber].getName() << ")";
            }
            cout << "\n";
        }
        if (p.seatNumber != -1) {
            cout << "    Seat: " << p.seatNumber << "\n";
        }
    }

    // -------------------------------------------------------------------
    // VIEW CANCELLATION HISTORY: Display recent cancellations (LIFO).
    // Uses the stack to show most recent cancellations first.
    // Useful for undo: the most recent cancellation is on top.
    // -------------------------------------------------------------------
    void viewCancellationHistory() {
        if (cancelStack.empty()) {
            cout << "  [INFO] No cancellations recorded.\n";
            return;
        }
        // Copy stack to preserve the original
        stack<CancellationRecord> temp = cancelStack;
        cout << "  Cancellation History (most recent first):\n";
        cout << "  " << left << setw(15) << "PNR"
             << setw(20) << "Passenger"
             << setw(10) << "Train"
             << setw(8) << "Seat"
             << "Timestamp\n";
        cout << "  " << string(75, '-') << "\n";
        int count = 0;
        while (!temp.empty() && count < 10) {  // Show last 10
            CancellationRecord& r = temp.top();
            cout << "  " << left << setw(15) << r.pnr
                 << setw(20) << r.passengerName
                 << setw(10) << r.trainNumber
                 << setw(8) << (r.seatNumber == -1 ? "WL" : to_string(r.seatNumber))
                 << r.timestamp << "\n";
            temp.pop();
            count++;
        }
    }

    // -------------------------------------------------------------------
    // VIEW WAITLIST STATUS: Show current waitlist queue (FIFO order).
    // -------------------------------------------------------------------
    void viewWaitlist() {
        if (waitlist.empty()) {
            cout << "  [INFO] Waitlist is empty.\n";
            return;
        }
        queue<WaitlistRequest> temp = waitlist;
        cout << "  Waitlist Queue (FIFO order):\n";
        int pos = 1;
        while (!temp.empty()) {
            WaitlistRequest& req = temp.front();
            cout << "  " << pos << ". " << req.pnr << " -> Train #" << req.trainNumber;
            if (passengers.find(req.pnr) != passengers.end()) {
                cout << " (" << passengers[req.pnr].name << ")";
            }
            cout << "\n";
            temp.pop();
            pos++;
        }
    }

    // ==================================================================
    // RUN - Interactive menu-driven console interface
    // ==================================================================
    void run() {
        cout << "\n=============================================\n";
        cout << "   TRAIN RESERVATION SYSTEM\n";
        cout << "=============================================\n\n";

        int choice;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  1. Add Train\n";
            cout << "  2. Display All Trains\n";
            cout << "  3. Book Ticket\n";
            cout << "  4. Cancel Ticket\n";
            cout << "  5. View Seat Layout\n";
            cout << "  6. Find Shortest Path (Dijkstra)\n";
            cout << "  7. Display Railway Network\n";
            cout << "  8. Find Trains Between Stations\n";
            cout << "  9. View Passenger by PNR\n";
            cout << " 10. View Cancellation History\n";
            cout << " 11. View Waitlist Status\n";
            cout << "  0. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore();  // Clear input buffer

            switch (choice) {
                case 1:
                    addTrain();
                    break;

                case 2:
                    displayAllTrains();
                    break;

                case 3:
                    bookTicket();
                    break;

                case 4:
                    cancelTicket();
                    break;

                case 5:
                    viewSeatLayout();
                    break;

                case 6:
                    findShortestPath();
                    break;

                case 7:
                    displayNetwork();
                    break;

                case 8:
                    findTrainsBetweenStations();
                    break;

                case 9:
                    viewPassengerByPNR();
                    break;

                case 10:
                    viewCancellationHistory();
                    break;

                case 11:
                    viewWaitlist();
                    break;

                case 0:
                    cout << "  Exiting system. Safe journey!\n";
                    break;

                default:
                    cout << "  [ERROR] Invalid choice. Please try again.\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION - Entry point
// ============================================================================
int main() {
    TrainReservationSystem system;
    system.run();
    return 0;
}
