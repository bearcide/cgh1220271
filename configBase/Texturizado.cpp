#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"


// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void SetMode(int mode);
void CargarTextura(const char* ruta, GLuint& textura, bool usaAlpha);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// VBO global para poder actualizar las UVs en runtime
GLuint VBO, VAO, EBO;

// Texturas globales
GLuint texturaChecker, texturaVentana;

// Modo actual (6 modos en total)
// 0 = Ejercicio 1: Cuadrante inferior derecho (azul)     - checker
// 1 = Ejercicio 2: Cuadrante superior izquierdo          - checker
// 2 = Ejercicio 3: Centro de la imagen                   - checker
// 3 = Ejercicio 4: Todos los cuadrantes (textura completa) - checker
// 4 = Ejercicio 5: Repeticion 3.5 x 5.5 de checker       - checker
// 5 = Ejercicio 6: Una sola ventana                      - window
int currentMode = 0;

// Estructura base: posiciones y colores fijos, UVs cambian segun modo
GLfloat basePositionsAndColors[] =
{
    // Positions            // Colors
    -0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 1.0f,   // 0: inf-izq
     0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 1.0f,   // 1: inf-der
     0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 1.0f,   // 2: sup-der
    -0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 1.0f,   // 3: sup-izq
};

// UVs para cada uno de los 6 modos.
// Orden de cada fila: vertice 0, 1, 2, 3 -> (u,v) consecutivos
// Recordatorio: vertice 0 = inf-izq, 1 = inf-der, 2 = sup-der, 3 = sup-izq
GLfloat uvsPerMode[6][8] =
{
    // ============================================================
    // Modo 0: Ejercicio 1 - Cuadrante INFERIOR DERECHO (azul)
    // U: 0.5 .. 1.0    V: 0.0 .. 0.5
    // ============================================================
    { 0.5f, 0.0f,
      1.0f, 0.0f,
      1.0f, 0.5f,
      0.5f, 0.5f },

      // ============================================================
      // Modo 1: Ejercicio 2 - Cuadrante SUPERIOR IZQUIERDO
      // U: 0.0 .. 0.5    V: 0.5 .. 1.0
      // ============================================================
      { 0.0f, 0.5f,
        0.5f, 0.5f,
        0.5f, 1.0f,
        0.0f, 1.0f },

        // ============================================================
        // Modo 2: Ejercicio 3 - CENTRO de la imagen
        // U: 0.25 .. 0.75    V: 0.25 .. 0.75
        // ============================================================
        { 0.25f, 0.25f,
          0.75f, 0.25f,
          0.75f, 0.75f,
          0.25f, 0.75f },

          // ============================================================
          // Modo 3: Ejercicio 4 - TODOS LOS CUADRANTES (textura completa)
          // U: 0.0 .. 1.0    V: 0.0 .. 1.0
          // ============================================================
          { 0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f },

            // ============================================================
            // Modo 4: Ejercicio 5 - REPETICION 3.5 x 5.5 de checker
            // U: 0.0 .. 3.5    V: 0.0 .. 5.5
            // ============================================================
            { 0.0f, 0.0f,
              3.5f, 0.0f,
              3.5f, 5.5f,
              0.0f, 5.5f },

              // ============================================================
              // Modo 5: Ejercicio 6 - UNA SOLA VENTANA
              // U: 0.0 .. 1.0    V: 0.0 .. 1.0
              // ============================================================
              { 0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f },
};

GLuint indices[] =
{
    0, 1, 3,
    1, 2, 3
};


