#include <iostream>
#include <vector>
#include "student.h"
#include "StudentManager.h"
#include "grade_logic.h"
#include "reports.h"

using namespace std;


int main()
{
    addStudent(1, "Ahmed", {85, 90});
    addStudent(2, "Sara", {78, 82});
    addStudent(3, "Ali", {88, 91});

    removeStudent(2);
    addStudent(2, "Sara_New", {95, 96});
    removeStudent(5);

    displayAllStudents();

    cout << "\n-------- Grade Logic Demo --------\n";

    vector<Student>& db = getStudentDB();

    for (const Student& s : db)
    {
        float avg = calculateAverage(s.grades);

        char letter = calculateLetterGrade(avg);
        int gpa = calculateGPA(letter);
        bool passed = isPass(avg);

        cout << "\nStudent: " << s.name << " (ID: " << s.id << ")\n";
        cout << "Average      : " << avg << "\n";
        cout << "Letter Grade : " << letter << "\n";
        cout << "GPA Points   : " << gpa << "\n";
        cout << "Passed       : " << (passed ? "Yes" : "No") << "\n";
    }
    return 0;
}
