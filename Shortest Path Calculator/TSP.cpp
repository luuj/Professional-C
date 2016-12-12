#include "TSP.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
#include <sstream>

/***Setup and generating the initial population functions***/

//Split string based on comma delimiter
std::string SplitString(std::stringstream& ss) noexcept
{
    std::string placeHolder;
    std::getline(ss, placeHolder, ',');

    return placeHolder;
}

//Given an input file, parse it and store the locations in a vector
std::vector<Location> ParseLocations(const std::string& inputFile) noexcept
{
    std::vector<Location> parsedLocations;
    std::stringstream ss;
    std::string currLine;
    const double radianConstant = 0.0174533;
    
    std::ifstream file (inputFile, std::ios::in);
    if (file.is_open())
    {
        while (getline(file,currLine))
        {
            Location newLocation;
            
            //Parse and store in location
            ss.str(currLine);
            newLocation.mName = SplitString(ss);
            newLocation.mLatitude = std::stod(SplitString(ss)) * radianConstant;
            newLocation.mLongitude = std::stod(SplitString(ss)) * radianConstant;
            
            ss.clear();
            parsedLocations.push_back(newLocation);
        }
        
        file.close();
    }
    
    return parsedLocations;
}

//Create a population based on popSize parameter
Population GeneratePopulation(const int popSize, const unsigned long locSize, std::mt19937& randGen) noexcept
{
    Population createdPopulation;
    std::vector<std::vector<int>> initialVector(popSize);
    
    std::generate(initialVector.begin(), initialVector.end(), [&locSize, &randGen]()
    {
        return RandomizeVector(locSize, randGen);
    });
    
    createdPopulation.mMembers = initialVector;
    return createdPopulation;
}

//Create a sequential vector and randomize it
std::vector<int> RandomizeVector(const unsigned long locSize, std::mt19937& randGen) noexcept
{
    std::vector<int> initialVector(locSize);
    
    int count = -1;
    std::generate(initialVector.begin(), initialVector.end(), [&count]()
    {
        ++count;
        return count;
    });
    
    std::shuffle(initialVector.begin()+1, initialVector.end(), randGen);
    
    return initialVector;
}

//Write population to log.txt
const void OutputPopulation(const Population initialPop, std::ofstream& outfile, const std::string& title) noexcept
{
    outfile << title << "\n";
    
    std::for_each(initialPop.mMembers.begin(), initialPop.mMembers.end(), [&outfile](const std::vector<int>& currMember)
    {
        bool first = true;
        std::for_each(currMember.begin(), currMember.end(), [&first, &outfile](const int i)
        {
            if (first)
            {
                outfile << i;
                first = false;
            }
            else
            {
                outfile << "," << i;
            }
        });
        outfile << "\n";
    });
}


/***Fitness functions***/
std::vector<std::pair<int, double>> ComputeFitness(const Population& inputPop, const std::vector<Location>& parsedLocations) noexcept
{
    std::vector<std::pair<int, double>> fitnessValues;
    
    int counter = 0;
    std::for_each(inputPop.mMembers.begin(), inputPop.mMembers.end(), [&counter, &parsedLocations, &fitnessValues](const std::vector<int> &currMember)
    {
        std::vector<double> distanceValuesOne = ComputeAdjacent(currMember, parsedLocations);
        fitnessValues.push_back(std::make_pair(counter, ComputeSum(distanceValuesOne)));
        ++counter;
    });
    
    return fitnessValues;
}

//Use adjacent_difference to find distance between locations within a population
std::vector<double> ComputeAdjacent(const std::vector<int>& popMember, const std::vector<Location>& parsedLocations) noexcept
{
    std::vector<double> result;
    std::adjacent_difference(popMember.begin(), popMember.end(), std::back_inserter(result), [&parsedLocations](const int& locA, const int& locB)
    {
        Location a = parsedLocations.at(locA);
        Location b = parsedLocations.at(locB);
        
        return ComputeHaversine(a, b);
    });
    
    //Add in distance from last to LAX
    result.push_back(ComputeHaversine(parsedLocations.at(popMember.back()), parsedLocations.at(popMember.front())));
  
    return result;
}

//Add up distance values into a sum
double ComputeSum(const std::vector<double>& distanceValues) noexcept
{
    return std::accumulate(distanceValues.begin(), distanceValues.end(), 0.0, [](const double& a, const double& b){return a+b;});
}

