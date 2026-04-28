#include <QtWidgets>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "ChipPile.hpp"

#include <iostream>

/*#define XScalingFactor 0.533
#define YScalingFactor 0.711

#define XScalingFactor 1
#define YScalingFactor 1*/

ChipPile::ChipPile(QWidget *parent, bool AcceptDrops): QStackedWidget(parent)
{
	QRect Screen = QGuiApplication::primaryScreen()->geometry();

	float wScale = Screen.width() / 1920.0;
	float hScale = Screen.height() / 1080.0;

	XScalingFactor = wScale;
	YScalingFactor = hScale;

	setMinimumSize(200, 220);
	setFrameStyle(QFrame::NoFrame);
	setAcceptDrops(AcceptDrops);
	setFrameShape (NoFrame);
	Active = true;

	pos = 125;
}

void ChipPile::setInactive(bool activeState)
{
	Active = activeState;
}

void ChipPile::populate(int ChipValue)
{
	Chip *BettingChip = new Chip(this);
	
	switch (ChipValue)
	{
		case 5:
			BettingChip->setPixmap(QPixmap(":/Images/RedChipCroppedSmall.gif").scaledToWidth(88*XScalingFactor));
			break;
		case 10:
			BettingChip->setPixmap(QPixmap(":/Images/BlueChipCroppedSmall.gif").scaledToWidth(88*XScalingFactor));
			break;
		case 25:
			BettingChip->setPixmap(QPixmap(":/Images/GreenChipCroppedSmall.gif").scaledToWidth(88*XScalingFactor));
			break;
		case 50:
			BettingChip->setPixmap(QPixmap(":/Images/OrangeChipCroppedSmall.gif").scaledToWidth(88*XScalingFactor));
			break;
		case 100:
			BettingChip->setPixmap(QPixmap(":/Images/BlackChipCroppedSmall.gif").scaledToWidth(88*XScalingFactor));
			break;
	}	

	BettingChip->setEnabled(true);
	BettingChip->show();
	BettingChip->setAttribute(Qt::WA_DeleteOnClose);
	BettingChip->setGeometry(0, 0, 88*XScalingFactor, 61*YScalingFactor);
	BettingChip->SetValue(ChipValue);
}

void ChipPile::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-dnditemdata"))
	{
		if (event->source() == this)
		{
			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
		else
		{
			event->acceptProposedAction();
		}
	}
	else
	{
		event->ignore();
	}
}

