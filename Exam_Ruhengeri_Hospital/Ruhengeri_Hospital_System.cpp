#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

struct PatientNode {
    int patient_id;
    string name;
    string dob;
    string gender;
    PatientNode* next;

    PatientNode(int id, string n, string d, string g) 
        : patient_id(id), name(n), dob(d), gender(g), next(nullptr) {}
};

struct DoctorNode {
    int doctor_id;
    string name;
    string specialization;
    DoctorNode* next;

    DoctorNode(int id, string n, string spec) 
        : doctor_id(id), name(n), specialization(spec), next(nullptr) {}
};

struct AppointmentNode {
    int appointment_id;
    int patient_id;
    int doctor_id;
    string appointment_date;
    AppointmentNode* next;

    AppointmentNode(int a_id, int p_id, int d_id, string date) 
        : appointment_id(a_id), patient_id(p_id), doctor_id(d_id), appointment_date(date), next(nullptr) {}
};

PatientNode* patientsHead = nullptr;
DoctorNode* doctorsHead = nullptr;
AppointmentNode* appointmentsHead = nullptr;

bool patientExists(int id) {
    PatientNode* curr = patientsHead;
    while (curr != nullptr) {
        if (curr->patient_id == id) return true;
        curr = curr->next;
    }
    return false;
}

bool doctorExists(int id) {
    DoctorNode* curr = doctorsHead;
    while (curr != nullptr) {
        if (curr->doctor_id == id) return true;
        curr = curr->next;
    }
    return false;
}

bool appointmentExists(int id) {
    AppointmentNode* curr = appointmentsHead;
    while (curr != nullptr) {
        if (curr->appointment_id == id) return true;
        curr = curr->next;
    }
    return false;
}

void registerPatient() {
    cout << "PATIENT REGISTRATION" << endl;
    cout << "--------------------" << endl;
    
    int id;
    string name, dob, gender;
    
    cout << "ID:";
    cin >> id;
    
    if (patientExists(id)) {
        cout << "Error: A patient with this ID already exists!" << endl;
        return;
    }
    
    cin.ignore();
    
    cout << "NAME: ";
    getline(cin, name);
    
    cout << "Dob: ";
    getline(cin, dob);
    
    cout << "GENDER: ";
    getline(cin, gender);
    

    PatientNode* newNode = new PatientNode(id, name, dob, gender);
    newNode->next = patientsHead;
    patientsHead = newNode;
    cout << "Patient successfully registered!\n";
}

void registerDoctor() {
    cout << "DOCTOR REGISTRATION" << endl;
    cout << "-------------------" << endl;
    
    int id;
    string name, specialization;
    
    cout << "ID:";
    cin >> id;
    
    if (doctorExists(id)) {
        cout << "Error: A doctor with this ID already exists!" << endl;
        return;
    }
    
    cin.ignore();
    
    cout << "NAME: ";
    getline(cin, name);
    
    cout << "SPECIALIZATION: ";
    getline(cin, specialization);
    
    cout << "------------------" << endl;
    
    DoctorNode* newNode = new DoctorNode(id, name, specialization);
    newNode->next = doctorsHead;
    doctorsHead = newNode;
    cout << "Doctor successfully registered!\n";
}

void registerAppointment() {
    cout << "APPOINTMENT REGISTRATION" << endl;
    cout << "------------------------" << endl;
    
    int id, p_id, d_id;
    string date;
    
    cout << "ID:";
    cin >> id;
    
    if (appointmentExists(id)) {
        cout << "Error: An appointment with this ID already exists!" << endl;
        return;
    }
    
    cout << "P_ID:";
    cin >> p_id;
    
    cout << "D_ID:";
    cin >> d_id;
    
    if (!patientExists(p_id) || !doctorExists(d_id)) {
        cout << "Error: The given Patient ID or Doctor ID does not exist!" << endl;
        return;
    }
    
    cin.ignore();
    cout << " DATE: ";
    getline(cin, date);
    
    AppointmentNode* newNode = new AppointmentNode(id, p_id, d_id, date);
    newNode->next = appointmentsHead;
    appointmentsHead = newNode;
    cout << "Appointment successfully registered!\n";
}

void displayPatients() {
    PatientNode* curr = patientsHead;
    if (!curr) {
        cout << "No patients registered." << endl;
        return;
    }
    while (curr != nullptr) {
        cout << "Patient ID: " << curr->patient_id 
             << ", Name: " << curr->name 
             << ", DOB: " << curr->dob 
             << ", Gender: " << curr->gender << endl;
        curr = curr->next;
    }
}

