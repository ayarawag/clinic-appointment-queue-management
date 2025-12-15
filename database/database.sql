DROP TABLE IF EXISTS patients;
DROP TABLE IF EXISTS doctors;
DROP TABLE IF EXISTS appointments;
DROP TABLE IF EXISTS notifications;
DROP TABLE IF EXISTS receipts;

CREATE TABLE patients (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    phone TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    createdAt INTEGER
);

CREATE TABLE doctors (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    specialization TEXT,
    schedule TEXT,
    createdAt INTEGER
);

CREATE TABLE appointments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    patientId INTEGER,
    doctorId INTEGER,
    dateTime TEXT,
    status TEXT DEFAULT 'booked',
    paid INTEGER DEFAULT 0,
    queue_position INTEGER DEFAULT 0,
    createdAt INTEGER
);

CREATE TABLE notifications (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    patientId INTEGER,
    message TEXT,
    sentAt INTEGER
);

CREATE TABLE receipts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    appointmentId INTEGER,
    method TEXT,
    status TEXT,
    message TEXT,
    createdAt INTEGER
);