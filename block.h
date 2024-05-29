#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "position.h"
#include "color.h"
#include <map>
#include <vector>

// 基类
class Block
{
public:
  int id;
  std::map<int, std::vector<Position>> cells;
  Block();
  std::vector<Position> GetCellPositions();
  void Draw(int offsetX, int offsetY);
  void Rotate();
  void Move(int row, int col);

private:
  int cellSize;
  int rotateState;
  std::vector<Color> colors;
  int rowOffset;
  int colOffset;
};

#endif // __BLOCK_H__