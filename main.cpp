#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

// Your existing project headers
#include "StudentManager.h"
#include "reports.h"
#include "grade_logic.h"
#include "file_manager.h"

// 1. PLACE THE STRUCT DEFINITION HERE (Before main)
struct GuiButton {
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color idleColor = sf::Color(45, 52, 54);
    sf::Color hoverColor = sf::Color(9, 132, 227);
    bool isHovered = false;

    GuiButton(std::string label, sf::Vector2f pos, sf::Font& font) {
        shape.setSize({220.f, 45.f});
        shape.setPosition(pos);
        shape.setFillColor(idleColor);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::White);

        // Center text in button
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        text.setPosition(pos.x + 110.f, pos.y + 22.5f);
    }

    void update(sf::Vector2f mousePos) {
        isHovered = shape.getGlobalBounds().contains(mousePos);
        shape.setFillColor(isHovered ? hoverColor : idleColor);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
};

int main() {
    loadFromFile(); // Load existing data

    sf::RenderWindow window(sf::VideoMode(1200, 800), "Student Management System");
    window.setFramerateLimit(60);

    sf::Font font;
    // Make sure arial.ttf is in your project folder!
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Error loading font!" << std::endl;
        return -1;
    }

    // 2. DECLARE THE BUTTONS INSIDE MAIN
    GuiButton btnAdd("1. Add Student", {20, 100}, font);
    GuiButton btnView("2. View All", {20, 160}, font);
    GuiButton btnSave("3. Save Data", {20, 220}, font);

    std::string statusMessage = "System Ready.";

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                saveToFile();
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (btnAdd.isHovered) {
                    statusMessage = "Follow console instructions to add student...";
                    // You can call your existing case 1 logic here
                }
                if (btnView.isHovered) {
                    statusMessage = "Displaying Table in Console.";
                    displayAllStudentsTable();
                }
                if (btnSave.isHovered) {
                    saveToFile();
                    statusMessage = "Data saved to file.";
                }
            }
        }

        // Update Button Hover Effects
        btnAdd.update(mousePos);
        btnView.update(mousePos);
        btnSave.update(mousePos);

        // Rendering
        window.clear(sf::Color(30, 30, 30));

        // Draw Sidebar background
        sf::RectangleShape sidebar({260, 800});
        sidebar.setFillColor(sf::Color(45, 45, 45));
        window.draw(sidebar);

        // Draw Buttons
        btnAdd.draw(window);
        btnView.draw(window);
        btnSave.draw(window);

        // Draw Status Message
        sf::Text status(statusMessage, font, 18);
        status.setPosition(300, 100);
        status.setFillColor(sf::Color::Cyan);
        window.draw(status);

        window.display();
    }

    return 0;
}
