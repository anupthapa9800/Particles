#include<iostream>
#include "Engine.h" // header file for the Engine class
#include "Particle.h"
#include "SFML/Graphics.hpp" // For RenderWindow and VideoMode
#include<random>
using namespace sf; // Use the SFML namespace globally
using namespace std;

Engine::Engine() {
    // Call create on m_Window to populate the RenderWindow member variable
    // Using desktop mode for custom resolution, though a fixed size could also be used.
    // E.g., m_Window.create(sf::VideoMode(1920, 1080), "Particles");
    // Or to use the screen resolution:
    m_Window.create(VideoMode::getDesktopMode(), "Particles"); 

    // Note: m_Particles (vector<Particle>) is automatically default-constructed.
}

void Engine::run() {
    // Construct a local Clock object to track time per frame 
    Clock clock;

    // Unit Tests setup and call (Use the exact code provided) 
    cout << "Starting Particle unit tests..." << endl;
    // Assuming Particle constructor is accessible and unitTests() exists.
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete. Starting engine..." << endl;

    // Game Loop 
    while (m_Window.isOpen()) { // Loop while m_Window is open 
        // Restart the clock (this returns the time elapsed since the last frame) 
        Time dt = clock.restart();

        // Convert the clock time to seconds 
        float dtAsSeconds = dt.asSeconds(); // Time differential (dt) 

        // Call input 
        input();

        // Call update 
        update(dtAsSeconds);

        // Call draw 
        draw();
    }
}

void Engine::input() {
    Event event;
    // Poll the Windows event queue 
    while (m_Window.pollEvent(event)) {
        // Handle the Escape key pressed and closed events so your program can exit
        if (event.type == Event::Closed ||
            (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape))
        {
            m_Window.close();
        }

        // Handle the left mouse button pressed event 
        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
            // Get mouse click position
            Vector2i mouseClickPosition(event.mouseButton.x, event.mouseButton.y);

            // Create a loop to construct 5 particles (or an experimental number) 
            for (int i = 0; i < 5; ++i) {
                // Generate a random number of points in the range [25:50] 
                // Using C++ standard library's random features for better randomness, 
                // but the assignment might imply C's rand() as used in the Particle constructor's instructions.
                // Sticking to standard for now, but can switch if needed.
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> distrib(25, 50); 
                int numPoints = distrib(gen);

                // Construct and add the new particle to the vector 
                m_particles.emplace_back(m_Window, numPoints, mouseClickPosition);
            }
        }
    }
}

void Engine::update(float dtAsSeconds) {
    // Loop through m_particles and call update on each Particle 
    // Use an iterator-based for-loop for safe erasure 
    for (auto it = m_particles.begin(); it != m_particles.end(); /* No increment here */) { // 
        // if getTTL() > 0.0 
        if (it->getTTL() > 0.0f) {
            // Call update on that Particle 
            it->update(dtAsSeconds); // Pass in the time differential (dt) 
            // Increment the iterator 
            ++it;
        }
        else {
            // If particle's ttl has expired, erase it from the vector 
            // erase returns an iterator that points to the next element 
            it = m_particles.erase(it); // Assign the iterator to this return value 
            // Do not increment the iterator here 
        }
    }
}

void Engine::draw() {
    // clear the window [cite: 161]
    m_Window.clear(sf::Color::Black); // Using black for the background, as shown in the image 

    // Loop through each Particle in m_Particles 
    for (const auto& particle : m_particles) {
        // Pass each element into m_Window.draw() 
        // This will call Particle::draw() due to polymorphism (since Particle inherits from sf::Drawable) 
        m_Window.draw(particle);
    }

    // display the window [cite: 169]
    m_Window.display();
}