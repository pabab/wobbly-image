#include "WobblyImageApp.hpp"
#include <SFML/Window/Event.hpp>

WobblyImageApp::WobblyImageApp(){
	window.create(sf::VideoMode(640,400),"Wobbly Image");
	window.setFramerateLimit(60);
	mouseJoint = nullptr;
	isDragging = false;
	drawDebug = false;
	
	// set window view
	sf::View view;
	float zoom = 0.01;
	view.setSize(window.getSize().x * zoom, window.getSize().y * zoom);
	view.setCenter(window.getSize().x * zoom/2, window.getSize().y * zoom/2);
	window.setView(view);
	
	
	tex.loadFromFile("assets/images/photo.png");
	
	world = new b2World({0, 2});
	world->SetDebugDraw(&debugRenderer);
	createWalls(view);
	meshSize.x = meshSize.y = 16;
	createSoftBody({2, 1}, {2, 2}, meshSize);
	
	b2BodyDef newBodyDef;
	newBodyDef.type = 	b2_kinematicBody ;
	mouseBody = world->CreateBody(&newBodyDef);
}


void WobblyImageApp::run(){
	
	
	sf::Time delta;
	while(window.isOpen()){
		
		manageEvents();
		update();
		draw();
	}
	
}


void WobblyImageApp::manageEvents(){
	// events
	sf::Event e;
	while(window.pollEvent(e)) {
		if(e.type == sf::Event::Closed){
			window.close();	
		}else if(e.type == sf::Event::MouseButtonPressed){
			b2Body *clickedBody = getClickedBody(window.mapPixelToCoords(sf::Vector2i(e.mouseButton.x, e.mouseButton.y)));
			if(clickedBody){
				if(e.mouseButton.button == sf::Mouse::Button::Left){
					
					b2DistanceJointDef distanceJointDef;
					distanceJointDef.frequencyHz = 5;
					distanceJointDef.dampingRatio = 1;
					distanceJointDef.length = 0;
					distanceJointDef.localAnchorA = b2Vec2(0,0);
					distanceJointDef.localAnchorB = b2Vec2(0,0);
					distanceJointDef.bodyA = clickedBody;
					distanceJointDef.bodyB = mouseBody;
					mouseJoint = world->CreateJoint(&distanceJointDef);
					isDragging = true;
				}else{
					clickedBody->SetType(clickedBody->GetType()==b2_dynamicBody?b2_staticBody:b2_dynamicBody);
				}
			}
		}else if(e.type == sf::Event::MouseButtonReleased){
			if(mouseJoint){
				world->DestroyJoint(mouseJoint);
				mouseJoint = nullptr;
				isDragging = false;
			}
		}else if(e.type == sf::Event::KeyPressed){
			switch(e.key.code){
				case sf::Keyboard::D:
					drawDebug = !drawDebug;
				break;
				
			case sf::Keyboard::R:
				createSoftBody({2, 1}, {2, 2}, meshSize);
			break;
			}
		}
	}
}


void WobblyImageApp::createSoftBody(const sf::Vector2f &pos, const sf::Vector2f &bodySize, const sf::Vector2u &meshSize){
	// destroy existing bodies
	for(auto body: bodies){
		world->DestroyBody(body);
	}
	bodies.clear();
	
	
	unsigned nBodies = meshSize.x * meshSize.y;
	for(unsigned i = 0; i < nBodies; ++i){
		b2Vec2 bodyPos(pos.x+(i%meshSize.y)*(bodySize.x/meshSize.x),pos.y+(i/meshSize.y)*(bodySize.y/meshSize.y));
		b2BodyDef newBodyDef;
		newBodyDef.position = bodyPos;
		newBodyDef.type = 	b2_dynamicBody ;
		newBodyDef.fixedRotation = true;
		b2Body *newBody = world->CreateBody(&newBodyDef);
		
		// add a fixture (shape) to the newly created body
		b2CircleShape newBodyShape;
		newBodyShape.m_p.Set(0, 0);
		newBodyShape.m_radius = 0.02;
		
		newBody->CreateFixture(&newBodyShape, 1)->SetFriction(0.85);
		
		bodies.push_back(newBody);
	}
	
	b2DistanceJointDef distanceJointDef;
	distanceJointDef.frequencyHz = 20;
	distanceJointDef.dampingRatio = 1;
	distanceJointDef.localAnchorA = b2Vec2(0,0);
	distanceJointDef.localAnchorB = b2Vec2(0,0);
	for(unsigned i = 0; i < nBodies; ++i){
		unsigned body_i, body_j;
		body_i = i/meshSize.y;
		body_j = i%meshSize.y;
		
		
		
		if(body_j < meshSize.x - 1){
			distanceJointDef.bodyA = bodies[i];
			distanceJointDef.bodyB = bodies[i+1];
			distanceJointDef.length = (bodySize.x/meshSize.x);
			world->CreateJoint(&distanceJointDef);
		}
		
		if(body_i < meshSize.y - 1){
			distanceJointDef.bodyA = bodies[i];
			distanceJointDef.bodyB = bodies[i+meshSize.x];
			distanceJointDef.length = (bodySize.y/meshSize.y);
			world->CreateJoint(&distanceJointDef);
		}
		
		if(body_j < meshSize.x - 1 && body_i < meshSize.y - 1){
			distanceJointDef.bodyA = bodies[i];
			distanceJointDef.bodyB = bodies[i+meshSize.x+1];
			distanceJointDef.length = sqrt(powf(bodySize.x/meshSize.x, 2) +powf(bodySize.y/meshSize.y, 2));
			world->CreateJoint(&distanceJointDef);
		}
		
		if(body_j < meshSize.x - 1 && body_i > 0 && body_i < meshSize.y){
			distanceJointDef.bodyA = bodies[i];
			distanceJointDef.bodyB = bodies[i-meshSize.x+1];
			distanceJointDef.length = sqrt(powf(bodySize.x/meshSize.x, 2) +powf(bodySize.y/meshSize.y, 2));
			world->CreateJoint(&distanceJointDef);
		}
		
		
	}
	
}


