-- Patients Table
CREATE TABLE IF NOT EXISTS patients (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    phone TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    createdAt INTEGER
);

-- Doctors Table
CREATE TABLE IF NOT EXISTS doctors (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    specialization TEXT NOT NULL,
    schedule TEXT
);

-- Appointments Table
CREATE TABLE IF NOT EXISTS appointments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    patient_id INTEGER,
    doctor_id INTEGER,
    date_time TEXT NOT NULL,
    status TEXT DEFAULT 'Scheduled',
    paid INTEGER DEFAULT 0,
    queue_position INTEGER DEFAULT NULL,
    FOREIGN KEY(patient_id) REFERENCES patients(id),
    FOREIGN KEY(doctor_id) REFERENCES doctors(id)
);

-- Notification Table
CREATE TABLE IF NOT EXISTS notifications (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    patient_id INTEGER,
    message TEXT,
    createdAt INTEGER DEFAULT (strftime('%s', 'now')),
    FOREIGN KEY(patient_id) REFERENCES patients(id)
);

-- Payment Receipts Table
CREATE TABLE IF NOT EXISTS receipts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    appointmentId INTEGER,
    method TEXT,
    status TEXT,
    message TEXT,
    createdAt INTEGER,
    FOREIGN KEY(appointmentId) REFERENCES appointments(id)
);