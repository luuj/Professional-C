#pragma once
#include <string>
#include <vector>
#include <random>

struct Location
{
	std::string mName;
	double mLatitude;
	double mLongitude;
};

struct Population
{
	std::vector<std::vector<int>> mMembers;
};

/***Setup and generating the initial population functions***/
std::string SplitString(std::stringstream& ss) noexcept;
std::vector<Location> ParseLocations(const std::string& inputFile) noexcept;
Population GeneratePopulation(const int popSize, const unsigned long locSize, std::mt19937& randGen) noexcept;
std::vector<int> RandomizeVector(const unsigned long locSize, std::mt19937& randGen) noexcept;
const void OutputPopulation(const Population initialPop, std::ofstream& outfile, const std::string& title) noexcept;

/***Fitness functions***/
std::vector<std::pair<int, double>> ComputeFitness(const Population& inputPop, const std::vector<Location>& parsedLocations) noexcept;
std::vector<double> ComputeAdjacent(const std::vector<int>& popMember, const std::vector<Location>& parsedLocations) noexcept;
double ComputeSum(const std::vector<double>& distanceValues) noexcept;
double ComputeHaversine(const Location& a, const Location& b) noexcept;
const void OutputFitness(const std::vector<std::pair<int, double>>& pairs, std::ofstream& outfile) noexcept;

/***Selection functions***/
std::vector<std::pair<int, int>> ComputeSelection(const std::vector<std::pair<int, double>>& fitnessPairs, std::mt19937& randGen) noexcept;
std::vector<std::pair<int, double>> SortFitnessPairs(std::vector<std::pair<int, double>> fitnessPairs) noexcept;
std::vector<double> GenerateProbabilityVector(const unsigned long& populationCount) noexcept;
std::vector<double> MutateProbabilityVector(std::vector<double> initialVector, const std::vector<std::pair<int, double>>& sortedFitnessPairs) noexcept;
std::vector<double> NormalizeProbabilityVector(const std::vector<double>& mutatedVector) noexcept;
std::pair<int, int> SelectOnePair(const std::vector<double>& mutatedProbability, std::mt19937& randGen) noexcept;
int SelectAlgorithm(const std::vector<double>& mutatedProbability, const double& selectionNum) noexcept;
const void OutputSelection(const std::vector<std::pair<int, int>>& pairs, std::ofstream& outfile) noexcept;

/***Crossover and Generation functions***/
Population GenerateNewPopulation(std::vector<std::pair<int, int>>& selectedPairs, std::mt19937& randGen, Population& pop, const int& mutationChance) noexcept;
std::vector<int> GenerateNewMember(std::mt19937& randGen, const int& locSize, std::vector<int>& parentOne, std::vector<int>& parentTwo) noexcept;
std::vector<int> CopyCrossover(const std::vector<int>& parentA, const std::vector<int>& parentB, const int& crossoverIndex) noexcept;
std::vector<int> MutateNewMember(std::vector<int> newMember, const int& mutationChance, std::mt19937& randGen) noexcept;
void OutputFinalSolution(std::ofstream& outfile, const std::vector<int>& popMember, const std::vector<Location>& parsedLocations) noexcept;
