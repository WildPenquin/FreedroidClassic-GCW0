/* 
 *
 *   Copyright (c) 2002 Johannes Prix
 *   Copyright (c) 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeParadroid+
 *
 *  FreeParadroid+ is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeParadroid+ is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeParadroid+; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _struct_h
#define _struct_h

#ifndef _BYTE
  typedef unsigned char byte;
#define _BYTE
#endif
#ifndef _WORD
  typedef unsigned word;
#define _WORD
#endif
#ifndef _BOOL
  typedef char bool;
#define _BOOL
#endif

typedef struct {
  char signature;
  char version;
  char encoding;
  char bytes_per_pixel;
  unsigned short int xmin;
  unsigned short int ymin;
  unsigned short int xmax;
  unsigned short int ymax;
  unsigned short int vres;
  unsigned short int hres;
  char palette[48];
  char reserved;
  char color_layers;
  unsigned short int bytes_per_line;
  unsigned short int palette_type;
  char unused[58];
} PCX_Header;

typedef struct {
  long PlayerScore;
  void* NextPlayer;
  char* PlayerName;
} HallElement;

typedef struct {
  unsigned char rot;
  unsigned char gruen;
  unsigned char blau;
} color, *Color;

typedef struct {
  signed short x;
  signed short y;
} point, *Point;

typedef struct {
  float x;
  float y;
} finepoint, *Finepoint;

typedef struct {
   signed char x;
   signed char y;
} grob_point, *grob_Point;


typedef grob_point vect;
typedef grob_Point Vect;


typedef struct {
  const char *druidname;
  float maxspeed;    /* the maximum of speed it can go */
  int class;
  byte accel;       /* its acceleration */
  float maxenergy;    /* the maximum energy the batteries can carry */
  byte lose_health; /* the energy/time the duid loses under influence-control */
  byte gun;         /* Which gun does this druid use */
  byte vneutral;		/* Is there a velocityneutralisator for Bullets ? */
  byte aggression;	/* The aggressiveness of this druidtype */
  byte firewait;		/* Wait approx. after shoot, in 1/4 sec. */
  byte flashimmune;	/* is the droid immune to FLASH-bullets */
  int score;			/* Punkte f"ur die Vernichtung dieses Modells */
  char *notes;		/* notes on the druid of this type */
} druidspec, *Druidspec;

typedef struct {
  int type;         /* what kind of druid is this ? */
  byte status;       /* attacking, defense, dead, ... */
  finepoint speed;       /* the current speed of the druid */
  finepoint pos;        /* current position in level levelnum */
  float health;		/* the max. possible energy in the moment */
  float energy;       /* current energy */
  byte firewait;	/* counter after fire */
  float phase;        /* the current phase of animation */
  int autofire;		/* Status of the Firecontrolautomatics */
  int vneut;		/* Status of Velocityneutralizer for the gun */
  int MyFCU;		/* FCU (Fire Control Unit) installed */
  int MyShield;		/* Shield device installed */
  int Shield[4];	/* Status of Partial Shields */
} influence_t, *Influence_t;

typedef struct {
  byte type;		/* gibt die Nummer in Druidmap an */
  int levelnum;		/* Level in dem sich enemy befindet */
  finepoint pos;	/* gibt die Koordinaten der Momentanposition an */
  finepoint speed;	/* current speed  */
  float energy;		/* gibt die Energie dieses Robots an */
  float feindphase;	/* gibt die Phase an in der der Feind gedreht ist */
  byte nextwaypoint;	/* gibt den naechsten Zielpunkt an */
  byte lastwaypoint;	/* Waypoint, von dem ausgegangen wurde */
  byte Status;		/* gibt z.B. an ob der Robotter abgeschossen wurde */
  byte warten;		/* gibt Wartezeit an bis Fahrt wieder aufgenommen wird */
  byte passable;	/* Zeit (counter), in der druid passable ist */
  byte firewait;	/* gibt die Zeit bis zum naechsten Schuss an */
  byte onscreen;	/* gibt an ob der Robot im moment sichtbar ist */
  int Shield[4];	// Status of Partial shields
} enemy, *Enemy;

typedef struct {
   int  speed;        /* speed of the bullet */
   int  damage;					/* damage done by this bullettype */
   int  time;              /* how long does bullet exist */
   byte phases;             /* how many phases in motion to show */
   byte blast;              /* which blast does this bullet create */
   unsigned char *picpointer; /* pointer to picture of bullet */
   signed char oneshotonly;	/* if this is set, there is never more than 1 shot from this Influweapon */
   int WaitNextTime;
} bulletspec, *Bulletspec;

typedef struct {
  finepoint pos;
  finepoint speed;
  byte type;
  byte phase;
  byte time;
  signed char mine;
  int owner;
} bullet, *Bullet;

typedef struct {
   byte phases;
   unsigned char *picpointer;
} blastspec, *Blastspec;

typedef struct {
   int PX;  /* PosX */
   int PY;  /* PosY */
   byte type;
   float phase;
} blast, *Blast;

