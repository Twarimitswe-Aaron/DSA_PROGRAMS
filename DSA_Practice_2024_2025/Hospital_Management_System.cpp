// Hospital Management System using 3 Linked Lists
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

// Structs for Linked List Nodes
struct Patient {
    int patient_id;
    string name;
    string dob;
    string gender;
    Patient* next;
};

struct Doctor {
    int doctor_id;
    string name;
    string specialization;
    Doctor* next;
};

struct Appointment {
    int appointment_id;
    int patient_id;
    int doctor_id;
    string appointment_date;
    Appointment* next;
};

// Head pointers for our Linked Lists
Patient* patientsHead = nullptr;
Doctor* doctorsHead = nullptr;
Appointment* appointmentsHead = nullptr;

// Helper function to check if Patient ID exists
bool patientExists(int id) {
    Patient* temp = patientsHead;
    while (temp != nullptr) {
        if (temp->patient_id == id) return true;
        temp = temp->next;
    }
    return false;
}

// Helper function to check if Doctor ID exists
bool doctorExists(int id) {
    Doctor* temp = doctorsHead;
    while (temp != nullptr) {
        if (temp->doctor_id == id) return true;
        temp = temp->next;
    }
    return false;
}

// Helper function to check if Appointment ID exists
bool appointmentExists(int id) {
    Appointment* temp = appointmentsHead;
    while (temp != nullptr) {
        if (temp->appointment_id == id) return true;
        temp = temp->next;
    }
    return false;
}

// Helper function to get a string with spaces using getline
string readString(const string& prompt) {
    string input;
    cout << prompt;
    cin >> ws; // Clear leading whitespace
    getline(cin, input);
    return input;
}

// 1. Register Patient
void registerPatient() {
    int id;
    cout << "Enter Patient ID: ";
    cin >> id;

    if (patientExists(id)) {
        cout << "Error: Patient with ID " << id << " already exists!\\n";
        return;
    }

    string name = readString("Enter Name: ");
    string dob = readString("Enter Date of Birth (YYYY-MM-DD): ");
    string gender = readString("Enter Gender: ");

    Patient* newPatient = new Patient{id, name, dob, gender, nullptr};

    // Insert at the beginning (O(1))
    newPatient->next = patientsHead;
    patientsHead = newPatient;

    cout << "Patient registered successfully!\\n";
}

// 2. Register Doctor
void registerDoctor() {
    int id;
    cout << "Enter Doctor ID: ";
    cin >> id;

    if (doctorExists(id)) {
        cout << "Error: Doctor with ID " << id << " already exists!\\n";
        return;
    }

    string name = readString("Enter Name: ");
    string specialization = readString("Enter Specialization: ");

    Doctor* newDoctor = new Doctor{id, name, specialization, nullptr};

    newDoctor->next = doctorsHead;
    doctorsHead = newDoctor;

    cout << "Doctor registered successfully!\\n";
}

// 3. Register Appointment
void registerAppointment() {
    int appId, patId, docId;
    
    cout << "Enter Appointment ID: ";
    cin >> appId;

    if (appointmentExists(appId)) {
        cout << "Error: Appointment with ID " << appId << " already exists!\\n";
        return;
    }

    cout << "Enter Patient ID: ";
    cin >> patId;
    if (!patientExists(patId)) {
        cout << "Error: Patient with ID " << patId << " does not exist!\\n";
        return;
    }

    cout << "Enter Doctor ID: ";
    cin >> docId;
    if (!doctorExists(docId)) {
        cout << "Error: Doctor with ID " << docId << " does not exist!\\n";
        return;
    }

    string date = readString("Enter Appointment Date (YYYY-MM-DD): ");

    Appointment* newAppt = new Appointment{appId, patId, docId, date, nullptr};

    newAppt->next = appointmentsHead;
    appointmentsHead = newAppt;

    cout << "Appointment registered successfully!\\n";
}

// 4. Display Patients
void displayPatients() {
    if (patientsHead == nullptr) {
        cout << "No patients registered yet.\\n";
        return;
    }
    
    cout << "\\n--- Patient List ---\\n";
    cout << left << setw(10) << "ID" << setw(25) << "Name" 
         << setw(15) << "DOB" << setw(10) << "Gender" << "\\n";
    cout << "------------------------------------------------------------\\n";
    
    Patient* temp = patientsHead;
    while (temp != nullptr) {
        cout << left << setw(10) << temp->patient_id 
             << setw(25) << temp->name 
             << setw(15) << temp->dob 
             << setw(10) << temp->gender << "\\n";
        temp = temp->next;
    }
    cout << "------------------------------------------------------------\\n";
}

// 5. Display Doctors
void displayDoctors() {
    if (doctorsHead == nullptr) {
        cout << "No doctors registered yet.\\n";
        return;
    }
    
    cout << "\\n--- Doctor List ---\\n";
    cout << left << setw(10) << "ID" << setw(25) << "Name" 
         << setw(20) << "Specialization" << "\\n";
    cout << "-------------------------------------------------------\\n";
    
    Doctor* temp = doctorsHead;
    while (temp != nullptr) {
        cout << left << setw(10) << temp->doctor_id 
             << setw(25) << temp->name 
             << setw(20) << temp->specialization << "\\n";
        temp = temp->next;
    }
    cout << "-------------------------------------------------------\\n";
}

// 6. Display Appointments
void displayAppointments() {
    if (appointmentsHead == nullptr) {
        cout << "No appointments registered yet.\\n";
        return;
    }
    
    cout << "\\n--- Appointment List ---\\n";
    cout << left << setw(10) << "Appt ID" << setw(15) << "Patient ID" 
         << setw(15) << "Doctor ID" << setw(15) << "Date" << "\\n";
    cout << "-------------------------------------------------------\\n";
    
    Appointment* temp = appointmentsHead;
    while (temp != nullptr) {
        cout << left << setw(10) << temp->appointment_id 
             << setw(15) << temp->patient_id 
             << setw(15) << temp->doctor_id 
             << setw(15) << temp->appointment_date << "\\n";
        temp = temp->next;
    }
    cout << "-------------------------------------------------------\\n";
}

// Memory Cleanup
void cleanupMemory() {
    while (patientsHead != nullptr) {
        Patient* temp = patientsHead;
        patientsHead = patientsHead->next;
        delete temp;
    }
    while (doctorsHead != nullptr) {
        Doctor* temp = doctorsHead;
        doctorsHead = doctorsHead->next;
        delete temp;
    }
    while (appointmentsHead != nullptr) {
        Appointment* temp = appointmentsHead;
        appointmentsHead = appointmentsHead->next;
        delete temp;
    }
}

int main() {
    int choice;
    do {
        cout << "\\n========================================\\n";
        cout << "   Ruhengeri Referral Hospital System\\n";
        cout << "========================================\\n";
        cout << "1. Register Patient\\n";
        cout << "2. Register Doctor\\n";
        cout << "3. Register Appointment\\n";
        cout << "4. Display Patients\\n";
        cout << "5. Display Doctors\\n";
        cout << "6. Display Appointments\\n";
        cout << "7. Exit\\n";
        cout << "========================================\\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: registerPatient(); break;
            case 2: registerDoctor(); break;
            case 3: registerAppointment(); break;
            case 4: displayPatients(); break;
            case 5: displayDoctors(); break;
            case 6: displayAppointments(); break;
            case 7: 
                cout << "Exiting System... Cleaning up memory...\\n";
                cleanupMemory();
                break;
            default: cout << "Invalid choice! Please try again.\\n";
        }
    } while (choice != 7);

    return 0;
}
