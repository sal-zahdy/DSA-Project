#include <iostream>
#include <vector>
#include <string>
#include <iomanip>   // for setw() and setprecision() — used to align table columns
#include <stack>     // for std::stack — used in report history (Chapter 3)
#include <queue>     // for std::queue — used in report request queue (Chapter 4)
#include "student.h"
#include "reports.h"

using namespace std;

// Forward declaration — tells the compiler that getStudentDB() is defined in StudentManager.cpp
vector<Student>& getStudentDB();
// Access studentDB through the getter defined in StudentManager.cpp
// This avoids exposing the raw global variable across files
//vector<Student>& studentDB = getStudentDB();



// -------------------------------------------------------
// HELPER FUNCTION: calculateAverage()
// Used internally by all report functions
// Takes a vector of grades and returns the average as a float
// If no grades exist, returns 0 to avoid dividing by zero
// Algorithm: single linear pass — O(n) where n = number of grades
// -------------------------------------------------------
float calculateAverage(const vector<float>& grades)
{
    if (grades.empty())
        return 0.0f;

    float sum = 0;

    for (float g : grades)   // add up all grades
        sum += g;

    return sum / grades.size();   // divide by count to get average
}


// -------------------------------------------------------
// CHAPTER 2 — VECTOR (dynamic array)
// displayAllStudentsTable(): prints every student in a formatted table
// Loops through studentDB once — O(n)
// Never modifies studentDB
// -------------------------------------------------------
void displayAllStudentsTable()
{
    cout << "\n========== Student Report ==========\n";

    // Print column headers using setw() to fix each column to a set width
    cout << left
     << setw(6)  << "ID"
     << setw(20) << "Name"
     << setw(12) << "Average"
     //<< setw(8)  << "Grade"
     << "\n";

    cout << "-------------------------------------\n";

    // Loop through every student in the database
    for (const auto& s : getStudentDB())
    {
        float avg = calculateAverage(s.grades);   // calculate this student's average

        cout << left
             << setw(6)  << s.id
             << setw(20) << s.name
             << setw(15) << fixed << setprecision(2) << avg   // 2 decimal places
             << "\n";
    }

    cout << "=====================================\n";

}


// -------------------------------------------------------
// CHAPTER 2 — VECTOR
// displayClassAverage(): calculates the average grade across all students
// Sums each student's average then divides by number of students
// O(n * g) where n = students, g = grades per student
// -------------------------------------------------------
void displayClassAverage()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }

    float totalSum = 0;

    for (const auto& s : getStudentDB())
        totalSum += calculateAverage(s.grades);   // add each student's average to total

    float classAvg = totalSum / getStudentDB().size();   // divide by number of students

    cout << "\nClass Average: " << fixed << setprecision(2) << classAvg << "\n";

}


// -------------------------------------------------------
// CHAPTER 2 — VECTOR
// displayTopStudent(): finds the student with the highest average
// Linear scan keeping track of the best seen so far
// O(n) time — visits every student exactly once
// -------------------------------------------------------
void displayTopStudent()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }

    // Start by assuming the first student is the top student
    int topIndex = 0;
    float topAvg = calculateAverage(getStudentDB()[0].grades);

    for (int i = 1; i < (int)getStudentDB().size(); i++)
    {
        // Early exit: if we already found a perfect score, no need to continue
        if (topAvg == 100.0f) break;

        float avg = calculateAverage(getStudentDB()[i].grades);

        if (avg > topAvg)       // if this student has a higher average
        {
            topAvg = avg;       // update the highest average seen so far
            topIndex = i;       // remember this student's position
        }
    }

    cout << "\nTop Student:\n";
    cout << "  ID      : " << getStudentDB()[topIndex].id << "\n";
    cout << "  Name    : " << getStudentDB()[topIndex].name << "\n";
    cout << "  Average : " << fixed << setprecision(2) << topAvg << "\n";

}


// -------------------------------------------------------
// CHAPTER 2 — VECTOR
// displayLowestStudent(): finds the student with the lowest average
// Same logic as displayTopStudent() but looks for the minimum
// O(n) time
// -------------------------------------------------------
void displayLowestStudent()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }

    int lowIndex = 0;
    float lowAvg = calculateAverage(getStudentDB()[0].grades);

    for (int i = 1; i <(int)getStudentDB().size(); i++)
    {
        float avg = calculateAverage(getStudentDB()[i].grades);

        if (avg < lowAvg)       // if this student has a lower average
        {
            lowAvg = avg;
            lowIndex = i;
        }
    }

    cout << "\nLowest Student:\n";
    cout << "  ID      : " << getStudentDB()[lowIndex].id << "\n";
    cout << "  Name    : " << getStudentDB()[lowIndex].name << "\n";
    cout << "  Average : " << fixed << setprecision(2) << lowAvg << "\n";

}


