Repositório desenvolvido para o trabalho prático da disciplina de **Computação Gráfica**, consistindo na modelagem de um ambiente virtual inspirado nas instalações do **Centro de Computação (C3)** da **Universidade Federal do Rio Grande (FURG)** utilizando **C++** e **OpenGL (Fixed-Function Pipeline)**.

---

## Tecnologias e Ferramentas

* **Linguagem:** C++ (C++17)
* **API Gráfica:** OpenGL / GLUT / FreeGLUT
* **Sistema de Build:** CMake

---

## Modificações Recentes no Modelo 3D

Mesa de Dois Níveis: Modelagem inspirada nos laboratórios do C3, contendo um nível inferior (para teclado, mouse e gabinete) e um nível superior elevado (onde o monitor fica perfeitamente encaixado e recuado para o fundo).
Periféricos e Hardware: Adicionados o gabinete desktop posicionado na horizontal, teclado centralizado e mouse ao lado direito, todos posicionados geometricamente com matrizes customizadas.
Ajuste de Cores e Materiais: Correção dos materiais e fontes de luz para evitar brilhos estourados em branco, aplicando tons realistas de madeira, metal escuro e display azulado simulando o monitor ligado.

---

## Status dos Requisitos e Funcionalidades
Modelagem Geométrica do Ambiente (25%): [IMPLEMENTADO]
Reprodução de uma mesa de laboratório avançada de dois níveis com computador, monitor, teclado, mouse e gabinete no C3/FURG.
Matrizes de Transformação Customizadas (`glMultMatrix`) (20%): [IMPLEMENTADO]
Funções matemáticas próprias para Translação, Rotação ($X$ e $Y$) e Escala, aplicadas via `glMultMatrixf()`.
Projeções Perspectiva e Ortogonal (15%): [IMPLEMENTADO]
Desenvolvimento e aplicação de matrizes matemáticas de projeção customizadas com alternância.
Iluminação e Tonalização (Phong / Gouraud) (15%): [PARCIALMENTE IMPLEMENTADO / EM REFINAMENTO]
Luzes e tonalização suave via OpenGL ativadas, necessitando de ajustes finos de reflexão especular e componentes de Phong.
Algoritmo de Rasterização — Bresenham / DDA (15%): [PENDENTE / TO DO]
Necessário implementar e aplicar na cena para traçado algorítmico de linhas.
Algoritmo de Visibilidade — Back-Face Culling (10%): [PARCIALMENTE IMPLEMENTADO]
Ativado via `glEnable(GL_CULL_FACE)`, faltando documentação de teste e validação aprofundada no relatório.

---

## TO DO / Pendências para Entrega
[ ] Implementar Algoritmo de Iluminação. Refinar Iluminação e Materiais: Ajustar os componentes especulares do modelo de Phong e verificar o comportamento das normais nos cubos primitivos.
[ ] Implementar Algoritmo de Rasterização: Desenvolver e aplicar uma rotina baseada no algoritmo de Bresenham ou DDA para rasterização de elementos gráficos no espaço virtual.
[ ] Implementar e validar Algoritmo de Visibilidade: Concluir os testes formais do Back-Face Culling e do Z-buffer para garantir que a ocultação de polígonos esteja otimizada.
[ ] Expandir o Ambiente: Adicionar um chão e cadeira, trazer mais elementos visuais característicos do C3.

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