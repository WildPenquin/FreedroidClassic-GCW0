/*=@Header==============================================================
 * $Source$
 *
 * @Desc: enemy related functions 
 *
 * $Revision$
 *
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.1  1993/07/29 17:28:02  prix
 * Initial revision
 *
 * Revision 1.3  1993/10/25  18:18:57  prix
 * Wed Aug 11 20:33:40 1993: New EnemyEnmeyCollision handling
 * Sat Aug 14 09:23:30 1993: enemys warten nicht
 * Sat Aug 14 09:31:18 1993: firewait doesnt depend on guntype but on druid !!
 * Sat Aug 14 16:05:19 1993: Neues Tuning der Feind-aggressivitaet
 * Tue Aug 24 10:31:19 1993: better shuffleEnemys
 * Tue Aug 24 16:46:41 1993: written InitEnemys()
 * Sun Aug 29 10:49:57 1993: enemys shoot only if visible
 * Fri Sep 17 11:47:56 1993: waypoint - Suche beschleunigt
 * Thu Sep 30 14:14:28 1993: written ClassOfDruid()
 *
 * Revision 1.2  1993/08/12  00:25:22  prix
 * Fri Jul 30 07:30:17 1993: Enemys schiessen nicht immer wenn sie koennen
 * Sat Jul 31 11:08:07 1993: renamed WAYPOINTMAX to MAXWAYPOINTS
 * Sat Jul 31 12:39:30 1993: type instead of typennummer in enemy - struct
 * Sat Jul 31 12:44:21 1993: no abgschossen in enemy but Status
 * Sat Jul 31 18:08:36 1993: agression is type-specific, not individual for each enemy !
 * Sat Jul 31 19:55:24 1993: only work on enemys that are alive and on current level
 * Sun Aug 08 21:33:15 1993: changed MoveEnemys() to new Waypoint-management
 * Mon Aug 09 17:18:36 1993: written ShuffleEnemys()
 * Mon Aug 09 18:17:24 1993: written CheckEnemeyCollsion()
 * Mon Aug 09 18:25:46 1993: renamed CheckEnemyCollsion to EnemyEnemyCollision()
 * Mon Aug 09 18:58:38 1993: emergency: total collsion: trennt enemys
 * Tue Aug 10 19:49:21 1993: enemys shoot only if dist is ok
 * Tue Aug 10 20:07:55 1993: kein EnemyGetroffen mehr
 * Tue Aug 10 21:04:24 1993: written AttackInfluence
 *
 * Revision 1.1  1993/07/29  17:28:02  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

static const char RCSid[]=\
"$Id$";

#define _enemy_c

#define NOSTRAIGHTDIR 255
#undef ENEMYPHASEDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <dos.h>
#include <math.h>
#include <time.h>
#include <conio.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"


#define ENEMYMAXWAIT 2			/* seconds: wait after reached waypoint */

#define COL_SPEED		3			/* wegstossen bei enemy-enemy collision */

/* Distance, where to attack Influencer */
// #define FIREDIST2				INTERNBREITE*INTERNHOEHE*BLOCKBREITE*BLOCKHOEHE/4
#define FIREDIST2	(INTERNBREITE*BLOCKBREITE/2)*(INTERNBREITE*BLOCKBREITE/2)+(INTERNHOEHE*BLOCKHOEHE/2)*(INTERNHOEHE*BLOCKHOEHE/2)

void PermanentHealRobots(void);

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void PermanentHealRobots(void){
	static int TimerCounter=2*18;
	int i;

	if ( --TimerCounter) return;
	if (TimerCounter == 0) TimerCounter=2*18;

	for (i=0;i<MAX_ENEMYS_ON_SHIP;i++){
		if (Feindesliste[i].Status == OUT) continue;
		if (Feindesliste[i].energy < Druidmap[Feindesliste[i].type].maxenergy)
			Feindesliste[i].energy += Druidmap[Feindesliste[i].type].lose_health;
	}
}

