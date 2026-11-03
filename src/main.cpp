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
    float kc; // atenuação constante
    float kl; // linear
    float kq; // quadrática
};

light_config glight;
int GSUBDIV = 8; // sub-divisões por lado de cada face

// FUNÇÕES
void init();
void display();
void drawTwoLevelDesk();
void drawComputerStation(float posX);
void multTranslation(float x, float y, float z);
void multRotationX(float angle);
void multRotationY(float angle);
void multScale(float x, float y, float z);
void multPerspective(float fov, float aspect, float zNear, float zFar);
void multOrtho(float left, float right, float bottom, float top, float zNear, float zFar);

// Rasterização usando DDA
void dda(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int length;

    if (abs(dx) >= abs(dy)) {
        length = abs(dx);
    } else {
        length = abs(dy);
    }

    if (length == 0) {
        glBegin(GL_POINTS);
        glVertex2i(x1, y1);
        glEnd();
        return;
    }

    float xInc = (float)dx / length;
    float yInc = (float)dy / length;
    float x = x1, y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= length; i++) {
        glVertex2i((int)floor(x + 0.5f), (int)floor(y + 0.5f));
        x += xInc;
        y += yInc;
    }
    glEnd();
}

// Configuração da Iluminação
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

void global_ambient(float r, float g, float b) {
    GLfloat amb[] = { r, g, b, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
}

void starter_light() {
    glEnable(GL_NORMALIZE);                 // corrige normais após multScale
    glShadeModel(GL_SMOOTH);                // tonalização de Gouraud
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // especular mais preciso
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
    glutCreateWindow("Ambiente Virtual - C3 / FURG - Bancada de 4 Estações");

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

    configure_light(4.0f, 6.0f, 5.0f,
                    0.3f, 0.3f, 0.3f,
                    0.8f, 0.8f, 0.8f,
                    1.0f, 1.0f, 1.0f,
                    1.0f, 0.01f, 0.0f);

    global_ambient(0.1f, 0.1f, 0.1f);
    GSUBDIV = 10;
}

// Matrizes de transformação com glMultMatrix
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

// Aplicação de materiais
void setMaterial(float r, float g, float b, float specular = 0.5f, float shininess = 32.0f) {
    GLfloat ambient[]  = { r * 0.3f, g * 0.3f, b * 0.3f, 1.0f };
    GLfloat diffuse[]  = { r, g, b, 1.0f };
    GLfloat spec[]     = { specular, specular, specular, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  spec);
    glMaterialf (GL_FRONT, GL_SHININESS, shininess);
}

// Desenha cubo subdividido com normais (suporta iluminação Phong/Gouraud)
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

// Desenha uma estação de trabalho completa (Gabinete, Teclado, Mouse e Monitor)
void drawComputerStation(float posX) {
    // 1. Gabinete deitado (Nível inferior)
    setMaterial(0.2f, 0.2f, 0.22f);
    glPushMatrix();
        multTranslation(posX - 0.2f, 0.05f, -0.1f);
        multScale(0.9f, 0.30f, 0.85f);
        drawLitCube();
    glPopMatrix();

    // 2. Teclado
    setMaterial(0.25f, 0.25f, 0.25f);
    glPushMatrix();
        multTranslation(posX - 0.1f, -0.1f, 0.65f);
        multScale(0.95f, 0.04f, 0.40f);
        drawLitCube();
    glPopMatrix();

    // 3. Mouse
    glPushMatrix();
        multTranslation(posX + 0.55f, -0.1f, 0.65f);
        multScale(0.18f, 0.04f, 0.28f);
        drawLitCube();
    glPopMatrix();

    // 4. Base do Monitor (Nível superior)
    setMaterial(0.15f, 0.15f, 0.15f);
    glPushMatrix();
        multTranslation(posX, 0.35f, -0.7f);
        multScale(0.45f, 0.04f, 0.30f);
        drawLitCube();
    glPopMatrix();

    // 5. Haste do Monitor
    glPushMatrix();
        multTranslation(posX, 0.55f, -0.7f);
        multScale(0.07f, 0.40f, 0.07f);
        drawLitCube();
    glPopMatrix();

    // 6. Carcaça do Monitor
    glPushMatrix();
        multTranslation(posX, 0.90f, -0.72f);
        multScale(1.25f, 0.80f, 0.08f);
        drawLitCube();
    glPopMatrix();

    // 7. Display/Tela do Monitor (Azulado)
    setMaterial(0.15f, 0.45f, 0.75f);
    glPushMatrix();
        multTranslation(posX, 0.90f, -0.67f);
        multScale(1.15f, 0.70f, 0.02f);
        drawLitCube();
    glPopMatrix();
}

// Desenha a bancada completa de 4 lugares com pés laterais em formato de painel
void drawTwoLevelDesk() {
    float deskWidth = 8.0f; // Bancada larga para 4 computadores

    // 1. Tampo Inferior da Mesa (Madeira)
    setMaterial(0.69f, 0.22f, 0.09f);
    glPushMatrix();
        multTranslation(0.0f, -0.2f, 0.0f);
        multScale(deskWidth, 0.15f, 2.0f);
        drawLitCube();
    glPopMatrix();

    // 2. Tampo Superior (Nível acima)
    glPushMatrix();
        multTranslation(0.0f, 0.25f, -0.4f);
        multScale(deskWidth, 0.15f, 1.2f);
        drawLitCube();
    glPopMatrix();

    // 3. Painel do Fundo (Fechamento Traseiro da Prateleira)
    glPushMatrix();
        multTranslation(0.0f, 0.025f, -0.95f);
        multScale(deskWidth, 0.3f, 0.1f);
        drawLitCube();
    glPopMatrix();

    // 4. PÉS LATERAIS EM PLACA (Pé Esquerdo e Pé Direito em formato de painel sólido)
    setMaterial(0.35f, 0.35f, 0.38f);

    float sidePanelWidth = 0.15f; // Espessura da placa lateral
    float sidePanelHeight = 1.3f;  // Altura dos pés
    float sidePanelDepth = 2.0f;   // Profundidade da bancada
    float sidePanelY = -0.85f;
    float posX_Left = -deskWidth / 2.0f + sidePanelWidth / 2.0f;
    float posX_Right = deskWidth / 2.0f - sidePanelWidth / 2.0f;

    // Pé Lateral Esquerdo (Placa Sólida)
    glPushMatrix();
        multTranslation(posX_Left, sidePanelY, 0.0f);
        multScale(sidePanelWidth, sidePanelHeight, sidePanelDepth);
        drawLitCube();
    glPopMatrix();

    // Pé Lateral Direito (Placa Sólida)
    glPushMatrix();
        multTranslation(posX_Right, sidePanelY, 0.0f);
        multScale(sidePanelWidth, sidePanelHeight, sidePanelDepth);
        drawLitCube();
    glPopMatrix();

    // 5. DESENHO DAS 4 ESTAÇÕES DE TRABALHO
    // As posições em X são calculadas para distribuir os 4 computadores uniformemente
    float stationsX[4] = { -2.7f, -0.9f, 0.9f, 2.7f };

    for (int i = 0; i < 4; i++) {
        drawComputerStation(stationsX[i]);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Projeção
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (projectionMode) {
        multPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    } else {
        multOrtho(-6.0f, 6.0f, -6.0f, 6.0f, 0.1f, 100.0f);
    }
    
    // Câmera / Modelo
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Câmera um pouco mais afastada (Z = -10.5f) para enquadrar toda a bancada de 4 lugares
    multTranslation(0.0f, 0.0f, -10.5f);
    multRotationX(angleX);
    multRotationY(angleY);

    apply_ligth();
    apply_ligth();

    drawTwoLevelDesk();

    glutSwapBuffers();
}