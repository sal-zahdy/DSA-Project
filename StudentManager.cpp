#include <iostream>
#include <vector>
#include "student.h"

using namespace std;

vector<Student> studentDB;

int findStudentIndex(int id)
{
    for (int i = 0; i < studentDB.size(); i++)
    {
        if (studentDB[i].id == id)
        {
            return i;
        }
    }
    return -1; // not found
}

void addStudent(int id, string name, vector<float> grades)
{
    if (findStudentIndex(id) != -1)
    {
        cout << "Student with this ID already exists.\n";
        return;
    }

    if (id <= 0)
    {
        cout << "Invalid ID.\n";
        return;
    }

    if (name.empty())
    {
        cout << "Name cannot be empty.\n";
        return;
    }

    for (float g : grades)
    {
        if (g < 0 || g > 100)
        {
            cout << "Invalid grade detected.\n";
            return;
        }
    }

    Student newStudent(id, name, grades);
    studentDB.push_back(newStudent);

    cout << "Student added successfully.\n";
}

void removeStudent(int id)
{
    int index = findStudentIndex(id);

    if (index == -1)
    {
        cout << "Student not found.\n";
        return;
    }

    studentDB.erase(studentDB.begin() + index);

    cout << "Student removed successfully.\n";
}

void displayAllStudents() {
    cout << "\n--- Student List ---\n";
    for (const auto& s : studentDB) {
        cout << "ID: " << s.id << ", Name: " << s.name << "\n";
    }
}


//DOAA
// Getter function: provides read-only access to studentDB for other files
vector<Student>& getStudentDB()
{
    return studentDB;
}