#ifndef GRADE_LOGIC_H_INCLUDED
#define GRADE_LOGIC_H_INCLUDED

#include <vector>
#include "student.h"

//  Update/set grade function for a student by ID
void updateStudentGrades(int studentID, const std::vector<float>& newGrades);

// Letter grade calculation (A, B, C, D, F) based on average
char calculateLetterGrade(float average);

// GPA calculation based on letter grade (A=4, B=3, C=2, D=1, F=0)
int calculateGPA(char letterGrade);

// Pass/fail rule based on 50 threshold
bool isPass(float average);


#endif // GRADE_LOGIC_H_INCLUDED
