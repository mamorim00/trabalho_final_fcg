//     Universidade Federal do Rio Grande do Sul
//             Instituto de Inform�tica
//       Departamento de Inform�tica Aplicada
//
//    INF01047 Fundamentos de Computa��o Gr�fica
//               Prof. Eduardo Gastal
//
//                   LABORAT�RIO 2
//

// Arquivos "headers" padr�es de C podem ser inclu�dos em um
// programa C++, sendo necess�rio somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include<iostream>
//biblioteca para simplificar o uso dos arrays de classes, para as bolas de neve
#include<vector>

// Headers abaixo s�o espec�ficos de C++
#include <map>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
#include <ctime>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Cria��o de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Cria��o de janelas do sistema operacional

// Headers da biblioteca GLM: cria��o de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"

#define end_map 10
#define ceiling 10

// Declara��o de v�rias fun��es utilizadas em main().  Essas est�o definidas
// logo ap�s a defini��o de main() neste arquivo.
GLuint BuildTriangles(); // Constr�i tri�ngulos para renderiza��o
void LoadShadersFromFiles(); // Carrega os shaders de v�rtice e fragmento, criando um programa de GPU
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Fun��o utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU

// Declara��o de fun��es auxiliares para renderizar texto dentro da janela
// OpenGL. Estas fun��es est�o definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Fun��es abaixo renderizam como texto na janela OpenGL algumas matrizes e
// outras informa��es do programa. Definidas ap�s main().
void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Fun��es callback para comunica��o com o sistema operacional e intera��o do
// usu�rio. Veja mais coment�rios nas defini��es das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Definimos uma estrutura que armazenar� dados necess�rios para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    const char*  name;        // Nome do objeto
    void*        first_index; // �ndice do primeiro v�rtice dentro do vetor indices[] definido em BuildTriangles()
    int          num_indices; // N�mero de �ndices do objeto dentro do vetor indices[] definido em BuildTriangles()
    GLenum       rendering_mode; // Modo de rasteriza��o (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
};

// Abaixo definimos vari�veis globais utilizadas em v�rias fun��es do c�digo.

// A cena virtual � uma lista de objetos nomeados, guardados em um dicion�rio
// (map).  Veja dentro da fun��o BuildTriangles() como que s�o inclu�dos
// objetos dentro da vari�vel g_VirtualScene, e veja na fun��o main() como
// estes s�o acessados.
std::map<const char*, SceneObject> g_VirtualScene;

// Raz�o de propor��o da janela (largura/altura). Veja fun��o FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// �ngulos de Euler que controlam a rota��o de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usu�rio est� com o bot�o esquerdo do mouse
// pressionado no momento atual. Veja fun��o MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false;


// Vari�veis que definem a c�mera em coordenadas esf�ricas, controladas pelo
// usu�rio atrav�s do mouse (veja fun��o CursorPosCallback()). A posi��o
// efetiva da c�mera � calculada dentro da fun��o main(), dentro do loop de
// renderiza��o.
float g_CameraTheta = 0.0f; // �ngulo no plano ZX em rela��o ao eixo Z
float g_CameraPhi = 0.0f;   // �ngulo em rela��o ao eixo Y
float g_CameraDistance = 2.5f; // Dist�ncia da c�mera para a origem


double g_LastCursorPosX;
double g_LastCursorPosY;

float g_Theta = 3.141592f / 4;
float g_Phi = 3.141592f / 6;

//Booleanas para saber quando a tecla esta precionada para fazer o movimento da camera
bool tecla_A_pressionada = false;
bool tecla_S_pressionada = false;
bool tecla_D_pressionada = false;
bool tecla_W_pressionada = false;
bool tecla_L_pressionada = false;

float delta_t = 0.0f;


// Vari�vel que controla o tipo de proje��o utilizada: perspectiva ou ortogr�fica.
bool g_UsePerspectiveProjection = true;

// Vari�vel que controla se o texto informativo ser� mostrado na tela.
bool g_ShowInfoText = true;

// Vari�veis que definem um programa de GPU (shaders). Veja fun��o LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;




class snowball{
public:
    bool isShooting;
    glm::vec4 position;
    glm::vec4 direction;
    float speed;
    glm::vec4 startPoint;
    glm::vec4 endPoint ;
    glm::vec4 controlPoint1;
    glm::vec4 controlPoint2;
    float timeOnCurve;
    float totalTimeOnCurve = 10.0f;
    snowball(glm::vec4 position, glm::vec4 direction)
    {
        isShooting = false;
        speed = 10.0f;
        this->direction = direction;
        this->position = position;
        timeOnCurve = 0.0f;
        totalTimeOnCurve = 1.0f;
    }

    void ballMove(float deltat){
        if (isShooting) {
            // Use cubic Bézier curve to move snowball
            glm::vec4 curvePos = calculateBezierCurvePosition();
            this->position = curvePos;
            this->timeOnCurve += deltat;
            if (timeOnCurve >= totalTimeOnCurve) {
                // Snowball has reached the end of the curve
                isShooting = false;
            }
        } else {
            this->position += this->direction * this->speed * deltat;
        }
    }

    void shoot(glm::vec4 startPoint, glm::vec4 endPoint, glm::vec4 controlPoint1, glm::vec4 controlPoint2, float totalTimeOnCurve){
        this->isShooting = true;
        this->startPoint = startPoint;
        this->endPoint = endPoint;
        this->controlPoint1 = controlPoint1;
        this->controlPoint2 = controlPoint2;
        this->totalTimeOnCurve = totalTimeOnCurve;
        this->timeOnCurve = 0.0f;
    }

    void setPosDir(glm::vec4 position, glm::vec4 direction){
        this->position = position;
        this->direction = direction;
    }

