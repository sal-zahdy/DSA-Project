#ifndef STUDENTMANAGER_H_INCLUDED
#define STUDENTMANAGER_H_INCLUDED

#include <vector>
#include <string>

using namespace std;


struct Student {
    int id;
    string name;
    vector<float> grades;

    Student(int id, string name, vector<float> grades)
        : id(id), name(name), grades(grades) {}
};

extern vector<Student> studentDB;

int findStudentIndex(int id);

void addStudent(int id, string name, vector<float> grades);

void removeStudent(int id);

void displayAllStudents();

vector<Student>& getStudentDB();

void searchStudentByName(string targetName);

#endif // STUDENTMANAGER_H_INCLUDED
