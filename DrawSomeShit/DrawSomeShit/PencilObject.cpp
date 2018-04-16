#include "PencilObject.h"


#define PI 3.14159265f
#define DRAW_NEW_DISTANCE 5

PencilObject::PencilObject()
{
	mTimer = 0.0f;
	mColorValue = 0.0f;
	mFinishedDrawing = false;
}

PencilObject::PencilObject(Vector2 startPos, int objectID, int ownerID) : GameObject(startPos, objectID, ownerID)
{
	mTimer = 0.0f;
	mColorValue = 0.0f;
	addPoints(startPos);
	mLastPointAddedPosition = Vector2(0, 0);
	mFinishedDrawing = false;
	SetColorByPlayerID();
}


PencilObject::~PencilObject()
{

}

void PencilObject::update(float deltaTime)
{
	// dummy update logic for testing
/*	float cycleDuration = 4.0f; // takes 4 seconds to fill the screen
	mTimer += deltaTime;
	if (mTimer > cycleDuration)
	{
		mTimer = mTimer - cycleDuration;
	}

	mColorValue = (2 * PI) * (mTimer / cycleDuration);*/
}

void PencilObject::render()
{

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// move object to position, relavent for models, not so much for drawing
	//glTranslatef(mPosition.X, mPosition.Y, 0.0f);

	glColor3f(mColor.r, mColor.g, mColor.b);

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

void PencilObject::SetColorByPlayerID()
{
	switch (mOwnerID)
	{
	case 0:
		mColor.SetColor(1, 0, 0);
		break;
	case 1:
		mColor.SetColor(0, 1, 0);
		break;
	case 2:
		mColor.SetColor(0, 0, 1);
		break;
	case 3:
		mColor.SetColor(1, 1, 0);
		break;
	case 4:
		mColor.SetColor(0, 1, 1);
		break;
	case 5:
		mColor.SetColor(1, 0, 1);
		break;
	case 6:
		mColor.SetColor(1, 1, 1);
		break;
	case 7:
		mColor.SetColor(0.4f, 1, .25f);
		break;
	default:
		mColor.SetColor(1, 1, 1);
		break;
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
				int offset = 0;

//				int id = GetID();
				int idPackedData = 0;
				idPackedData |= Game::Instance()->GetPlayerID();
				idPackedData |= GetID() << 8;

				memcpy(buf + offset, &idPackedData, sizeof(int));
				offset += sizeof(int);

				int packedData = 0;
				packedData |= ObjectNetworkMessageType::UPDATE;
				packedData |= event.motion.x << 8;
				packedData |= event.motion.y << 20;
				memcpy(buf + offset, &packedData, sizeof(int));
				Game::Instance()->SendNetworkMessage(buf, 8);
			}
			break;
		case SDL_MOUSEBUTTONUP:
		{
			mFinishedDrawing = true;
			charbuf buf;
			int offset = 0;

//			int id = GetID();
			int idPackedData = 0;
			idPackedData |= Game::Instance()->GetPlayerID();
			idPackedData |= GetID() << 8;

			memcpy(buf + offset, &idPackedData, sizeof(int));
			offset += sizeof(int);

			int messageType = ObjectNetworkMessageType::FINISH;
			memcpy(buf + offset, &messageType, sizeof(int));
			Game::Instance()->SendNetworkMessage(buf, 8);
			break;
		}
		default:
			break;
	}
}

void PencilObject::HandleNetworkData(int packedData)
{
	ObjectNetworkMessageType type2 = (ObjectNetworkMessageType)(packedData & 0x000000FF);// = (ObjectNetworkMessageType)atoi(pch);
	int x = (packedData & 0x000FFF00) >> 8;
	int y = (packedData & 0xFFF00000) >> 20;

	switch (type2)
	{
	case UPDATE:
		addPoints(Vector2((float)x, (float)y));
		break;
	case FINISH:
		mFinishedDrawing = true;
		break;
	}
}
