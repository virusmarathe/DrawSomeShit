#include "PencilObject.h"


#define PI 3.14159265f
#define DRAW_NEW_DISTANCE 5

PencilObject::PencilObject()
{
	mTimer = 0.0f;
	mColorValue = 0.0f;
	mFinishedDrawing = false;
}

PencilObject::PencilObject(Vector2 startPos) : GameObject(startPos)
{
	mTimer = 0.0f;
	mColorValue = 0.0f;
	addPoints(startPos);
	mLastPointAddedPosition = Vector2(0, 0);
	mFinishedDrawing = false;
}


PencilObject::~PencilObject()
{
}

void PencilObject::update(float deltaTime)
{
	// dummy update logic for testing
	float cycleDuration = 4.0f; // takes 4 seconds to fill the screen
	mTimer += deltaTime;
	if (mTimer > cycleDuration)
	{
		mTimer = mTimer - cycleDuration;
	}

	mColorValue = (2 * PI) * (mTimer / cycleDuration);
}

void PencilObject::render()
{

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// move object to position, relavent for models, not so much for drawing
	//glTranslatef(mPosition.X, mPosition.Y, 0.0f);

	glColor3f(cos(mColorValue), sin(mColorValue), 0.7f);

	glBegin(GL_POINTS);
	glVertex2f(mPosition.X, mPosition.Y);
	glEnd();

	// draws a line geometry of all the points your mouse has gone over
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < mPoints.size(); i++)
	{
		glVertex2f(mPoints[i].X, mPoints[i].Y);
	}
	glEnd();
}

void PencilObject::addPoints(Vector2 newPoint)
{
	if (mLastPointAddedPosition.distSqr(newPoint) > DRAW_NEW_DISTANCE * DRAW_NEW_DISTANCE)
	{
		mPoints.push_back(newPoint);
		mLastPointAddedPosition = newPoint;
	}
}

void PencilObject::handleInput(SDL_Event event)
{
	if (mFinishedDrawing) return; // handling input for this object until mouse lifted

	switch (event.type)
	{
		case SDL_MOUSEMOTION:
			if (!mFinishedDrawing)
			{
				addPoints(Vector2((float)event.motion.x, (float)event.motion.y));
				charbuf buf;
				sprintf_s(buf, "%d, %d, %d", ObjectNetworkMessageType::UPDATE, event.motion.x, event.motion.y);
				Game::Instance()->SendNetworkMessage(buf);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			mFinishedDrawing = true;
			charbuf buf;
			sprintf_s(buf, "%d", ObjectNetworkMessageType::FINISH);
			Game::Instance()->SendNetworkMessage(buf);
			break;
	
		default:
			break;
	}
}

void PencilObject::HandleNetworkData(charbuf & buf)
{
	char * nextToken;
	char separators[] = ",";
	char * pch = strtok_s(buf, separators, &nextToken);
	ObjectNetworkMessageType type = (ObjectNetworkMessageType)atoi(pch);
	int x, y;
	switch (type)
	{
	case UPDATE:
		pch = strtok_s(NULL, separators, &nextToken);
		x = atoi(pch);
		pch = strtok_s(NULL, separators, &nextToken);
		y = atoi(pch);
		addPoints(Vector2((float)x, (float)y));
		break;
	case FINISH:
		mFinishedDrawing = true;
		break;
	}
}
