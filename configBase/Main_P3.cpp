#include <iostream>
#include <vector>
#include <cstdlib> 
#include <ctime>   

//#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shaders
#include "Shader.h"

const GLint WIDTH = 800, HEIGHT = 600;

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

glm::mat4 crearTransformacionAleatoria() {
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 traslacion(randomFloat(-5.0f, 5.0f), randomFloat(-5.0f, 5.0f), randomFloat(-5.0f, 5.0f));
    model = glm::translate(model, traslacion);
    float angulo = randomFloat(0.0f, 360.0f);
    glm::vec3 ejeRotacion(randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f));
    if (glm::length(ejeRotacion) > 0.0f) {
        ejeRotacion = glm::normalize(ejeRotacion);
    }
    else {
        ejeRotacion = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    model = glm::rotate(model, glm::radians(angulo), ejeRotacion);
    float escala = randomFloat(0.0f, 1.0f);
    model = glm::scale(model, glm::vec3(escala, escala, escala));

    return model;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecciones y transformaciones basicas", nullptr, nullptr);
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (nullptr == window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit()) {
        std::cout << "Failed to initialise GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader ourShader("Shader/core.vs", "Shader/core.frag");

    float vertices[] = {
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Front
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Back
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

        -0.5f,  0.5f, -0.5f, 1.0f, 0.2f, 0.5f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.2f, 0.5f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.2f, 0.5f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.2f, 0.5f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.2f, 0.5f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.2f, 0.5f,
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);
   
    std::vector<glm::mat4> cubosEscena;
    for (int i = 0; i < 1000000; i++) { 
        cubosEscena.push_back(crearTransformacionAleatoria());
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projecLoc = glGetUniformLocation(ourShader.Program, "projection");

        glUniformMatrix4fv(projecLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(VAO);

        for (glm::mat4 cuboModel : cubosEscena) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cuboModel));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return EXIT_SUCCESS;
}