/*=Header==============================================================
 * $Source$
 *
 * @Desc: the paraplus main program
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 *-@Header------------------------------------------------------------*/

/* static const char RCSid[]=\
   "$Id$"; */

#define _paraplus_c

/* *********************************************************************** */
/* ************  P R AE P R O Z E S S O R D I R E K T I V E N  ************ */
/* *********************************************************************** */

// Die folgenden Schalter schalten Debugmeldungen in gewissen speziellen Funktionen aus oder ein
#undef DEBUG_MYRANDOM

/* Schalter fuer die bedingte Kompilierung */
#undef DREHSCHUESSE
#undef SCHUSSDEBUG
#undef INFLUNCECLPOSDEBUG
#undef INFLUENCEWALLDEBUG
#undef DEBUGREPORT
#undef BULLETSPEEDUPONRICHTUNG

#undef SAYJOYPOS
#undef SPEICHERSPAREN

#define TITLE_EIN

#undef MODSCHASEIN
#undef NOJUNKWHILEINIT


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>
#include <sys/stat.h>
#include <sys/asoundlib.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "paratext.h"
#include "paravars.h"
#include "ship.h"

#define MOVEENEMYSOFF 		FALSE
#define ANIMATEENEMYSOFF 	FALSE
#define INFLUENCEGETROFFENOFF 	FALSE

#define FENSTEROFF 		FALSE
#define ENERGIEBALKENOFF 	TRUE
#define REDRAW001OFF 		FALSE
#define USEINTSOFF 		FALSE

/* Scroll- Fenster */
#define SCROLLSTARTX		USERFENSTERPOSX 
#define SCROLLSTARTY		SCREENHOEHE
#define CHARSPERLINE		(int)(USERFENSTERBREITE/FONTBREITE)

int ThisMessageTime;

int card = 0;
int device = 0;
void* handle;

struct timeval now, oneframetimestamp, tenframetimestamp, onehundredframetimestamp, differenz;

long oneframedelay=0;
long tenframedelay=0;
long onehundredframedelay=0;
// float oneframedelay, tenframedelay, onehundredframedelay;
float FPSover1=10;
float FPSover10=10;
float FPSover100=10;
int framenr=0;

int TestSound(void);
void CalibratedDelay(long);
void Debriefing(void);
void ShowHighscoreList(void);
void Pause(void);
void UpdateCountersForThisFrame(void);

float Frame_Time(void){
  if (FPSover1 > 10) {
    return (1/FPSover1);
  } else {
    return (1/10);
  }
} // float Frame_Time(void)

/*@Function============================================================
@Desc: main(): the heart of the Game 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
int main(void)
{
  int i;

  GameOver = FALSE;
  QuitProgram = FALSE;
  Conceptview = FALSE;
  
  InterruptInfolineUpdate=TRUE;

  /* Initialisieren der globalen Variablen und Arrays */
  InitParaplus();

#if REDRAW001OFF == 0
  RedrawInfluenceNumber();
