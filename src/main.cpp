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

void init();
void display();
void drawTwoLevelDesk();
void drawComputerStation(float posX);
void multTranslation(float x, float y, float z); // matriz de translação
void multRotationX(float angle); // matriz de rotação eixo X
void multRotationY(float angle); // matriz de rotação eixo Y
void multScale(float x, float y, float z); // matriz de escala
void multPerspective(float fov, float aspect, float zNear, float zFar); // matriz de perspectiva
void multOrtho(float left, float right, float bottom, float top, float zNear, float zFar); // matriz ortogonal
void multLookAt(float eyeX, float eyeY, float eyeZ,
                float centerX, float centerY, float centerZ,
                float upX, float upY, float upZ); // matriz de visualização (LookAt)
void clampCamera();
void keyboard(unsigned char key, int x, int y); // controles de teclado (WASD)
void specialKeys(int key, int x, int y); // controles pelas setas do teclado

// rasterização usando DDA
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

// configuração da Iluminação
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
    glShadeModel(GL_SMOOTH);                // tonalização de gouraud
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

// Variáveis globais de controle de câmera orbital (com limites para não ver a parte de trás)
float camYaw = 25.0f;       // Ângulo horizontal (em graus): limitado entre -75° e +75° (frente da bancada)
float camPitch = 20.0f;     // Ângulo vertical (em graus): limitado entre -5° e +75°
float camRadius = 10.5f;    // Distância da câmera (zoom): limitado entre 4.0 e 18.0
bool projectionMode = true; // true = Perspectiva, false = Ortogonal

const float MIN_YAW    = -75.0f;
const float MAX_YAW    =  75.0f;
const float MIN_PITCH  =  -5.0f;
const float MAX_PITCH  =  75.0f;
const float MIN_RADIUS =   4.0f;
const float MAX_RADIUS =  18.0f;

