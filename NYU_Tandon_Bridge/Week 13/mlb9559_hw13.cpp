/*
NYU Tandon Bridge - Week 13 (Winter 2024)
Predator-Prey Simulation (Doodlebug)

Assignment:
Create a two-dimensional predator-prey simulation with doodlebugs (predators) and ants (prey)
on a 20×20 grid. Each cell can contain only one critter, and the grid is enclosed.

Organism Behaviors:
1. Ants:
   • Move: Random direction (up, down, left, right) if cell is empty
   • Breed: After surviving 3 time steps, create new ant in adjacent empty cell
   • Reset breeding counter after producing offspring

2. Doodlebugs:
   • Move: Eat adjacent ant if present, otherwise move randomly
   • Breed: After surviving 8 time steps
   • Starve: Die if haven't eaten in 3 time steps

Implementation Requirements:
• Create Organism base class with virtual move function
• Derive Ant and Doodlebug classes
• Initialize world with 5 doodlebugs and 100 ants
• Display grid using:
  - 'X' for doodlebugs
  - 'o' for ants
  - '-' for empty spaces
• Process doodlebugs before ants in each time step
• Prompt user to press Enter between steps

Note: Submit as single .cpp file containing all necessary classes and structures
*/

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
using namespace std;

//Constants
const int WORLD_ROWS=20;
const int WORLD_COLUMNS=20;
const int INITIAL_ANTS=100;
const int INITIAL_DOODLEBUGS=5;
const char ANT_SYMBOL='o';
const char DOODLEBUG_SYMBOL='X';
const char EMPTY_SPACE_SYMBOL='-';

//Structures and classes
struct Position {
    int row;
    int column;
};
class World;
class Organism {
private:
    Position position;
public:
    Organism(){}
    Organism (const Position& pos) : position(pos) {}
    virtual ~Organism() {}
    Position getPosition () const;
    void setPosition(const Position& pos);
    virtual char getSymbol () const=0;
    virtual void move (World& world)=0;
    virtual void breed (World& world)=0;
    virtual void starve (World& world)=0;
};
class Ant : public Organism{
private:
    int survivalCount;
public:
    Ant() : Organism(), survivalCount(0) {};
    Ant (const Position& pos) : Organism(pos), survivalCount(0) {};
    virtual ~Ant() {}
    virtual char getSymbol () const;
    virtual void move (World& world);
    virtual void breed (World& world);
    virtual void starve (World& world);
};
class Doodlebug : public Organism{
private:
    int survivalCount;
    int roundsWithoutFood;
public:
    Doodlebug() : Organism(), survivalCount(0), roundsWithoutFood(0) {};
    Doodlebug (const Position& pos) : Organism(pos), survivalCount(0), roundsWithoutFood (0) {};
    virtual ~Doodlebug() {}
    virtual char getSymbol () const;
    virtual void move (World& world);
    virtual void breed (World& world);
    virtual void starve (World& world);
};
class World{
private:
    Organism *grid[WORLD_ROWS][WORLD_COLUMNS];
public:
    World();
    ~World();
    void printWorld() const;
    vector <Position> getAdjacentPos (const Position& pos) const;
    Organism* getOrganism (const Position& pos) const;
    void setOrganismAtPosition(const Position& pos, Organism* organism);
    bool isPositionEmpty (const Position& pos) const;
    bool isPositionWithinBoundaries (const Position& pos) const;
    void moveOrganism (const Position& currPos, const Position& newPos);
    void removeOrganism (const Organism* org);
    void singleTimeStep();
};

//Function definitions - Organism class
Position Organism :: getPosition() const {
    return position;
}
void Organism :: setPosition(const Position& pos) {
    position.row=pos.row;
    position.column=pos.column;
}
void Organism :: starve (World& world){}