    void collide(){
        this->isShooting = false;
    }

private:
    glm::vec4 calculateBezierCurvePosition() {
        // Calculate position on cubic Bézier curve based on current time on curve
        float t = timeOnCurve / totalTimeOnCurve;
        float oneMinusT = 1.0f - t;
        glm::vec4 curvePos = startPoint * oneMinusT * oneMinusT * oneMinusT +
                             3.0f * controlPoint1 * oneMinusT * oneMinusT * t +
                             3.0f * controlPoint2 * oneMinusT * t * t +
                             endPoint * t * t * t;
        return curvePos;
    }
};




class enemy{
    public:
        bool isAlive = false;
        float speed;
        int hp;
        glm::vec4 position;
        glm::vec4 direction;
        enemy(glm::vec4 position, glm::vec4 playerPos){
            this->position = position;
            this->direction = playerPos - position;
            this->hp = 3;
            this->speed = 0.1f;
            this->isAlive = true;
        }
        void enemyMove(float delta_t, glm::vec4 playerPos){
            this->direction = playerPos - this->position;
            this->position += this->direction * this->speed * delta_t;
        }
        void kill(){
            this->isAlive = false;
        }
        void takeDamage(){
            this->hp --;
            std::cout << this->hp << std::endl;
        }
    };
class game{
public:

    int points;

    game(){
        this->points = 0;
    }
    void checkHp(std::vector<enemy> &enemies){
        for(size_t j = 0; j < enemies.size(); j ++){
            if(enemies[j].hp <= 0){
                enemies[j].kill();
                enemies.erase(enemies.begin()+j);
            }
        }

    }
    void checkCollision(std::vector<snowball> &snowballs, std::vector<enemy> &enemies){

        //collision snowballs/floor/ceiling
        for(size_t j = 0; j < snowballs.size(); j ++){
            if(snowballs[j].position.y <= -0.6f ||
               snowballs[j].position.y >= ceiling ){
                snowballs[j].collide();
                snowballs.erase(snowballs.begin() + j);
            }
            //collision snowballs/endMap
            if(snowballs[j].position.x >= end_map ||
               snowballs[j].position.x <= -end_map ||
               snowballs[j].position.z >= end_map ||
               snowballs[j].position.z <= -end_map ){
                    snowballs[j].collide();
                    snowballs.erase(snowballs.begin() + j);
               }
            //collision snowballs/enemies
            for(size_t i = 0; i < enemies.size(); i++){
                if(enemies[i].isAlive){
                    if(fabs(snowballs[j].position.x - enemies[i].position.x) < 0.1f)
                    {
                        if(fabs(snowballs[j].position.y - enemies[i].position.y) < 0.1f)
                        {
                            if(fabs(snowballs[j].position.z - enemies[i].position.z) < 0.1f){
                                snowballs[j].collide();
                                snowballs.erase(snowballs.begin() + j);
                                enemies[i].takeDamage();
                            }

                        }

                    }
                }
            }
        }
        //collision player/enemies

    }


};
class player{
public:
    float speed;
    int hp;
    glm::vec4 position;
    player(glm::vec4 position){
        this->position = position;
        this->hp = 5;
        this->speed = 0.5f;
    }

};

int main()
{
    //##inicia o gerador de numeros aleat�rios
    srand(time(0));
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impress�o de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL vers�o 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto �, utilizaremos somente as
    // fun��es modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 800 linhas
    // de pixels, e com t�tulo "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 800, "INF01047 - Trabalho Final", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }
    //Setar as posi��es do cursor
    glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);

    // Definimos a fun��o de callback que ser� chamada sempre que o usu�rio
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);


    //velocidade da camera e tempo inicial
    float prev_time = 0.0f;
    float speed = 0.5f;
    // ... ou clicar os bot�es do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
