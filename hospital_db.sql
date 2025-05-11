create database hospital_db;
use hospital_db;
CREATE TABLE Patients (
    id INT PRIMARY KEY AUTO_INCREMENT,
    first_name VARCHAR(50) NOT NULL,
    middle_name VARCHAR(50),
    last_name VARCHAR(50) NOT NULL,
    gender TINYINT NOT NULL CHECK (gender IN (1, 2)), -- 1 for Male, 2 for Female
    age INT NOT NULL,
    weight DOUBLE,
    height DOUBLE,
    disease TEXT
);
CREATE TABLE Bills (
    bill_id INT PRIMARY KEY AUTO_INCREMENT,
    patient_id INT NOT NULL,
    consultation_fee DOUBLE NOT NULL,
    medication_fee DOUBLE NOT NULL,
    lab_fee DOUBLE NOT NULL,
    room_charge DOUBLE NOT NULL,
    total_amount DOUBLE GENERATED ALWAYS AS (consultation_fee + medication_fee + lab_fee + room_charge),
    status VARCHAR(20) DEFAULT 'Unpaid',
    FOREIGN KEY (patient_id) REFERENCES Patients(id) ON DELETE CASCADE
);
CREATE TABLE Receipts (
    receipt_id INT PRIMARY KEY AUTO_INCREMENT,
    bill_id INT NOT NULL,
    patient_id INT NOT NULL,
    receipt_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    total DOUBLE NOT NULL,
    FOREIGN KEY (bill_id) REFERENCES Bills(bill_id) ON DELETE CASCADE,
    FOREIGN KEY (patient_id) REFERENCES Patients(id) ON DELETE CASCADE
);
CREATE TABLE Payments (
    payment_id INT PRIMARY KEY AUTO_INCREMENT,
    bill_id INT NOT NULL,
    amount_paid DOUBLE NOT NULL,
    payment_method VARCHAR(50) NOT NULL,
    card_number VARCHAR(16),
    cvc VARCHAR(3),
    expiry_date DATE,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (bill_id) REFERENCES Bills(bill_id) ON DELETE CASCADE
);