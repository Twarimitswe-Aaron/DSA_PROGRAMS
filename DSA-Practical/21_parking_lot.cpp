/*
 * ============================================================================
 * FILE: 21_parking_lot.cpp
 * ============================================================================
 * PARKING LOT MANAGEMENT SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: High. Parking lot systems test your understanding of
 * multiple data structures (Stack, Queue, Array, Linked List, Hash Table)
 * working together in a spatial/physical context.
 *
 * DATA STRUCTURES USED:
 *   1. Stack              - Row-level entry/exit control (LIFO per row)
 *                           Last car to enter a row must leave first.
 *   2. Queue              - Waiting vehicle processing (FIFO order)
 *                           Cars wait in arrival order for a free slot.
 *   3. Array              - Parking slot grid (fixed-size 3D array)
 *                           O(1) access to any slot's status.
 *   4. Singly Linked List - Vehicle records with parking session history.
 *                           Each VehicleRecord has a chain of ParkingSession
 *                           nodes representing all completed parking events.
 *   5. Hash Table         - License plate lookup (O(1) via unordered_map)
 *                           Maps plate string to location "floor row col".
 *
 * FUNCTIONALITIES:
 *   - Car entry with nearest-free-slot assignment
 *   - Car exit with fee calculation and waiting queue processing
 *   - Display parking lot layout (array grid visualization with stack info)
 *   - Search vehicle by license plate (hash table - O(1) instant find)
 *   - View vehicle parking history (linked list traversal)
 *   - View waiting queue (FIFO order)
 *   - Multi-floor/multi-row with LIFO constraint per row
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>          // For waiting vehicle queue (FIFO)
#include <stack>          // For per-row LIFO entry/exit tracking
#include <vector>
#include <unordered_map>  // For hash table (license plate -> location)
#include <iomanip>        // For formatted output (setw, setprecision)
#include <ctime>          // For parking session timestamps
#include <sstream>        // For string/stream conversions
#include <algorithm>      // For utility operations
using namespace std;

// ============================================================================
// SECTION 1: CONFIGURATION CONSTANTS
// ============================================================================

const int NUM_FLOORS = 2;         // Number of floors in the parking facility
const int ROWS_PER_FLOOR = 3;    // Number of rows on each floor
const int SLOTS_PER_ROW = 5;     // Number of parking slots per row
const double HOURLY_RATE = 5.0;  // $5.00 per hour parking fee
const double MINIMUM_FEE = 2.0;  // Minimum parking fee ($2.00)

// Total capacity of parking lot (2 x 3 x 5 = 30 slots)
const int TOTAL_SLOTS = NUM_FLOORS * ROWS_PER_FLOOR * SLOTS_PER_ROW;

// ============================================================================
// SECTION 2: CORE DATA STRUCTURES
// ============================================================================

// -------------------------------------------------------------------
// ParkingSession - Represents one completed parking event.
// Stored as a node in a singly linked list attached to each vehicle.
// Records entry/exit timestamps, location, and the fee charged.
// -------------------------------------------------------------------
struct ParkingSession {
    time_t entryTime;         // When vehicle entered the lot
    time_t exitTime;          // When vehicle left the lot
    int floor;                // Floor where parked (0-based)
    int row;                  // Row where parked (0-based)
    int col;                  // Column where parked (0-based)
    double fee;               // Fee charged for this session
    ParkingSession* next;     // Pointer to next session in history chain

    ParkingSession(time_t entry, time_t exit, int f, int r, int c, double f2)
        : entryTime(entry), exitTime(exit), floor(f), row(r), col(c),
          fee(f2), next(nullptr) {}
};

// -------------------------------------------------------------------
// VehicleRecord - Node in the master singly linked list of all vehicles
// that have ever used the parking lot. Each node holds the vehicle's
// info and a linked list of ParkingSession nodes (its history).
// -------------------------------------------------------------------
struct VehicleRecord {
    string licensePlate;         // Vehicle license plate (unique identifier)
    string ownerName;            // Vehicle owner's full name
    int totalVisits;             // Count of completed parking sessions
    ParkingSession* sessionsHead; // Head of parking history linked list
    VehicleRecord* next;         // Next vehicle in the master linked list

    VehicleRecord(string plate, string owner)
        : licensePlate(plate), ownerName(owner), totalVisits(0),
          sessionsHead(nullptr), next(nullptr) {}
};

// -------------------------------------------------------------------
// WaitingVehicle - Represents a vehicle waiting for a free slot.
// Stores both plate and owner so the queue can be auto-processed
// without additional user prompts when a slot becomes available.
// -------------------------------------------------------------------
struct WaitingVehicle {
    string licensePlate;
    string ownerName;

    WaitingVehicle(string plate, string owner)
        : licensePlate(plate), ownerName(owner) {}
};

// -------------------------------------------------------------------
// ParkingSlot - Represents a single parking space in the facility.
// The lot is modeled as a 3D array [floor][row][col] of these slots.
// Each slot tracks occupancy status, the parked vehicle's plate, and
// the entry timestamp for fee calculation.
// -------------------------------------------------------------------
struct ParkingSlot {
    int floor;             // Floor number (0-based)
    int row;               // Row number (0-based)
    int col;               // Column number (0-based)
    bool occupied;         // Whether a vehicle is currently parked here
    string licensePlate;   // License plate of parked vehicle (empty if free)
    time_t entryTime;      // Timestamp when vehicle was parked

    ParkingSlot()
        : floor(0), row(0), col(0), occupied(false),
          licensePlate(""), entryTime(0) {}
};

// ============================================================================
// SECTION 3: PARKING LOT SYSTEM CLASS
//
// Integrates ALL FIVE data structures into a unified parking management
// system with interactive menu-driven console interface.
// ============================================================================

class ParkingLot {
private:
    // ==================================================================
    // DATA STRUCTURE 1: ARRAY - Parking Slot Map (3D fixed-size grid)
    //
    // A 3D array of ParkingSlot structs indexed by [floor][row][col].
    // Provides O(1) access to any slot's occupancy status, the vehicle
    // parked there, and its entry time for fee calculation.
    // ==================================================================
    ParkingSlot slots[NUM_FLOORS][ROWS_PER_FLOOR][SLOTS_PER_ROW];

    // ==================================================================
    // DATA STRUCTURE 2: STACK - Per-row entry/exit control (LIFO)
    //
    // Each row has a stack<int> tracking the column indices of vehicles
    // currently parked in that row. The LIFO constraint means the last
    // vehicle to enter a row must be the first to leave. This enforces
    // the fundamental LIFO property of stacks in a physical context.
    // ==================================================================
    stack<int> rowStacks[NUM_FLOORS][ROWS_PER_FLOOR];

    // ==================================================================
    // DATA STRUCTURE 3: QUEUE - Waiting vehicles (FIFO)
    //
    // When the parking lot is full, arriving vehicles join the waiting
    // queue. They are processed strictly in FIFO (First-In-First-Out)
    // order as slots become available after a vehicle exits.
    // ==================================================================
    queue<WaitingVehicle> waitingQueue;

    // ==================================================================
    // DATA STRUCTURE 4: HASH TABLE - License plate lookup (O(1))
    //
    // unordered_map<string, string> maps a license plate string to a
    // location string formatted as "floor row col". This enables
    // instant O(1) average lookup without scanning the entire grid.
    // ==================================================================
    unordered_map<string, string> plateToLocation;

    // ==================================================================
    // DATA STRUCTURE 5: SINGLY LINKED LIST - Vehicle records/history
    //
    // Master linked list of VehicleRecord nodes. Each node holds vehicle
    // information (plate, owner) and a chain of ParkingSession nodes
    // representing the vehicle's complete parking history.
    // ==================================================================
    VehicleRecord* vehicleListHead;

    // -------------------------------------------------------------------
    // Helper: Find a vehicle record by license plate in the linked list.
    // Traverses the master list and returns a pointer to the matching
    // VehicleRecord node, or nullptr if no record exists.
    // Time complexity: O(n) where n is the number of unique vehicles.
    // -------------------------------------------------------------------
    VehicleRecord* findVehicle(const string& plate) {
        VehicleRecord* current = vehicleListHead;
        while (current) {
            if (current->licensePlate == plate) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    // -------------------------------------------------------------------
    // Helper: Find an existing vehicle record or create a new one.
    // If the vehicle already has a record, returns it. Otherwise creates
    // a new VehicleRecord node and inserts it at the head of the linked
    // list (O(1) insertion at head). The owner name is used for new
    // records but ignored if the vehicle already exists.
    // -------------------------------------------------------------------
    VehicleRecord* findOrCreateVehicle(const string& plate,
                                       const string& owner) {
        VehicleRecord* existing = findVehicle(plate);
        if (existing) {
            return existing;
        }
        // Insert new node at head of linked list
        VehicleRecord* newNode = new VehicleRecord(plate, owner);
        newNode->next = vehicleListHead;
        vehicleListHead = newNode;
        return newNode;
    }

    // -------------------------------------------------------------------
    // Helper: Append a ParkingSession to a vehicle's history linked list.
    // Traverses to the end of the session list and appends the new node.
    // This maintains chronological order of parking sessions.
    // Time complexity: O(m) where m is the number of sessions for that
    // vehicle. A tail pointer could optimize this to O(1).
    // -------------------------------------------------------------------
    void addParkingSession(VehicleRecord* vehicle, time_t entry,
                           time_t exit, int f, int r, int c, double fee) {
        ParkingSession* session = new ParkingSession(entry, exit,
                                                     f, r, c, fee);
        if (!vehicle->sessionsHead) {
            vehicle->sessionsHead = session;
        } else {
            ParkingSession* temp = vehicle->sessionsHead;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = session;
        }
        vehicle->totalVisits++;
    }

    // -------------------------------------------------------------------
    // Helper: Find the nearest free parking slot by scanning the array.
    // Searches floor-by-floor, then row-by-row, then column-by-column.
    // "Nearest" means the lowest floor, lowest row, lowest column that
    // is free. Returns true if found and sets the output parameters.
    // -------------------------------------------------------------------
    bool findNearestFreeSlot(int& outFloor, int& outRow, int& outCol) {
        for (int f = 0; f < NUM_FLOORS; f++) {
            for (int r = 0; r < ROWS_PER_FLOOR; r++) {
                for (int c = 0; c < SLOTS_PER_ROW; c++) {
                    if (!slots[f][r][c].occupied) {
                        outFloor = f;
                        outRow = r;
                        outCol = c;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // -------------------------------------------------------------------
    // Helper: Process the waiting queue after a slot becomes free.
    // Assigns freed slots to waiting vehicles in strict FIFO order.
    // Stops when either the queue is empty or no free slots remain.
    // Updates all data structures: array, stack, hash table, linked list.
    // -------------------------------------------------------------------
    void processWaitingQueue() {
        while (!waitingQueue.empty()) {
            int floor, row, col;
            if (!findNearestFreeSlot(floor, row, col)) {
                break;  // No more free slots available
            }

            // Dequeue the next waiting vehicle (FIFO order)
            WaitingVehicle waiting = waitingQueue.front();
            waitingQueue.pop();

            // Mark the slot as occupied in the array
            slots[floor][row][col].occupied = true;
            slots[floor][row][col].licensePlate = waiting.licensePlate;
            slots[floor][row][col].entryTime = time(nullptr);

            // Push column onto the row's LIFO stack
            rowStacks[floor][row].push(col);

            // Insert into hash table for O(1) lookup
            stringstream ss;
            ss << floor << " " << row << " " << col;
            plateToLocation[waiting.licensePlate] = ss.str();

            // Create or update vehicle record in linked list
            findOrCreateVehicle(waiting.licensePlate, waiting.ownerName);

            cout << "  [QUEUE] " << waiting.licensePlate
                 << " assigned to Floor " << (floor + 1)
                 << ", Row " << (row + 1)
                 << ", Slot " << (col + 1) << "\n";
        }

        if (waitingQueue.empty()) {
            cout << "  [INFO] Waiting queue is now empty.\n";
        } else {
            cout << "  [INFO] " << waitingQueue.size()
                 << " vehicle(s) still waiting.\n";
        }
    }

    // -------------------------------------------------------------------
    // Helper: Convert a time_t value to a readable string.
    // Uses ctime() for the conversion and strips the trailing newline
    // character that ctime() appends.
    // -------------------------------------------------------------------
    string timeToString(time_t t) {
        string s = ctime(&t);
        if (!s.empty() && s[s.length() - 1] == '\n') {
            s.pop_back();
        }
        return s;
    }

    // -------------------------------------------------------------------
    // Helper: Calculate parking fee based on elapsed time.
    // Charged at HOURLY_RATE per hour (or fraction thereof, rounded up).
    // Minimum fee is MINIMUM_FEE for any duration under one hour.
    // -------------------------------------------------------------------
    double calculateFee(time_t entry, time_t exit) {
        double seconds = difftime(exit, entry);
        double hours = seconds / 3600.0;
        // Ceiling: round up to the nearest whole hour
        int billableHours = static_cast<int>(hours);
        if (hours > static_cast<double>(billableHours)) {
            billableHours++;
        }
        if (billableHours < 1) {
            billableHours = 1;
        }
        double fee = static_cast<double>(billableHours) * HOURLY_RATE;
        if (fee < MINIMUM_FEE) {
            fee = MINIMUM_FEE;
        }
        return fee;
    }

    // -------------------------------------------------------------------
    // Helper: Count currently occupied slots by scanning the 3D array.
    // Iterates through every slot and tallies occupied ones.
    // Time complexity: O(TOTAL_SLOTS) = O(30) constant in practice.
    // -------------------------------------------------------------------
    int getOccupiedCount() {
        int count = 0;
        for (int f = 0; f < NUM_FLOORS; f++) {
            for (int r = 0; r < ROWS_PER_FLOOR; r++) {
                for (int c = 0; c < SLOTS_PER_ROW; c++) {
                    if (slots[f][r][c].occupied) {
                        count++;
                    }
                }
            }
        }
        return count;
    }

    // -------------------------------------------------------------------
    // Helper: Check if a vehicle is currently in the waiting queue.
    // Creates a copy of the queue and scans it (preserving the original).
    // Time complexity: O(n) where n is the queue size.
    // -------------------------------------------------------------------
    bool isInWaitingQueue(const string& plate) {
        queue<WaitingVehicle> temp = waitingQueue;
        while (!temp.empty()) {
            if (temp.front().licensePlate == plate) {
                return true;
            }
            temp.pop();
        }
        return false;
    }

    // -------------------------------------------------------------------
    // Helper: Compute total parking fees collected from a vehicle.
    // Traverses the vehicle's session linked list and sums all fees.
    // Used in history display and exit summary.
    // -------------------------------------------------------------------
    double getTotalFeesForVehicle(const string& plate) {
        VehicleRecord* vr = findVehicle(plate);
        if (!vr || !vr->sessionsHead) return 0.0;

        double total = 0.0;
        ParkingSession* session = vr->sessionsHead;
        while (session) {
            total += session->fee;
            session = session->next;
        }
        return total;
    }

public:
    // -------------------------------------------------------------------
    // Constructor: Initialize all parking slots as free and set their
    // position indices. The vehicle linked list starts as empty.
    // -------------------------------------------------------------------
    ParkingLot() : vehicleListHead(nullptr) {
        for (int f = 0; f < NUM_FLOORS; f++) {
            for (int r = 0; r < ROWS_PER_FLOOR; r++) {
                for (int c = 0; c < SLOTS_PER_ROW; c++) {
                    slots[f][r][c].floor = f;
                    slots[f][r][c].row = r;
                    slots[f][r][c].col = c;
                    slots[f][r][c].occupied = false;
                }
            }
        }
    }

    // -------------------------------------------------------------------
    // Destructor: Free all dynamically allocated memory.
    // Deletes all VehicleRecord nodes and their ParkingSession chains.
    // -------------------------------------------------------------------
    ~ParkingLot() {
        VehicleRecord* current = vehicleListHead;
        while (current) {
            // Delete the parking session history linked list
            ParkingSession* session = current->sessionsHead;
            while (session) {
                ParkingSession* toDelete = session;
                session = session->next;
                delete toDelete;
            }
            // Delete the vehicle record itself
            VehicleRecord* toDelete = current;
            current = current->next;
            delete toDelete;
        }
        vehicleListHead = nullptr;
    }

    // ==================================================================
    // FEATURE 1: Car Entry
    //
    // 1. Accept license plate and owner name
    // 2. Validate: check if already parked (hash table O(1))
    // 3. Validate: check if already waiting (queue scan O(n))
    // 4. Pre-fill owner name from linked list if available
    // 5. Find nearest free slot via array scan
    // 6. If found: mark occupied, push stack, add to hash table,
    //    create/update linked list record
    // 7. If full: add to waiting queue (FIFO)
    // ==================================================================
    void carEntry() {
        string plate, owner;
        cout << "  License Plate: ";
        getline(cin, plate);
        if (plate.empty()) {
            cout << "  [ERROR] License plate cannot be empty.\n";
            return;
        }

        // Hash table lookup - check if vehicle is already parked O(1)
        if (plateToLocation.find(plate) != plateToLocation.end()) {
            cout << "  [ERROR] Vehicle " << plate
                 << " is already parked in the lot.\n";
            return;
        }

        // Check if already in waiting queue
        if (isInWaitingQueue(plate)) {
            cout << "  [ERROR] Vehicle " << plate
                 << " is already in the waiting queue.\n";
            return;
        }

        // Pre-fill owner name if vehicle has a linked list record
        VehicleRecord* existing = findVehicle(plate);
        if (existing) {
            cout << "  Owner Name (Enter for: " << existing->ownerName << "): ";
            string input;
            getline(cin, input);
            if (input.empty()) {
                owner = existing->ownerName;
            } else {
                owner = input;
                existing->ownerName = owner;  // Update record
            }
        } else {
            cout << "  Owner Name: ";
            getline(cin, owner);
            if (owner.empty()) {
                owner = "Unknown";
            }
        }

        // Search for nearest free slot via array scan
        int floor, row, col;
        if (findNearestFreeSlot(floor, row, col)) {
            // Free slot found - park the vehicle
            slots[floor][row][col].occupied = true;
            slots[floor][row][col].licensePlate = plate;
            slots[floor][row][col].entryTime = time(nullptr);

            // Push column index onto the row's LIFO stack
            rowStacks[floor][row].push(col);

            // Add to hash table: plate -> "floor row col"
            stringstream ss;
            ss << floor << " " << row << " " << col;
            plateToLocation[plate] = ss.str();

            // Create or update vehicle record in the linked list
            findOrCreateVehicle(plate, owner);

            cout << "  [OK] " << plate << " parked at Floor " << (floor + 1)
                 << ", Row " << (row + 1) << ", Slot " << (col + 1) << "\n";
        } else {
            // No free slots - add to waiting queue (FIFO)
            waitingQueue.push(WaitingVehicle(plate, owner));
            cout << "  [INFO] Parking lot is full (" << TOTAL_SLOTS << "/"
                 << TOTAL_SLOTS << " occupied).\n";
            cout << "  [INFO] " << plate << " added to waiting queue (position "
                 << waitingQueue.size() << ").\n";
        }
    }

    // ==================================================================
    // FEATURE 2: Car Exit
    //
    // 1. Look up plate in hash table (O(1))
    // 2. Parse location string to get floor, row, col
    // 3. Enforce LIFO: verify column is top of row's stack
    // 4. Pop from stack, record exit time, calculate fee
    // 5. Add completed session to vehicle's history linked list
    // 6. Mark slot free, erase from hash table, pop stack
    // 7. Process waiting queue for the freed slot
    // ==================================================================
    void carExit() {
        string plate;
        cout << "  License Plate: ";
        getline(cin, plate);

        // Hash table lookup - O(1) average
        if (plateToLocation.find(plate) == plateToLocation.end()) {
            cout << "  [ERROR] Vehicle " << plate
                 << " is not currently in the parking lot.\n";
            return;
        }

        // Parse location string from hash table value
        string location = plateToLocation[plate];
        int floor, row, col;
        istringstream locationStream(location);
        locationStream >> floor >> row >> col;

        // Enforce LIFO constraint: the exiting vehicle must be the
        // last one that entered its row (top of the row's stack).
        if (rowStacks[floor][row].empty()) {
            cout << "  [ERROR] Internal error: row stack is empty.\n";
            return;
        }
        int topCol = rowStacks[floor][row].top();
        if (col != topCol) {
            cout << "  [ERROR] LIFO constraint violation: Vehicle " << plate
                 << " is not the last car in Row " << (row + 1)
                 << " on Floor " << (floor + 1) << ".\n";
            cout << "  [ERROR] The last car to enter this row (Slot "
                 << (topCol + 1) << ") must leave first.\n";
            return;
        }
        // Pop from the row's LIFO stack
        rowStacks[floor][row].pop();

        // Record exit time and calculate parking fee
        time_t entryTime = slots[floor][row][col].entryTime;
        time_t exitTime = time(nullptr);
        double fee = calculateFee(entryTime, exitTime);

        // Add completed session to vehicle's history linked list
        VehicleRecord* vehicle = findVehicle(plate);
        if (vehicle) {
            addParkingSession(vehicle, entryTime, exitTime,
                             floor, row, col, fee);
        }

        // Display exit summary
        cout << "  [OK] Vehicle " << plate << " exiting.\n";
        cout << "  Entry time: " << timeToString(entryTime) << "\n";
        cout << "  Exit time:  " << timeToString(exitTime) << "\n";
        double minutes = difftime(exitTime, entryTime) / 60.0;
        cout << "  Duration: " << fixed << setprecision(1) << minutes
             << " minutes\n";
        cout << "  Parking fee: $" << fixed << setprecision(2) << fee << "\n";

        // Mark the parking slot as free in the array
        slots[floor][row][col].occupied = false;
        slots[floor][row][col].licensePlate = "";
        slots[floor][row][col].entryTime = 0;

        // Remove from hash table
        plateToLocation.erase(plate);

        // Process waiting queue (FIFO) for the newly freed slot
        if (!waitingQueue.empty()) {
            cout << "  [INFO] Processing waiting queue...\n";
            processWaitingQueue();
        }
    }

    // ==================================================================
    // FEATURE 3: Display Parking Lot Layout
    //
    // Prints a visual grid of the entire parking lot. Shows occupied
    // slots with truncated license plates and free slots with their
    // coordinates. Also displays the LIFO stack order for each row
    // so the user can see which cars must leave first.
    // ==================================================================
    void displayLayout() {
        cout << "\n  ===== PARKING LOT LAYOUT =====\n";
        cout << "  Capacity: " << TOTAL_SLOTS << " slots ("
             << NUM_FLOORS << " floors x "
             << ROWS_PER_FLOOR << " rows x "
             << SLOTS_PER_ROW << " slots)\n\n";

        for (int f = 0; f < NUM_FLOORS; f++) {
            cout << "  FLOOR " << (f + 1) << ":\n";
            cout << "  " << string(65, '-') << "\n";

            for (int r = 0; r < ROWS_PER_FLOOR; r++) {
                cout << "  Row " << (r + 1) << ":  ";

                for (int c = 0; c < SLOTS_PER_ROW; c++) {
                    if (slots[f][r][c].occupied) {
                        string p = slots[f][r][c].licensePlate;
                        if (p.length() > 5) p = p.substr(0, 5);
                        cout << "[" << setw(5) << left << p << "]";
                    } else {
                        cout << "[(" << f << "," << r << "," << c << ")]";
                    }
                    if (c < SLOTS_PER_ROW - 1) cout << " ";
                }

                // Display stack contents for this row (LIFO order)
                cout << "  | Stack(top->bottom): ";
                if (!rowStacks[f][r].empty()) {
                    stack<int> temp = rowStacks[f][r];
                    vector<int> elems;
                    while (!temp.empty()) {
                        elems.push_back(temp.top());
                        temp.pop();
                    }
                    for (size_t i = 0; i < elems.size(); i++) {
                        cout << "S" << (elems[i] + 1);
                        if (i < elems.size() - 1) cout << "->";
                    }
                } else {
                    cout << "(empty)";
                }
                cout << "\n";
            }
            cout << "\n";
        }

        // Summary
        int occupied = getOccupiedCount();
        int freeSlots = TOTAL_SLOTS - occupied;
        cout << "  Summary: " << occupied << " occupied, "
             << freeSlots << " free\n";
        cout << "  Waiting queue: " << waitingQueue.size() << " vehicle(s)\n";
        cout << "  Legend: [PLATE] = Occupied, [(f,r,c)] = Free\n";
        cout << "  Stack shows exit order (left = must leave first)\n";
    }

    // ==================================================================
    // FEATURE 4: Search Vehicle by License Plate
    //
    // Uses the hash table (unordered_map) for O(1) average lookup of
    // a vehicle's current location. Also queries the linked list for
    // historical owner information and total visit count.
    // ==================================================================
    void searchVehicle() {
        string plate;
        cout << "  License Plate: ";
        getline(cin, plate);

        // Hash table lookup - O(1) average
        if (plateToLocation.find(plate) == plateToLocation.end()) {
            cout << "  [INFO] Vehicle " << plate
                 << " is not currently in the parking lot.\n";

            // Check for historical record in linked list
            VehicleRecord* vr = findVehicle(plate);
            if (vr) {
                cout << "  [INFO] Historical record: " << vr->ownerName
                     << ", " << vr->totalVisits << " past visit(s).\n";
            }
            return;
        }

        // Parse location from hash table value
        string location = plateToLocation[plate];
        int floor, row, col;
        istringstream locStream(location);
        locStream >> floor >> row >> col;

        cout << "  [FOUND] Vehicle " << plate << "\n";
        cout << "  Location:  Floor " << (floor + 1) << ", Row "
             << (row + 1) << ", Slot " << (col + 1) << "\n";
        cout << "  Parked at: "
             << timeToString(slots[floor][row][col].entryTime) << "\n";

        // Show owner and historical data from linked list
        VehicleRecord* vr = findVehicle(plate);
        if (vr) {
            cout << "  Owner:     " << vr->ownerName << "\n";
            cout << "  Visits:    " << (vr->totalVisits + 1)
                 << " (including current)\n";
        }
    }

    // ==================================================================
    // FEATURE 5: View Vehicle Parking History
    //
    // Traverses the vehicle's ParkingSession linked list to display
    // all completed parking events with entry/exit times, location,
    // and fee. Demonstrates singly linked list traversal.
    // ==================================================================
    void viewVehicleHistory() {
        string plate;
        cout << "  License Plate: ";
        getline(cin, plate);

        // Find vehicle record in the master linked list
        VehicleRecord* vr = findVehicle(plate);
        if (!vr) {
            cout << "  [INFO] No records found for vehicle " << plate << ".\n";
            return;
        }

        bool currentlyParked = (plateToLocation.find(plate)
                                != plateToLocation.end());

        cout << "  Vehicle: " << vr->licensePlate
             << " (Owner: " << vr->ownerName << ")\n";
        cout << "  Completed visits: " << vr->totalVisits << "\n";

        if (!vr->sessionsHead) {
            if (currentlyParked) {
                cout << "  [INFO] Vehicle is on its first visit (still parked).\n";
            } else {
                cout << "  [INFO] No completed parking sessions.\n";
            }
            return;
        }

        // Table header
        cout << "\n  " << left << setw(18) << "Entry Time"
             << setw(18) << "Exit Time"
             << setw(10) << "Location"
             << "Fee\n";
        cout << "  " << string(65, '-') << "\n";

        // Traverse the session linked list
        ParkingSession* session = vr->sessionsHead;
        while (session) {
            string entryStr = timeToString(session->entryTime).substr(4, 15);
            string exitStr = timeToString(session->exitTime).substr(4, 15);

            cout << "  " << left << setw(18) << entryStr
                 << setw(18) << exitStr
                 << "F" << (session->floor + 1)
                 << "R" << (session->row + 1)
                 << "S" << (session->col + 1) << "   "
                 << "$" << fixed << setprecision(2) << session->fee << "\n";
            session = session->next;
        }

        cout << "\n  Total fees paid: $"
             << fixed << setprecision(2)
             << getTotalFeesForVehicle(plate) << "\n";
    }

    // ==================================================================
    // FEATURE 6: View Waiting Queue
    //
    // Displays all vehicles currently waiting for a parking slot in
    // FIFO (First-In-First-Out) order. Creates a copy of the queue
    // to preserve the original while reading its contents.
    // ==================================================================
    void viewWaitingQueue() {
        if (waitingQueue.empty()) {
            cout << "  [INFO] Waiting queue is empty.\n";
            return;
        }

        cout << "  Waiting Queue (" << waitingQueue.size()
             << " vehicle(s)):\n";

        // Create a copy so we don't modify the original queue
        queue<WaitingVehicle> temp = waitingQueue;
        int position = 1;

        while (!temp.empty()) {
            WaitingVehicle v = temp.front();
            temp.pop();
            cout << "  " << position << ". " << left << setw(12)
                 << v.licensePlate << " (Owner: " << v.ownerName << ")\n";
            position++;
        }
    }

    // ==================================================================
    // FEATURE 7: Display All Vehicle Records
    //
    // Traverses the master linked list of all vehicles that have ever
    // used the parking lot. Shows each vehicle's plate, owner, visit
    // count, and current status (parked, waiting, or not present).
    // ==================================================================
    void displayAllVehicles() {
        if (!vehicleListHead) {
            cout << "  [INFO] No vehicle records in the system.\n";
            return;
        }

        int count = 0;
        VehicleRecord* temp = vehicleListHead;
        while (temp) { count++; temp = temp->next; }

        cout << "  Vehicle Records (" << count << " total):\n";
        cout << "  " << left << setw(15) << "Plate"
             << setw(20) << "Owner"
             << setw(10) << "Visits"
             << "Status\n";
        cout << "  " << string(60, '-') << "\n";

        // Traverse the linked list
        VehicleRecord* vr = vehicleListHead;
        while (vr) {
            bool inLot = (plateToLocation.find(vr->licensePlate)
                         != plateToLocation.end());
            bool inQueue = isInWaitingQueue(vr->licensePlate);

            string status;
            if (inLot)       status = "PARKED";
            else if (inQueue) status = "WAITING";
            else             status = "NOT HERE";

            cout << "  " << left << setw(15) << vr->licensePlate
                 << setw(20) << vr->ownerName
                 << setw(10) << vr->totalVisits
                 << status << "\n";
            vr = vr->next;
        }
    }

    // ==================================================================
    // RUN - Interactive menu-driven console interface
    //
    // Displays a menu, reads user input, and dispatches to the
    // corresponding feature function. Loops until exit (0).
    // ==================================================================
    void run() {
        cout << "\n=============================================\n";
        cout << "   PARKING LOT MANAGEMENT SYSTEM\n";
        cout << "   DSA: Stack | Queue | Array | LinkedList | Hash\n";
        cout << "=============================================\n";
        cout << "   " << NUM_FLOORS << " floors, " << ROWS_PER_FLOOR
             << " rows/floor, " << SLOTS_PER_ROW << " slots/row\n";
        cout << "   Total capacity: " << TOTAL_SLOTS << " vehicles\n\n";

        int choice;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  1. Car Entry (Park Vehicle)\n";
            cout << "  2. Car Exit  (Remove Vehicle)\n";
            cout << "  3. Display Parking Layout\n";
            cout << "  4. Search Vehicle by Plate (Hash Table)\n";
            cout << "  5. View Vehicle Parking History (Linked List)\n";
            cout << "  6. View Waiting Queue (FIFO)\n";
            cout << "  7. Display All Vehicle Records\n";
            cout << "  0. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore();  // Clear newline from input buffer

            switch (choice) {
                case 1:
                    carEntry();
                    break;

                case 2:
                    carExit();
                    break;

                case 3:
                    displayLayout();
                    break;

                case 4:
                    searchVehicle();
                    break;

                case 5:
                    viewVehicleHistory();
                    break;

                case 6:
                    viewWaitingQueue();
                    break;

                case 7:
                    displayAllVehicles();
                    break;

                case 0:
                    // Calculate total revenue before exiting
                    {
                        double grandTotal = 0.0;
                        VehicleRecord* vr = vehicleListHead;
                        while (vr) {
                            grandTotal += getTotalFeesForVehicle(
                                              vr->licensePlate);
                            vr = vr->next;
                        }
                        cout << "\n  Exiting. Total revenue: $"
                             << fixed << setprecision(2)
                             << grandTotal << "\n";
                        cout << "  Goodbye!\n";
                    }
                    break;

                default:
                    cout << "  [ERROR] Invalid choice (0-7).\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION - Entry point of the program
// ============================================================================
int main() {
    ParkingLot parkingLot;
    parkingLot.run();
    return 0;
}