#endif

  printf("void main(void): Vor Beginn der !QuitProgram - Schreife....\n");

  while( !QuitProgram ) {
		
    /* InitNewGame */
    InitNewGame();
	
    GameOver = FALSE;
		
    printf("void main(void): Vor Beginn der !GameOver && !QuitProgram - Schleife....\n");

    while (!GameOver && !QuitProgram) {

      framenr++; // This ensures, that 0 is never an encountered framenr, therefore count to 100 here
      // take the time now for calculating the frame rate (DO NOT MOVE THIS COMMAND PLEASE!)
      gettimeofday(&oneframetimestamp,NULL);
      if (  framenr %  10 == 1) gettimeofday(&tenframetimestamp,NULL); 
      if (  framenr % 100 == 1) gettimeofday(&onehundredframetimestamp,NULL);

      UpdateCountersForThisFrame();

	
      printf("void main(void): Innerhalb der !GameOver && !QuitProgram - Schleife....\n");
      keyboard_update();
      DisplayRahmen(RealScreen);

      // usleep(20000);  //  This should cause a little delay to make the game more playable

      if(keyboard_keypressed(SCANCODE_Q)) {
	printf("\n*****************************************************");
	printf("\nvoid main(void): Termination cause of Q-Pressing!!!!!");
	printf("\n*****************************************************\n\n");
	Terminate(0);
      }
      if(keyboard_keypressed(SCANCODE_D)) Me.energy=0; 
      if(keyboard_keypressed(SCANCODE_L)) ShowHighscoreList();
      if(keyboard_keypressed(SCANCODE_I)) ShowDebugInfos();
      if(keyboard_keypressed(SCANCODE_V)) HideInvisibleMap = !HideInvisibleMap; 
      if(keyboard_keypressed(SCANCODE_C)) Cheatmenu();
      if(PPressed()) Pause();

      if( ShipEmptyCounter == 1) GameOver = TRUE;

      LastBlastHit++;

      MoveLevelDoors();       /* Die Tueren im Level auf und zu bewegen */
      AnimateRefresh();       /* Refreshes Animieren */

      for (i=0;i<NumEnemys;i++) {
	if (Feindesliste[i].warten > 0) Feindesliste[i].warten--;
	if (Feindesliste[i].firewait > 0) Feindesliste[i].firewait--;
      }

      MoveBullets();      /* Bullets entsprechend ihrer Geschwindigkeit weiterbewegen */
      ExplodeBlasts();      /* Blasts in der Explosionsphase weiterbewegen */

      GetView();      /* Einen Ausschnitt aus der Gesamtlevelkarte machen */

      GetInternFenster();
      PutInternFenster();

#ifdef PARADROID_SOUND_SUPPORT_ON
      YIFF_Server_Check_Events();
#endif

      UpdateInfoline();
      for (i=0;i<MAXBULLETS;i++) CheckBulletCollisions(i);
      PutMessages();

      JoystickControl(); // Wenn vorhanden: Joystick einlesen 
      MoveInfluence();   // Gemaess den gedrueckten Tasten die Geschwindigkeit veraendern 
      MoveEnemys();	 // Auch die Feinde bewegen 
      AnimateInfluence();  // Bei animierten Influencer die Phasen weiterzaehlen 
      AnimateEnemys();   // Bei den Feinden auch Phasen weiterzaehlen 

      /* Raeder bremsen die Fahrt des Influencers erheblich */
      printf("\nvoid main(void): SpeedX ist jetzt: %f!",SpeedX);
      printf("\nvoid main(void): SpeedY ist jetzt: %f!",SpeedY);
      Reibung();

      /* Influencedruid nach der momentanen Geschwindigkeit weiterbewegen */
      printf("\nvoid main(void): SpeedX: %f!",SpeedX);
      gl_printf(1,40,"\nvoid main(void): SpeedY: %f!",SpeedY);
      Me.pos.x+=SpeedX * Frame_Time();
      Me.pos.y+=SpeedY * Frame_Time();
      AdjustSpeed();

      BounceInfluencer();       /* Testen ob der Weg nicht durch Mauern verstellt ist */
      InfluenceEnemyCollision();
      RotateBulletColor();

      if( CurLevel->empty == 2) {
	LevelGrauFaerben();
	CurLevel->empty = TRUE;
      } /* if */

      // calculate the framerate:
      gettimeofday(&now,NULL);

      oneframedelay=(now.tv_usec-oneframetimestamp.tv_usec)+(now.tv_sec-oneframetimestamp.tv_sec)*1000000; 
      if (framenr % 10 == 0) 
	tenframedelay=((now.tv_usec-tenframetimestamp.tv_usec))+(now.tv_sec-tenframetimestamp.tv_sec)*1000000;
      if ((framenr % 100) == 0) {
	onehundredframedelay=(now.tv_sec-onehundredframetimestamp.tv_sec)*1000000+
	  (now.tv_usec-onehundredframetimestamp.tv_usec);
	framenr=0;
      }

      FPSover1  =1000000*  1 / (float)oneframedelay;
      FPSover10 =1000000* 10 / (float)tenframedelay;
      FPSover100=1000000*100 / (float)onehundredframedelay;
      // gl_printf(1,30,"   1fr: %d ms FPS1: %f \n",oneframedelay,FPSover1);
      // gl_printf(-1,-1," 10fr: %d ms FPS10: %f \n",tenframedelay,FPSover10);
      gl_printf(1,35,"100fr: %d ms FPS100: %f \n",onehundredframedelay,FPSover100);
      gl_printf(-1,-1,"Frame_Time(): %f \n",Frame_Time());
      // gl_printf(-1,-1,"sec : %d usec : %d \n",now.tv_sec,now.tv_usec);
      // gl_printf(-1,-1,"sec : %d usec : %d \n",onehundredframetimestamp.tv_sec,onehundredframetimestamp.tv_usec);
      // gl_printf(-1,-1,"sec : %d usec : %d \n",now.tv_sec-onehundredframetimestamp.tv_sec,now.tv_usec-onehundredframetimestamp.tv_usec);
      //      gl_printf(-1,-1,"sec : %d \n",onehundredframedelay);
      // gl_printf(-1,-1,"sec : %d \n",framenr % 100);

      // gl_printf(-1,-1,"%f\n",oneframetimestamp);
      //      gl_printf(-1,-1,ltoa((long)onehundredframedelay,Dummystring,10));
    } /* while !GameOver */
  } /* while !QuitProgram */
  Terminate(0);
  return (0);
} // void main(void)

