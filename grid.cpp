#include "grid.h"

Grid::Grid()
{
  rowNum = 10;
  colNum = 20;
  cellSize = 30;
  initGrid();
  printGrid();
  colors = GetCellColors();
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
  for (int i = 0; i < rowNum; i++)
  {
    for (int j = 0; j < colNum; j++)
    {
      int cellValue = grid[i][j];
      DrawRectangle(i * cellSize + 1, j * cellSize + 1, cellSize - 1, cellSize - 1, colors[cellValue]);
    }
  }
}