typedef struct {
  byte level;	/* Level - 'Koordinate' */
  int x;	/* x,y Koordinaten */
  int y;
} location, *Location;

typedef struct {
  /* current location */
  byte level;
  byte x;		/* Grob */
  byte y;
	
  /* connections: Numbers in Elevator-Array */
  signed char up;
  signed char down;
  
  /* row */
  byte elevator_row; /* number of elev. column */
} elevator, *Elevator;

typedef struct {
  byte x;		/* Grob */
  byte y;
  signed char connections[MAX_WP_CONNECTIONS];
} waypoint, *Waypoint;

typedef struct {
  byte empty;
  unsigned int levelnum;  /* Number of this level */
  char *Levelname;	  /* Name of this level */
  unsigned int xlen;      /* X dimension */
  unsigned int ylen;
  unsigned int color;
  char *map[MAXMAPLINES]; /* this is a vector of pointers ! */
  grob_point refreshes[MAX_REFRESHES_ON_LEVEL];
  grob_point doors[MAX_DOORS_ON_LEVEL];
  waypoint AllWaypoints[MAXWAYPOINTS];
} level, *Level; 

typedef struct {
  int LevelsOnShip;
  Level AllLevels[MAX_LEVELS_ON_SHIP];
  elevator AllElevators[ALLELEVATORS];
} ship, *Ship;


typedef struct {
  char* FCUName;
} FCU;

typedef struct {
  char* shieldname;
} shieldspec, *Shieldspec;

typedef struct {
  point pos;
  int len;
  int hgt;
  int oldval;
  int col;
} bar, *Bar;

#endif

