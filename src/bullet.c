/*
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 */

/* ----------------------------------------------------------------------
 * Desc: all Bullet AND Blast - related functions.
 * ---------------------------------------------------------------------- */

/*
 * This file has now been checked for remains of german (in the comments
 * at least.  Please eliminate anything else mercilessly if you find something
 * german in this file still.
 */

#define _bullet_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/* Distances for hitting a druid */
#define MORE		(4/64.0)
#define DRUIDHITDIST2		(0.3+MORE)*(Druid_Radius_Y+MORE)

/* ----------------------------------------------------------------------
 * This function does the rotation of a given vector by a given angle
 * ---------------------------------------------------------------------- */
void
RotateVectorByAngle ( moderately_finepoint* vector , float rot_angle )
{
  moderately_finepoint new_vect;
  float rad_angle;

  rad_angle = rot_angle * ( M_PI / 180.0 ) ; // / 180 );
  
  DebugPrintf( 1 , "\n RAD_ANGLE : %f " , rad_angle );
  new_vect.x =  sin( rad_angle ) * vector->y + cos( rad_angle ) * vector->x;
  new_vect.y =  cos( rad_angle ) * vector->y - sin( rad_angle ) * vector->x;
  vector->x = new_vect.x;
  vector->y = new_vect.y;
}; // void RotateVectorByAngle ( ... )

/* ----------------------------------------------------------------------
 * This function moves all the bullets according to their speeds and the
 * current frame rate of course.
 * ---------------------------------------------------------------------- */
void
MoveBullets (void)
{
  int i;
  int map_x;
  int map_y;
  Bullet CurBullet;
  moderately_finepoint dist_vector;

  // movement of hte bullets
  for (CurBullet = AllBullets, i = 0; i < MAXBULLETS; CurBullet++, i++)
    {
      //--------------------
      // We need not move any bullets, that are OUT already...
      //
      if ( CurBullet->type == OUT )
	continue;

      //--------------------
      // In case of a bullet, which is not a melee weapon, we just move
      // the bullets as specified in it's speed vector
      //
      CurBullet->pos.x += CurBullet->speed.x * Frame_Time ();
      CurBullet->pos.y += CurBullet->speed.y * Frame_Time ();
      if ( CurBullet->angle_change_rate > 0 )
	{
	  //--------------------
	  // We change the angle of the bullet itself.  That's the easier part.
	  // Rotation by selecting angle.  Very easy indead.
	  //
	  DebugPrintf( 1 , "\n Angle change rate : %f " , CurBullet->angle_change_rate );
	  CurBullet->angle += CurBullet->angle_change_rate * Frame_Time();

	  //--------------------
	  // Now we must rotate the bullet around the influence device or other
	  // owner as specified in the bullets owner pointer
	  //
	  dist_vector.x = 0;
	  dist_vector.y = - CurBullet->fixed_offset;

	  DebugPrintf( 1 , "\n distance vector : (%f/%f) " , dist_vector.x , dist_vector.y );

	  RotateVectorByAngle ( &dist_vector , CurBullet->angle );

	  CurBullet->pos.x = CurBullet->owner_pos->x + dist_vector.x;
	  CurBullet->pos.y = CurBullet->owner_pos->y + dist_vector.y;

	  // We tell the graphics functions, that they shall generate new pictures...
	  if ( CurBullet->Surfaces_were_generated )
	    {
	      SDL_FreeSurface( CurBullet->SurfacePointer[0] );
	      CurBullet->Surfaces_were_generated = FALSE ;
	    }
	}

      //--------------------
      // Maybe the bullet has a limited lifetime.  In that case we check if the
      // bullet has expired yet or not.
      //
      if ( ( CurBullet->bullet_lifetime != (-1) ) && 
	   ( CurBullet->time_in_seconds > CurBullet->bullet_lifetime ) )
	{
	  DeleteBullet( i , FALSE );
	  continue;
	}
      CurBullet->time_in_frames++;
      CurBullet->time_in_seconds += Frame_Time();

      //--------------------
      // Maybe the bullet is currently on a converyor belt.
      // In this case, be must move on the bullet accordinly
      //
      map_x= (int) rintf( CurBullet->pos.x );
      map_y= (int) rintf( CurBullet->pos.y );

      //--------------------
      // But maybe the bullet is also outside the map already, which would
      // cause a SEGFAULT directly afterwards, when the map is queried.
      // Therefore we introduce some extra security here...
      //
      if ( ( map_x < 0 ) || ( map_x >= CurLevel->xlen ) ||
	   ( map_y < 0 ) || ( map_y >= CurLevel->ylen ) )
	{
	  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A BULLET WAS FOUND TO EXIST OUTSIDE THE BOUNDS OF THE MAP.\n\
This is an idication for an error of some form, but might also be due
to short occasions of very low frame rates sometimes or it might be due
to a melee weapon swing active when the swing robot was destroyed, which
swapped him outside and the bullet thereby also outside.\n\
\n\
This problem is not severe, so this is a warning message only.\n\
\n\
Freedroid will NOT terminate now to draw attention to the problem\n\
encounteres in the function MoveBullets().  Game should proceed as usual.\n\
----------------------------------------------------------------------\n\
\n" );
	  DeleteBullet ( i , FALSE );
	  return;
	  
	  Terminate(ERR);
	}
      

      switch ( CurLevel->map[ map_y ] [ map_x ] )
	{
	case CONVEY_L:
	  CurBullet->pos.x += Conveyor_Belt_Speed * Frame_Time();
	  break;
	case CONVEY_R:
	  CurBullet->pos.x -= Conveyor_Belt_Speed * Frame_Time();
	  break;
	case CONVEY_U:
	  CurBullet->pos.y -= Conveyor_Belt_Speed * Frame_Time();
	  break;
	case CONVEY_D:
	  CurBullet->pos.y += Conveyor_Belt_Speed * Frame_Time();
	  break;
	default:
	  break;
	}
    }				/* for */
}; // void MoveBullets(void)

