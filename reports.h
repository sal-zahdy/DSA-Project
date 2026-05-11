#ifndef REPORTS_H_INCLUDED
#define REPORTS_H_INCLUDED
#include <string>
#include <vector>
#include "StudentManager.h"
using namespace std;

void displayAllStudentsTable();
void displayClassAverage();
void displayTopStudent();
void displayLowestStudent();
void displayPassFailSummary();
float calculateAverage(const vector<float>& grades);
void displayRankedStudents();
void displayStudentByID(int targetID);
vector<Student>& getStudentDB();


#endif // REPORTS_H_INCLUDED
