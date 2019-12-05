#define _CRT_SECURE_NO_WARNINGS
#include"Button.h"
#include<math.h>
#include<vector>
#include<GL/glut.h>
#include<locale.h>
#include<iostream>
#include<soil.h>
#include<ctime>
using namespace std;


#define MENUPROC 100
#define GAMEPROC 101
#define SELECTLVLPROC 103
#define TRAINMODE 104
#define INITPROC 105 
#define SELECTMODEPROC 106
#define NORMALMODE 107
#define SHOWRECORDS 108
#define ENTERTEXT 109
#define GAMEOVERPROC 110
#define CHOOSEPLATEPROC 111
#define EXIT 112

using namespace std;

Button playBtn, recordsBtn, exitBtn, nextLvlBtn, menuBtn, replayBtn, *lvlBtns,
choosePlateBtn, gameModeButtons[2], backBtn, nextPageBtn,
buyBtn, prevPageBtn, selectBtn, continueBtn;
int windowWidth = 490, windowHeight = 600, lifeCost = 100;
int bonusK = 1, plateTexesAmount;
int plateHeight = 15;
int x = -1, y = -1, btnWidth = 150, btnState, btnStart, btnPressed = -1, plateWidth = windowWidth / 6, xLeftPlatePos, xRightPlatePos, lifes = 3;
int xMousePos = 0, yPlatePos = 3 * windowHeight / 4 + 5, yPrevPlatePos = 0, currentTrainLvl = 1, availableLvls = 0;
int blockAmount = 0, score = 0, mode, recordsRows = 0;
bool destroyWnd = false, maxAvailableLvl = false, activeKey = false;
float xAngle = 2.0, yAngle = 1.0;
int blockHeight = 20, blockWidth = 50, process = INITPROC, lvlsPassed, lvls, bonusAmount = 0, ballsAmount = 0, normalModeLvlPassed = 1;
int coins = 50;
char *purchased = NULL;
int visiblePart = 10, lvlsPage = 0, shopPage = 0;

unsigned char keyS;


struct Ball {
	int radius = 7;
	int x;
	int y;
	int xSpeed = 2.0;
	int ySpeed = -1.0;
	bool move;
}ball;

struct Bonus {
	int x;
	int y;
	int bonusType;
	GLuint texture;
	bool move;
}bonus;

struct Block {
	int xb;
	int yb;
	int durability;
	GLuint tex;
} item;

struct Record {
	int priority;
	char name[20];
	int score;
}record;

vector<Block> blocks;
vector<Bonus> bonuses;
vector<Ball> balls;
vector<Record> records;

void MouseMove(int ax, int ay) {
	xMousePos = ax;
}


void display();
void timer(int);
void reshape(int, int);
void initBlocks(char *lvlPath);
void setLvlsInfo();
int readFile(char *path);
void addRecord();
void saveRecords();
void keyBoardFunc(unsigned char key, int x, int y);
void specialKeysFunc(int key, int x, int y);
void clearData();

void init() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, windowHeight, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);

}

GLuint bgTexture, playBtnTex, recBtnTex, exitBtnTex, menuBtnTex,
*lvlBtnTex, normalBtnTex, trainBtnTex, nextLvlBtnTex, backBtnTex, *blocksTex, *plateTexes,
prevBtnTex, nextBtnTex, currentPlateTex, shopBtnTex, selectBtnTex, coinsTex, buyBtnTex,
priceBgTex, bonusTexture, recordTex, adsBG, continueBtnTex, life, buyLifeTex;

GLuint loadTexture(const char *apFileName) {
	GLuint texture;
	texture = SOIL_load_OGL_texture(apFileName,
		SOIL_LOAD_RGBA,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y);
	return texture;
}

GLuint *loadTextureArr(int amount, char *nameSubStr) {
	char buff[3];
	GLuint *dest = NULL;
	dest = (GLuint*)realloc(dest, (amount) * sizeof(GLuint));
	for (int i = 0; i < amount; i++) {
		char path[15] = "";
		strcat(path, nameSubStr);
		_itoa(i + 1, buff, 10);
		strcat(path, buff);
		strcat(path, ".jpg");
		dest[i] = loadTexture(path);
	}
	return dest;
}

void loadTextures() {
	coinsTex = loadTexture("coins1.jpg");
	bgTexture = loadTexture("bg.png");
	menuBtnTex = loadTexture("menuBtn.jpg");
	playBtnTex = loadTexture("playBtn.jpg");
	recBtnTex = loadTexture("recBtn.jpg");
	exitBtnTex = loadTexture("exitBtn.jpg");
	priceBgTex = loadTexture("priceBG.jpg");
	trainBtnTex = loadTexture("trainModeBtn.jpg");
	nextLvlBtnTex = loadTexture("nextLvlBtn.jpg");
	normalBtnTex = loadTexture("normalModeBtn.jpg");
	buyBtnTex = loadTexture("buyBtn.jpg");
	backBtnTex = loadTexture("backBtn.jpg");
	prevBtnTex = loadTexture("prevBtn.jpg");
	nextBtnTex = loadTexture("nextBtn.jpg");
	shopBtnTex = loadTexture("shopBtn.jpg");
	selectBtnTex = loadTexture("selectBtn.jpg");
	blocksTex = loadTextureArr(4, "block_");
	lvlBtnTex = loadTextureArr(lvls, "lvlBtn_");
	currentPlateTex = loadTexture("plate_1.jpg");
	plateTexes = loadTextureArr(plateTexesAmount, "plate_");
	bonusTexture = loadTexture("bonusTex.jpg");
	recordTex = loadTexture("recordBG.jpg");
	adsBG = loadTexture("adsBG.jpg");
	continueBtnTex = loadTexture("continueBtn.jpg");
	life = loadTexture("life.png");
	buyLifeTex = loadTexture("buyLifeTex.jpg");
}