//    glfwSetScrollCallback(window, ScrollCallback);

    // Definimos a fun��o de callback que ser� chamada sempre que a janela for
    // redimensionada, por consequ�ncia alterando o tamanho do "framebuffer"
    // (regi�o de mem�ria onde s�o armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, 800, 800); // For�amos a chamada do callback acima, para definir g_ScreenRatio.

    // Indicamos que as chamadas OpenGL dever�o renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas fun��es definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Imprimimos no terminal informa��es sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de v�rtices e de fragmentos que ser�o utilizados
    // para renderiza��o. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();
    // Constru�mos a representa��o de um tri�ngulo
    GLuint vertex_array_object_id = BuildTriangles();

    // Inicializamos o c�digo para renderiza��o de texto.
    TextRendering_Init();

    // Buscamos o endere�o das vari�veis definidas dentro do Vertex Shader.
    // Utilizaremos estas vari�veis para enviar dados para a placa de v�deo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    GLint model_uniform           = glGetUniformLocation(g_GpuProgramID, "model"); // Vari�vel da matriz "model"
    GLint view_uniform            = glGetUniformLocation(g_GpuProgramID, "view"); // Vari�vel da matriz "view" em shader_vertex.glsl
    GLint projection_uniform      = glGetUniformLocation(g_GpuProgramID, "projection"); // Vari�vel da matriz "projection" em shader_vertex.glsl
    GLint render_as_black_uniform = glGetUniformLocation(g_GpuProgramID, "render_as_black"); // Vari�vel booleana em shader_vertex.glsl

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Vari�veis auxiliares utilizadas para chamada � fun��o
    // TextRendering_ShowModelViewProjection(), armazenando matrizes 4x4.
    glm::mat4 the_projection;
    glm::mat4 the_model;
    glm::mat4 the_view;

    glm::vec4 camera_position_c  = glm::vec4(3.0f,0.0f,3.0f,1.0f); // Ponto "c", centro da c�mera


    bool holding = false;
    std::vector<snowball> snowballs;
    std::vector <enemy> enemies;
    float enemySpawnTimerMax = 500.0f;
    float enemySpawnTimer = enemySpawnTimerMax;
    int maxEnemies = 5;
    game *jogo = new game();
    //snowball *bola = new snowball(glm::vec4(0.0f,0.0f,0.0f,0.0f), glm::vec4(0.0f,0.0f,0.0f,0.0f));



    // Ficamos em um loop infinito, renderizando, at� que o usu�rio feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as opera��es de renderiza��o

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor �
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto �:
        // Vermelho, Verde, Azul, Alpha (valor de transpar�ncia).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Ilumina��o.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e tamb�m resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de v�rtice e fragmentos).
        glUseProgram(g_GpuProgramID);

        // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
        // v�rtices apontados pelo VAO criado pela fun��o BuildTriangles(). Veja
        // coment�rios detalhados dentro da defini��o de BuildTriangles().
        glBindVertexArray(vertex_array_object_id);





        // Computamos a posi��o da c�mera utilizando coordenadas esf�ricas.  As
        // vari�veis g_CameraDistance, g_CameraPhi, e g_CameraTheta s�o
        // controladas pelo mouse do usu�rio. Veja as fun��es CursorPosCallback()
        // e ScrollCallback().
        //float r = g_CameraDistance;
        float y = sin(g_Phi);
        float z = cos(g_Phi)*cos(g_Theta);
        float x = cos(g_Phi)*sin(g_Theta);


        // Abaixo definimos as var�veis que efetivamente definem a c�mera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.

        //glm::vec4 camera_lookat_l    = glm::vec4(x,y,z,1.0f); // Ponto "l", para onde a c�mera (look-at) estar� sempre olhando
        glm::vec4 camera_view_vector = glm::vec4(-x,-y,-z,0.0f); // Vetor "view", sentido para onde a c�mera est� virada
        glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "c�u" (eito Y global)


        glm::vec4 vector_u = crossproduct(camera_view_vector, camera_up_vector);
          //Calculo do delta_t
        float current_time = (float)glfwGetTime();
        delta_t = current_time - prev_time;
        prev_time = current_time;
        //#########
        //usando um vetor para zerar a posi��o y, mantendo o jogador sempre no y = 0
        glm::vec4 floor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);


        //Calculo da movimenta��o da camera livre
        if(tecla_A_pressionada)
            camera_position_c +=  -vector_u * speed * delta_t *floor;
        if(tecla_W_pressionada)
            camera_position_c +=  +camera_view_vector * speed * delta_t *floor;
        if(tecla_S_pressionada)
            camera_position_c +=   -camera_view_vector * speed * delta_t * floor;
        if(tecla_D_pressionada)
            camera_position_c +=   vector_u * speed * delta_t * floor;


        //bola de neve ######################################
        jogo->checkCollision(snowballs, enemies);
        jogo->checkHp(enemies);
        //spawn do inimigo
        if(enemies.size() < maxEnemies){
            if(enemySpawnTimer >= enemySpawnTimerMax){
                //isso pode ser uma fun��o
                int x = 5.0 + static_cast <float> (rand()) / ( static_cast <float> (RAND_MAX/(5.0-7.0))),
                    y = 0.0f,
                    z = 5.0 + static_cast <float> (rand()) / ( static_cast <float> (RAND_MAX/(5.0-7.0)));
                enemy *newEnemie = new enemy(glm::vec4(x,y,z,0.0f), glm::vec4(-1.0f,0.0f,-1.0f,0.0f));
                enemies.push_back(*newEnemie);

                enemySpawnTimer = 0.0f;
            }
            else
                enemySpawnTimer += 1.0f;
        }
        //for(size_t j = 0; j < enemies.size(); j++){
            //enemies[j].enemyMove(delta_t, camera_position_c);
        //}


        if(tecla_L_pressionada){
            //calculando a posição em que o jogador segura a bola, depende de onde o jogador está e para onde está olhando
            glm::vec4 norm_view = glm::vec4((camera_view_vector/norm(camera_view_vector)).x, (camera_view_vector/norm(camera_view_vector)).y,(camera_view_vector/norm(camera_view_vector)).z, 0.0f);
            glm::vec4 hold = glm::vec4(camera_position_c.x + 0.3f * norm_view.x, camera_position_c.y - 0.05f, camera_position_c.z + 0.3f * norm_view.z, 0.0f);
            //caso não esteja segurando nenhuma bola, cria uma nova na posição em que o jogador a segura
            if(!holding){
                snowball *newBall = new snowball(hold, camera_view_vector);
                snowballs.push_back(*newBall);
            }
            //caso já esteja segurando a bola, apenas realoca para a nova posição para o caso de a câmera ter sido alterada
            snowballs[snowballs.size() - 1].setPosDir(hold, camera_view_vector);
            //para sabermos que já existe uma bola sendo segurada
            holding = true;
        }
        else{
            //bola atirada
            if(snowballs.size() > 0 && holding){
                glm::vec4 norm_view = glm::vec4((camera_view_vector/norm(camera_view_vector)).x, (camera_view_vector/norm(camera_view_vector)).y,(camera_view_vector/norm(camera_view_vector)).z, 0.0f);
                glm::vec4 hold = snowballs[snowballs.size() - 1].position;
                glm::vec4 target = glm::vec4(hold.x + camera_view_vector.x * 5.0f, hold.y + camera_view_vector.y * 5.0f, hold.z + camera_view_vector.z * 5.0f, 0.0f);
                glm::vec4 control_point1 = hold + 0.3f * (target - hold);
                glm::vec4 control_point2 = target - 0.3f * (target - hold);
                snowballs[snowballs.size() - 1].shoot(hold,control_point1, control_point2, target, 5.0);
            }
            holding = false;
        }
        //movimenta todas as bolas
        if(snowballs.size() > 0){
            //movemos todas as bolas com o tempo delta_t
            for(size_t j = 0; j < snowballs.size();j++){
                if(snowballs[j].isShooting)
                    snowballs[j].ballMove(delta_t);
            }
        }

        // Computamos a matriz "View" utilizando os par�metros da c�mera para
        // definir o sistema de coordenadas da c�mera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        // Agora computamos a matriz de Proje��o.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da c�mera, os planos near e far
        // est�o no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posi��o do "near plane"
        float farplane  = -10.0f; // Posi��o do "far plane"

        if (g_UsePerspectiveProjection)
        {
            // Proje��o Perspectiva.
            // Para defini��o do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
            float field_of_view = 3.141592 / 3.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            // Proje��o Ortogr�fica.
            // Para defini��o dos valores l, r, b, t ("left", "right", "bottom", "top"),
            // PARA PROJE��O ORTOGR�FICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
            // Para simular um "zoom" ortogr�fico, computamos o valor de "t"
            // utilizando a vari�vel g_CameraDistance.
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        // Enviamos as matrizes "view" e "projection" para a placa de v�deo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas s�o
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));


        // Vamos desenhar 3 inst�ncias (c�pias) do cubo
        for (int i = 1; i <= 6; ++i)
        {
            // Cada c�pia do cubo possui uma matriz de modelagem independente,
            // j� que cada c�pia estar� em uma posi��o (rota��o, escala, ...)
            // diferente em rela��o ao espa�o global (World Coordinates). Veja
            // slides 2-14 e 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
            glm::mat4 model;

            if (i == 1)
            {
                // A primeira c�pia do cubo n�o sofrer� nenhuma transforma��o
                // de modelagem. Portanto, sua matriz "model" � a identidade, e
                // suas coordenadas no espa�o global (World Coordinates) ser�o
                // *exatamente iguais* a suas coordenadas no espa�o do modelo
                // (Model Coordinates).
                model = Matrix_Identity();
            }
            else if ( i == 2 )
            {
                // A segunda c�pia do cubo sofrer� um escalamento n�o-uniforme,
                // seguido de uma rota��o no eixo (1,1,1), e uma transla��o em Z (nessa ordem!).
                model = Matrix_Translate(0.0f, 0.0f, -2.0f) // TERCEIRO transla��o
                      * Matrix_Rotate(3.141592f / 8.0f, glm::vec4(1.0f,1.0f,1.0f,0.0f)) // SEGUNDO rota��o
                      * Matrix_Scale(2.0f, 0.5f, 0.5f); // PRIMEIRO escala
            }
            else if ( i == 3 )
            {
                // A terceira c�pia do cubo sofrer� rota��es em X,Y e Z (nessa
                // ordem) seguindo o sistema de �ngulos de Euler, e ap�s uma
                // transla��o em X. Veja slides 106-107 do documento Aula_07_Transformacoes_Geometricas_3D.pdf.
                model = Matrix_Translate(-2.0f, 0.0f, 0.0f) // QUARTO transla��o
                      * Matrix_Rotate_Z(g_AngleZ)  // TERCEIRO rota��o Z de Euler
                      * Matrix_Rotate_Y(g_AngleY)  // SEGUNDO rota��o Y de Euler
                      * Matrix_Rotate_X(g_AngleX); // PRIMEIRO rota��o X de Euler

                // Armazenamos as matrizes model, view, e projection do terceiro cubo
                // para mostrar elas na tela atrav�s da fun��o TextRendering_ShowModelViewProjection().
                the_model = model;
                the_projection = projection;
                the_view = view;
            }

            else if(i == 4)
            {
                for(size_t j = 0; j < snowballs.size();j++){
                    //if(snowballs[j].isShooting)
                        model = Matrix_Translate(snowballs[j].position.x,snowballs[j].position.y,snowballs[j].position.z) * Matrix_Scale(0.2f, 0.2f, 0.2f);
                            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                            glUniform1i(render_as_black_uniform, false);
                            glDrawElements(
                            g_VirtualScene["snowballs"].rendering_mode, // Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf
                            g_VirtualScene["snowballs"].num_indices,
                            GL_UNSIGNED_INT,
                            (void*)g_VirtualScene["snowballs"].first_index
                        );
                }

            }
            //####ch�o
            else if(i == 5)
            {
                model = Matrix_Translate(0.0f, -0.8f, 0.0f) * Matrix_Scale(20.0f, 0.2f, 20.0f) * Matrix_Identity();

            }
            else if(i == 6){
                for(size_t j = 0; j < enemies.size();j++){
                        model = Matrix_Translate(enemies[j].position.x,enemies[j].position.y,enemies[j].position.z) * Matrix_Scale(0.2f, 0.2f, 0.2f);
                            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                            glUniform1i(render_as_black_uniform, false);
                            glDrawElements(
                            g_VirtualScene["enemy"].rendering_mode, // Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf
                            g_VirtualScene["enemy"].num_indices,
                            GL_UNSIGNED_INT,
                            (void*)g_VirtualScene["enemy"].first_index
                        );
                }
            }
            // Enviamos a matriz "model" para a placa de v�deo (GPU). Veja o
            // arquivo "shader_vertex.glsl", onde esta � efetivamente
            // aplicada em todos os pontos.
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

            // Informamos para a placa de v�deo (GPU) que a vari�vel booleana
            // "render_as_black" deve ser colocada como "false". Veja o arquivo
            // "shader_vertex.glsl".
            glUniform1i(render_as_black_uniform, false);

            // Pedimos para a GPU rasterizar os v�rtices do cubo apontados pelo
            // VAO como tri�ngulos, formando as faces do cubo. Esta
            // renderiza��o ir� executar o Vertex Shader definido no arquivo
            // "shader_vertex.glsl", e o mesmo ir� utilizar as matrizes
            // "model", "view" e "projection" definidas acima e j� enviadas
            // para a placa de v�deo (GPU).
            //
            // Veja a defini��o de g_VirtualScene["cube_faces"] dentro da
            // fun��o BuildTriangles(), e veja a documenta��o da fun��o
            // glDrawElements() em http://docs.gl/gl3/glDrawElements.
            glDrawElements(
                g_VirtualScene["cube_faces"].rendering_mode, // Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf
                g_VirtualScene["cube_faces"].num_indices,
                GL_UNSIGNED_INT,
                (void*)g_VirtualScene["cube_faces"].first_index
            );

            glDrawElements(
                g_VirtualScene["floor"].rendering_mode, // Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf
                g_VirtualScene["floor"].num_indices,
                GL_UNSIGNED_INT,
                (void*)g_VirtualScene["floor"].first_index
            );
           /* glDrawElements(
                g_VirtualScene["snowballs"].rendering_mode, // Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf
                g_VirtualScene["snowballs"].num_indices,
                GL_UNSIGNED_INT,
                (void*)g_VirtualScene["snowballs"].first_index
            );*/
            // Pedimos para OpenGL desenhar linhas com largura de 4 pixels.
            glLineWidth(4.0f);

            // Pedimos para a GPU rasterizar os v�rtices dos eixos XYZ
            // apontados pelo VAO como linhas. Veja a defini��o de
            // g_VirtualScene["axes"] dentro da fun��o BuildTriangles(), e veja
            // a documenta��o da fun��o glDrawElements() em
            // http://docs.gl/gl3/glDrawElements.
            //
            // Importante: estes eixos ser�o desenhamos com a matriz "model"
            // definida acima, e portanto sofrer�o as mesmas transforma��es
            // geom�tricas que o cubo. Isto �, estes eixos estar�o
            // representando o sistema de coordenadas do modelo (e n�o o global)!
            glDrawElements(
                g_VirtualScene["axes"].rendering_mode,
                g_VirtualScene["axes"].num_indices,
                GL_UNSIGNED_INT,
                (void*)g_VirtualScene["axes"].first_index
            );

            // Informamos para a placa de v�deo (GPU) que a vari�vel booleana
            // "render_as_black" deve ser colocada como "true". Veja o arquivo
            // "shader_vertex.glsl".
            glUniform1i(render_as_black_uniform, true);

            // Pedimos para a GPU rasterizar os v�rtices do cubo apontados pelo
            // VAO como linhas, formando as arestas pretas do cubo. Veja a
            // defini��o de g_VirtualScene["cube_edges"] dentro da fun��o
            // BuildTriangles(), e veja a documenta��o da fun��o
            // glDrawElements() em http://docs.gl/gl3/glDrawElements.
            glDrawElements(
                g_VirtualScene["cube_edges"].rendering_mode,
                g_VirtualScene["cube_edges"].num_indices,
                GL_UNSIGNED_INT,
                (void*)g_VirtualScene["cube_edges"].first_index
            );

            // Desenhamos um ponto de tamanho 15 pixels em cima do terceiro v�rtice
            // do terceiro cubo. Este v�rtice tem coordenada de modelo igual �
            // (0.5, 0.5, 0.5, 1.0).
            if ( i == 3 )
            {
                glPointSize(15.0f);
                glDrawArrays(GL_POINTS, 3, 1);
            }
        }

        // Agora queremos desenhar os eixos XYZ de coordenadas GLOBAIS.
        // Para tanto, colocamos a matriz de modelagem igual � identidade.
        // Veja slides 2-14 e 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 model = Matrix_Identity();

        // Enviamos a nova matriz "model" para a placa de v�deo (GPU). Veja o
        // arquivo "shader_vertex.glsl".
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        // Pedimos para OpenGL desenhar linhas com largura de 10 pixels.
        glLineWidth(10.0f);

        // Informamos para a placa de v�deo (GPU) que a vari�vel booleana
        // "render_as_black" deve ser colocada como "false". Veja o arquivo
        // "shader_vertex.glsl".
        glUniform1i(render_as_black_uniform, false);

        // Pedimos para a GPU rasterizar os v�rtices dos eixos XYZ
        // apontados pelo VAO como linhas. Veja a defini��o de
        // g_VirtualScene["axes"] dentro da fun��o BuildTriangles(), e veja
        // a documenta��o da fun��o glDrawElements() em
        // http://docs.gl/gl3/glDrawElements.
        glDrawElements(
            g_VirtualScene["axes"].rendering_mode,
            g_VirtualScene["axes"].num_indices,
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["axes"].first_index
        );

        // "Desligamos" o VAO, evitando assim que opera��es posteriores venham a
        // alterar o mesmo. Isso evita bugs.
        glBindVertexArray(0);

        // Pegamos um v�rtice com coordenadas de modelo (0.5, 0.5, 0.5, 1) e o
        // passamos por todos os sistemas de coordenadas armazenados nas
        // matrizes the_model, the_view, e the_projection; e escrevemos na tela
        // as matrizes e pontos resultantes dessas transforma��es.
        glm::vec4 p_model(0.5f, 0.5f, 0.5f, 1.0f);
        TextRendering_ShowModelViewProjection(window, the_projection, the_view, the_model, p_model);

        // Imprimimos na tela os �ngulos de Euler que controlam a rota��o do
        // terceiro cubo.
        TextRendering_ShowEulerAngles(window);

        // Imprimimos na informa��o sobre a matriz de proje��o sendo utilizada.
        TextRendering_ShowProjection(window);

        // Imprimimos na tela informa��o sobre o n�mero de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        // O framebuffer onde OpenGL executa as opera��es de renderiza��o n�o
        // � o mesmo que est� sendo mostrado para o usu�rio, caso contr�rio
        // seria poss�vel ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usu�rio
        // tudo que foi renderizado pelas fun��es acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma intera��o do
        // usu�rio (teclado, mouse, ...). Caso positivo, as fun��es de callback
        // definidas anteriormente usando glfwSet*Callback() ser�o chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }
    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Constr�i tri�ngulos para futura renderiza��o