void clampCamera() {
    if (camYaw < MIN_YAW)       camYaw = MIN_YAW;
    if (camYaw > MAX_YAW)       camYaw = MAX_YAW;
    if (camPitch < MIN_PITCH)   camPitch = MIN_PITCH;
    if (camPitch > MAX_PITCH)   camPitch = MAX_PITCH;
    if (camRadius < MIN_RADIUS) camRadius = MIN_RADIUS;
    if (camRadius > MAX_RADIUS) camRadius = MAX_RADIUS;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ambiente Virtual - C3 / FURG - Bancada de 4 Estações");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}

void init() {
    glClearColor(0.15f, 0.15f, 0.22f, 1.0f); // fundo limpo
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

// matrizes de transformação
void multTranslation(float x, float y, float z) {
    GLfloat mat[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x,    y,    z,    1.0f
    };
    glMultMatrixf(mat); // multiplica a matriz atual pela matriz de translação
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

void multPerspective(float fov, float aspect, float zNear, float zFar) { // matriz de perspectiva
    float f = 1.0f / tan((fov * M_PI / 180.0f) / 2.0f);
    GLfloat mat[16] = {
        f / aspect, 0.0f,  0.0f,                          0.0f,
        0.0f,       f,     0.0f,                          0.0f,
        0.0f,       0.0f,  (zFar + zNear) / (zNear - zFar), -1.0f,
        0.0f,       0.0f,  (2.0f * zFar * zNear) / (zNear - zFar), 0.0f
    };
    glMultMatrixf(mat);
}

void multOrtho(float left, float right, float bottom, float top, float zNear, float zFar) { // matriz ortogonal
    GLfloat mat[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1.0f
    };
    glMultMatrixf(mat);
}

// matriz de visualização (LookAt) usando glMultMatrixf
void multLookAt(float eyeX, float eyeY, float eyeZ,
                float centerX, float centerY, float centerZ,
                float upX, float upY, float upZ) {
    // vetor direção (forward = normalize(center - eye))
    float fx = centerX - eyeX;
    float fy = centerY - eyeY;
    float fz = centerZ - eyeZ;
    float fLen = sqrt(fx * fx + fy * fy + fz * fz);
    if (fLen > 0.0001f) {
        fx /= fLen; fy /= fLen; fz /= fLen;
    }

    // vetor lateral direito (s = normalize(f x up))
    float sx = fy * upZ - fz * upY;
    float sy = fz * upX - fx * upZ;
    float sz = fx * upY - fy * upX;
    float sLen = sqrt(sx * sx + sy * sy + sz * sz);
    if (sLen > 0.0001f) {
        sx /= sLen; sy /= sLen; sz /= sLen;
    }

    // vetor superior ortogonal (u = s x f)
    float ux = sy * fz - sz * fy;
    float uy = sz * fx - sx * fz;
    float uz = sx * fy - sy * fx;

    // matriz LookAt em ordem column-major (padrão OpenGL)
    GLfloat mat[16] = {
        sx,  ux, -fx, 0.0f,
        sy,  uy, -fy, 0.0f,
        sz,  uz, -fz, 0.0f,
        -(sx * eyeX + sy * eyeY + sz * eyeZ),
        -(ux * eyeX + uy * eyeY + uz * eyeZ),
         (fx * eyeX + fy * eyeY + fz * eyeZ),
        1.0f
    };
    glMultMatrixf(mat);
}

// controles de teclado
void keyboard(unsigned char key, int x, int y) {
    const float angleStep = 3.0f;
    const float zoomStep  = 0.5f;

    switch (key) {
        case 'a':
        case 'A':
            camYaw -= angleStep;
            break;
        case 'd':
        case 'D':
            camYaw += angleStep;
            break;
        case 'w':
        case 'W':
            camPitch += angleStep;
            break;
        case 's':
        case 'S':
            camPitch -= angleStep;
            break;
        case '+':
        case '=':
        case 'q':
        case 'Q':
            camRadius -= zoomStep;
            break;
        case '-':
        case '_':
        case 'e':
        case 'E':
            camRadius += zoomStep;
            break;
        case 'p':
        case 'P':
            projectionMode = !projectionMode;
            break;
        case 'r':
        case 'R':
            camYaw = 25.0f;
            camPitch = 20.0f;
            camRadius = 10.5f;
            break;
        case 27: // ESC
            exit(0);
            break;
    }

    clampCamera();
    glutPostRedisplay();
}

// controles pelas setas
void specialKeys(int key, int x, int y) {
    const float angleStep = 3.0f;

    switch (key) {
        case GLUT_KEY_LEFT:
            camYaw -= angleStep;
            break;
        case GLUT_KEY_RIGHT:
            camYaw += angleStep;
            break;
        case GLUT_KEY_UP:
            camPitch += angleStep;
            break;
        case GLUT_KEY_DOWN:
            camPitch -= angleStep;
            break;
    }

    clampCamera();
    glutPostRedisplay();
}

// aplicação de materiais
void setMaterial(float r, float g, float b, float specular = 0.5f, float shininess = 32.0f) {
    GLfloat ambient[]  = { r * 0.3f, g * 0.3f, b * 0.3f, 1.0f };
    GLfloat diffuse[]  = { r, g, b, 1.0f };
    GLfloat spec[]     = { specular, specular, specular, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  spec);
    glMaterialf (GL_FRONT, GL_SHININESS, shininess);
}

// desenha cubo subdividido com normais (suporta iluminação Phong/Gouraud)
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


struct Seg { int x1, y1, x2, y2; };

static const Seg GLYPH_C[] = { {4,5,3,6}, {3,6,1,6}, {1,6,0,5}, {0,5,0,1}, {0,1,1,0}, {1,0,3,0}, {3,0,4,1} };
static const Seg GLYPH_3[] = { {0,6,3,6}, {3,6,4,5}, {4,5,4,4}, {4,4,3,3}, {1,3,3,3}, {3,3,4,2}, {4,2,4,1}, {4,1,3,0}, {3,0,0,0} };

#define COUNT(a) ((int)(sizeof(a) / sizeof((a)[0])))


void draw_letter(const Seg* g, int n, int ox, int oy, int k) {
    for (int i = 0; i < n; i++)
        dda(ox + g[i].x1 * k, oy + g[i].y1 * k,
            ox + g[i].x2 * k, oy + g[i].y2 * k);          
}


void draw_logo(float posX) { // desenha o logotipo
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POINT_BIT);
    glDisable(GL_LIGHTING);                 
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(3.0f);

    glPushMatrix();
        multTranslation(posX, 0.90f, -0.655f);   
        multScale(0.01f, 0.01f, 1.0f);           

        // moldura
        dda(-52, -31,  52, -31);
        dda( 52, -31,  52,  31);
        dda( 52,  31, -52,  31);
        dda(-52,  31, -52, -31);

        
        const int k = 8;
        draw_letter(GLYPH_C, COUNT(GLYPH_C), -38,      -24, k);
        draw_letter(GLYPH_3, COUNT(GLYPH_3), -38 + 44, -24, k);
    glPopMatrix();

    glPopAttrib();                          // restaura luz, cor e tamanho do ponto
}

// desenha desktop
void drawComputerStation(float posX) {
    // gabinete
    setMaterial(0.2f, 0.2f, 0.22f);
    glPushMatrix();
        multTranslation(posX - 0.2f, 0.05f, -0.1f);
        multScale(0.9f, 0.30f, 0.85f);
        drawLitCube();
    glPopMatrix();

    //teclado
    setMaterial(0.25f, 0.25f, 0.25f);
    glPushMatrix();
        multTranslation(posX - 0.1f, -0.1f, 0.65f);
        multScale(0.95f, 0.04f, 0.40f);
        drawLitCube();
    glPopMatrix();

    //mouse
    glPushMatrix();
        multTranslation(posX + 0.55f, -0.1f, 0.65f);
        multScale(0.18f, 0.04f, 0.28f);
        drawLitCube();
    glPopMatrix();

    //pe do monitor
    setMaterial(0.15f, 0.15f, 0.15f);
    glPushMatrix();
        multTranslation(posX, 0.35f, -0.7f);
        multScale(0.45f, 0.04f, 0.30f);
        drawLitCube();
    glPopMatrix();

    //haste monitor
    glPushMatrix();
        multTranslation(posX, 0.55f, -0.7f);
        multScale(0.07f, 0.40f, 0.07f);
        drawLitCube();
    glPopMatrix();

    //moldura monitor
    glPushMatrix();
        multTranslation(posX, 0.90f, -0.72f);
        multScale(1.25f, 0.80f, 0.08f);
        drawLitCube();
    glPopMatrix();

    //tela
    setMaterial(0.15f, 0.45f, 0.75f);
    glPushMatrix();
        multTranslation(posX, 0.90f, -0.67f);
        multScale(1.15f, 0.70f, 0.02f);
        drawLitCube();
    glPopMatrix();

    // rasterização nos monitores
    draw_logo(posX);
}

//bancada
void drawTwoLevelDesk() {
    float deskWidth = 8.0f; // largura

    //tampo de baixo
    setMaterial(0.69f, 0.22f, 0.09f);
    glPushMatrix();
        multTranslation(0.0f, -0.2f, 0.0f);
        multScale(deskWidth, 0.15f, 2.0f);
        drawLitCube();
    glPopMatrix();

    //tampo de cima
    glPushMatrix();
        multTranslation(0.0f, 0.25f, -0.4f);
        multScale(deskWidth, 0.15f, 1.2f);
        drawLitCube();
    glPopMatrix();

    //painel de fundo
    glPushMatrix();
        multTranslation(0.0f, 0.025f, -0.95f);
        multScale(deskWidth, 0.3f, 0.1f);
        drawLitCube();
    glPopMatrix();

    //pes
    setMaterial(0.35f, 0.35f, 0.38f);

    float sidePanelWidth = 0.15f; // largura
    float sidePanelHeight = 1.3f;  // alura
    float sidePanelDepth = 2.0f;   // profundidade
    float sidePanelY = -0.85f; // posição em Y (altura do pé)
    float posX_Left = -deskWidth / 2.0f + sidePanelWidth / 2.0f;
    float posX_Right = deskWidth / 2.0f - sidePanelWidth / 2.0f;

    //pe esquerdo
    glPushMatrix();
        multTranslation(posX_Left, sidePanelY, 0.0f);
        multScale(sidePanelWidth, sidePanelHeight, sidePanelDepth);
        drawLitCube();
    glPopMatrix();

    //pe direito
    glPushMatrix();
        multTranslation(posX_Right, sidePanelY, 0.0f);
        multScale(sidePanelWidth, sidePanelHeight, sidePanelDepth);
        drawLitCube();
    glPopMatrix();

    //clona os 4 computadores
    float stationsX[4] = { -2.7f, -0.9f, 0.9f, 2.7f };

    for (int i = 0; i < 4; i++) {
        drawComputerStation(stationsX[i]);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // projeção
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = 800.0f / 600.0f;
    if (projectionMode) {
        multPerspective(45.0f, aspect, 0.1f, 100.0f);
    } else {
        float orthoSize = camRadius * 0.55f;
        multOrtho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, 0.1f, 100.0f);
    }
    
    // câmera / Modelo
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Cálculo da posição da câmera em coordenadas esféricas (sempre à frente da bancada, Z > 0)
    float yawRad   = camYaw   * M_PI / 180.0f;
    float pitchRad = camPitch * M_PI / 180.0f;

    float targetX = 0.0f;
    float targetY = 0.2f;
    float targetZ = 0.0f;

    float eyeX = targetX + camRadius * cos(pitchRad) * sin(yawRad);
    float eyeY = targetY + camRadius * sin(pitchRad);
    float eyeZ = targetZ + camRadius * cos(pitchRad) * cos(yawRad);

    multLookAt(eyeX, eyeY, eyeZ,
               targetX, targetY, targetZ,
               0.0f, 1.0f, 0.0f);

    apply_ligth();

    drawTwoLevelDesk();

    glutSwapBuffers();
}
