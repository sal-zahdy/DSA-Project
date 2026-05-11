Student Management System

Group Members and IDs

Salma Abdulaziz Ahmed-2300426
Mariam Tamer Shafiek-2300507
Rewan Abdo Mohamed-Ameen 2300239
Malak Mohammed Ali-2300429
Doaa Mohamed-2300390

Project Description

The Student Management System is a C++ application designed to manage and analyze academic records. The system allows users to store student information, manage multiple grades for each student, and generate detailed academic performance reports.

The application provides tools for calculating class averages, identifying top and lowest-performing students, displaying pass/fail summaries, and ranking students based on their performance. Additionally, the system supports data persistence by saving and loading records from local files, ensuring that student information is maintained between program executions.

The project also includes a graphical user interface developed using SFML to improve usability and user interaction.

Selected Data Structures

1. vector (Dynamic Array)

Used for the primary studentDB container to store Student objects.

Why It Was Chosen
Provides efficient O(1) access to elements by index.
Dynamically resizes as new students are added.
Suitable for managing a growing list of student records without requiring a fixed size. 

2. struct Student

A custom structure used to group related student data into a single entity.

Attributes:
Student ID
Student Name
Vector of Grades

Why It Was Chosen
Improves code organization and readability.
Keeps all student-related information together.
Simplifies data handling throughout the program.

Implemented Features:
Student Management
Add new students
Remove existing students
Update student information
Manage multiple grades per student
Search students by unique ID
Search students by name
Class Average

Performance Tracking:
Identifies top-performing students
Identifies the lowest-performing students

Pass/Fail Summary:
Displays the number of students who passed or failed based on a 50% threshold.

Student Ranking:
Displays all students ranked according to their average grades.

Data Persistence:
Automatically loads saved records at startup
Saves updated records into a local file
This ensures that all data remains available between sessions.

Input Validation

The system validates:
Numeric student IDs
Grade ranges between 0 and 100
Correct menu selections

This prevents invalid or corrupted data from being entered.

Technologies Used
C++
SFML (Simple and Fast Multimedia Library)
File Handling
Object-Oriented Programming Concepts
Dynamic Arrays (vector)

How to Compile and Run
Prerequisites:

Ensure that you have:

A C++ compiler installed (such as g++)
SFML library is properly installed and configured

Compilation

Open the terminal inside the project directory and run:

g++ main.cpp StudentManager.cpp reports.cpp grade_logic.cpp file_manager.cpp -o StudentSystem

If using SFML, compile with:

g++ main.cpp StudentManager.cpp reports.cpp grade_logic.cpp file_manager.cpp -o StudentSystem -lsfml-graphics -lsfml-window -lsfml-system

Execution
Windows
StudentSystem.exe
Linux / Mac
./StudentSystem

AI Usage Declaration
AI Tools Used
ChatGPT

Purpose of AI Usage

AI tools were used for:

Debugging assistance
Brainstorming project features and structure
Assistance while implementing the SFML graphical user interface
Improving code organization and documentation
General programming guidance

The project team completed all final implementation, testing, and integration.
