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
 *
 */

/* ----------------------------------------------------------------------
 * This file contains all the functions managing the character attributes
 * and the character stats.
 * ---------------------------------------------------------------------- */

#define _character_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#include "items.h"

#define CLASS_X 175

#define EXPERIENCE_Y 55
#define NEXT_LEVEL_Y 82

#define GOLD_Y 132

#define STR_BASE_X 100
#define STR_NOW_X 148
#define STR_Y 143
#define MAG_Y 171
#define DEX_Y 197
#define VIT_Y 227
#define POINTS_Y 252

#define BUTTON_MOD_X (-6)
#define BUTTON_MOD_Y (-4)
#define BUTTON_WIDTH 35
#define BUTTON_HEIGHT 19

#define DAMAGE_X 260
#define DAMAGE_Y 225

#define RECHARGE_X 260
#define RECHARGE_Y 200

#define AC_X 260
#define AC_Y 171

#define CHARACTERRECT_X (SCREENLEN/2)
// #define CHARACTERRECT_Y (User_Rect.y)
#define CHARACTERRECT_W (SCREENLEN/2)
#define CHARACTERRECT_H (User_Rect.h)

#define ENERGY_GAIN_PER_VIT_POINT 2
#define MANA_GAIN_PER_MAGIC_POINT 2
// #define DAMAGE_GAIN_PER_STR_POINT 2
#define AC_GAIN_PER_DEX_POINT 1
#define RECHARGE_SPEED_PERCENT_PER_DEX_POINT 0
#define TOHIT_PERCENT_PER_DEX_POINT (0.5)

// #define INV_BUTTON_X 20
#define INV_BUTTON_X 600
#define INV_BUTTON_Y 400
// #define INV_BUTTON_Y 10
// #define CHA_BUTTON_X 20
#define CHA_BUTTON_X 600
#define CHA_BUTTON_Y 430
// #define CHA_BUTTON_Y 30
#define INV_BUTTON_WIDTH 38
#define INV_BUTTON_HEIGHT 22

SDL_Rect CharacterRect;