void displayDoctors() {
    DoctorNode* curr = doctorsHead;
    if (!curr) {
        cout << "No doctors registered." << endl;
        return;
    }
    while (curr != nullptr) {
        cout << "Doctor ID:" << curr->doctor_id 
             << ", Name: " << curr->name 
             << ", Specialization: " << curr->specialization << endl;
        curr = curr->next;
    }
}

void displayAppointments() {
    AppointmentNode* curr = appointmentsHead;
    if (!curr) {
        cout << "No appointments registered." << endl;
        return;
    }
    while (curr != nullptr) {
        cout << "Appointment ID: " << curr->appointment_id 
             << ", Patient ID: " << curr->patient_id 
             << ", Doctor ID: " << curr->doctor_id 
             << ", Date: " << curr->appointment_date << endl;
        curr = curr->next;
    }
    cout << "------------------------" << endl;
}


void saveData() {
    
    ofstream pFile("patients_db.txt");
    if (pFile.is_open()) {
        PatientNode* pCurr = patientsHead;
        while (pCurr != nullptr) {
            pFile << pCurr->patient_id << "," << pCurr->name << "," << pCurr->dob << "," << pCurr->gender << "\n";
            pCurr = pCurr->next;
        }
        pFile.close();
    }

  
    ofstream dFile("doctors_db.txt");
    if (dFile.is_open()) {
        DoctorNode* dCurr = doctorsHead;
        while (dCurr != nullptr) {
            dFile << dCurr->doctor_id << "," << dCurr->name << "," << dCurr->specialization << "\n";
            dCurr = dCurr->next;
        }
        dFile.close();
    }

   
    ofstream aFile("appointments_db.txt");
    if (aFile.is_open()) {
        AppointmentNode* aCurr = appointmentsHead;
        while (aCurr != nullptr) {
            aFile << aCurr->appointment_id << "," << aCurr->patient_id << "," << aCurr->doctor_id << "," << aCurr->appointment_date << "\n";
            aCurr = aCurr->next;
        }
        aFile.close();
    }
    cout << "Data successfully saved to local text databases!\n";
}

void loadData() {
    string line;
    
  
    ifstream pFile("patients_db.txt");
    if (pFile.is_open()) {
        while (getline(pFile, line)) {
            stringstream ss(line);
            string idStr, name, dob, gender;
            getline(ss, idStr, ',');
            getline(ss, name, ',');
            getline(ss, dob, ',');
            getline(ss, gender, ',');
            
            if (!idStr.empty()) {
                PatientNode* newNode = new PatientNode(stoi(idStr), name, dob, gender);
                newNode->next = patientsHead;
                patientsHead = newNode;
            }
        }
        pFile.close();
    }

 
    ifstream dFile("doctors_db.txt");
    if (dFile.is_open()) {
        while (getline(dFile, line)) {
            stringstream ss(line);
            string idStr, name, spec;
            getline(ss, idStr, ',');
            getline(ss, name, ',');
            getline(ss, spec, ',');
            
            if (!idStr.empty()) {
                DoctorNode* newNode = new DoctorNode(stoi(idStr), name, spec);
                newNode->next = doctorsHead;
                doctorsHead = newNode;
            }
        }
        dFile.close();
    }

  
    ifstream aFile("appointments_db.txt");
    if (aFile.is_open()) {
        while (getline(aFile, line)) {
            stringstream ss(line);
            string idStr, pIdStr, dIdStr, date;
            getline(ss, idStr, ',');
            getline(ss, pIdStr, ',');
            getline(ss, dIdStr, ',');
            getline(ss, date, ',');
            
            if (!idStr.empty()) {
                AppointmentNode* newNode = new AppointmentNode(stoi(idStr), stoi(pIdStr), stoi(dIdStr), date);
                newNode->next = appointmentsHead;
                appointmentsHead = newNode;
            }
        }
        aFile.close();
    }
}

int main() {
   
    loadData();
    
    int choice;
    
    do {
        cout << "\nMenu:" << endl;
        cout << "1. Register a Patient" << endl;
        cout << "2. Register a Doctor" << endl;
        cout << "3. Register an appointment" << endl;
        cout << "4. Display  Patients" << endl;
        cout << "5. Display  Doctors" << endl;
        cout << "6. Display  Appointments" << endl;
        cout << "7. Save Data to File" << endl;
        cout << "8. Exit" << endl;
        cout << "Enter your choice: ";
        
        cin >> choice;
        
        switch (choice) {
            case 1:
                registerPatient();
                break;
            case 2:
                registerDoctor();
                break;
            case 3:
                registerAppointment();
                break;
            case 4:
                displayPatients();
                break;
            case 5:
                displayDoctors();
                break;
            case 6:
                displayAppointments();
                break;
            case 7:
                saveData();
                break;
            case 8:
             
                saveData();
                cout << "Exiting the program..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 8);
    
    return 0;
}
