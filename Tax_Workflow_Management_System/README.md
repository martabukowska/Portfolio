# Tax Workflow Management System

A full-stack workflow management application designed to integrate practice management and tax preparation systems, addressing critical workflow challenges and enhancing operational efficiency.

## Core Features

### Automated Workflow Management
- **Deadline Compliance Tracking**
  - Automated tracking of internal and external deadlines
  - SQLite database integration for real-time compliance monitoring
  - Customizable alerts and notifications

### Smart Project Dependencies
- **Intelligent Sequencing Algorithm**
  - Automated sequencing of PTET and related corporate tax returns
  - Dedicated algorithm for dependency management
  - Priority-based workflow optimization

### Advanced Client Matching
- **Hybrid Matching System**
  - Token-based and fuzzy string comparison algorithms
  - Resolution of business name variations and abbreviations
  - Cross-system client identification

### Personalized Analytics
- **Role-Based Dashboard**
  - Customizable project analytics
  - Real-time workflow insights
  - Parameterized report generation with custom filtering

### Security & Authentication
- **Secure Authentication System**
  - JWT token management
  - BCrypt password hashing
  - Role-based access control

## Current Development

### In Progress Features
1. **User Interface Components**
   - Crow-based web interface
   - Responsive dashboard design
   - Interactive analytics views

2. **Automated Notifications**
   - Email reminder system
   - Status update alerts
   - Deadline proximity warnings

3. **IRS Integration**
   - Automated detection of projects requiring status updates based on IRS e-filing status
   - Real-time filing status updates
   - Compliance monitoring

## Business Impact

- Projected 50% reduction in workflow management time
- Improved deadline compliance tracking
- Enhanced project dependency management
- Streamlined client matching across systems
- Real-time workflow insights and analytics

## Technical Stack

- **Language**: C++ (Crow)
- **Database**: SQLite3 with transaction management
- **Authentication**: JWT (JSON Web Tokens)
- **Password Encryption**: BCrypt
- **External Libraries**:
  - `jwt-cpp`
  - `BCrypt`
  - `SQLite3`
  - `OpenSSL`

## System Architecture

### Core Components

#### Data Management
- **CSV Management**
  - Project data import/export
  - Database operations
  - Custom filtering system
  - Column index configuration

#### Client Matching
- **Lacerte Cross-Reference**
  - Name preprocessing and tokenization
  - Levenshtein distance calculations
  - Equivalent terms handling
  - Excel report generation

#### Project Management
- **Reminder System**
  - Project deadline tracking
  - Email notification system
  - CSV-based reminder loading
  - Event logging

#### Reporting & Analytics
- **Report Generator**
  - Condition-based report generation
  - Deadline management
  - CSV export functionality
  - Project status tracking

#### User Management
- **Authentication System**
  - JWT-based authentication
  - Password management
  - Rate limiting
  - Account security

#### API & Interface
- **Web API (Crow)**
  - RESTful endpoints
  - CORS handling
  - Authentication middleware
  - JSON response handling

#### Statistics & Analytics
- **Statistics Engine**
  - Role-based filtering
  - Deadline analytics
  - Extension tracking
  - Custom metrics calculation

#### System Infrastructure
- **Logging System**
  - Thread-safe operation
  - Timestamp management
  - Automatic file handling
  - Event tracking

## Prerequisites

- C++ Compiler (C++17 or later)
- SQLite3
- OpenSSL
- CMake (3.10 or later)

## Installation

1. Clone the repository:
```bash
git clone https://github.com/martabukowska/Portfolio/Tax_Workflow_Management_System.git
cd Portfolio/Tax_Workflow_Management_System
```

2. Create build directory:
```bash
mkdir build
cd build
```

3. Build the project:
```bash
cmake ..
make
```

## Usage

1. Initialize the database:
```bash
./tax-system --init-db
```

2. Start the system:
```bash
./tax-system
```

## Security Features

- Password hashing with BCrypt
- JWT token-based authentication
- Rate limiting for login attempts
- Account lockout protection
- Secure password reset mechanism

## Statistics Features

- Projects per deadline
- Extension statistics
- Role-based statistical views
- Filtering capabilities:
  - By deadline
  - By project type
  - By manager/partner
  - By group