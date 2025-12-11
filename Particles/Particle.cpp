#include "Particle.h"
#include "Matrices.h" // For Matrix, RotationMatrix, etc.
#include <cmath> // For cos, sin, PI
#include <cstdlib> // For rand(), RAND_MAX
#include <iostream>
using namespace sf;
using namespace std;


Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition)
// The Matrix member variable m_A must be constructed in an initialization list
    : m_A(2, numPoints) 
{
    // 1. Initialize Member Variables
    m_ttl = TTL; // Initialize m_ttl with the global constant TTL (5 seconds)
    m_numPoints = numPoints; // Initialize m_numPoints with numPoints 

    // Angular Velocity: m_radiansPerSec
    // Random angular velocity in the range [0:PI] 
    m_radiansPerSec = ((float)rand() / RAND_MAX) * M_PI; 

        // 2. Setup the Cartesian Plane View
        // m_cartesianPlane maps monitor pixels (centered at (W/2, H/2)) to Cartesian (centered at (0,0)) 
        m_cartesianPlane.setCenter(0.0f, 0.0f); 
        // Invert the y-axis to match standard Cartesian coordinates 
        m_cartesianPlane.setSize(target.getSize().x, (-1.0f) * target.getSize().y); 

        // 3. Center Coordinate Initialization
        // Map mouseClickPosition from pixel coordinates to Cartesian coordinates and store it 
        m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

        // 4. Initial Velocities (m_vx, m_vy)
        // Assign m_vx and m_vy to random pixel velocities, e.g., between 100 and 500 
        
        float initialSpeed = (float)rand() / RAND_MAX * (500.0f - 100.0f) + 100.0f;
    m_vx = initialSpeed;
    m_vy = initialSpeed;

    // Randomize direction for m_vx (positive or negative) 
    if (rand() % 2 != 0) {
        m_vx *= -1.0f;
    }

    // 5. Colors
    // Assign m_color1 and m_color2 
    m_color1 = sf::Color::White; // m_color1 white 

    // Generate a random color for m_color2 
    m_color2 = sf::Color(rand() % 256, rand() % 256, rand() % 256);

    // 6. Generate Vertices (The Randomized Shape)
    // The algorithm sweeps a circular arc with randomized radii 

    // Initialize theta to an angle between [0: PI/2]
    double theta = ((float)rand() / RAND_MAX) * (M_PI / 2.0);

    // Initialize dTheta to 2*PI / (numPoints - 1) 
    // We divide by numPoints - 1 so the last vertex overlaps with the first 
    double dTheta = 2.0 * M_PI / (numPoints - 1);

    // Loop for generating numPoint vertices
    for (int j = 0; j < numPoints; ++j) {
        double r, dx, dy; // Declare local variables r, dx, and dy 

        // Assign a random number between [20:80] to r 
        r = (double)rand() / RAND_MAX * (80.0 - 20.0) + 20.0;

        // Calculate dx and dy 
        dx = r * std::cos(theta); 
            dy = r * std::sin(theta); 

            // Assign the Cartesian coordinate of the new vertex to m_A 
            // The coordinate is relative to the centerCoordinate
            m_A(0, j) = m_centerCoordinate.x + dx; 
            m_A(1, j) = m_centerCoordinate.y + dy; 

            // Increment theta by dTheta 
            theta += dTheta;
    }
}

