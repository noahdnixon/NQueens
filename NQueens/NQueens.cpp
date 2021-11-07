#include <iostream>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>  
#include <string>
#include <random>
using namespace std;

//Header File
void GeneratePop();
void SortCandidates();
void Crossover();
void Mutate();
void DisplayBoard();
void DisplayPop();
void BestCandidate();
float AverageFitness();
int EvaluateCandidate(int CandToCheck);
int CheckRows();
int CheckDiagonals();


//Variables
const int RowSize = 8;
const int ColumnSize = 8;
const int PopSize = 8;
const float MutationRate = .5;
int BestConf = 8;
string BestStr = "";

//Random number generator
random_device rd;
default_random_engine rng(rd());

uniform_int_distribution<> PopMake(1, 8);
uniform_int_distribution<> SplitMake(1, 7);
uniform_int_distribution<> FavorMake(1, 4);
uniform_int_distribution<> Favor3(0, 5);
uniform_int_distribution<> Favor2(0, 3);
uniform_int_distribution<> Favor1(0, 1);
uniform_int_distribution<> ArrayMake(0, 7);


//Struct that contains queen info for each position on Chess board
struct ChessPos
{
    int HasQueen;
};

//Struct that contains info for a single candidate in a population
struct Candidate
{
    string Values = "";
    int TotalConflicts;
};

//Arrays for populations and boards
Candidate Population[PopSize];
ChessPos Board[RowSize][ColumnSize];


//Make an empty Chess board of zeroes(no queens)
void InitializeEmpty()
{
    for (int i = 0; i < RowSize; i++)
    {
        for (int j = 0; j < ColumnSize; j++)
        {
            Board[i][j].HasQueen = 0;
        }
    }
}

//Generate random numbers from 1-8
//Put numbers in population array
void GeneratePop()
{

    for(int i = 0; i < RowSize; i++)
    {
        for (int j = 0; j < RowSize; j++)
        {
            Population[i].Values += PopMake(rng) + '0';
        }
    }
    DisplayPop();

}

//Place numbers from array on Chess board and evaluate each direction to get total conflicts for this candidate
int EvaluateCandidate(int CandToCheck)
{
    InitializeEmpty();

    string StringConv = Population[CandToCheck].Values;
    for (int i = 0; i < ColumnSize; i++)
    {
            if (i == 0)
            {
                Board[stoi(StringConv.substr(0, 1)) - 1][i].HasQueen = 1;
                continue;
            }
            else if (i == ColumnSize - 1)
            {
                Board[stoi(StringConv.substr(i, i + 1)) - 1][i].HasQueen = 1;
                continue;
            }
            else
            {
                Board[stoi(StringConv.substr(i, 1)) - 1][i].HasQueen = 1;
            }
    }

    // Debug
    /*
    DisplayBoard();
    cout << "\nRow Conflicts: " << CheckRows() << endl;
    cout << "Diagonal Conflicts: " << CheckDiagonals() << endl;
    cout << "Total Conflicts: " << (CheckRows() + CheckDiagonals()) << endl;
    */

    Population[CandToCheck].TotalConflicts = (CheckRows() + CheckDiagonals());

    return 0;
}

//Checks for conflicts in rows of one candidate
//No need to check for column conflicts because pop generation automatically gives 0 column conflicts
int CheckRows()
{
    int RowAttackCount = 0;
    int NumInThisRow = 0;
    int RowAttackTotal = 0;

    for (int i = 0; i < RowSize; i++)
    {
        for (int j = 0; j < ColumnSize; j++)
        {
            if (Board[i][j].HasQueen == 1)
            {
                NumInThisRow += 1;
                if (NumInThisRow >= 2)
                {
                    RowAttackCount += 1;
                }
            }
        }
              
        RowAttackTotal += RowAttackCount;
        RowAttackCount = 0;  
        NumInThisRow = 0;

    }
    return RowAttackTotal;
}

//Checks upper and lower diagonals for conflicts with one particular candidate
int CheckDiagonals()
{
    int DiagAttacks = 0;
    int DiagTotal = 0;
    for (int i = 0; i < RowSize; i++)
    {
        for (int j = 0; j < ColumnSize; j++)
        {
            if (Board[j][i].HasQueen == 1)
            {  
                for (int x = 1; x < ColumnSize; x++)
                {
                    //Check Lower Diagonal
                    if (j + x > 7 || i + x > 7)
                    {
                        continue;
                    }
                    if (Board[j + x][i + x].HasQueen == 1)
                    {
                        DiagAttacks += 1;
                        break;
                    }
                    
                }

                for (int c = 1; c < ColumnSize; c++)
                {
                    // Check upper diagonal
                    if (j - c < 0 || i + c > 7)
                    {
                        continue;
                    }
                    if (Board[j - c][i + c].HasQueen == 1)
                    {
                        DiagAttacks += 1;
                    }
                }
                
            }

            DiagTotal += DiagAttacks;
            DiagAttacks = 0;
        }
    }
    return DiagTotal;
}

//Displays the board for one particular candidate - debug method
void DisplayBoard()
{
    cout << "\nDisplaying Board..." << endl;
    for (int i = 0; i < RowSize; i++)
    {
        for (int j = 0; j < ColumnSize; j++)
        {
            cout << Board[i][j].HasQueen << " ";
        }
        cout << endl;
    }

    cout << endl;
}

