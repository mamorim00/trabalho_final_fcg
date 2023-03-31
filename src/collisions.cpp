#define ceiling 10
#define end_map 10
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glm/vec4.hpp>

//Ball plane collision
 bool checkCollisionBallsMap(glm::vec4 position1){

        //collision snowballs/floor/ceiling
        float radius = 1;
            if(position1.y + radius <= 0 ||
               position1.y + radius >= ceiling ){
                return true;
            }
            //collision snowballs/endMap
            if(position1.x + radius >= end_map ||
               position1.x + radius <= -end_map ||
               position1.x + radius >= end_map ||
               position1.x + radius <= -end_map ){
                return true;
               }
 }

//Para colis�es entre os pinguins, considerando apenas o jogador com os inimigos
bool checkCollisionBoxBox(glm::vec4 box1, glm::vec4 box2){
     float dx = std::abs(box1.x - box2.x);
    float dy = std::abs(box1.y - box2.y);
    float dz = std::abs(box1.z - box2.z);
    // calcula as somas das metades das larguras, alturas e profundidades das caixas, considerando altura de 2 unidades e largura 0.01 unidade
    float rw = (0.75 + 0.75) / 2;
    float rh = (2 + 2) / 2;
    float rd = (0.75 + 0.75) / 2;

    // verifica se as dist�ncias nas tr�s dimens�es est�o dentro das somas das metades das larguras, alturas e profundidades das caixas
    if (dx <= rw && dy <= rh && dz <= rd) {
        return true; // houve colis�o
    }
    else {
        return false; // n�o houve colis�o
    }

}

bool checkCollisionBoxBall(glm::vec4 b, glm::vec4 s){
    float width = 1, height = 1.25, depth = 1;
    float radius = 1;
     // calcula a dist�ncia entre a posi��o central da bola e a posi��o central da caixa nas tr�s dimens�es
    float dx = std::abs(s.x - b.x);
    float dy = std::abs(s.y - b.y);
    float dz = std::abs(s.z - b.z);


    // verifica se a bola est� mais perto da caixa do que a metade da largura, altura e profundidade da caixa
    if ((dx <= width / 2)
        && (dy <= height / 2)
        && (dz <= depth / 2)){
        return true;}
    else
        return false;


    // verifica se a bola est� colidindo com um canto da caixa
    float cornerDistance_sq = pow(dx - width / 2, 2) + pow(dy - height / 2, 2) + pow(dz - depth / 2, 2);
    return cornerDistance_sq <= pow(radius, 2);
}