/*@Function============================================================
@Desc: InitEnenmys(): initialisiert Feindesliste vollstaendig

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void InitEnemys(void)
{
	int i;
	int type;

	for(i=0; i<NumEnemys; i++) {
		type = Feindesliste[i].type;
		Feindesliste[i].energy = Druidmap[type].maxenergy;
	}

	/* und gut umruehren */
	ShuffleEnemys();
	
	return;
} /* InitEnemys */

/*@Function============================================================
@Desc: ClearEnemys(): setzt Feindesliste - Array auf 000...

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void ClearEnemys(void)
{
	int i;
	
	for ( i = 0; i < MAX_ENEMYS_ON_SHIP; i++ ) {
		Feindesliste[i].type = -1;
		Feindesliste[i].levelnum = Feindesliste[i].energy = 0;
		Feindesliste[i].feindphase = Feindesliste[i].feindrehcode = 0;
		Feindesliste[i].nextwaypoint = Feindesliste[i].lastwaypoint = 0;
		Feindesliste[i].Status = OUT;
		Feindesliste[i].warten = Feindesliste[i].firewait = 0;
	}
}


/*@Function============================================================
@Desc: ShuffleEnemys(): Vermischt enemys in CurLevel auf die Waypoints
		
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void ShuffleEnemys(void)
{
	int curlevel = CurLevel->levelnum;
	int i;
	int nth_enemy;
	int wp_num;
	int wp;
	int influ_x, influ_y;
	
	/* Anzahl der Waypoints auf CurLevel abzaehlen */
	wp_num = 0;
	while( CurLevel->AllWaypoints[wp_num++].x != 0);
	wp_num --;		/* einer zuviel */

	nth_enemy = 0;
	for(i=0; i<NumEnemys; i++) {
		if( Feindesliste[i].Status == OUT || Feindesliste[i].levelnum != curlevel)
			continue;		/* dont handle dead enemys or on other level */

		nth_enemy ++;
		if( nth_enemy < wp_num ) wp = nth_enemy;
		else {
			gotoxy(1,1);
			printf("\nWeniger waypoints als \n Gegner hier !!");
			getch();
			return;
		}
			
		Feindesliste[i].pos.x =
			(CurLevel->AllWaypoints[wp].x)*BLOCKBREITE+BLOCKBREITE/2;
			
		Feindesliste[i].pos.y =
			(CurLevel->AllWaypoints[wp].y)*BLOCKHOEHE+BLOCKHOEHE/2;

		Feindesliste[i].lastwaypoint = wp;
		Feindesliste[i].nextwaypoint = wp;
		 
	} /* for */

	/* enemys ein bisschen sich selbst ueberlassen */
	
	/* Influencer zuerst entfernen */
	influ_x = Me.pos.x;
	influ_y = Me.pos.y;
	Me.pos.x = Me.pos.y = 0;
	
	for( i=0; i<30; i++ ) MoveEnemys();

	/* influencer wieder her */
	Me.pos.x = influ_x;
	Me.pos.y = influ_y;
	
} /* ShuffleEnemys() */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void MoveEnemys(void){
	int i;
	int j;
	point Restweg;
	Waypoint WpList;	/* Pointer to waypoint-liste */
	int nextwp;
	point nextwp_pos;
	int trywp;
	int PossibleConnections;
	int swap;

	if (BeamLine) return;
	
	PermanentHealRobots();
	
	for (i=0;i<NumEnemys;i++) {
		
		if( Feindesliste[i].nextwaypoint == 100 ) continue;
		
		/* ignore robots on other levels */
		if (Feindesliste[i].levelnum != CurLevel->levelnum) continue;
		
		/* Wenn der robot tot ist dann weiter */
		if (Feindesliste[i].Status == OUT) continue;

		/* ist der Robot gerade toedlich getroffen worden ?? */
		if( Feindesliste[i].energy <= 0) {
			Feindesliste[i].Status = OUT;
			RealScore+=Druidmap[Feindesliste[i].type].score;
			StartBlast(Feindesliste[i].pos.x, Feindesliste[i].pos.y, DRUIDBLAST);
			if( LevelEmpty() ) CurLevel->empty = WAIT_LEVELEMPTY;
			continue; /* naechster Enemy, der ist hin */
		}

		if( Druidmap[Feindesliste[i].type].aggression ) AttackInfluence(i);
		
		/* Wenn der Robot noch zu warten hat dann gleich weiter */
		if (Feindesliste[i].warten > 0) continue;

		/* collision mit anderem Druid */
		EnemyEnemyCollision(i);
				
		/* Ermittlung des Restweges zum naechsten Ziel */
		WpList = CurLevel->AllWaypoints;
		nextwp = Feindesliste[i].nextwaypoint;
		nextwp_pos.x = WpList[nextwp].x*BLOCKBREITE+BLOCKBREITE/2;
		nextwp_pos.y = WpList[nextwp].y*BLOCKHOEHE+BLOCKHOEHE/2;
	
		Restweg.x=nextwp_pos.x - Feindesliste[i].pos.x;
		Restweg.y=nextwp_pos.y -Feindesliste[i].pos.y;


		/* Bewegung wenn der Abstand noch groesser als maxspeed ist */
		if (abs(Restweg.x) >= Druidmap[Feindesliste[i].type].maxspeed) {
			Feindesliste[i].speed.x =
				(Restweg.x/abs(Restweg.x))*Druidmap[Feindesliste[i].type].maxspeed; 
			Feindesliste[i].pos.x += Feindesliste[i].speed.x;
		}

		if (abs(Restweg.y) >= Druidmap[Feindesliste[i].type].maxspeed) {
			Feindesliste[i].speed.y =
				(Restweg.y/abs(Restweg.y))*Druidmap[Feindesliste[i].type].maxspeed;
			Feindesliste[i].pos.y += Feindesliste[i].speed.y;
		}

		/* Endannaeherung aktuellen waypoint und anvisieren des naechsten */
		if ( abs(Restweg.x) < Druidmap[Feindesliste[i].type].maxspeed) {
	  		Feindesliste[i].pos.x = nextwp_pos.x;
  			Feindesliste[i].speed.x = 0;
	  	}
		  		
		if ( abs(Restweg.y) < Druidmap[Feindesliste[i].type].maxspeed ) {
  			Feindesliste[i].pos.y = nextwp_pos.y;
  			Feindesliste[i].speed.y = 0;
	  	}
					
		if ( (Restweg.x == 0) && (Restweg.y == 0) ) {
			Feindesliste[i].lastwaypoint = Feindesliste[i].nextwaypoint;
			Feindesliste[i].warten = random(ENEMYMAXWAIT)*18;
		
		/* suche moegliche Verbindung von hier */
		PossibleConnections = -1;
		while((PossibleConnections < MAX_WP_CONNECTIONS) && 
			(WpList[nextwp].connections[++PossibleConnections] != -1) );

			if( PossibleConnections > 0) {
				do {
					trywp = (WpList[nextwp]).connections[random(PossibleConnections)];
				} while( trywp == -1 );
			
				/* setze neuen Waypoint */
				Feindesliste[i].nextwaypoint = trywp;
			} /* if */
		} /* if */

	} /* for */
	
} /* MoveEnemys */