void MouseMoveClick(int btn, int state, int ax, int ay) {
	x = ax;
	y = ay;
	btnStart = btn;
	btnState = state;
}

void bonusValidityPeriod() {
	static int c = 0;
	do {
		Sleep(970);
		c++;
		cout << c;
	} while (c < 15);
}

int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "");
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Arcanoid");	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(MouseMove);
	glutMouseFunc(MouseMoveClick);
	glutKeyboardFunc(keyBoardFunc);
	glutSpecialFunc(specialKeysFunc);
	glutTimerFunc(0, timer, 0);
	init();
	glutMainLoop();
	return 0;
}

void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 100, timer, 0);
	display();
}

void drawTexture(int aX, int aY, int aW, int aH, GLuint aTextID) {
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, aTextID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glTexCoord2i(1, 0); glVertex2i(aX + aW, aY + aH);
	glTexCoord2i(0, 0); glVertex2i(aX, aY + aH);
	glTexCoord2i(0, 1); glVertex2i(aX, aY);
	glTexCoord2i(1, 1); glVertex2i(aX + aW, aY);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void renderBM(float x, float y, void *font, char *str) {
	char *c;
	glRasterPos2f(x, y);
	for (c = str; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void printText(int xPos, int yPos, int info) {
	glColor3f(0.0, 0.0, 0.0);
	char str[15];
	_itoa(info, str, 10);
	renderBM(xPos, yPos, GLUT_BITMAP_TIMES_ROMAN_24, str);
}

void printText(int xPos, int yPos, char *str, float r = 0.0, float g = 0.0, float b = 0.0) {
	glColor3f(r, g, b);
	renderBM(xPos, yPos, GLUT_BITMAP_TIMES_ROMAN_24, str);
}
int s = 0, xEnterningTextPos = windowWidth / 2;

void enterRecordName(unsigned char key) {
	printText(windowWidth / 2 - strlen("YOU SET THE RECORD") * 6, windowHeight / 6, "YOU SET THE RECORD");
	printText(windowWidth / 2 - strlen("Enter Your Name : ")*6, windowHeight / 4, "Enter Your Name : ");
	if (activeKey && key != 13) {
		xEnterningTextPos = windowWidth / 2 - strlen(record.name) * 4;
		if (key == 8 && s > 0) {
			s--;
			record.name[s] = '\0';
		}
		else if(s+1 < 20){
			record.name[s] = key;
			s++;
			record.name[s] = '\0';
		}
		activeKey = false;
	}
	printText(xEnterningTextPos, windowHeight / 2, record.name);
	printText(150, windowHeight / 2, "__________________");
}

void specialKeysFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		if (process == TRAINMODE && lvlsPage > 0)
			lvlsPage--;
		if (process == CHOOSEPLATEPROC && shopPage > 0)
			shopPage--;
		break;
	case GLUT_KEY_RIGHT:
		if (process == TRAINMODE && lvlsPage < lvls / visiblePart)
			lvlsPage++;
		if (process == CHOOSEPLATEPROC && shopPage < plateTexesAmount - 1)
			shopPage++;
		break;
	default:
		return;
	}
}

void keyBoardFunc(unsigned char key, int x, int y) {
	keyS = key;
	activeKey = true;
	btnStart = -1;
	switch (key) {
		case 27:
			if (process == MENUPROC)
				exit(0);
			clearData();
			process = MENUPROC;
			break;
		case 13:
			if (process == ENTERTEXT) {
				addRecord();
				saveRecords();
				process = GAMEOVERPROC;
				break;
			}
			if(process == CHOOSEPLATEPROC)
				currentPlateTex = plateTexes[shopPage];
			break;
	default:
		break;
	}
}

void setLvlsInfo() {
	availableLvls = readFile("progress.txt");
}

void saveProgress(char *fName, int value) {
	FILE *file = fopen(fName, "w+");
	if (bonusValidityPeriod != NULL) {
		fprintf(file, "%d", value);
		fclose(file);
	}
	else
		return;
}

void sortRecords() {
	for (int i = 0; i < recordsRows; i++) {
		for (int j = i; j < recordsRows; j++) {
			if (records[i].score < records[j].score)
				std::swap(records[i], records[j]);
		}
	}
}

void saveRecords() {
	sortRecords();
	FILE *file = fopen("records.txt", "w");
	if (file != NULL) {
		for (int i = 0; i < recordsRows; i++) {
			records[i].priority = i + 1;
			fprintf(file, "%d.%s %d\n", records[i].priority, records[i].name, records[i].score);
		}
	}
	else
		return;
}

void readRecords() {
	FILE *file = fopen("records.txt", "r");
	if (file != NULL) {
		while (!feof(file)) {
			if (fscanf(file, "%d.%s %d\n", &record.priority, &record.name, &record.score) == 3) {
				recordsRows++;
				records.push_back(record);
			}
		}
	}
	else 
		return;
}

void showRecords() {
	sortRecords();
	drawTexture(40, 50, 420, 430, recordTex);
	int xPos = 80, yPos = 120;
	if (recordsRows > 0) {
		for (int i = 0; i < recordsRows; i++) {
			char numBuff[7] = "";
			_itoa(records[i].priority, numBuff, 10);
			printText(xPos + (strlen("10") - strlen(numBuff))*10, yPos, records[i].priority);
			xPos += 30;
			printText(xPos, yPos, records[i].name);
			xPos += 3 * windowWidth / 6;
			_itoa(records[i].score, numBuff, 10);
			printText(xPos + (strlen("10000")-strlen(numBuff))*9, yPos, records[i].score);
			yPos += 30;
			xPos = 80;
		}
	}
	else
		printText(windowWidth / 2 - strlen("NO RECORDS!!!")*6, windowHeight / 3, "NO RECORDS!!!");
	backBtn.ShowBtn();
	if (backBtn.isClicked(x, y, btnState) && btnStart == GLUT_LEFT_BUTTON) {
		process = MENUPROC;
		btnStart = -1;
	}
}

int countAvailableLvls(char *subStr, char *extension) {
	FILE *file = NULL;
	int tempCount = -1;
	char txt[5] = "", numBuff[3] = "";
	strcat(txt, extension);
	do {
		tempCount++;
		char path[12] = "";
		strcat(path, subStr);
		_itoa(tempCount + 1, numBuff, 10);
		strcat(path, numBuff);
		strcat(path, txt);
		file = fopen(path, "r");
	} while (file != NULL);
	return tempCount;
}

int readFile(char *path) {
	int output = -1;
	FILE *file = fopen(path, "r");
	if (file != NULL) {
		while (fscanf(file, "%d", &output)) {
			fclose(file);
			return output;
		}
	}
	return output;
}

char *loadLvl(char *path, int strLen) {
	FILE *file;
	file = fopen(path, "r");
	if (file != NULL) {
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		int buffSize = strLen;
		fseek(file, 0, SEEK_SET);
		char *buff = (char*)calloc(buffSize, sizeof(char));
		char *dest = (char*)calloc(size, sizeof(char));
		while (fgets(buff, buffSize, file) != NULL) {
			strcat(dest, buff);
		}
		free(buff);
		fclose(file);
		return dest;
	}
	else
		process = TRAINMODE;
	return NULL;
}

void setBonus(int blockNumber) {
	bonus.bonusType = rand() % 4;
	bonus.x = blocks[blockNumber].xb;
	bonus.y = blocks[blockNumber].yb;
	bonus.move = false;
	bonus.texture = bonusTexture;
	bonusAmount++;
}

void initBonuses() {
	int num;
	for (int i = 0; i < blockAmount; i++) {
		num = rand() % 10;
		if (num % 6 == 0) {
			setBonus(i);
			bonuses.push_back(bonus);
		}
	}
}

void initMenuButtons() {
	int btnHeight = 40;
	int lineSpace = btnHeight + 50;
	playBtn.setButtonPosition(windowWidth / 2 - btnWidth / 2, windowHeight / 4, btnWidth, btnHeight, playBtnTex);
	choosePlateBtn.setButtonPosition(windowWidth / 2 - btnWidth / 2, windowHeight / 4 + lineSpace, btnWidth, btnHeight, shopBtnTex);
	recordsBtn.setButtonPosition(windowWidth / 2 - btnWidth / 2, windowHeight / 4 + 2 * lineSpace, btnWidth, btnHeight, recBtnTex);
	exitBtn.setButtonPosition(windowWidth / 2 - btnWidth / 2, windowHeight / 4 + 3 * lineSpace, btnWidth, btnHeight, exitBtnTex);
}

void showMenu() {
	playBtn.ShowBtn();
	choosePlateBtn.ShowBtn();
	recordsBtn.ShowBtn();
	exitBtn.ShowBtn();
}

void initSubMenuButtons() {
	int subMenuBtnH = 40, subMenuBtnW = 80;
	menuBtn.setButtonPosition(windowWidth / 2 - subMenuBtnW / 2, 3 * windowHeight / 4 - subMenuBtnH, subMenuBtnW, subMenuBtnH, menuBtnTex);
	nextLvlBtn.setButtonPosition(windowWidth / 2 - subMenuBtnW / 2, 3 * windowHeight / 4 + subMenuBtnH, subMenuBtnW, subMenuBtnH, nextLvlBtnTex);
	selectBtn.setButtonPosition(windowWidth / 2 - subMenuBtnW/2, 3 * windowHeight / 4 - subMenuBtnH, subMenuBtnW, subMenuBtnH, selectBtnTex);
	backBtn.setButtonPosition(windowWidth / 2 - subMenuBtnW / 2, 3 * windowHeight / 4 + subMenuBtnH, subMenuBtnW, subMenuBtnH, backBtnTex);
	nextPageBtn.setButtonPosition(windowWidth / 2 + subMenuBtnW, 3 * windowHeight / 4, subMenuBtnW, subMenuBtnH, nextBtnTex);
	buyBtn.setButtonPosition(windowWidth / 2 - subMenuBtnW/2, 3 * windowHeight / 4 - subMenuBtnH, subMenuBtnW, subMenuBtnH, buyBtnTex);
	prevPageBtn.setButtonPosition(windowWidth / 2 - 2*subMenuBtnW, 3 * windowHeight / 4, subMenuBtnW, subMenuBtnH, prevBtnTex);
	continueBtn.setButtonPosition(windowWidth / 2 - subMenuBtnW / 2, 3 * windowHeight / 4 + subMenuBtnH, subMenuBtnW, subMenuBtnH, continueBtnTex);
}

void addBall() {
	ball.x = xLeftPlatePos + plateWidth / 2;
	ball.y = yPlatePos - ball.radius;
	if (ballsAmount == 0)
		ball.move = false;
	else
		ball.move = true;
	balls.push_back(ball);
	ballsAmount++;
}

void loadLvl(int i) {
	char lvlPath[12] = "lvl_", lvl[4] = "";
	_itoa(i, lvl, 10);
	strcat(lvlPath, lvl);
	strcat(lvlPath, ".txt");
	process = GAMEPROC;
	btnStart = -1;
	yAngle = 1.0;
	if (mode == TRAINMODE || i == 1) {
		lifes = 3;
		score = 0;
	}
	initBlocks(lvlPath);
	initBonuses();
	addBall();
	glutSetCursor(GLUT_CURSOR_NONE);
}

void initLvlMenu() {
	int lvlBtnH = 60, lvlBtnW = 80, xSpacing = 120, ySpacing = 30, t = 0;
	lvlBtns = (Button*)calloc(lvls, sizeof(Button));
	for (int i = 0; i < lvls; i++) {
		if ((i) % 2 == 0 && i != 0) {
			ySpacing += 4 * lvlBtnH / 3;
			xSpacing = 120;
			t = 0;
		}
		if (i == 19)
			i = 19;
		if (i > 0 && i % visiblePart == 0) {
			xSpacing = 120, ySpacing = 30, t = 0;
		}
		xSpacing += t * (lvlBtnW + 80);
		lvlBtns[i].setButtonPosition(xSpacing, ySpacing, lvlBtnW, lvlBtnH, lvlBtnTex[i]);
		t++;
	}
}

void flipping(int &currentPage, int maxPage) {
	backBtn.ShowBtn();
	if (lvls > 10) {
		nextPageBtn.ShowBtn();
		prevPageBtn.ShowBtn();
	}
	if (btnStart == GLUT_LEFT_BUTTON) {
		if (nextPageBtn.isClicked(x, y, btnState) && currentPage < maxPage) {
			currentPage++;
			btnStart = -1;
		}
		if (prevPageBtn.isClicked(x, y, btnState) && currentPage > 0) {
			currentPage--;
			btnStart = -1;
		}
	}
}

void addLvlMenu() {
	flipping(lvlsPage, lvls / visiblePart);
	int max;
	if (lvlsPage < lvls/visiblePart) {
		max = lvlsPage*visiblePart + visiblePart;
	}
	else
		max = lvls;

	for (int i = lvlsPage*visiblePart; i < max; i++)
		lvlBtns[i].ShowBtn();
}

bool isLvlOpen(int lvl) {
	if (lvl <= availableLvls)
		return true;
	return false;
}

void selectLvl() {
	if (btnStart == GLUT_LEFT_BUTTON) {
		for (int i = lvlsPage*visiblePart; i < lvlsPage*visiblePart + lvls - lvlsPage*visiblePart; i++) {
			if (lvlBtns[i].isClicked(x, y, btnState)) {
				currentTrainLvl = i + 1;
				if (isLvlOpen(currentTrainLvl)) {
					if (availableLvls == currentTrainLvl)
						maxAvailableLvl = true;
					loadLvl(currentTrainLvl);
				}
				break;
			}
		}
		if (backBtn.isClicked(x, y, btnState))
			process = SELECTMODEPROC;
	}
	btnState = -1;
}

void initGameModeMenu() {
	int modeBtnH = 40, modeBtnW = 100, xSpacing = windowWidth / 2 - modeBtnW, ySpacing = windowHeight / 2 - modeBtnH / 2;
	gameModeButtons[0].setButtonPosition(xSpacing - 20, ySpacing, modeBtnW, modeBtnH, normalBtnTex);
	xSpacing += 30 + modeBtnW;
	gameModeButtons[1].setButtonPosition(xSpacing - 20, ySpacing, modeBtnW, modeBtnH, trainBtnTex);
}

void showGameModeMenu() {
	for (int i = 0; i < 2; i++) {
		gameModeButtons[i].ShowBtn();
	}
	backBtn.ShowBtn();
}

void selectGameMode() {
	showGameModeMenu();
	if (btnStart == GLUT_LEFT_BUTTON) {
		for (int i = 0; i < 2; i++) {
			if (gameModeButtons[i].isClicked(x, y, btnState)) {
				if (i == 0) {
					process = NORMALMODE;
					normalModeLvlPassed = 1;
					mode = NORMALMODE;
				}
				if (i == 1) {
					process = TRAINMODE;
					mode = TRAINMODE;
				}
				return;
			}
		}
		if (backBtn.isClicked(x, y, btnState))
			process = MENUPROC;
	}
}

void menu() {
	showMenu();
	if (btnStart == GLUT_LEFT_BUTTON) {
		if (playBtn.isClicked(x, y, btnState)) {
			process = SELECTMODEPROC;
			score = 0;
		}
		else if (choosePlateBtn.isClicked(x, y, btnState))
			process = CHOOSEPLATEPROC;
		else if (recordsBtn.isClicked(x, y, btnState))
			process = SHOWRECORDS;
		else if (exitBtn.isClicked(x, y, btnState))
			process = EXIT;
	}

}

void savePlateShopInfo() {
	FILE *bonusValidityPeriod = fopen("purchasedPlates.txt", "w");
	if (bonusValidityPeriod != NULL) {
		fprintf(bonusValidityPeriod, "%s", purchased);
		fclose(bonusValidityPeriod);
	}
	else
		return;
}

int hasNewPlateSkins() {
	int newSkinsAmount = 0;
	newSkinsAmount = readFile("plateSkinsAmount.txt");
	return plateTexesAmount - newSkinsAmount;
}

void appendNewSkinsInfo(int differece) {
	FILE *bonusValidityPeriod = fopen("purchasedPlates.txt", "a+");
	if (bonusValidityPeriod != NULL) {
		for (int i = 0; i < differece; i++) {
			fprintf(bonusValidityPeriod, "%d", 0);
		}
		fclose(bonusValidityPeriod);
	}
	else
		return;
}

void checkingNewPlateSkins() {
	int newPlatesAmount = hasNewPlateSkins();
	if (newPlatesAmount != 0) {
		saveProgress("plateSkinsAmount.txt", plateTexesAmount);
		appendNewSkinsInfo(newPlatesAmount);
	}
}

void initFunctions() {
	lvls = countAvailableLvls("lvl_", ".txt");
	plateTexesAmount = countAvailableLvls("plate_", ".jpg");
	checkingNewPlateSkins();
	purchased = loadLvl("purchasedPlates.txt", plateTexesAmount);
	coins = readFile("coins.txt");
	loadTextures();
	readRecords();
	initMenuButtons();
	initLvlMenu();
	initMenuButtons();
	setLvlsInfo();
	initSubMenuButtons();
	initGameModeMenu();
	process = MENUPROC;
}

void drawPlateSkins(int curPlatePage, int maxPage) {
	if (curPlatePage > 0 && curPlatePage < maxPage) {
		drawTexture(windowWidth / 2 - 5 * plateWidth / 2, windowHeight / 3, plateWidth, plateHeight, plateTexes[curPlatePage-1]);
		drawTexture(windowWidth / 2 - plateWidth, windowHeight / 3 - plateHeight / 2, plateWidth*2, plateHeight*2, plateTexes[curPlatePage]);
		drawTexture(windowWidth / 2 + 3 * plateWidth / 2, windowHeight / 3, plateWidth, plateHeight, plateTexes[curPlatePage + 1]);
		return;
	}
	if (curPlatePage == 0) {
		drawTexture(windowWidth / 2 - plateWidth, windowHeight / 3 - plateHeight / 2, plateWidth * 2, plateHeight * 2, plateTexes[curPlatePage]);
		drawTexture(windowWidth / 2 + 3 * plateWidth / 2, windowHeight / 3, plateWidth, plateHeight, plateTexes[curPlatePage + 1]);
		return;
	}
	if (curPlatePage == maxPage) {
		drawTexture(windowWidth / 2 - plateWidth, windowHeight / 3 - plateHeight / 2, plateWidth * 2, plateHeight * 2, plateTexes[curPlatePage]);
		drawTexture(windowWidth / 2 - 5 * plateWidth / 2, windowHeight / 3, plateWidth, plateHeight, plateTexes[curPlatePage - 1]);
		return;
	}
	
}

void showPrice(int page, bool redraw) {
	static int w = 0, prevPage = -1;
	int cost = 150 * page;
	char price[6];
	_itoa(cost, price, 10);
	if (prevPage != page || redraw) {
		prevPage = page;
		w = 0;
	}
	if (w < 140)
		w += 4;
	drawTexture(windowWidth / 2 - w/2, windowHeight / 2, w, 40, priceBgTex);
	printText(windowWidth / 2 - strlen(price)*6, windowHeight / 2 + 28, cost);
}

void choosePlate() {
	static bool redraw = false;
	flipping(shopPage, plateTexesAmount-1);
	if (purchased[shopPage] == '1') {
		selectBtn.ShowBtn();
		redraw = true;
	}
	else {
		showPrice(shopPage, redraw);
		redraw = false;
		buyBtn.ShowBtn();
		if(btnStart == GLUT_DOWN)
			if (buyBtn.isClicked(x, y, btnState) && coins >= shopPage * 150) {
				purchased[shopPage] = '1';
				coins -= shopPage * 150;
				btnStart = -1;
				savePlateShopInfo();
				saveProgress("coins.txt", coins);
			}
	}
	backBtn.ShowBtn();
	drawPlateSkins(shopPage, plateTexesAmount - 1);
	if (btnStart == GLUT_DOWN) {
		if (selectBtn.isClicked(x, y, btnState))
			currentPlateTex = plateTexes[shopPage];
		if (backBtn.isClicked(x, y, btnState))
			process = MENUPROC;
		else
			return;
	}
	return;
}

bool hasBonus(int blockIndx, int &bonusIndex) {
	for (int i = 0; i < bonusAmount; i++) {
		if (blocks[blockIndx].xb == bonuses[i].x && blocks[blockIndx].yb == bonuses[i].y) {
			bonusIndex = i;
			return true;
		}
	}
	return false;
}

void reboundDir(int index) {
	if (yPrevPlatePos < xLeftPlatePos && balls[index].xSpeed > 0 || yPrevPlatePos > xLeftPlatePos && balls[index].xSpeed < 0)
		balls[index].ySpeed *= -1;
	else if (yPrevPlatePos < xLeftPlatePos && balls[index].xSpeed < 0 || yPrevPlatePos > xLeftPlatePos && balls[index].xSpeed > 0) {
		balls[index].ySpeed *= -1;
		balls[index].xSpeed *= -1;
	}
	else
		balls[index].ySpeed *= -1;
}

bool verticalRebound(int blockIndex) {
	for (int i = 0; i < ballsAmount; i++) {
		if (balls[i].y + balls[i].radius == blocks[blockIndex].yb || balls[i].y - balls[i].radius - blockHeight == blocks[blockIndex].yb)
			if (blocks[blockIndex].yb && balls[i].x >= blocks[blockIndex].xb && balls[i].x <= blocks[blockIndex].xb + blockWidth || balls[i].y - balls[i].radius == blocks[blockIndex].yb + blockHeight && balls[i].x >= blocks[blockIndex].xb && balls[i].x <= blocks[blockIndex].xb + blockWidth) {
				balls[i].ySpeed *= -1;
				return true;
			}
	}
	return false;
}


bool isTouch(int i, int j) {
	if (balls[i].y + balls[i].radius >= blocks[j].yb
		&& balls[i].y - balls[i].radius <= blocks[j].yb + blockHeight
		&& balls[i].x + balls[i].radius >= blocks[j].xb
		&& balls[i].x - balls[i].radius <= blocks[j].xb + blockWidth)
		return true;
	return false;
}

int setScore(int lifes, int bonusK) {
	if (lifes == 1)
		return 10 * bonusK;
	if (lifes == 2)
		return 20 * bonusK;
	return 30 * bonusK;
}

void collision() {
	int index;
	for (int i = 0; i < ballsAmount; i++) {
		for (int j = 0; j < blockAmount; j++) {
			if (isTouch(i, j)) {
				if (!verticalRebound(j))
					balls[i].xSpeed *= -1;
				blocks[j].durability--;
				blocks[j].tex = blocksTex[blocks[j].durability - 1];
				score += setScore(lifes, bonusK);
			}
			if (blocks[j].durability == 0) {
				if (hasBonus(j, index))
					bonuses[index].move = true;
				blocks.erase(blocks.begin() + j);
				blockAmount--;
			}
		}
	}
}

void setBlockAmount(char *lvl) {
	for (int i = 0; i < strlen(lvl); i++) {
		if (lvl[i] != '0' && lvl[i] != '\n')
			blockAmount++;
	}
}

void initBlocks(char *lvlPath) {
	char *lvlMap = loadLvl(lvlPath, windowWidth/blockWidth);
	if (lvlMap != NULL) {
		int ySpacing = 0, t = 0, xSpasing = 0, k = 0;
		setBlockAmount(lvlMap);
		if (blockAmount > 0)
			ySpacing = 0;
		for (int i = 0; i < strlen(lvlMap); i++) {
			if (lvlMap[i] == '\n') {
				ySpacing += blockHeight + 5;
				t = 0;
				xSpasing = 0;
			}
			if (lvlMap[i] != '0' && lvlMap[i] != '\n') {
				item.xb = xSpasing;
				item.yb = ySpacing;
				item.durability = lvlMap[i] - 48;
				item.tex = blocksTex[item.durability - 1];
				blocks.push_back(item);
				t++;
				xSpasing += blockWidth + 5;
			}
			if (lvlMap[i] == '0')
				xSpasing += blockWidth + 5;
		}
	}
	else {
		return;
	}
}

bool isRecord() {
	if (recordsRows < 10) 
		return true;
	for (int i = 0; i < recordsRows; i++) {
		if (score > records[i].score) 
			return true;
	}
	return false;
}

void clearData() {
	balls.clear();
	ballsAmount = 0;
	blocks.clear();
	blockAmount = 0;
	bonuses.clear();
	bonusAmount = 0;
}

void lvlPassed(int mode) {
	coins += score / 10;
	saveProgress("coins.txt", coins);
	if (mode == NORMALMODE && normalModeLvlPassed < lvls) {
		process = NORMALMODE;
		normalModeLvlPassed++;
	}
	else if (isRecord() && mode == NORMALMODE) {
		process = ENTERTEXT;
		strcpy(record.name, "");
	}
	else
		process = GAMEOVERPROC;

	if(mode == TRAINMODE) {
		if (maxAvailableLvl) {
			availableLvls++;
		}
		currentTrainLvl++;
		maxAvailableLvl = false;
		process = GAMEOVERPROC;
	}
	saveProgress("availableLvls.txt", availableLvls);
	clearData();
}

void ViewBlocks() {
	for (int i = 0; i < blockAmount; i++) {
		drawTexture(blocks[i].xb, blocks[i].yb, blockWidth, blockHeight, blocks[i].tex);
	}
	if (blockAmount == 0) {
		lvlPassed(mode);
	}
}

void platePosition() {
	yPrevPlatePos = xLeftPlatePos;
	if (xMousePos >= plateWidth / 2 && xMousePos <= windowWidth - plateWidth / 2) {
		xLeftPlatePos = xMousePos - plateWidth / 2;
		xRightPlatePos = xMousePos + plateWidth / 2;
	}
	else if (xMousePos <= plateWidth / 2) {
		xLeftPlatePos = 0;
		xRightPlatePos = plateWidth;
	}
	else {
		xLeftPlatePos = windowWidth - plateWidth;
		xRightPlatePos = windowWidth;
	}
}

void drawPlate() {
	glColor3f(1.0, 0.0, 0.0);
	platePosition();
	drawTexture(xLeftPlatePos, yPlatePos, plateWidth, plateHeight, currentPlateTex);
}

bool plateCollision(int x, int y, int r) {
	if (y + r == yPlatePos && x >= xLeftPlatePos && x <= xRightPlatePos + r)
		return true;
	return false;
}

void plateRebound(int index) {
	float angle = abs(xMousePos - balls[index].x) / (plateWidth / 6);
	if (angle == 0)
		angle = 1;
	if (balls[index].xSpeed < 0)
		balls[index].xSpeed = -angle;
	else
		balls[index].xSpeed = angle;
	reboundDir(index);
}

void wallsRebound(int x, int y, int r, int index) {
	if (x - r <= 0 || x + r >= windowWidth)
		balls[index].xSpeed *= -1;
	if (y - r <= 0)
		balls[index].ySpeed *= -1;
}

bool ballLose(int x, int y, int index) {
	if (y > yPlatePos + 40) {
		if (lifes == 0)
			process = GAMEOVERPROC;
		if (ballsAmount > 1) {
			btnStart = -1;
			balls.erase(balls.begin() + index);
			ballsAmount--;
		}
		else {
			lifes--;
			balls[index].move = false;
		}
		return true;
	}
	return false;
}

void ballMotion() {
	if (btnStart == GLUT_LEFT_BUTTON) {
		btnStart = -1;
		for (int i = 0; i < ballsAmount; i++) {
			if (!balls[i].move) {
				balls[i].move = true;
				balls[i].ySpeed = -1;
			}
		}
	}
	for (int i = 0; i < ballsAmount; i++) {
		if (balls[i].move == false) {
			balls[i].x = xLeftPlatePos + plateWidth / 2;
			balls[i].y = yPlatePos - balls[i].radius;
		}
		else {
			if (!ballLose(balls[i].x, balls[i].y, i)) {
				balls[i].x += balls[i].xSpeed;
				balls[i].y += balls[i].ySpeed;
				if (plateCollision(balls[i].x, balls[i].y, balls[i].radius))
					plateRebound(i);
				wallsRebound(balls[i].x, balls[i].y, balls[i].radius, i);
			}
		}
	}
}

void drawBall(int index) {
	glBegin(GL_TRIANGLE_FAN);
	for (int j = 0; j <= 50; j++) {
		float angle = 2.0 * 3.1415926 * float(j) / float(50);
		float dx = balls[index].radius * cosf(angle);
		float dy = balls[index].radius * sinf(angle);
		glVertex2f(balls[index].x + dx, balls[index].y + dy);
	}
	glEnd();
}

void drawBalls() {
	ballMotion();
	glColor3f(0.0, 1.0, 0.0);
	for (int i = 0; i < ballsAmount; i++) {
			drawBall(i);
	}	
}

void applyBonus(int bonusType) {
	if (bonusType == 0)
		plateWidth += plateWidth / 5;
	if (bonusType == 1)
		bonusK = 2;
	if (bonusType == 2)
		lifes++;
	if (bonusType == 3)
		addBall();
	
}

void drawBonus(int bonusIndex) {
	if (bonuses[bonusIndex].y < yPlatePos + 10) {
		glColor3f(0.0, 1.0, 0.0);
		drawTexture(bonuses[bonusIndex].x, bonuses[bonusIndex].y, 20, 20, bonusTexture);
		bonuses[bonusIndex].y++;
		if (plateCollision(bonuses[bonusIndex].x, bonuses[bonusIndex].y, 0))
			applyBonus(bonuses[bonusIndex].bonusType);
	}
	else {
		bonuses.erase(bonuses.begin() + bonusIndex);
		bonusAmount--;
	}
}

void activeBonuses() {
	for (int i = 0; i < bonusAmount; i++) {
		if (bonuses[i].move == true)
			drawBonus(i);
	}
}

void printGameInfo(int x, int y) {
	printText(x, y - 60, score);
	int cx = windowWidth / 2 - lifes * 20;
	for (int i = 0; i < lifes; i++) {
		drawTexture(cx, windowHeight - 50, 30, 30, life);
		cx += 50;
	}
}

void addRecord() {
	record.score = score;
	record.priority = recordsRows;
	if (recordsRows < 10) {
		recordsRows++;
	}
	else {
		records.erase(records.begin() + recordsRows - 1);
	}
	records.push_back(record);
}


void showCoins() {
	char buff[6] = "";
	drawTexture(windowWidth - 100, 0, 100, 35, coinsTex);
	_itoa(coins, buff, 10);
	printText(windowWidth - strlen(buff) * 9 - 45, 27, coins);
}

bool toOfferExtraLife() {
	char numBuff[7] = "";
	_itoa(lifeCost, numBuff, 10);
	showCoins();
	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	drawTexture(windowWidth / 2 - 150, windowHeight / 2 - 100, 300, 200, adsBG);
	drawTexture(windowWidth / 2 - 50, windowHeight / 2 + 45, 100, 30, buyLifeTex);
	printText(windowWidth / 2 - strlen(numBuff) * 6, windowHeight / 2 + 70, numBuff, 0.0, 1.0, 0.0);
	buyBtn.ShowBtn();
	continueBtn.ShowBtn();
	if (btnStart == GLUT_LEFT_BUTTON) {
		if (coins >= 100 && buyBtn.isClicked(x, y, btnState)) {
			coins -= lifeCost;
			lifes++;
			lifeCost *= 2;
			process = GAMEPROC;
		}
		if (continueBtn.isClicked(x, y, btnState))
			return false;
	}
	return true;
}

void game() {
	if (lifes > 0) {
		printGameInfo(windowWidth / 2, windowHeight);
		drawBalls();
		collision();
		activeBonuses();
		drawPlate();
		ViewBlocks();
	}
	else {
		if (!toOfferExtraLife()) {
			coins += score / 10;
			lifeCost = 100;
			saveProgress("coins.txt", coins);
			if (isRecord() && mode == NORMALMODE) {
				s = 0;
				xEnterningTextPos = windowWidth / 2;
				process = ENTERTEXT;
				strcpy(record.name, "");
			}
			else
				process = GAMEOVERPROC;
			clearData();
		}
		btnStart = -1;
	}
}

void printGameOverInfo() {
	menuBtn.ShowBtn();
	if(mode == TRAINMODE)
		nextLvlBtn.ShowBtn();
	printText(windowWidth / 2 - strlen("GAME OVER!") * 6, windowHeight / 5, "GAME OVER!");
	if (normalModeLvlPassed == lvls)
		printText(windowWidth / 2 - strlen("YOU'VE PASSED ALL LEVELES") * 6, windowHeight / 4, "YOU'VE PASSED ALL LEVELES");
	printText(windowWidth / 2 - strlen("YOUR SCORE :  ") * 6, windowHeight / 3, "YOU SCORE : ");
	printText(windowWidth / 2 + strlen("       10000") * 6, windowHeight / 3, score);
	if (btnStart == GLUT_LEFT_BUTTON) {
		if (menuBtn.isClicked(x, y, btnState)) {
			process = MENUPROC;
			btnStart = -1;
		}
		if (nextLvlBtn.isClicked(x, y, btnState) && mode == TRAINMODE) {
			if (currentTrainLvl + 1 <= lvls)
				currentTrainLvl++;
			loadLvl(currentTrainLvl);
			btnStart = -1;
		}
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	drawTexture(0, 0, windowWidth, windowHeight, bgTexture);
	if (process != GAMEPROC) {
		showCoins();
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}
	switch (process) {
	case MENUPROC:
		menu();
		break;
	case TRAINMODE:
		addLvlMenu();
		selectLvl();
		break;
	case NORMALMODE:
		loadLvl(normalModeLvlPassed);
		break;
	case SELECTMODEPROC:
		selectGameMode();
		break;
	case INITPROC:
		initFunctions();
		break;
	case GAMEPROC:
		glutSetCursor(GLUT_CURSOR_NONE);
		game();
		break;
	case SHOWRECORDS:
		showRecords();
		break;
	case ENTERTEXT:
		enterRecordName(keyS);
		break;
	case GAMEOVERPROC:
		printGameOverInfo();
		break;
	case CHOOSEPLATEPROC:
		choosePlate();
		break;
	case EXIT:
		free(blocksTex);
		free(plateTexes);
		free(lvlBtnTex);
		free(lvlBtns);
		glutDestroyWindow(1);
		break;
	}
	if (process != EXIT)
		glutSwapBuffers();
	btnState = -1;
}

void reshape(int w, int h) {
	if(w != windowWidth || h != windowHeight)
		glutReshapeWindow(windowWidth, windowHeight);
}