GLuint BuildTriangles()
{
    // Primeiro, definimos os atributos de cada v�rtice.

    // A posi��o de cada v�rtice � definida por coeficientes em um sistema de
    // coordenadas local de cada modelo geom�trico. Note o uso de coordenadas
    // homog�neas.  Veja as seguintes refer�ncias:
    //
    //  - slides 35-48 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //  - slides 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //
    // Este vetor "model_coefficients" define a GEOMETRIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLfloat model_coefficients[] = {
    // V�rtices de um cubo
    //    X      Y     Z     W
        -0.5f,  0.5f,  0.5f, 1.0f, // posi��o do v�rtice 0
        -0.5f, -0.5f,  0.5f, 1.0f, // posi��o do v�rtice 1
         0.5f, -0.5f,  0.5f, 1.0f, // posi��o do v�rtice 2
         0.5f,  0.5f,  0.5f, 1.0f, // posi��o do v�rtice 3
        -0.5f,  0.5f, -0.5f, 1.0f, // posi��o do v�rtice 4
        -0.5f, -0.5f, -0.5f, 1.0f, // posi��o do v�rtice 5
         0.5f, -0.5f, -0.5f, 1.0f, // posi��o do v�rtice 6
         0.5f,  0.5f, -0.5f, 1.0f, // posi��o do v�rtice 7
    // V�rtices para desenhar o eixo X
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 8
         1.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 9
    // V�rtices para desenhar o eixo Y
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 10
         0.0f,  1.0f,  0.0f, 1.0f, // posi��o do v�rtice 11
    // V�rtices para desenhar o eixo Z
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 12
         0.0f,  0.0f,  1.0f, 1.0f, // posi��o do v�rtice 13
    };

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO �
    // um buffer de mem�ria que ir� conter os valores de um certo atributo de
    // um conjunto de v�rtices; por exemplo: posi��o, cor, normais, coordenadas
    // de textura.  Neste exemplo utilizaremos v�rios VBOs, um para cada tipo de atributo.
    // Agora criamos um VBO para armazenarmos um atributo: posi��o.
    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // cont�m a defini��o de v�rios atributos de um certo conjunto de v�rtices;
    // isto �, um VAO ir� conter ponteiros para v�rios VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // est� contido na vari�vel "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID est� contido na
    // vari�vel VBO_model_coefficients_id ser� modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer � de fato um VBO, e
    // ir� conter atributos de v�rtices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

    // Alocamos mem�ria para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
    // para alocar um n�mero de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" d� uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que n�o pretendemos alterar tais dados (s�o est�ticos: "STATIC"), e
    // tamb�m dizemos que tais dados ser�o utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array model_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

    // Precisamos ent�o informar um �ndice de "local" ("location"), o qual ser�
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Tamb�m, informamos a dimens�o (n�mero de
    // coeficientes) destes atributos. Como em nosso caso s�o pontos em coordenadas
    // homog�neas, temos quatro coeficientes por v�rtice (X,Y,Z,W). Isso define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados est�o em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta fun��o tamb�m informa que o VBO "ligado" acima em glBindBuffer()
    // est� dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com �ndice de local
    // definido acima, na vari�vel "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Agora repetimos todos os passos acima para atribuir um novo atributo a
    // cada v�rtice: uma cor (veja slides 109-112 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 111 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    // Tal cor � definida como coeficientes RGBA: Red, Green, Blue, Alpha;
    // isto �: Vermelho, Verde, Azul, Alpha (valor de transpar�ncia).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Ilumina��o.
    GLfloat color_coefficients[] = {
    // Cores dos v�rtices do cubo
    //  R     G     B     A
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 0
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 1
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 2
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 3
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 4
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 5
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 6
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 7
    // Cores para desenhar o eixo X
        1.0f, 0.0f, 0.0f, 1.0f, // cor do v�rtice 8
        1.0f, 0.0f, 0.0f, 1.0f, // cor do v�rtice 9
    // Cores para desenhar o eixo Y
        0.0f, 1.0f, 0.0f, 1.0f, // cor do v�rtice 10
        0.0f, 1.0f, 0.0f, 1.0f, // cor do v�rtice 11
    // Cores para desenhar o eixo Z
        0.0f, 0.0f, 1.0f, 1.0f, // cor do v�rtice 12
        0.0f, 0.0f, 1.0f, 1.0f, // cor do v�rtice 13
    };
    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos ent�o definir pol�gonos utilizando os v�rtices do array
    // model_coefficients.
    //
    // Para refer�ncia sobre os modos de renderiza��o, veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLuint indices[] = {
    // Definimos os �ndices dos v�rtices que definem as FACES de um cubo
    // atrav�s de 12 tri�ngulos que ser�o desenhados com o modo de renderiza��o
    // GL_TRIANGLES.
        0, 1, 2, // tri�ngulo 1
        7, 6, 5, // tri�ngulo 2
        3, 2, 6, // tri�ngulo 3
        4, 0, 3, // tri�ngulo 4
        4, 5, 1, // tri�ngulo 5
        1, 5, 6, // tri�ngulo 6
        0, 2, 3, // tri�ngulo 7
        7, 5, 4, // tri�ngulo 8
        3, 6, 7, // tri�ngulo 9
        4, 3, 7, // tri�ngulo 10
        4, 1, 0, // tri�ngulo 11
        1, 6, 2, // tri�ngulo 12
    // Definimos os �ndices dos v�rtices que definem as ARESTAS de um cubo
    // atrav�s de 12 linhas que ser�o desenhadas com o modo de renderiza��o
    // GL_LINES.
        0, 1, // linha 1
        1, 2, // linha 2
        2, 3, // linha 3
        3, 0, // linha 4
        0, 4, // linha 5
        4, 7, // linha 6
        7, 6, // linha 7
        6, 2, // linha 8
        6, 5, // linha 9
        5, 4, // linha 10
        5, 1, // linha 11
        7, 3, // linha 12
    // Definimos os �ndices dos v�rtices que definem as linhas dos eixos X, Y,
    // Z, que ser�o desenhados com o modo GL_LINES.
        8 , 9 , // linha 1
        10, 11, // linha 2
        12, 13  // linha 3
    };

    // Criamos um primeiro objeto virtual (SceneObject) que se refere �s faces
    // coloridas do cubo.
    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = (void*)0; // Primeiro �ndice est� em indices[0]
    cube_faces.num_indices    = 36;       // �ltimo �ndice est� em indices[35]; total de 36 �ndices.
    cube_faces.rendering_mode = GL_TRIANGLES; // �ndices correspondem ao tipo de rasteriza��o GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_faces"] = cube_faces;

    // Criamos um segundo objeto virtual (SceneObject) que se refere �s arestas
    // pretas do cubo.
    SceneObject cube_edges;
    cube_edges.name           = "Cubo (arestas pretas)";
    cube_edges.first_index    = (void*)(36*sizeof(GLuint)); // Primeiro �ndice est� em indices[36]
    cube_edges.num_indices    = 24; // �ltimo �ndice est� em indices[59]; total de 24 �ndices.
    cube_edges.rendering_mode = GL_LINES; // �ndices correspondem ao tipo de rasteriza��o GL_LINES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_edges"] = cube_edges;

    // Criamos um terceiro objeto virtual (SceneObject) que se refere aos eixos XYZ.
    SceneObject axes;
    axes.name           = "Eixos XYZ";
    axes.first_index    = (void*)(60*sizeof(GLuint)); // Primeiro �ndice est� em indices[60]
    axes.num_indices    = 6; // �ltimo �ndice est� em indices[65]; total de 6 �ndices.
    axes.rendering_mode = GL_LINES; // �ndices correspondem ao tipo de rasteriza��o GL_LINES.
    g_VirtualScene["axes"] = axes;


    //Objeto virtual para bolas de neve
    SceneObject Snowballs;
    Snowballs.name = "snowballs";
    Snowballs.first_index = (void*)0;
    Snowballs.num_indices = 32;
    Snowballs.rendering_mode = GL_TRIANGLES;
    g_VirtualScene["snowballs"] = Snowballs;

    //Objeto virtual para bolas de neve
    SceneObject Floor;
    Floor.name = "floor";
    Floor.first_index = (void*)0;
    Floor.num_indices = 32;
    Floor.rendering_mode = GL_TRIANGLES;
    g_VirtualScene["floor"] = Snowballs;

    SceneObject Enemy;
    Enemy.name = "enemy";
    Enemy.first_index = (void*)0;
    Enemy.num_indices = 32;
    Enemy.rendering_mode = GL_TRIANGLES;
    g_VirtualScene["enemy"] = Enemy;

    // Criamos um buffer OpenGL para armazenar os �ndices acima
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora � GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos mem�ria para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array indices[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

    // N�O fa�a a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de �ndices (GL_ELEMENT_ARRAY_BUFFER) n�o pode ser "desligado",
    // caso contr�rio o VAO ir� perder a informa��o sobre os �ndices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso � tudo que ser� necess�rio para renderizar
    // os tri�ngulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja defini��o de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // ser� aplicado nos v�rtices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja defini��o de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // ser� aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Fun��o auxilar, utilizada pelas duas fun��es acima. Carrega c�digo de GPU de
// um arquivo GLSL e faz sua compila��o.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela vari�vel "filename"
    // e colocamos seu conte�do em mem�ria, apontado pela vari�vel
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o c�digo do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o c�digo do shader GLSL (em tempo de execu��o)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compila��o
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos mem�ria para guardar o log de compila��o.
    // A chamada "new" em C++ � equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compila��o
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ � equivalente ao "free()" do C
    delete [] log;
}

