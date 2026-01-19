-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 19, 2026 at 07:20 PM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `divingresortdb`
--

-- --------------------------------------------------------

--
-- Table structure for table `booking`
--

CREATE TABLE `booking` (
  `bookingID` int(11) NOT NULL,
  `cusID` int(11) NOT NULL,
  `bookingDate` date NOT NULL,
  `bookingStatus` enum('Confirmed','Cancelled','Pending') NOT NULL,
  `totalAmount` decimal(10,2) NOT NULL,
  `cancellationDate` date DEFAULT NULL,
  `refundAmount` decimal(10,2) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `booking`
--

INSERT INTO `booking` (`bookingID`, `cusID`, `bookingDate`, `bookingStatus`, `totalAmount`, `cancellationDate`, `refundAmount`) VALUES
(2, 1, '2025-10-10', 'Cancelled', 400.00, '2026-01-17', 200.00),
(3, 2, '2025-10-11', 'Cancelled', 980.00, '2025-10-14', 980.00),
(4, 3, '2025-10-19', 'Confirmed', 980.00, NULL, NULL),
(6, 4, '2025-10-19', 'Confirmed', 980.00, NULL, NULL),
(7, 5, '2025-10-20', 'Confirmed', 200.00, NULL, NULL),
(8, 6, '2025-11-02', 'Confirmed', 1200.00, NULL, NULL),
(9, 7, '2025-11-03', 'Confirmed', 1100.00, NULL, NULL),
(10, 8, '2025-12-08', 'Confirmed', 1250.00, NULL, NULL),
(11, 9, '2025-12-09', 'Confirmed', 1250.00, NULL, NULL),
(22, 22, '2025-12-17', 'Confirmed', 300.00, NULL, NULL),
(27, 12, '2026-01-07', 'Cancelled', 1100.00, '2026-01-07', 1100.00),
(28, 14, '2026-01-07', 'Confirmed', 1100.00, NULL, NULL),
(29, 13, '2026-01-07', 'Confirmed', 1100.00, NULL, NULL),
(30, 8, '2026-01-07', 'Confirmed', 200.00, NULL, NULL),
(31, 9, '2026-01-07', 'Confirmed', 400.00, NULL, NULL),
(32, 18, '2026-01-07', 'Confirmed', 750.00, NULL, NULL),
(33, 11, '2026-01-07', 'Confirmed', 750.00, NULL, NULL),
(35, 29, '2026-01-08', 'Confirmed', 1100.00, NULL, NULL),
(36, 31, '2026-01-08', 'Confirmed', 680.00, NULL, NULL),
(37, 15, '2026-01-08', 'Confirmed', 1250.00, NULL, NULL),
(38, 21, '2026-01-08', 'Cancelled', 1250.00, '2026-01-08', 1250.00),
(39, 5, '2026-01-08', 'Confirmed', 1250.00, NULL, NULL),
(40, 33, '2026-01-10', 'Confirmed', 400.00, NULL, NULL),
(42, 40, '2026-01-12', 'Cancelled', 1100.00, '2026-01-12', 550.00),
(43, 42, '2026-01-19', 'Cancelled', 1100.00, '2026-01-19', 550.00);

-- --------------------------------------------------------

--
-- Table structure for table `booking_service`
--

CREATE TABLE `booking_service` (
  `bookingID` int(11) NOT NULL,
  `serviceID` int(11) NOT NULL,
  `serviceDate` date NOT NULL,
  `quantity` int(11) NOT NULL DEFAULT 1,
  `subtotal` decimal(10,2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `booking_service`
--

INSERT INTO `booking_service` (`bookingID`, `serviceID`, `serviceDate`, `quantity`, `subtotal`) VALUES
(2, 5, '2025-10-25', 1, 400.00),
(3, 6, '2025-10-28', 1, 980.00),
(4, 6, '2025-10-28', 1, 980.00),
(6, 6, '2025-10-28', 1, 980.00),
(7, 8, '2025-10-24', 1, 200.00),
(8, 3, '2025-11-15', 1, 1200.00),
(9, 2, '2025-11-09', 1, 1100.00),
(10, 1, '2025-12-20', 1, 1250.00),
(11, 1, '2025-12-20', 1, 1250.00),
(22, 8, '2025-11-13', 1, 300.00),
(27, 2, '2026-02-22', 1, 1100.00),
(28, 2, '2026-02-22', 1, 1100.00),
(29, 2, '2026-02-22', 1, 1100.00),
(30, 8, '2026-03-12', 1, 200.00),
(31, 5, '2026-03-01', 1, 400.00),
(32, 4, '2026-02-28', 1, 750.00),
(33, 4, '2026-01-25', 1, 750.00),
(35, 2, '2026-02-22', 1, 1100.00),
(36, 7, '2026-01-23', 1, 680.00),
(37, 1, '2026-01-31', 1, 1250.00),
(38, 1, '2026-01-31', 1, 1250.00),
(39, 1, '2026-01-31', 1, 1250.00),
(40, 5, '2026-02-27', 1, 400.00),
(42, 2, '2026-01-20', 1, 1100.00),
(43, 2, '2026-01-29', 1, 1100.00);

-- --------------------------------------------------------

--
-- Table structure for table `certificate`
--

CREATE TABLE `certificate` (
  `certID` int(11) NOT NULL,
  `bookingID` int(11) NOT NULL,
  `issueDate` date NOT NULL,
  `filePath` varchar(255) NOT NULL,
  `status` enum('Generated','Revoked') NOT NULL DEFAULT 'Generated',
  `createdAt` timestamp NOT NULL DEFAULT current_timestamp(),
  `updatedAt` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `certificate`
--

INSERT INTO `certificate` (`certID`, `bookingID`, `issueDate`, `filePath`, `status`, `createdAt`, `updatedAt`) VALUES
(1, 4, '2026-01-20', 'certificates/booking_4.txt', 'Generated', '2026-01-19 17:14:09', '2026-01-19 17:14:09');

-- --------------------------------------------------------

--
-- Table structure for table `customer`
--

CREATE TABLE `customer` (
  `cusID` int(11) NOT NULL,
  `cusName` varchar(100) NOT NULL,
  `cusEmail` varchar(100) DEFAULT NULL,
  `cusPhoneNo` varchar(20) NOT NULL,
  `certificationLevel` enum('No cert','Open Water Diver','Advanced Open Water Diver','Rescue Diver') NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `customer`
--

INSERT INTO `customer` (`cusID`, `cusName`, `cusEmail`, `cusPhoneNo`, `certificationLevel`) VALUES
(1, 'Alice Tan', 'alice.tan@example.com', '0123456789', 'No cert'),
(2, 'Benjamin Lee', 'ben.lee@example.com', '0129876543', 'Open Water Diver'),
(3, 'Chong Wei', 'chong.wei@example.com', '0134567890', 'Advanced Open Water Diver'),
(4, 'Siti Diana', 'siii.diana@example.com', '0145678901', 'Open Water Diver'),
(5, 'Ethan Wong', 'ethan.wong@example.com', '0156789012', 'No cert'),
(6, 'Farah Aziz', 'farah.aziz@example.com', '0167890123', 'Advanced Open Water Diver'),
(7, 'George Tan', 'george.tan@example.com', '0178901234', 'Open Water Diver'),
(8, 'Hannah Ng', 'hannah.ng@example.com', '0189012345', 'No cert'),
(9, 'Ivan Lim', 'ivan.lim@example.com', '0190123456', 'No cert'),
(10, 'Jasmine Teo', 'jasmine.teo@example.com', '0101234567', 'Open Water Diver'),
(11, 'Muhamad Nasrizal Hamidi', 'nasrizalhamidi@example.com', '0123019667', 'No cert'),
(12, 'Thum Ler Chou', 'lerchouthum@example.com', '0191388688', 'Open Water Diver'),
(13, 'Rahma Nurhayati', 'rahma@mail.com', '01455461209', 'Open Water Diver'),
(14, 'Xiao Ming', 'xm@mail.com', '0125123044', 'Open Water Diver'),
(15, 'Bobby', 'bobby@mail.com', '01754302212', 'No cert'),
(18, 'Mary the Lamb', 'mary@mail.com', '0121078836', 'No cert'),
(19, 'Ahmad Ali', 'admadali@mail.com', '01298775133', 'No cert'),
(20, 'Banana King', 'bananana@mail.com', '0111234123', 'Open Water Diver'),
(21, 'Cookie Monster', 'mamee@mail.com', '0122345234', 'No cert'),
(22, 'De Santa', 'ds@mail.com', '0145430119', 'No cert'),
(24, 'Daniel Eng', 'daniel33@mail.com', '01937109422', 'No cert'),
(29, 'zlan', 'a@yahoo.com', '0987654321', 'Open Water Diver'),
(30, 'r Bean', 'teddy@mail.com', '0192883743', 'Open Water Diver'),
(31, 'anda', 'panda@mail.com', '01276343245', 'Rescue Diver'),
(32, 'azlan', 'azlan@mail.com', '0167498921', 'Open Water Diver'),
(33, 'Fiona Wong', 'fiona11@mail.com', '0189923525', 'No cert'),
(34, 'Fiona Lee', 'fiaona11@mail.com', '0183924765', 'No cert'),
(40, 'fang yee', 'fyyy@hotmail.com', '01466726539', 'Advanced Open Water Diver'),
(41, 'Stitch', 'stitch33@mail.com', '01988764742', 'No cert'),
(42, 'che fang yee', 'fy@gmail.com', '01766635667', 'Advanced Open Water Diver');

-- --------------------------------------------------------

--
-- Table structure for table `instructor`
--

CREATE TABLE `instructor` (
  `instructorID` int(11) NOT NULL,
  `instructorName` varchar(100) NOT NULL,
  `instructorPhoneNo` varchar(20) NOT NULL,
  `instructorEmail` varchar(100) NOT NULL,
  `instructorStatus` enum('Available','Unavailable') NOT NULL DEFAULT 'Available',
  `specialization` enum('Diving','Swimming') NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `instructor`
--

INSERT INTO `instructor` (`instructorID`, `instructorName`, `instructorPhoneNo`, `instructorEmail`, `instructorStatus`, `specialization`) VALUES
(1, 'Ravichandran', '+60123456789', 'vasudevan07@example.com', 'Available', 'Diving'),
(2, 'Ikmal Zamre', '+60129876543', 'zamrez@example.com', 'Available', 'Swimming'),
(3, 'Gary Toh', '+60134567890', 'garynotgary@example.com', 'Available', 'Swimming'),
(4, 'Leng Quen Ki', '+60198875163', 'quenkileng@example.com', 'Available', 'Diving'),
(5, 'Darden Liliya', '+60146046022', 'darlili@example.com', 'Available', 'Diving'),
(6, 'Iqbal Harith', '+60115530579', 'toymytoy@example.com', 'Available', 'Diving'),
(7, 'Agustina Pradeep', '+60181106579', 'deepagus@example.com', 'Available', 'Diving'),
(8, 'Kayley Collins', '+60181106579', 'kaycol728@example.com', 'Available', 'Diving'),
(9, 'John Jayawardene', '+60145591203', 'johnjaya@example.com', 'Available', 'Swimming');

-- --------------------------------------------------------

--
-- Table structure for table `instructor_assignment`
--

CREATE TABLE `instructor_assignment` (
  `bookingID` int(11) NOT NULL,
  `instructorID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `instructor_assignment`
--

INSERT INTO `instructor_assignment` (`bookingID`, `instructorID`) VALUES
(2, 1),
(3, 4),
(4, 4),
(6, 4),
(7, 2),
(8, 8),
(9, 5),
(10, 7),
(11, 7),
(22, 2),
(27, 1),
(28, 4),
(29, 5),
(30, 2),
(31, 7),
(32, 5),
(33, 8),
(35, 1),
(36, 4),
(37, 5),
(38, 5),
(39, 5),
(42, 1),
(43, 1);

-- --------------------------------------------------------

--
-- Table structure for table `payment`
--

CREATE TABLE `payment` (
  `paymentID` int(11) NOT NULL,
  `bookingID` int(10) NOT NULL,
  `paymentDate` date NOT NULL,
  `paymentAmount` decimal(10,2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `payment`
--

INSERT INTO `payment` (`paymentID`, `bookingID`, `paymentDate`, `paymentAmount`) VALUES
(2, 2, '2025-10-18', 400.00),
(3, 3, '2025-10-19', 980.00),
(4, 4, '2025-10-19', 980.00),
(5, 22, '2025-12-17', 350.00),
(6, 6, '2025-10-19', 980.00),
(7, 7, '2025-10-20', 200.00),
(8, 8, '2025-11-02', 1200.00),
(9, 9, '2025-11-03', 1100.00),
(10, 10, '2025-12-08', 1250.00),
(11, 11, '2025-12-09', 1250.00),
(27, 27, '2026-01-07', 1100.00),
(28, 28, '2026-01-07', 1100.00),
(29, 29, '2026-01-07', 1100.00),
(30, 30, '2026-01-07', 200.00),
(31, 31, '2026-01-07', 400.00),
(32, 32, '2026-01-07', 750.00),
(33, 33, '2026-01-07', 750.00),
(35, 35, '2026-01-08', 1100.00),
(36, 36, '2026-01-08', 680.00),
(37, 37, '2026-01-08', 1250.00),
(38, 38, '2026-01-08', 1250.00),
(39, 39, '2026-01-08', 1250.00),
(41, 42, '2026-01-12', 1100.00),
(42, 43, '2026-01-19', 1100.00);

-- --------------------------------------------------------

--
-- Table structure for table `service`
--

CREATE TABLE `service` (
  `serviceID` int(11) NOT NULL,
  `serviceName` varchar(100) NOT NULL,
  `serviceType` enum('Diving Package','Swimming Lesson') NOT NULL,
  `serviceDescription` text DEFAULT NULL,
  `duration` int(11) NOT NULL,
  `paxPerSession` int(11) NOT NULL,
  `price` decimal(10,2) NOT NULL,
  `prerequisiteLevel` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `service`
--

INSERT INTO `service` (`serviceID`, `serviceName`, `serviceType`, `serviceDescription`, `duration`, `paxPerSession`, `price`, `prerequisiteLevel`) VALUES
(1, 'Open Water Course', 'Diving Package', 'Beginner course with certification included.', 4, 2, 1250.00, 0),
(2, 'Advanced Open Water Course', 'Diving Package', 'Advanced course with deep dive training.', 3, 3, 1100.00, 2),
(3, 'Rescue Diver Course', 'Diving Package', 'Learn to recognize and respond to dive emergencies.', 6, 3, 1200.00, 3),
(4, 'Discover Scuba Diving', 'Diving Package', 'Introductory dive for non-certified participants', 1, 4, 750.00, 0),
(5, 'Snorkelling', 'Diving Package', 'Swim along the surface of the water and enjoy the underwater world', 1, 4, 400.00, 0),
(6, 'Island hopping', 'Diving Package', 'Enjoy continuous diving that travel from one island to another.', 2, 5, 980.00, 2),
(7, 'Night Dive', 'Diving Package', 'Scuba diving during the hours of darkness to witness nocturnal marine life.', 1, 5, 680.00, 2),
(8, 'Swimming Lesson - Beginner ', 'Swimming Lesson', 'Basic swimming techniques', 3, 4, 200.00, 0);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `booking`
--
ALTER TABLE `booking`
  ADD PRIMARY KEY (`bookingID`),
  ADD KEY `cusID` (`cusID`);

--
-- Indexes for table `booking_service`
--
ALTER TABLE `booking_service`
  ADD PRIMARY KEY (`bookingID`,`serviceID`) USING BTREE,
  ADD KEY `serviceID` (`serviceID`);

--
-- Indexes for table `certificate`
--
ALTER TABLE `certificate`
  ADD PRIMARY KEY (`certID`),
  ADD UNIQUE KEY `uq_certificate_booking` (`bookingID`);

--
-- Indexes for table `customer`
--
ALTER TABLE `customer`
  ADD PRIMARY KEY (`cusID`),
  ADD UNIQUE KEY `cusEmail` (`cusEmail`);

--
-- Indexes for table `instructor`
--
ALTER TABLE `instructor`
  ADD PRIMARY KEY (`instructorID`),
  ADD UNIQUE KEY `instructorEmail` (`instructorEmail`);

--
-- Indexes for table `instructor_assignment`
--
ALTER TABLE `instructor_assignment`
  ADD PRIMARY KEY (`bookingID`,`instructorID`),
  ADD KEY `instructorID` (`instructorID`);

--
-- Indexes for table `payment`
--
ALTER TABLE `payment`
  ADD PRIMARY KEY (`paymentID`),
  ADD KEY `payment_ibfk_1` (`bookingID`);

--
-- Indexes for table `service`
--
ALTER TABLE `service`
  ADD PRIMARY KEY (`serviceID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `booking`
--
ALTER TABLE `booking`
  MODIFY `bookingID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=44;

--
-- AUTO_INCREMENT for table `certificate`
--
ALTER TABLE `certificate`
  MODIFY `certID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT for table `customer`
--
ALTER TABLE `customer`
  MODIFY `cusID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=43;

--
-- AUTO_INCREMENT for table `instructor`
--
ALTER TABLE `instructor`
  MODIFY `instructorID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=10;

--
-- AUTO_INCREMENT for table `payment`
--
ALTER TABLE `payment`
  MODIFY `paymentID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=43;

--
-- AUTO_INCREMENT for table `service`
--
ALTER TABLE `service`
  MODIFY `serviceID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=10;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `booking`
--
ALTER TABLE `booking`
  ADD CONSTRAINT `booking_ibfk_1` FOREIGN KEY (`cusID`) REFERENCES `customer` (`cusID`);

--
-- Constraints for table `booking_service`
--
ALTER TABLE `booking_service`
  ADD CONSTRAINT `booking_service_ibfk_1` FOREIGN KEY (`bookingID`) REFERENCES `booking` (`bookingID`),
  ADD CONSTRAINT `booking_service_ibfk_2` FOREIGN KEY (`serviceID`) REFERENCES `service` (`serviceID`);

--
-- Constraints for table `certificate`
--
ALTER TABLE `certificate`
  ADD CONSTRAINT `fk_certificate_booking` FOREIGN KEY (`bookingID`) REFERENCES `booking` (`bookingID`) ON DELETE CASCADE;

--
-- Constraints for table `instructor_assignment`
--
ALTER TABLE `instructor_assignment`
  ADD CONSTRAINT `instructor_assignment_ibfk_1` FOREIGN KEY (`bookingID`) REFERENCES `booking` (`bookingID`),
  ADD CONSTRAINT `instructor_assignment_ibfk_2` FOREIGN KEY (`instructorID`) REFERENCES `instructor` (`instructorID`);

--
-- Constraints for table `payment`
--
ALTER TABLE `payment`
  ADD CONSTRAINT `payment_ibfk_1` FOREIGN KEY (`bookingID`) REFERENCES `booking` (`bookingID`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
