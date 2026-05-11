#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <stack>
#include <queue>
#include <algorithm>
#include "reports.h"
#include "grade_logic.h"

using namespace std;

// Returns reference to the global student database
vector<Student>& getStudentDB()
{
    return studentDB;
}

// Calculates the arithmetic mean of a grade vector
float calculateAverage(const vector<float>& grades)
{
    if (grades.empty()) return 0.0f;
    float sum = 0;
    for (float g : grades) sum += g;
    return sum / grades.size();
}

// Returns database copy sorted by ID
vector<Student> sortStudentsByID(const vector<Student>& students)
{
    vector<Student> sorted = students;
    sort(sorted.begin(), sorted.end(), [](const Student& a, const Student& b)
    {
        return a.id < b.id;
    });
    return sorted;
}

// Returns database copy sorted by Name
vector<Student> sortStudentsByName(const vector<Student>& students)
{
    vector<Student> sorted = students;
    sort(sorted.begin(), sorted.end(), [](const Student& a, const Student& b)
    {
        return a.name < b.name;
    });
    return sorted;
}

// Returns database copy sorted by Average descending
vector<Student> sortStudentsByAverage(const vector<Student>& students)
{
    vector<Student> sorted = students;
    sort(sorted.begin(), sorted.end(), [](const Student& a, const Student& b)
    {
        return calculateAverage(a.grades) > calculateAverage(b.grades);
    });
    return sorted;
}

// Binary search for a student ID
int binarySearchByID(const vector<Student>& sortedList, int targetID)
{
    int left = 0, right = sortedList.size() - 1;
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        if (sortedList[mid].id == targetID) return mid;
        if (sortedList[mid].id < targetID) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

// Binary search for a student Name
int binarySearchByName(const vector<Student>& sortedList, string targetName)
{
    int left = 0, right = sortedList.size() - 1;
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        if (sortedList[mid].name == targetName) return mid;
        if (sortedList[mid].name < targetName) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

// Displays all students in a formatted table
void displayAllStudentsTable()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }
    cout << "\n========== Student Report ==========\n";
    cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Avg" << setw(8) << "Grade" << setw(5) << "GPA" << "\n";
    cout << "----------------------------------------------------\n";
    for (const auto& s : getStudentDB())
    {
        float avg = calculateAverage(s.grades);
        char letter = calculateLetterGrade(avg);
        cout << left << setw(8) << s.id << setw(20) << s.name << setw(10) << fixed << setprecision(2) << avg
             << setw(8) << letter << setw(5) << calculateGPA(letter) << "\n";
    }
}

// Displays a student found by ID
void displayStudentByID(int targetID)
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }
    vector<Student> sorted = sortStudentsByID(getStudentDB());
    int index = binarySearchByID(sorted, targetID);
    if (index == -1)
    {
        cout << "\nStudent ID " << targetID << " not found.\n";
        return;
    }
    float avg = calculateAverage(sorted[index].grades);
    char letter = calculateLetterGrade(avg);
    cout << "\n========== Search Result ==========\n";
    cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Avg" << setw(8) << "Grade" << setw(5) << "GPA" << "\n";
    cout << "----------------------------------------------------\n";
    cout << left << setw(8) << sorted[index].id << setw(20) << sorted[index].name << setw(10) << fixed << setprecision(2) << avg
         << setw(8) << letter << setw(5) << calculateGPA(letter) << "\n";
}

// Displays a student found by Name
void displayStudentByName(string targetName)
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }
    vector<Student> sorted = sortStudentsByName(getStudentDB());
    int index = binarySearchByName(sorted, targetName);
    if (index == -1)
    {
        cout << "\nStudent \"" << targetName << "\" not found.\n";
        return;
    }
    float avg = calculateAverage(sorted[index].grades);
    char letter = calculateLetterGrade(avg);
    cout << "\n========== Search Result ==========\n";
    cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Avg" << setw(8) << "Grade" << setw(5) << "GPA" << "\n";
    cout << "----------------------------------------------------\n";
    cout << left << setw(8) << sorted[index].id << setw(20) << sorted[index].name << setw(10) << fixed << setprecision(2) << avg
         << setw(8) << letter << setw(5) << calculateGPA(letter) << "\n";
}

// Displays students ranked by performance
void displayRankedStudents()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }
    vector<Student> sorted = sortStudentsByAverage(getStudentDB());
    cout << "\n========== Performance Ranking ==========\n";
    cout << left << setw(6) << "Rank" << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Avg" << "\n";
    cout << "-------------------------------------------------\n";
    for (int i = 0; i < (int)sorted.size(); i++)
    {
        cout << left << setw(6) << (i + 1) << setw(8) << sorted[i].id << setw(20) << sorted[i].name
             << setw(10) << fixed << setprecision(2) << calculateAverage(sorted[i].grades) << "\n";
    }
}

// Displays the student with the highest average
void displayTopStudent()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }
    vector<Student> sorted = sortStudentsByAverage(getStudentDB());
    float avg = calculateAverage(sorted[0].grades);
    char letter = calculateLetterGrade(avg);
    cout << "\n========== Top Performer ==========\n";
    cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Avg" << setw(8) << "Grade" << "\n";
    cout << "-------------------------------------------------\n";
    cout << left << setw(8) << sorted[0].id << setw(20) << sorted[0].name << setw(10) << fixed << setprecision(2) << avg << setw(8) << letter << "\n";
}

// Displays the student with the lowest average
void displayLowestStudent()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }
    vector<Student> sorted = sortStudentsByAverage(getStudentDB());
    int last = sorted.size() - 1;
    float avg = calculateAverage(sorted[last].grades);
    char letter = calculateLetterGrade(avg);
    cout << "\n========== Lowest Performer ==========\n";
    cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Avg" << setw(8) << "Grade" << "\n";
    cout << "-------------------------------------------------\n";
    cout << left << setw(8) << sorted[last].id << setw(20) << sorted[last].name << setw(10) << fixed << setprecision(2) << avg << setw(8) << letter << "\n";
}

// Displays the total class average
void displayClassAverage()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }
    float totalSum = 0;
    for (const auto& s : getStudentDB()) totalSum += calculateAverage(s.grades);
    cout << "\nClass Average: " << fixed << setprecision(2) << (totalSum / getStudentDB().size()) << "\n";
}

// Displays pass/fail statistics
void displayPassFailSummary()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }
    int passCount = 0, failCount = 0;
    for (const auto& s : getStudentDB())
    {
        if (isPass(calculateAverage(s.grades))) passCount++;
        else failCount++;
    }
    cout << "\n========== Pass/Fail Summary ==========\n";
    cout << left << setw(20) << "Status" << setw(10) << "Count" << "\n";
    cout << "------------------------------------\n";
    cout << left << setw(20) << "Passing" << setw(10) << passCount << "\n";
    cout << left << setw(20) << "Failing " << setw(10) << failCount << "\n";
    cout << left << setw(20) << "Total " << setw(10) << getStudentDB().size() << "\n";
}
