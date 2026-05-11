#ifndef GRADE_LOGIC_H_INCLUDED
#define GRADE_LOGIC_H_INCLUDED

#include <vector>

void updateStudentGrades(int studentID, const std::vector<float>& newGrades);
char calculateLetterGrade(float average);
int calculateGPA(char letterGrade);
bool isPass(float average);


#endif // GRADE_LOGIC_H_INCLUDED
