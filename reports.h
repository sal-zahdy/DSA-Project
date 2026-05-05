#ifndef REPORTS_H_INCLUDED
#define REPORTS_H_INCLUDED
#include <string>
#include <vector>
#include "student.h"
using namespace std;

void displayAllStudentsTable();         // prints all students in a formatted table
void displayClassAverage();             // calculates and prints the class average
void displayTopStudent();               // finds and prints the highest average student
void displayLowestStudent();            // finds and prints the lowest average student
void displayPassFailSummary();          // counts and prints how many passed and failed
float calculateAverage(const vector<float>& grades);
void displayRankedStudents();           // sorts a copy by average and displays ranked table
void displayStudentByID(int targetID);  // sorts a copy by ID then binary searches it
// Expose the getter so any file that includes reports.h can use it
vector<Student>& getStudentDB();


#endif // REPORTS_H_INCLUDED
