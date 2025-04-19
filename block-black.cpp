#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

const int width = 600;
const int height = 600;
const int tile_size = 20;
const int num_tiles_x = width / tile_size;
const int num_tiles_y = height / tile_size;
const int num_blocks = 4;
const int offset = 50;

const string font_name = "Arial.ttf";
const string music_path = "Sounds/ttfaf.ogg";

uint16_t tiles[num_blocks] = {
    0b010111000,  // T
    0b111111111,  // 3x3
    0b000111111,  // 3x2
    0b110110110   // 2x3
};

array<sf::Color, num_blocks> colors = {
    sf::Color(20, 20, 155),
    sf::Color(100, 100, 0),
    sf::Color(0, 255, 40),
    sf::Color(255, 0, 0)
};

int grid[num_tiles_y][num_tiles_x] = {0};  

struct Pos {
    int x, y;
};

sf::Font font;

void draw_text(sf::RenderWindow &window, string value, sf::Vector2f position) {
    sf::Text text;
    text.setFont(font);
    text.setString(value);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    text.setPosition(position);
    window.draw(text);
}

void start_music(string file) {
    static sf::Music music;
    if (!music.openFromFile(file)) {
        cerr << "Failed to load music!" << endl;
        return;
    }
    music.setLoop(true);
    music.play();
}

void clear_row(int row) {
    for (int i = row; i > 0; i--) {
        for (int j = 0; j < num_tiles_x; j++) {
            grid[i][j] = grid[i - 1][j];
        }
    }
    for (int j = 0; j < num_tiles_x; j++) {
        grid[0][j] = 0;
    }
}

void check_lines() {
    for (int i = 0; i < num_tiles_y; i++) {
        int count = 0;
        for (int j = 0; j < num_tiles_x; j++) {
            if (grid[i][j] > 0) count++;
        }
        if (count == num_tiles_x) {
            clear_row(i);
        }
    }
}

void render_grid(sf::RenderWindow &window) {
    for (int i = 0; i < num_tiles_y; i++) {
        for (int j = 0; j < num_tiles_x; j++) {
            sf::RectangleShape tile(sf::Vector2f(tile_size - 1, tile_size - 1));
            tile.setPosition(j * tile_size, i * tile_size);
            if (grid[i][j] > 0) {
                tile.setFillColor(colors[grid[i][j] - 1]);
            } else {
                tile.setFillColor(sf::Color(200, 200, 200));
            }
            window.draw(tile);
        }
    }
}

void place_block(int x, int y, uint16_t shape, int color_index) {
    int index = 0;
    for (int i = 0; i < 3; i++) {  // 3x3 grid for shape
        for (int j = 0; j < 3; j++) {
            if (shape & (1 << (8 - index))) {
                int gx = x + j;
                int gy = y + i;
                if (gx >= 0 && gx < num_tiles_x && gy >= 0 && gy < num_tiles_y) {
                    grid[gy][gx] = color_index + 1;
                }
            }
            index++;
        }
    }
}

bool can_place_block(int x, int y, uint16_t shape) {
    int index = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (shape & (1 << (8 - index))) {
                int gx = x + j;
                int gy = y + i;
                if (gx >= num_tiles_x || gy >= num_tiles_y || grid[gy][gx] != 0) {
                    return false;
                }
            }
            index++;
        }
    }
    return true;
}

int main() {
    srand(time(0));

    if (!font.loadFromFile(font_name)) {
        cerr << "Failed to load font!" << endl;
        return -1;
    }

    sf::RenderWindow window(sf::VideoMode(width, height), "Block Blast - C++ SFML");
    window.setFramerateLimit(60);

    int block_type = rand() % num_blocks;
    int score = 0;

    start_music(music_path);

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                int mx = e.mouseButton.x / tile_size;
                int my = e.mouseButton.y / tile_size;

                uint16_t selected = tiles[block_type];
                if (can_place_block(mx, my, selected)) {
                    place_block(mx, my, selected, block_type);
                    check_lines();
                    block_type = rand() % num_blocks;
                    score += 10;
                }
            }
        }

        window.clear(sf::Color::White);
        render_grid(window);
        draw_text(window, "Score: " + to_string(score), sf::Vector2f(10.f, 10.f));
        window.display();
    }

    return 0;
}