/* ----------------------------------------------------------------------
 * This function eliminates the bullet with the given number.  As an 
 * additional parameter you can specify if there should be a blast 
 * generated at the location where the bullet died (=TRUE) or not (=FALSE).
 * ---------------------------------------------------------------------- */
void 
DeleteBullet ( int Bulletnumber , int ShallWeStartABlast )
{
  Bullet CurBullet = &AllBullets[ Bulletnumber ];
  int i;

  //--------------------
  // At first we generate the blast at the collision spot of the bullet,
  // cause later, after the bullet is deleted, it will be hard to know
  // the correct location ;)
  //
  if ( ShallWeStartABlast ) StartBlast ( CurBullet->pos.x, CurBullet->pos.y, BULLETBLAST );

  //--------------------
  // maybe, the bullet had several SDL_Surfaces attached to it.  Then we need to 
  // free the SDL_Surfaces again as well...
  //
  if ( CurBullet->Surfaces_were_generated ) 
    {
      DebugPrintf( 1 , "\nvoid DeleteBullet(...): freeing this bullets attached surfaces...");
      for ( i=0 ; i < Bulletmap[ CurBullet->type ].phases ; i++ )
	{
	  SDL_FreeSurface( CurBullet->SurfacePointer[i] );
	  CurBullet->SurfacePointer[i] = NULL;
	}
      CurBullet->Surfaces_were_generated = FALSE;
    }

  //--------------------
  // Now that the memory has been freed again, we can finally delete this bullet entry.
  // Hope, that this does not give us a SEGFAULT, but it should not do so.
  //
  CurBullet->type = OUT;
  CurBullet->time_in_seconds = 0;
  CurBullet->time_in_frames = 0;
  CurBullet->mine = FALSE;
  CurBullet->phase = 0;
  CurBullet->pos.x = 0;
  CurBullet->pos.y = 0;
  CurBullet->angle = 0;

}; // void DeleteBullet( int Bulletnumber , int StartBlast )

/* ----------------------------------------------------------------------
 * This function starts a blast (i.e. an explosion) at the given location
 * in the usual map coordinates of course.  The type of blast must also
 * be specified, where possible values are defined in defs.h as follows:
 *
 * BULLETBLAST = 0 , (explosion of a small bullet hitting the wall)
 * DRUIDBLAST,       (explosion of a dying droid)
 * OWNBLAST          (not implemented)
 *
 * ---------------------------------------------------------------------- */
