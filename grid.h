#ifndef __GRID_H__
#define __GRID_H__

#include "raylib.h"
#include <iostream>
#include <vector>
#include "color.h"

class Grid
{
public:
  Grid();
  void initGrid();
  void printGrid();
  void Draw();
  int grid[20][10];

public:
  int rowNum;
  int colNum;
  int cellSize;
  std::vector<Color> colors;
};

#endif //__GRID_H__