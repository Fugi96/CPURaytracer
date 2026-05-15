#include "Scenes.h"
#include "objects/Camera.h"
#include "objects/Pattern.h"
#include "objects/Plane.h"
#include "objects/World.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <thread>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"   TexCoord = aTexCoord;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"    FragColor = texture(ourTexture, TexCoord);\n"
"}\0";

World* scene1()
{
    std::shared_ptr<Plane> floor = std::make_shared<Plane>(Plane("floor"));
    floor->material.m_color = Color(1.0f, 0.9f, 0.9f);
    floor->material.m_reflective = 0.6f;
    Pattern p = Patterns::Checkers({0, 0, 0}, {1, 0.5f, 0.5f});
    p.set_transform(Matrix<4,4>::rotation_y(PI/4));
    floor->material.pattern = p;

    std::shared_ptr<Plane> left_wall = std::make_shared<Plane>(Plane("left_wall"));
    left_wall->set_transform(
        Matrix<4, 4>::identity()
        .rotate_x(PI / 2.0f)
        .rotate_y(-PI / 4.0f)
        .translate(0.0f, 0.0f, 15.0f));
    left_wall->material.m_color = Color(1.0f, 0.9f, 0.9f);

    std::shared_ptr<Plane> right_wall = std::make_shared<Plane>(Plane("right_wall"));
    right_wall->set_transform(
        Matrix<4, 4>::identity()
        .rotate_x(PI / 2.0f)
        .rotate_y(PI / 4.0f)
        .translate(0.0f, 0.0f, 15.0f));
    right_wall->material.m_color = Color(1.0f, 0.9f, 0.9f);

    std::shared_ptr<Sphere> middle = std::make_shared<Sphere>(Sphere("middle"));
    middle->set_transform(Matrix<4,4>::translation(-0.5f, 1.0f, 0.5f));
    middle->material.m_color = Color(0.1f, 1.0f, 0.5f);
    middle->material.m_diffuse = 0.8f;
    middle->material.m_specular = 0.9f;
    middle->material.m_reflective = 0.2f;

    std::shared_ptr<Sphere> right = std::make_shared<Sphere>(Sphere("right"));
    right->set_transform(
        Matrix<4, 4>::identity()
        .scale(0.5f, 0.5f, 0.5f)
        .translate(1.5f, 0.5f, -1.5f));
    right->material.m_color = Color(1, 1, 1);
    right->material.m_ambient = 0.0f;
    right->material.m_diffuse = 0.05f;
    right->material.m_specular = 1.0f;
    right->material.m_shininess = 300.0f;
    right->material.m_reflective = 1.0f;

    std::shared_ptr<Sphere> left = std::make_shared<Sphere>(Sphere("left"));
    left->set_transform(
        Matrix<4, 4>::identity()
        .scale(0.33f, 0.33f, 0.33f)
        
        .translate(-1.5f, 0.33f, -0.75f));
    left->material.m_color = Color(1.0f, 0.8f, 0.1f);
    left->material.m_diffuse = 0.7f;
    left->material.m_specular = 0.3f;

    Light light = Light(Point(-10.0f, 10.0f, -10.0f), Color(1.0f, 1.0f, 1.0f));

    World* world = new World();
    world->light = light;
    //world->add_object(left_wall);
    world->add_object(floor);
    //world->add_object(right_wall);
    world->add_object(middle);
    world->add_object(left);
    world->add_object(right);

    return world;
}