// The MAIN function
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Texturizado - Practica 6", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // Build and compile our shader program
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

    // Buffer setup
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Reservamos espacio para 4 vertices * 8 floats (3 pos + 3 color + 2 uv)
    glBufferData(GL_ARRAY_BUFFER, 4 * 8 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Texture Coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Cargamos el modo inicial
    SetMode(currentMode);

    // Cargar ambas texturas al inicio
    stbi_set_flip_vertically_on_load(true);
    CargarTextura("images/checker_Tex.png", texturaChecker, false);
    CargarTextura("images/window.png", texturaVentana, true);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lampShader.Use();

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 model(1);

        GLint modelLoc = glGetUniformLocation(lampShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lampShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lampShader.Program, "projection");

        // Elegir textura segun el modo actual
        // Modo 5 -> ventana, todos los demas -> checker
        GLuint texturaActiva = (currentMode == 5) ? texturaVentana : texturaChecker;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturaActiva);

        // Pasar el sampler al shader
        GLint texLoc = glGetUniformLocation(lampShader.Program, "ourTexture");
        glUniform1i(texLoc, 0);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texturaChecker);
    glDeleteTextures(1, &texturaVentana);
    glfwTerminate();

    return 0;
}


// Carga una textura desde archivo y la deja lista para usar
void CargarTextura(const char* ruta, GLuint& textura, bool usaAlpha)
{
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);

    // Parametros de wrapping y filtrado
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int textureWidth, textureHeight, nrChannels;
    unsigned char* image = stbi_load(ruta, &textureWidth, &textureHeight, &nrChannels, 0);

    if (image)
    {
        GLenum formato = usaAlpha ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, formato, textureWidth, textureHeight,
            0, formato, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);
        std::cout << "Textura cargada: " << ruta << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture: " << ruta << std::endl;
    }
}


// Reconstruye el array de vertices con las UVs del modo pedido y lo sube al VBO
void SetMode(int mode)
{
    if (mode < 0 || mode > 5) return;

    // Armamos el array completo: 4 vertices * 8 floats
    GLfloat vertices[4 * 8];

    for (int i = 0; i < 4; i++)
    {
        // Posiciones + colores (3 + 3 floats)
        vertices[i * 8 + 0] = basePositionsAndColors[i * 6 + 0];
        vertices[i * 8 + 1] = basePositionsAndColors[i * 6 + 1];
        vertices[i * 8 + 2] = basePositionsAndColors[i * 6 + 2];
        vertices[i * 8 + 3] = basePositionsAndColors[i * 6 + 3];
        vertices[i * 8 + 4] = basePositionsAndColors[i * 6 + 4];
        vertices[i * 8 + 5] = basePositionsAndColors[i * 6 + 5];

        // UVs del modo actual
        vertices[i * 8 + 6] = uvsPerMode[mode][i * 2 + 0];
        vertices[i * 8 + 7] = uvsPerMode[mode][i * 2 + 1];
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    currentMode = mode;

    const char* nombres[] = {
        "Ejercicio 1: Cuadrante inferior derecho (azul)",
        "Ejercicio 2: Cuadrante superior izquierdo",
        "Ejercicio 3: Centro de la imagen",
        "Ejercicio 4: Todos los cuadrantes (textura completa)",
        "Ejercicio 5: Repeticion 3.5 x 5.5",
        "Ejercicio 6: Una sola ventana"
    };
    std::cout << nombres[mode] << std::endl;
}


// Moves/alters the camera positions based on user input
void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Cambiar de modo con las teclas 1..6
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_1) SetMode(0);  // Cuadrante inf-der
        if (key == GLFW_KEY_2) SetMode(1);  // Cuadrante sup-izq
        if (key == GLFW_KEY_3) SetMode(2);  // Centro
        if (key == GLFW_KEY_4) SetMode(3);  // Todos los cuadrantes
        if (key == GLFW_KEY_5) SetMode(4);  // Repeticion 3.5x5.5
        if (key == GLFW_KEY_6) SetMode(5);  // Una sola ventana
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}


void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = (GLfloat)xPos;
        lastY = (GLfloat)yPos;
        firstMouse = false;
    }

    GLfloat xOffset = (GLfloat)xPos - lastX;
    GLfloat yOffset = lastY - (GLfloat)yPos;

    lastX = (GLfloat)xPos;
    lastY = (GLfloat)yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}