//Apply haversine equation on two locations
double ComputeHaversine(const Location& a, const Location& b) noexcept
{
    double dlon = b.mLongitude - a.mLongitude;
    double dlat = b.mLatitude - a.mLatitude;
    double intermediate = pow((sin(dlat/2)),2) + cos(a.mLatitude) * cos(b.mLatitude) * pow((sin(dlon/2)),2);
    double cintermediate =  2 * atan2( sqrt(intermediate), sqrt(1-intermediate) );
    return 3961 * cintermediate;
}

//Write fitness results to log.txt
const void OutputFitness(const std::vector<std::pair<int, double>>& pairs, std::ofstream& outfile) noexcept
{
    outfile << "FITNESS:\n";
    
    std::for_each(pairs.begin(), pairs.end(), [&outfile](const std::pair<int, double>& currPair)
    {
        outfile << currPair.first << ":" << currPair.second << "\n";
    });
}


/***Selection functions***/
std::vector<std::pair<int, int>> ComputeSelection(const std::vector<std::pair<int, double>>& fitnessPairs, std::mt19937& randGen) noexcept
{
    std::vector<std::pair<int, int>> selectedPairs(fitnessPairs.size());
    std::vector<std::pair<int, double>> sortedFitnessPairs = SortFitnessPairs(fitnessPairs);
    
    std::vector<double> initialProbability = GenerateProbabilityVector(sortedFitnessPairs.size());
    std::vector<double> mutatedProbability = MutateProbabilityVector(initialProbability, sortedFitnessPairs);
    
    std::generate(selectedPairs.begin(), selectedPairs.end(), [&mutatedProbability, &randGen]()
    {
        return SelectOnePair(mutatedProbability, randGen);
    });
    
    return selectedPairs;
}

//Sort fitness pairs
std::vector<std::pair<int, double>> SortFitnessPairs(std::vector<std::pair<int, double>> fitnessPairs) noexcept
{
    std::vector<std::pair<int, double>> sortedFitnessPairs = fitnessPairs;
    std::sort(sortedFitnessPairs.begin(), sortedFitnessPairs.end(), [](std::pair<int,double> lhs, std::pair<int,double> rhs)
    {
        return lhs.second < rhs.second;
    });
    
    return sortedFitnessPairs;
}

//Generate initial probability vector
std::vector<double> GenerateProbabilityVector(const unsigned long& populationCount) noexcept
{
    std::vector<double> initialProbabilityVector(populationCount);
    std::generate(initialProbabilityVector.begin(), initialProbabilityVector.end(), [&populationCount](){return 1/static_cast<double>(populationCount);});

    return initialProbabilityVector;
}

//Mutate probability vector
std::vector<double> MutateProbabilityVector(std::vector<double> initialVector, const std::vector<std::pair<int, double>>& sortedFitnessPairs) noexcept
{
    std::vector<double> mutatedVector = initialVector;
    int size = static_cast<int>(sortedFitnessPairs.size());
    
    //Two individuals with highest fitness have probability x6
    mutatedVector.at(sortedFitnessPairs.at(0).first) *= 6;
    mutatedVector.at(sortedFitnessPairs.at(1).first) *= 6;

    //Remainder of top half have probability x3
    std::for_each(sortedFitnessPairs.begin()+2, sortedFitnessPairs.begin()+(size/2), [&mutatedVector](std::pair<int, double> currPair)
    {
        mutatedVector.at(currPair.first) *= 3;
    });
    
    mutatedVector = NormalizeProbabilityVector(mutatedVector);
    return mutatedVector;
}

//Normalize probability vector
std::vector<double> NormalizeProbabilityVector(const std::vector<double>& mutatedVector) noexcept
{
    std::vector<double> normalizedVector;
    double totalSum = ComputeSum(mutatedVector);

    std::transform(mutatedVector.begin(), mutatedVector.end(), std::back_inserter(normalizedVector), [&totalSum](const double& currVal)
    {
        return currVal / totalSum;
    });
    
    return normalizedVector;
}

//Choose a pair given a probability vector and randGen
std::pair<int, int> SelectOnePair(const std::vector<double>& mutatedProbability, std::mt19937& randGen) noexcept
{
    std::uniform_real_distribution<double> distribution(0,1);
    int parentOne, parentTwo;
    
    double firstParent = distribution(randGen);
    parentOne = SelectAlgorithm(mutatedProbability, firstParent);
    
    double secondParent = distribution(randGen);
    parentTwo = SelectAlgorithm(mutatedProbability, secondParent);
    
    return std::make_pair(parentOne, parentTwo);
}