/*@Function============================================================
@Desc: Diese Funktion Sprengt den Influencer und beendet das Programm

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ThouArtDefeated(void){
  int i,j;

  printf("\nvoid ThouArtDefeated(void): Real function call confirmed.");
  Me.status=TERMINATED;
  ExplodeInfluencer();
  
  for (i=0;i<WAIT_AFTER_KILLED; i++) {
    //		UpdateInfoline();
    //		SetInfoline();
    DisplayRahmen(RealScreen);
    GetInternFenster();
    PutInternFenster();
    ExplodeBlasts();
    MoveBullets();
    MoveEnemys();
    for(j=0;j<MAXBULLETS;j++) CheckBulletCollisions(j);
    RotateBulletColor();
    usleep(10000);
  }

  /* Ein Flimmer zieht "uber den Schirm */
  Flimmern();
  Debriefing();

  /* Soundblaster soll keine Toene mehr spucken */
  //PORT sbfm_silence();

  GameOver = TRUE;
  
  printf("\nvoid ThouArtDefeated(void): Usual end of function reached.");
} // void ThouArtDefeated(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ThouArtVictorious(void){
  ShipEmptyCounter = WAIT_SHIPEMPTY;
  GameOver = TRUE;				  /*  */
	
  KillTastaturPuffer();
  ClearUserFenster();
  printf(" BRAVO ! Sie haben es geschafft ! ");
  getchar();
}

