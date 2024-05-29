#include "color.h"

const Color darkGrey = Color{26, 31, 40, 255};
const Color green = Color{47, 230, 23, 255};
const Color red = Color{232, 18, 18, 255};
const Color orange = Color{226, 116, 17, 255};
const Color yellow = Color{237, 234, 4, 255};
const Color purple = Color{166, 0, 247, 255};
const Color cyan = Color{21, 204, 209, 255};
const Color blue = Color{13, 64, 216, 255};
const Color lightBlue = Color{59, 85, 162, 255};
const Color darkBlue = Color{44, 44, 127, 255};

std::vector<Color> GetCellColors()
{
  return {darkGrey, green, red, orange, yellow, purple, cyan, blue, lightBlue, darkBlue};
}