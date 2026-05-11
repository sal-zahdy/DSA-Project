#include <iostream>
#include <vector>
#include "StudentManager.h"
#include "file_manager.h"

using namespace std;

vector<Student> studentDB;

// Search Student By Name
void searchStudentByName(string targetName)
{
    bool found = false;

    for (const auto& s : studentDB)
    {
        if (s.name == targetName)
        {
            cout << "\nStudent Found:\n";
            cout << "ID: " << s.id << "\n";
            cout << "Name: " << s.name << "\n";

            cout << "Grades: ";

            for (float g : s.grades)
            {
                cout << g << " ";
            }

            cout << "\n";

            found = true;
        }
    }

    if (!found)
    {
        cout << "Student not found.\n";
    }
}

//find student by id
int findStudentIndex(int id)
{
    for (int i = 0; i < (int)studentDB.size(); i++)
    {
        if (studentDB[i].id == id)
        {
            return i;
        }
    }
    return -1; // not found
}

//adds student to database
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

//removes the student
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

//display all student in data base
void displayAllStudents()
{
    cout << "\n--- Student List ---\n";
    for (const auto& s : studentDB)
    {
        cout << "ID: " << s.id << ", Name: " << s.name << "\n";
    }
}

