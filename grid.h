#ifndef __GRID_H__
#define __GRID_H__

#include "raylib.h"
#include <iostream>
#include <vector>

class Grid
{
public:
  Grid();
  void initGrid();
  void printGrid();
  void Draw();
  int grid[20][10];

public:
  std::vector<Color> getCellColor();
  int rowNum;
  int colNum;
  int cellSize;
  std::vector<Color> Colors;
};

#endif //__GRID_H__