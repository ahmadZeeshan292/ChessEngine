#pragma once

#include<SFML/Graphics.hpp>

struct Ray {
	sf::Vector2i start;
	sf::Vector2i dir;

	Ray(sf::Vector2i a, sf::Vector2i b) : start(a), dir(b) {}

    sf::Vector2i raysIntersect(const Ray& a, const Ray& b) {
        sf::Vector2i diff = { b.start.x - a.start.x, b.start.y - a.start.y };
        int det = cross(a.dir, b.dir);

        if (det == 0) {
            return sf::Vector2i();
        }

        double t1 = static_cast<double>(cross(diff, b.dir)) / det;
        double t2 = static_cast<double>(cross(diff, a.dir)) / det;

        // Rays intersect only if both parameters are in the forward direction
        if (t1 >= 0.0 && t2 >= 0.0) {
            int interX = static_cast<int>(std::round(a.start.x + t1 * a.dir.x));
            int interY = static_cast<int>(std::round(a.start.y + t1 * a.dir.y));

			if (interX >= 0 && interX <= 7 && interY >= 0 && interY <= 7)
                return sf::Vector2i(interX, interY);
            else
				return sf::Vector2i();
        }

        return sf::Vector2i();
    }


private:
    int cross(const sf::Vector2i& a, const sf::Vector2i& b) {
        return a.x * b.y - a.y * b.x;
    }
};

