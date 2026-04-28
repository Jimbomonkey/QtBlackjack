
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
	// Find out the user's screen resolution
	QRect Screen = QGuiApplication::primaryScreen()->geometry();

	/* Scale = User's screen size / My screen size */
	XScale = Screen.width() / 1920.0;
	YScale = Screen.height() / 1080.0;

	// Group the sets of cards to make
	// it easier to move them together
	PlayersCards = new QGroupBox(this);
	DealersCards = new QGroupBox(this);

	// Use a stylesheet to remove the group box borders
	PlayersCards->setObjectName("PlayersCardsGroupBox");
	DealersCards->setObjectName("DealersCardsGroupBox");
	PlayersCards->setStyleSheet("#PlayersCardsGroupBox{border:0px;}");
	DealersCards->setStyleSheet("#DealersCardsGroupBox{border:0px;}");

	// Set the size and geometry of each card hand container
	PlayersCards->setGeometry(195*XScale, 310*YScale, 300*XScale, 300*YScale);
	DealersCards->setGeometry(230*XScale, 50*YScale, 300*XScale, 300*YScale);

	// Set initial hand positions
	int PlayerHandPosition = 0;
	int DealerHandPosition = 0;

	// Create a card label for every possible card in each hand
	// No hand can hold more than 11 cards
	for(int CardNumber = 0; CardNumber < 11; CardNumber++)
	{
		PlayerHandPosition += 24;
		DealerHandPosition += 19;

		DealersHand[CardNumber] = new QLabel(DealersCards);
		DealersHand[CardNumber]->setGeometry(DealerHandPosition*XScale, 0, 191*XScale, 250*YScale);
		DealersHand[CardNumber]->lower();

		PlayersHand[CardNumber] = new QLabel(PlayersCards);
		PlayersHand[CardNumber]->setGeometry(PlayerHandPosition*XScale, 0, 191*XScale, 250*YScale);
		PlayersHand[CardNumber]->lower();
	}

	// Sound file locations
	DrawFile = ":/Sounds/DrawCard.wav";
	BustFile = ":/Sounds/Bust.wav";
	GaspFile = ":/Sounds/Gasp.wav";
	YeahFile = ":/Sounds/Yeah.wav";
	DealerDrawFile = ":/Sounds/DrawCard.wav";
	PunchFile = ":/Sounds/Punch.wav";
	ChipFile = ":/Sounds/Chip.wav";
	ChipFile2 = ":/Sounds/Chip2.wav";
	ChipFile3 = ":/Sounds/Chip3.wav";
	ChingFile = ":/Sounds/Ching.wav";

	// Create a set of menu actions
  	NewGame = new QAction("&New Game", this);
	ToggleSound = new QAction("&Toggle Sound", this);
	About = new QAction("&About", this);
	Quit = new QAction("&Quit", this);

	// Allow ToggleSound action to be checkable
	ToggleSound->setCheckable(true);
	ToggleSound->setChecked(true);

	// Create a menu and populate it with the actions
	QMenu *Menu;
	Menu = menuBar()->addMenu("&Menu");
	Menu->addAction(NewGame);
	Menu->addAction(ToggleSound);
	Menu->addAction(About);
	Menu->addAction(Quit);

	// Link menu actions to functions
	connect(About, SIGNAL(triggered()), this, SLOT(DisplayAboutBox()));
	connect(Quit, SIGNAL(triggered()), qApp, SLOT(quit()));

	// Set the two font colours to be used throughout the game
	// Black for all text...
	QPalette* TextPalette = new QPalette();
	TextPalette->setColor(QPalette::WindowText, Qt::black);

	// ...except the bet value text which is a kind of gold
	QRgb BetTextRGBValue;
	BetTextRGBValue = qRgb(205, 205, 0);
	QPalette* BetValueTextPalette = new QPalette();
	BetValueTextPalette->setColor(QPalette::WindowText, BetTextRGBValue);

	// Setup the different fonts to be used throughout the game...
	QFont LabelFont("mry_KacstQurn");
	QFont StatusFont("mry_KacstQurn");
	QFont ResultsFont("mry_KacstQurn");
	QFont HandValueFont("mry_KacstQurn");

	// ...and their associated sizes
	LabelFont.setPointSizeF(24.0*XScale);
	StatusFont.setPointSizeF(22.0*XScale);
	ResultsFont.setPointSizeF(16.0*XScale);
	HandValueFont.setPointSizeF(16.0*XScale);

	// Create bet ring graphic and set its geometry
	labelBetRing = new QLabel(this);
	labelBetRing->setGeometry(240*XScale, 520*YScale, 140*XScale, 89*YScale);
	QPixmap BetRingPixMap(":/Images/Bet_Ring2.gif");
	labelBetRing->setScaledContents(true);
	labelBetRing->setPixmap(BetRingPixMap);

	// Create stack value graphic and set its geometry
	labelStackSpot = new QLabel(this);
	labelStackSpot->setGeometry(240*XScale, 665*YScale, 139*XScale, 39*YScale);
	QPixmap StackSpotPixMap(":/Images/StackSpot.png");
	labelStackSpot->setScaledContents(true);
	labelStackSpot->setPixmap(StackSpotPixMap);

	// Create hand value graphic and set its geometry
	labelHandValueSpot = new QLabel(this);
	labelHandValueSpot->setGeometry(240*XScale, 655*YScale, 139*XScale, 39*YScale);
	QPixmap HandValueSpotPixMap(":/Images/HandValueSpot.png");
	labelHandValueSpot->setScaledContents(true);
	labelHandValueSpot->setPixmap(HandValueSpotPixMap);
	// Not visible at startup
	labelHandValueSpot->setVisible(false);

	// Create dealer hand value graphic and set its geometry
	labelDealersHandValueSpot = new QLabel(this);
	labelDealersHandValueSpot->setGeometry(240*XScale, 655*YScale, 139*XScale, 39*YScale);
	QPixmap DealerHandValueSpotPixMap(":/Images/HandValueSpot.png");
	labelDealersHandValueSpot->setScaledContents(true);
	labelDealersHandValueSpot->setPixmap(HandValueSpotPixMap);
	// Not visible at startup
	labelDealersHandValueSpot->setVisible(false);

	// Create results summary graphic and set its geometry
	labelResultsBubble = new QLabel(this);
	labelResultsBubble->setGeometry(25*XScale, 250*YScale, 567*XScale, 165*YScale);
	QPixmap ResultsBubblePixMap(":/Images/ResultsSummary.gif");
	labelResultsBubble->setScaledContents(true);	
	labelResultsBubble->setPixmap(ResultsBubblePixMap);
	// Not visible at startup
	labelResultsBubble->setVisible(false);

	// Create game status graphic and set its geometry
	labelStatusBubble = new QLabel(this);
	labelStatusBubble->setGeometry(20*XScale, 15*YScale, 559*XScale, 94*YScale);
	QPixmap StatusBubblePixMap(":/Images/StatusBubble.png");
	labelStatusBubble->setScaledContents(true);
	labelStatusBubble->setPixmap(StatusBubblePixMap);

	// Create bet value label and set its geometry and font
	labelBetValue = new QLabel(this);
	labelBetValue->setGeometry(110*XScale, 555*YScale, 121*XScale, 41*YScale);
	labelBetValue->setPalette(*BetValueTextPalette);
	labelBetValue->setFont(LabelFont);
	labelBetValue->setAlignment(Qt::AlignRight);

	// Create stack value label and set its geometry
	labelStackValue = new QLabel(this);
	labelStackValue->setGeometry(250*XScale, 662*YScale, 121*XScale, 41*YScale);
	labelStackValue->setPalette(*TextPalette);	
	labelStackValue->setFont(LabelFont);
	labelStackValue->setAlignment(Qt::AlignCenter);

	// Create game status label and set its geometry
	labelGameStatus = new QLabel(this);
	labelGameStatus->setGeometry(30*XScale, 20*YScale, 541*XScale, 81*YScale);
	labelGameStatus->setPalette(*TextPalette);
	labelGameStatus->setFont(StatusFont);
	labelGameStatus->setAlignment(Qt::AlignCenter);

	// Create player hand value label and set its geometry
	labelPlayersHandValue = new QLabel(this);
	labelPlayersHandValue->setPalette(*TextPalette);
	labelPlayersHandValue->setFont(HandValueFont);
	labelPlayersHandValue->setAlignment(Qt::AlignCenter);
	// Not visible at startup
	labelPlayersHandValue->setVisible(false);

	// Create dealer hand value label and set its geometry
	labelDealersHandValue = new QLabel(this);
	labelDealersHandValue->setPalette(*TextPalette);
	labelDealersHandValue->setFont(HandValueFont);
	labelDealersHandValue->setAlignment(Qt::AlignCenter);
	// Not visible at startup
	labelDealersHandValue->setVisible(false);

	// Create results summary label and set its geometry
	labelResultsSummary = new QLabel(this);
	labelResultsSummary->setGeometry(35*XScale, 270*YScale, 547*XScale, 315*YScale);
	labelResultsSummary->setPalette(*TextPalette);
	labelResultsSummary->setFont(ResultsFont);
	labelResultsSummary->setAlignment(Qt::AlignTop);
	// Not visible at startup
	labelResultsSummary->setVisible(false);

	// Create player bust graphic and set its geometry
	labelBustText = new QLabel(this);
	labelBustText->setGeometry(160*XScale, 330*YScale, 321*XScale, 141*YScale);
	QPixmap BustTextPixMap(":/Images/BustText.gif");
	labelBustText->setPixmap(BustTextPixMap);
	labelBustText->setScaledContents(true);
	// Not visible at startup
	labelBustText->setVisible(false);

	// Create dealer bust graphic and set its geometry
	labelDealersBustText = new QLabel(this);
	labelDealersBustText->setGeometry(160*XScale, 120*YScale, 291*XScale, 111*YScale);
	labelDealersBustText->setPixmap(BustTextPixMap);
	labelDealersBustText->setScaledContents(true);
	// Not visible at startup
	labelDealersBustText->setVisible(false);

	// Create player blackjack graphic and set its geometry
	labelBlackjackText = new QLabel(this);
	labelBlackjackText->setGeometry(100*XScale, 330*YScale, 400*XScale, 227*YScale);
	QPixmap BlackjackTextPixMap(":/Images/BlackjackText.gif");
	labelBlackjackText->setPixmap(BlackjackTextPixMap);
	labelBlackjackText->setScaledContents(true);
	// Not visible at startup
	labelBlackjackText->setVisible(false);

	// Create dealer blackjack graphic and set its geometry
	labelDealersBlackjackText = new QLabel(this);
	labelDealersBlackjackText->setGeometry(140*XScale, 125*YScale, 320*XScale, 168*YScale);
	labelDealersBlackjackText->setPixmap(BlackjackTextPixMap);
	labelDealersBlackjackText->setScaledContents(true);
	// Not visible at startup
	labelDealersBlackjackText->setVisible(false);

	// Create draggable chip pile for fives
    FivePile = new ChipPile(this, false);
	FivePile->populate(5);
	FivePile->setGeometry(15*XScale, 690*YScale, 88*XScale, 61*YScale);
	FivePile->raise();

	// Create draggable chip pile for tens
	TenPile = new ChipPile(this, false);
	TenPile->populate(10);
	TenPile->setGeometry(135*XScale, 705*YScale, 88*XScale, 61*YScale);
	TenPile->raise();

	// Create draggable chip pile for twenty-fives
	TwentyFivePile = new ChipPile(this, false);	
	TwentyFivePile->populate(25);
	TwentyFivePile->setGeometry(265*XScale, 715*YScale, 88*XScale, 61*YScale);
	TwentyFivePile->raise();

	// Create draggable chip pile for fiftys
	FiftyPile = new ChipPile(this, false);	
	FiftyPile->populate(50);
	FiftyPile->setGeometry(385*XScale, 705*YScale, 88*XScale, 61*YScale);
	FiftyPile->raise();

	// Create draggable chip pile for hundreds
	HundredPile = new ChipPile(this, false);	
	HundredPile->populate(100);
	HundredPile->setGeometry(500*XScale, 690*YScale, 88*XScale, 61*YScale);
	HundredPile->raise();

	// Create draggable chip bet pile
	BettingPile = new ChipPile(this, true);
	BettingPile->populate(200);
	BettingPile->setGeometry(210*XScale, 414*YScale, 200*XScale, 220*YScale);

	// Create clickable done button
	DoneButton = new ClickableLabel(this);
	DoneButton->setGeometry(300*XScale, 580*YScale, 100*XScale, 37*YScale);
	QPixmap DoneButtonPixMap(":/Images/DoneGreenButtonSmall.png");
	DoneButton->setScaledContents(true);
	DoneButton->setPixmap(DoneButtonPixMap);
	
	// Create clickable hit button
	HitButton = new ClickableLabel(this);
	HitButton->setGeometry(335*XScale, 350*YScale, 100*XScale, 37*YScale);
	QPixmap HitButtonPixMap(":/Images/HitButtonSmall.png");
	HitButton->setScaledContents(true);
	HitButton->setPixmap(HitButtonPixMap);

	// Create clickable stand button
	StandButton = new ClickableLabel(this);
	StandButton->setGeometry(345*XScale, 382*YScale, 100*XScale, 37*YScale);
	QPixmap StandButtonPixMap(":/Images/StandButtonSmall.png");
	StandButton->setScaledContents(true);
	StandButton->setPixmap(StandButtonPixMap);

	// Create clickable yes button
	YesButton = new ClickableLabel(this);
	YesButton->setGeometry(335*XScale, 350*YScale, 100*XScale, 37*YScale);
	QPixmap YesButtonPixMap(":/Images/YesButtonSmall.png");
	YesButton->setScaledContents(true);
	YesButton->setPixmap(YesButtonPixMap);

	// Create clickable no button
	NoButton = new ClickableLabel(this);
	NoButton->setGeometry(345*XScale, 382*YScale, 100*XScale, 37*YScale);
	QPixmap NoButtonPixMap(":/Images/NoButtonSmall.png");
	NoButton->setScaledContents(true);
	NoButton->setPixmap(NoButtonPixMap);

	// Create clickable surrender button
	SurrenderButton = new ClickableLabel(this);
	SurrenderButton->setGeometry(355*XScale, 414*YScale, 100*XScale, 37*YScale);
	QPixmap SurrenderButtonPixMap(":/Images/SurrenderButtonSmall.png");
	SurrenderButton->setScaledContents(true);
	SurrenderButton->setPixmap(SurrenderButtonPixMap);

	// Create clickable split button
	SplitButton = new ClickableLabel(this);
	SplitButton->setGeometry(365*XScale, 446*YScale, 100*XScale, 37*YScale);
	QPixmap SplitButtonPixMap(":/Images/SplitButtonSmall.png");
	SplitButton->setScaledContents(true);
	SplitButton->setPixmap(SplitButtonPixMap);

	// Create clickable double button
	DoubleButton = new ClickableLabel(this);
	DoubleButton->setGeometry(375*XScale, 478*YScale, 100*XScale, 37*YScale);
	QPixmap DoubleButtonPixMap(":/Images/DoubleButtonSmall.png");
	DoubleButton->setScaledContents(true);
	DoubleButton->setPixmap(DoubleButtonPixMap);

	// Set up the About Box
	AboutBox = new QWidget;
	AboutBox->setGeometry(50, 580, 525, 155);
	AboutBox->setFixedWidth(580);
	AboutBox->setWindowTitle("About Blackjack");
	AboutBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	labelAboutPicture = new QLabel(AboutBox);
	labelAboutPicture->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	labelAbout = new QLabel(AboutBox);
	labelAbout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
	labelAbout->setWordWrap(true);
	labelAbout->setScaledContents(true);
	labelAbout->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
	labelAbout->setOpenExternalLinks(true);

	OKButton = new QPushButton("&OK", AboutBox);
	OKButton->setDefault(true);
	LicenceButton = new QPushButton("&Licence", AboutBox);
	CreditsButton = new QPushButton("&Credits", AboutBox);
	AboutButton = new QPushButton("&About", AboutBox);

	LicenceButton->setFocusPolicy(Qt::NoFocus);
	CreditsButton->setFocusPolicy(Qt::NoFocus);
	AboutButton->setFocusPolicy(Qt::NoFocus);

	AboutBoxMapper = new QSignalMapper();
	connect(OKButton, SIGNAL(clicked()), AboutBox, SLOT(hide()));
	connect(LicenceButton, SIGNAL(clicked()), AboutBoxMapper, SLOT(map()));
	connect(CreditsButton, SIGNAL(clicked()), AboutBoxMapper, SLOT(map()));
	connect(AboutButton, SIGNAL(clicked()), AboutBoxMapper, SLOT(map()));
	AboutBoxMapper->setMapping(LicenceButton, 2);
	AboutBoxMapper->setMapping(CreditsButton, 3);
	AboutBoxMapper->setMapping(AboutButton, 1);
	connect(AboutBoxMapper, SIGNAL(mappedInt(int)), this, SLOT(ChangeAboutBoxText(int)));

    QVBoxLayout *PicLayout = new QVBoxLayout();
	PicLayout->addWidget(labelAboutPicture);
	QSpacerItem* VerticalSpacer = new QSpacerItem(0, 550, QSizePolicy::Minimum, QSizePolicy::Expanding);
	PicLayout->addItem(VerticalSpacer);

    QHBoxLayout *TextLayout = new QHBoxLayout();
    QHBoxLayout *ButtonLayout = new QHBoxLayout();
    QVBoxLayout *AboutLayout = new QVBoxLayout(AboutBox);

	TextLayout->addLayout(PicLayout);
	TextLayout->addWidget(labelAbout);

	ButtonLayout->addWidget(LicenceButton);
	ButtonLayout->addWidget(CreditsButton);
	ButtonLayout->addWidget(AboutButton);
	ButtonLayout->addWidget(OKButton);

	AboutLayout->addLayout(TextLayout);
	AboutLayout->addLayout(ButtonLayout);

	ChangeAboutBoxText(1);
	AboutButton->hide();

	AboutBox->setLayout(AboutLayout);
	AboutBox->setWindowFlags(Qt::Dialog);

	// Create a new game thread
	myThread = new GameThread();

	// Connect game thread signals to the GUI slots

	// Updates during play
	connect(myThread, SIGNAL(UpdatePlayersHand(QString,int)), this, SLOT(UpdatePlayersHand(QString,int)));
	connect(myThread, SIGNAL(UpdateDealersHand(QString,int)), this, SLOT(UpdateDealersHand(QString,int)));
	connect(myThread, SIGNAL(UpdatePlayersHandValue(QString)), this, SLOT(UpdatePlayersHandValue(QString)));
	connect(myThread, SIGNAL(UpdateDealersHandValue(QString)), this, SLOT(UpdateDealersHandValue(QString)));
	connect(myThread, SIGNAL(UpdateStackValue(QString)), this, SLOT(UpdateStackValue(QString)));
	connect(myThread, SIGNAL(UpdateBetValue(QString)), this, SLOT(UpdateBetValue(QString)));
	connect(myThread, SIGNAL(UpdateGameStatus(QString)), this, SLOT(UpdateGameStatus(QString)));
	connect(myThread, SIGNAL(UpdateResultsSummary(QString)), this, SLOT(UpdateResultsSummary(QString)));
	connect(myThread, SIGNAL(EnableChips(bool)), this, SLOT(EnableChips(bool)));
	// Sound Effects
	connect(myThread, SIGNAL(PlayWinSound()), this, SLOT(PlayWinSound()));
	connect(myThread, SIGNAL(PlayLoseSound()), this, SLOT(PlayLoseSound()));
	// Tidying up after play
	connect(myThread, SIGNAL(ClearPlayersHand()), this, SLOT(ClearPlayersHand()));
	connect(myThread, SIGNAL(ClearDealersHand()), this, SLOT(ClearDealersHand()));
	connect(myThread, SIGNAL(ClearChips()), this, SLOT(HideHandValues()));
	// Detect game thread has finished
	connect(myThread, SIGNAL(finished()), this, SLOT(HandleEndGame()));


	connect(myThread, SIGNAL(ButtonVisibility(bool, bool, bool, bool, bool, bool, bool, bool)), this, SLOT(HideButtons(bool, bool, bool, bool, bool, bool, bool, bool)));
	connect(myThread, SIGNAL(ResultTextVisibility(bool, bool, bool, bool)), this, SLOT(ResultText(bool, bool, bool, bool)));

	// Connect game thread signal to a chip object slot
	connect(myThread, SIGNAL(ClearChips()), BettingPile, SLOT(ClearChips()));

	// Connect chip object signals to the GUI slots
	connect(FivePile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));
	connect(TenPile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));
	connect(TwentyFivePile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));
	connect(FiftyPile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));
	connect(HundredPile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));
	connect(BettingPile, SIGNAL(PlayChipSound()), this, SLOT(PlayChipSound()));

	// Connect chip object signals to the thread slots
	connect(BettingPile, SIGNAL(IncreaseBet(float)), myThread, SIGNAL(IncreaseBet(float)));
	connect(BettingPile, SIGNAL(DecreaseBet(float)), myThread, SIGNAL(DecreaseBet(float)));

	// Create new signal mapper object for the buttons
	ButtonMapping = new QSignalMapper();

	// Connect button presses to the signal map...
	connect(HitButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
	connect(StandButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
	connect(DoubleButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
	connect(SplitButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
	connect(SurrenderButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
	connect(YesButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
	connect(NoButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
	// ...including the New Game menu item, but not including...
	connect(NewGame, SIGNAL(triggered()), ButtonMapping, SLOT(map()));
	// ...the done button which is linked to bet completion signal
	connect(DoneButton, SIGNAL(clicked()), myThread, SIGNAL(BetDone()));

	// Give each button a unique ID
	ButtonMapping->setMapping(HitButton, 1);
	ButtonMapping->setMapping(StandButton, 2);
	ButtonMapping->setMapping(DoubleButton, 3);
	ButtonMapping->setMapping(SplitButton, 4);
	ButtonMapping->setMapping(SurrenderButton, 5);
	ButtonMapping->setMapping(YesButton, 6);
	ButtonMapping->setMapping(NoButton, 7);
	ButtonMapping->setMapping(NewGame, 8);

	// Connect the signal map to the game thread
	connect(ButtonMapping, SIGNAL(mappedInt(int)), myThread, SLOT(ChoiceMade(int)));

	// Chip piles need re-raising to be clickable on some systems
	FivePile->raise();
	TenPile->raise();
	TwentyFivePile->raise();
	FiftyPile->raise();
	HundredPile->raise();

	// Initiate a new game
	MakeConnections();
}

// Destructor
MainWindow::~MainWindow()
{
	delete myThread;
}

// Initiate a new game
void MainWindow::MakeConnections()
{
	// Clear the text and reset the stack
	UpdateResultsSummary("");
	UpdateStackValue("100");
	// Set yes/no buttons to work with insurance question again (for after game over)
	disconnect(YesButton, SIGNAL(clicked()), this, SLOT(MakeConnections()));
	disconnect(NoButton, SIGNAL(clicked()), qApp, SLOT(quit()));
	connect(YesButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
	connect(NoButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
	// Start the game!
	myThread->start();
}

// Enable or disable control of the chips
void MainWindow::EnableChips(bool ActiveState)
{
	FivePile->setInactive(ActiveState);
	TenPile->setInactive(ActiveState);
	TwentyFivePile->setInactive(ActiveState);
	FiftyPile->setInactive(ActiveState);
	HundredPile->setInactive(ActiveState);
	BettingPile->setInactive(ActiveState);
}

// Hide all hand values and their associated graphics
void MainWindow::HideHandValues()
{
	labelHandValueSpot->setVisible(false);
	labelDealersHandValueSpot->setVisible(false);
	labelPlayersHandValue->setVisible(false);
	labelDealersHandValue->setVisible(false);
}

// Change the text and icon displayed in the About Box depending on button selection
void MainWindow::ChangeAboutBoxText(int TextSet)
{
	LicenceButton->show();
	CreditsButton->show();
	AboutButton->show();

	// Prepare different images for each option
	QPixmap LicencePixMap(":/Images/GPLV3.png");
	LicencePixMap = LicencePixMap.scaled(127, 51);

	QPixmap AboutPixMap(":/Images/AboutCards.png");
	AboutPixMap = AboutPixMap.scaled(100, 100);	

	QPixmap CreditsPixMap(":/Images/JimboFace.gif");
	CreditsPixMap = CreditsPixMap.scaled(100, 100);		

	// Change the text and icon depending on button selection
	switch (TextSet)
	{
		case 1:
			AboutBoxText = "<b><i><font size = 4>Blackjack</i></b><br><br>Another JimboMonkey Production<br>Version 1.0<br><br>Copyright \u24B8 2014 JimboMonkey Productions<br>";
			labelAboutPicture->setPixmap(AboutPixMap);
			AboutButton->hide();
			break;
		case 2:
			AboutBoxText = "<b><i><font size = 4>Licence</i></b><br><br>Copyright 2014 James O'Hea<br><br>Blackjack is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.<br><br>Blackjack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.<br><br> You should have received a copy of the GNU General Public License along with Blackjack.  If not, see <a href=\"http://www.gnu.org/licenses\">http://www.gnu.org/licenses</a><br><br><p>";
			labelAboutPicture->setPixmap(LicencePixMap);
			LicenceButton->hide();	
			break;
		case 3:
			AboutBoxText = "<b><i><font size = 4>Credits</i></b><br><br><b>Sounds</b><ul><li>All sounds are from <a href=\"http://www.freesound.org\">freesound.org:</a></li></ul><ul><li>Drawing Cards is 'draw-card2' by themfish</li><li>Bust is 'broken_bottle_01' by cdrk</li><li>Chip Sounds are 'Ceramic Chips' by ArtOrDie</li><li>Win is 'Till With Bell' by Benboncan</li><li>Gasp is 'Gasp-03' by EBrown15</li><li>Lose is 'kung fu punch' by nextmaking</li><li>Yeah is 'yeah' by totya</li></ul><b>Graphics</b><ul><li>Icon by tango from <a href=\"http://openiconlibrary.sourceforge.net\">openiconlibrary.sourceforge.net</a></li><li>Cards by nicubunu from <a href=\"http://www.openclipart.org\">openclipart.org</a></li><li>All other graphics made by JimboMonkey</li><li>Chips, graphical text, and the table logo relied on tutorials by <a href=\"http://www.youtube.com/user/bluelightningtv\">BlueLightningTV</a>, and <a href=\"http://www.tutorial9.net>tutorial9\">Tutorial9</a></li></ul><b>Code</b><ul><li>All code by JimboMonkey<//li></ul><br>";
			labelAboutPicture->setPixmap(CreditsPixMap);
			CreditsButton->hide();
			break;
	}

	// Adjust the size of the About Box to accomodate the text
	labelAbout->setText(AboutBoxText);		
	QSize mysize = labelAbout->sizeHint();
	AboutBox->setFixedHeight(mysize.height() + 60);
}

// Check how game ended - New Game request or Game Over
void MainWindow::HandleEndGame()
{
	// Game Over scenario
	if(labelGameStatus->text() == "Game Over")
	{
		// Change insurance buttons to handle game over response
		disconnect(YesButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
		disconnect(NoButton, SIGNAL(clicked()), ButtonMapping, SLOT(map()));
		connect(YesButton, SIGNAL(clicked()), this, SLOT(MakeConnections()));
		connect(NoButton, SIGNAL(clicked()), qApp, SLOT(quit()));
		// Reposition them and display
		YesButton->setGeometry(200*XScale, 360*YScale, 100*XScale, 37*YScale);
		NoButton->setGeometry(320*XScale, 360*YScale, 100*XScale, 37*YScale);
		YesButton->setVisible(true);
		NoButton->setVisible(true);
		YesButton->raise();
		NoButton->raise();
	}
	// New Game requested
	else
	{
		MakeConnections();
	}
}

// Display About Box after menu call
void MainWindow::DisplayAboutBox()
{
	ChangeAboutBoxText(1);
	AboutBox->show();
}

void MainWindow::PlaySound(QString SoundFile)
{
	if (ToggleSound->isChecked())
	{
		QSound::play(SoundFile);
	}
}

void MainWindow::HideButtons(bool HitVisible, bool StandVisible, bool SurrenderVisible, bool DoubleVisible, bool SplitVisible, bool YesVisible, bool NoVisible, bool DoneVisible)
{
	HitButton->setVisible(HitVisible);
	StandButton->setVisible(StandVisible);
	SurrenderButton->setVisible(SurrenderVisible);
	DoubleButton->setVisible(DoubleVisible);
	SplitButton->setVisible(SplitVisible);
	YesButton->setVisible(YesVisible);
	NoButton->setVisible(NoVisible);
	DoneButton->setVisible(DoneVisible);
	HitButton->raise();
	StandButton->raise();
	SurrenderButton->raise();
	DoubleButton->raise();
	SplitButton->raise();
	YesButton->raise();
	NoButton->raise();
	DoneButton->raise();
}

// Display the appropriate 
void MainWindow::ResultText(bool BustVisible, bool DealerBustVisible, bool BlackjackVisible, bool DealerBlackjackVisible)
{
	labelBustText->setVisible(BustVisible);
	labelDealersBustText->setVisible(DealerBustVisible);
	labelBustText->raise();
	labelDealersBustText->raise();

	labelBlackjackText->setVisible(BlackjackVisible);
	labelDealersBlackjackText->setVisible(DealerBlackjackVisible);
	labelBlackjackText->raise();
	labelDealersBlackjackText->raise();

	if(BustVisible == true or DealerBustVisible == true)
	{
		PlaySound(BustFile);
	}
	else if(BlackjackVisible == true)
	{
		PlaySound(YeahFile);
	}
	else if(DealerBlackjackVisible == true)
	{
		PlaySound(GaspFile);
	}
}

// Add a card to the player's hand.  Adjust the hand position to keep it central
void MainWindow::UpdatePlayersHand(QString LoadCardName, int CardPosition)
{
	QImage CardImage(LoadCardName);
	int NewHandPosition;

	// Only play the draw card sound if the card is drawn during play
	// ie not during the initial draw
	if(CardPosition > 1)
	{
		PlaySound(DrawFile);
	}

	// Load the image for the given card into the given position (and scale if needed)
	// New cards are raised to ensure all cards of hand are visible
	PlayersHand[CardPosition]->setPixmap(QPixmap::fromImage(CardImage).scaledToWidth(161*XScale));
	PlayersHand[CardPosition]->show();
	PlayersHand[CardPosition]->raise();
	
	// Adjust the overall hand position and value as cards are drawn to ensure the hand remains centered on the screen
	NewHandPosition = 216 - ((CardPosition + 1) * 12);
	PlayersCards->setGeometry(NewHandPosition*XScale, 310*YScale, 541*XScale, 331*YScale);
	labelHandValueSpot->setGeometry((238 - (CardPosition + 1) * 12)*XScale, 470*YScale, 55*XScale, 55*YScale);
	labelPlayersHandValue->setGeometry((210 - (CardPosition + 1) * 12)*XScale, 482*YScale, 111*XScale, 31*YScale);
	labelHandValueSpot->raise();
	labelPlayersHandValue->raise();

	// Adjust all the button positions as well
	HitButton->setGeometry((315 + (CardPosition + 1) * 10)*XScale, 350*YScale, 100*XScale, 37*YScale);
	YesButton->setGeometry((315 + (CardPosition + 1) * 10)*XScale, 350*YScale, 100*XScale, 37*YScale);
	StandButton->setGeometry((325 + (CardPosition + 1) * 10)*XScale, 382*YScale, 100*XScale, 37*YScale);
	NoButton->setGeometry((325 + (CardPosition + 1) * 10)*XScale, 382*YScale, 100*XScale, 37*YScale);
	SurrenderButton->setGeometry((335 + (CardPosition + 1) * 10)*XScale, 414*YScale, 100*XScale, 37*YScale);
	SplitButton->setGeometry((345 + (CardPosition + 1) * 10)*XScale, 446*YScale, 100*XScale, 37*YScale);
	DoubleButton->setGeometry((355 + (CardPosition + 1) * 10)*XScale, 478*YScale, 100*XScale, 37*YScale);
}

// Add a card to the dealer's hand.  Adjust the hand position to keep it central
void MainWindow::UpdateDealersHand(QString LoadCardName, int CardPosition)
{
    QImage CardImage(LoadCardName);
	int NewHandPosition;

	// Only play the draw card sound if the card is drawn during play
	// ie not during the initial draw
	if(CardPosition > 0 and LoadCardName != ":/DealerCards/CardBack.png")
	{
		PlaySound(DealerDrawFile);
	}

	// Load the image for the given card into the given position (and scale if needed)
	// New cards are raised to ensure all cards of hand are visible
	DealersHand[CardPosition]->setPixmap(QPixmap::fromImage(CardImage).scaledToWidth(94*XScale));
	DealersHand[CardPosition]->show();
	DealersHand[CardPosition]->raise();

	// Adjust the overall hand position and value as cards are drawn to ensure the hand remains centered on the screen
	NewHandPosition = 250 - ((CardPosition + 1) * 10);
	DealersCards->setGeometry(NewHandPosition*XScale, 50*YScale, 541*XScale, 331*YScale);
	labelDealersHandValueSpot->setGeometry((265 - (CardPosition + 1) * 10)*XScale, 170*YScale, 55*XScale, 55*YScale);
	labelDealersHandValue->setGeometry((235 - (CardPosition + 1) * 10)*XScale, 181*YScale, 111*XScale, 31*YScale);
	labelDealersHandValueSpot->raise();
	labelDealersHandValue->raise();
}

// Play a win sound
void MainWindow::PlayWinSound()
{
	PlaySound(ChingFile);
}

// Play a fail sound
void MainWindow::PlayLoseSound()
{
	PlaySound(PunchFile);
}

// Play a random chip movement sound
void MainWindow::PlayChipSound()
{
	int RandomNumber;

	// Initialize random seed from time
	srand (time(NULL));

	// Generate a random number between 1 and 3
	RandomNumber = ((rand() % 3) + 1);

	// Pick one of 3 sounds
	switch (RandomNumber)
	{
		case 1:
			PlaySound(ChipFile);
			break;
		case 2:
			PlaySound(ChipFile2);
			break;
		case 3:
			PlaySound(ChipFile3);
			break;
	}	
}

// Update the game status
void MainWindow::UpdateGameStatus(QString Status)
{
	labelGameStatus->setText(Status);
}

// Update the results summary
void MainWindow::UpdateResultsSummary(QString ResultsSummary)
{
	// If a blank string is passed in, hide the summary and graphic
	if (ResultsSummary == "")
	{
		labelResultsSummary->setVisible(false);
		labelResultsBubble->setVisible(false);
	}
	else
	{
		labelResultsSummary->setVisible(true);
		labelResultsBubble->setVisible(true);
		labelResultsBubble->raise();
		labelResultsSummary->raise();
		labelResultsSummary->setText(ResultsSummary);
		labelResultsSummary->setAlignment(Qt::AlignHCenter);
	}
}

// Update the GUI player's hand value
void MainWindow::UpdatePlayersHandValue(QString HandValue)
{
	labelPlayersHandValue->setText(HandValue);
	labelHandValueSpot->setVisible(true);
	labelPlayersHandValue->setVisible(true);
}

// Update the GUI dealer's hand value
void MainWindow::UpdateDealersHandValue(QString HandValue)
{
	labelDealersHandValue->setText(HandValue);
	labelDealersHandValueSpot->setVisible(true);
	labelDealersHandValue->setVisible(true);
}

// Update stack value and refresh GUI to only present playable chips
void MainWindow::UpdateStackValue(QString StackValue)
{
	labelStackValue->setText(StackValue);
	// Float is used instead of int because half values 
	// are possible with side bet or blackjack payouts
	float NewStackValue = StackValue.toFloat();

	if(NewStackValue >= 5.0)
	{
		FivePile->setVisible(true);
	}
	else
	{
		FivePile->setVisible(false);
	}

	if(NewStackValue >= 10.0)
	{
		TenPile->setVisible(true);
	}
	else
	{
		TenPile->setVisible(false);
	}

	if(NewStackValue >= 25.0)
	{
		TwentyFivePile->setVisible(true);
	}
	else
	{
		TwentyFivePile->setVisible(false);
	}

	if(NewStackValue >= 50.0)
	{
		FiftyPile->setVisible(true);
	}
	else
	{
		FiftyPile->setVisible(false);
	}

	if(NewStackValue >= 100.0)
	{
		HundredPile->setVisible(true);
	}	
	else
	{
		HundredPile->setVisible(false);
	}
}

// Update the GUI bet value box
void MainWindow::UpdateBetValue(QString BetValue)
{
	// Don't display the Done button or
	// any number if the value is zero
	if(BetValue == "0")
	{
		BetValue = "";
		DoneButton->setVisible(false);
	}
	else
	{
		DoneButton->setVisible(true);
	}
	// Write the value to the box
	labelBetValue->setText(BetValue);
}

// 'Clear' player's hand by hiding all card labels
void MainWindow::ClearPlayersHand()
{
	int CardNumber;

	for(CardNumber = 0; CardNumber < 11; CardNumber++)
	{
		PlayersHand[CardNumber]->hide();
	}
}

// 'Clear' dealer's hand by hiding all card labels
void MainWindow::ClearDealersHand()
{
	int CardNumber;

	for(CardNumber = 0; CardNumber < 11; CardNumber++)
	{
		DealersHand[CardNumber]->hide();
	}
}

