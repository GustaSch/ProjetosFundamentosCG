# Lista 3 – Criando Triângulos a partir do Clique do Mouse

Processamento Gráfico: Fundamentos – UNISINOS
Aluno: Gustavo Ribeiro Schwert

## Como compilar e executar

C++17, GLFW, GLM e GLAD, via `CMakeLists` na raiz do repositório:

```bash
cmake -S . -B build
cmake --build build
./build/L3Ex1
```

## Como usar

- **Botão esquerdo do mouse:** cria 1 vértice na posição do clique.
- A cada 3 vértices, forma-se um triângulo com uma cor nova.
- Vértices que ainda não completaram um triângulo aparecem como pontos.
- **C:** limpa a tela. **ESC:** fecha o programa.
- O título da janela mostra a quantidade de vértices e de triângulos criados.

## Como funciona

- **Projeção:** `glm::ortho(0, largura, altura, 0)`, com a janela do mundo igual à janela da aplicação.
  Cada unidade do mundo é um pixel e a posição do cursor (`glfwGetCursorPos`) já é a coordenada do mundo.
- **Buffers:** um único VAO/VBO com vértices intercalados `(x, y, r, g, b)`. A cada clique o vetor
  de vértices é reenviado com `glBufferData`.
- **Desenho:** os vértices em múltiplos de 3 são desenhados com `GL_TRIANGLES` e os pendentes
  (no máximo 2) com `GL_POINTS`.
- **Cores:** o primeiro vértice de cada grupo de 3 define a cor do triângulo. O matiz (HSV) avança
  em 0,618 a cada triângulo, o que gera cores distintas das anteriores.
- **Eventos:** `glfwSetMouseButtonCallback`, conforme o GLFW Input Guide.