//Select an index if the given double is less than the running sum
int SelectAlgorithm(const std::vector<double>& mutatedProbability, const double& selectionNum) noexcept
{
    double runningSum = 0;

    for (int i=0; i<mutatedProbability.size(); i++)
    {
        runningSum += mutatedProbability.at(i);
        if (runningSum >= selectionNum)
        {
            return i;
        }
    }
    
    return -1;
}

//Write selection results to log.txt
const void OutputSelection(const std::vector<std::pair<int, int>>& pairs, std::ofstream& outfile) noexcept
{
    outfile << "SELECTED PAIRS:\n";
    
    std::for_each(pairs.begin(), pairs.end(), [&outfile](const std::pair<int, int>& currPair)
    {
        outfile << "(" << currPair.first << "," << currPair.second << ")\n";
    });
}


/***Crossover and Generation functions***/
Population GenerateNewPopulation(std::vector<std::pair<int, int>>& selectedPairs, std::mt19937& randGen, Population& pop, const int& mutationChance) noexcept
{
    Population newPop;
    std::vector<std::vector<int>> newPopMembers;
    
    std::for_each(selectedPairs.begin(), selectedPairs.end(), [&pop, &randGen, &mutationChance, &newPopMembers](std::pair<int,int> currPair)
    {
        std::vector<int> parentOne = pop.mMembers.at(currPair.first);
        std::vector<int> parentTwo = pop.mMembers.at(currPair.second);
        int locSize = static_cast<int>(parentOne.size());

        std::vector<int> newMember = GenerateNewMember(randGen, locSize, parentOne, parentTwo);
        std::vector<int> mutatedNewMember = MutateNewMember(newMember, mutationChance, randGen);
        
        newPopMembers.push_back(mutatedNewMember);
    });
    
    newPop.mMembers = newPopMembers;
    return newPop;
}

//Given a pair to crossover, generate a new member of the population
std::vector<int> GenerateNewMember(std::mt19937& randGen, const int& locSize, std::vector<int>& parentOne, std::vector<int>& parentTwo) noexcept
{
    std::vector<int> child(locSize);
    
    std::uniform_int_distribution<int> distributionOne(1, locSize-2);
    int crossoverIndex = distributionOne(randGen);
    
    std::uniform_int_distribution<int> distributionTwo(0, 1);
    int chooseParent = distributionTwo(randGen);
    
    //If chooseParent is 1, parentOne=parentA, else parentTwo=parentA
    if (chooseParent)
    {
        child = CopyCrossover(parentOne, parentTwo, crossoverIndex);
    }
    else
    {
        child = CopyCrossover(parentTwo, parentOne, crossoverIndex);
    }
    
    return child;
}

//Copy parentA over up to the crossoverIndex point, then copy the rest of parentB over
std::vector<int> CopyCrossover(const std::vector<int>& parentA, const std::vector<int>& parentB, const int& crossoverIndex) noexcept
{
    std::vector<int> copiedChild;
    std::copy_n(parentA.begin(), crossoverIndex+1, std::back_inserter(copiedChild));

    std::copy_if(parentB.begin(), parentB.end(), std::back_inserter(copiedChild), [&copiedChild](const int& currVal)
    {
        return std::find(copiedChild.begin(), copiedChild.end(), currVal) == copiedChild.end();
    });
    
    return copiedChild;
}

//Mutate a vector based on the mutation chance
std::vector<int> MutateNewMember(std::vector<int> newMember, const int& mutationChance, std::mt19937& randGen) noexcept
{
    std::vector<int> mutatedChild = newMember;
    std::uniform_real_distribution<double> distributionOne(0,1);
    double mutateProb = distributionOne(randGen);
    int locSize = static_cast<int>(newMember.size());

    if (mutateProb <= (static_cast<double>(mutationChance)/100))
    {
        //Swap two randomly found indices
        std::uniform_int_distribution<int> distributionTwo(1,locSize-1);
        int randIndexOne = distributionTwo(randGen);
        int randIndexTwo = distributionTwo(randGen);
        
        std::iter_swap(mutatedChild.begin() + randIndexOne, mutatedChild.begin() + randIndexTwo);
    }
    
    return mutatedChild;
}

//Print out all locations in the solution
void OutputFinalSolution(std::ofstream& outfile, const std::vector<int>& popMember, const std::vector<Location>& parsedLocations) noexcept
{
    outfile << "SOLUTION:\n";
    
    std::for_each(popMember.begin(), popMember.end(), [&parsedLocations, &outfile](const int& currIndex)
    {
        outfile << parsedLocations.at(currIndex).mName << "\n";
    });
    
    //Return to original position
    outfile << parsedLocations.at(0).mName << "\n";
}