/*@Function============================================================
@Desc: AttackInfluence(): enemynum schiesst unter gegebenen Umstaenden auf
								Influencer

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void AttackInfluence(int enemynum)
{
	int j;
	int guntype;
	int xdist, ydist;
	long dist2;
	int dummy;
	
	/* Ermittlung des Abstandsvektors zum Influencer */
	xdist = Me.pos.x-Feindesliste[enemynum].pos.x;
	ydist =Me.pos.y-Feindesliste[enemynum].pos.y;
	
	/* Sicherheit gegen Division durch 0 */
	if (abs(xdist) < 2) xdist = 2;
	if (abs(ydist) < 2) ydist = 2;

	/* wenn die Vorzeichen gut sind einen Schuss auf den 001 abgeben */
	guntype = Druidmap[Feindesliste[enemynum].type].gun;

	dist2 = (xdist*xdist+ydist*ydist);
	if (  (dist2 < (long)FIREDIST2) &&
		( !Feindesliste[enemynum].firewait ) &&
		IsVisible( &Feindesliste[enemynum].pos) )
	{
 		if( random(AGGRESSIONMAX) >= Druidmap[Feindesliste[enemynum].type].aggression) {
 			/* Diesmal nicht schiessen */
			Feindesliste[enemynum].firewait =
				random(Druidmap[Feindesliste[enemynum].type].firewait)*18;
			return;
		}
		
		FireBulletSound();
			
		/* Einen bulleteintragg suchen, der noch nicht belegt ist */
		for (j=0;j<MAXBULLETS;j++) {
			if (AllBullets[j].type == OUT) break; 
		}

		/* Schussrichtung festlegen */
		if (abs(xdist) > abs(ydist) ) {
			AllBullets[j].SX=Bulletmap[guntype].speed;
			AllBullets[j].SY=ydist*AllBullets[j].SX/xdist;
			if (xdist < 0) {
				AllBullets[j].SX=-AllBullets[j].SX;
				AllBullets[j].SY=-AllBullets[j].SY;
			}
		} 
  		
		if (abs(xdist) < abs(ydist) ) {
			AllBullets[j].SY=Bulletmap[guntype].speed;
			AllBullets[j].SX=xdist*AllBullets[j].SY/ydist;
			if (ydist < 0) {
				AllBullets[j].SX=-AllBullets[j].SX;
				AllBullets[j].SY=-AllBullets[j].SY;
			}
		}
		
		/* Schussphase festlegen ( ->phase=Schussbild ) */
		AllBullets[j].phase=NOSTRAIGHTDIR;
		if ((abs(xdist)*2/3)/abs(ydist)) AllBullets[j].phase=RECHTS;
		if ((abs(ydist)*2/3)/abs(xdist)) AllBullets[j].phase=OBEN;
		if (AllBullets[j].phase == NOSTRAIGHTDIR) {
			if (((xdist < 0) && (ydist < 0)) || ((xdist > 0) && (ydist > 0)))
				AllBullets[j].phase=RECHTSUNTEN;
				else AllBullets[j].phase=RECHTSOBEN;
		}

		/* Bullets im Zentrum des Schuetzen starten */
		AllBullets[j].PX=Feindesliste[enemynum].pos.x;
		AllBullets[j].PY=Feindesliste[enemynum].pos.y;
		
		/* Bullets so abfeuern, dass sie nicht den Schuetzen treffen */
		AllBullets[j].PX+=AllBullets[j].SX;
		AllBullets[j].PY+=AllBullets[j].SY;
		AllBullets[j].PX+=Feindesliste[enemynum].speed.x;
		AllBullets[j].PY+=Feindesliste[enemynum].speed.y;
			
		/* Dem Bullettype entsprechend lange warten vor naechstem Schuss */

		Feindesliste[enemynum].firewait=
			random(Druidmap[Feindesliste[enemynum].type].firewait)*18+4;
					
		/* Bullettype gemaes dem ueblichen guntype fuer den robottyp setzen */
		AllBullets[j].type=guntype;
			
	} /* if */
		

} /* AttackInfluence */