//Display candidates in population with total conflicts for each
void DisplayPop()
{
    cout << endl;
    for (int x = 0; x < RowSize; x++)
    {
        cout << (x+1) << ": " << Population[x].Values << " - " << Population[x].TotalConflicts << endl;
    }
}

//Sorts candidates in population from least to most conflicts
void SortCandidates()
{
    for (int i = 0; i < ColumnSize; i++)
    {
        for (int j = 0; j < (ColumnSize - 1); j++)
        {
            if (Population[j].TotalConflicts > Population[j + 1].TotalConflicts)
            {
                Candidate temp = Population[j + 1];
                Population[j + 1] = Population[j];
                Population[j] = temp;
            }
        }
    }
}

//Picks a split point and crosses over random candidates based on that split point
//Favors picking candidates with less conflicts
void Crossover()
{
    int SplitPoint;
    int PopInsert = 0;
    int Favor = 0;

    string CrossString1;
    string CrossString2;

    string CrossString1Part1;
    string CrossString1Part2;

    string CrossString2Part1;
    string CrossString2Part2;

    for (int i = 0; i < 4; i++)
    {

        //Generate random split point
        SplitPoint = SplitMake(rng);

        //Generate favor to more likely pick better candidates
        Favor = FavorMake(rng);

        //Lower favor number means more likely to pick candidates with least conflicts
        if(Favor == 4)
        { 
            CrossString1 = Population[ArrayMake(rng)].Values;
            CrossString2 = Population[ArrayMake(rng)].Values;
        }
        else if (Favor == 3)
        {
            CrossString1 = Population[Favor3(rng)].Values;
            CrossString2 = Population[Favor3(rng)].Values;
        }
        else if (Favor == 2)
        {
            CrossString1 = Population[Favor2(rng)].Values;
            CrossString2 = Population[Favor2(rng)].Values;
        }
        else if (Favor == 1)
        {
            CrossString1 = Population[Favor1(rng)].Values;
            CrossString2 = Population[Favor1(rng)].Values;
        }
        

        /* Debug
        cout << "\nCandidates chosen:" << endl;
        cout << CrossString1 << endl;
        cout << CrossString2 << endl;

        cout << "Split Point: " << SplitPoint << endl;
        */

        //Crossover for each at split point
        CrossString1Part1 = CrossString1.substr(0, SplitPoint);
        CrossString1Part2 = CrossString1.substr(SplitPoint);

        CrossString2Part1 = CrossString2.substr(0, SplitPoint);
        CrossString2Part2 = CrossString2.substr(SplitPoint);

        CrossString1 = CrossString1Part1 + CrossString2Part2;
        CrossString2 = CrossString2Part1 + CrossString1Part2;

        /*Debug
        cout << "\nCandidates crossed:" << endl;
        cout << CrossString1 << endl;
        cout << CrossString2 << endl;
        */

        //Add new candidates to population array for next generation
        Population[PopInsert].Values = CrossString1;  
        Population[PopInsert + 1].Values = CrossString2;
        PopInsert += 2;
    }

    //Evaluate candidates in new population for display purposes
    for (int j = 0; j < PopSize; j++)
    {
        EvaluateCandidate(j);
    }
}


//Mutate random numbers of random candidates in population based on mutation rate
void Mutate()
{
    
    for (int i = 0; i < PopSize; i++)
    {
        if ((rand() % 100) < (100 * MutationRate))
        {
            int MutateLocation = ArrayMake(rng);
            Population[i].Values[MutateLocation] = '0' + PopMake(rng);

        }
    }

    //Evaluate candidates in new mutated population for display purposes
    for (int j = 0; j < PopSize; j++)
    {
        EvaluateCandidate(j);
    }
}

//Check and return average fitness of population
float AverageFitness()
{
    int Total = 0;
    for (int i = 0; i < PopSize; i++)
    {
        Total += Population[i].TotalConflicts;
    }

    return Total / (float)PopSize;
}

//Check population for new best candidate
void BestCandidate()
{
    for (int i = 0; i < PopSize; i++)
    {
        if (Population[i].TotalConflicts < BestConf)
        {
            BestConf = Population[i].TotalConflicts;
            BestStr = Population[i].Values;
        }
    }
}

int main()
{
    //Show average fitness and best candidate found over 1000 generations
   /* cout << "Initial Population" << endl;
    GeneratePop();
    for (int c = 0; c < 1000; c++)
    {
        for (int i = 0; i < PopSize; i++)
        {
            EvaluateCandidate(i);
        }

        BestCandidate();
        
        if (c % 10 == 0)
        {
            cout << "Average Fitness: " << AverageFitness() << endl;
            cout << "Best Candidate: " << BestStr << " - " << BestConf << endl;
        }

        SortCandidates();

        Crossover();

        Mutate();
    }
    cout << "\n Final Population" << endl;
    DisplayPop();*/

    //Show entire process for one generation
    
    cout << "Generating population..." << endl;
    GeneratePop();

        for (int i = 0; i < PopSize; i++)
        {
            EvaluateCandidate(i);
        }

        SortCandidates();

        cout << "\nEvaluate and Sort candidates..." << endl;
        DisplayPop();

        Crossover();

        cout << "\nCrossover candidates for next generation..." << endl;
        DisplayPop();


        Mutate();

        cout << "\nMutate new population..." << endl;
        DisplayPop();
    

}

