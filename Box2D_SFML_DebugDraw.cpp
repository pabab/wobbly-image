#include "Box2D_SFML_DebugDraw.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <cmath>
#include <cstdio>

Box2D_SFML_DebugDraw::Box2D_SFML_DebugDraw(){
	alpha = 200;
	SetFlags(e_shapeBit	|e_jointBit);
}

void Box2D_SFML_DebugDraw::DrawPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color){
	b2Vec2 center;
	center.Set(0, 0);
	sf::VertexArray va_vertices(sf::LinesStrip, vertexCount);
	for(int i = 0; i<vertexCount; i++){
		va_vertices[i].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
		va_vertices[i].position.x = vertices[i].x;
		va_vertices[i].position.y = vertices[i].y;
		center.x+=vertices[i].x;
		center.y+=vertices[i].y;
	}
	//va_vertices[vertexCount-1] = va_vertices[0];
	va_vertices[vertexCount-1].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
	va_vertices[vertexCount-1].position.x = center.x/vertexCount;
	va_vertices[vertexCount-1].position.y = center.y/vertexCount;
	vertexArrays.push_back(va_vertices);
}

void Box2D_SFML_DebugDraw::DrawSolidPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color){
	sf::VertexArray va_vertices(sf::TrianglesFan, vertexCount+1);
	for(int i = 0; i<vertexCount; i++){
		va_vertices[i].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
		va_vertices[i].position.x = vertices[i].x;
		va_vertices[i].position.y = vertices[i].y;
	}
	va_vertices[vertexCount].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
	va_vertices[vertexCount].position.x = vertices[0].x;
	va_vertices[vertexCount].position.y = vertices[0].y;
	vertexArrays.push_back(va_vertices);
	DrawPolygon(vertices, vertexCount, color);
}

void Box2D_SFML_DebugDraw::DrawCircle (const b2Vec2 &center, float32 radius, const b2Color &color){
	const int nDivisions = 16;
	sf::VertexArray va_vertices(sf::LinesStrip, nDivisions+1);
	float angle = 0;
	for(int i = 0; i<nDivisions; i++, angle+=2*3.1416/nDivisions){
		va_vertices[i].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
		va_vertices[i].position.x = center.x + radius * std::cos(angle);
		va_vertices[i].position.y = center.y + radius * std::sin(angle);
	}
	va_vertices[nDivisions] = va_vertices[0];
	vertexArrays.push_back(va_vertices);
}

void Box2D_SFML_DebugDraw::DrawSolidCircle (const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color){
	const int nDivisions = 16;
	sf::VertexArray va_vertices(sf::TrianglesFan, nDivisions+2);
	va_vertices[0].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
	va_vertices[0].position.x = center.x;
	va_vertices[0].position.y = center.y;
	
	float angle = 0;
	for(int i = 0; i<nDivisions; i++, angle+=2*3.1416/nDivisions){
		va_vertices[i+1].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
		va_vertices[i+1].position.x = center.x + radius * std::cos(angle);
		va_vertices[i+1].position.y = center.y + radius * std::sin(angle);
	}
	va_vertices[nDivisions+1] = va_vertices[1];
	vertexArrays.push_back(va_vertices);
	
	sf::VertexArray axis_line(sf::Lines, 2);
	axis_line[0].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
	axis_line[0].position.x = center.x;
	axis_line[0].position.y = center.y;
	axis_line[1].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
	axis_line[1].position.x = center.x + radius * axis.x;
	axis_line[1].position.y = center.y + radius * axis.y;
	
	vertexArrays.push_back(axis_line);
	DrawCircle(center, radius, color);
}

void Box2D_SFML_DebugDraw::DrawSegment (const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color){
	sf::VertexArray va_vertices(sf::Lines, 2);
	va_vertices[0].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
	va_vertices[0].position.x = p1.x;
	va_vertices[0].position.y = p1.y;
	va_vertices[1].color = sf::Color(color.r*255, color.g*255, color.b*255, alpha);
	va_vertices[1].position.x = p2.x;
	va_vertices[1].position.y = p2.y;
	vertexArrays.push_back(va_vertices);
}

void Box2D_SFML_DebugDraw::DrawTransform (const b2Transform &xf){
	
}


void Box2D_SFML_DebugDraw::Draw(sf::RenderWindow &w){
	for(auto &a: vertexArrays){
		w.draw(a);
	}
	vertexArrays.clear();
}
