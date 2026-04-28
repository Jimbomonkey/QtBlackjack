
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt includes
#include <QMainWindow>
#include <QGroupBox>
#include <QThread>
#include <QLabel>
#include <QSoundEffect>

#include <QtWidgets>

#include "GameThread.hpp"

#include "ClickableLabel.hpp"
#include "ChipPile.hpp"


class MainWindow: public QMainWindow 
{
    Q_OBJECT

	public:
	    MainWindow(QWidget *parent = 0);
	    ~MainWindow();
		GameThread *myThread;
		float XScale;
		float YScale;
	
	private:
		// Grouping hands of cards
		QGroupBox* PlayersCards;
		QGroupBox* DealersCards;

		// Array of card placeholders
		QLabel* DealersHand[12];
		QLabel* PlayersHand[12];

		// Custom widgets for clickable buttons
		ClickableLabel *HitButton;
		ClickableLabel *StandButton;
		ClickableLabel *SplitButton;
		ClickableLabel *DoubleButton;
		ClickableLabel *SurrenderButton;
		ClickableLabel *YesButton; 
		ClickableLabel *NoButton; 
		ClickableLabel *DoneButton; 

		// Mapping button functions
	    QSignalMapper *ButtonMapping;

		// Custom widget for piles of chips
		ChipPile *FivePile;
		ChipPile *TenPile;
		ChipPile *TwentyFivePile;
		ChipPile *FiftyPile;
		ChipPile *HundredPile;
		ChipPile *BettingPile;

		// Graphic elements
		QLabel* labelBetValue;
		QLabel* labelStackValue;
		QLabel* labelGameStatus;
		QLabel* labelResultsSummary;
		QLabel* labelBetRing;
		QLabel* labelStackSpot;
		QLabel* labelBustText;
		QLabel* labelDealersBustText;
		QLabel* labelBlackjackText;
		QLabel* labelDealersBlackjackText;
		QLabel* labelPlayersHandValue;
		QLabel* labelDealersHandValue;
		QLabel* labelHandValueSpot;
		QLabel* labelDealersHandValueSpot;
		QLabel* labelResultsBubble;
		QLabel* labelStatusBubble;

		// Menu actions
		QAction *NewGame;
		QAction *ToggleSound;
		QAction *About;
		QAction *Quit;

		// About Box elements
		QSignalMapper *AboutBoxMapper;
		QWidget *AboutBox;
	    QVBoxLayout *AboutLayout;
	    QHBoxLayout *TextLayout;
	    QHBoxLayout *ButtonLayout;
		QVBoxLayout *PicLayout;
		QLabel *labelAboutPicture;
		QLabel *labelAbout;
		QString AboutBoxText;
		QPushButton *OKButton;
		QPushButton *LicenceButton;
		QPushButton *CreditsButton;
		QPushButton *AboutButton;

		// Sound file locations
		QString DrawFile;
		QString BustFile;
		QString GaspFile;
		QString YeahFile;
		QString DealerDrawFile;
		QString PunchFile;
		QString ChipFile;
		QString ChipFile2;
		QString ChipFile3;
		QString ChingFile;

	private slots:
		void UpdateDealersHand(QString LoadCardName, int CardPosition);
		void UpdatePlayersHand(QString LoadCardName, int CardPosition);
		void UpdatePlayersHandValue(QString HandValue);
		void UpdateDealersHandValue(QString HandValue);
		void UpdateStackValue(QString StackValue);
		void UpdateBetValue(QString BetValue);
		void ClearPlayersHand();
		void ClearDealersHand();
		void UpdateGameStatus(QString Status);
		void HideButtons(bool HitVisible, bool StandVisible, bool SurrenderVisible, bool DoubleVisible, bool SplitVisible, bool YesVisible, bool NoVisible, bool DoneVisible);
		void ResultText(bool BustVisible, bool DealerBustVisible, bool BlackjackVisible, bool DealerBlackjackVisible);
		void HideHandValues();
		void UpdateResultsSummary(QString ResultsSummary);
		void EnableChips(bool ActiveState);
		void HandleEndGame();
		void PlayWinSound();
		void PlayLoseSound();
		void PlayChipSound();
		void DisplayAboutBox();
		void MakeConnections();
		void ChangeAboutBoxText(int TextSet);
		void PlaySound(QString SoundFile);
};

#endif