World* scene2() {
    World* world = new World();
    world->light = Light(
        Point(-8.0f, 10.0f, -10.0f),
        Color(1.0f, 1.0f, 1.0f)
    );
    std::shared_ptr<Plane> floor = std::make_shared<Plane>("floor");

    floor->material.m_color = Color(1,1,1);
    floor->material.m_specular = 0.2f;
    floor->material.m_reflective = 0.5f;

    Pattern floor_pattern =
        Patterns::Checkers(
            Color(0.05f, 0.05f, 0.05f),
            Color(0.85f, 0.85f, 0.85f)
        );

    floor_pattern.set_transform(
        Matrix<4,4>::identity()
            .scale(0.35f, 0.35f, 0.35f)
            .rotate_y(PI / 4.0f)
    );

    floor->material.pattern = floor_pattern;

    world->add_object(floor);

    std::shared_ptr<Plane> back_wall =
    std::make_shared<Plane>("back_wall");

    back_wall->set_transform(
        Matrix<4,4>::identity()
            .rotate_x(PI / 2.0f)
            .translate(0.0f, 0.0f, 12.0f)
    );

    Pattern wall_pattern =
        Patterns::Gradient(
            Color(0.1f, 0.1f, 0.3f),
            Color(0.6f, 0.1f, 0.7f)
        );

    wall_pattern.set_transform(
        Matrix<4,4>::identity()
            .scale(4.0f, 4.0f, 4.0f)
    );

    back_wall->material.pattern = wall_pattern;
    back_wall->material.m_specular = 0.0f;

    world->add_object(back_wall);

    std::shared_ptr<Sphere> mirror =
    std::make_shared<Sphere>("mirror");

    mirror->set_transform(
        Matrix<4,4>::translation(0.0f, 1.2f, 0.0f)
    );

    mirror->material.m_color = Color(1,1,1);
    mirror->material.m_ambient = 0.0f;
    mirror->material.m_diffuse = 0.05f;
    mirror->material.m_specular = 1.0f;
    mirror->material.m_shininess = 400.0f;
    mirror->material.m_reflective = 0.95f;

    //world->add_object(mirror);

    std::shared_ptr<Sphere> striped =
    std::make_shared<Sphere>("striped");

    striped->set_transform(
        Matrix<4,4>::identity()
            .scale(0.7f, 0.7f, 0.7f)
            .translate(-2.0f, 0.7f, -1.5f)
    );

    Pattern stripes =
        Patterns::Stripe(
            Color(1.0f, 0.2f, 0.2f),
            Color(1.0f, 0.9f, 0.2f)
        );

    stripes.set_transform(
        Matrix<4,4>::identity()
            .rotate_z(PI / 6.0f)
            .scale(0.15f, 0.15f, 0.15f)
    );

    striped->material.pattern = stripes;
    striped->material.m_diffuse = 0.7f;
    striped->material.m_specular = 0.5f;
    striped->material.m_shininess = 100.0f;

    world->add_object(striped);

    std::shared_ptr<Sphere> ring =
        std::make_shared<Sphere>("ring");

    ring->set_transform(
        Matrix<4,4>::identity()
            .scale(0.9f, 0.9f, 0.9f)
            .translate(2.2f, 0.9f, -8.0f)
    );

    Pattern rings =
        Patterns::Ring(
            Color(0.2f, 0.8f, 1.0f),
            Color(0.0f, 0.1f, 0.3f)
        );

    rings.set_transform(
        Matrix<4,4>::identity()
            .scale(0.2f, 0.2f, 0.2f)
    );

    ring->material.pattern = rings;
    ring->material.m_diffuse = 0.7f;
    ring->material.m_specular = 0.9f;
    ring->material.m_shininess = 300.0f;
    ring->material.m_reflective = 0.3f;

    world->add_object(ring);

    std::shared_ptr<Sphere> foreground =
    std::make_shared<Sphere>("foreground");

    foreground->set_transform(
        Matrix<4,4>::identity()
            .scale(0.3f, 0.3f, 0.3f)
            .translate(-0.8f, 0.3f, -2.5f)
    );

    foreground->material.m_color =
        Color(1.0f, 0.9f, 1.0f);

    foreground->material.m_diffuse = 0.6f;
    foreground->material.m_specular = 1.0f;
    foreground->material.m_shininess = 500.0f;
    foreground->material.m_reflective = 0.6f;

    world->add_object(foreground);

    return world;
}