// Fun��o que carrega os shaders de v�rtices e de fragmentos que ser�o
// utilizados para renderiza��o. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" est�o fixados, sendo que assumimos a exist�ncia
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);
}

// Esta fun��o cria um programa de GPU, o qual cont�m obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Defini��o dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos mem�ria para guardar o log de compila��o.
        // A chamada "new" em C++ � equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ � equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Retornamos o ID gerado acima
    return program_id;
}

// Defini��o da fun��o que ser� chamada sempre que a janela do sistema
// operacional for redimensionada, por consequ�ncia alterando o tamanho do
// "framebuffer" (regi�o de mem�ria onde s�o armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda regi�o do framebuffer. A
    // fun��o "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa � a opera��o de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos tamb�m a raz�o que define a propor��o da janela (largura /
    // altura), a qual ser� utilizada na defini��o das matrizes de proje��o,
    // tal que n�o ocorra distor��es durante o processo de "Screen Mapping"
    // acima, quando NDC � mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float � necess�rio pois n�meros inteiros s�o arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Vari�veis globais que armazenam a �ltima posi��o do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.

// Fun��o callback chamada sempre que o usu�rio aperta algum dos bot�es do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usu�rio pressionou o bot�o esquerdo do mouse, guardamos a
        // posi��o atual do cursor nas vari�veis g_LastCursorPosX e
        // g_LastCursorPosY.  Tamb�m, setamos a vari�vel
        // g_LeftMouseButtonPressed como true, para saber que o usu�rio est�
        // com o bot�o esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usu�rio soltar o bot�o esquerdo do mouse, atualizamos a
        // vari�vel abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        g_RightMouseButtonPressed = true;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        g_RightMouseButtonPressed = false;
}