// -------------------------------------------------------
// CHAPTER 2 — VECTOR
// displayPassFailSummary(): counts how many students passed and failed
// Pass threshold is 50 — coordinate this value with Member 3
// O(n) time — single pass through the database
// -------------------------------------------------------
void displayPassFailSummary()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }

    const float PASS_THRESHOLD = 50.0f;   // change this to match Member 3's threshold

    int passCount = 0;
    int failCount = 0;

    for (const auto& s : getStudentDB())
    {
        float avg = calculateAverage(s.grades);

        if (avg >= PASS_THRESHOLD)
            passCount++;   // student passed
        else
            failCount++;   // student failed
    }

    cout << "\nPass/Fail Summary:\n";
    cout << "  Passed : " << passCount << " student(s)\n";
    cout << "  Failed : " << failCount << " student(s)\n";
    cout << "  Total  : " << getStudentDB().size() << " student(s)\n";

}


// -------------------------------------------------------
// CHAPTER 9 — SELECTION SORT
// CHAPTER 2 — VECTOR (local copy)
// displayRankedStudents(): sorts a local copy of the database by average grade
// Uses selection sort (descending) to rank students highest to lowest
// The original studentDB is never modified
// O(n^2) time for selection sort — acceptable for small class sizes
// -------------------------------------------------------
void displayRankedStudents()
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }

    // Copy the database into a local vector — we sort this, not the original
    // Data structure: vector<Student> (Chapter 2)
    vector<Student> sorted = getStudentDB();

    // SELECTION SORT (descending by average)
    // Outer loop: we are deciding what goes in position i
    for (int i = 0; i < (int)sorted.size() - 1; i++)
    {
        int maxIndex = i;   // assume position i already has the highest average

        // Inner loop: scan everything after position i to find a higher average
        for (int j = i + 1; j < (int)sorted.size(); j++)
        {
            if (calculateAverage(sorted[j].grades) > calculateAverage(sorted[maxIndex].grades))
            {
                maxIndex = j;   // found a higher average, remember its index
            }
        }

        // Swap the student at maxIndex with the student at position i
        if (maxIndex != i)
        {
            Student temp  = sorted[i];
            sorted[i]     = sorted[maxIndex];
            sorted[maxIndex] = temp;
        }
    }

    // Print the ranked table
    cout << "\n========== Students Ranked by Average ==========\n";
    cout << left
         << setw(6)  << "Rank"
         << setw(6)  << "ID"
         << setw(20) << "Name"
         << setw(10) << "Average"
         << "\n";
    cout << "-------------------------------------------------\n";

    for (int i = 0; i < (int)sorted.size(); i++)
    {
        float avg = calculateAverage(sorted[i].grades);

        cout << left
             << setw(6)  << (i + 1)          // rank starts from 1
             << setw(6)  << sorted[i].id
             << setw(20) << sorted[i].name
             << setw(10) << fixed << setprecision(2) << avg
             << "\n";
    }

}


// -------------------------------------------------------
// CHAPTER 8 — BINARY SEARCH
// CHAPTER 9 — SELECTION SORT (to prepare the sorted copy)
// CHAPTER 2 — VECTOR (local copy)
//
// displayStudentByID(): searches for a student by ID using binary search
// Binary search requires a sorted list, so we sort a local copy first
// Sorting: O(n^2) using selection sort
// Searching: O(log n) using binary search
// The original studentDB is never modified
// -------------------------------------------------------

// Helper: binary search on a sorted-by-ID vector
// Returns the index of the student if found, or -1 if not found
int binarySearchByID(const vector<Student>& sortedList, int targetID)
{
    int left  = 0;
    int right = sortedList.size() - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;   // calculate middle index safely

        if (sortedList[mid].id == targetID)
            return mid;                         // found — return the index

        else if (sortedList[mid].id < targetID)
            left = mid + 1;                     // target is in the right half

        else
            right = mid - 1;                    // target is in the left half
    }

    return -1;   // not found
}

void displayStudentByID(int targetID)
{
    if (getStudentDB().empty())
    {
        cout << "\nNo students in the system.\n";
        return;
    }

    // Copy the database — we will sort this copy by ID, never the original
    vector<Student> sorted = getStudentDB();

    // Sort by ID ascending using selection sort (Chapter 9)
    // Binary search requires sorted data — this is why we sort first
    for (int i = 0; i < (int)sorted.size() - 1; i++)
    {
        int minIndex = i;

        for (int j = i + 1; j < (int)sorted.size(); j++)
        {
            if (sorted[j].id < sorted[minIndex].id)
                minIndex = j;   // found a smaller ID
        }

        if (minIndex != i)
        {
            Student temp     = sorted[i];
            sorted[i]        = sorted[minIndex];
            sorted[minIndex] = temp;
        }
    }

    // Now apply binary search on the sorted copy (Chapter 8)
    int index = binarySearchByID(sorted, targetID);

    if (index == -1)
    {
        cout << "\nStudent with ID " << targetID << " not found.\n";
        return;
    }

    float avg = calculateAverage(sorted[index].grades);

    cout << "\nStudent Found:\n";
    cout << "  ID      : " << sorted[index].id << "\n";
    cout << "  Name    : " << sorted[index].name << "\n";
    cout << "  Average : " << fixed << setprecision(2) << avg << "\n";

}