World* sceneObservatory() {
    World* world = new World();
    
    world->light = Light(
    Point(3.0f, 7.0f, -8.0f),
    Color(1.0f, 1.0f, 1.0f));

    std::shared_ptr<Plane> floor =
    std::make_shared<Plane>("floor");

    Pattern floor_pattern =
        Patterns::Checkers(
            Color(0.08f, 0.08f, 0.08f),
            Color(0.55f, 0.55f, 0.55f)
        );

    floor_pattern.set_transform(
        Matrix<4,4>::identity()
            .scale(0.25f, 0.25f, 0.25f)
            .rotate_y(PI / 4.0f)
    );

    floor->material.pattern = floor_pattern;
    floor->material.m_specular = 0.8f;
    floor->material.m_shininess = 300.0f;
    floor->material.m_reflective = 0.65f;

    world->add_object(floor);

    std::shared_ptr<Plane> left_wall =
    std::make_shared<Plane>("left_wall");

    left_wall->set_transform(
        Matrix<4,4>::identity()
            .rotate_x(PI / 2.0f)
            .rotate_y(1.2f)
            .translate(0, 0.0f, 6.5f)
    );

    left_wall->material.m_color = Color(0.85f, 1, 0.9f);
    left_wall->material.m_ambient = 0.0f;
    left_wall->material.m_specular = 1.0f;
    left_wall->material.m_diffuse = 0.03f;
    left_wall->material.m_shininess = 500.0f;
    left_wall->material.m_reflective = 0.95;

    world->add_object(left_wall);


    //world->add_object(right_wall);

    std::shared_ptr<Sphere> centerpiece =
    std::make_shared<Sphere>("centerpiece");

    centerpiece->set_transform(
        Matrix<4,4>::translation(1.0f, 1.0f, -1.0f)
    );

    Pattern rings =
        Patterns::Ring(
            Color(0.1f, 0.7f, 1.0f),
            Color(0.0f, 0.05f, 0.2f)
        );

    rings.set_transform(
        Matrix<4,4>::identity()
            .scale(0.12f, 0.12f, 0.12f)
    );

    //centerpiece->material.pattern = rings;
    centerpiece->material.m_color = Color(1, 1, 1);
    centerpiece->material.m_ambient = 0;
    centerpiece->material.m_diffuse = 0.02f;
    centerpiece->material.m_specular = 1.0f;
    centerpiece->material.m_shininess = 500.0f;
    centerpiece->material.m_reflective = 1.0f;

    world->add_object(centerpiece);

    std::shared_ptr<Sphere> mirror =
    std::make_shared<Sphere>("mirror");

    mirror->set_transform(
        Matrix<4,4>::identity()
            .scale(0.6f, 0.6f, 0.6f)
            .translate(2.8f, 0.6f, -1.5f)
    );

    mirror->material.m_color = Color(1,1,1);
    mirror->material.m_ambient = 0.0f;
    mirror->material.m_diffuse = 0.02f;
    mirror->material.m_specular = 1.0f;
    mirror->material.m_shininess = 500.0f;
    mirror->material.m_reflective = 1.0f;

    //world->add_object(mirror);

    std::shared_ptr<Sphere> planet =
    std::make_shared<Sphere>("planet");

    planet->set_transform(
        Matrix<4,4>::identity()
            .scale(1.2f, 1.2f, 1.2f)
            .translate(-2.0f, 1.2f, 1.5f)
    );

    Pattern planet_pattern =
        Patterns::Stripe(
            Color(1.0f, 0.5f, 0.1f),
            Color(0.5f, 0.1f, 0.05f)
        );

    planet_pattern.set_transform(
        Matrix<4,4>::identity()
            .rotate_z(PI / 4.0f)
            .scale(0.15f, 0.15f, 0.15f)
    );

    planet->material.pattern = planet_pattern;
    planet->material.m_diffuse = 0.8f;
    planet->material.m_specular = 0.9f;
    planet->material.m_shininess = 150.0f;

    world->add_object(planet);

    std::shared_ptr<Sphere> gold =
    std::make_shared<Sphere>("gold");

    gold->set_transform(
        Matrix<4,4>::identity()
            .scale(0.35f, 0.35f, 0.35f)
            .translate(-3.0f, 0.35f, -3.0f)
    );

    gold->material.m_color =
        Color(1.0f, 0.84f, 0.0f);

    gold->material.m_diffuse = 0.6f;
    gold->material.m_specular = 1.0f;
    gold->material.m_shininess = 250.0f;
    gold->material.m_reflective = 0.5f;

    world->add_object(gold);

    std::shared_ptr<Sphere> blue =
    std::make_shared<Sphere>("blue");

    blue->set_transform(
        Matrix<4,4>::identity()
            .scale(0.5f, 0.5f, 0.5f)
            .translate(1.2f, 0.55f, -4.0f)
    );

    blue->material.m_color =
        Color(0.1f, 0.4f, 1.0f);

    blue->material.m_diffuse = 0.7f;
    blue->material.m_specular = 1.0f;
    blue->material.m_shininess = 300.0f;

    world->add_object(blue);

    return world;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, bool& restart_render)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        restart_render = true;
    }
}

int mainWindowLoop()
{
    World* world = sceneObservatory();

    Camera camera = Camera(3840*2, 2160*2, PI / 3);

    camera.set_transform(
    Matrix<4,4>::view_transform(
        Point(-2.5f, 2.0f, -9.0f),
        Point(0.0f, 1.0f, 0.0f),
        Vector(0.0f, 1.0f, 0.0f)));

    bool restart_render = false;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Best Raytracer Ever", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << '\n';
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << '\n';
        return -1;
    }

    glViewport(0, 0, 1920, 1080);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << '\n';
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << '\n';
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << '\n';
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // top right
         1.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f    // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Canvas canvas = Canvas(camera.hsize, camera.vsize);
    std::thread rendering([&camera, world, &canvas, &restart_render, &window]() {
        camera.benchmark(*world, canvas, restart_render);
        //glfwSetWindowShouldClose(window, true);
        });

    std::vector<Color> color = canvas.texture();
    float* magic = reinterpret_cast<float*>(color.data());

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB32F,
        canvas.m_width,
        canvas.m_height,
        0,
        GL_RGB,
        GL_FLOAT,
        magic
    );

    while (!glfwWindowShouldClose(window)) {
        processInput(window, restart_render);

        if (restart_render) {
        
            if (rendering.joinable()) {
                rendering.join();
            }
        
            restart_render = false;
        
            canvas.reset_image();
        
            rendering = std::thread(
                [&camera, &world, &canvas, &restart_render, &window]() {
                    camera.benchmark(*world, canvas, restart_render);
                    glfwSetWindowShouldClose(window, true);
                }
            );
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        color = canvas.texture();
        magic = reinterpret_cast<float*>(color.data());

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB32F,
            canvas.m_width,
            canvas.m_height,
            0,
            GL_RGB,
            GL_FLOAT,
            magic
        );

        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    rendering.join();

    return 0;
}

void test(Canvas c)
{

}
