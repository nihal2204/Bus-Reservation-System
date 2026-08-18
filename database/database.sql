CREATE DATABASE bus_reservation;

USE bus_reservation;

CREATE TABLE buses (
    id INT PRIMARY KEY,
    name VARCHAR(50),
    source VARCHAR(50),
    destination VARCHAR(50),
    seats INT
);

CREATE TABLE bookings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    passenger VARCHAR(100),
    bus_id INT,
    seat INT,

    FOREIGN KEY (bus_id)
    REFERENCES buses(id),

    UNIQUE(bus_id, seat)
);

INSERT INTO buses VALUES
(1, 'Bhopal Express', 'Bhopal', 'Indore', 40),
(2, 'Jabalpur Express', 'Bhopal', 'Jabalpur', 40),
(3, 'Mahakal Express', 'Bhopal', 'Ujjain', 40),
(4, 'Indore Express', 'Indore', 'Bhopal', 40),
(5, 'Ind-Ujjain Express', 'Indore', 'Ujjain', 40),
(6, 'Ind-Jabalpur', 'Indore', 'Jabalpur', 40),
(7, 'Capital', 'Jabalpur', 'Bhopal', 40),
(8, 'Jabalpur-Ind', 'Jabalpur', 'Indore', 40),
(9, 'Jabalpur-Mahakal', 'Jabalpur', 'Ujjain', 40),
(10, 'City Express', 'Ujjain', 'Bhopal', 40),
(11, 'Ujj-Ind', 'Ujjain', 'Indore', 40),
(12, 'Ujj-Jab', 'Ujjain', 'Jabalpur', 40);
