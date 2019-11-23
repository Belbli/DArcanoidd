#pragma once
#include <GL\glut.h>

struct Button
{
private:
	int cx;
	int cy;
	int width;
	int height;
	int btnState;
	int curW;
	int curH;
	GLuint text;

public:
	void Button::ShowBtn();
	void Button::setButtonPosition(int cx, int cy, int width, int height, GLuint text);
	bool Button::isClicked(float xMousePos, float yMousePos, int btnState);
};
