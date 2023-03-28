#ifndef COLLISIONS_H_INCLUDED
#define COLLISIONS_H_INCLUDED
#include "matrices.h"

bool checkCollisionBallsMap(glm::vec4 position1);
bool checkCollisionBallEnemie(glm::vec4 position1, glm::vec4 position2);
bool checkCollisionPlayerEnemie(glm::vec4 position1, glm::vec4 position2);
#endif // COLLISIONS_H_INCLUDED