/*=@Header==============================================================
 * $Source$
 *
 * @Desc: struct and typedef definitions 
 *    
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.21  1997/06/10 20:10:47  rprix
 * Put complete source under GPL Licence: added GPL-Licence file: COPYING
 * and added Copyright-header to all .c and .h files.
 * PS: How do you copyright binaries (.pcx,.wav...) and data-files (.shp)??
 *
 * Neue Header braucht das Land: Alte RCS- Keyword header ganz entfernt,
 * only Description-entry in header remains.
 *
 * Revision 1.20  1997/06/09 23:08:59  jprix
 * Blast phases now adapted to the current framerate.  New constant for speed of animation independant of framerate.
 *
 * Revision 1.19  1997/06/09 21:53:49  jprix
 * Rotation of enemys and influencer now independant of the framerate.
 *
 * Revision 1.18  1997/06/09 21:00:56  jprix
 * The constants for the druids have been largely rescaled to MUCH larger values.
 * This is for the new float and framedependent movement of the enemys.  It works nicley
 * as you will see from the now very smooth movement of each of them.
 *
 * Revision 1.17  1997/06/09 16:22:07  jprix
 * Original images partly overwritten by old versions corrected.
 * Started Load_PCX_Image(..) function.
 *
 * Revision 1.16  1997/06/09 13:01:29  jprix
 * Bullet position and speed now also as float.  Program still functionin. Heeyooh! Great!
 *
 * Revision 1.15  1997/06/09 10:50:29  jprix
 * Halfway through with making robot coordinates also info floats.  Still works :->
 *
 * Revision 1.14  1997/06/08 23:46:44  jprix
 * influence_t uses floats now for the coodinates of the influ.  transition successful.
 *
 * Revision 1.13  1997/06/08 23:19:38  jprix
 * Transition to floating point coordinates started.  This version is still working.
 *
 * Revision 1.12  1997/06/08 21:46:00  jprix
 * Modified energy of influence and enemys to be now floats instead of ints. cool.
 *
 * Revision 1.11  1997/06/08 16:33:10  jprix
 * Eliminated all warnings, that resulted from the new -Wall gcc flag.
 *
 * Revision 1.10  2002/04/08 19:19:09  rp
 * Johannes latest (and last) non-cvs version to be checked in. Added graphics,sound,map-subdirs. Sound support using ALSA started.
 *
 * Revision 1.10  1997/05/31 13:30:32  rprix
 * Further update by johannes. (sent to me in tar.gz)
 *
 * Revision 1.7  1994/06/19  16:41:38  prix
 * Thu Sep 30 13:57:49 1993: Header moved to end of file
 * Sun Oct 03 09:07:38 1993: enemy-struct hat "onscreen" dazubekommen
 * Sun Oct 03 09:16:34 1993: "flashimmune" added to druidspec
 * Sun Oct 03 09:23:53 1993: "time" added to bullet
 * Mon Oct 04 15:30:32 1993: reduced sizes of structs wherever possible
 * Sun Oct 17 09:02:30 1993: vneutral added to druidspec
 * Mon Oct 25 14:23:04 1993: enemy-struct um (byte passable) erweitert
 * Tue Jun 14 10:47:50 1994: added structure for bar
 * Wed Jun 15 20:06:11 1994: Shield[4] added to enemy
 *
 * Revision 1.6  1993/09/30  17:57:20  prix
 * Fri Aug 06 14:56:25 1993: added first version of ship-struct
 * Fri Aug 06 15:29:06 1993: removed levelname from level-struct
 * Sat Aug 07 15:53:45 1993: made location more "plain"
 * Sat Aug 07 16:03:52 1993: changed elevator struct
 * Sun Aug 08 17:58:49 1993: little change in  ship-struct
 * Sun Aug 08 20:53:11 1993: added struct waypoint and changed enemy - struct
 * Mon Aug 09 20:00:15 1993: added emty to level-struct
 * Mon Aug 09 20:01:37 1993: added color to level-struct
 * Mon Aug 09 21:10:50 1993: changes in bulletspec: time and in bullet: timecounter
 * Tue Aug 10 14:52:11 1993: removed timecounter from bulletstruct
 * Sat Aug 14 09:38:26 1993: firewait now in druidspec
 * Sat Aug 21 19:50:22 1993: added refreshes array to level-struct
 * Tue Aug 24 17:29:30 1993: lose_health and health added
 * Thu Sep 30 13:56:59 1993: No picpointer in Druidspec !!
 *
 * Revision 1.5  1993/08/03  18:31:23  prix
 * Mon May 24 20:18:48 1993: *picture in struct druidmodel to type void*
 * Mon May 24 20:20:54 1993: druidname is now const char*
 * Tue May 25 14:35:29 1993: Bullet auf Absolutkoordinaten reduziert
 * Fri May 28 17:36:51 1993: added struct guntype
 * Fri May 28 19:47:59 1993: made picpointers far
 * Fri May 28 21:20:44 1993: added phase-vars to druid-structs
 * Fri May 28 21:46:09 1993: changed Blast-struct: no grob/fein pos !
 * Fri May 28 22:05:32 1993: made point and vect structs with signed ints
 * Sat May 29 22:38:29 1993: made map-array char in struct Level
 * Sun May 30 10:16:26 1993: map in Levelstruct ist wieder int !
 * Sun May 30 10:45:58 1993: map is char* again
 * Sun May 30 18:50:46 1993: renamed a bit
 * Sun May 30 19:27:16 1993: additions to druidspec struct
 * Sun May 30 20:36:27 1993: added blastspec struct
 * Mon May 31 14:40:55 1993: made blast/bullet the struct and Blast/Bullet a pointer to it
 * Tue Jun 01 08:49:41 1993: druidspec includes maxenergy now
 * Tue Jun 01 11:57:32 1993: bulletspec includes the blast it makes now
 * Tue Jun 01 16:08:09 1993: detabbed for printing
 * Sat Jul 24 14:15:17 1993: enemy-Struktur eingefuehrt
 * Sat Jul 24 16:25:28 1993: feindphase eingefuehrt
 * Sun Jul 25 07:53:45 1993: aenderung der Struct enemy: waypoints und point-Koordinaten
 * Mon Jul 26 12:33:49 1993: notes on the druid in the structure
 * Thu Jul 29 07:39:06 1993: warten in enemy-struct eingefuehrt
 * Thu Jul 29 09:23:23 1993: PX,PY,SX,SY sind int
 * Thu Jul 29 12:14:28 1993: Elevator struct eingefuehrt ???
 * Thu Jul 29 12:34:29 1993: energy auf damage umbenannt
 * Sat Jul 31 07:41:52 1993: struct color hinzugefuegt
 * Sat Jul 31 10:46:56 1993: waypoints - array now out of enemy-array (easier to init !)
 * Sat Jul 31 11:35:08 1993: added firewait to druidspec
 * Sat Jul 31 11:49:35 1993: changed druidspec: no phases, turnable; added aggression
 * Sat Jul 31 12:05:02 1993: added levelnum to level-struct
 * Sat Jul 31 12:11:05 1993: removed firewait again: in bulletspec-struct !!
 * Sat Jul 31 12:15:12 1993: changed druid-struct: firewait instead of firedir
 * Sat Jul 31 12:22:59 1993: changed nofire in enemy to firewait
 * Sat Jul 31 12:50:44 1993: Influence now has his own type
 * Sat Jul 31 17:47:35 1993: firedir added to Influence_t -struct
 * Sat Jul 31 19:43:57 1993: added levelnum to enemy struct
 * Sat Jul 31 21:30:52 1993: added doorsarray to level struct
 * Tue Aug 03 14:27:31 1993: removed drive from druidspec-struct
 * Tue Aug 03 14:31:16 1993: removed firedir and levelnum of influence_t struct
 *
 * Revision 1.4  1993/05/23  21:07:55  prix
 * Sun May 23 16:15:04 1993: added druid and druidmodel structs: first concepts
 *
 * Revision 1.3  1993/05/23  19:33:53  prix
 * Sat May 22 18:21:16 1993: Soundblastertypen hizugefuegt
 * Sat May 22 18:52:08 1993: Zuruecknahmen
 *
 * Revision 1.2  1993/05/22  21:58:42  rp
 * added level struct
 *
 * Revision 1.1  1993/05/22  20:56:16  rp
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

