#include "block.h"

Block::Block()
{
  id = 1;
  cellSize = 30;
  rotateState = 0;
  colors = GetCellColors();
  rowOffset = 0;
  colOffset = 0;
}

std::vector<Position> Block::GetCellPositions()
{
  std::vector<Position> tiles = cells[rotateState];
  std::vector<Position> movedTiles;
  for (auto item : tiles)
  {
    auto newPos = Position{item.row + rowOffset, item.col + colOffset};
    movedTiles.push_back(newPos);
  }

  return movedTiles;
}

void Block::Move(int row, int col)
{
  rowOffset += row;
  colOffset += col;
}

void Block::Draw(int offsetX, int offsetY)
{
  std::vector<Position> tiles = GetCellPositions();
  for (auto item : tiles)
  {
    DrawRectangle(item.row * cellSize + offsetX, item.col * cellSize + offsetY, cellSize - 1, cellSize - 1, colors[id]);
  };
}

void Block::Rotate()
{
  ++rotateState;
  if (rotateState == (int)cells.size())
  {
    rotateState = 0;
  }
}