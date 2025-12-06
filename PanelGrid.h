#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>

class PanelGrid {
private:
    int rows, cols;
    float color[3];

    // Struktura za grafiku dugmeta (VAO, VBO, tekstura)
    struct ButtonGraphic {
        GLuint VAO;
        GLuint VBO;
        GLuint texture;
        float x0, y0, x1, y1; // koordinatni opseg za detekciju klika
    };
    std::vector<ButtonGraphic> buttons;

    // Struktura za stanje dugmeta (sprat, pressed, highlight)
    struct ButtonState {
        int floor;         // broj sprata
        bool pressed;      // da li je taster trenutno pritisnut
        bool highlight;    // da li da se crta uokviren
    };
    std::vector<ButtonState> floorButtons;

    // Putanje do tekstura dugmadi
    std::vector<std::string> texturePaths = {
        "resources/su.png",
        "resources/pr.png",
        "resources/number-one.png",
        "resources/number-2.png",
        "resources/number-3.png",
        "resources/number-four.png",
        "resources/number-five.png",
        "resources/number-six.png",
        "resources/close.png",
        "resources/open.png",
        "resources/stop.png",
        "resources/fan.png"
    };

public:
    // Konstruktor
    PanelGrid(float left, float right, float bottom, float top,
        int r, int c, float buttonWidth, float buttonHeight,
        float hSpacing, float vSpacing,
        float red = 0.6f, float green = 0.6f, float blue = 0.6f);

    // Crtanje svih dugmica
    void draw(GLuint shader);

    // Provera klika miša
    void checkClick(float mouseX, float mouseY, bool inLift);

    std::vector<ButtonState>& getFloorButtons() {
        return floorButtons;
    }
    // Destruktor
    ~PanelGrid();
};