/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * strength plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnINVButton( int x , int y )
{
  if ( x > INV_BUTTON_X + INV_BUTTON_WIDTH  ) return ( FALSE );
  if ( x < INV_BUTTON_X                     ) return ( FALSE );
  if ( y > INV_BUTTON_Y + INV_BUTTON_HEIGHT ) return ( FALSE );
  if ( y < INV_BUTTON_Y                     ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * strength plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnCHAButton( int x , int y )
{
  if ( x > CHA_BUTTON_X + INV_BUTTON_WIDTH  ) return ( FALSE );
  if ( x < CHA_BUTTON_X                     ) return ( FALSE );
  if ( y > CHA_BUTTON_Y + INV_BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CHA_BUTTON_Y                     ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )


/* ----------------------------------------------------------------------
 * This function displays all the buttons that open up the character
 * screen and the invenotry screen
 * ---------------------------------------------------------------------- */
void
DisplayButtons( void )
{
  static SDL_Surface *CHA_ButtonImage = NULL;
  static SDL_Surface *INV_ButtonImage = NULL;
  static SDL_Surface *PlusButtonImage = NULL;
  static SDL_Rect CHA_Button_Rect;
  static SDL_Rect INV_Button_Rect;
  static int WasPressed;
  char* fpath;

  if ( GameConfig.CharacterScreen_Visible == TRUE ) return;
  
  // --------------------
  // Some things like the loading of the character screen
  // need to be done only once at the first call of this
  // function. 
  //
  if ( INV_ButtonImage == NULL )
    {
      // SDL_FillRect( Screen, & InventoryRect , 0x0FFFFFF );
      fpath = find_file ( "CHAButton.png" , GRAPHICS_DIR, FALSE);
      CHA_ButtonImage = IMG_Load( fpath );
      fpath = find_file ( "INVButton.png" , GRAPHICS_DIR, FALSE);
      INV_ButtonImage = IMG_Load( fpath );
      fpath = find_file ( "PlusButton.png" , GRAPHICS_DIR, FALSE);
      PlusButtonImage = IMG_Load( fpath );
    }

  CHA_Button_Rect.x = CHA_BUTTON_X;
  CHA_Button_Rect.y = CHA_BUTTON_Y;
  // CHA_Button_Rect.w = CHARACTERRECT_W;
  // CHA_Button_Rect.h = CHARACTERRECT_H;
  
  INV_Button_Rect.x = INV_BUTTON_X;
  INV_Button_Rect.y = INV_BUTTON_Y;
  // INV_Button_Rect.w = CHARACTERRECT_W;
  // INV_Button_Rect.h = CHARACTERRECT_H;

  //--------------------
  // Now we can draw either the plus button or the 'cha' button, depending
  // on whether there are points to distribute or not
  //
  if ( Me.PointsToDistribute > 0 )
    {
      SDL_BlitSurface( PlusButtonImage , NULL , Screen , &CHA_Button_Rect );
    }
  else
    {
      SDL_BlitSurface( CHA_ButtonImage , NULL , Screen , &CHA_Button_Rect );
    }

  SDL_BlitSurface( INV_ButtonImage , NULL , Screen , &INV_Button_Rect );
  
  if ( CursorIsOnINVButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
    {
      GameConfig.Inventory_Visible = ! GameConfig.Inventory_Visible;
    }

  if ( CursorIsOnCHAButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
    {
      GameConfig.CharacterScreen_Visible = ! GameConfig.CharacterScreen_Visible;
    }

  WasPressed = axis_is_active;

}; // void DisplayButtons( void )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * strength plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnStrButton( int x , int y )
{
  if ( x > CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X + BUTTON_WIDTH ) return ( FALSE );
  if ( x < CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X ) return ( FALSE );
  if ( y > CharacterRect.y + BUTTON_MOD_X + STR_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CharacterRect.y + BUTTON_MOD_X + STR_Y ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * dexterity plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnDexButton( int x , int y )
{
  if ( x > CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X + BUTTON_WIDTH ) return ( FALSE );
  if ( x < CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X ) return ( FALSE );
  if ( y > CharacterRect.y + BUTTON_MOD_X + DEX_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CharacterRect.y + BUTTON_MOD_X + DEX_Y ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnDexButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * magic plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnMagButton( int x , int y )
{
  if ( x > CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X + BUTTON_WIDTH ) return ( FALSE );
  if ( x < CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X ) return ( FALSE );
  if ( y > CharacterRect.y + BUTTON_MOD_X + MAG_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CharacterRect.y + BUTTON_MOD_X + MAG_Y ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnMagButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * vitality plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnVitButton( int x , int y )
{
  if ( x > CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X + BUTTON_WIDTH ) return ( FALSE );
  if ( x < CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X ) return ( FALSE );
  if ( y > CharacterRect.y + BUTTON_MOD_X + VIT_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CharacterRect.y + BUTTON_MOD_X + VIT_Y ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnVitButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function adds any bonuses that might be on the influencers things
 * concerning ONLY PRIMARY STATS, NOT SECONDARY STATS!
 * ---------------------------------------------------------------------- */
void
AddInfluencerItemAttributeBonus( item* BonusItem )
{
  //--------------------
  // In case of no item, the thing to do is pretty easy...
  //
  if ( BonusItem->type == ( -1 ) ) return;

  //--------------------
  // In case of a suffix modifier, we need to apply the suffix...
  //
  if ( ( ( BonusItem->suffix_code != ( -1 ) ) || ( BonusItem->prefix_code != ( -1 ) ) ) &&
       BonusItem -> is_identified )
    {
      Me.Strength  += BonusItem->bonus_to_str + BonusItem->bonus_to_all_attributes ;
      Me.Dexterity += BonusItem->bonus_to_dex + BonusItem->bonus_to_all_attributes ;
      Me.Magic     += BonusItem->bonus_to_mag + BonusItem->bonus_to_all_attributes ;
      Me.Vitality  += BonusItem->bonus_to_vit + BonusItem->bonus_to_all_attributes ;
    }

}; // void AddInfluencerItemAttributeBonus( item* BonusItem )

/* ----------------------------------------------------------------------
 * This function adds any bonuses that might be on the influencers things
 * concerning ONLY SECONDARY STATS, NOT PRIMARY STATS!
 * ---------------------------------------------------------------------- */
void
AddInfluencerItemSecondaryBonus( item* BonusItem )
{
  //--------------------
  // In case of no item, the thing to do is pretty easy...
  //
  if ( BonusItem->type == ( -1 ) ) return;

  //--------------------
  // In case of present suffix or prefix modifiers, we need to apply the suffix...
  //
  if ( ( ( BonusItem->suffix_code != ( -1 ) ) || ( BonusItem->prefix_code != ( -1 ) ) ) &&
       BonusItem->is_identified )
    {

      Me.to_hit    += BonusItem->bonus_to_tohit ;
      Me.maxmana   += BonusItem->bonus_to_force ;
      Me.maxenergy += BonusItem->bonus_to_life ; 
      Me.Vitality  += BonusItem->bonus_to_vit ;

      Me.resist_force       += BonusItem->bonus_to_resist_force ;
      Me.resist_fire        += BonusItem->bonus_to_resist_fire ;
      Me.resist_electricity += BonusItem->bonus_to_resist_electricity ;

    }

}; // void AddInfluencerItemSecondaryBonus( item* BonusItem )

/* ----------------------------------------------------------------------
 * This function should re-compute all character stats according to the
 * currently equipped items and currenly distributed stats points.
 * ---------------------------------------------------------------------- */
void 
UpdateAllCharacterStats ( void )
{
  int BaseExpRequired = 2000;

  //--------------------
  // Maybe the influencer has reached a new experience level?
  // Let's check this.
  // 
  Me.ExpRequired = BaseExpRequired * ( exp ( ( Me.exp_level - 1 ) * log ( 2 ) ) ) ;

  if ( Me.Experience > Me.ExpRequired ) 
    {
      Me.exp_level ++ ;
      Me.PointsToDistribute += 5;

      //--------------------
      // When a droid reaches a new experience level, all health and 
      // force are restored to full this one time
      //
      Me.energy = Me.maxenergy ;
      Me.mana   = Me.maxmana   ;
    }

  //--------------------
  // Now we base PRIMARY stats
  //
  Me.Strength = Me.base_strength;
  Me.Dexterity = Me.base_dexterity;
  Me.Magic = Me.base_magic;
  Me.Vitality = Me.base_vitality;

  //--------------------
  // Now we add all bonuses to the influencers PRIMARY stats
  //
  AddInfluencerItemAttributeBonus( & Me.armour_item );
  AddInfluencerItemAttributeBonus( & Me.weapon_item );
  AddInfluencerItemAttributeBonus( & Me.drive_item );
  AddInfluencerItemAttributeBonus( & Me.shield_item );
  AddInfluencerItemAttributeBonus( & Me.special_item );
  AddInfluencerItemAttributeBonus( & Me.aux1_item );
  AddInfluencerItemAttributeBonus( & Me.aux2_item );

  //--------------------
  // At this point we know, that the primary stats of the influencer
  // have been fully computed.  So that means, that finally we can compute
  // all base SECONDARY stats, that are dependent upon the influencer primary
  // stats.  Once we are done with that, the modifiers to the secondary
  // stats can be applied as well.
  //
  Me.to_hit = 60 + ( Me.Dexterity - 15 ) * TOHIT_PERCENT_PER_DEX_POINT;
  Me.maxenergy = (Me.Vitality) * ENERGY_GAIN_PER_VIT_POINT;
  Me.maxmana   = (Me.Magic)    * MANA_GAIN_PER_MAGIC_POINT;
  // This includes damage done as well...
  if ( Me.weapon_item.type != (-1) )
    {
      if ( ItemMap[ Me.weapon_item.type ].item_gun_angle_change != 0 )
	{
	  Me.base_damage = Me.weapon_item.damage * 
	    ( Me.Strength + 100.0) / 100.0 ;
	  Me.damage_modifier = Me.weapon_item.damage_modifier * 
	    ( Me.Strength + 100.0) / 100.0 ;
	}
      else
	{
	  Me.base_damage = Me.weapon_item.damage * 
	    ( Me.Dexterity + 100.0) / 100.0 ;
	  Me.damage_modifier = Me.weapon_item.damage_modifier * 
	    ( Me.Dexterity + 100.0) / 100.0 ;
	}
    }
  else
    {
      Me.base_damage = 0;
      Me.damage_modifier = 0;
    }
  // ... and also armour class
  Me.AC = ( Me.Dexterity - 15 ) * AC_GAIN_PER_DEX_POINT;
  if ( Me.armour_item.type != (-1) )
    {
      Me.AC += Me.armour_item.ac_bonus;
    }
  if ( Me.shield_item.type != (-1) )
    {
      Me.AC += Me.shield_item.ac_bonus;
    }

  //--------------------
  // So at this point we can finally apply all the modifiers to the influencers
  // SECONDARY stats due to 'magical' items and spells and the like
  //
  AddInfluencerItemSecondaryBonus( & Me.armour_item );
  AddInfluencerItemSecondaryBonus( & Me.weapon_item );
  AddInfluencerItemSecondaryBonus( & Me.drive_item );
  AddInfluencerItemSecondaryBonus( & Me.shield_item );
  AddInfluencerItemSecondaryBonus( & Me.special_item );
  AddInfluencerItemSecondaryBonus( & Me.aux1_item );
  AddInfluencerItemSecondaryBonus( & Me.aux2_item );


}; // void UpdateAllCharacterStats ( void )


/* ----------------------------------------------------------------------
 * This function displays the character screen.
 * ---------------------------------------------------------------------- */
void 
ShowCharacterScreen ( void )
{
  static SDL_Rect ButtonRect;
  static SDL_Surface *CharacterScreenImage = NULL;
  static SDL_Surface *PlusButtonImage = NULL;
  char *fpath;
  char CharText[1000];
  // SDL_Rect TargetRect;
  static int MouseButtonPressedPreviousFrame = FALSE;
  point CurPos;

  DebugPrintf (2, "\nvoid ShowInventoryMessages( ... ): Function call confirmed.");

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more, but to restore the usual user rectangle size
  // back to normal and to return...
  //
  if ( GameConfig.CharacterScreen_Visible == FALSE ) return;

  // --------------------
  // We will need the current mouse position on several spots...
  //
  CurPos.x = GetMousePos_x() + 16;
  CurPos.y = GetMousePos_y() + 16;

  // --------------------
  // Some things like the loading of the character screen
  // need to be done only once at the first call of this
  // function. 
  //
  if ( CharacterScreenImage == NULL )
    {
      // SDL_FillRect( Screen, & InventoryRect , 0x0FFFFFF );
      fpath = find_file ( "character.png" , GRAPHICS_DIR, FALSE);
      CharacterScreenImage = IMG_Load( fpath );
      fpath = find_file ( "PlusButton.png" , GRAPHICS_DIR, FALSE);
      PlusButtonImage = IMG_Load( fpath );

      //--------------------
      // We define the right side of the user screen as the rectangle
      // for our inventory screen.
      //
      CharacterRect.x = CHARACTERRECT_X;
      CharacterRect.y = 0; 
      CharacterRect.w = CHARACTERRECT_W;
      CharacterRect.h = CHARACTERRECT_H;
    }
  
  //--------------------
  // At this point we know, that the character screen is desired and must be
  // displayed in-game:
  //
  // Into this inventory rectangle we draw the inventory mask
  //
  SDL_SetClipRect( Screen, NULL );
  SDL_BlitSurface ( CharacterScreenImage , NULL , Screen , &CharacterRect );

  //--------------------
  // Now we can start to fill in the character values:
  // Name, Class, Level, Exp, Strength, Dex, ...
  //
  DisplayText( Me.character_name , 20 + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );
  switch ( Me.character_class )
    {
    case WAR_BOT:
      DisplayText( "War Bot" , CLASS_X + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );
      break;
    case SNIPER_BOT:
      DisplayText( "Sniper Bot" , CLASS_X + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );
      break;
    case MIND_BOT:
      DisplayText( "Mind Bot" , CLASS_X + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );
      break;
    default:
      DebugPrintf( 0 , "\n\nILLEGAL CHARACTER CLASS FOUND!!! ERROR!!! TERMINATING....." );
      Terminate( ERR );
      break;
    }

  sprintf( CharText , "%4d", Me.exp_level );
  DisplayText( CharText , 62 + CharacterRect.x , 56 + CharacterRect.y , &CharacterRect );

  // Me.Experience = RealScore;
  sprintf( CharText , "%6ld", Me.Experience ); // this should be the real score, sooner or later
  DisplayText( CharText , 240 + CharacterRect.x ,  EXPERIENCE_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%6ld", Me.ExpRequired ); 
  DisplayText( CharText , 240 + CharacterRect.x ,  NEXT_LEVEL_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%6ld", Me.Gold ); 
  DisplayText( CharText , 240 + CharacterRect.x ,  GOLD_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me.base_strength );
  DisplayText( CharText , STR_BASE_X + CharacterRect.x , STR_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Strength );
  if ( Me.Strength != Me.base_strength ) SetCurrentFont( Red_BFont) ;
  DisplayText( CharText , STR_NOW_X + CharacterRect.x , STR_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me.base_magic );
  DisplayText( CharText , 100 + CharacterRect.x , MAG_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Magic );
  if ( Me.Magic != Me.base_magic ) SetCurrentFont( Red_BFont) ;
  DisplayText( CharText , 148 + CharacterRect.x , MAG_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me.base_dexterity );
  DisplayText( CharText , 100 + CharacterRect.x , DEX_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Dexterity );
  if ( Me.Dexterity != Me.base_dexterity ) SetCurrentFont( Red_BFont) ;
  DisplayText( CharText , 148 + CharacterRect.x , DEX_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me.base_vitality );
  DisplayText( CharText , 100 + CharacterRect.x , VIT_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Vitality );
  if ( Me.Vitality != Me.base_vitality ) SetCurrentFont( Red_BFont) ;
  DisplayText( CharText , 148 + CharacterRect.x , VIT_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me.PointsToDistribute );
  DisplayText( CharText , 100 + CharacterRect.x , POINTS_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.maxenergy );
  DisplayText( CharText , 95 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.energy );
  DisplayText( CharText , 143 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.maxmana );
  DisplayText( CharText , 95 + CharacterRect.x , 318 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.mana );
  DisplayText( CharText , 143 + CharacterRect.x , 318 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d-%d", (int) Me.base_damage , (int) Me.base_damage + (int) Me.damage_modifier );
  DisplayText( CharText , DAMAGE_X + CharacterRect.x , DAMAGE_Y + CharacterRect.y , &CharacterRect );

  // sprintf( CharText , "%d", (int) Me.RechargeTimeModifier );
  sprintf( CharText , "%d", (int) Me.to_hit );
  strcat( CharText , "%" );
  DisplayText( CharText , RECHARGE_X + CharacterRect.x , RECHARGE_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.AC );
  DisplayText( CharText , AC_X + CharacterRect.x , AC_Y + CharacterRect.y , &CharacterRect );

  //--------------------
  // It might be the case, that the character has some points to distribute upon the character
  // stats.  Then of course, we must display the plus button instead of all character 'now' values
  //
  // Me.PointsToDistribute = 5;
  if ( Me.PointsToDistribute > 0 )
    {
      ButtonRect.x = STR_NOW_X + BUTTON_MOD_X + CharacterRect.x;
      ButtonRect.y = STR_Y + BUTTON_MOD_Y + CharacterRect.y;
      SDL_BlitSurface ( PlusButtonImage , NULL , Screen , &ButtonRect );
      ButtonRect.x = STR_NOW_X + BUTTON_MOD_X + CharacterRect.x;
      ButtonRect.y = MAG_Y + BUTTON_MOD_Y + CharacterRect.y;
      SDL_BlitSurface ( PlusButtonImage , NULL , Screen , &ButtonRect );
      ButtonRect.x = STR_NOW_X + BUTTON_MOD_X + CharacterRect.x;
      ButtonRect.y = DEX_Y + BUTTON_MOD_Y + CharacterRect.y;
      SDL_BlitSurface ( PlusButtonImage , NULL , Screen , &ButtonRect );
      ButtonRect.x = STR_NOW_X + BUTTON_MOD_X + CharacterRect.x;
      ButtonRect.y = VIT_Y + BUTTON_MOD_Y + CharacterRect.y;
      SDL_BlitSurface ( PlusButtonImage , NULL , Screen , &ButtonRect );

      if ( CursorIsOnStrButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me.base_strength++;
	  Me.PointsToDistribute--;
	}
      if ( CursorIsOnDexButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me.base_dexterity++;
	  Me.PointsToDistribute--;
	}
      if ( CursorIsOnMagButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me.base_magic++;
	  Me.PointsToDistribute--;
	  Me.mana += MANA_GAIN_PER_MAGIC_POINT;
	}
      if ( CursorIsOnVitButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me.base_vitality++;
	  Me.PointsToDistribute--;
	  Me.health += ENERGY_GAIN_PER_VIT_POINT;
	  Me.energy += ENERGY_GAIN_PER_VIT_POINT;
	}

      //--------------------
      // It might happen that the last str point was just spent.  Then we can
      // automatically close the character window for convenience of the player.
      //
      if ( Me.PointsToDistribute == 0 ) GameConfig.CharacterScreen_Visible = FALSE;
    }

  //--------------------
  // Finally, we want the part of the screen we have been editing to become
  // visible and therefore we must updated it here, since it is currently not
  // contained within the user rectangle that also gets updated every frame.
  //
  // SDL_UpdateRect( Screen , CharacterRect.x , CharacterRect.y , CharacterRect.w , CharacterRect.h );


  //--------------------
  // We want to know, if the button was pressed the previous frame when we
  // are in the next frame and back in this function.  Therefore we store
  // the current button situation, so that we can conclude on button just
  // pressed later.
  //
  MouseButtonPressedPreviousFrame = axis_is_active;

}; // ShowCharacterScreen ( void )

#undef _character_c