// Fun��o callback chamada sempre que o usu�rio movimentar o cursor do mouse em
// cima da janela OpenGL.

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o bot�o esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o �ltimo
    // instante de tempo, e usamos esta movimenta��o para atualizar os
    // par�metros que definem a posi��o da c�mera dentro da cena virtual.
    // Assim, temos que o usu�rio consegue controlar a c�mera.


    if (!g_LeftMouseButtonPressed)
        return;

    // Calculamos o deslocamento horizontal (dx) e vertical (dy) do ponteiro do
    // mouse, em COORDENADAS DE TELA (pixels), desde a �ltima chamada � fun��o
    // CursorPosCallback().
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    // Mapeamos o deslocamento horizontal do mouse para uma varia��o no �ngulo
    // theta, e o deslocamento vertical para uma varia��o no �ngulo phi, que
    // ir�o definir o view vector. O valor 0.003 � um fator de escalamento
    // escolhido empiricamente, que define a convers�o entre coordenadas de TELA
    // para radianos. Este fator controla a "velocidade" de rota��o da c�mera de
    // acordo com a movimenta��o do mouse (a "sensitividade" do mouse).
    g_Theta -= 0.01f*dx;
    g_Phi   += 0.01f*dy;

    // Em coordenadas esf�ricas, o �ngulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (g_Phi > phimax)
        g_Phi = phimax;

    if (g_Phi < phimin)
        g_Phi = phimin;

    // Atualizamos as vari�veis globais para armazenar a posi��o atual do
    // mouse como sendo a �ltima posi��o conhecida do mouse.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
}
/*
// Fun��o callback chamada sempre que o usu�rio movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a dist�ncia da c�mera para a origem utilizando a
    // movimenta��o da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma c�mera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela est� olhando, pois isto gera problemas de divis�o por zero na
    // defini��o do sistema de coordenadas da c�mera. Isto �, a vari�vel abaixo
    // nunca pode ser zero. Vers�es anteriores deste c�digo possu�am este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}*/