void
StartBlast (float x, float y, int type)
{
  int i;
  Blast NewBlast;

  //--------------------
  // Maybe there is a box under the blast.  In this case, the box will
  // get smashed and perhaps an item will drop.
  Smash_Box ( x , y );

  // find out the position of the next free blast
  for (i = 0; i < MAXBLASTS; i++)
    if (AllBlasts[i].type == OUT)
      break;

  // didn't fine any --> then take the first one
  if (i >= MAXBLASTS)
    i = 0;

  // get pointer to it: more comfortable 
  NewBlast = &(AllBlasts[i]);

  // create a blast at the specified x/y coordinates
  NewBlast->pos.x = x;
  NewBlast->pos.y = y;

  NewBlast->type = type;
  NewBlast->phase = 0;

  NewBlast->MessageWasDone = 0;

  if (type == DRUIDBLAST)
    {
      DruidBlastSound ();
    }

}; // void StartBlast( ... )

/* ----------------------------------------------------------------------
 * This function advances the different phases of an explosion according
 * to the current lifetime of each explosion (=blast).
 * ---------------------------------------------------------------------- */
void
ExplodeBlasts (void)
{
  int i, map_x, map_y;
  Blast CurBlast = AllBlasts;

  for (i = 0; i < MAXBLASTS; i++, CurBlast++)
    if (CurBlast->type != OUT)
      {
	
	//--------------------
	// But maybe the bullet is also outside the map already, which would
	// cause a SEGFAULT directly afterwards, when the map is queried.
	// Therefore we introduce some extra security here...
	//
	map_x= (int) rintf( CurBlast->pos.x );
	map_y= (int) rintf( CurBlast->pos.y );
	if ( ( map_x < 0 ) || ( map_x >= CurLevel->xlen ) ||
	     ( map_y < 0 ) || ( map_y >= CurLevel->ylen ) )
	  {
	    fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A BLAST WAS FOUND TO EXIST OUTSIDE THE BOUNDS OF THE MAP.\n\
This is an idication for a severe error in Freedroid.\n\
\n\
Please report the problem to the Freedroid developers.
\n\
Sorry for interrupting your game, but Freedroid will terminate now\n\
to draw attention to the internal problem that indicates an error\n\
in the code.  Please tell the developers about the problem detected\n\
from the function MoveBullets().  Sorry for destroying your game...\n\
----------------------------------------------------------------------\n\
\n" );
	    Terminate(ERR);
	  }

	//--------------------
	// Druid blasts are dangerous, so we check if someone gets
	// hurt by this particular droid explosion
	//
	if (CurBlast->type == DRUIDBLAST) CheckBlastCollisions (i);

	//--------------------
	// And now we advance the phase of the blast according to the
	// time that has passed since the last frame (approximately)
	//
	CurBlast->phase += Frame_Time () * Blastmap[ CurBlast->type ].phases / Blastmap[ CurBlast->type ].total_animation_time;

	//--------------------
	// Maybe the blast has lived over his normal lifetime already.
	// Then of course it's time to delete the blast, which is done
	// here.
	//
	if (((int) floorf (CurBlast->phase)) >=
	    Blastmap[CurBlast->type].phases)
	  DeleteBlast (i);
      }				/* if */
}; // void ExplodeBlasts( ... )

/* ----------------------------------------------------------------------
 * This function deletes a single blast entry from the list of all blasts
 * ---------------------------------------------------------------------- */
void
DeleteBlast (int BlastNum)
{
  AllBlasts[ BlastNum ].phase = OUT;
  AllBlasts[ BlastNum ].type = OUT;
}; // void DeleteBlast( int BlastNum )

/* ----------------------------------------------------------------------
 * THIS FUNCTION IS CURRENTLY NOWHERE USED!!!
 * Is shall obviously tell from which direction a given robot was hit.
 * (I guess this function may fail utterly when framerate is low!)
 * ---------------------------------------------------------------------- */
