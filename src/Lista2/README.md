# Lista 2 – Matriz de Projeção Ortográfica e mapeamento com a Viewport

Processamento Gráfico: Fundamentos – UNISINOS
Aluno: Gustavo Ribeiro Schwert

## Como compilar e executar

Os exercícios usam C++17, GLFW, GLM e GLAD e fazem parte do `CMakeLists` na raiz do repositório
(um executável por exercício).

```bash
cmake -S . -B build
cmake --build build
./build/L2Ex1     # ... L2Ex2, L2Ex3, L2Ex4, L2Ex5, L2Ex6
```

Em todos os exercícios, `ESC` fecha a janela.

## Estrutura

| Pasta | Exercício | O que faz |
| ----- | --------- | --------- |
| `L2Ex1/` | 1 | `ortho(-10, 10, -10, 10)`: origem no centro, y para cima |
| `L2Ex2/` | 2 | `ortho(0, 800, 600, 0)`: origem no canto superior esquerdo, y para baixo |
| `L2Ex3/` | 3 | Retângulo, quadrado e triângulo posicionados em pixels; clique imprime a posição no terminal |
| `L2Ex4/` | 4 | Viewport restrita ao quadrante superior direito |
| `L2Ex5/` | 5 | Mesma cena nos 4 quadrantes (4 chamadas de `glViewport`) |
| `L2Ex6/` | 6 | Triângulos a partir do clique do mouse (mesmo código da Lista 3) |
| `helpers.h` | – | Janela, shaders e malha simples compartilhados pelos Ex1–Ex5 |

## Respostas

### Exercício 3 – O que acontece quando posicionamos os objetos? Por que é útil essa configuração?

Com a janela do mundo `(xmin=0, xmax=800, ymin=600, ymax=0)` e uma janela de 800x600, cada
unidade do mundo corresponde a exatamente um pixel. Por isso:

- Os objetos são posicionados diretamente em pixels. Um objeto em `(100, 50)` aparece a 100 px da borda
  esquerda e 50 px do topo.
- A origem é o canto superior esquerdo e o eixo y cresce para baixo, igual ao sistema de coordenadas
  de telas, imagens e do mouse na GLFW (`glfwGetCursorPos`). Não é preciso converter nada ao tratar cliques.
- Isso é muito útil para jogos 2D, sprites, interfaces e editores, em que se pensa em "pixels da tela".

Uma consequência: a geometria do Ex1 (valores entre -5 e 5) quase não apareceria nesta câmera,
porque 10 unidades de largura viram só 10 pixels, no canto superior esquerdo. Os objetos precisam ser
descritos em coordenadas de pixel (`L2Ex2` e `L2Ex3`).

### Observações

- No Ex1 a janela do mundo é quadrada (20x20) e a janela é 800x600, então o triângulo aparece esticado
  horizontalmente. Já `(0, 800, 600, 0)` tem a mesma proporção da janela, sem distorção.
- O viewport do OpenGL tem origem no canto inferior esquerdo, ao contrário da câmera do Ex2.
  Por isso o quadrante superior direito é `glViewport(largura/2, altura/2, largura/2, altura/2)`.
- `glClear` ignora o viewport. Para pintar o fundo de um quadrante usei `GL_SCISSOR_TEST`.