/* **********************************************************************
   Diese Funktion updated die Highscoreliste
**********************************************************************/
void Debriefing(void){
  char* Scoretext;
  HallElement* Oldptr;
  HallElement* Newptr;
  HallElement* SaveHallptr=Hallptr;
  int DebriefColor;

  printf("\nvoid Debriefing(void): Real function call confirmed.");

  DebriefColor=FONT_WHITE;

  Me.status=DEBRIEFING;
  if (!PlusExtentionsOn) {
    Scoretext=MyMalloc(1000);
    if (RealScore>GreatScore) {
      SetUserfenster(DebriefColor,RealScreen); // KON_BG_COLOR
      SetTextColor(DebriefColor, KON_TEXT_COLOR); // KON_BG_COLOR
      strcpy(Scoretext,"\n    Great Score !\n Enter your name:");
      DisplayText(Scoretext,USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
      free(GreatScoreName);
      GreatScoreName=GetString(18);
      GreatScore=RealScore;
    } else
      if (RealScore<LowestScoreOfDay) {
	SetUserfenster(DebriefColor,RealScreen); // KON_BG_COLOR
	SetTextColor(DebriefColor, KON_TEXT_COLOR); // KON_BG_COLOR
	strcpy(Scoretext,"\n   Lowest Score of Day! \n Enter your name:");
	DisplayText(Scoretext,USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
	free(LowestName);
	LowestName=GetString(18);
	LowestScoreOfDay=RealScore;
      } else 
	if (RealScore>HighestScoreOfDay) {
	  SetUserfenster(DebriefColor,RealScreen); // KON_BG_COLOR
	  SetTextColor(DebriefColor, KON_TEXT_COLOR); // KON_BG_COLOR
	  strcpy(Scoretext,"\n   Highest Score of Day! \n Enter your name:");
	  DisplayText(Scoretext,USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
	  free(HighestName);
	  HighestName=GetString(18);
	  HighestScoreOfDay=RealScore;
	}
    free(Scoretext);
    
  } else {
    SaveHallptr=Hallptr;
    
    /* Wir brauchen keine Versager ! */
    if (RealScore == 0) return;
    /* Setzten der Umgebung */
    SetUserfenster(KON_BG_COLOR, RealScreen);
    SetTextColor(KON_BG_COLOR, KON_TEXT_COLOR);
    DisplayText(" You have gained entry to the hall\n of fame!\nEnter your name:\n  ",
		USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
    
    /* Den neuen Eintrag in die Liste integrieren */
    if (Hallptr->PlayerScore < RealScore) {
      Oldptr=Hallptr;
      Hallptr=MyMalloc(sizeof(HallElement)+1);
      Hallptr->PlayerScore=RealScore;
      Hallptr->PlayerName=GetString(18);
      Hallptr->NextPlayer=Oldptr;
      SaveHallptr=Hallptr;
    } else {
      Oldptr=Hallptr;
      while (Hallptr->PlayerScore > RealScore) {
	Hallptr=Hallptr->NextPlayer;
	if (Hallptr->PlayerScore > RealScore) Oldptr=Oldptr->NextPlayer;
      }
      Newptr=MyMalloc(sizeof(HallElement)+1);
      Newptr->PlayerScore=RealScore;
      Newptr->PlayerName=GetString(18);
      Newptr->NextPlayer=Hallptr;
      Oldptr->NextPlayer=Newptr;
    }
    
    /* Message an exit */
    DisplayText("You are now added to the hall\n of fame!\n",
		USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
    Hallptr=SaveHallptr;
    getchar();
  }

  printf("\nvoid Debriefing(void): Usual end of function reached.");
}  // void Debriefing(void)

// This function does the Pause-Mode, which means, that the game process is halted,
// while the graphics and animations are not.  The "Mode" can be set to CHEESE, which is
// a feature from the original program that should allow for better screenshots and
// of course allow the player to take a rest.
//
// Status: functioning perfectly
//

void Pause(void){
  Me.status=PAUSE;
  SetInfoline();
  UpdateInfoline();
  ClearKbState();
  while (!PPressed() ) {
    usleep(30000);
    JoystickControl();
    //PORT: worfuer war das??  
    //while (!TimerFlag) JoystickControl();
    AnimateInfluence();
    AnimateRefresh();
    RotateBulletColor();
    AnimateEnemys();
    GetView();
    GetInternFenster();
    PutInternFenster();
    //PORT	    if (kbhit()) taste=getch();
    if (CPressed()) {
      JoystickControl();
      Me.status=CHEESE;
      SetInfoline();
      UpdateInfoline();
      while (!SpacePressed()) {
	JoystickControl();
	keyboard_update();
      }
      Me.status=PAUSE;
      SetInfoline();
      UpdateInfoline();
      while (SpacePressed()) {
	JoystickControl();
	keyboard_update();
      }
      taste = 1;
    }
  }
  ClearKbState();
} // void Pause(void)

/* **********************************************************************
	Diese Funktion gibt die momentane Highscoreliste aus
	Derweil ist sie noch im Textmodus.
	Wenn sie fertig ist, soll sie die Liste in Paraplusart nach oben
	scrollen.
	**********************************************************************/
void ShowHighscoreList(void){
  int Rankcounter=0;
  HallElement* SaveHallptr=Hallptr;

  vga_clear();
  keyboard_close();  // for the moment we don't bother, just use normal kb mode
  if (!PlusExtentionsOn) {
    gl_printf(20,5," Dies ist die aktuelle Highsoreliste:\n\n");
    gl_printf(-1,-1," Highest Score: %10s : %4d\n",HighestName, HighestScoreOfDay);
    gl_printf(-1,-1," Ok Score:      %10s : %4d\n",GreatScoreName, GreatScore);
    gl_printf(-1,-1," Lowest Score:  %10s : %4d\n",LowestName, LowestScoreOfDay);
    getchar();
  } else {
    gl_printf(-1,-1," This is today's Hall of Fame:\n\n");
    gl_printf(-1,-1,"\tRank\tName\tScore\n");
    while (Hallptr->NextPlayer != NULL) {
      gl_printf(-1,-1,"\t%d\t",Rankcounter);
      gl_printf(-1,-1,"%s",Hallptr->PlayerName);
      gl_printf(-1,-1,"\t%d\n",Hallptr->PlayerScore);
      Hallptr=Hallptr->NextPlayer;
      Rankcounter++;
    }
    getchar();
  }
  Hallptr=SaveHallptr;

  vga_clear();
  // return to raw kb mode
  keyboard_init();
  
} // void ShowHighscoreList(void)

void UpdateCountersForThisFrame(void){
  // Here are some things, that were previously done by some periodic interrupt function
  ThisMessageTime++;
  if (Me.firewait > 0) Me.firewait--;
  if ( ShipEmptyCounter > 1 ) ShipEmptyCounter --;
  if ( WaitElevatorCounter > 0) WaitElevatorCounter --;
  if ( CurLevel->empty > 2) CurLevel->empty--;
  if(RealScore > ShowScore) ShowScore++;
  if(RealScore < ShowScore) ShowScore--;
  if (InterruptInfolineUpdate) {
    UpdateInfoline();
    SetInfoline();
  }
  // This is the end of the things, that were previously done by periodic interrupt
} // void UpdateCountersForThisFrame(void)


#undef _paraplus_c
