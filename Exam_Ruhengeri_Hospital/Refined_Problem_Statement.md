# Examination Problem: Ruhengeri Referral Hospital Healthcare System

## Background
Ruhengeri Referral Hospital is both a referral hospital and the only district hospital in Musanze District, which comprises 15 sectors, 68 cells, and 432 villages. It serves a population of approximately 506,557 people. The hospital covers 16 health centers, 1 prison dispensary, and 13 health posts. Its mission is to improve the well-being and promote the health of the population in Musanze. However, the hospital also serves parts of neighboring districts including Burera, Nyabihu, and Gakenke. The population from these neighboring districts is estimated to be 386,080, covering an additional 18 health centers. 

Ruhengeri Referral Hospital was built in 1939 during the colonial period and started its activities as a public clinic between 1939 and 1964. In the 1980s, Ruhengeri Referral Hospital was recognized as a national referral hospital. Since 1999, it was ranked as a district hospital but continued to receive transfers from other neighboring district hospitals (such as Shyira DH, Nemba DH, Gisenyi DH, and Butaro DH). Since 2013, Ruhengeri Referral Hospital has been officially recognized by the Government of Rwanda as a Referral Hospital.

## Problem Statement
Ruhengeri Referral Hospital is requesting a C++ programmer to develop its healthcare system to manage patient appointments using **3 Linked Lists**. 

The three linked lists and their respective required data fields are:
1. **PatientsLL** (`patient_id` INTEGER, `name` STRING, `dob` STRING, `gender` STRING)
2. **DoctorsLL** (`doctor_id` INTEGER, `name` STRING, `specialization` STRING)
3. **AppointmentsLL** (`appointment_id` INTEGER, `patient_id` INTEGER, `doctor_id` INTEGER, `appointment_date` STRING)

### Technical Constraints
As a software engineer, you are requested to develop this system for Ruhengeri Referral Hospital considering the following strict constraints:
- One `patient_id` references only one patient.
- One `doctor_id` references only one doctor.
- One `appointment_id` references only one appointment.
  - *If a user attempts to register an ID that is already in the system, notify the user that the given ID already exists.*
- An appointment is only possible if both the `patient_id` and `doctor_id` exist in `PatientsLL` and `DoctorsLL` respectively. 
  - *Otherwise, notify the user that one of them does not exist.*

### Menu Interface
The program must run on a menu-driven basis exactly as follows:
1. Register a Patient
2. Register a Doctor
3. Register an Appointment
4. Display all Patients
5. Display Doctors
6. Display Appointments
7. Exit

### Terminal Output Examples
Your output must exactly match the formatting shown below:

```text
Menu:
1.Register a Patient
2.Register a Doctor
3.Register an Appointment
4.Display all Patients
5.Display  Doctors
6.Display  Appointments
7.Exit

Enter your choice: 1
PATIENT REGISTRATION
--------------------
ID:1
NAME: Muhirwa Hassan
Dob: 20/10/2000
GENDER: Male

Enter your choice: 2
DOCTOR REGISTRATION
-------------------
ID:1
NAME: Drc Ngango
SPECIALIZATION: Cardiologist  
------------------

Enter your choice:4
Patient ID: 1, Name: Muhirwa Hassan, DOB: 20/10/2000, Gender: Male

Enter your choice: 5
Doctor ID:1, Name: Drc Ngango, Specialization: Cardiologist

Enter your choice: 3
APPOINTMENT REGISTRATION
------------------------
ID:1
P_ID:1
D_ID:1
DATE: 20/10/2022

Enter your choice: 6
Appointment ID: 1, Patient ID: 1, Doctor ID: 1, Date: 20/10/2022
------------------------
```
