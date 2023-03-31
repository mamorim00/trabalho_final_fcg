#ifndef COLLISIONS_H_INCLUDED
#define COLLISIONS_H_INCLUDED
#include "matrices.h"

bool checkCollisionBallsMap(glm::vec4 position1);
bool checkCollisionBoxBox(glm::vec4 box1, glm::vec4 box2);
bool checkCollisionBoxBall(glm::vec4 box, glm::vec4 ball);
#endif // COLLISIONS_H_INCLUDED
