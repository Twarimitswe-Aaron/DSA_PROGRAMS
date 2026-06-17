/*
 * ============================================================================
 * FILE: 23_blood_bank.cpp
 * ============================================================================
 * BLOOD BANK MANAGEMENT SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Very High. Blood bank / hospital management is a commonly
 * asked DSA practical exam project. Tests understanding of MULTIPLE
 * data structures working together in a time-sensitive medical context.
 *
 * DATA STRUCTURES USED:
 *   1. Singly Linked List  - Blood inventory (each bag/unit is a node)
 *   2. Queue               - Patient blood requests (FIFO by arrival/urgency)
 *   3. Binary Search Tree  - Donor records (sorted by donor ID, O(log n))
 *   4. Stack               - Issue history / undo last issue (LIFO)
 *   5. Hash Table          - Blood group stock lookup (O(1) by group)
 *
 * FUNCTIONALITIES:
 *   - Add blood units to inventory (linked list - new donations)
 *   - Search blood by group (hash table - O(1) stock check)
 *   - Process patient blood request (queue - FIFO urgent handling)
 *   - Register/manage donors (BST - sorted by ID)
 *   - Issue blood to patient (with undo via stack)
 *   - View inventory levels and low-stock alerts
 *   - Display all donors sorted by ID (BST in-order)
 *   - Track expiration dates (remove expired units)
 *   - View issue history (stack - most recent first)
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>         // For patient blood request queue (FIFO)
#include <stack>         // For issue history / undo stack (LIFO)
#include <vector>
#include <unordered_map> // For hash table (blood group -> stock count)
#include <iomanip>       // For formatted table output
#include <ctime>         // For date/time handling (expiry, donation dates)
#include <sstream>       // For string stream parsing dates
using namespace std;

// ============================================================================
// SECTION 1: DATA STRUCTURES USED BY THE SYSTEM
// ============================================================================

// -------------------------------------------------------------------
// Date utility: Simple date structure for tracking donations and expiry.
// We use our own struct instead of <chrono> for C++11 compatibility
// and clarity in a DSA exam context.
// -------------------------------------------------------------------
struct Date {
    int day;
    int month;
    int year;

    Date() : day(0), month(0), year(0) {}

    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    // -------------------------------------------------------------------
    // Create a Date from a string in DD/MM/YYYY format
    // -------------------------------------------------------------------
    static Date fromString(const string& dateStr) {
        Date d;
        // Simple parsing: assume DD/MM/YYYY format
        if (dateStr.length() < 10) return d;
        d.day   = stoi(dateStr.substr(0, 2));
        d.month = stoi(dateStr.substr(3, 2));
        d.year  = stoi(dateStr.substr(6, 4));
        return d;
    }

    // -------------------------------------------------------------------
    // Get today's date using ctime. Used for expiry checking.
    // -------------------------------------------------------------------
    static Date today() {
        time_t now = time(nullptr);
        tm* local = localtime(&now);
        return Date(local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);
    }

    // -------------------------------------------------------------------
    // Compare two dates. Returns true if this date is earlier than 'other'.
    // Used to check if a blood unit has expired.
    // -------------------------------------------------------------------
    bool isBefore(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    // -------------------------------------------------------------------
    // Convert date to display string (DD/MM/YYYY)
    // -------------------------------------------------------------------
    string toString() const {
        ostringstream oss;
        oss << (day < 10 ? "0" : "") << day << "/"
            << (month < 10 ? "0" : "") << month << "/"
            << year;
        return oss.str();
    }
};

// ============================================================================
// SECTION 2: SINGLY LINKED LIST FOR BLOOD INVENTORY
//
// Why Linked List? Blood units are donated continuously; we don't know
// how many units we'll receive. A linked list allows dynamic insertion
// (O(1) at head, O(n) at tail) without pre-allocating an array.
// Each node represents one physical blood bag/unit with its blood group,
// donation date, and expiry date. We also maintain a hash table for
// O(1) group-wise stock lookup.
// ============================================================================

// -------------------------------------------------------------------
// BloodUnit structure - Each bag/unit of donated blood
// -------------------------------------------------------------------
struct BloodUnit {
    string bloodGroup;   // Blood group: A+, A-, B+, B-, AB+, AB-, O+, O-
    Date donationDate;   // When it was donated
    Date expiryDate;     // Expiry date (typically 42 days for RBC)
    string donorId;      // Donor who donated this unit
    bool isAvailable;    // true = in inventory, false = issued to patient

    BloodUnit(string bg, Date dd, Date ed, string did)
        : bloodGroup(bg), donationDate(dd), expiryDate(ed),
          donorId(did), isAvailable(true) {}
};

// -------------------------------------------------------------------
// InventoryNode - Node for the singly linked list
// -------------------------------------------------------------------
struct InventoryNode {
    BloodUnit unit;
    InventoryNode* next;

    InventoryNode(BloodUnit u) : unit(u), next(nullptr) {}
};

class BloodInventory {
private:
    InventoryNode* head;                           // Head of linked list
    int totalUnits;                                // Total units in inventory
    unordered_map<string, int> stockByGroup;       // Hash: group -> count

public:
    BloodInventory() : head(nullptr), totalUnits(0) {}

    ~BloodInventory() {
        InventoryNode* current = head;
        while (current) {
            InventoryNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // -------------------------------------------------------------------
    // ADD BLOOD UNIT: Insert at head (O(1)) + update hash table.
    // The hash table (unordered_map) maps blood group to stock count,
    // giving us O(1) lookups when checking availability.
    // -------------------------------------------------------------------
    void addUnit(string bloodGroup, Date donationDate,
                 Date expiryDate, string donorId) {
        InventoryNode* newNode = new InventoryNode(
            BloodUnit(bloodGroup, donationDate, expiryDate, donorId));

        // Insert at head for O(1) insertion
        newNode->next = head;
        head = newNode;

        totalUnits++;
        // Update hash table: increment count for this blood group
        stockByGroup[bloodGroup]++;
        cout << "  [OK] " << bloodGroup << " unit added (Donor: "
             << donorId << ").\n";
    }

    // -------------------------------------------------------------------
    // SEARCH STOCK BY GROUP: O(1) using hash table (unordered_map).
    // Returns the number of available units of that blood group.
    // -------------------------------------------------------------------
    int checkStock(string bloodGroup) {
        if (stockByGroup.find(bloodGroup) != stockByGroup.end()) {
            return stockByGroup[bloodGroup];
        }
        return 0;
    }

    // -------------------------------------------------------------------
    // ISSUE UNIT: Find and mark the first available unit of given group
    // as issued, decrement hash count. O(n) worst-case scan of list.
    // Returns true if unit was issued successfully.
    // -------------------------------------------------------------------
    bool issueUnit(string bloodGroup) {
        if (checkStock(bloodGroup) == 0) {
            cout << "  [ERROR] No " << bloodGroup << " units available.\n";
            return false;
        }

        InventoryNode* temp = head;
        while (temp) {
            if (temp->unit.bloodGroup == bloodGroup
                && temp->unit.isAvailable) {
                temp->unit.isAvailable = false;
                stockByGroup[bloodGroup]--;
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    // -------------------------------------------------------------------
    // UNDO LAST ISSUE: Mark the most recently issued unit as available
    // again. Scans the list for the first issued unit. O(n).
    // This pairs with the stack for "undo last issue" functionality.
    // -------------------------------------------------------------------
    bool undoIssue(string bloodGroup) {
        // Find the most recently issued unit (last in list that was issued)
        // We scan from head and take the first issued unit we find,
        // since new insertions are at head (most recent issues are at front).
        InventoryNode* temp = head;
        while (temp) {
            if (temp->unit.bloodGroup == bloodGroup
                && !temp->unit.isAvailable) {
                temp->unit.isAvailable = true;
                stockByGroup[bloodGroup]++;
                cout << "  [OK] Undo issue: " << bloodGroup
                     << " unit returned to inventory.\n";
                return true;
            }
            temp = temp->next;
        }
        cout << "  [ERROR] No issued " << bloodGroup << " unit to undo.\n";
        return false;
    }

    // -------------------------------------------------------------------
    // REMOVE EXPIRED UNITS: Scan linked list and unlink expired units.
    // Blood has a limited shelf life (RBCs: ~42 days, platelets: ~5 days).
    // This function removes units past their expiry date. O(n).
    // -------------------------------------------------------------------
    int removeExpiredUnits() {
        Date today = Date::today();
        int count = 0;

        // Remove expired units from head
        while (head && head->unit.expiryDate.isBefore(today)) {
            InventoryNode* temp = head;
            string bg = head->unit.bloodGroup;
            head = head->next;
            if (temp->unit.isAvailable) {
                stockByGroup[bg]--;
            }
            delete temp;
            totalUnits--;
            count++;
        }

        // Remove expired units from rest of list
        InventoryNode* current = head;
        while (current && current->next) {
            if (current->next->unit.expiryDate.isBefore(today)) {
                InventoryNode* temp = current->next;
                string bg = temp->unit.bloodGroup;
                current->next = temp->next;
                if (temp->unit.isAvailable) {
                    stockByGroup[bg]--;
                }
                delete temp;
                totalUnits--;
                count++;
            } else {
                current = current->next;
            }
        }

        if (count > 0) {
            cout << "  [INFO] Removed " << count << " expired unit(s).\n";
        }
        return count;
    }

    // -------------------------------------------------------------------
    // LOW-STOCK ALERT: Check if any blood group falls below threshold.
    // A hospital typically maintains minimum stock levels for emergencies.
    // -------------------------------------------------------------------
    void checkLowStock(int threshold = 3) {
        cout << "  Low-Stock Alert (threshold: " << threshold << " units):\n";
        bool found = false;

        // Iterate through hash table to check each group count
        unordered_map<string, int>::iterator it;
        for (it = stockByGroup.begin(); it != stockByGroup.end(); ++it) {
            if (it->second < threshold) {
                cout << "    [ALERT] " << it->first << ": only "
                     << it->second << " unit(s) remaining!\n";
                found = true;
            }
        }

        // Also check groups that have zero stock (not in hash table)
        string groups[] = {"A+", "A-", "B+", "B-",
                           "AB+", "AB-", "O+", "O-"};
        for (int i = 0; i < 8; i++) {
            if (stockByGroup.find(groups[i]) == stockByGroup.end()) {
                cout << "    [ALERT] " << groups[i] << ": 0 units available!\n";
                found = true;
            }
        }

        if (!found) {
            cout << "    All blood groups have sufficient stock.\n";
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY INVENTORY: Traverse linked list and show all units. O(n).
    // -------------------------------------------------------------------
    void displayInventory() {
        if (!head) {
            cout << "  [INFO] No blood units in inventory.\n";
            return;
        }

        cout << "  " << left << setw(8) << "Group"
             << setw(14) << "Donation"
             << setw(14) << "Expiry"
             << setw(12) << "Donor ID"
             << "Status\n";
        cout << "  " << string(65, '-') << "\n";

        InventoryNode* temp = head;
        while (temp) {
            cout << "  " << left << setw(8) << temp->unit.bloodGroup
                 << setw(14) << temp->unit.donationDate.toString()
                 << setw(14) << temp->unit.expiryDate.toString()
                 << setw(12) << temp->unit.donorId
                 << (temp->unit.isAvailable ? "Available" : "Issued")
                 << "\n";
            temp = temp->next;
        }

        cout << "\n  Stock Summary (Hash Table):\n";
        unordered_map<string, int>::iterator it;
        for (it = stockByGroup.begin(); it != stockByGroup.end(); ++it) {
            cout << "    " << it->first << ": " << it->second << " unit(s)\n";
        }
        cout << "  Total units: " << totalUnits << "\n";
    }

    int getTotalUnits() { return totalUnits; }
};

// ============================================================================
// SECTION 3: BST FOR DONOR RECORDS
//
// Why BST? Donors need to be searchable by their unique donor ID
// (e.g., "D001"). BST gives O(log n) average search, insertion, and
// deletion. We store donors sorted by donorId (string comparison).
// In-order traversal gives us sorted donor listing.
// ============================================================================

struct Donor {
    string donorId;        // Unique identifier (e.g., "D001")
    string name;           // Full name of donor
    string bloodGroup;     // Blood group of donor
    string contact;        // Phone number
    Date lastDonation;     // Date of most recent donation

    Donor(string id, string n, string bg, string c)
        : donorId(id), name(n), bloodGroup(bg), contact(c) {}
};

struct DonorNode {
    Donor donor;
    DonorNode* left;
    DonorNode* right;

    DonorNode(Donor d) : donor(d), left(nullptr), right(nullptr) {}
};

class DonorTree {
private:
    DonorNode* root;

    // -------------------------------------------------------------------
    // Helper: Insert donor into BST recursively. O(log n) average.
    // Nodes are sorted by donorId alphabetically/numerically.
    // -------------------------------------------------------------------
    DonorNode* insert(DonorNode* node, Donor d) {
        if (!node) return new DonorNode(d);
        if (d.donorId < node->donor.donorId)
            node->left = insert(node->left, d);
        else if (d.donorId > node->donor.donorId)
            node->right = insert(node->right, d);
        // If equal, donor already exists (we check before calling)
        return node;
    }

    // -------------------------------------------------------------------
    // Helper: Search donor by ID recursively. O(log n) average.
    // -------------------------------------------------------------------
    DonorNode* search(DonorNode* node, const string& donorId) {
        if (!node || node->donor.donorId == donorId)
            return node;
        if (donorId < node->donor.donorId)
            return search(node->left, donorId);
        return search(node->right, donorId);
    }

    // -------------------------------------------------------------------
    // Helper: Find minimum node (leftmost leaf). Used in deletion.
    // -------------------------------------------------------------------
    DonorNode* findMin(DonorNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    // -------------------------------------------------------------------
    // Helper: Delete node from BST recursively. O(log n) average.
    // Handles three cases: leaf, one child, two children (inorder successor).
    // -------------------------------------------------------------------
    DonorNode* deleteNode(DonorNode* node, const string& donorId) {
        if (!node) return nullptr;
        if (donorId < node->donor.donorId)
            node->left = deleteNode(node->left, donorId);
        else if (donorId > node->donor.donorId)
            node->right = deleteNode(node->right, donorId);
        else {
            // Found the node to delete
            if (!node->left) {
                DonorNode* temp = node->right;
                delete node;
                return temp;
            }
            if (!node->right) {
                DonorNode* temp = node->left;
                delete node;
                return temp;
            }
            // Two children: find inorder successor (smallest in right subtree)
            DonorNode* successor = findMin(node->right);
            node->donor = successor->donor;
            node->right = deleteNode(node->right, successor->donor.donorId);
        }
        return node;
    }

    // -------------------------------------------------------------------
    // Helper: Inorder traversal to display donors sorted by ID.
    // This is the key BST feature: sorted output from an unsorted input.
    // -------------------------------------------------------------------
    void inorderDisplay(DonorNode* node) {
        if (!node) return;
        inorderDisplay(node->left);              // Visit left subtree (smaller IDs)
        cout << "  " << left << setw(10) << node->donor.donorId
             << setw(25) << node->donor.name
             << setw(8) << node->donor.bloodGroup
             << "Last donation: "
             << node->donor.lastDonation.toString() << "\n";
        inorderDisplay(node->right);             // Visit right subtree (larger IDs)
    }

    // -------------------------------------------------------------------
    // Helper: Recursively destroy tree to free memory
    // -------------------------------------------------------------------
    void destroy(DonorNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    DonorTree() : root(nullptr) {}

    ~DonorTree() { destroy(root); }

    // -------------------------------------------------------------------
    // REGISTER DONOR: Insert into BST. O(log n) average.
    // -------------------------------------------------------------------
    void registerDonor(string id, string name,
                       string bloodGroup, string contact) {
        if (search(root, id)) {
            cout << "  [ERROR] Donor ID " << id << " already exists!\n";
            return;
        }
        Donor d(id, name, bloodGroup, contact);
        root = insert(root, d);
        cout << "  [OK] Donor '" << name << "' (" << bloodGroup
             << ") registered successfully.\n";
    }

    // -------------------------------------------------------------------
    // FIND DONOR: O(log n) average. Returns pointer to Donor or nullptr.
    // -------------------------------------------------------------------
    Donor* findDonor(const string& donorId) {
        DonorNode* node = search(root, donorId);
        return node ? &(node->donor) : nullptr;
    }

    // -------------------------------------------------------------------
    // REMOVE DONOR: O(log n) average.
    // -------------------------------------------------------------------
    bool removeDonor(const string& donorId) {
        Donor* d = findDonor(donorId);
        if (!d) {
            cout << "  [ERROR] Donor " << donorId << " not found.\n";
            return false;
        }
        root = deleteNode(root, donorId);
        cout << "  [OK] Donor " << donorId << " (" << d->name << ") removed.\n";
        return true;
    }

    // -------------------------------------------------------------------
    // UPDATE LAST DONATION DATE for a donor after they donate
    // -------------------------------------------------------------------
    void updateLastDonation(const string& donorId, Date donationDate) {
        Donor* d = findDonor(donorId);
        if (d) {
            d->lastDonation = donationDate;
        }
    }

    // -------------------------------------------------------------------
    // DISPLAY ALL DONORS: Inorder traversal => sorted by donor ID. O(n).
    // -------------------------------------------------------------------
    void displayAllDonors() {
        if (!root) {
            cout << "  [INFO] No donors registered.\n";
            return;
        }
        cout << "  " << left << setw(10) << "Donor ID"
             << setw(25) << "Name"
             << setw(8) << "Group"
             << "Last Donation\n";
        cout << "  " << string(60, '-') << "\n";
        inorderDisplay(root);
    }
};

// ============================================================================
// SECTION 4: QUEUE FOR PATIENT BLOOD REQUESTS
//
// Why Queue? Patient blood requests must be processed in FIFO order
// (First-In-First-Out) to ensure fairness. The first patient requesting
// blood gets served first. In a real system, urgency/priority would
// override simple FIFO, but here FIFO models a fair emergency system.
// ============================================================================

struct BloodRequest {
    string patientId;      // Patient identifier
    string patientName;    // Patient name
    string bloodGroup;     // Required blood group
    int unitsRequired;     // Number of units needed
    string urgency;        // "NORMAL" or "URGENT"

    BloodRequest(string pid, string pn, string bg, int ur, string urg)
        : patientId(pid), patientName(pn), bloodGroup(bg),
          unitsRequired(ur), urgency(urg) {}
};

class RequestQueue {
private:
    queue<BloodRequest> requests;  // STL queue (FIFO data structure)
    int totalRequests;             // Counter for tracking

public:
    RequestQueue() : totalRequests(0) {}

    // -------------------------------------------------------------------
    // ENQUEUE REQUEST: Add patient request to the back of the queue. O(1).
    // In a hospital, every incoming blood request is queued for processing.
    // -------------------------------------------------------------------
    void addRequest(string patientId, string patientName,
                    string bloodGroup, int units, string urgency) {
        requests.push(BloodRequest(patientId, patientName,
                                   bloodGroup, units, urgency));
        totalRequests++;
        cout << "  [OK] Request queued: " << patientName
             << " needs " << units << " unit(s) of " << bloodGroup
             << " [" << urgency << "]\n";
    }

    // -------------------------------------------------------------------
    // PROCESS NEXT REQUEST: Dequeue the front request. O(1).
    // Returns the request for the caller to fulfill.
    // -------------------------------------------------------------------
    BloodRequest processNext() {
        if (requests.empty()) {
            return BloodRequest("", "", "", 0, "NONE");
        }
        BloodRequest req = requests.front();
        requests.pop();
        return req;
    }

    bool hasPending() { return !requests.empty(); }
    int pendingCount() { return requests.size(); }

    // -------------------------------------------------------------------
    // DISPLAY PENDING REQUESTS: Show all queued requests. O(n).
    // -------------------------------------------------------------------
    void displayPending() {
        if (requests.empty()) {
            cout << "  [INFO] No pending blood requests.\n";
            return;
        }
        queue<BloodRequest> temp = requests;
        cout << "  Pending requests (" << requests.size() << "):\n";
        cout << "  " << left << setw(12) << "Patient ID"
             << setw(20) << "Name"
             << setw(8) << "Group"
             << setw(8) << "Units"
             << "Urgency\n";
        cout << "  " << string(60, '-') << "\n";
        while (!temp.empty()) {
            BloodRequest& r = temp.front();
            cout << "  " << left << setw(12) << r.patientId
                 << setw(20) << r.patientName
                 << setw(8) << r.bloodGroup
                 << setw(8) << r.unitsRequired
                 << r.urgency << "\n";
            temp.pop();
        }
    }
};

// ============================================================================
// SECTION 5: STACK FOR BLOOD ISSUE HISTORY
//
// Why Stack? When blood is issued to a patient, we record it on a stack.
// The most recent issue is on top, allowing us to:
//   1. View recent issues (LIFO - most recent first)
//   2. Undo the last issue (pop from stack, return unit to inventory)
// Stack is the natural choice for this "undo/review recent" pattern.
// ============================================================================

struct IssueRecord {
    string issueId;         // Unique issue ID (auto-generated)
    string patientName;     // Who received the blood
    string bloodGroup;      // Which blood group was issued
    int unitsIssued;        // How many units
    string date;            // Date of issue

    IssueRecord(string iid, string pn, string bg, int units)
        : issueId(iid), patientName(pn), bloodGroup(bg), unitsIssued(units) {
        time_t now = time(nullptr);
        date = ctime(&now);
        if (!date.empty() && date[date.length() - 1] == '\n')
            date.pop_back();
    }
};

class IssueHistory {
private:
    stack<IssueRecord> history;  // STL stack (LIFO). Most recent on top.
    int nextIssueId;             // For generating unique issue IDs

public:
    IssueHistory() : nextIssueId(1001) {}

    // -------------------------------------------------------------------
    // RECORD ISSUE: Push onto stack. O(1).
    // -------------------------------------------------------------------
    void recordIssue(string patientName, string bloodGroup, int units) {
        string issueId = "ISS" + to_string(nextIssueId++);
        history.push(IssueRecord(issueId, patientName, bloodGroup, units));
    }

    // -------------------------------------------------------------------
    // UNDO LAST ISSUE: Pop from stack and return details. O(1).
    // The caller uses this to return the unit to inventory.
    // Returns an empty IssueRecord if nothing to undo.
    // -------------------------------------------------------------------
    IssueRecord undoLast() {
        if (history.empty()) {
            return IssueRecord("", "", "", 0);
        }
        IssueRecord last = history.top();
        history.pop();
        return last;
    }

    bool hasHistory() { return !history.empty(); }
    int historyCount() { return history.size(); }

    // -------------------------------------------------------------------
    // VIEW ISSUE HISTORY: Display most recent issues first (LIFO). O(n).
    // -------------------------------------------------------------------
    void viewHistory(int count = 10) {
        if (history.empty()) {
            cout << "  [INFO] No issue history.\n";
            return;
        }
        // Copy stack to preserve original while displaying
        stack<IssueRecord> temp = history;
        int shown = 0;

        cout << "  " << left << setw(12) << "Issue ID"
             << setw(20) << "Patient"
             << setw(10) << "Group"
             << setw(8) << "Units"
             << "Date\n";
        cout << "  " << string(75, '-') << "\n";

        while (!temp.empty() && shown < count) {
            IssueRecord& r = temp.top();
            cout << "  " << left << setw(12) << r.issueId
                 << setw(20) << r.patientName
                 << setw(10) << r.bloodGroup
                 << setw(8) << r.unitsIssued
                 << r.date << "\n";
            temp.pop();
            shown++;
        }
    }
};

// ============================================================================
// SECTION 6: MAIN BLOOD BANK SYSTEM - Integrates all data structures
// ============================================================================

class BloodBankSystem {
private:
    BloodInventory inventory;    // Linked list + Hash Table
    DonorTree donors;            // BST for donor records
    RequestQueue requestQueue;   // Queue for patient requests
    IssueHistory issueHistory;   // Stack for issue history/undo

    // -------------------------------------------------------------------
    // Check and remove expired units from inventory.
    // Called periodically (before each menu action in this simulation).
    // -------------------------------------------------------------------
    void checkExpiry() {
        inventory.removeExpiredUnits();
    }

    // -------------------------------------------------------------------
    // Process all pending blood requests from the queue (FIFO order).
    // For each request, check if sufficient stock exists and issue.
    // -------------------------------------------------------------------
    void processRequests() {
        while (requestQueue.hasPending()) {
            BloodRequest req = requestQueue.processNext();
            if (req.urgency == "NONE") break;

            int stock = inventory.checkStock(req.bloodGroup);
            if (stock >= req.unitsRequired) {
                // Issue the required units
                for (int i = 0; i < req.unitsRequired; i++) {
                    inventory.issueUnit(req.bloodGroup);
                }
                issueHistory.recordIssue(req.patientName,
                                         req.bloodGroup,
                                         req.unitsRequired);
                cout << "  [OK] Issued " << req.unitsRequired
                     << " unit(s) of " << req.bloodGroup
                     << " to " << req.patientName << ".\n";
            } else {
                cout << "  [WARN] Insufficient " << req.bloodGroup
                     << " for " << req.patientName
                     << " (needs " << req.unitsRequired
                     << ", has " << stock << "). "
                     << "Request skipped.\n";
            }
        }
    }

public:
    // -------------------------------------------------------------------
    // RUN the interactive blood bank management system
    // -------------------------------------------------------------------
    void run() {
        cout << "\n=============================================\n";
        cout << "   BLOOD BANK MANAGEMENT SYSTEM\n";
        cout << "=============================================\n\n";

        int choice;
        string bloodGroup, donorId, donorName, contact;
        string patientId, patientName, urgency;
        int units, day, month, year;
        Date donateDate, expireDate;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  1. Add Blood Unit (Donation)\n";
            cout << "  2. Check Blood Stock by Group (Hash Table)\n";
            cout << "  3. Register Donor (BST)\n";
            cout << "  4. Find Donor (BST Search)\n";
            cout << "  5. Display All Donors (BST In-order)\n";
            cout << "  6. Queue Blood Request (Patient)\n";
            cout << "  7. Process Pending Requests (Queue FIFO)\n";
            cout << "  8. Issue Blood to Patient\n";
            cout << "  9. Undo Last Issue (Stack)\n";
            cout << " 10. View Issue History (Stack - Recent First)\n";
            cout << " 11. Display Full Inventory\n";
            cout << " 12. Low-Stock Alerts\n";
            cout << " 13. Remove Expired Units\n";
            cout << " 14. Remove Donor\n";
            cout << "  0. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: {
                    // --------------------------------------------------
                    // ADD BLOOD UNIT: Insert into linked list + hash.
                    // Each donation creates a new node in the inventory.
                    // --------------------------------------------------
                    cout << "  Blood Group (A+/A-/B+/B-/AB+/AB-/O+/O-): ";
                    getline(cin, bloodGroup);
                    cout << "  Donor ID: "; getline(cin, donorId);
                    cout << "  Donation Date (DD MM YYYY): ";
                    cin >> day >> month >> year;
                    donateDate = Date(day, month, year);

                    // Standard expiry: 42 days after donation for RBC
                    expireDate = Date(day, month, year + 1);
                    cout << "  Expiry Date set to: "
                         << expireDate.toString() << "\n";

                    inventory.addUnit(bloodGroup, donateDate,
                                      expireDate, donorId);
                    // Update donor's last donation date
                    donors.updateLastDonation(donorId, donateDate);
                    checkExpiry();  // Auto-check expired units
                    break;
                }

                case 2: {
                    // --------------------------------------------------
                    // CHECK STOCK: O(1) hash table lookup.
                    // The unordered_map stores blood group -> count.
                    // --------------------------------------------------
                    cout << "  Blood Group: "; getline(cin, bloodGroup);
                    int stock = inventory.checkStock(bloodGroup);
                    cout << "  [INFO] " << bloodGroup
                         << " available: " << stock << " unit(s).\n";
                    break;
                }

                case 3: {
                    // --------------------------------------------------
                    // REGISTER DONOR: Insert into BST by donor ID.
                    // O(log n) average insertion time.
                    // --------------------------------------------------
                    cout << "  Donor ID: "; getline(cin, donorId);
                    cout << "  Name: "; getline(cin, donorName);
                    cout << "  Blood Group: "; getline(cin, bloodGroup);
                    cout << "  Contact: "; getline(cin, contact);
                    donors.registerDonor(donorId, donorName,
                                         bloodGroup, contact);
                    break;
                }

                case 4: {
                    // --------------------------------------------------
                    // FIND DONOR: BST search. O(log n) average.
                    // --------------------------------------------------
                    cout << "  Donor ID: "; getline(cin, donorId);
                    Donor* d = donors.findDonor(donorId);
                    if (d) {
                        cout << "  Found: " << d->name
                             << " | Group: " << d->bloodGroup
                             << " | Contact: " << d->contact
                             << " | Last Donation: "
                             << d->lastDonation.toString() << "\n";
                    } else {
                        cout << "  [ERROR] Donor not found.\n";
                    }
                    break;
                }

                case 5: {
                    // --------------------------------------------------
                    // DISPLAY ALL DONORS: BST in-order traversal.
                    // Output is automatically sorted by donor ID.
                    // --------------------------------------------------
                    donors.displayAllDonors();
                    break;
                }

                case 6: {
                    // --------------------------------------------------
                    // QUEUE BLOOD REQUEST: Add patient to FIFO queue.
                    // Incoming requests are processed in order.
                    // --------------------------------------------------
                    cout << "  Patient ID: "; getline(cin, patientId);
                    cout << "  Patient Name: "; getline(cin, patientName);
                    cout << "  Blood Group Needed: "; getline(cin, bloodGroup);
                    cout << "  Units Required: "; cin >> units;
                    cin.ignore();
                    cout << "  Urgency (NORMAL/URGENT): ";
                    getline(cin, urgency);
                    requestQueue.addRequest(patientId, patientName,
                                            bloodGroup, units, urgency);
                    break;
                }

                case 7: {
                    // --------------------------------------------------
                    // PROCESS QUEUE: Handle all pending FIFO requests.
                    // --------------------------------------------------
                    if (requestQueue.hasPending()) {
                        processRequests();
                    } else {
                        cout << "  [INFO] No pending requests.\n";
                    }
                    break;
                }

                case 8: {
                    // --------------------------------------------------
                    // ISSUE BLOOD: Immediate issue to patient.
                    // Also records on the issue stack for undo capability.
                    // --------------------------------------------------
                    cout << "  Patient Name: "; getline(cin, patientName);
                    cout << "  Blood Group: "; getline(cin, bloodGroup);
                    cout << "  Units: "; cin >> units;
                    cin.ignore();

                    int stock = inventory.checkStock(bloodGroup);
                    if (stock >= units) {
                        for (int i = 0; i < units; i++) {
                            inventory.issueUnit(bloodGroup);
                        }
                        issueHistory.recordIssue(patientName,
                                                 bloodGroup, units);
                        cout << "  [OK] Issued " << units
                             << " unit(s) of " << bloodGroup
                             << " to " << patientName << ".\n";
                    } else {
                        cout << "  [ERROR] Insufficient stock. "
                             << "Available: " << stock
                             << ", Required: " << units << ".\n";
                    }
                    break;
                }

                case 9: {
                    // --------------------------------------------------
                    // UNDO LAST ISSUE: Pop from stack, return to inventory.
                    // Stack provides natural LIFO undo behavior.
                    // --------------------------------------------------
                    IssueRecord rec = issueHistory.undoLast();
                    if (rec.issueId == "") {
                        cout << "  [INFO] Nothing to undo.\n";
                    } else {
                        // Return the units to inventory
                        for (int i = 0; i < rec.unitsIssued; i++) {
                            inventory.undoIssue(rec.bloodGroup);
                        }
                        cout << "  [OK] Undid issue to " << rec.patientName
                             << " (" << rec.unitsIssued
                             << " unit(s) of " << rec.bloodGroup << ").\n";
                    }
                    break;
                }

                case 10: {
                    // --------------------------------------------------
                    // VIEW ISSUE HISTORY: Stack displayed LIFO.
                    // Most recent issues appear first.
                    // --------------------------------------------------
                    issueHistory.viewHistory();
                    break;
                }

                case 11: {
                    // --------------------------------------------------
                    // DISPLAY FULL INVENTORY: Linked list traversal + hash.
                    // --------------------------------------------------
                    inventory.displayInventory();
                    break;
                }

                case 12: {
                    // --------------------------------------------------
                    // LOW-STOCK ALERTS: Check all blood groups.
                    // --------------------------------------------------
                    inventory.checkLowStock();
                    break;
                }

                case 13: {
                    // --------------------------------------------------
                    // REMOVE EXPIRED: Purge past-expiry units from list.
                    // --------------------------------------------------
                    inventory.removeExpiredUnits();
                    break;
                }

                case 14: {
                    // --------------------------------------------------
                    // REMOVE DONOR: BST deletion. O(log n).
                    // --------------------------------------------------
                    cout << "  Donor ID: "; getline(cin, donorId);
                    donors.removeDonor(donorId);
                    break;
                }

                case 0:
                    cout << "  Exiting Blood Bank System. Thank you!\n";
                    break;

                default:
                    cout << "  [ERROR] Invalid choice. Enter 0-14.\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION - Entry point
// ============================================================================
int main() {
    BloodBankSystem system;
    system.run();
    return 0;
}
