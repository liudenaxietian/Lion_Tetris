#include "grid.h"

Grid::Grid()
{
  rowNum = 10;
  colNum = 20;
  cellSize = 30;
  initGrid();
  printGrid();
  Colors = getCellColor();
}

std::vector<Color> Grid::getCellColor()
{
  Color darkGrey = Color{26, 31, 40, 255};
  Color green = Color{47, 230, 23, 255};
  Color red = Color{232, 18, 18, 255};
  Color orange = Color{226, 116, 17, 255};
  Color yellow = Color{237, 234, 4, 255};
  Color purple = Color{166, 0, 247, 255};
  Color cyan = Color{21, 204, 209, 255};
  Color blue = Color{13, 64, 216, 255};
  Color lightBlue = Color{59, 85, 162, 255};
  Color darkBlue = Color{44, 44, 127, 255};

  return {darkGrey, green, red, orange, yellow, purple, cyan, blue, lightBlue, darkBlue};
}

void Grid::initGrid()
{
  for (int i = 0; i < rowNum; i++)
  {
    for (int j = 0; j < colNum; j++)
    {
      grid[i][j] = 0;
    }
  }
}

void Grid::printGrid()
{
  for (int i = 0; i < rowNum; i++)
  {
    for (int j = 0; j < colNum; j++)
    {
      std::cout << grid[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

void Grid::Draw()
{
  Color darkGrey = Color{26, 31, 40, 255};
  for (int i = 0; i < rowNum; i++)
  {
    for (int j = 0; j < colNum; j++)
    {
      DrawRectangle(i * cellSize + 1, j * cellSize + 1, cellSize - 1, cellSize - 1, darkGrey);
    }
  }
}