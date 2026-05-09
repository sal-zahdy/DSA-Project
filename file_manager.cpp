#include <iostream>
#include <fstream>
#include <sstream>

#include "StudentManager.h"
#include "file_manager.h"

using namespace std;

// --------------------------------------------------
// Save all students to file
// Format:
// id,name,grade1,grade2,grade3...
// --------------------------------------------------
void saveToFile()
{
    ofstream file("students.txt");

    if (!file)
    {
        cout << "Error opening file for saving.\n";
        return;
    }

    for (const auto& s : getStudentDB())
    {
        file << s.id << ",";
        file << s.name;

        for (float g : s.grades)
        {
            file << "," << g;
        }

        file << "\n";
    }

    file.close();

    cout << "Data saved successfully.\n";
}

// --------------------------------------------------
// Load students from file
// --------------------------------------------------
void loadFromFile()
{
    ifstream file("students.txt");

    if (!file)
    {
        cout << "No previous data file found.\n";
        return;
    }

    string line;

    while (getline(file, line))
    {
        stringstream ss(line);

        string token;

        getline(ss, token, ',');
        int id = stoi(token);

        getline(ss, token, ',');
        string name = token;

        vector<float> grades;

        while (getline(ss, token, ','))
        {
            grades.push_back(stof(token));
        }

        addStudent(id, name, grades);
    }

    file.close();

    cout << "Data loaded successfully.\n";
}
