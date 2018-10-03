#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>

const int none {0};
const int undiscovered {9};

using namespace std;

void checkTile(vector<vector<int>> &tiles , vector<vector<int>> &visible_tiles, vector<sf::Vector2u> &vec, sf::Vector2u pos) {
    //primero comprobamos que el rango sea válido
    uint16_t max_X = tiles.size() - 1;
    uint16_t max_Y = tiles.at(0).size() - 1;
    if ( pos.x < 0 || pos.x > max_X || pos.y < 0 || pos.y > max_Y  ) {
        return;
    }
    int currentTile = tiles.at(pos.y).at(pos.x);
    if( tiles.at(pos.y).at(pos.x) > 0 && tiles.at(pos.y).at(pos.x) <= 8) {
        visible_tiles.at(pos.y).at(pos.x) = none;
    }
    if( currentTile == none) {
        vec.push_back(pos);
    }
}

void fillTiles(vector<vector<int>> &tiles, vector<vector<int>> &visible_tiles, sf::Vector2u pos) {
    // vector con las coordenadas a explorar
    vector<sf::Vector2u> vec;
    vec.clear();
    vec.push_back(pos);

    //cout << "Posicion: " << pos.x << ":" << pos.y << endl << endl;
    while (!vec.empty()) {
        sf::Vector2u coord(vec.back());
        vec.pop_back();
        int currentTile = tiles.at(coord.y).at(coord.x);
        int visibleTile = visible_tiles.at(coord.y).at(coord.x);
        visible_tiles.at(coord.y).at(coord.x) = none;

        if (currentTile == none && visibleTile == undiscovered) {
            checkTile(tiles, visible_tiles, vec, sf::Vector2u(coord.x + 1, coord.y));
            checkTile(tiles, visible_tiles, vec, sf::Vector2u(coord.x - 1, coord.y));
            checkTile(tiles, visible_tiles, vec, sf::Vector2u(coord.x, coord.y + 1));
            checkTile(tiles, visible_tiles, vec, sf::Vector2u(coord.x, coord.y - 1));

            checkTile(tiles, visible_tiles, vec, sf::Vector2u(coord.x + 1, coord.y + 1));
            checkTile(tiles, visible_tiles, vec, sf::Vector2u(coord.x - 1, coord.y + 1));
            checkTile(tiles, visible_tiles, vec, sf::Vector2u(coord.x + 1, coord.y - 1));
            checkTile(tiles, visible_tiles, vec, sf::Vector2u(coord.x - 1, coord.y - 1));
        }
    }

}

