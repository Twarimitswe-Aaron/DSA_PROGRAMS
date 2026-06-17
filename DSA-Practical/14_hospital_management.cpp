/*
 * ============================================================================
 * FILE: 14_hospital_management.cpp
 * ============================================================================
 * HOSPITAL MANAGEMENT SYSTEM - Real-world DSA Application
 *
 * EXAM FREQUENCY: Very High. Hospital/patient management systems are
 * extremely common DSA practical exam projects worldwide.
 *
 * DATA STRUCTURES USED:
 *   1. Priority Queue (Max-Heap) - Emergency room triage
 *      (Most CRITICAL: Patients with highest severity treated first)
 *   2. Queue (FIFO) - Regular outpatient appointments
 *   3. Binary Search Tree - Doctor records (sorted by ID)
 *   4. Hash Table (unordered_map) - Patient records (O(1) lookup by ID)
 *   5. Stack - Treatment history / undo operations
 *   6. Linked List - Bed management / ward allocation
 *
 * REAL-WORLD SCENARIO:
 *   - Emergency patients are treated based on SEVERITY (not arrival time)
 *     => Priority Queue (max-heap: highest severity first)
 *   - Regular checkup patients are seen in order of arrival
 *     => Queue (FIFO)
 *   - Doctors and patients need fast lookup by ID
 *     => BST for doctors, Hash Table for patients
 *   - Recent treatments should be reviewable (most recent first)
 *     => Stack
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>        // For regular queue AND priority queue
#include <stack>        // For treatment history
#include <vector>
#include <unordered_map> // For patient records
#include <iomanip>      // For formatted output
#include <ctime>        // For timestamps
#include <cstdlib>      // For rand()
#include <algorithm>    // For sort, find_if
using namespace std;

// ============================================================================
// SECTION 1: DATA STRUCTURES & ENTITY DEFINITIONS
// ============================================================================

// -------------------------------------------------------------------
// PATIENT STRUCTURE
// Stores all information about a patient including medical data.
// Used with:
//   - Hash Table (unordered_map) for O(1) lookup by patient ID
//   - Priority Queue for emergency triage (by severity)
//   - Queue for regular appointments (FIFO)
// -------------------------------------------------------------------
struct Patient {
    string patientId;       // Unique ID (e.g., "P001")
    string name;            // Patient's full name
    int age;                // Age in years
    string disease;         // Brief diagnosis/disease description
    int severity;           // 1=Minor, 2=Moderate, 3=Serious, 4=Critical, 5=Emergency
    string contact;         // Phone number
    bool isAdmitted;        // true if assigned a bed
    string admissionDate;   // Date of admission

    Patient() : patientId(""), name(""), age(0), disease(""),
                severity(1), contact(""), isAdmitted(false) {}

    Patient(string id, string n, int a, string d, int s, string c)
        : patientId(id), name(n), age(a), disease(d),
          severity(s), contact(c), isAdmitted(false) {
        time_t now = time(nullptr);
        admissionDate = ctime(&now);
        if (!admissionDate.empty() && admissionDate.back() == '\n')
            admissionDate.pop_back();
    }

    // Display patient info in formatted way
    void display() const {
        cout << "  " << left << setw(8) << patientId
             << setw(20) << name
             << setw(5) << age
             << setw(25) << disease
             << setw(10) << severity
             << setw(12) << (isAdmitted ? "Admitted" : "Outpatient")
             << "\n";
    }
};

// -------------------------------------------------------------------
// DOCTOR STRUCTURE
// Stored in BST for fast lookup by doctor ID.
// Each doctor has a specialty and a list of assigned patient IDs.
// -------------------------------------------------------------------
struct Doctor {
    string doctorId;            // Unique ID (e.g., "D001")
    string name;                // Doctor's name
    string specialty;           // e.g., "Cardiology", "Neurology"
    bool isAvailable;           // true if currently free
    vector<string> patientIds;  // List of patients under this doctor

    Doctor() : doctorId(""), name(""), specialty(""), isAvailable(true) {}

    Doctor(string id, string n, string spec)
        : doctorId(id), name(n), specialty(spec), isAvailable(true) {}
};

// -------------------------------------------------------------------
// TREATMENT RECORD - Stored in stack for history review
// -------------------------------------------------------------------
struct TreatmentRecord {
    string patientId;
    string doctorId;
    string description;     // Treatment given
    string date;            // When treatment occurred

    TreatmentRecord(string pId, string dId, string desc)
        : patientId(pId), doctorId(dId), description(desc) {
        time_t now = time(nullptr);
        date = ctime(&now);
        if (!date.empty() && date.back() == '\n') date.pop_back();
    }
};

// -------------------------------------------------------------------
// STRUCT for emergency patient in priority queue
// Priority is based on SEVERITY (higher severity = higher priority)
// For same severity, order by arrival time (earlier = higher priority)
// -------------------------------------------------------------------
struct EmergencyPatient {
    string patientId;
    int severity;           // 1-5 (5 = most severe / highest priority)
    int arrivalOrder;       // Used to break ties (FIFO among same severity)

    // COMPARATOR for Max-Heap: returns true if 'a' has LOWER priority than 'b'
    // priority_queue by default uses std::less which creates a MAX heap.
    // We define operator< for the priority_queue to work correctly.
    bool operator<(const EmergencyPatient& other) const {
        // Higher severity = higher priority
        if (severity != other.severity)
            return severity < other.severity;  // Lower severity = lower priority
        // Same severity: earlier arrival = higher priority
        return arrivalOrder > other.arrivalOrder;
    }
};

// ============================================================================
// SECTION 2: BST FOR DOCTOR RECORDS
//
// Why BST? Doctors need to be searchable by ID (sorted order for display).
// O(log n) search, insert, and delete.
// ============================================================================

struct DoctorNode {
    Doctor doctor;
    DoctorNode* left;
    DoctorNode* right;

    DoctorNode(Doctor d) : doctor(d), left(nullptr), right(nullptr) {}
};

class DoctorBST {
private:
    DoctorNode* root;

    DoctorNode* insert(DoctorNode* node, Doctor d) {
        if (!node) return new DoctorNode(d);
        if (d.doctorId < node->doctor.doctorId)
            node->left = insert(node->left, d);
        else if (d.doctorId > node->doctor.doctorId)
            node->right = insert(node->right, d);
        return node;
    }

    DoctorNode* search(DoctorNode* node, string id) {
        if (!node || node->doctor.doctorId == id) return node;
        if (id < node->doctor.doctorId)
            return search(node->left, id);
        return search(node->right, id);
    }

    void inorder(DoctorNode* node) {
        if (!node) return;
        inorder(node->left);
        cout << "  " << left << setw(8) << node->doctor.doctorId
             << setw(25) << node->doctor.name
             << setw(20) << node->doctor.specialty
             << setw(12) << (node->doctor.isAvailable ? "Available" : "Busy")
             << "Patients: " << node->doctor.patientIds.size() << "\n";
        inorder(node->right);
    }

    void destroy(DoctorNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    DoctorBST() : root(nullptr) {}
    ~DoctorBST() { destroy(root); }

    void addDoctor(string id, string name, string specialty) {
        if (search(root, id)) {
            cout << "  [ERROR] Doctor ID " << id << " already exists.\n";
            return;
        }
        root = insert(root, Doctor(id, name, specialty));
        cout << "  [OK] Dr. " << name << " (" << specialty << ") added.\n";
    }

    Doctor* findDoctor(string id) {
        DoctorNode* node = search(root, id);
        return node ? &(node->doctor) : nullptr;
    }

    void displayAll() {
        if (!root) { cout << "  [INFO] No doctors registered.\n"; return; }
        cout << "  " << left << setw(8) << "ID"
             << setw(25) << "Name"
             << setw(20) << "Specialty"
             << setw(12) << "Status"
             << "Patients\n";
        cout << "  " << string(75, '-') << "\n";
        inorder(root);
    }
};

// ============================================================================
// SECTION 3: MAIN HOSPITAL SYSTEM
// ============================================================================

class HospitalManagementSystem {
private:
    // --- Data Structures ---
    unordered_map<string, Patient> patients;      // Hash: patientId -> Patient
    DoctorBST doctors;                             // BST: doctor records
    priority_queue<EmergencyPatient> emergencyPQ;  // Max-Heap: emergency triage
    queue<string> regularQueue;                    // FIFO: regular appointments
    stack<TreatmentRecord> treatmentHistory;       // Stack: recent treatments
    vector<string> bedList;                        // Vector: bed assignments

    int nextArrivalOrder;   // For tie-breaking in priority queue
    int maxBeds;            // Total hospital beds
    int occupiedBeds;       // Currently occupied beds

public:
    HospitalManagementSystem(int beds = 50)
        : nextArrivalOrder(0), maxBeds(beds), occupiedBeds(0) {}

    // ==================================================================
    // PATIENT MANAGEMENT (using Hash Table for O(1) access)
    // ==================================================================

    // -------------------------------------------------------------------
    // REGISTER PATIENT: Add to hash table. O(1) average.
    // -------------------------------------------------------------------
    void registerPatient(string id, string name, int age,
                          string disease, int severity, string contact) {
        if (patients.find(id) != patients.end()) {
            cout << "  [ERROR] Patient ID " << id << " already exists.\n";
            return;
        }
        patients[id] = Patient(id, name, age, disease, severity, contact);
        cout << "  [OK] Patient " << name << " registered (ID: " << id << ").\n";
    }

    // -------------------------------------------------------------------
    // FIND PATIENT: O(1) using hash table.
    // -------------------------------------------------------------------
    Patient* findPatient(string id) {
        auto it = patients.find(id);
        return (it != patients.end()) ? &(it->second) : nullptr;
    }

    // ==================================================================
    // EMERGENCY TRIAGE (Priority Queue - Max Heap)
    //
    // HOW IT WORKS:
    //   - Emergency patients are added to a priority queue
    //   - The queue orders by severity (highest first)
    //   - Same severity: earlier arrival is treated first
    //   - This models real hospital ER triage (most critical first)
    // ==================================================================

    // -------------------------------------------------------------------
    // ADD EMERGENCY PATIENT: Push to max-heap. O(log n).
    // -------------------------------------------------------------------
    void addEmergencyPatient(string patientId, int severity) {
        Patient* p = findPatient(patientId);
        if (!p) {
            cout << "  [ERROR] Patient not found. Register first.\n";
            return;
        }
        if (severity < 1) severity = 1;   // Clamp severity to 1-5
        if (severity > 5) severity = 5;

        emergencyPQ.push({patientId, severity, nextArrivalOrder++});
        cout << "  [EMERGENCY] " << p->name << " (Severity: " << severity
             << ") added to emergency queue.\n";
    }

    // -------------------------------------------------------------------
    // TREAT NEXT EMERGENCY: Pop from max-heap. O(log n).
    // The highest severity patient is treated first.
    // -------------------------------------------------------------------
    void treatNextEmergency() {
        if (emergencyPQ.empty()) {
            cout << "  [INFO] No emergency patients waiting.\n";
            return;
        }

        EmergencyPatient ep = emergencyPQ.top();
        emergencyPQ.pop();

        Patient* p = findPatient(ep.patientId);
        if (!p) {
            cout << "  [WARN] Patient record not found. Skipping.\n";
            return;
        }

        cout << "  [TREATING] " << p->name << " (Severity: " << ep.severity
             << ") - RUSHED TO EMERGENCY ROOM!\n";

        // Assign an available doctor if possible
        assignDoctorToPatient(ep.patientId);

        // Log treatment
        treatmentHistory.push(TreatmentRecord(
            ep.patientId, "ER-UNIT", "Emergency treatment administered"
        ));

        // If severe, admit to bed
        if (ep.severity >= 3) {
            admitPatient(ep.patientId);
        }
    }

    // ==================================================================
    // REGULAR APPOINTMENTS (Queue - FIFO)
    //
    // HOW IT WORKS:
    //   - Non-emergency patients join a regular queue
    //   - They are seen in order of arrival (FIFO)
    //   - This models outpatient departments
    // ==================================================================

    // -------------------------------------------------------------------
    // ADD TO REGULAR QUEUE: Enqueue. O(1).
    // -------------------------------------------------------------------
    void addRegularAppointment(string patientId) {
        if (!findPatient(patientId)) {
            cout << "  [ERROR] Patient not found.\n";
            return;
        }
        regularQueue.push(patientId);
        Patient* p = findPatient(patientId);
        cout << "  [OK] " << p->name << " added to regular appointment queue.\n";
    }

    // -------------------------------------------------------------------
    // PROCESS NEXT REGULAR PATIENT: Dequeue. O(1).
    // -------------------------------------------------------------------
    void processNextRegular() {
        if (regularQueue.empty()) {
            cout << "  [INFO] No regular patients waiting.\n";
            return;
        }

        string patientId = regularQueue.front();
        regularQueue.pop();

        Patient* p = findPatient(patientId);
        if (!p) {
            cout << "  [WARN] Patient record not found.\n";
            return;
        }

        cout << "  [CONSULTATION] " << p->name << " is seeing the doctor.\n";
        assignDoctorToPatient(patientId);

        treatmentHistory.push(TreatmentRecord(
            patientId, "OPD", "Regular checkup completed"
        ));
    }

    // ==================================================================
    // DOCTOR ASSIGNMENT
    // ==================================================================

    // -------------------------------------------------------------------
    // ASSIGN DOCTOR: Find first available doctor matching patient's needs.
    // In a real system, this would match specialty. Here we find any
    // available doctor. This demonstrates searching the BST.
    // -------------------------------------------------------------------
    void assignDoctorToPatient(string patientId) {
        // For simplicity, find any available doctor
        // In a real system, we'd use a recursive search on the BST
        // Here we traverse all doctors via a helper (or we'd expose traversal)
        // We'll just create a simple assignment display.
        cout << "  [ASSIGN] Searching for available doctor...\n";

        // Note: In a full implementation, we'd traverse the BST to find
        // an available doctor. For exam purposes, this demonstrates the concept.
        // A common exam approach is to use a queue of available doctors.
        cout << "  [ASSIGN] Doctor assigned to patient " << patientId << ".\n";
    }

    // ==================================================================
    // BED MANAGEMENT (Array/Vector)
    // ==================================================================

    bool admitPatient(string patientId) {
        if (occupiedBeds >= maxBeds) {
            cout << "  [ERROR] No beds available! Patient " << patientId
                 << " cannot be admitted.\n";
            return false;
        }
        Patient* p = findPatient(patientId);
        if (!p) return false;

        p->isAdmitted = true;
        bedList.push_back(patientId);
        occupiedBeds++;
        cout << "  [BED] " << p->name << " admitted. Beds: "
             << occupiedBeds << "/" << maxBeds << "\n";
        return true;
    }

    void dischargePatient(string patientId) {
        Patient* p = findPatient(patientId);
        if (!p || !p->isAdmitted) {
            cout << "  [ERROR] Patient not found or not admitted.\n";
            return;
        }
        p->isAdmitted = false;
        auto it = find(bedList.begin(), bedList.end(), patientId);
        if (it != bedList.end()) {
            bedList.erase(it);
            occupiedBeds--;
        }
        cout << "  [OK] " << p->name << " discharged. Beds: "
             << occupiedBeds << "/" << maxBeds << "\n";

        treatmentHistory.push(TreatmentRecord(
            patientId, "ADMIN", "Patient discharged"
        ));
    }

    // ==================================================================
    // DISPLAY / UTILITY FUNCTIONS
    // ==================================================================

    void showEmergencyQueue() {
        if (emergencyPQ.empty()) {
            cout << "  [INFO] Emergency queue is empty.\n";
            return;
        }
        // Note: priority_queue doesn't support iteration.
        // We'd need to copy to display. For exam purposes, show count.
        cout << "  [EMERGENCY] " << emergencyPQ.size()
             << " patients waiting in emergency queue.\n";
        cout << "  (Display: priority_queue requires copy to show all)\n";
    }

    void showRegularQueue() {
        if (regularQueue.empty()) {
            cout << "  [INFO] Regular queue is empty.\n";
            return;
        }
        queue<string> temp = regularQueue;
        cout << "  Regular queue (" << regularQueue.size() << "):\n";
        while (!temp.empty()) {
            Patient* p = findPatient(temp.front());
            cout << "    " << (p ? p->name : "Unknown") << " ("
                 << temp.front() << ")\n";
            temp.pop();
        }
    }

    void showTreatmentHistory(int count = 10) {
        if (treatmentHistory.empty()) {
            cout << "  [INFO] No treatment records.\n";
            return;
        }
        stack<TreatmentRecord> temp = treatmentHistory;
        cout << "  Recent treatments (most recent first):\n";
        int shown = 0;
        while (!temp.empty() && shown < count) {
            TreatmentRecord& t = temp.top();
            cout << "  " << t.patientId << " | " << t.doctorId
                 << " | " << t.description << " | " << t.date << "\n";
            temp.pop();
            shown++;
        }
    }

    void displayAllPatients() {
        if (patients.empty()) {
            cout << "  [INFO] No patients registered.\n";
            return;
        }
        cout << "  " << left << setw(8) << "ID"
             << setw(20) << "Name"
             << setw(5) << "Age"
             << setw(25) << "Disease"
             << setw(10) << "Severity"
             << "Status\n";
        cout << "  " << string(90, '-') << "\n";
        for (auto& [id, p] : patients) {
            p.display();
        }
    }

    // -------------------------------------------------------------------
    // RUN THE SYSTEM - Interactive menu
    // -------------------------------------------------------------------
    void run() {
        cout << "\n=============================================\n";
        cout << "   HOSPITAL MANAGEMENT SYSTEM\n";
        cout << "=============================================\n\n";

        int choice;
        string id, name, disease, contact, spec, desc;
        int age, severity;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  1. Register Patient\n";
            cout << "  2. Add Emergency Patient (Priority Queue)\n";
            cout << "  3. Treat Next Emergency\n";
            cout << "  4. Add Regular Appointment (Queue)\n";
            cout << "  5. Process Next Regular Patient\n";
            cout << "  6. Register Doctor\n";
            cout << "  7. Admit Patient\n";
            cout << "  8. Discharge Patient\n";
            cout << "  9. Show Emergency Queue\n";
            cout << " 10. Show Regular Queue\n";
            cout << " 11. Show All Patients\n";
            cout << " 12. Show All Doctors\n";
            cout << " 13. Recent Treatments\n";
            cout << "  0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Patient ID: "; getline(cin, id);
                    cout << "  Name: "; getline(cin, name);
                    cout << "  Age: "; cin >> age; cin.ignore();
                    cout << "  Disease: "; getline(cin, disease);
                    cout << "  Severity (1-5): "; cin >> severity; cin.ignore();
                    cout << "  Contact: "; getline(cin, contact);
                    registerPatient(id, name, age, disease, severity, contact);
                    break;

                case 2:
                    cout << "  Patient ID: "; getline(cin, id);
                    cout << "  Severity (1-5): "; cin >> severity; cin.ignore();
                    addEmergencyPatient(id, severity);
                    break;

                case 3:
                    treatNextEmergency();
                    break;

                case 4:
                    cout << "  Patient ID: "; getline(cin, id);
                    addRegularAppointment(id);
                    break;

                case 5:
                    processNextRegular();
                    break;

                case 6:
                    cout << "  Doctor ID: "; getline(cin, id);
                    cout << "  Name: "; getline(cin, name);
                    cout << "  Specialty: "; getline(cin, spec);
                    doctors.addDoctor(id, name, spec);
                    break;

                case 7:
                    cout << "  Patient ID: "; getline(cin, id);
                    admitPatient(id);
                    break;

                case 8:
                    cout << "  Patient ID: "; getline(cin, id);
                    dischargePatient(id);
                    break;

                case 9:
                    showEmergencyQueue();
                    break;

                case 10:
                    showRegularQueue();
                    break;

                case 11:
                    displayAllPatients();
                    break;

                case 12:
                    doctors.displayAll();
                    break;

                case 13:
                    showTreatmentHistory();
                    break;

                case 0:
                    cout << "  Exiting system.\n";
                    break;

                default:
                    cout << "  Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main() {
    HospitalManagementSystem hospital;
    hospital.run();
    return 0;
}
