/*=@Header==============================================================
 * $Source$
 *
 * @Desc:	Rahmen - related functions 
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.1  1994/06/19 16:39:46  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

static const char RCSid[]=\
"$Id$";

#define _rahmen_c


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <alloc.h>
#include <mem.h>
#include <string.h>
#include <dos.h>
#include <math.h>
#include <process.h>

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "paratext.h"

extern char *InfluenceModeNames[];

/* Startpositionen fuer Rahmen-Texte */
#define LEFTINFO_X 	12
#define LEFTINFO_Y	8

#define RIGHTINFO_X	242
#define RIGHTINFO_Y	8


#define LEFT_TEXT_LEN 10
#define RIGHT_TEXT_LEN 6

// Dieses Array enth"alt die Datenstrukturen "uber alle im Spiel vorkommenden
// Anzeigebalken.
//
// { {point pos}, int len, int hgt, int oldval, int col }
//

bar AllBars[]={
// Bar for the Energydisplay
	{   {3,52}  ,300, 10, 0, FONT_RED },
// Bars for Shield1-4
	{ {260,5 } , 50, 5, 0, FONT_GREEN },
	{ {260,12} , 50, 5, 0, FONT_GREEN },
	{ {260,19} , 50, 5, 0, FONT_GREEN },
	{ {260,26} , 50, 5, 0, FONT_GREEN }
};

void DrawBar(int,int,unsigned char*);

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void DrawBar(int BarCode,int Wert,unsigned char* Screen){
	unsigned char* BarPoint=Screen;
	int xlen;
	int barcol=0;
	int i,j;

	if (Wert<0) Wert=0;
	BarPoint+=AllBars[BarCode].pos.x+AllBars[BarCode].pos.y*SCREENBREITE;

	if (InitBars) {
 		for(i=0;i<AllBars[BarCode].hgt;i++){
			memset(BarPoint,AllBars[BarCode].col,Wert);
			memset(BarPoint+Wert,0,abs(AllBars[BarCode].len-Wert));
			BarPoint+=SCREENBREITE;
		}
		AllBars[BarCode].oldval=Wert;
		return;
	}
	
	if (Wert==AllBars[BarCode].oldval) return;

	xlen=abs(Wert-AllBars[BarCode].oldval);

// Den Cursor an die Position stellen und rot oder schwarz einstellen.	
	if (Wert>AllBars[BarCode].oldval) {
		barcol=AllBars[BarCode].col;
		BarPoint+=AllBars[BarCode].oldval;
	} else BarPoint+=Wert;

// Balken soweit zeichnen, wie die Ver"anderung ausmacht.
	for(i=0;i<AllBars[BarCode].hgt;i++){
		memset(BarPoint,barcol,xlen);
		BarPoint+=SCREENBREITE;
	}

	AllBars[BarCode].oldval=Wert;
}

/*@Function============================================================
@Desc: SayLeftInfo( char* text):
						gibt text oben links im Rahmen aus, loescht
						alten text zuerst

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void SayLeftInfo(char *text, unsigned char *screen ) {
	char textbox[LEFT_TEXT_LEN + 10];
	
	if (!PlusExtentionsOn) {
	/* Hintergrund Textfarbe setzen */
		SetTextColor(FONT_WHITE,FONT_RED);	// FONT_RED, 0
	
		strncpy(textbox, text, LEFT_TEXT_LEN);
		if( strlen(text) < LEFT_TEXT_LEN )
			strncat(textbox, "           ", LEFT_TEXT_LEN -strlen(text));
		textbox[LEFT_TEXT_LEN] = '\0'; 	/* String abschliessen */
	
	/* Text ausgeben */
		DisplayText( textbox, LEFTINFO_X, LEFTINFO_Y, screen,FALSE);
		return;
	}

}

/*@Function============================================================
@Desc: SayRightInfo(char *text): wie SayLeftInfo()

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void SayRightInfo(char *text, unsigned char *screen)
{
	char textbox[RIGHT_TEXT_LEN + 10];

	if (!PlusExtentionsOn) {
	/* Hintergrund Textfarbe richtig setzen */
		SetTextColor(FONT_WHITE, FONT_RED);

		strncpy(textbox, text, RIGHT_TEXT_LEN);
		if( strlen(text) < RIGHT_TEXT_LEN) 
			strncat(textbox, "           ", RIGHT_TEXT_LEN-strlen(text));
		textbox[RIGHT_TEXT_LEN] = '\0';
		
	/* Text ausgeben */
		DisplayText( textbox, RIGHTINFO_X, RIGHTINFO_Y, screen,FALSE);
	}
	
 	return;
} /* SayRightInfo */

 
/*@Function============================================================
@Desc: DisplayRahmen(*screen):  gibt Rahmen mit Info-Texten auf screen aus
    Die Funktion sichert und restauriert jetzt auch die Schriftfarben

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void DisplayRahmen(unsigned char *screen)
{
	unsigned int bg;
	unsigned int fg;

	if (PlusExtentionsOn) PlusRahmen(screen);
	else DisplayBlock(0, 0, RahmenPicture, RAHMENBREITE, RAHMENHOEHE, screen);

/*	GetTextColor(&bg,&fg);
	SetTextColor(FONT_WHITE,FONT_RED);    */    /* BG: Rahmenwei"s FG: FONT_RED */
	SayRightInfo(RightInfo, screen);
	SayLeftInfo(LeftInfo, screen);