// Defini��o da fun��o que ser� chamada sempre que o usu�rio pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // =================
    // N�o modifique este loop! Ele � utilizando para corre��o automatizada dos
    // laborat�rios. Deve ser sempre o primeiro comando desta fun��o KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // =================

    // Se o usu�rio pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // O c�digo abaixo implementa a seguinte l�gica:
    //   Se apertar tecla X       ent�o g_AngleX += delta;
    //   Se apertar tecla shift+X ent�o g_AngleX -= delta;
    //   Se apertar tecla Y       ent�o g_AngleY += delta;
    //   Se apertar tecla shift+Y ent�o g_AngleY -= delta;
    //   Se apertar tecla Z       ent�o g_AngleZ += delta;
    //   Se apertar tecla shift+Z ent�o g_AngleZ -= delta;

    float delta = 3.141592 / 16; // 22.5 graus, em radianos.

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    // Se o usu�rio apertar a tecla espa�o, resetamos os �ngulos de Euler para zero.
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        g_AngleX = 0.0f;
        g_AngleY = 0.0f;
        g_AngleZ = 0.0f;
    }

    // Se o usu�rio apertar a tecla P, utilizamos proje��o perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = true;
    }

    // Se o usu�rio apertar a tecla O, utilizamos proje��o ortogr�fica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = false;
    }

    // Se o usu�rio apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfoText = !g_ShowInfoText;
    }

    if(key == GLFW_KEY_W)
    {
        if(action == GLFW_PRESS)
            tecla_W_pressionada = true;
        else if(action == GLFW_RELEASE)
            tecla_W_pressionada = false;
        else if(action == GLFW_REPEAT);
    }

    if(key == GLFW_KEY_S)
    {
        if(action == GLFW_PRESS)
            tecla_S_pressionada = true;
        else if(action == GLFW_RELEASE)
            tecla_S_pressionada = false;
        else if(action == GLFW_REPEAT);
    }

    if(key == GLFW_KEY_A)
    {
        if(action == GLFW_PRESS)
            tecla_A_pressionada = true;
        else if(action == GLFW_RELEASE)
            tecla_A_pressionada = false;
        else if(action == GLFW_REPEAT);
    }

    if(key == GLFW_KEY_D)
    {
        if(action == GLFW_PRESS)
            tecla_D_pressionada = true;
        else if(action == GLFW_RELEASE)
            tecla_D_pressionada = false;
        else if(action == GLFW_REPEAT);

    }
    if(key == GLFW_KEY_L)
    {
        if(action == GLFW_PRESS)
            tecla_L_pressionada = true;
        else if(action == GLFW_RELEASE)
            tecla_L_pressionada = false;
        else if(action == GLFW_REPEAT);

    }

    

}

