#define ceiling 10
#define end_map 10
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include<iostream>
#include <glm/vec4.hpp>
 bool checkCollisionBallsMap(glm::vec4 position1){

        //collision snowballs/floor/ceiling

            if(position1.y <= -0.6f ||
               position1.y >= ceiling ){
                return true;
            }
            //collision snowballs/endMap
            if(position1.x >= end_map ||
               position1.x <= -end_map ||
               position1.x >= end_map ||
               position1.x <= -end_map ){
                return true;
               }
 }
 bool checkCollisionBallEnemie(glm::vec4 position1, glm::vec4 position2){
            //collision snowballs/enemies

                    if(fabs(position1.x - position2.x) < 0.1f)
                    {
                        if(fabs(position1.y - position2.y) < 0.5f)
                        {
                            if(fabs(position1.z - position2.z) < 0.1f){
                                return true;
                            }

                        }

                    }
    }
        //collision player/enemies
bool checkCollisionPlayerEnemie(glm::vec4 position1, glm::vec4 position2){

                    if(fabs(position1.x - position2.x) < 0.5f)
                    {
                        if(fabs(position1.y - position2.y) < 0.5f)
                        {
                            if(fabs(position1.z - position2.z) < 0.5f){
                                return true;
                            }

                        }

                    }

}
