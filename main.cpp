/*  Lab 19: Program auto creates movie object and auto populates reviews
    IDE: Vscode

    Design Decisions:
    1. We already have all the structure in place from Lab 18 with reviews as a subclass for each movie object, and a method to delete
        all reviews in the Movie object destructor.
    2. I think we'll use a do while loop to continuously add movies, and during each iteration, we'll use rand() for the number of
        fake movie reviews to the movie, along with rand() to assign the rating.
    3. Planning to keep it simple with a vector to hold the movie elements and delete the vector at the end of the program.
*/

#include <iostream>
#include <iomanip> // setprecision()
#include <fstream> // file io
#include <string>
#include <sstream> // stringstream for Movie.getReview()
#include <exception>
#include <vector>
using namespace std;

// Movie object file declaration to hold info on movie and list of reviews
class Movie
{
private:
    string name;
    int yearReleased;
    // Linked list struct to hold Movie reviews
    struct review
    {
        double rating;
        string comment;
        review *next;
    };
    review *head;
    review *tail;
    int reviewCount;

public:
    Movie();
    Movie(string, int);
    void setName(string);
    string getName();
    void setYearReleased(int);
    int getYearReleased();
    void addReview(double, string, int);
    string getReview(int);   // Returns a review at given index
    void deleteReview(int);  // Deletes Movie review at given index
    void deleteAllReviews(); // Loops through all Movie reviews and deletes
    int getNumReviews();
    double getReviewRatingAve(); // Returns average rating for movie
    bool isReview(int);          // Checks if review at given index is valid
    string reviewsToString();    // Returns string containing given number of reviews, 0 for all
    ~Movie();                    // Deletes object, as well as all associated Movie reviews
};

// Returns a random double
double GetRandomDouble(double, double);

int main()
{
    string userInput = "";
    string movieName = "";
    int movieYearReleased = 0;
    int headOrTail = 0; // Will insert reviews at head
    int numOfReviews = 0;
    // double reviewRating = -1;
    // string reviewComment = "";
    Movie *moviePtr = nullptr;
    vector<Movie *> movieList = vector<Movie *>();
    const string INPUT_FILE_NAME = "movieReviews.txt";
    ifstream inputFile;
    string fileLine = "";
    bool endOfFile = false;

    // Phew, now that the object structure is in place, let's start working on the console ui
    // Get user input for program, either 0 or 1 for how to add new reviews to linked list
    cout << "Linked list will insert review nodes at head." << endl;

    // Try opening file and output error to console if file is no good
    inputFile.open(INPUT_FILE_NAME);
    if (!inputFile) // Output error if problem opening file
    {
        std::cerr << "Error opening file! Better luck next time..." << endl;
        return 1;
    }

    for (size_t i = 0; i < 5 && !endOfFile; i++)
    {
        // Auto populate Movie name and year released.
        moviePtr = new Movie(string("Movie #" + to_string(i + 1)), 1995 + i);

        // Add moviePtr pointer and add object to list of movies
        movieList.push_back(moviePtr);

        // Use rand() to add random number of reviews < 7, with random ratings to movie
        numOfReviews = rand() % 3 + 4;
        for (size_t i = 0; i < numOfReviews; i++)
        {
            // Check if input file has data to read
            if (getline(inputFile, fileLine))
            {
                // Add random rating and review from file
                moviePtr->addReview(GetRandomDouble(1.0, 5.0), fileLine, headOrTail);
            }
            else
            {
                // Break and flag if no more reviews in file to read from
                endOfFile = true;
                break;
            }
        }
    }

    // Movie addition done, nullify pointers
    moviePtr = nullptr;

    // Ouput all reviews to console
    for (size_t i = 0; i < movieList.size(); i++)
    {
        cout << "All movie reviews for movie: " << movieList.at(i)->getName() << " (" << movieList.at(i)->getYearReleased() << "):" << endl;
        cout << movieList.at(i)->reviewsToString() << endl;
        cout << "\t> Average rating: " << fixed << setprecision(1) << movieList.at(i)->getReviewRatingAve() << endl;
    }

    return 0;
}

// Movie CLASS DEFINITIONS
Movie::Movie()
    : reviewCount(0), head(nullptr), tail(nullptr)
{
}

Movie::Movie(string n, int yrReleased)
    : reviewCount(0), head(nullptr), tail(nullptr)
{
    setName(n);
    setYearReleased(yrReleased);
}
void Movie::setName(string n)
{
    name = n;
}
string Movie::getName()
{
    return name;
}
void Movie::setYearReleased(int yr)
{
    if (yr < 1887)
        throw invalid_argument("Must be a valid year >= 1888 (the first movie).");
    yearReleased = yr;
}
int Movie::getYearReleased()
{
    return yearReleased;
}
void Movie::addReview(double movRating, string movComment, int headOrTail)
{
    // Create new object to add to reviews
    review *r = new review;
    r->rating = movRating;
    r->comment = movComment;
    // If head and tail are null, initialize list
    if (!head && !tail)
    {
        head = tail = r;
        tail->next = nullptr;
    }
    else if (headOrTail == 0) // Place new node at head
    {
        r->next = head;
        head = r;
    }
    else if (headOrTail == 1) // Place new node at tail
    {
        tail->next = r;
        tail = r;
        tail->next = nullptr;
    }
    else
    {
        // Return without performing any operation if no condition matches
        delete r;
        return;
    }
    reviewCount++;
}
// Returns a review at given index
string Movie::getReview(int index)
{
    // Check to make sure index in in range
    if (!isReview(index))
        throw invalid_argument("Review not found at index: " + index);

    // Set current pointer equal to review object
    stringstream output;
    review *current = head;
    for (size_t i = 0; i < index; i++)
    {
        current = current->next;
    }
    output << fixed << setprecision(1) << current->rating << ": " << current->comment;

    return output.str();
}
// Deletes Movie review at given index
void Movie::deleteReview(int index)
{
    // Make sure index is valid
    if (!isReview(index))
        throw invalid_argument("No reviews exist for this movie.");

    // Now that index is valid find review, reroute next pointer, and delete
    // Set current pointer = to review to be deleted
    review *current = head;
    review *prev = head;
    for (size_t i = 0; i < index; i++)
    {
        if (i == 0)
        {
            current = current->next;
        }
        else
        {
            current = current->next;
            prev = prev->next;
        }
    }
    // If current is head, point head to next element and delete current
    // If current is tail, point tail to prev element and delete current
    // Else, point prev = current->next, and delete current
    if (current == head && current == tail)
    {
        head = tail = nullptr;
    }
    else if (current == head)
    {
        head = head->next;
    }
    else if (current == tail)
    {
        tail = prev;
        tail->next = nullptr;
    }
    else
    {
        prev->next = current->next;
    }
    delete current;
}
// Loops through all Movie reviews and deletes
void Movie::deleteAllReviews()
{
    // Set current = head and continue looping and deleting until current == nullptr
    review *current = head;
    while (current)
    {
        head = head->next;
        delete current;
        current = head;
        reviewCount--;
    }
    // Reset head/tail pointers
    head = tail = nullptr;
}
int Movie::getNumReviews()
{
    return reviewCount;
}

// Returns average of movie ratings
double Movie::getReviewRatingAve()
{
    // Set current = head and continue looping and deleting until current == nullptr
    double sum = 0.0;
    int count = 0;
    review *current = head;
    while (current)
    {
        sum += current->rating;
        count++;
        current = current->next;
    }
    return sum / count;
}

// Checks if review at given index is valid
bool Movie::isReview(int index)
{
    if (index >= 0 && reviewCount > 0 && index < reviewCount)
        return true;
    return false;
}

// Returns string containing given number of reviews, 0 for all
string Movie::reviewsToString()
{
    // Loop through all reviews and return string
    string output = "";
    for (size_t i = 0; i < reviewCount; i++)
    {
        if (i > 0)
            output += "\n";
        output += "\t> " + getReview(i);
    }
    return output;
}

// Make sure to clean up all movie reviews from heap before object destruction
Movie::~Movie()
{
    deleteAllReviews();
}

// Returns a random double
double GetRandomDouble(double min, double max)
{
    return min + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (max - min)));
}