int
GetDirection (point robo, point bul)
{

  if ((robo.x < bul.x) && (robo.y > bul.y))
    return 0;
  if ((robo.x < bul.x) && (robo.y < bul.y))
    return 1;
  if ((robo.x > bul.x) && (robo.y < bul.y))
    return 2;
  if ((robo.x > bul.x) && (robo.y > bul.y))
    return 3;
  if ((robo.x == bul.x) && (robo.y == bul.y))
    {
      DebugPrintf (2, " Center hit directy!");
      getchar ();
    }
  return 0;
}; // int GetDirection (point robo, point bul)

/* ----------------------------------------------------------------------
 * This function checks if there are some collisions of the one bullet
 * with number num with anything else in the game, like blasts, walls,
 * droids, the tux and other bullets.
 * ---------------------------------------------------------------------- */
void
CheckBulletCollisions (int num)
{
  int i;
  int level = CurLevel->levelnum;
  double xdist, ydist;
  Bullet CurBullet = &AllBullets[num];
  static int FBTZaehler = 0;

  switch (CurBullet->type)
    {
      // --------------------
      // Never do any collision checking if the bullet is OUT already...
    case OUT:
      return;
      break;
      
      // --------------------
      // Next we handle the case that the bullet is of type FLASH
    case FLASH:
      // if the flash is over, just delete it and return
      if ( CurBullet->time_in_seconds > FLASH_DURATION_IN_SECONDS )
	{
	  CurBullet->time_in_frames = 0;
	  CurBullet->time_in_seconds = 0;
	  CurBullet->type = OUT;
	  CurBullet->mine = FALSE;
	  return;
	}
      
      // if the flash is not yet over, do some checking for who gets
      // hurt by it.  
      // Two different methode for doing this are available:
      // The first but less elegant Method is just to check for
      // flash immunity, for distance and visiblity.
      // The second and more elegant method is to recursively fill
      // out the room where the flash-maker is in and to hurt all
      // robots in there except of course for those immune.
      if ( CurBullet->time_in_frames != 1 ) return; // we only do the damage once and thats at frame nr. 1 of the flash
      
      // for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
      for (i = 0; i < Number_Of_Droids_On_Ship ; i++)
	{
	  if ( IsVisible (&AllEnemys[i].pos) &
	       (!Druidmap[AllEnemys[i].type].flashimmune) )
	    {
	      // ITEMS AllEnemys[i].energy -= Bulletmap[FLASH].damage;
	      AllEnemys[i].energy -= CurBullet->damage;
	      // Since the enemy just got hit, it might as well say so :)
	      EnemyHitByBulletText( i );
	    }
	}
      
      if (!InvincibleMode && !Druidmap[Me.type].flashimmune)
	{
	  // ITEMS Me.energy -= Bulletmap[FLASH].damage ;
	  Me.energy -= CurBullet->damage ;
	}

      return;
      break;

      // --------------------
      // If its a "normal" Bullet, several checks have to be
      // done, one for collisions with background, 
      // one for collision with influencer
      // some for collisions with enemys
      // and some for collisions with other bullets
      // and some for collisions with blast
      //
    default:
      
      // Check for collision with background
      if (IsPassable (CurBullet->pos.x, CurBullet->pos.y, CENTER) != CENTER)
	{
	  if ( CurBullet->ignore_wall_collisions )
	    {
	      StartBlast ( CurBullet->pos.x, CurBullet->pos.y, BULLETBLAST );
	    }
	  else
	    {
	      DeleteBullet ( num , TRUE ); // we want a bullet-explosion
	      return;
	    }
	}

      // check for collision with influencer
      xdist = Me.pos.x - CurBullet->pos.x;
      ydist = Me.pos.y - CurBullet->pos.y;
      if ((xdist * xdist + ydist * ydist) < DRUIDHITDIST2)
	{

	  if ( (!InvincibleMode) && ( CurBullet->miss_hit_influencer == UNCHECKED ) )
	    {
	      if ( MyRandom ( 100 ) < CurBullet->to_hit )
		{
		  CurBullet->miss_hit_influencer = HIT ;

		  //--------------------
		  // NEW RULE:  Even when the bullet hits, there's still a chance that
		  // the armour will compensate the shot
		  //
		  if ( MyRandom( 100 ) < Me.AC )
		    {
		      Me.TextVisibleTime = 0;
		      Me.TextToBeDisplayed = "That one went into the armour.";
		      BulletReflectedSound ( ) ;
		    }
		  else
		    {

		      Me.TextVisibleTime = 0;
		      Me.TextToBeDisplayed = "Ouch!";
		      Me.energy -= CurBullet->damage;	// loose some energy

		      //--------------------
		      // As the new rule, the influencer after getting hit, must completely
		      // start anew to recover his weapon from the previous shot
		      //
		      Me.firewait = ItemMap[ Me.weapon_item.type ].item_gun_recharging_time;
		      Me.got_hit_time = 0;

		      // GotHitSound ();
		      Influencer_Scream_Sound ( );
		    }
		  //--------------------
		  // NEW RULE:  All items equipped suffer damage when the influencer gets hit
		  //
		  DamageAllEquipment(  );
		  DeleteBullet( num , TRUE ); // we want a bullet-explosion
		  return;			/* Bullet ist hin */
		}
	      else
		{
		  CurBullet->miss_hit_influencer = MISS ;
		}
	    }
	}


      // check for collision with enemys
      // for (i = 0; i < NumEnemys; i++)
      for (i = 0; i < Number_Of_Droids_On_Ship; i++)
	{
	  if (AllEnemys[i].Status == OUT || AllEnemys[i].levelnum != level)
	    continue;

	  xdist = CurBullet->pos.x - AllEnemys[i].pos.x;
	  ydist = CurBullet->pos.y - AllEnemys[i].pos.y;

	  if ( (xdist * xdist + ydist * ydist) < DRUIDHITDIST2 )
	    {
	      if ( CurBullet->total_miss_hit[ i ] == UNCHECKED )
		{
		  if ( MyRandom ( 100 ) < CurBullet->to_hit + Druidmap [ AllEnemys[ i ].type ].getting_hit_modifier )
		    {
		      CurBullet->total_miss_hit[ i ] = HIT;

		      //--------------------
		      // The enemy who was hit, loses some energy, depending on the bullet, and 
		      // also gets stunned from the hit, which only means that the enemy can't
		      // fire immediately now but takes double normal time for the next shot.
		      //
		      AllEnemys[i].energy -= CurBullet->damage;
		      
		      AllEnemys[i].firewait =
			2 * ItemMap [ Druidmap [ AllEnemys[ i ].type ].weapon_item.type ].item_gun_recharging_time ;

		      // Maybe he will also stop doing his fixed routine and return to normal
		      // operation as well
		      AllEnemys[i].AdvancedCommand = 0;

		      // We might also start a little bullet-blast even after the
		      // collision of the bullet with an enemy (not in Paradroid)

		      //--------------------
		      // If the blade can pass through dead and not dead bodies, it will so
		      // so and create a small explosion passing by.  But if it can't, it should
		      // be completely deleted of course, with the same small explosion as well
		      //
		      if ( CurBullet->pass_through_hit_bodies )
			StartBlast ( CurBullet->pos.x, CurBullet->pos.y, BULLETBLAST );
		      else DeleteBullet( num , TRUE ); // we want a bullet-explosion

		      Enemy_Post_Bullethit_Behaviour( i );

		      if (!CurBullet->mine)
			{
			  FBTZaehler++;
			}
		      return;
		    }
		  else
		    {
		      CurBullet->total_miss_hit[ i ] = MISS;
		      AllEnemys[ i ].TextVisibleTime = 0;
		      AllEnemys[ i ].TextToBeDisplayed = "Haha, you missed me!";
		    }
		}
	    } // if distance low enough to possibly be at hit
	}  /* for AllEnemys */

      // check for collisions with other bullets
      for (i = 0; i < MAXBULLETS; i++)
	{
	  if (i == num) continue;  // never check for collision with youself.. ;)
	  if (AllBullets[i].type == OUT) continue; // never check for collisions with dead bullets.. 
	  if (AllBullets[i].type == FLASH) continue; // never check for collisions with flashes bullets.. 

	  if ( fabsf(AllBullets[i].pos.x-CurBullet->pos.x) > BULLET_BULLET_COLLISION_DIST ) continue;
	  if ( fabsf(AllBullets[i].pos.y-CurBullet->pos.y) > BULLET_BULLET_COLLISION_DIST ) continue;
	  // it seems like we have a collision of two bullets!
	  // both will be deleted and replaced by blasts..
	  DebugPrintf ( 1 , "\nBullet-Bullet-Collision detected..." );
	  
	  //CurBullet->type=OUT;
	  //AllBullets[num].type=OUT;

	  if ( CurBullet->reflect_other_bullets )
	    {
	      if ( AllBullets[ i ].was_reflected )
		{
		  // well, if it has been reflected once, we don't do any more
		  // reflections after that...
		}
	      else
		{
		  AllBullets[i].speed.x = - AllBullets[i].speed.x;
		  AllBullets[i].speed.y = - AllBullets[i].speed.y;
		  AllBullets[i].was_reflected = TRUE;
		}
	    }

	  if ( AllBullets[ i ].reflect_other_bullets )
	    {
	      if ( CurBullet->was_reflected )
		{
		  // well, if it has been reflected once, we don't do any more
		  // reflections after that...
		}
	      else
		{
		  CurBullet->speed.x = - CurBullet->speed.x;
		  CurBullet->speed.y = - CurBullet->speed.y;
		  CurBullet->was_reflected = TRUE;
		}
	    }

	}
      break;
    } // switch ( Bullet-Type )
}; // CheckBulletCollisions( ... )
  
