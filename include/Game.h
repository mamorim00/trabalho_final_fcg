#ifndef GAME_H
#define GAME_H

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<vector>


class Game
{
    public:
        Game();
        virtual ~Game();

        //snowball
        void createSnowball();

    protected:

    private:
         std::vector<Snowball> snowballs;
};

class Game::Snowball{
        public:
        bool isShooting;
        glm::vec4 position;
        glm::vec4 direction;
        float speed;
        snowball(glm::vec4 position, glm::vec4 direction);
        void ballMove(float deltat);
        void shoot();
        void setPosDir(glm::vec4 position, glm::vec4 direction);
        void collide();
};


#endif // GAME_H