/*	SetTextColor(bg,fg); */
	return;
} /* DisplayRahmen */


/*@Function============================================================
@Desc: SetInfoline(): schreibt Modus und Punkte-Werte in die
						globalen Infoline-Variable

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void SetInfoline(void)
{
	char dummy[80];
	/* Modus des Influencers links angeben  */
	strncpy(LeftInfo, InfluenceModeNames[Me.status], LEFT_TEXT_LEN-1);
	LeftInfo[LEFT_TEXT_LEN-1] = '\0';
	
	
	/* Punkte des Users rechts ausgeben */
	strncpy(RightInfo, ltoa(ShowScore, dummy, 10), RIGHT_TEXT_LEN-1);
	RightInfo[RIGHT_TEXT_LEN-1] = '\0';
	
	return;
} /* SetInfoline */


/*@Function============================================================
@Desc: UpdateInfoline(): 		setzt Infos im Rahmen neu (nur RealScreen),
								wenn sie sich geaendert haben: Flimmern 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void UpdateInfoline(void)
{
	static char LastLeft[50];			/* the change-detectors */
	static char LastRight[50];
	int NoNeedToSaveEnv = 1;
	int i;

	if ((Me.status == CONSOLE) || (Me.status == DEBRIEFING)) NoNeedToSaveEnv = 0;
	
	if (!NoNeedToSaveEnv) StoreTextEnvironment();

	
	if( strcmp(LastLeft, LeftInfo) != 0 ) {
	 	SetTextColor(FONT_WHITE,FONT_RED);
		SayLeftInfo(LeftInfo, RealScreen);
		strcpy(LastLeft, LeftInfo);
	}

	if( strcmp(LastRight, RightInfo) != 0 ) {
		SayRightInfo(RightInfo, RealScreen);
		strcpy(LastRight, RightInfo);
	}

	if (!NoNeedToSaveEnv) RestoreTextEnvironment();
	return;
} /* UpdateInfoline */



/*@Function============================================================
@Desc: 	Diese Funktion stellt den leeren Plusrahmen dar

@Ret: 

@Int:
* $Function----------------------------------------------------------*/
void PlusRahmen(unsigned char* Screen){
	int RCount,RCount2;

	if (!PlusExtentionsOn) return;

	if (InitBars) TransparentLadeLBMBild(PARAPLUSRAHMENBILD,Screen,FALSE);

/*	gotoxy(1,2);
	printf("FCU:%s",AllFCUs[Me.MyFCU].FCUName);
	gotoxy(1,3);
	printf("Shield:%s",AllShields[Me.MyShield].shieldname);
	gotoxy(1,4);
	printf("Energy:%d ",Me.energy);

	gotoxy(20,2);
	printf("Shield1:%d",Me.Shield[0]);
	gotoxy(20,3);
	printf("Shield2:%d",Me.Shield[1]);
	gotoxy(20,4);
	printf("Shield3:%d",Me.Shield[2]);
	gotoxy(20,5);
	printf("Shield4:%d",Me.Shield[3]);

*/

	DrawBar(0,Me.energy,Screen);
	DrawBar(1,Me.Shield[0],Screen);
	DrawBar(2,Me.Shield[1],Screen);
	DrawBar(3,Me.Shield[2],Screen);
	DrawBar(4,Me.Shield[3],Screen);

	InitBars=FALSE;
	 
	return;
/*

	Hier steht eine desaktivierte Routine, die den Bildschirm von links nach
	rechts mit vertikaten Farblienien "uberzieht.
	Es handelt sich um eine Routine zu Testzwecken in der "Ubergangszeit zu
	einem ordentlichen Bild als Plusrahmen
	

	for(RCount=0;RCount<320;RCount++){
		for(RCount2=0;RCount2<200;RCount2++) *(Screen+RCount2*SCREENBREITE+RCount)=(char)RCount;
	}
	*/
}

#undef _rahmen_c