void ChipPile::dragMoveEvent(QDragMoveEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-dnditemdata")) 
	{
		if (event->source() == this) 
		{
			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
		else
		{
			event->acceptProposedAction();
		}
	}
	else
	{
		event->ignore();
	}
}

void ChipPile::AddChip(Chip* MyChip)
{
	ListChips.push_back(MyChip);
}

void ChipPile::RemoveChip()
{
	ListChips.pop_back();
}

// Remove all the used chip objects
void ChipPile::ClearChips()
{
	unsigned int i;

	qDebug() << "List is " << ListChips.size() << " in size";

	for (i = 0; i < ListChips.size(); i++)
	{
		ListChips.at(i)->close();
	}
	ListChips.clear();
	qDebug() << "List is " << ListChips.size() << " in size";
    pos = 125;
}

void ChipPile::dropEvent(QDropEvent *event)
{
	int ChipValue;
	bool UsedValue;

	if (event->mimeData()->hasFormat("application/x-dnditemdata")) 
	{
		QByteArray ChipData = event->mimeData()->data("application/x-dnditemdata");
		QDataStream DataStream(&ChipData, QIODevice::ReadOnly);

		QPixmap pixmap;
		QPoint offset;

		DataStream >> pixmap >> offset >> ChipValue >> UsedValue;

		// Check origin of dropped chip
		if(UsedValue == false)
		{
			// Dropped chip came from a stack box
			// Limit the chip stack height to 15 chips
			if(ListChips.size() < 15)
			{
				// Add chip's value to current bet
				IncreaseBet(ChipValue);

				// Make a copy of the dropped chip
				// giving it the same pixmap
				Chip *newIcon = new Chip(this);
				newIcon->setPixmap(pixmap);
				newIcon->move(event->pos() - offset);
				newIcon->show();
				// Raise it to ensure it sits on the top of the pile
				newIcon->raise();
				// Object will be deleted when its close event is called
				newIcon->setAttribute(Qt::WA_DeleteOnClose);
				// This is required so if the chip is removed from the
				// the bet pile, the game knows what value to subtract
				// from the current bet
				newIcon->SetValue(ChipValue);
				newIcon->SetUsed();
				event->acceptProposedAction();
				pos -= 8*YScalingFactor;
				/* The value of pos in the line below needs to start at 20 to sit on the wooden sill */

				/********************** rand **********************/
				int RandomNumber;
				int yPos;

				// Initialize random seed from time
				srand (time(NULL));

				// Generate a random number between 0 and 4
				RandomNumber = rand() % 5;
				// Generate a random number between 0 and 1
				yPos = rand() % 2;
				/***************************************************/

				newIcon->setGeometry((56 +RandomNumber)*XScalingFactor, (pos+yPos)*YScalingFactor, 88*XScalingFactor,61*YScalingFactor);
				// Keep a record of all chip object on the pile
				// so they can be deleted properly 
				AddChip(newIcon);
			}
		}
	}
	else 
	{
		event->ignore();
	}
}

// A chip is clicked on
void ChipPile::mousePressEvent(QMouseEvent *event)
{
	// Create pointer to clicked on chip
	Chip *child = static_cast<Chip*>(childAt(event->pos()));

	// If object is disabled or not created then ignore
	if(Active == false || child == nullptr)
	{
		return;
	}

	// Find out the chip's value
	// and whether it has been used before
	int ChipValue = child->GetValue();
	bool UsedValue = child->CheckChipUsed();

	// Check whether chip is on a bet or stack pile
	if(ListChips.size() > 0)
	{
		// Chip is on the bet pile
		pos = (125 - ((ListChips.size() - 1) * (8*YScalingFactor)));

		// Check if chip is within the bet pile or on top
		if(child != ListChips.back())
		{
			// Ignore clicks on chips within the betting pile
			return;			
		}
	}

	// Set the cursor to a gripping hand
	setCursor(Qt::ClosedHandCursor);

	QPixmap pixmap = child->pixmap();

	// Setup a QByteArray and QDataStream in order to drag custom data
	QByteArray ChipData;
	QDataStream DataStream(&ChipData, QIODevice::WriteOnly);

	// Add custom data into the QDataStream
	DataStream << pixmap << QPoint(event->pos() - child->pos()) << ChipValue << UsedValue;

	// Create a QMimeData to hold the chip data
	QMimeData *MimeData = new QMimeData;
	MimeData->setData("application/x-dnditemdata", ChipData);

	// Create a QDrag object which holds the MimeData
	QDrag *DragObject = new QDrag(this);
	DragObject->setMimeData(MimeData);

	// Give the DragObject a pixmap to display as it is dragged
	// and set its hotspot so the object stays under the cursor
	QPixmap scaledPixmap = pixmap.scaledToWidth(88*XScalingFactor, Qt::SmoothTransformation);
	DragObject->setPixmap(scaledPixmap);
	DragObject->setHotSpot(event->pos() - child->pos());

	/* Comment next 6 lines to stop auto-regeneration */
	QPixmap tempPixmap = scaledPixmap;
	child->setPixmap(tempPixmap);

	// Check that the executed drag n drop operation is a CopyAction  
	if (DragObject->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) != Qt::MoveAction)
	{
		// Check if chip is from a bet or stack pile
		if(UsedValue == true)
		{
			// Chip is from the bet pile
			DecreaseBet(ChipValue);
			RemoveChip();				
			child->close();
		}
		// Play a chip sound
		emit PlayChipSound();
	}
	// Return the cursor to an open hand
	setCursor(Qt::OpenHandCursor);
}