// Definimos o callback para impress�o de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Esta fun��o recebe um v�rtice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transforma��es.
void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
)
{
    if ( !g_ShowInfoText )
        return;

    glm::vec4 p_world = model*p_model;
    glm::vec4 p_camera = view*p_world;
    glm::vec4 p_clip = projection*p_camera;
    glm::vec4 p_ndc = p_clip / p_clip.w;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f-pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f-2*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-6*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-7*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-8*pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f-9*pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f-10*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-14*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-15*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-16*pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f-17*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f-18*pad, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glm::vec2 a = glm::vec2(-1, -1);
    glm::vec2 b = glm::vec2(+1, +1);
    glm::vec2 p = glm::vec2( 0,  0);
    glm::vec2 q = glm::vec2(width, height);

    glm::mat4 viewport_mapping = Matrix(
        (q.x - p.x)/(b.x-a.x), 0.0f, 0.0f, (b.x*p.x - a.x*q.x)/(b.x-a.x),
        0.0f, (q.y - p.y)/(b.y-a.y), 0.0f, (b.y*p.y - a.y*q.y)/(b.y-a.y),
        0.0f , 0.0f , 1.0f , 0.0f ,
        0.0f , 0.0f , 0.0f , 1.0f
    );

    TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f-22*pad, 1.0f);
    TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f-23*pad, 1.0f);
    TextRendering_PrintString(window, "                            V                           ", -1.0f, 1.0f-24*pad, 1.0f);

    TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f-25*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f-26*pad, 1.0f);
}

// Escrevemos na tela os �ngulos de Euler definidos nas vari�veis globais
// g_AngleX, g_AngleY, e g_AngleZ.
void TextRendering_ShowEulerAngles(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n", g_AngleZ, g_AngleY, g_AngleX);

    TextRendering_PrintString(window, buffer, -1.0f+pad/10, -1.0f+2*pad/10, 1.0f);
}

// Escrevemos na tela qual matriz de proje��o est� sendo utilizada.
void TextRendering_ShowProjection(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    if ( g_UsePerspectiveProjection )
        TextRendering_PrintString(window, "Perspective", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
    else
        TextRendering_PrintString(window, "Orthographic", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
}

// Escrevemos na tela o n�mero de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Vari�veis est�ticas (static) mant�m seus valores entre chamadas
    // subsequentes da fun��o!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o n�mero de segundos que passou desde a execu��o do programa
    float seconds = (float)glfwGetTime();

    // N�mero de segundos desde o �ltimo c�lculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}


// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :


