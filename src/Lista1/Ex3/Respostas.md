# Exercício 3 – Buffers (VBO/VAO) e Atributos de Vértice

Triângulo formado pelos vértices **P1**, **P2** e **P3**, coloridos respectivamente em **vermelho**, **verde** e **azul**.

## a) Configuração dos buffers (VBO, VAO)

Cada vértice do triângulo carrega dois atributos: a posição `(x, y, z)` e a cor `(r, g, b)`. A forma mais simples de representar isso é com um único **VBO** contendo os dados entrelaçados (interleaved), ou seja, posição e cor de cada vértice alternadas sequencialmente no mesmo array:

```
P1.x  P1.y  P1.z  P1.r  P1.g  P1.b
P2.x  P2.y  P2.z  P2.r  P2.g  P2.b
P3.x  P3.y  P3.z  P3.r  P3.g  P3.b
```

Cada linha acima tem 6 floats, então o **stride** (passo entre um vértice e o próximo) é `6 * sizeof(float)`.

O **VAO** guarda a configuração de como ler esse VBO, através de dois atributos:

| Atributo | Localização (`location`) | Componentes | Offset                | Stride              |
| -------- | ------------------------- | ----------- | ---------------------- | ------------------- |
| Posição  | 0                          | 3 (x, y, z) | `0`                     | `6 * sizeof(float)` |
| Cor      | 1                          | 3 (r, g, b) | `3 * sizeof(float)`     | `6 * sizeof(float)` |

Em código:

```cpp
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// Atributo 0: posicao
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// Atributo 1: cor
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
```

## b) Como esses atributos são identificados no *vertex shader*?

Os atributos são identificados pelo índice de `location`, que deve bater exatamente com o índice usado no `glVertexAttribPointer`/`glEnableVertexAttribArray` do VAO. No *vertex shader*, isso é declarado com o qualificador `layout(location = N)`:

```glsl
#version 460 core
layout (location = 0) in vec3 aPos;    // corresponde ao atributo 0 (posicao)
layout (location = 1) in vec3 aColor;  // corresponde ao atributo 1 (cor)

out vec3 vColor; // repassado para o fragment shader

void main() {
    gl_Position = vec4(aPos, 1.0);
    vColor = aColor;
}
```

A GPU associa automaticamente cada `location` declarado no shader com o índice configurado no VAO — é essa correspondência de índices que faz a posição e a cor corretas chegarem a cada vértice processado.

No *fragment shader*, a cor recebida (`vColor`) é interpolada automaticamente entre os três vértices, o que produz o efeito de gradiente colorido dentro do triângulo:

```glsl
#version 460 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
```