/*@Function============================================================
@Desc: EnemyEnemyCollision()

@Ret: 	TRUE: Enemynum kollidierte mit anderem Enemy
@Int:
* $Function----------------------------------------------------------*/
int EnemyEnemyCollision(int enemynum)
{
	int i;
	int curlev = CurLevel->levelnum;
	int check_x, check_y;
	int swap;
	long xdist, ydist;
	long dist2;
	long crit_dist2 = 4*DRUIDRADIUSX*DRUIDRADIUSX;
	int speed_x, speed_y;
	
	check_x = Feindesliste[enemynum].pos.x;
	check_y = Feindesliste[enemynum].pos.y;
	
	for(i=0; i<NumEnemys; i++) {
		/* only living enemys on this level */
		if( Feindesliste[i].Status == OUT || Feindesliste[i].levelnum != curlev)
			continue;
		/* dont check yourself */
		if( i == enemynum ) continue;

		/* get distance between enemy i and enemynum */
		xdist = check_x - Feindesliste[i].pos.x;
		ydist = check_y - Feindesliste[i].pos.y;

		dist2 = xdist*xdist+ydist*ydist;

		/* Kollision ?? */
		if( dist2 <= crit_dist2 ) {

			/* Warte ich ??: */	
			if( Feindesliste[enemynum].warten ) {
				/* weiter warten */
				Feindesliste[enemynum].warten = WAIT_COLLISION;
				continue;
			}
		
			/* Sonst: Feind stoppen und selbst umdrehen */
			Feindesliste[i].warten = WAIT_COLLISION;
			
			/* gestoppten gegner ein wenig zurueckstossen */
			if( xdist ) Feindesliste[i].pos.x -= xdist/abs((int)xdist);
			if( ydist ) Feindesliste[i].pos.y -= ydist/abs((int)ydist);
			
			swap = Feindesliste[enemynum].nextwaypoint;
			Feindesliste[enemynum].nextwaypoint = Feindesliste[enemynum].lastwaypoint;
			Feindesliste[enemynum].lastwaypoint = swap;
			
			/* Etwas aus Gegner herausbewegen !! */
			speed_x = Feindesliste[enemynum].speed.x;
			speed_y = Feindesliste[enemynum].speed.y;

			if( speed_x )
				Feindesliste[enemynum].pos.x -= COL_SPEED*(speed_x)/abs(speed_x);
			if( speed_y )
				Feindesliste[enemynum].pos.y -= COL_SPEED*(speed_y)/abs(speed_y);
			
			return TRUE;
			
		} /* if dist zu klein */

	} /* for */

	return FALSE;
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void AnimateEnemys(void) {
	int i;
	gotoxy(1,1);
	for (i=0;i<NumEnemys;i++) {
		if (Feindesliste[i].type == DRUID598) {
	//			printf(" \n Feindrehcode : %d \n maxenergy: %d \n nowenergy: %d \n phase: %d ! ",
	//			Feindesliste[i].feindrehcode,
	//			Druidmap[Feindesliste[i].type].maxenergy,
	//			Feindesliste[i].energy,
	//			Feindesliste[i].feindphase);
		}
			
		/* ignore enemys that are dead or on other levels or dummys */
		if( Feindesliste[i].type == DEBUG_ENEMY) continue;
		if ( Feindesliste[i].levelnum != CurLevel->levelnum) continue;
		if ( Feindesliste[i].Status == OUT ) continue;
		
		Feindesliste[i].feindrehcode+=Feindesliste[i].energy;
		Feindesliste[i].feindphase=Feindesliste[i].feindrehcode/Druidmap[Feindesliste[i].type].maxenergy;
			
		if (Feindesliste[i].feindphase>=ENEMYPHASES) {
#ifdef ENEMYPHASEDEBUG			
			if (Feindesliste[i].type == DRUID598) {
				printf(" Broke at: %d ",Feindesliste[i].feindphase);
				getch();
			}
#endif			
			Feindesliste[i].feindphase=0;
			Feindesliste[i].feindrehcode=0;
		}
	}
	
}

/*@Function============================================================
@Desc: ClassOfDruid(druidtype): liefert die Classe des Druidtypes type

@Ret: int class: 0-9
@Int:
* $Function----------------------------------------------------------*/
int ClassOfDruid(int druidtype)
{
	char classnumber[2];

	classnumber[0] = Druidmap[druidtype].druidname[0];	/* first digit is class */
	classnumber[1] = '\0';

	return( atoi(classnumber) );
} /* ClassOfDruid */



#undef _enemy_c
