#include "levels.hpp"

#include <random>

using namespace std;

namespace nibbles { namespace levels {

namespace {

LevelDefinition correction(
    LevelId id,
    string name,
    vector<Block>& walls,
    vector<Position>& starts
  )
{
  // Adjust for QBASIC's 1-based indexing and score line at
  // the top
  Point offset(1,3);
  for (size_t i=0; i<starts.size(); i++)
  {
    starts[i] -= offset;
  }

  for (size_t i=0; i<walls.size(); i++)
  {
    walls[i] -= offset;
  }

  return LevelDefinition(id, name, 80, 48, walls, starts);
}

// TODO: when DataClass supports it, remove the wrapping of values by
// Direction() and (later) string().
LevelDefinition classicLevel(
  uint32_t whichLevel,
  LevelId levelNumber
)
{
  string name;
  vector<Position> starts(2);
  vector<Block> walls;

  switch (whichLevel)
  {
    case 0:
      name = "Field";
      starts[0] = Position(Point(50, 25), Direction(Direction::right));
      starts[1] = Position(Point(30, 25), Direction(Direction::left));
      break;
    case 1:
      name = "Bar";
      walls.push_back(Block(20, 25, 41, 1));
      starts[0] = Position(Point(60, 7), Direction(Direction::left));
      starts[1] = Position(Point(20, 43), Direction(Direction::right));
      break;
    case 2:
      name = "Legs";
      walls.push_back(Block(20, 10, 1, 31));
      walls.push_back(Block(60, 10, 1, 31));
      starts[0] = Position(Point(50, 25), Direction(Direction::up));
      starts[1] = Position(Point(30, 25), Direction(Direction::down));
      break;
    case 3:
      name = "Propellor";
      // |
      // | -----
      // |
      //      |
      //----- |
      //      |
      walls.push_back(Block(20, 4, 1, 27));
      walls.push_back(Block(60, 23, 1, 27));
      walls.push_back(Block(2, 38, 39, 1));
      walls.push_back(Block(41, 15, 39, 1));
      starts[0] = Position(Point(60, 7), Direction(Direction::left));
      starts[1] = Position(Point(20, 43), Direction(Direction::right));
      break;
    case 4:
      name = "Box";
      walls.push_back(Block(21, 13, 1, 27));
      walls.push_back(Block(59, 13, 1, 27));
      walls.push_back(Block(23, 11, 35, 1));
      walls.push_back(Block(23, 41, 35, 1));
      starts[0] = Position(Point(50, 25), Direction(Direction::up));
      starts[1] = Position(Point(30, 25), Direction(Direction::down));
      break;
    case 5:
      name = "Car park";
      for (int i=10; i<80; i+=10)
      {
        walls.push_back(Block(i, 4, 1, 19));
        walls.push_back(Block(i, 31, 1, 19));
      }
      starts[0] = Position(Point(65, 7), Direction(Direction::down));
      starts[1] = Position(Point(15, 43), Direction(Direction::up));
      break;
    case 6:
      name = "Semi-permeable membrane";
      for (int i=4; i<=49; i+=2)
      {
        walls.push_back(Block(40, i, 1, 1));
      }
      starts[0] = Position(Point(65, 7), Direction(Direction::down));
      starts[1] = Position(Point(15, 43), Direction(Direction::up));
      break;
    case 7:
      name = "Snake";
      for (int i=10; i<=70; i+=20)
        walls.push_back(Block(i, 4, 1, 37));
      for (int i=20; i<=60; i+=20)
        walls.push_back(Block(i, 13, 1, 37));
      starts[0] = Position(Point(65, 7), Direction(Direction::down));
      starts[1] = Position(Point(15, 43), Direction(Direction::up));
      break;
    case 8:
      name = "Slant";
      for (int i=6; i<=47; i++)
      {
        walls.push_back(Block(i, i, 1, 1));
        walls.push_back(Block(i+28, i, 1, 1));
      }
      starts[0] = Position(Point(75, 40), Direction(Direction::up));
      starts[1] = Position(Point(5, 15), Direction(Direction::down));
      break;
    default:
      name = "Diffraction";
      for (int i=4; i<=49; i+=2)
      {
        for (int j=10; j<=70; j+=20)
        {
          walls.push_back(Block(j, i, 1, 1));
        }
        for (int j=20; j<=60; j+=20)
        {
          walls.push_back(Block(j, i+1, 1, 1));
        }
      }
      starts[0] = Position(Point(65, 7), Direction(Direction::down));
      starts[1] = Position(Point(15, 43), Direction(Direction::up));
      break;
  }

  return correction(levelNumber, name, walls, starts);
}

LevelDefinition ultraLevel(LevelId levelNumber)
{
  string name;
  vector<Position> starts(2);
  vector<Block> walls;
  // Standard start locations
  starts[0] = Position(Point(70, 25), Direction(Direction::up));
  starts[1] = Position(Point(10, 25), Direction(Direction::down));

  switch(levelNumber)
  {
    case 0:
      // *empty*
      return classicLevel(0, levelNumber);
    case 1:
      //
      // ---
      //
      return classicLevel(1, levelNumber);
    case 2:
      //
      // | |
      // | |
      //
      return classicLevel(2, levelNumber);
    case 3:
      name = "T";
      //
      // ---
      //  |
      //
      walls.push_back(Block(20, 15, 41, 1));
      walls.push_back(Block(40, 16, 1, 25));
      break;
    case 4:
      name = "Cul-de-sac";
      //
      //   ---
      // |     |
      // |     |
      //
      walls.push_back(Block(20, 10, 41, 1));
      walls.push_back(Block(20, 20, 1, 30));
      walls.push_back(Block(60, 20, 1, 30));
      break;
    case 5:
      name = "Clasp";
      //
      // /- -\   //
      // |   |
      // \- -/
      //
      walls.push_back(Block(21, 11, 1, 31));
      walls.push_back(Block(59, 11, 1, 31));
      walls.push_back(Block(22, 11, 9, 1));
      walls.push_back(Block(22, 41, 9, 1));
      walls.push_back(Block(50, 11, 9, 1));
      walls.push_back(Block(50, 41, 9, 1));
      break;
    case 6:
      // |
      // | -----
      // |
      //      |
      //----- |
      //      |
      return classicLevel(3, levelNumber);
    case 7:
      name = "3 bar";
      //
      // -----
      //
      // -----
      //
      // -----
      //
      walls.push_back(Block(20, 15, 41, 1));
      walls.push_back(Block(20, 25, 41, 1));
      walls.push_back(Block(20, 35, 41, 1));
      starts[0] = Position(Point(60, 7), Direction(Direction::left));
      starts[1] = Position(Point(20, 43), Direction(Direction::right));
      break;
    case 8:
      name = "Fence";
      //
      // | | | |
      // | | | |
      //
      walls.push_back(Block(20, 10, 1, 31));
      walls.push_back(Block(30, 10, 1, 31));
      walls.push_back(Block(50, 10, 1, 31));
      walls.push_back(Block(60, 10, 1, 31));
      break;
    case 9:
      name = "Judgement day";
      // Doubled 'T'
      walls.push_back(Block(20, 15, 41, 1));
      walls.push_back(Block(20, 21, 17, 1));
      walls.push_back(Block(43, 21, 18, 1));
      walls.push_back(Block(36, 22, 1, 19));
      walls.push_back(Block(43, 22, 1, 19));
      break;
    case 10:
      name = "Table";
      //
      //  ------
      //
      //  ------
      //
      // | |  | |
      // | |  | |
      // | |  | |
      walls.push_back(Block(20, 7, 41, 1));
      walls.push_back(Block(20, 13, 41, 1));
      walls.push_back(Block(17, 20, 1, 30));
      walls.push_back(Block(23, 20, 1, 30));
      walls.push_back(Block(57, 20, 1, 30));
      walls.push_back(Block(63, 20, 1, 30));
      break;
    case 11:
      name = "Keep";
      //
      // /--- ---\   //
      // |       |
      // | /- -\ |
      // | |   | |
      // | \- -/ |
      // |       |
      // \--- ---/
      //
      walls.push_back(Block(21, 11, 1, 31));
      walls.push_back(Block(59, 11, 1, 31));
      walls.push_back(Block(25, 15, 1, 23));
      walls.push_back(Block(55, 15, 1, 23));
      walls.push_back(Block(22, 11, 9, 1));
      walls.push_back(Block(22, 41, 9, 1));
      walls.push_back(Block(50, 11, 9, 1));
      walls.push_back(Block(50, 41, 9, 1));
      walls.push_back(Block(26, 15, 5, 1));
      walls.push_back(Block(26, 37, 5, 1));
      walls.push_back(Block(50, 15, 5, 1));
      walls.push_back(Block(50, 37, 5, 1));
      break;
    case 12:
      //
      //  ---
      // |   |
      // |   |
      //  ---
      //
      return classicLevel(4, levelNumber);
    case 13:
      name = "Skew";
      //
      //     --------\   //
      //             |
      //     ------\ |
      // | |       | |
      // | |  ---  | |
      // | |       | |
      // | \------
      // |
      // \--------
      //
      walls.push_back(Block(21, 18, 1, 24));
      walls.push_back(Block(25, 18, 1, 20));
      walls.push_back(Block(55, 15, 1, 20));
      walls.push_back(Block(59, 11, 1, 24));
      walls.push_back(Block(28, 11, 32, 1));
      walls.push_back(Block(28, 15, 27, 1));
      walls.push_back(Block(26, 37, 27, 1));
      walls.push_back(Block(21, 41, 32, 1));
      walls.push_back(Block(30, 26, 21, 1));
      break;
    case 14:
      name = "T-3";
      // Trebled 'T'
      walls.push_back(Block(15, 10, 51, 1));
      walls.push_back(Block(15, 14, 51, 1));
      walls.push_back(Block(15, 18, 22, 1));
      walls.push_back(Block(44, 18, 22, 1));
      walls.push_back(Block(40, 15, 1, 26));
      walls.push_back(Block(36, 19, 1, 22));
      walls.push_back(Block(44, 19, 1, 22));
      break;
    case 15:
      // | | | |
      // | | | |
      //
      // | | | |
      // | | | |
      return classicLevel(5, levelNumber);
    case 16:
      name = "Castle";
      //
      // /---   ---\   //
      // |         |
      // | /-----\ |
      // | |     | |
      // |   | |   |
      // | |     | |
      // | \-----/ |
      // |         |
      // \---   ---/
      //
      walls.push_back(Block(25, 10, 1, 31));
      walls.push_back(Block(55, 10, 1, 31));
      walls.push_back(Block(30, 15, 1, 6));
      walls.push_back(Block(50, 15, 1, 6));
      walls.push_back(Block(30, 30, 1, 6));
      walls.push_back(Block(50, 30, 1, 6));
      walls.push_back(Block(35, 20, 1, 11));
      walls.push_back(Block(45, 20, 1, 11));
      walls.push_back(Block(26, 10, 10, 1));
      walls.push_back(Block(26, 40, 10, 1));
      walls.push_back(Block(45, 10, 10, 1));
      walls.push_back(Block(45, 40, 10, 1));
      walls.push_back(Block(31, 15, 19, 1));
      walls.push_back(Block(31, 35, 19, 1));
      break;
    case 17:
      // Central vertical dotted line
      return classicLevel(6, levelNumber);
    case 18:
      name = "Fortress";
      //
      // | /---   ---\ |
      // | |         | |
      // | | /-----\ | |
      // | | |     | | |
      // | |    -    | |
      // | |   | |   | |
      // | |    -    | |
      // | | |     | | |
      // | | \-----/ | |
      // | |         | |
      // | \---   ---/ |
      //
      walls.push_back(Block(15, 10, 1, 31));
      walls.push_back(Block(25, 10, 1, 31));
      walls.push_back(Block(55, 10, 1, 31));
      walls.push_back(Block(65, 10, 1, 31));
      walls.push_back(Block(30, 15, 1, 6));
      walls.push_back(Block(50, 15, 1, 6));
      walls.push_back(Block(30, 30, 1, 6));
      walls.push_back(Block(50, 30, 1, 6));
      walls.push_back(Block(35, 22, 1, 7));
      walls.push_back(Block(45, 22, 1, 7));
      walls.push_back(Block(26, 10, 10, 1));
      walls.push_back(Block(26, 40, 10, 1));
      walls.push_back(Block(45, 10, 10, 1));
      walls.push_back(Block(45, 40, 10, 1));
      walls.push_back(Block(31, 15, 19, 1));
      walls.push_back(Block(31, 35, 19, 1));
      walls.push_back(Block(37, 20, 7, 1));
      walls.push_back(Block(37, 30, 7, 1));
      break;
    case 19:
      // |   |   |
      // | | | | |
      // | | | | |
      //   |   |
      return classicLevel(7, levelNumber);
    case 20:
      name = "Deception";
      //
      // /---------\ |
      // |         | |
      // | /---\ | | |
      // | |   |   | |
      // | | | \---/ |
      // | |         |
      // | \---------/
      //
      walls.push_back(Block(25, 10, 1, 31));
      walls.push_back(Block(30, 15, 1, 26));
      walls.push_back(Block(35, 20, 1, 16));
      walls.push_back(Block(40, 15, 1, 21));
      walls.push_back(Block(45, 15, 1, 16));
      walls.push_back(Block(50, 10, 1, 26));
      walls.push_back(Block(55, 10, 1, 31));
      walls.push_back(Block(26, 10, 25, 1));
      walls.push_back(Block(31, 15, 9, 1));
      walls.push_back(Block(41, 35, 9, 1));
      walls.push_back(Block(31, 40, 24, 1));
      break;
    case 21:
      //
      // \  \     //
      //  \  \    //
      //   \  \   //
      //
      return classicLevel(8, levelNumber);
    case 22:
      name = "Spill-proof";
      //
      // /-\/-\   //
      // | || |
      // \-  -/
      // /-  -\   //
      // | || |
      // \-/\-/
      //
      walls.push_back(Block(25, 10, 1, 15));
      walls.push_back(Block(55, 10, 1, 15));
      walls.push_back(Block(25, 26, 1, 15));
      walls.push_back(Block(55, 26, 1, 15));
      walls.push_back(Block(25, 10, 15, 1));
      walls.push_back(Block(41, 10, 15, 1));
      walls.push_back(Block(25, 40, 15, 1));
      walls.push_back(Block(41, 40, 15, 1));
      walls.push_back(Block(39, 10, 1, 12));
      walls.push_back(Block(41, 10, 1, 12));
      walls.push_back(Block(39, 29, 1, 12));
      walls.push_back(Block(41, 29, 1, 12));
      walls.push_back(Block(25, 24, 12, 1));
      walls.push_back(Block(44, 24, 12, 1));
      walls.push_back(Block(25, 26, 12, 1));
      walls.push_back(Block(44, 26, 12, 1));
      walls.push_back(Block(39, 24, 3, 3));
      break;
    case 23:
      name = "Sift";
      // Many aligned dotted lines
      for (int i=4; i<50; i+=2)
      {
        for (int j=10; j<=70; j+=10)
        {
          walls.push_back(Block(j, i, 1, 1));
        }
      }
      starts[0] = Position(Point(65, 7), Direction(Direction::down));
      starts[1] = Position(Point(15, 43), Direction(Direction::up));
      break;
    case 24:
      name = "X";
      // X
      for (int i=6; i<=47; i++)
      {
        walls.push_back(Block(i+14,    i, 1, 1));
        walls.push_back(Block(i+14, 53-i, 1, 1));
      }
      starts[0] = Position(Point(75, 40), Direction(Direction::up));
      starts[1] = Position(Point(5, 15), Direction(Direction::down));
      break;
    case 25:
      name = "Crease";
      // X with additions
      for (int i=6; i<=47; i++)
      {
        walls.push_back(Block(i+14,    i, 1, 1));
        walls.push_back(Block(i+14, 53-i, 1, 1));
      }
      for (int i=0; i<=10; i++)
      {
        walls.push_back(Block(30+i,  6+i, 1, 1));
        walls.push_back(Block(51-i,  6+i, 1, 1));
        walls.push_back(Block(20+i, 16+i, 1, 1));
        walls.push_back(Block(20+i, 37-i, 1, 1));
        walls.push_back(Block(61-i, 16+i, 1, 1));
        walls.push_back(Block(61-i, 37-i, 1, 1));
        walls.push_back(Block(30+i, 47-i, 1, 1));
        walls.push_back(Block(51-i, 47-i, 1, 1));
      }
      starts[0] = Position(Point(75, 40), Direction(Direction::up));
      starts[1] = Position(Point(5, 15), Direction(Direction::down));
      break;
    case 26:
      // Many non-aligned dotted lines
      return classicLevel(9, levelNumber);
    case 27:
      name = "Manhattan";
      // Dense grid of dots
      for (int i=5; i<=48; i+=2)
      {
        for (int j=3; j<=78; j+=2)
        {
          walls.push_back(Block(j, i, 1, 1));
        }
      }
      starts[0] = Position(Point(66, 8), Direction(Direction::down));
      starts[1] = Position(Point(14, 42), Direction(Direction::up));
      break;
    case 28:
      name = "Origami";
      // Cramped X with bits
      walls.push_back(Block(2, 4, 16, 46));
      walls.push_back(Block(64, 4, 16, 46));
      for (int i=6; i<=47; i++)
      {
        walls.push_back(Block(i+14,    i, 1, 1));
        walls.push_back(Block(i+14, 53-i, 1, 1));
      }
      for (int i=0; i<=10; i++)
      {
        walls.push_back(Block(30+i,  6+i, 1, 1));
        walls.push_back(Block(51-i,  6+i, 1, 1));
        walls.push_back(Block(20+i, 16+i, 1, 1));
        walls.push_back(Block(20+i, 37-i, 1, 1));
        walls.push_back(Block(61-i, 16+i, 1, 1));
        walls.push_back(Block(61-i, 37-i, 1, 1));
        walls.push_back(Block(30+i, 47-i, 1, 1));
        walls.push_back(Block(51-i, 47-i, 1, 1));
      }
      starts[0] = Position(Point(63, 40), Direction(Direction::up));
      starts[1] = Position(Point(18, 15), Direction(Direction::down));
      break;
    default:
      name = "Chaos "+levelNumber;
      int gap = 40 - levelNumber;
      if (gap < 1)
        gap = 1;
      random_device device;
      mt19937 random(device);
      bernoulli_distribution dist(0.15);

      for (int i=4+gap; i<=49-gap; i++)
      {
        for (int j=2+gap; j<=79-gap; j++)
        {
          if (dist(random))
            walls.push_back(Block(j, i, 1, 1));
        }
      }
      starts[0] = Position(Point(79, 4), Direction(Direction::down));
      starts[1] = Position(Point(2, 49), Direction(Direction::up));
      break;
  }

  return correction(levelNumber, name, walls, starts);
}

} // Anonymous namespace

LevelPack classicLevels()
{
  vector<LevelDefinition> levels;
  for (LevelId i; i<10; i++)
    levels.push_back(classicLevel(i, i));
  return LevelPack(string("classic"), std::move(levels));
}

LevelPack ultraLevels()
{
  vector<LevelDefinition> levels;
  for (LevelId i; i<40; i++)
    levels.push_back(ultraLevel(i));
  return LevelPack(string("ultra"), std::move(levels));
}

}}

