#include <iostream>
#include <vector>
#include <time.h>
#include <SFML/Graphics.hpp>

using namespace std;

bool playing = true;
bool first_click = true;

int mine_num{};

const float SCALE {1.5f};
const int WIDTH {20};
const int HEIGHT {20};
const float CELL_PIX_SIZE {16.f * SCALE};
const int CELLS {WIDTH*HEIGHT};
const float TOP_MARGIN {CELL_PIX_SIZE * 1.5f};
const float MARGIN {10.f};
const float WINDOW_WIDTH {WIDTH * CELL_PIX_SIZE + MARGIN * 2};
const float WINDOW_HEIGHT {HEIGHT * CELL_PIX_SIZE + TOP_MARGIN + MARGIN * 2};

vector<sf::Sprite> tiles;
vector<sf::Sprite> faces;
vector<sf::Sprite> numbers;
vector<int> mines(CELLS);
vector<vector<int>> number_tiles(HEIGHT);
vector<vector<int>> visible_tiles(HEIGHT);

enum E_tiles {none, t1, t2, t3, t4, t5, t6, t7, t8, undiscovered, flag, question, pressed_mine, mine, no_mine};
sf::RenderTexture renderTiles;
sf::RectangleShape rectTiles;

void draw(sf::RenderWindow &window);
void initializeVectors();
void minesPos(vector<int> &vec, int num, sf::Vector2i pos);
bool loadSprites(vector<sf::Sprite> &vec, sf::Texture &texture, const string file, int sprite_num);
void mouse(sf::Event &event);
extern void fillTiles(vector<vector<int>> &tiles, vector<vector<int>> &visible_tiles, sf::Vector2u pos);

int main() {

    mine_num = int(CELLS/5.f); //podrá dividir teniendo en cuenta dificultad

    srand(time(NULL));
    initializeVectors();

    sf::Texture textureFaces, textureNumbers, textureTiles;
    if (!loadSprites(faces, textureFaces, "Sprites/faces.png", 5)) return EXIT_FAILURE;
    if (!loadSprites(numbers, textureNumbers, "Sprites/numbers.png", 10)) return EXIT_FAILURE;
    if (!loadSprites(tiles, textureTiles, "Sprites/tiles.png", 15)) return EXIT_FAILURE;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "MineSweeper2", sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60);

    //Tiles RenderTexture
    uint16_t tilesWidth = WIDTH * CELL_PIX_SIZE;
    uint16_t tilesHeight = HEIGHT * CELL_PIX_SIZE;
    renderTiles.create(tilesWidth, tilesHeight);
    rectTiles.setSize(sf::Vector2f(tilesWidth, tilesHeight));
    rectTiles.setPosition(sf::Vector2f(MARGIN, TOP_MARGIN + MARGIN));


    while (playing) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    playing = false;
                    break;

                case sf::Event::MouseButtonPressed:
                    mouse(event);
                    break;

                default:
                    break;
            }
        }

        window.clear(sf::Color(0x8f,0xbc,0x8f));
        draw(window);
        window.display();
    }
    return EXIT_SUCCESS;

}

void draw(sf::RenderWindow &window) {

    renderTiles.clear(sf::Color::White);

    for (int y{0}; y < HEIGHT; ++y) {
        for (int x{0}; x < WIDTH; ++x) {
            if (visible_tiles.at(y).at(x) == none) {
                tiles.at(number_tiles.at(y).at(x)).setPosition(sf::Vector2f(CELL_PIX_SIZE * x, CELL_PIX_SIZE * y));
                renderTiles.draw(tiles.at(number_tiles.at(y).at(x)));
            } else {
                tiles.at(visible_tiles.at(y).at(x)).setPosition(sf::Vector2f(CELL_PIX_SIZE * x, CELL_PIX_SIZE * y));
                renderTiles.draw(tiles.at(visible_tiles.at(y).at(x)));
            }

        }
    }

    renderTiles.display();
    rectTiles.setTexture(&renderTiles.getTexture());
    window.draw(rectTiles);

}

void mouse (sf::Event &event) {
    int x = event.mouseButton.x - rectTiles.getPosition().x;
    int y = event.mouseButton.y - rectTiles.getPosition().y;
    int tilex = x / CELL_PIX_SIZE;
    int tiley = y / CELL_PIX_SIZE;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if(rectTiles.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
            if (first_click) {
                minesPos(mines, mine_num, sf::Vector2i(tilex, tiley));
                initializeVectors();
                first_click = false;
                fillTiles(number_tiles, visible_tiles, sf::Vector2u(tilex, tiley));
            } else if (visible_tiles.at(tiley).at(tilex) != flag) {
                fillTiles(number_tiles, visible_tiles, sf::Vector2u(tilex, tiley));
            }
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        switch(visible_tiles.at(tiley).at(tilex)) {
            case undiscovered:
                visible_tiles.at(tiley).at(tilex) = flag;
                break;
            case flag:
                visible_tiles.at(tiley).at(tilex) = question;
                break;
            case question:
                visible_tiles.at(tiley).at(tilex) = undiscovered;
                break;
        }

    }
}

void minesPos(vector<int> &vec, int num, sf::Vector2i pos) {
    size_t cells = vec.size();
    while (num > 0) {
        int rand_pos = rand() % cells;
        uint16_t randx = rand_pos % WIDTH;
        uint16_t randy = rand_pos / WIDTH;
        if (vec.at(rand_pos) != 1 && (abs(pos.x - randx) > 1 || abs(pos.y - randy) > 1)) {
            vec.at(rand_pos) = 1;
            num--;
        }
    }
}

void initializeVectors () {

    //We calculate the number tiles
    for (int y{0}; y < HEIGHT; ++y) {
        number_tiles.at(y) = vector<int>(WIDTH);
        for (int x{0}; x < WIDTH; ++x) {
            uint16_t mine_pos = y * WIDTH + x;
            if (mines.at(mine_pos) == 0) {
                uint8_t mines_found = 0;
                for (int posy{y-1}; posy <= y + 1; ++posy) {
                    for (int posx{x-1}; posx <= x + 1; ++posx) {
                        if (posx >= 0 && posx < WIDTH && posy >= 0 && posy < HEIGHT) {
                            uint16_t m = posy * WIDTH + posx;
                            mines_found = (mines.at(m) == 1) ? mines_found + 1 : mines_found;
                        }
                    }
                    //assign number to tile or undiscovered
                    if (mines_found == 0) {
                        number_tiles.at(y).at(x) = none;
                    } else {
                        number_tiles.at(y).at(x) = mines_found;
                    }
                }
            } else {
                number_tiles.at(y).at(x) = mine;
            }
        }
    }

    for (int y{0}; y < HEIGHT; ++y) {
        visible_tiles.at(y) = vector<int>(WIDTH);
        for (int x{0}; x < WIDTH; ++x) {
                visible_tiles.at(y).at(x) = undiscovered;
        }
    }

}

bool loadSprites(vector<sf::Sprite> &vec, sf::Texture &texture, const string file, int sprite_num) {

    if (!texture.loadFromFile(file)) {
        cout << "Can't load file - " << file << endl;
        return false;
    }

    int height = texture.getSize().y;
    int width = texture.getSize().x / sprite_num;
    for (int offset{0}; offset < sprite_num; ++offset) {
        sf::Sprite sprite(texture, sf::IntRect(offset * width, 0, width, height));
        sprite.scale(SCALE, SCALE);
        vec.push_back(sprite);
    }
    return true;
}
