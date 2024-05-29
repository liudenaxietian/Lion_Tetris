#include "game.h"

Game::Game()
{
  grid = Grid();
  blocks = GetAllBlocks();
  currentBlock = GetRandomBlock();
}

Game::~Game(){};

std::vector<Block> Game::GetAllBlocks()
{
  return {IBlock(), JBlock(), LBlock(), OBlock(), SBlock(), TBlock(), ZBlock()};
}

Block Game::GetRandomBlock()
{
  if (blocks.empty())
  {
    blocks = GetAllBlocks();
  }
  int randomIndex = rand() % blocks.size();
  Block block = blocks[randomIndex];
  blocks.erase(blocks.begin() + randomIndex);
  return block;
}

void Game::Draw()
{
  grid.Draw();
  currentBlock.Draw(1, 1);
}