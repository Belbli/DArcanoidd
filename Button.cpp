#include "Button.h"
#include <iostream>
using namespace std;


void Button::setButtonPosition(int cx, int cy, int width, int height, GLuint text) {
	this->cx = cx;
	this->cy = cy;
	this->width = width;
	this->height = height;
	this->text = text;
	curH = height;
	curW = width;
	curCX = cx;
	curCY = cy;
}

bool Button::isClicked(float xMousePos, float yMousePos, int btnState) {
	if (xMousePos >= cx && xMousePos <= cx + width && yMousePos >= cy && yMousePos <= cy + height) {
		if (btnState == GLUT_DOWN) {
			curW -= width / 25;
			curH -= height / 15;
			curCX += width / 50;
			curCY += height / 30;
		}
		if (btnState == GLUT_UP) {
			curW = width;
			curH = height;
			curCX = cx;
			curCY = cy;
			return true;
		}
	}
	else if (btnState == GLUT_UP && curH != height && curW != width) {
		curW = width;
		curH = height;
		curCX = cx;
		curCY = cy;
	}
	return false;
}

void Button::ShowBtn() {
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, text);
	glBegin(GL_QUADS);
	glTexCoord2i(1, 0); glVertex2i(curCX + curW, curCY + curH);
	glTexCoord2i(0, 0); glVertex2i(curCX, curCY + curH);
	glTexCoord2i(0, 1); glVertex2i(curCX, curCY);
	glTexCoord2i(1, 1); glVertex2i(curCX + curW, curCY);
	glEnd();
	glDisable(GL_TEXTURE_2D);

}