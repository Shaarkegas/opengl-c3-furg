Repositório desenvolvido para o trabalho prático da disciplina de **Computação Gráfica**, consistindo na modelagem de um ambiente virtual inspirado nas instalações do **Centro de Computação (C3)** da **Universidade Federal do Rio Grande (FURG)** utilizando **C++** e **OpenGL (Fixed-Function Pipeline)**.

---

## Tecnologias e Ferramentas

* **Linguagem:** C++ (C++17)
* **API Gráfica:** OpenGL / GLUT / FreeGLUT
* **Sistema de Build:** CMake

---

## Requisitos e Funcionalidades Implementadas

O projeto foi estruturado para contemplar integralmente os critérios de avaliação solicitados:

1. **Modelagem Geométrica do Ambiente (25%):**
* Reprodução de elementos que fazem referência ao C3/FURG (como uma mesa de laboratório em 3D no centro da cena composta por tampo e pernas estruturadas).


2. **Matrizes de Transformação Customizadas (`glMultMatrix`) (20%):**
* Implementação matemática própria para funções de **Translação**, **Rotação** ($X$ e $Y$) e **Escala**, aplicadas através de matrizes homogêneas customizadas via `glMultMatrixf()`.


3. **Projeções Perspectiva e Ortogonal (15%):**
* Desenvolvimento de funções matemáticas para as matrizes de projeção **Perspectiva** e **Ortogonal**, permitindo a alternância de visualização da cena.


4. **Iluminação e Tonalização (Phong / Gouraud) (15%):**
* Configuração de fontes de luz pontuais e materiais com reflexão difusa e ambiente, utilizando o modelo de tonalização suave (`glShadeModel(GL_SMOOTH)` / Gouraud Shading) nativo do OpenGL.


5. **Algoritmo de Rasterização — Bresenham (15%):**
* Implementação de algoritmo clássico de rasterização para traçado de linhas 2D/3D no ambiente.


6. **Algoritmo de Visibilidade — Back-Face Culling (10%):**
* Ativação e configuração do teste de visibilidade de faces traseiras (`glEnable(GL_CULL_FACE)`), otimizando a renderização e ocultando polígonos não visíveis.



---

## Como Compilar e Executar (Linux / Ubuntu)

Certifique-se de possuir as dependências do OpenGL, GLUT e CMake instaladas em sua máquina.

### 1. Instalar dependências (caso não possua)

```bash
sudo apt update
sudo apt install build-essential cmake freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev

```

### 2. Clonar o repositório e compilar via CMake

```bash
# Entre na pasta do projeto
cd opengl

# Crie e entre no diretório de build
mkdir build && cd build

# Gere os arquivos de build com o CMake
cmake ..

# Compile o projeto
make

```

### 3. Executar o programa

```bash
./window