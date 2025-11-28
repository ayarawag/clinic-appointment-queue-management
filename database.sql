PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS patients(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    phone TEXT UNIQUE,
    email TEXT UNIQUE,
    password TEXT,
    failedAttempts INTEGER DEFAULT 0,
    lockedUntil INTEGER DEFAULT 0,
    preferredChannel TEXT DEFAULT 'app'
);

CREATE TABLE IF NOT EXISTS doctors(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    specialization TEXT,
    schedule TEXT
);

CREATE TABLE IF NOT EXISTS appointments(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    patientId INTEGER,
    doctorId INTEGER,
    dateTime TEXT,
    paid INTEGER DEFAULT 0,
    FOREIGN KEY(patientId) REFERENCES patients(id),
    FOREIGN KEY(doctorId) REFERENCES doctors(id)
);

CREATE TABLE IF NOT EXISTS queue(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    patientId INTEGER,
    position INTEGER,
    FOREIGN KEY(patientId) REFERENCES patients(id)
);

CREATE TABLE IF NOT EXISTS receipts(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    appointmentId INTEGER,
    method TEXT,
    status TEXT,
    message TEXT,
    createdAt INTEGER,
    FOREIGN KEY(appointmentId) REFERENCES appointments(id)
);