/* ----------------------------------------------------------------------
 * This function checks for collisions of blasts with bullets and druids
 * and delivers damage to the hit objects according to how long they have
 * been in the blast.
 * 
 * Maybe even some text like 'Ouch, this was hot' might be generated.
 *
 * ---------------------------------------------------------------------- */
void
CheckBlastCollisions (int num)
{
  int i;
  int level = CurLevel->levelnum;
  Blast CurBlast = &(AllBlasts[num]);

  //--------------------
  // At first, we check for collisions of this blast with all bullets 
  //
  for (i = 0; i < MAXBULLETS; i++)
    {
      if (AllBullets[i].type == OUT)
	continue;
      if (CurBlast->phase > 4)
	break;

      if (abs (AllBullets[i].pos.x - CurBlast->pos.x ) < Blast_Radius)
	if (abs (AllBullets[i].pos.y - CurBlast->pos.y ) < Blast_Radius)
	  {
	    if ( ! AllBullets[i].pass_through_explosions )
	      {
		DeleteBullet( i , TRUE ); // we want a bullet-explosion
	      }
	  }

    }	

  //--------------------
  // Now we check for enemys, that might have stepped into this
  // one blasts area of effect...
  //
  for (i = 0; i < NumEnemys; i++)
    {
      if ((AllEnemys[i].Status == OUT)
	  || (AllEnemys[i].levelnum != level))
	continue;

      if ( ( fabsf (AllEnemys[i].pos.x - CurBlast->pos.x ) < Blast_Radius ) &&
	   ( fabsf (AllEnemys[i].pos.y - CurBlast->pos.y ) < Blast_Radius ) )
	  {
	    /* drag energy of enemy */
	    AllEnemys[i].energy -= Blast_Damage_Per_Second * Frame_Time ();
	  }
    }				/* for */

  //--------------------
  // Now we check, if perhaps the influencer has stepped into the area
  // of effect of this one blast.  Then he'll get burnt ;)
  // 
  if ( (Me.status != OUT) && 
       ( fabsf (Me.pos.x - CurBlast->pos.x ) < Blast_Radius ) &&
       ( fabsf (Me.pos.y - CurBlast->pos.y ) < Blast_Radius ) )
    {
      if (!InvincibleMode)
	{
	  Me.energy -= Blast_Damage_Per_Second * Frame_Time ();
	  
	  // So the influencer got some damage from the hot blast
	  // Now most likely, he then will also say so :)
	  if ( !CurBlast->MessageWasDone )
	    {
	      AddInfluBurntText();
	      CurBlast->MessageWasDone=TRUE;
	    }
	  
	}
      // In order to avoid a new sound EVERY frame we check for how long the previous blast 
      // lies back in time.  LastBlastHit is a float, that counts SECONDS real-time !!
      if (LastGotIntoBlastSound > 1.2)
	{
	  GotIntoBlastSound ();
	  LastGotIntoBlastSound = 0;
	}
    }

}; // CheckBlastCollisions( ... )

#undef _bullet_c
