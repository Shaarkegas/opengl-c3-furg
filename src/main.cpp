#include <GL/glut.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct light_config {
    bool enable;
    float position[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float kc; // atenuação
    float kl; // linear
    float kq; // quadrática
};

light_config glight;
int GSUBDIV = 8; // sub divisões por lado de cada face 

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

// iluminação
void configure_light(float px, float py, float pz,
                    float ar, float ag, float ab,     // ambiente
                    float dr, float dg, float db,     // difusa
                    float sr, float sg, float sb,     // especular
                    float kc = 1.0f, float kl = 0.0f, float kq = 0.0f) {
    glight.enable = true;
    glight.position[0] = px; glight.position[1] = py; glight.position[2] = pz; glight.position[3] = 1.0f;
    glight.ambient[0]  = ar; glight.ambient[1]  = ag; glight.ambient[2]  = ab; glight.ambient[3]  = 1.0f;
    glight.diffuse[0]  = dr; glight.diffuse[1]  = dg; glight.diffuse[2]  = db; glight.diffuse[3]  = 1.0f;
    glight.specular[0] = sr; glight.specular[1] = sg; glight.specular[2] = sb; glight.specular[3] = 1.0f;
    glight.kc = kc; glight.kl = kl; glight.kq = kq;
}

void on_of_light(bool value) {
    if (value == true) {
        glight.enable = true;
    }

    if (value == false) {
        glight.enable = false;
    }
}

void global_ambient(float r, float g, float b) {
    GLfloat amb[] = { r, g, b, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
}

void starter_light() {
    glEnable(GL_NORMALIZE);                 // corrige normais após multScale (ESSENCIAL)
    glShadeModel(GL_SMOOTH);                // tonalização de Gouraud
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // especular mais correto
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void apply_ligth() {
    if (!glight.enable) { glDisable(GL_LIGHT0); return; }
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, glight.position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  glight.ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  glight.diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, glight.specular);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  glight.kc);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    glight.kl);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, glight.kq);
}



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

    starter_light();

    configure_light(3.0f, 5.0f, 4.0f,
                    0.3f, 0.3f, 0.3f,
                    0.8f, 0.8f, 0.8f,
                    1.0f, 1.0f, 1.0f,
                    1.0f, 0.02f, 0.0f);

    global_ambient(0.1f, 0.1f, 0.1f);
    GSUBDIV = 10;
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

// funcao para aplicar as cores e materiais aos objetos 
void setMaterial(float r, float g, float b, float specular = 0.5f, float shininess = 32.0f) {
    GLfloat ambient[]  = { r * 0.3f, g * 0.3f, b * 0.3f, 1.0f };
    GLfloat diffuse[]  = { r, g, b, 1.0f };
    GLfloat spec[]     = { specular, specular, specular, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  spec);
    glMaterialf (GL_FRONT, GL_SHININESS, shininess);
}

void drawLitCube() {
    static const float F[6][9] = {
        { 1, 0, 0,   0, 1, 0,   0, 0, 1},   // +X
        {-1, 0, 0,   0, 0, 1,   0, 1, 0},   // -X
        { 0, 1, 0,   0, 0, 1,   1, 0, 0},   // +Y
        { 0,-1, 0,   1, 0, 0,   0, 0, 1},   // -Y
        { 0, 0, 1,   1, 0, 0,   0, 1, 0},   // +Z
        { 0, 0,-1,   0, 1, 0,   1, 0, 0}    // -Z
    };
    const int   n    = GSUBDIV;
    const float step = 1.0f / n;

    glBegin(GL_QUADS);
    for (int f = 0; f < 6; f++) {
        const float* nl = &F[f][0];
        const float* u  = &F[f][3];
        const float* v  = &F[f][6];
        glNormal3f(nl[0], nl[1], nl[2]);

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                float s[2] = { i * step, (i + 1) * step };
                float t[2] = { j * step, (j + 1) * step };
                int si[4] = {0, 1, 1, 0};
                int ti[4] = {0, 0, 1, 1};
                for (int k = 0; k < 4; k++) {
                    float a = s[si[k]] - 0.5f, b = t[ti[k]] - 0.5f;
                    glVertex3f(nl[0]*0.5f + u[0]*a + v[0]*b,
                               nl[1]*0.5f + u[1]*a + v[1]*b,
                               nl[2]*0.5f + u[2]*a + v[2]*b);
                }
            }
        }
    }
    glEnd();
}