void Particle::draw(RenderTarget& target, RenderStates states) const {
  
    // Construct a VertexArray named lines of primitive type TriangleFan 
    // numPoints + 1 to account for the center 
    VertexArray lines(TriangleFan, m_numPoints + 1);

    // Declare a local Vector2f named center
    Vector2f center;

    // Assign center by mapping m_centerCoordinate from Cartesian to pixel coordinates 
    center = (Vector2f)target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane); 

        // Assign center vertex properties
        lines[0].position = center; 
        lines[0].color = m_color1; // Center color 

    // Loop j from 1 up to and including m_numPoints for the outer vertices 
    for (int j = 1; j <= m_numPoints; ++j) {
        // The index in lines is 1-off from the index in m_A 

        // Get the Cartesian coordinate from m_A (column j - 1)
        Vector2f cartesianCoord(m_A(0, j - 1), m_A(1, j - 1));

        // Assign lines[j].position by mapping Cartesian to pixel coordinates 
        lines[j].position = (Vector2f)target.mapCoordsToPixel(cartesianCoord, m_cartesianPlane); 

            // Assign lines[j].color with m_Color2 
            lines[j].color = m_color2;
    }

    // Draw the VertexArray 
    target.draw(lines, states); 
}
void Particle::update(float dt) {
  
    // Subtract dt from m_ttl 
    m_ttl -= dt;

    // 1. Rotation
    // Call rotate with an angle of dt * m_radiansPerSec 
    rotate(dt * m_radiansPerSec);

    // 2. Scaling
    // Call scale using the global constant SCALE (e.g., 0.999)
    scale(SCALE);

    // 3. Translation (Physics/Movement)
    float dx, dy; // Declare local float variables dx and dy

    // dx = m_vx * dt 
    dx = m_vx * dt;

    // Vertical velocity should change by some gravitational constant G 
    // Subtract G * dt from m_vy 
    m_vy -= G * dt;

    // dy = m_vy * dt 
    dy = m_vy * dt;

    // Call translate using dx, dy as arguments
    translate(dx, dy);
}

void Particle::translate(double xShift, double yShift) {

    // Construct a TranslationMatrix T 
    TranslationMatrix T(xShift, yShift, m_numPoints);

    m_A = T + m_A; 

    // Update the particle's center coordinate 
    m_centerCoordinate.x += xShift; 
    m_centerCoordinate.y += yShift; 
}

void Particle::rotate(double theta) {
    
    // 1. Store center and shift to origin
    Vector2f temp = m_centerCoordinate; 
        // Shift our particle's center back to the origin 
        translate(-m_centerCoordinate.x, -m_centerCoordinate.y);

    // 2. Rotate
    // Construct a Rotation Matrix R
    RotationMatrix R(theta);

    // Multiply it by m_A as m_A = R * m_A (Left-multiply R)
    m_A = R * m_A; // Assuming Matrix class overloads the multiplication operator '*'

    // 3. Shift back
    // Shift our particle back to its original center 
    translate(temp.x, temp.y);
}

void Particle::scale(double c) {
   
    // 1. Store center and shift to origin
    Vector2f temp = m_centerCoordinate; 
        // Shift our particle's center back to the origin 
        translate(-m_centerCoordinate.x, -m_centerCoordinate.y);

    // 2. Scale
    // Construct a ScalingMatrix S 
    ScalingMatrix S(c);

    // Multiply it by m_A as m_A = S * m_A 
    m_A = S * m_A; // Assuming Matrix class overloads the multiplication operator '*'

    // 3. Shift back
    // Shift our particle back to its original center 
    translate(temp.x, temp.y);
}
bool Particle::almostEqual(double a, double b, double eps)
{
	return fabs(a - b) < eps;
}

void Particle::unitTests()
{
    int score = 0;

    cout << "Testing RotationMatrix constructor...";
    double theta = M_PI / 4.0;
    RotationMatrix r(M_PI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta))
        && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta)))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing ScalingMatrix constructor...";
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2
        && almostEqual(s(0, 0), 1.5)
        && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0)
        && almostEqual(s(1, 1), 1.5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing TranslationMatrix constructor...";
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3
        && almostEqual(t(0, 0), 5)
        && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5)
        && almostEqual(t(1, 1), -5)
        && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    
    cout << "Testing Particles..." << endl;
    cout << "Testing Particle mapping to Cartesian origin..." << endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0)
    {
        cout << "Failed.  Expected (0,0).  Received: (" << m_centerCoordinate.x << "," << m_centerCoordinate.y << ")" << endl;
    }
    else
    {
        cout << "Passed.  +1" << endl;
        score++;
    }

    cout << "Applying one rotation of 90 degrees about the origin..." << endl;
    Matrix initialCoords = m_A;
    rotate(M_PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1, j), initialCoords(0, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            rotationPassed = false;
        }
    }
    if (rotationPassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a scale of 0.5..." << endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0,j)) || !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            scalePassed = false;
        }
    }
    if (scalePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a translation of (10, 5)..." << endl;
    initialCoords = m_A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !almostEqual(m_A(1, j), 5 + initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            translatePassed = false;
        }
    }
    if (translatePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Score: " << score << " / 7" << endl;
}

