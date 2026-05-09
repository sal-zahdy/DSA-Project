#ifndef STUDENTMANAGER_H_INCLUDED
#define STUDENTMANAGER_H_INCLUDED

#include <vector>
#include <string>
#include "student.h"

using namespace std;


struct Student {
    int id;
    string name;
    vector<float> grades;

    Student(int id, string name, vector<float> grades)
        : id(id), name(name), grades(grades) {}
};

// Global database (defined in StudentManager.cpp)
extern vector<Student> studentDB;

// Function declarations
int findStudentIndex(int id);

void addStudent(int id, string name, vector<float> grades);

void removeStudent(int id);

void displayAllStudents();

vector<Student>& getStudentDB();

void searchStudentByName(string targetName);

#endif // STUDENTMANAGER_H_INCLUDED
