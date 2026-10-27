#include <GL/glut.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//FUNÇÕES
void init();
void display();
void drawTwoLevelDesk();
void multTranslation(float x, float y, float z); //funcao para translação
void multRotationX(float angle); //funcao para rotacao x
void multRotationY(float angle); //funcao rotacao y
void multScale(float x, float y, float z); //funcao para escala
void multPerspective(float fov, float aspect, float zNear, float zFar); //funcao para projecao perspectiva
void multOrtho(float left, float right, float bottom, float top, float zNear, float zFar); //funcao para projecao ortogonal

// Variáveis globais de controle de câmera/rotação
float angleX = 20.0f;
float angleY = -30.0f;
bool projectionMode = true; // true = Perspectiva, false = Ortogonal

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ambiente Virtual - C3 / FURG - Mesa de Dois Níveis");

    init();

    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}

void init() {
    glClearColor(0.15f, 0.15f, 0.22f, 1.0f); // Fundo limpo
    glEnable(GL_DEPTH_TEST);               // Z-buffer ativado
    
    // Algoritmo de Visibilidade: Back-Face Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

// matrizes de transformacao
void multTranslation(float x, float y, float z) {
    GLfloat mat[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x,    y,    z,    1.0f
    };
    glMultMatrixf(mat);
}

// multiplicacao de escala
void multScale(float x, float y, float z) {
    GLfloat mat[16] = {
        x,    0.0f, 0.0f, 0.0f,
        0.0f, y,    0.0f, 0.0f,
        0.0f, 0.0f, z,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glMultMatrixf(mat);
}

//funcao de rotacao x
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

//rotacao soq no eixo y
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

// funcao de projecao perspectiva
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

//funcao de projecao ortogonal
void multOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
    GLfloat mat[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1.0f
    };
    glMultMatrixf(mat);
}

// funcao para aplicar as cores e materiais aos objetos (NAO CONSEGUI FAZER FUNCIONAR!!!!)
void setMaterial(float r, float g, float b) {
    GLfloat ambient[] = { r * 0.3f, g * 0.3f, b * 0.3f, 1.0f };
    GLfloat diffuse[] = { r, g, b, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
}

//funcoes de draw
void drawTwoLevelDesk() {
    //tampo Inferior da Mesa
    setMaterial(0.55f, 0.35f, 0.18f);
    glPushMatrix();
        multTranslation(0.0f, -0.2f, 0.0f); //posicao
        multScale(3.0f, 0.15f, 2.0f); //escala
        glutSolidCube(1.0f);
    glPopMatrix();

    // tampo superior
    glPushMatrix();
        multTranslation(0.0f, 0.25f, -0.4f); //posicao
        multScale(3.0f, 0.15f, 1.2f); //escala
        glutSolidCube(1.0f);
    glPopMatrix();

    // painel do fundo da mesa
    glPushMatrix();
        multTranslation(0.0f, 0.025f, -0.95f); //posicao
        multScale(3.0f, 0.3f, 0.1f); //escala
        glutSolidCube(1.0f);
    glPopMatrix();

    // pernas da mesa
    setMaterial(0.35f, 0.35f, 0.38f);

    float legHeight = 1.2f;
    float legRadius = 0.1f;
    float offsetX = 1.3f;
    float offsetZ = 0.8f;
    float posY = -0.9f;

    // dianteira direita
    glPushMatrix();
        multTranslation(offsetX, posY, offsetZ);
        multScale(legRadius, legHeight, legRadius);
        glutSolidCube(1.0f);
    glPopMatrix();

    // dianteira esquerda
    glPushMatrix();
        multTranslation(-offsetX, posY, offsetZ);
        multScale(legRadius, legHeight, legRadius);
        glutSolidCube(1.0f);
    glPopMatrix();

    // traseira direita
    glPushMatrix();
        multTranslation(offsetX, posY, -offsetZ);
        multScale(legRadius, legHeight, legRadius);
        glutSolidCube(1.0f);
    glPopMatrix();

    // traseira esquerda
    glPushMatrix();
        multTranslation(-offsetX, posY, -offsetZ);
        multScale(legRadius, legHeight, legRadius);
        glutSolidCube(1.0f);
    glPopMatrix();  

    // gabinete deitado
    setMaterial(0.2f, 0.2f, 0.22f);
    glPushMatrix();
        multTranslation(0.0f, 0.05f, -0.1f); //posicao
        multScale(1.0f, 0.30f, 0.9f); //escala
        glutSolidCube(1.0f);
    glPopMatrix();

    // teclado
    setMaterial(0.25f, 0.25f, 0.25f);
    glPushMatrix();
        multTranslation(0.1f, -0.1f, 0.75f);
        multScale(1.1f, 0.04f, 0.45f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // mouse
    glPushMatrix();
        multTranslation(0.9f, -0.1f, 0.3f);
        multScale(0.2f, 0.04f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // base do monitor
    setMaterial(0.15f, 0.15f, 0.15f);
    glPushMatrix();
        multTranslation(0.1f, 0.35f, -0.7f); //posicao
        multScale(0.5f, 0.04f, 0.35f); //escala
        glutSolidCube(1.0f);
    glPopMatrix();

    //haste do monitor
    glPushMatrix();
        multTranslation(0.1f, 0.55f, -0.7f);
        multScale(0.08f, 0.4f, 0.08f);
        glutSolidCube(1.0f);
    glPopMatrix();

    //carcaca do monitor
    glPushMatrix();
        multTranslation(0.1f, 0.9f, -0.72f);
        multScale(1.4f, 0.85f, 0.1f);
        glutSolidCube(1.0f);
    glPopMatrix();

    //tela do monitor
    setMaterial(0.15f, 0.45f, 0.75f);
    glPushMatrix();
        multTranslation(0.1f, 0.9f, -0.66f);
        multScale(1.3f, 0.75f, 0.02f);
        glutSolidCube(1.0f);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //projecao
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (projectionMode) {
        multPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    } else {
        multOrtho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    }
    
    //camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //posicionamento da camera
    multTranslation(0.0f, 0.0f, -7.0f);
    multRotationX(angleX);
    multRotationY(angleY);

    //configuracao de iluminacao
    GLfloat light_position[] = { 3.0f, 5.0f, 4.0f, 1.0f };
    GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glShadeModel(GL_SMOOTH); // Gouraud Shading

    drawTwoLevelDesk();

    glutSwapBuffers();
}