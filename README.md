
# Optimal Development Problem

A builder is planning a residential development in a designated area and aims to include a set of points of interest (POIs)—such as parks, schools, and community centers—within the development. The builder has to work within his budget, which will cover the cost of buying/developing the land (this is related to a fixed square foot cost), as well as implementing a road network between the points of interest. In a large residential area, there are many different points that the builder may want to consider putting in his development, however it would be time-consuming for the builder to go through and run all the calculations himself. Our program aims to automate this process. Given a set of possible points for the builder to include in his development and a budget, the builder should be able to choose which points he wants, and the program decides if buying/developing the land and implementing roads would be feasible


## Contributors

* [Nicholas DeNobrega](https://www.linkedin.com/in/nickdeno/)
* [Samuel Buckler](https://www.linkedin.com/in/samuel-buckler-18998a259/)


## Program Outline
* User acts as builder in the scenario
* Set of POI is given to user, where each POI has name, x-coordinate, and y-coordinate
* User is prompted to pick a subset of given POI and a budget through terminal
* The optimal subset of these points is then outputted to user, where optimal subset is 
  the set of points that buying the land encapsulating them and building roads connecting
  them does not exceed budget given by user
## Problem Approach
In order to determine how much land the builder needs to buy, we can construct a convex hull that encapsulates all of the selected points of interest. We can calculate the area of the convex hull, and based on the fixed square-foot cost of buying the land, we can compute how much it would cost the builder to buy. The next step is to connect all of the points of interest by a roadway network, which will be implemented using a modified Floyd’s all-paths shortest paths graph algorithm. There is also a fixed linear foot cost for building roads, which will contribute to the total cost that the builder will have to pay for his development. After the cost of buying the land is computed, and the cost of building the roads is computed, we can determine whether or not the builder can afford to build his development around all of the points of interest selected. So we iterate over possible subsets of points that the builder chose, and for each subset we:
 
    1. Get Convex Hull of current subset (getHullDNC())
    2. Calculate area of current Convex Hull using Shoelace Formula, formally known 
       as Gauss's Area Formula (calculateConvexHullArea())
    3. Calculate land cost of area of current Convex Hull by mulitplying area * fixed 
       square-foot cost
    4. If cost exceeds budget, move on to the next subset, else continue
    5. Generate Undirected Graph based on current subset of points that connects every 
       point to every other point in graph (weight/distance of each edge is based on
       x and y coordinates of point)
    6. Determine shortest path between all set of points by running a modified Floyd's 
       Algorithm on the generated graph 
    7. Calculate total distance of graph (sum of edge weights for all shortest paths)
    8. Multiply total distance of graph by fixed linear-foot cost of building roads to
       calculate cost of building roads connecting all POI
    9. If the land cost + roadway cost exceeds budget, move on to next subset, else
       continue
    10. At this point we know we can afford to buy land and build roads for the current 
        subset of POI, so update best cost and best subset to current cost and current 
        subset, then move on to next subset
## Results/Discussion
For our test, we had a budget of $200,000, a land per unit cost of $2000, and a road cost per unit of $1000. We used the given set of points: 
* ("A", 0.0, 0.0), 
* ("B", 1.0, 1.0), 
* ("C", 2.0, 2.0),
* ("D", 2.0, 0.0),
* ("E", 2.0, 4.0),
* ("F", 3.0, 1.0),
* ("G", 5.0, 7.0),
* ("H", 7.0, 4.0),
* ("I", 9.0, 1.0),
* ("J", 3.0, 5.0),
* ("K", 4.0, 2.0),
* ("L", 1.0, 9.0),
* ("M", 8.0, 5.0),
* ("N", 2.0, 6.0),
* ("O", 7.0, 4.0),
* ("P", 4.0, 3.0),
* ("Q", 3.0, 8.0),
* ("R", 6.0, 1.0),
* ("S", 5.0, 7.0),
* ("T", 1.0, 9.0)
and had the user/builder select the following 10 points: 
* ("A", 0.0, 0.0), 
* ("B", 1.0, 1.0), 
* ("C", 2.0, 2.0),
* ("D", 2.0, 0.0),
* ("E", 2.0, 4.0),
* ("F", 3.0, 1.0),
* ("G", 5.0, 7.0),
* ("H", 7.0, 4.0),
* ("I", 9.0, 1.0),
* ("J", 3.0, 5.0)
Our optimal solution for these set of points had a land cost of $64,000, roadway cost of $135,000, and a total cost of $199,000. The following set of points was the optimal solution:
* ("E", 2.0, 4.0),
* ("A", 0.0, 0.0),
* ("I", 9.0, 1.0),
* ("H", 7.0, 4.0),
* ("G", 5.0, 7.0)
As we can see, 99.5% of the total budget was used. For 10 selected points, with 5 of them in optimal solution, it took 0.048492 seconds to run. The overall time complexity of the program is O(2^n), as it considered every possible subset of points from the set of points chosen by the builder. For each subset of points, we compute a Convex Hull, calculates area of said Convex Hull, and runs Floyd’s algorithm. Convex Hull D&C has a time complexity of O(nlogn), however being that we are calculating every possible convex hull, this program only supports the ability to select approximately ~10-12 points at a time, unfortunately making the Convex Hull Algorithm closer to O(n^3) in this application. 2^10 = 1024, while 2^20 = 1048576. So as the number of points that the builder selects increases, the program takes exponentially longer. This means that the Optimal Development problem is in NP-Complete since it cannot be computed in polynomial time

