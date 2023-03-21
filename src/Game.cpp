#include "Game.h"

Game::Game()
{

    class Snowball{
    public:
        bool isShooting;
        glm::vec4 position;
        glm::vec4 direction;
        float speed;
        snowball(glm::vec4 position, glm::vec4 direction)
        {
            isShooting = false;
            speed = 10.0f;
            this->direction = direction;
            this->position = position;
        }
        void ballMove(float deltat){
            this->position += this->direction * this->speed * deltat;
        }
        void shoot(){
            this->isShooting = true;
        }
        void setPosDir(glm::vec4 position, glm::vec4 direction){
            this->position = position;
            this->direction = direction;
        }
        void collide(){
            this->isShooting = false;
        }
    };

}

Game::~Game()
{
    //dtor
}