//Function definitions - Ant class
char Ant :: getSymbol() const {
    return ANT_SYMBOL;
}
void Ant :: move(World& world) {

    Position currPosition = getPosition();

    // Get adjacent empty cells within boundaries
    vector<Position> emptyCellsWithinBoundaries;
    vector<Position> cellsWithinBoundaries = world.getAdjacentPos(currPosition);
    for (unsigned int i = 0; i < cellsWithinBoundaries.size(); i++) {
        if (world.isPositionEmpty(cellsWithinBoundaries[i])) {
            emptyCellsWithinBoundaries.push_back(cellsWithinBoundaries[i]);
        }
    }
    // Check if there are any empty cells to move to
    if (emptyCellsWithinBoundaries.size()!=0) {
        // Generate a random index to select a random empty cell
        int randIndex = rand() % emptyCellsWithinBoundaries.size();
        Position newPosition = emptyCellsWithinBoundaries[randIndex];
        // Move organism
        world.moveOrganism(currPosition, newPosition);
    }
    // If there are no empty cells to move to, the ant stays in its current position
    survivalCount++;
}
void Ant :: breed(World& world) {

    if (survivalCount == 3) {
        // Breed new ant if survival count reaches 3
        Position currPosition = getPosition();
        // Get adjacent empty cells within boundaries
        vector<Position> emptyCellsWithinBoundaries;
        vector<Position> cellsWithinBoundaries = world.getAdjacentPos(currPosition);
        for (unsigned int i = 0; i < cellsWithinBoundaries.size(); i++) {
            if (world.isPositionEmpty(cellsWithinBoundaries[i])) {
                emptyCellsWithinBoundaries.push_back(cellsWithinBoundaries[i]);
            }
        }
        // Check if there are any empty cells to breed the new ant
        if (emptyCellsWithinBoundaries.size() != 0) {
            int randIndex = rand() % emptyCellsWithinBoundaries.size();
            Position newAntsPosition = emptyCellsWithinBoundaries[randIndex];
            // Set the organism
            world.setOrganismAtPosition(newAntsPosition, new Ant (newAntsPosition));
        }
        survivalCount=0;
    }
}
void Ant :: starve (World& world){}

//Function definitions - Doodlebug class
char Doodlebug :: getSymbol() const {
    return DOODLEBUG_SYMBOL;
}
void Doodlebug :: move (World& world) {

    Position currPosition = getPosition();

    //Get adjacent empty cells within boundaries
    vector<Position> cellsWithinBoundaries = world.getAdjacentPos(currPosition);
    //Find ants within those cells
    vector <Position> cellsWithAnts;
    for (unsigned int i = 0; i < cellsWithinBoundaries.size(); i++) {
        Organism* org=world.getOrganism(cellsWithinBoundaries[i]);
        if ((org!= nullptr) && (org->getSymbol()==ANT_SYMBOL)){
            cellsWithAnts.push_back(cellsWithinBoundaries[i]);
        }
    }
    //If at least one ant is found, pick a random ant to be eaten
    if (cellsWithAnts.size()!=0){
        // Generate a random index to select a random cell with ant
        int randIndex = rand() % cellsWithAnts.size();
        // Update position
        Position newPosition = cellsWithAnts[randIndex];
        // Remove the ant that will be eaten
        world.removeOrganism(world.getOrganism(newPosition));
        // Move organism in place of that ant
        world.moveOrganism(currPosition, newPosition);
        // Update counts
        roundsWithoutFood=0;
        survivalCount++;
    } else { //if there are no ants to be eaten, move into the random empty cell
        // Find empty cells
        vector<Position> emptyCellsWithinBoundaries;
        for (unsigned int i = 0; i < cellsWithinBoundaries.size(); i++) {
            if (world.isPositionEmpty(cellsWithinBoundaries[i])) {
                emptyCellsWithinBoundaries.push_back(cellsWithinBoundaries[i]);
            }
        }
        // Check if there are any empty cells to move to
        if (emptyCellsWithinBoundaries.size()!=0) {
            // Generate a random index to select a random empty cell
            int randIndex = rand() % emptyCellsWithinBoundaries.size();
            Position newPosition = emptyCellsWithinBoundaries[randIndex];
            // Move organism
            world.moveOrganism(currPosition, newPosition);
        }
        roundsWithoutFood++;
        survivalCount++;
    }
}
void Doodlebug :: breed (World& world){
    if (survivalCount==8){
        Position currPosition = getPosition();
        // Get adjacent empty cells within boundaries
        vector<Position> emptyCellsWithinBoundaries;
        // Get adjacent empty cells
        vector<Position> cellsWithinBoundaries = world.getAdjacentPos(currPosition);
        for (unsigned int i = 0; i < cellsWithinBoundaries.size(); i++) {
            if (world.isPositionEmpty(cellsWithinBoundaries[i])) {
                emptyCellsWithinBoundaries.push_back(cellsWithinBoundaries[i]);
            }
        }
        // Check if there are any empty cells to breed new doodlebug
        if (emptyCellsWithinBoundaries.size()!=0) {
            // Generate a random index to select a random empty cell
            int randIndex = rand() % emptyCellsWithinBoundaries.size();
            Position newDoodlebugPosition = emptyCellsWithinBoundaries[randIndex];
            // Breed new doodlebug
            world.setOrganismAtPosition(newDoodlebugPosition, new Doodlebug (newDoodlebugPosition));
            // Update survival count after breeding
            survivalCount=0;
        }
    }
}
void Doodlebug :: starve (World& world){
    if (roundsWithoutFood==3){
        world.removeOrganism(this);
    }
}

