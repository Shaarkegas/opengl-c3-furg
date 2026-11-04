# Ambiente Virtual 3D - C3 / FURG (OpenGL)

Repositório desenvolvido para o trabalho prático da disciplina de **Computação Gráfica**, consistindo na modelagem e renderização de um ambiente virtual inspirado nas instalações do **Centro de Ciências Computacionais (C3)** da **Universidade Federal do Rio Grande (FURG)** utilizando **C++** e **OpenGL (Fixed-Function Pipeline)**.

---

## Tecnologias e Ferramentas

* **Linguagem:** C++ (Padrão C++17)
* **API Gráfica:** OpenGL / GLUT / FreeGLUT
* **Sistema de Build:** CMake

---

## Características do Modelo 3D e Funcionalidades

* **Bancada de Dois Níveis (4 Estações):** Modelagem proporcional às bancadas dos laboratórios do C3, contendo tampo inferior para periféricos/gabinetes, tampo superior elevado para monitores e painéis laterais sólidos em formato de placa.
* **Kits de Computadores Replicados:** 4 estações de trabalho completas alinhadas horizontalmente, contendo monitor (base, haste, carcaça e display), gabinete deitado, teclado e mouse.
* **Câmera Orbital Interativa (`multLookAt`):** Navegação com controle de rotação e zoom em coordenadas esféricas, restrita à área frontal da bancada para melhor visualização.
* **Logotipo C3 Rasterizado:** Desenho vetorial das letras "C3" e moldura aplicado diretamente na tela dos 4 monitores usando o algoritmo DDA.

---

## Status dos Requisitos do Trabalho (100% Concluído)

1. **Modelagem Geométrica do Ambiente:**
   * Modelagem da bancada de dois níveis com 4 computadores completos (monitores, teclados, mouses e gabinetes) inspirada no C3/FURG.
2. **Matrizes de Transformação Customizadas (`glMultMatrix`) :**
   * Implementação matemática própria para matrizes de **Translação**, **Rotação ($X$ e $Y$)**, **Escala** e **Visualização (`multLookAt`)**, aplicadas via `glMultMatrixf()`.
3. **Projeções Perspectiva e Ortogonal:**
   * Implementação manual das matrizes de projeção **Perspectiva** (`multPerspective`) e **Ortogonal** (`multOrtho`), alternáveis em tempo de execução.
4. **Iluminação e Tonalização (Phong / Gouraud):**
   * Modelo de iluminação de Phong (ambientes, difusa e especular com atenuação) e tonalização suave de Gouraud (`GL_SMOOTH`), aplicada sobre cubos primitivos subdivididos com normais calculadas por face.
5. **Algoritmo de Rasterização — DDA:**
   * Algoritmo DDA (Digital Differential Analyzer) implementado do zero para traçar linhas 2D e renderizar a logo do C3 nas telas dos monitores.
6. **Algoritmo de Visibilidade — Back-Face Culling & Z-Buffer:**
   * Ativação e validação do **Back-Face Culling** (`glEnable(GL_CULL_FACE)`) e teste de profundidade Z-buffer (`GL_DEPTH_TEST`).

---

## Controles da Aplicação

| Tecla / Comando | Ação |
| :--- | :--- |
| **W / S** ou **Seta Para Cima / Baixo** | Rotacionar câmera verticalmente (Pitch) |
| **A / D** ou **Seta Para Esquerda / Direita** | Rotacionar câmera horizontalmente (Yaw) |
| **+ / Q** ou **- / E** | Zoom In / Zoom Out (Aproximar / Afastar) |
| **P** | Alternar entre Projeção Perspectiva e Ortogonal |
| **R** | Resetar posição da câmera para o padrão |
| **ESC** | Sair da aplicação |

---

## Como Compilar e Executar

Certifique-se de possuir as dependências do OpenGL, GLUT e CMake instaladas em sua máquina.

### Dependências

* **Ubuntu / Debian / Mint:**
  ```bash
  sudo apt update
  sudo apt install build-essential cmake freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
  ```

* **Fedora / RHEL:**
  ```bash
  sudo dnf update
  sudo dnf install gcc gcc-c++ make cmake freeglut-devel mesa-libGL-devel mesa-libGLU-devel
  ```

* **Arch Linux / Manjaro:**
  ```bash
  sudo pacman -S base-devel cmake freeglut mesa glu
  ```

### Compilar o Projeto

```bash
# Entre na pasta raiz do projeto
cd opengl

# Crie e acesse o diretório de build
mkdir build && cd build

# Gere os arquivos de build com o CMake
cmake ..

# Compile o código-fonte
make
```

### 3. Executar o Programa

```bash
./window
```

---

## Estrutura do Repositório

```text
opengl/
├── CMakeLists.txt     # Script de compilação do CMake
├── src/
│   └── main.cpp       # Código-fonte principal (matrizes, iluminação, DDA e geometria)
└── README.md          # Documentação do projeto
```

---