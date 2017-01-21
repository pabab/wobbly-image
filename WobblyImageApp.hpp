#ifndef WOBBLY_IMAGE_HPP
#define WOBBLY_IMAGE_HPP
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <Box2D/Box2D.h>
#include "Box2D_SFML_DebugDraw.hpp"

class WobblyImageApp{
public:
	WobblyImageApp();
	void run();
	
private:
	// SFML related variables
	sf::RenderWindow window;
	sf::Texture tex;
	
	// Box2D related variables
	b2World *world;
	Box2D_SFML_DebugDraw debugRenderer;
	b2Body *mouseBody;
	b2Joint *mouseJoint;
	
	
	// app logic related variables
	std::vector<b2Body *> bodies;
	sf::Vector2u meshSize;
	bool isDragging;
	bool drawDebug;
	
	
	
	// internal methods
	void createSoftBody(const sf::Vector2f &pos, const sf::Vector2f &size, const sf::Vector2u &meshDensity);
	void createWalls(const sf::View &view);
	void manageEvents();
	void update();
	void draw();
	b2Body *getClickedBody(const sf::Vector2f &clickPos);

};


#endif
