/*
 * LunchMoney Client - (c) 2002 Rink Springer, GPL
 *
 */
#include <stdio.h>

#ifndef __CARD_H__
#define __CARD_H__

/* CARD_TYPE_xxx is a card type */
#define CARD_TYPE_JAB		0
#define CARD_TYPE_HOOK		1
#define CARD_TYPE_PIMPSLAP	2
#define CARD_TYPE_ELBOW		3
#define CARD_TYPE_KICK2		4
#define CARD_TYPE_SPINBACKFIST	5
#define CARD_TYPE_HEADBUTT	6
#define CARD_TYPE_UPPERCUT	7
#define CARD_TYPE_ROUNDHOUSE	8
#define CARD_TYPE_HAILMARY	9
#define CARD_TYPE_KNIFE		10
#define CARD_TYPE_HAMMER	11
#define CARD_TYPE_PIPE		12
#define CARD_TYPE_CHAIN		13
#define CARD_TYPE_DODGE		14
#define CARD_TYPE_BLOCK		15
#define CARD_TYPE_DISARM	16
#define CARD_TYPE_FREEDOM	17
#define CARD_TYPE_FIRSTAID	18
#define CARD_TYPE_HUMILIATION	19
#define CARD_TYPE_GRAB		20
#define CARD_TYPE_POWERPLAY	21
#define CARD_TYPE_HEADLOCK	22
#define CARD_TYPE_STOMP		23
#define CARD_TYPE_POKEEYE	24
#define CARD_TYPE_UPPERCUT2	25
#define CARD_TYPE_BIGCOMBO	26
#define CARD_TYPE_CHOKE		27
#define CARD_TYPE_KICK4		28
#define CARD_TYPE_NOTHING	666

/* CARD_CAT_xxx is a card category */
#define CARD_CAT_BASIC		0
#define CARD_CAT_WEAPON		1
#define CARD_CAT_DEFENSE	2
#define CARD_CAT_SPECIAL	3

/* CARD_MAX_xxx are restrictions */
#define CARD_MAX_NAME_LEN	64

/* 
 * CARD is a genuine LunchMoney card.
 *
 */
class CARD {
public:
    CARD();
    CARD(char*, int, int, int);

    char* get_name();
    int   get_type();
    int   get_cat();
    int   get_damage();

private:
    char  name[CARD_MAX_NAME_LEN];
    int   type, cat, damage;
};

#endif
