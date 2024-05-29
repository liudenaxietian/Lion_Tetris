#ifndef __GAME_H__
#define __GAME_H__

#include "grid.h"
#include "blocks.cpp"

class Game
{
public:
  Game();
  ~Game();
  bool gameOver;
  void Draw();

private:
  Block GetRandomBlock();
  std::vector<Block> GetAllBlocks();
  std::vector<Block> blocks;
  Block currentBlock;
  Grid grid;
  Block block;
};

#endif // __GAME_H__