void WobblyImageApp::update(){
	if(isDragging){
		sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition());
		mouseBody->SetTransform({mousePos.x, mousePos.y}, 0);
	}
	world->Step(1/60.0, 10, 10);
	
}

void WobblyImageApp::draw(){
	window.clear(sf::Color::Black);
	
	sf::VertexArray va;
	va.setPrimitiveType(sf::Quads);
	for(unsigned i = 0; i<bodies.size(); ++i){
		unsigned body_i, body_j;
		body_i = i/meshSize.x;
		body_j = i%meshSize.x;
		if(body_i < meshSize.x - 1 && body_j < meshSize.y -1){
			va.append({sf::Vector2f(bodies[i]->GetPosition().x, bodies[i]->GetPosition().y), sf::Vector2f(body_j/float(meshSize.y)*tex.getSize().x, body_i/float(meshSize.x)*tex.getSize().y)});
			va.append({sf::Vector2f(bodies[i+1]->GetPosition().x, bodies[i+1]->GetPosition().y), sf::Vector2f((body_j+1)/float(meshSize.y)*tex.getSize().x, body_i/float(meshSize.x)*tex.getSize().y)});
			va.append({sf::Vector2f(bodies[i+meshSize.x+1]->GetPosition().x, bodies[i+meshSize.x+1]->GetPosition().y), sf::Vector2f((body_j+1)/float(meshSize.y)*tex.getSize().x, (body_i+1)/float(meshSize.x)*tex.getSize().y)});
			va.append({sf::Vector2f(bodies[i+meshSize.x]->GetPosition().x, bodies[i+meshSize.x]->GetPosition().y), sf::Vector2f(body_j/float(meshSize.y)*tex.getSize().x, (body_i+1)/float(meshSize.x)*tex.getSize().y)});
		}
	}
	
	
	sf::RenderStates rs;
	rs.texture = &tex;
	window.draw(va, rs);

	if(drawDebug){
		world->DrawDebugData();
		debugRenderer.Draw(window);
	}
	
	window.display();
}


void WobblyImageApp::createWalls(const sf::View &view){
	float viewLeft   = view.getCenter().x-view.getSize().x/2.0;
	float viewRight  = view.getCenter().x+view.getSize().x/2.0;
	float viewTop    = view.getCenter().y-view.getSize().y/2.0;
	float viewBottom = view.getCenter().y+view.getSize().y/2.0;
	
	b2BodyDef bodyDef;
	b2PolygonShape shape;
	
	// left wall
	bodyDef.position.Set(viewLeft, viewTop);
	b2Body *leftWall = world->CreateBody(&bodyDef);
	shape.SetAsBox(0.01, view.getSize().y);
	leftWall->CreateFixture(&shape, 1);
	
	// right wall
	bodyDef.position.Set(viewRight, viewTop);
	b2Body *rightWall = world->CreateBody(&bodyDef);
	shape.SetAsBox(0.01, view.getSize().y);
	rightWall->CreateFixture(&shape, 1);
	
	// floor
	bodyDef.position.Set(viewLeft, viewBottom);
	b2Body *floor = world->CreateBody(&bodyDef);
	shape.SetAsBox(view.getSize().x, 0.01);
	floor->CreateFixture(&shape, 1);
	
	// ceiling
	bodyDef.position.Set(viewLeft, viewTop);
	b2Body *ceiling = world->CreateBody(&bodyDef);
	shape.SetAsBox(view.getSize().x, 0.01);
	ceiling->CreateFixture(&shape, 1);
}

b2Body *WobblyImageApp::getClickedBody(const sf::Vector2f &clickPos){
	b2Body *clickedBody = nullptr;
	for(b2Body *b: bodies){
		const b2Vec2 &bodyPos = b->GetPosition();
		if(powf(bodyPos.x -clickPos.x,2)+powf(bodyPos.y -clickPos.y,2)<0.005){
			return b;
		}
	}
	
	return clickedBody;
}


int main(int argc, char *argv[]){
	WobblyImageApp app;
	app.run();
}

