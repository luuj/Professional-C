#include <iostream>
#include <random>
#include "TSP.h"
#include <fstream>
#include <algorithm>
#include <iomanip>

int main(int argc, const char* argv[])
{
    //Parsing command-line parameters
    std::string inputFile = argv[1];
    int popSize = std::stoi(argv[2]);
    int generations = std::stoi(argv[3]);
    int mutationChance = std::stoi(argv[4]);
    int startingSeed = std::stoi(argv[5]);
    int currGen;
    
    //Seed RNG
    std::mt19937 randGen(startingSeed);
    std::ofstream outfile("log.txt");
    
    //Initial population creation
    std::vector<Location> parsedLocations = ParseLocations(inputFile);
    Population initialPopulation = GeneratePopulation(popSize, parsedLocations.size(), randGen);
    OutputPopulation(initialPopulation, outfile, "INITIAL POPULATION:");
    
    //Repeat for # of generations
    for (currGen = 1; currGen <= generations; currGen++)
    {
        //Calculating Fitness
        std::vector<std::pair<int, double>> fitnessPairs = ComputeFitness(initialPopulation, parsedLocations);
        OutputFitness(fitnessPairs, outfile);
        
        //Selecting Pairs
        std::vector<std::pair<int, int>> selectedPairs = ComputeSelection(fitnessPairs, randGen);
        OutputSelection(selectedPairs, outfile);
        
        //Creating new population
        initialPopulation = GenerateNewPopulation(selectedPairs, randGen, initialPopulation, mutationChance);
        OutputPopulation(initialPopulation, outfile, "GENERATION: " + std::to_string(currGen));
    }
    
    //Calculate final fitness values and output result
    std::vector<std::pair<int, double>> fitnessPairs = ComputeFitness(initialPopulation, parsedLocations);
    OutputFitness(fitnessPairs, outfile);
    fitnessPairs = SortFitnessPairs(fitnessPairs);
    
    std::pair<int,double> finalPair = fitnessPairs.at(0);
    std::vector<int> finalPopulationMember = initialPopulation.mMembers.at(finalPair.first);
    OutputFinalSolution(outfile, finalPopulationMember, parsedLocations);
    
    outfile << "DISTANCE: " << finalPair.second << " miles";
    outfile.close();
    
	return 0;
}