//Function definitions - World class
World :: World () {
    //Populate grid with nullptr
    for (int i = 0; i < WORLD_ROWS; i++) {
        for (int j = 0; j < WORLD_COLUMNS; j++) {
            grid[i][j] = nullptr;
        }
    }
    // Populate array with doodlebugs
    for (int i = 0; i < INITIAL_DOODLEBUGS; i++) {
        int randRow, randColumn;
        // Generate random row number and random column number
        do {// Generate random row number and random column number
            randRow = rand() % WORLD_ROWS;
            randColumn = rand() % WORLD_COLUMNS;
        } while (grid[randRow][randColumn] != nullptr); // Check if the cell is already occupied
        // Create doodlebug's position
        Position doodlebugPos;
        doodlebugPos.row = randRow;
        doodlebugPos.column = randColumn;
        // Create doodlebug object in the random cell
        grid[randRow][randColumn] = new Doodlebug(doodlebugPos);
    }
    // Populate array with ants
    for (int i = 0; i < INITIAL_ANTS; i++) {
        int randRow, randColumn;
        do {// Generate random row number and random column number
            randRow = rand() % WORLD_ROWS;
            randColumn = rand() % WORLD_COLUMNS;
        } while (grid[randRow][randColumn] != nullptr); // Check if the cell is already occupied
        // Create ant's position
        Position antPos;
        antPos.row = randRow;
        antPos.column = randColumn;
        // Create doodlebug object in the random cell
        grid[randRow][randColumn] = new Ant(antPos);
    }
}
World :: ~World(){
    for (int i = 0; i < WORLD_ROWS; i++){
        for (int j = 0; j < WORLD_COLUMNS; j++){
            if (grid[i][j] != nullptr){
                delete grid[i][j];
            }
        }
    }
}
void World :: printWorld() const {
    //Iterate through the grid, return appropriate organism's character when its object is found
    for (int i=0; i<WORLD_ROWS; i++){
        for (int j=0; j<WORLD_COLUMNS; j++){
            if (grid[i][j]==nullptr){
                cout<<EMPTY_SPACE_SYMBOL;
            } else if (grid[i][j]->getSymbol()==ANT_SYMBOL){
                cout<<ANT_SYMBOL;
            } else if (grid[i][j]->getSymbol()==DOODLEBUG_SYMBOL){
                cout<<DOODLEBUG_SYMBOL;
            }
            if (j!=WORLD_COLUMNS-1){
                cout<<" ";
            } else {
                cout<<endl;
            }
        }
    }
}
vector<Position> World :: getAdjacentPos (const Position& pos) const {
    vector<Position> adjacentPositions;

    // Check and add positions above, below, to the left, and to the right of pos
    if (isPositionWithinBoundaries({pos.row - 1, pos.column}))
        adjacentPositions.push_back({pos.row - 1, pos.column}); // Up
    if (isPositionWithinBoundaries({pos.row + 1, pos.column}))
        adjacentPositions.push_back({pos.row + 1, pos.column}); // Down
    if (isPositionWithinBoundaries({pos.row, pos.column - 1}))
        adjacentPositions.push_back({pos.row, pos.column - 1}); // Left
    if (isPositionWithinBoundaries({pos.row, pos.column + 1}))
        adjacentPositions.push_back({pos.row, pos.column + 1}); // Right

    return adjacentPositions;
}
Organism* World :: getOrganism (const Position& pos) const{
    if(isPositionWithinBoundaries(pos)==true){
        return grid[pos.row][pos.column];
    } else {
        return nullptr;
    }
}
void World:: setOrganismAtPosition(const Position& pos, Organism* organism) {
    grid[pos.row][pos.column] = organism;
    organism ->setPosition(pos);
}
bool World :: isPositionEmpty (const Position& pos) const{
    return grid[pos.row][pos.column]==nullptr;
}
bool World :: isPositionWithinBoundaries (const Position& pos) const {
    return pos.row>=0 && pos.row<WORLD_ROWS && pos.column>=0 && pos.column<WORLD_COLUMNS;
}
void World :: moveOrganism (const Position& currPos, const Position& newPos){
    //Move to new position
    grid[newPos.row][newPos.column]=grid[currPos.row][currPos.column];
    //Empty current position
    grid[currPos.row][currPos.column]=nullptr;
    //Update objects position
    grid[newPos.row][newPos.column]->setPosition(newPos);
}
void World :: removeOrganism (const Organism* org){
    Position currPosition = org->getPosition();
    grid[currPosition.row][currPosition.column] = nullptr;
    delete org;
}
void World :: singleTimeStep() {
    vector <Organism*> ants;
    vector <Organism*> doodlebugs;

    //Populate doodlebugs vector with current doodlebugs positions
    for (int i=0; i<WORLD_ROWS; i++){
        for (int j=0; j<WORLD_COLUMNS; j++){
            if (grid[i][j] != nullptr && grid[i][j]->getSymbol()==DOODLEBUG_SYMBOL) {
                doodlebugs.push_back(grid[i][j]);
            }
        }
    }
    //Move doodlebugs
    for (unsigned int i=0; i<doodlebugs.size(); i++){
        doodlebugs[i]->move(*this);
    }
    //Populate ants vector with current ants positions (after doodlebugs already moved)
    for (int i=0; i<WORLD_ROWS; i++){
        for (int j=0; j<WORLD_COLUMNS; j++){
            if (grid[i][j] != nullptr && grid[i][j]->getSymbol()==ANT_SYMBOL) {
                ants.push_back(grid[i][j]);
            }
        }
    }
    //Move ants
    for (unsigned int i=0; i<ants.size(); i++){
        ants[i]->move(*this);
    }
    //Breed doodlebugs
    for (unsigned int i=0; i<doodlebugs.size(); i++){
        doodlebugs[i]->breed(*this);
    }
    //Remove starved doodlebugs
    for (unsigned int i=0; i<doodlebugs.size(); i++){
        doodlebugs[i]->starve(*this);
    }
    //Breed ants
    for (unsigned int i=0; i<ants.size(); i++){
        ants[i]->breed(*this);
    }

}

int main() {

    srand (time(NULL));
    World world;
    int timeStep = 0;
    string input;

    do {//Print world at given time
        cout<<"World at time "<<timeStep<<":"<<endl;
        world.printWorld();

        //Prompt user for input
        cout << "Press ENTER to continue, or any other character to exit." << endl;
        getline(cin, input);

        //If ENTER has been entered print world at another time step and update timeStep
        if (input.length()==0) {
            world.singleTimeStep();
            timeStep++;
        } else {
            cout<<"You have exited the simulation."<<endl;
            exit(1);
        }

    } while (input.length()==0);

    return 0;
}