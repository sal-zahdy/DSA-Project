#include <iostream>
#include <vector>
#include "grade_logic.h"
#include "reports.h"


using namespace std;

// Update/set grade function for a student by ID
void updateStudentGrades(int studentID, const vector<float>& newGrades)
{
    if (studentID <= 0)
    {
        cout << "Error: Invalid student ID.\n";
        return;
    }

    if (newGrades.empty())
    {
        cout << "Error: Cannot update with empty grades.\n";
        return;
    }

    // Check for invalid grade values
    for (float grade : newGrades)
    {
        if (grade < 0.0f || grade > 100.0f)
        {
            cout << "Error: Invalid grade input. Grades must be between 0 and 100.\n";
            return;
        }
    }

    vector<Student>& db = getStudentDB();
    bool found = false;

    // Linear search for the student
    for (int i = 0; i < (int)db.size(); ++i)
    {
        if (db[i].id == studentID)
        {
            db[i].grades = newGrades;
            cout << "Grades updated successfully for student ID " << studentID << ".\n";
            found = true;
            break;
        }
    }

    if (!found)
    {
        cout << "Error: Student with ID " << studentID << " not found.\n";
    }
}

// Letter grade calculation with clear grading thresholds
char calculateLetterGrade(float average)
{
    if (average >= 90.0f) return 'A';
    else if (average >= 80.0f) return 'B';
    else if (average >= 70.0f) return 'C';
    else if (average >= 50.0f) return 'D';
    else return 'F';
}

// GPA calculation based on letter grade
int calculateGPA(char letterGrade)
{
    switch (letterGrade)
    {
    case 'A':
        return 4;
    case 'B':
        return 3;
    case 'C':
        return 2;
    case 'D':
        return 1;
    case 'F':
        return 0;
    default:
        return 0;
    }
}

// Pass/fail rule based on 50 threshold
bool isPass(float average)
{
    const float PASS_THRESHOLD = 50.0f;
    return average >= PASS_THRESHOLD;
}


