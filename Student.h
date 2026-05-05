#ifndef STUDENT_H_INCLUDED
#define STUDENT_H_INCLUDED


#include <string>
#include <vector>

using namespace std;

struct Student {
    int id;
    string name;
    vector<float> grades;

    Student(int id, string name, vector<float> grades)
        : id(id), name(name), grades(grades) {}
};


#endif // STUDENT_H_INCLUDED
