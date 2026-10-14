#include <GL/glut.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- PROTÓTIPOS DAS FUNÇÕES ---
void init();
void display();
void drawTable();
void multTranslation(float x, float y, float z);
void multRotationX(float angle);
void multRotationY(float angle);
void multScale(float x, float y, float z);
void multPerspective(float fov, float aspect, float zNear, float zFar);
void multOrtho(float left, float right, float bottom, float top, float zNear, float zFar);

// Variáveis globais de controle de câmera/rotação
float angleX = 20.0f;
float angleY = -30.0f;
bool projectionMode = true; // true = Perspectiva, false = Ortogonal

// --- FUNÇÃO MAIN ---
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ambiente Virtual - C3 / FURG - Mesa 3D");

    init();

    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}

// --- IMPLEMENTAÇÃO DAS FUNÇÕES ---
void init() {
    glClearColor(0.15f, 0.15f, 0.2f, 1.0f); // Fundo cinza-azulado elegante
    glEnable(GL_DEPTH_TEST);               // Z-buffer ativado (Visibilidade)
    
    // Algoritmo de Visibilidade: Back-Face Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

// Matrizes customizadas exigidas pelo projeto (utilizando glMultMatrix)
void multTranslation(float x, float y, float z) {
    GLfloat mat[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x,    y,    z,    1.0f
    };
    glMultMatrixf(mat);
}

void multScale(float x, float y, float z) {
    GLfloat mat[16] = {
        x,    0.0f, 0.0f, 0.0f,
        0.0f, y,    0.0f, 0.0f,
        0.0f, 0.0f, z,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glMultMatrixf(mat);
}

void multRotationX(float angle) {
    float rad = angle * M_PI / 180.0f;
    float c = cos(rad);
    float s = sin(rad);
    GLfloat mat[16] = {
        1.0f, 0.0f,  0.0f, 0.0f,
        0.0f, c,     s,    0.0f,
        0.0f, -s,    c,    0.0f,
        0.0f, 0.0f,  0.0f, 1.0f
    };
    glMultMatrixf(mat);
}

void multRotationY(float angle) {
    float rad = angle * M_PI / 180.0f;
    float c = cos(rad);
    float s = sin(rad);
    GLfloat mat[16] = {
        c,    0.0f, -s,   0.0f,
        0.0f, 1.0f,  0.0f, 0.0f,
        s,    0.0f,  c,    0.0f,
        0.0f, 0.0f,  0.0f, 1.0f
    };
    glMultMatrixf(mat);
}

void multPerspective(float fov, float aspect, float zNear, float zFar) {
    float f = 1.0f / tan((fov * M_PI / 180.0f) / 2.0f);
    GLfloat mat[16] = {
        f / aspect, 0.0f,  0.0f,                          0.0f,
        0.0f,       f,     0.0f,                          0.0f,
        0.0f,       0.0f,  (zFar + zNear) / (zNear - zFar), -1.0f,
        0.0f,       0.0f,  (2.0f * zFar * zNear) / (zNear - zFar), 0.0f
    };
    glMultMatrixf(mat);
}

void multOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
    GLfloat mat[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1.0f
    };
    glMultMatrixf(mat);
}

// --- FUNÇÃO PARA DESENHAR A MESA EM 3D NO CENTRO ---
void drawTable() {
    // Definindo propriedades de material e cor para a mesa (Tom de madeira / laboratório)
    GLfloat mat_ambient[] = { 0.3f, 0.2f, 0.1f, 1.0f };
    GLfloat mat_diffuse[] = { 0.6f, 0.4f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    // 1. Tampo da Mesa
    glPushMatrix();
        multTranslation(0.0f, 0.0f, 0.0f); // Centro do cenário
        multScale(3.0f, 0.2f, 2.0f);      // Largura, Altura do tampo, Profundidade
        glutSolidCube(1.0f);
    glPopMatrix();

    // Cores das pernas da mesa (Metálicas / Cinza)
    GLfloat leg_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat leg_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, leg_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leg_diffuse);

    float legHeight = 1.4f;
    float legRadius = 0.1f;
    float offsetX = 1.3f;
    float offsetZ = 0.8f;
    float posY = -0.7f;

    // 2. Perna Dianteira Direita
    glPushMatrix();
        multTranslation(offsetX, posY, offsetZ);
        multScale(legRadius, legHeight, legRadius);
        glutSolidCube(1.0f);
    glPopMatrix();

    // 3. Perna Dianteira Esquerda
    glPushMatrix();
        multTranslation(-offsetX, posY, offsetZ);
        multScale(legRadius, legHeight, legRadius);
        glutSolidCube(1.0f);
    glPopMatrix();

    // 4. Perna Traseira Direita
    glPushMatrix();
        multTranslation(offsetX, posY, -offsetZ);
        multScale(legRadius, legHeight, legRadius);
        glutSolidCube(1.0f);
    glPopMatrix();

    // 5. Perna Traseira Esquerda
    glPushMatrix();
        multTranslation(-offsetX, posY, -offsetZ);
        multScale(legRadius, legHeight, legRadius);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Projeção
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (projectionMode) {
        multPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    } else {
        multOrtho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    }
    
    // Modelo / Câmera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Posicionamento da Câmera afastada para ver a mesa no centro
    multTranslation(0.0f, 0.0f, -7.0f);
    multRotationX(angleX);
    multRotationY(angleY);

    // Configuração de Iluminação (Phong / Gouraud)
    GLfloat light_position[] = { 3.0f, 5.0f, 3.0f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glShadeModel(GL_SMOOTH); // Gouraud Shading

    // Desenhar a mesa posicionada bem no centro do ambiente do C3/FURG
    drawTable();

    glutSwapBuffers();
}