#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>


using namespace std;
using namespace std::chrono;
const int INF = 1e9;

class Point {
   public:
    string name;
    double x, y;

    Point() : name("Generic"), x(-1), y(-1) {}
    Point(string name, double x, double y) : name(name), x(x), y(y) {}
    Point(double x, double y) : name("Generic"), x(x), y(y) {}
    
    //Custom implementation to print out objects of class "Point"
    friend ostream &operator<<(ostream &os, const Point &point) {
        os << "POI Name: " << point.name << ", POI Latitude: " << point.x
           << ", POI Longitude: " << point.y;
        return os;
    }

    // Overload < operator for comparison, compare based on x first, then y if x
    // is the same
    bool operator<(const Point &other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};
// Stores the center of polygon (Global because compare function uses it)
Point mid;

void readPointsFromFile(const string& fileName, vector<Point>& points) {
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        throw runtime_error("Unable to open file: " + fileName);
    }

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        string name;
        double x, y;
        if (!(iss >> name >> x >> y)) {
            throw runtime_error("Invalid line format: " + line);
        }
        points.push_back(Point(name, x, y));
    }
    inputFile.close();
}

// Given a set of POI's, allows "builder" to pick their budget and what points
// they want to buy/develop
void promptBuilder(const vector<Point> &allPoints, vector<Point> &selectedPoints, double &budget, double &landPricePerUnit, double &roadPricePerUnit, ofstream& outFile) {
    // Tracks what points have already been selected
    set<int> selectedIndices;

    while (true) {
        // Break if all points have been selected
        if (selectedPoints.size() == allPoints.size()) {
            cout << "No more POI's available to select.\n";
            break;
        }
        // Display points that have not been selected
        cout << "Available POI's:\n";
        for (int i = 0; i < allPoints.size(); ++i) {
            if (selectedIndices.find(i) == selectedIndices.end()) {
                cout << i + 1 << ": " << allPoints[i] << "\n";
            }
        }
        cout << "\nEnter the number of the POI you want to select (enter 0 to "
                "stop):\n";
        int choice;
        cin >> choice;
        if (choice == 0) break;
        // Check if the choice is valid
        if (choice < 1 || choice > allPoints.size() ||
            selectedIndices.find(choice - 1) != selectedIndices.end()) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        selectedPoints.push_back(allPoints[choice - 1]);
        selectedIndices.insert(choice - 1);
    }

    cout << "\nPlease enter your budget for development: ";
    cin >> budget;
    while (budget <= 0) {
        cout << "Invalid budget. Please enter a positive number: ";
        cin >> budget;
    }

    cout << "\nPlease enter price of land: ";
    cin >> landPricePerUnit;
    while (landPricePerUnit <= 0) {
        cout << "Invalid price. Please enter a positive number: ";
        cin >> budget;
    }

    cout << "\nPlease enter price of building road: ";
    cin >> roadPricePerUnit;
    while (landPricePerUnit <= 0) {
        cout << "Invalid price. Please enter a positive number: ";
        cin >> budget;
    }

    outFile << "\nYou selected the following POI's:\n";
    if (selectedPoints.empty()) {
        outFile << "No POI's were selected.\n";
    } else {
        for (const Point &point : selectedPoints) {
            outFile << point << "\n";
        }
    }
    outFile << "Your Selected budget is $" << budget << endl;
    outFile << "The price of land per unit is $" << landPricePerUnit << endl;
}

// Euclidean distance
int distance(Point a, Point b) {
    return round(sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y)));
}

void floyds(vector<vector<int>>& dist) {
    int V = dist.size();
    for (int k = 0; k < V; ++k) {
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < V; ++j) {
                if (dist[i][k] < INF && dist[k][j] < INF) {
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }
}

// Given a set of points, generate a graph from those points
vector<vector<int>> generateGraph(const vector<Point> &points) {
    int n = points.size();
    vector<vector<int>> graph(n, vector<int>(n, INF));
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int dist = distance(points[i], points[j]);
            graph[i][j] = graph[j][i] = dist;
        }
    }
    floyds(graph);
    return graph;
}


// TODO: Given a graph with verticies represent POI, calculate the "road cost" of connecting these points
double sumGraph(const vector<vector<int>>& dist) { 
    double totalDistance = 0;
    int V = dist.size();
    for (int i = 0; i < V; ++i) {
        for (int j = i+1; j < V; ++j) {
            if (dist[i][j] == INF)
                continue;
            else{
                totalDistance += dist[i][j];
            }
                
        }
    }
    return totalDistance;
    
}

// Checks whether the line is crossing the polygon
int orientation(Point p1, Point p2, Point p3) {
    int res = (p2.y - p1.y) * (p3.x - p2.x) - (p3.y - p2.y) * (p2.x - p1.x);
    if (res == 0) return 0;
    if (res > 0) return 1;
    return -1;
}

vector<Point> mergeHulls(vector<Point> leftHull, vector<Point> rightHull) {
    int n1 = leftHull.size();
    int n2 = rightHull.size();
    int iL = 0;
    int iR = 0;

    //Find rightmost point of leftHull, store in iL
    for (int i = 1; i < n1; i++)
        if (leftHull[i].x > leftHull[iL].x) iL = i;
    // Find leftmost point of rightHull, store in iR
    for (int i = 1; i < n2; i++)
        if (rightHull[i].x < rightHull[iR].x) iR = i;


    int indL = iL;
    int indR = iR;
    bool done = false;
    /*
    Given two starting points a (indL) and b (indr), where a is the rightmost left hull point and b is the leftmost right hull point,
    The goal is to find points a and b such that AB does not cross through the left hull or the right hull
    This would represent the upper tangent line
    */
    while (!done) {
        done = true; 
        //This while loop determines the point A (indL) that ensures AB does nt cross through the left hull
        //Moding by n1 (# points in left hull) ensures that the point incrementation does not go out of bounds of left hull
        while (orientation(rightHull[indR], leftHull[indL], leftHull[(indL + 1) % n1]) >= 0){ 
            indL = (indL + 1) % n1; //counter clockwise turn
        }
        //This while loop determines the point B (indR) that ensures AB does not cross through the right hull
        //Modding by n2 (# points in right hull) ensures that the point incrementation does not go out of bounds of right hull
        while (orientation(leftHull[indL], rightHull[indR], rightHull[(n2 + indR - 1) % n2]) <= 0) {
            indR = (n2 + indR - 1) % n2; //clockwise turn
            done = false;
        }
    }

    // Find the lower tangent, logic is the same as upper tangent calculations except it does it in reverse order
    int upperL = indL;
    int upperR = indR;
    indL = iL;
    indR = iR;
    done = false;
    while (!done){
        done = true;
        while (orientation(leftHull[indL], rightHull[indR], rightHull[(indR + 1) % n2]) >= 0){
            indR = (indR + 1) % n2; //counter clockwise turn
        }
        while (orientation(rightHull[indR], leftHull[indL], leftHull[(n1 + indL - 1) % n1]) <= 0) {
            indL = (n1 + indL - 1) % n1; //clockwise turn
            done = false;
        }
    }

    //Store the merged Convex Hulls in ret in counter-clockwise order
    int lowerL = indL;
    int lowerR = indR;
    vector<Point> ret; 
    //Traverse leftHull from upper tangent to lower tangent and add these points to ret
    int ind = upperL;
    ret.push_back(leftHull[upperL]);
    while (ind != lowerL) {
        ind = (ind + 1) % n1;
        ret.push_back(leftHull[ind]);
    }
    //Traverse rightHull from lower tangent to upper tangent and add these points to ret
    ind = lowerR;
    ret.push_back(rightHull[lowerR]);
    while (ind != upperR) {
        ind = (ind + 1) % n2;
        ret.push_back(rightHull[ind]);
    }
    return ret;
}

// Brute force algorithm to find Convex Hull for < 6 points
vector<Point> getHullBrute(vector<Point> points) {
    //Set of boundary points of the Convex Hull
    set<Point> boundaryPoints;

    //Loop through all set of points
    for (int i = 0; i < points.size(); i++) {
        for (int j = i + 1; j < points.size(); j++) {
            int x1 = points[i].x, x2 = points[j].x;
            int y1 = points[i].y, y2 = points[j].y;

            /*
            Line ax + by = c between 2 points (x1, y1) and (x2, y2)
            where a = y2-y1, b=x1-x2, c=x1y2-y1x2
            All points on one side of the line: ax + by > c
            All points on the other side: ax + by < c
            */
            int a1 = y2 - y1;
            int b1 = x1 - x2;
            int c = x1 * y2 - y1 * x2;
            int pointsAboveLine = 0, pointsBelowLine = 0;

            //See what side of the line any of the points are on
            for (int k = 0; k < points.size(); k++) {
                if (a1 * points[k].x + b1 * points[k].y >= c) pointsBelowLine++;
                if (a1 * points[k].x + b1 * points[k].y <= c) pointsAboveLine++;
            }

            //If all the points are either on one side of the line or the other,
            //Then the two points being examined are part of the hull's boundary
            if (pointsBelowLine == points.size() || pointsAboveLine == points.size()) {
                boundaryPoints.insert(points[i]);
                boundaryPoints.insert(points[j]);
            }
        }
    }
    vector<Point> ret;
    for (Point p : boundaryPoints){
        ret.push_back(p);
    }

    // Calculate the midpoint
    mid = {0, 0};
    for (const Point& p : ret) {
        mid.x += p.x;
        mid.y += p.y;
    }
    mid.x /= ret.size();
    mid.y /= ret.size();
    // Sort in counterclockwise order
    sort(ret.begin(), ret.end(), [&](const Point& p1, const Point& q1) {
        Point p = {p1.x - mid.x, p1.y - mid.y};
        Point q = {q1.x - mid.x, q1.y - mid.y};
        return (p.y * q.x < q.y * p.x);
    });
    return ret;
}

// Given set of points, construct a Convex Hull of those points
vector<Point> getHullDNC(vector<Point> points) {
    //Base Case: If # points is < 6, use Brute Force algorithm to get Convex Hull
    if (points.size() <= 5){
        return getHullBrute(points);
    }

    vector<Point> left, right;
    //Add left half of points to "left"
    for (int i = 0; i < points.size() / 2; i++){
        left.push_back(points[i]);
    }

    //Add right half of points to "right"
    for (int i = points.size() / 2; i < points.size(); i++){
        right.push_back(points[i]);
    }

    // Recursively get Convex Hulls for left and right sets
    vector<Point> leftHull = getHullDNC(left);
    vector<Point> rightHull = getHullDNC(right);
    // Merge the two Convex Hulls
    return mergeHulls(leftHull, rightHull);
}

// Given a Convex Hull, find the toal area
double calculateConvexHullArea(const vector<Point> &hull) {
    double area = 0;
    for (size_t i = 0; i < hull.size(); ++i) {
        Point p1 = hull[i];
        Point p2 = hull[(i + 1) % hull.size()];
        area += (p1.x * p2.y - p2.x * p1.y);
    }
    return abs(area) / 2.0;
}

// Given a chosen set of POI by the builder, the builder's budget, a land
// price, and a road price, gets the most optimal set of POI that a builder can buy. Optimal set =
// maxamize number of POI without exceeding budget, where the price includes
// cost of buying the land, and building roads to connect the POI
void findOptimalDevelopmentPlan(vector<Point> &points, int budget, double landPricePerUnit, double roadPricePerUnit, ofstream& outFile) {
    vector<Point> bestHull;
    double bestLandCost = 0;
    double bestRoadCost = 0;
    double bestCost = 0;
    int n = points.size();

    auto start = chrono::high_resolution_clock::now();
    //Iterate over all subsets of points
    for (int mask = 1; mask < (1 << n); ++mask) {
        // 1. Create current subset of points
        vector<Point> currentSubset;
        for (int i = 0; i < n; ++i) {
            if (mask & (1 << i)) currentSubset.push_back(points[i]);
        }
        // 2. Get Convex Hull of current subset
        vector<Point> currentHull = getHullDNC(currentSubset);
        // 3. Calculate area of current Convex Hull
        int currentLandArea = calculateConvexHullArea(currentHull);
        // 4. Calculate land cost of area of current Convex Hull
        int currentLandCost = currentLandArea * landPricePerUnit;
        // 5. If cost of current Convex Hull exceeds budget, do not update
        // bestHull with currentHull, else continue to calculating cost of roads
        if (currentLandCost > budget) continue;
        // 6. Create a graph to represent the current subset of points
        vector<vector<int>> currentGraph = generateGraph(currentSubset);
        // 7. Calculate cost of building roads to connects all points
        double currentRoadCost = sumGraph(currentGraph) * roadPricePerUnit;
        // 8. Calculate cost of current Convex Hull (Cost of land) + current road
        //    cost (Cost of building roads connecting POI)
        double currentCost = currentLandCost + currentRoadCost;
        // 9. If currentCost is <= budget, we know we can afford to buy the land for 
        //    the current subset of POI and build roads connecting all the POI, so we 
        //    update our bestHull to currentHull, and our bestCost to currentCost, then 
        //    move on to next subset of POI
        if (currentCost <= budget && currentCost > bestCost) {
            bestHull = currentHull;
            bestLandCost = currentLandCost;
            bestRoadCost = currentRoadCost;
            bestCost = currentCost;
        }
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    // 10. Print the optimal solution
    outFile << "POI in Optimal Solution:\n";
    for (const Point &p : bestHull) {
        outFile << p << endl;
    }
    outFile << "\nLand Cost: $" << bestLandCost;
    outFile << "\nRoadway Cost: $" << bestRoadCost;
    outFile << "\nTotal Cost: $" << bestCost << endl;
    outFile << "Optimal Development Problem Runtime: " << duration.count() << " microseconds" << endl;
    outFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];
    ofstream outFile(outputFile);

    double landPricePerUnit;
    double roadPricePerUnit;
    double budget;
    vector<Point> givenPoints;
    vector<Point> selectedPoints;
    readPointsFromFile(inputFile, givenPoints);
    promptBuilder(givenPoints, selectedPoints, budget, landPricePerUnit, roadPricePerUnit, outFile);
    sort(selectedPoints.begin(), selectedPoints.end()); 
    findOptimalDevelopmentPlan(selectedPoints, budget, landPricePerUnit, roadPricePerUnit, outFile);
    outFile.close();
    return 0;
}