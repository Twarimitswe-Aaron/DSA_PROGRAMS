/*
 * ============================================================================
 * FILE: 31_cab_booking.cpp
 * ============================================================================
 * CAB/TAXI BOOKING SYSTEM (Uber-like) - Real-world DSA Application
 *
 * EXAM FREQUENCY: Very High. Cab booking is a classic DSA practical exam
 * problem found on multiple GitHub repos and university lab manuals. Tests
 * graphs (route finding), queues (ride requests), hash tables (driver lookup),
 * stacks (trip history), and heaps (nearest driver).
 *
 * DATA STRUCTURES USED:
 *   1. Graph (Adjacency List) - City Map / Route Network
 *      - Each location is a vertex, roads are weighted edges
 *      - Dijkstra's algorithm for shortest path
 *   2. Queue - Ride Requests (FIFO)
 *      - Pending ride requests processed in order
 *   3. Hash Table (unordered_map) - Driver & Customer Lookup
 *      - O(1) driver search by ID
 *      - O(1) customer lookup
 *   4. Stack - Trip History (LIFO)
 *      - Most recent trip first
 *   5. Min-Heap (priority_queue) - Nearest Available Drivers
 *      - Sort drivers by distance from pickup
 *      - O(log n) insertion/extraction of nearest driver
 *
 * FUNCTIONALITIES:
 *   - Add locations & roads to graph
 *   - Register drivers & customers (hash table)
 *   - Request a ride (queue: pending requests)
 *   - Find nearest driver (min-heap by distance)
 *   - Calculate shortest fare (graph Dijkstra BFS)
 *   - Complete trip -> push to history stack
 *   - View trip history (stack: most recent first)
 *   - Mark driver available/unavailable
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
#include <climits>
#include <algorithm>
#include <cstdlib>
#include <cmath>
using namespace std;

// ============================================================================
// SECTION 1: ENTITY STRUCTS
// ============================================================================
struct Location {
    string locId;
    string name;
    double x, y;       // Coordinates for distance calc

    Location() : x(0), y(0) {}
    Location(string id, string n, double cx, double cy)
        : locId(id), name(n), x(cx), y(cy) {}
};

struct Edge {
    string destId;
    int weight;        // Distance in km or travel time in minutes

    Edge() : weight(0) {}
    Edge(string d, int w) : destId(d), weight(w) {}
};

struct Driver {
    string driverId;
    string name;
    string phone;
    string currentLocId;
    double rating;
    bool available;
    string carModel;

    Driver() : rating(0.0), available(true) {}
    Driver(string id, string n, string p, string loc, double r, string car)
        : driverId(id), name(n), phone(p), currentLocId(loc),
          rating(r), available(true), carModel(car) {}
};

struct Customer {
    string custId;
    string name;
    string phone;

    Customer() {}
    Customer(string id, string n, string p)
        : custId(id), name(n), phone(p) {}
};

struct Trip {
    string tripId;
    string custId;
    string custName;
    string driverId;
    string driverName;
    string pickupLoc;
    string dropLoc;
    double fare;
    string status;       // PENDING, ACTIVE, COMPLETED

    Trip() : fare(0.0) {}
    Trip(string tid, string cid, string cn, string did, string dn,
         string p, string d, double f, string s)
        : tripId(tid), custId(cid), custName(cn), driverId(did),
          driverName(dn), pickupLoc(p), dropLoc(d), fare(f), status(s) {}
};

struct RideRequest {
    string requestId;
    string custId;
    string pickupLocId;
    string dropLocId;
    string custName;

    RideRequest() {}
    RideRequest(string rid, string cid, string p, string dl, string cn)
        : requestId(rid), custId(cid), pickupLocId(p),
          dropLocId(dl), custName(cn) {}
};

// For min-heap: nearest driver
struct DriverDist {
    string driverId;
    string driverName;
    double distance;
    double rating;

    DriverDist() : distance(0), rating(0) {}
    DriverDist(string id, string n, double d, double r)
        : driverId(id), driverName(n), distance(d), rating(r) {}
};

struct CompareDriverDist {
    bool operator()(const DriverDist& a, const DriverDist& b) {
        return a.distance > b.distance;  // Min-heap: smaller distance first
    }
};

// ============================================================================
// SECTION 2: GRAPH - ADJACENCY LIST FOR CITY MAP
//
// Why Graph?
//   - Locations are vertices, roads are edges with weights (distance/time)
//   - Adjacency list for memory-efficient sparse graph
//   - Dijkstra's BFS algorithm for shortest path routing
// ============================================================================
class CityGraph {
private:
    unordered_map<string, vector<Edge>> adjList;  // Adjacency list
    unordered_map<string, Location> locations;    // Vertex data

public:
    void addLocation(Location loc) {
        locations[loc.locId] = loc;
        if (adjList.find(loc.locId) == adjList.end())
            adjList[loc.locId] = vector<Edge>();
    }

    void addRoad(string fromId, string toId, int weight, bool twoWay = true) {
        adjList[fromId].push_back(Edge(toId, weight));
        if (twoWay)
            adjList[toId].push_back(Edge(fromId, weight));
    }

    // Dijkstra's algorithm: find shortest path distance
    int shortestDistance(string fromId, string toId) {
        if (locations.find(fromId) == locations.end() ||
            locations.find(toId) == locations.end())
            return -1;

        unordered_map<string, int> dist;
        for (auto& pair : adjList)
            dist[pair.first] = INT_MAX;
        dist[fromId] = 0;

        // Min-heap: (distance, locationId)
        priority_queue<pair<int, string>,
                       vector<pair<int, string>>,
                       greater<pair<int, string>>> pq;
        pq.push({0, fromId});

        while (!pq.empty()) {
            auto top = pq.top(); pq.pop();
            int d = top.first;
            string u = top.second;

            if (d > dist[u]) continue;
            if (u == toId) return d;

            for (Edge& e : adjList[u]) {
                int nd = d + e.weight;
                if (nd < dist[e.destId]) {
                    dist[e.destId] = nd;
                    pq.push({nd, e.destId});
                }
            }
        }
        return (dist[toId] == INT_MAX) ? -1 : dist[toId];
    }

    // Euclidean distance between two locations
    double euclideanDist(string fromId, string toId) {
        auto fit = locations.find(fromId);
        auto tit = locations.find(toId);
        if (fit == locations.end() || tit == locations.end()) return -1;
        double dx = fit->second.x - tit->second.x;
        double dy = fit->second.y - tit->second.y;
        return sqrt(dx * dx + dy * dy);
    }

    string getLocationName(string locId) {
        auto it = locations.find(locId);
        return (it != locations.end()) ? it->second.name : "Unknown";
    }

    void showMap() {
        cout << "  City Map (Graph - Adjacency List):\n";
        for (auto& pair : adjList) {
            string locName = getLocationName(pair.first);
            cout << "  " << pair.first << " (" << locName << ") -> ";
            for (Edge& e : pair.second) {
                cout << getLocationName(e.destId) << "(" << e.weight << "km) ";
            }
            cout << "\n";
        }
    }

    void showLocations() {
        cout << "  Locations:\n";
        for (auto& pair : locations) {
            cout << "  " << pair.first << " - " << pair.second.name
                 << " (" << pair.second.x << "," << pair.second.y << ")\n";
        }
    }
};

// ============================================================================
// SECTION 3: MAIN CAB BOOKING SYSTEM
// ============================================================================
class CabBookingSystem {
private:
    CityGraph graph;                                // Graph route network
    unordered_map<string, Driver> drivers;          // Hash: driverId -> Driver
    unordered_map<string, Customer> customers;       // Hash: custId -> Customer
    queue<RideRequest> rideRequests;                 // Queue: pending requests
    stack<Trip> tripHistory;                         // Stack: completed trips
    int nextTripNum;

public:
    CabBookingSystem() : nextTripNum(1001) {
        seedData();
    }

    void seedData() {
        // Locations (with coordinates)
        graph.addLocation(Location("L01", "Central Station", 0, 0));
        graph.addLocation(Location("L02", "Mall Road", 2, 1));
        graph.addLocation(Location("L03", "University", 1, 3));
        graph.addLocation(Location("L04", "Hospital", -1, 2));
        graph.addLocation(Location("L05", "Airport", 4, 0));
        graph.addLocation(Location("L06", "Tech Park", 3, 2));
        graph.addLocation(Location("L07", "City Center", 1, 1));
        graph.addLocation(Location("L08", "Railway Station", -2, 0));
        graph.addLocation(Location("L09", "Beach Road", 2, -1));
        graph.addLocation(Location("L10", "Market Square", 0, -1));

        // Roads (weighted edges)
        graph.addRoad("L01", "L02", 3);
        graph.addRoad("L01", "L07", 2);
        graph.addRoad("L01", "L08", 4);
        graph.addRoad("L01", "L10", 2);
        graph.addRoad("L02", "L03", 3);
        graph.addRoad("L02", "L05", 3);
        graph.addRoad("L02", "L06", 2);
        graph.addRoad("L02", "L07", 2);
        graph.addRoad("L03", "L04", 3);
        graph.addRoad("L03", "L07", 2);
        graph.addRoad("L04", "L07", 2);
        graph.addRoad("L04", "L08", 3);
        graph.addRoad("L05", "L06", 2);
        graph.addRoad("L06", "L07", 2);
        graph.addRoad("L08", "L10", 3);
        graph.addRoad("L09", "L10", 2);

        // Drivers
        drivers["D001"] = Driver("D001", "Raj Patel", "9876543210", "L01", 4.8, "Sedan");
        drivers["D002"] = Driver("D002", "Priya Singh", "9876543211", "L03", 4.9, "Hatchback");
        drivers["D003"] = Driver("D003", "Amit Kumar", "9876543212", "L05", 4.7, "SUV");
        drivers["D004"] = Driver("D004", "Sneha Sharma", "9876543213", "L07", 4.6, "Sedan");
        drivers["D005"] = Driver("D005", "Vikram Reddy", "9876543214", "L09", 4.5, "Hatchback");

        // Customers
        customers["C001"] = Customer("C001", "Ananya Gupta", "9988776655");
        customers["C002"] = Customer("C002", "Rahul Verma", "9988776644");
        customers["C003"] = Customer("C003", "Neha Joshi", "9988776633");
    }

    // ====================================================================
    // GRAPH OPERATIONS
    // ====================================================================
    void showMap() {
        graph.showMap();
        graph.showLocations();
    }

    int getShortestPath(string from, string to) {
        return graph.shortestDistance(from, to);
    }

    // ====================================================================
    // DRIVER / CUSTOMER HASH TABLE OPERATIONS
    // ====================================================================
    void showDrivers() {
        cout << "  Drivers (Hash Table):\n";
        cout << "  " << left << setw(8) << "ID"
             << setw(20) << "Name"
             << setw(12) << "Location"
             << setw(6) << "Rating"
             << setw(10) << "Status"
             << "Car\n";
        cout << "  " << string(65, '-') << "\n";
        for (auto& pair : drivers) {
            Driver& d = pair.second;
            cout << "  " << left << setw(8) << d.driverId
                 << setw(20) << d.name
                 << setw(12) << graph.getLocationName(d.currentLocId)
                 << setw(6) << d.rating
                 << setw(10) << (d.available ? "Available" : "Busy")
                 << d.carModel << "\n";
        }
    }

    // ====================================================================
    // FIND NEAREST DRIVER: Min-Heap priority_queue
    //   - Calculate Euclidean distance from pickup
    //   - Insert into min-heap, extract nearest
    //   - O(n) to build heap, O(log n) to extract
    // ====================================================================
    void findNearestDriver(string pickupLocId) {
        priority_queue<DriverDist, vector<DriverDist>, CompareDriverDist> pq;

        for (auto& pair : drivers) {
            Driver& d = pair.second;
            if (d.available) {
                double dist = graph.euclideanDist(pickupLocId, d.currentLocId);
                if (dist >= 0) {
                    pq.push(DriverDist(d.driverId, d.name, dist, d.rating));
                }
            }
        }

        if (pq.empty()) {
            cout << "  [INFO] No available drivers nearby.\n";
            return;
        }

        cout << "  Nearest Drivers (Min-Heap by Distance):\n";
        cout << "  " << left << setw(20) << "Name"
             << setw(12) << "Distance"
             << "Rating\n";
        cout << "  " << string(40, '-') << "\n";
        int count = 0;
        while (!pq.empty() && count < 5) {
            DriverDist dd = pq.top(); pq.pop();
            cout << "  " << left << setw(20) << dd.driverName
                 << setw(12) << fixed << setprecision(2) << dd.distance
                 << dd.rating << "\n";
            count++;
        }
    }

    // ====================================================================
    // REQUEST RIDE: Queue - FIFO processing
    // ====================================================================
    void requestRide(string custId, string pickupId, string dropId) {
        auto cit = customers.find(custId);
        if (cit == customers.end()) {
            cout << "  [ERROR] Customer not found.\n";
            return;
        }
        string reqId = "REQ" + to_string(nextTripNum++);
        rideRequests.push(RideRequest(reqId, custId, pickupId, dropId, cit->second.name));
        cout << "  [OK] Ride requested by " << cit->second.name
             << " (from " << graph.getLocationName(pickupId)
             << " to " << graph.getLocationName(dropId) << ")\n";
    }

    // ====================================================================
    // PROCESS NEXT RIDE: Dequeue request, assign driver, calculate fare
    // ====================================================================
    void processNextRide() {
        if (rideRequests.empty()) {
            cout << "  [INFO] No pending ride requests.\n";
            return;
        }

        RideRequest req = rideRequests.front();
        rideRequests.pop();

        // Find nearest available driver
        string assignedDriverId;
        string assignedDriverName;
        double minDist = 1e18;

        for (auto& pair : drivers) {
            Driver& d = pair.second;
            if (d.available) {
                double dist = graph.euclideanDist(req.pickupLocId, d.currentLocId);
                if (dist >= 0 && dist < minDist) {
                    minDist = dist;
                    assignedDriverId = d.driverId;
                    assignedDriverName = d.name;
                }
            }
        }

        if (assignedDriverId.empty()) {
            cout << "  [INFO] No drivers available. Re-queueing request.\n";
            rideRequests.push(req);
            return;
        }

        // Calculate shortest path fare
        int pathDist = graph.shortestDistance(req.pickupLocId, req.dropLocId);
        if (pathDist <= 0) {
            // Fall back to Euclidean
            pathDist = (int)graph.euclideanDist(req.pickupLocId, req.dropLocId);
            if (pathDist <= 0) pathDist = 5;
        }

        double fare = pathDist * 12.0 + 25.0; // Base fare formula
        drivers[assignedDriverId].available = false;
        drivers[assignedDriverId].currentLocId = req.dropLocId;

        string tripId = "TRIP" + to_string(nextTripNum++);
        Trip trip(tripId, req.custId, req.custName, assignedDriverId,
                  assignedDriverName, graph.getLocationName(req.pickupLocId),
                  graph.getLocationName(req.dropLocId), fare, "COMPLETED");

        tripHistory.push(trip);    // Stack: history
        drivers[assignedDriverId].available = true; // Auto-available after trip

        cout << "  [RIDE ASSIGNED]\n";
        cout << "  Customer: " << req.custName << "\n";
        cout << "  Driver: " << assignedDriverName << "\n";
        cout << "  Route: " << graph.getLocationName(req.pickupLocId)
             << " -> " << graph.getLocationName(req.dropLocId) << "\n";
        cout << "  Distance: " << pathDist << " km\n";
        cout << "  Fare: Rs. " << fixed << setprecision(2) << fare << "\n";
    }

    // ====================================================================
    // TRIP HISTORY: Stack (most recent first)
    // ====================================================================
    void showTripHistory() {
        if (tripHistory.empty()) {
            cout << "  [INFO] No trip history.\n";
            return;
        }
        stack<Trip> temp = tripHistory;
        cout << "  Trip History (most recent first):\n";
        while (!temp.empty()) {
            Trip t = temp.top(); temp.pop();
            cout << "  " << t.tripId << " | " << t.custName
                 << " with " << t.driverName
                 << " | " << t.pickupLoc << " -> " << t.dropLoc
                 << " | Rs." << fixed << setprecision(2) << t.fare << "\n";
        }
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   CAB / TAXI BOOKING SYSTEM\n";
        cout << "   Data Structures: Graph (Routes),\n";
        cout << "   Queue (Requests), Hash Table (Drivers),\n";
        cout << "   Stack (History), Min-Heap (Nearest)\n";
        cout << "=============================================\n\n";

        int choice, pathDist;
        string custId, pickupId, dropId, driverId;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [GRAPH - City Map]\n";
            cout << "    1. Show City Map & Locations\n";
            cout << "    2. Shortest Path (Dijkstra)\n";
            cout << "  [HASH TABLE - Drivers]\n";
            cout << "    3. Show All Drivers\n";
            cout << "  [MIN-HEAP - Nearest Driver]\n";
            cout << "    4. Find Nearest Available Drivers\n";
            cout << "  [QUEUE - Ride Requests]\n";
            cout << "    5. Request a Ride\n";
            cout << "    6. Process Next Ride (Dequeue)\n";
            cout << "  [STACK - Trip History]\n";
            cout << "    7. Trip History (Most Recent)\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: showMap(); break;
                case 2:
                    cout << "  Pickup Location ID: "; getline(cin, pickupId);
                    cout << "  Drop Location ID: "; getline(cin, dropId);
                    pathDist = getShortestPath(pickupId, dropId);
                    if (pathDist < 0)
                        cout << "  [ERROR] No path found.\n";
                    else {
                        cout << "  Shortest distance: " << pathDist << " km\n";
                        cout << "  Estimated fare: Rs. "
                             << fixed << setprecision(2) << (pathDist * 12.0 + 25.0) << "\n";
                    }
                    break;
                case 3: showDrivers(); break;
                case 4:
                    cout << "  Pickup Location ID: "; getline(cin, pickupId);
                    findNearestDriver(pickupId);
                    break;
                case 5:
                    cout << "  Customer ID: "; getline(cin, custId);
                    cout << "  Pickup Location ID: "; getline(cin, pickupId);
                    cout << "  Drop Location ID: "; getline(cin, dropId);
                    requestRide(custId, pickupId, dropId);
                    break;
                case 6: processNextRide(); break;
                case 7: showTripHistory(); break;
                case 0: cout << "  Safe travels! Goodbye.\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    CabBookingSystem system;
    system.run();
    return 0;
}