//funcoes de draw
void drawTwoLevelDesk() {
    //tampo Inferior da Mesa
    setMaterial(0.69f, 0.22f, 0.09f);
    glPushMatrix();
        multTranslation(0.0f, -0.2f, 0.0f); //posicao
        multScale(3.0f, 0.15f, 2.0f); //escala
        drawLitCube();
    glPopMatrix();

    // tampo superior
    glPushMatrix();
        multTranslation(0.0f, 0.25f, -0.4f); //posicao
        multScale(3.0f, 0.15f, 1.2f); //escala
        drawLitCube();
    glPopMatrix();

    // painel do fundo da mesa
    glPushMatrix();
        multTranslation(0.0f, 0.025f, -0.95f); //posicao
        multScale(3.0f, 0.3f, 0.1f); //escala
        drawLitCube();
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
        drawLitCube();
    glPopMatrix();

    // dianteira esquerda
    glPushMatrix();
        multTranslation(-offsetX, posY, offsetZ);
        multScale(legRadius, legHeight, legRadius);
        drawLitCube();
    glPopMatrix();

    // traseira direita
    glPushMatrix();
        multTranslation(offsetX, posY, -offsetZ);
        multScale(legRadius, legHeight, legRadius);
        drawLitCube();
    glPopMatrix();

    // traseira esquerda
    glPushMatrix();
        multTranslation(-offsetX, posY, -offsetZ);
        multScale(legRadius, legHeight, legRadius);
        drawLitCube();
    glPopMatrix();  

    // gabinete deitado
    setMaterial(0.2f, 0.2f, 0.22f);
    glPushMatrix();
        multTranslation(0.0f, 0.05f, -0.1f); //posicao
        multScale(1.0f, 0.30f, 0.9f); //escala
        drawLitCube();
    glPopMatrix();

    // teclado
    setMaterial(0.25f, 0.25f, 0.25f);
    glPushMatrix();
        multTranslation(0.1f, -0.1f, 0.75f);
        multScale(1.1f, 0.04f, 0.45f);
        drawLitCube();
    glPopMatrix();

    // mouse
    glPushMatrix();
        multTranslation(0.9f, -0.1f, 0.3f);
        multScale(0.2f, 0.04f, 0.3f);
        drawLitCube();
    glPopMatrix();

    // base do monitor
    setMaterial(0.15f, 0.15f, 0.15f);
    glPushMatrix();
        multTranslation(0.1f, 0.35f, -0.7f); //posicao
        multScale(0.5f, 0.04f, 0.35f); //escala
        drawLitCube();
    glPopMatrix();

    //haste do monitor
    glPushMatrix();
        multTranslation(0.1f, 0.55f, -0.7f);
        multScale(0.08f, 0.4f, 0.08f);
        drawLitCube();
    glPopMatrix();

    //carcaca do monitor
    glPushMatrix();
        multTranslation(0.1f, 0.9f, -0.72f);
        multScale(1.4f, 0.85f, 0.1f);
        drawLitCube();
    glPopMatrix();

    //tela do monitor
    setMaterial(0.15f, 0.45f, 0.75f);
    glPushMatrix();
        multTranslation(0.1f, 0.9f, -0.66f);
        multScale(1.3f, 0.75f, 0.02f);
        drawLitCube();
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

    apply_ligth();

    drawTwoLevelDesk();

    glutSwapBuffers();
}