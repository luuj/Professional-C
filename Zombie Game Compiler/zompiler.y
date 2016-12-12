%{
#include "node.h"
#include <iostream>
#include <string>
extern int yylex();
extern void yyerror(const char* s);

// Global for the main block
NBlock* g_MainBlock = nullptr;

// Disable the warnings that Bison creates
#pragma warning(disable: 4065)
%}

/* Add one union member for each Node* type */
%union {
    Node* node;
	NBlock* block;
	NStatement* statement;
	NNumeric* numeric;
	NBoolean* boolean;
	std::string* string;
	int token;
}

%error-verbose

/* Terminal symbols */
%token <string> TINTEGER
%token <token> TLBRACE TRBRACE TSEMI TLPAREN TRPAREN
%token <token> TMAIN TROTATE TFORWARD TIF TELSE TATTACK TRANGEDATTACK
%token <boolean> TISHUMAN TISPASSABLE TISRANDOM TISZOMBIE

/* Statements */
%type <block> main_loop block
%type <statement> statement rotate forward attack ifelse ranged_attack
%type <boolean> boolean is_human is_passable is_random is_zombie

/* Expressions */
%type <numeric> numeric

%%

main_loop	: TMAIN TLBRACE block TRBRACE
            { g_MainBlock = $3; g_MainBlock->SetMainBlock();}
;

block		: statement
            { $$ = new NBlock(); ($$)->AddStatement($1);}
            | block statement
            { ($$)->AddStatement($2);}
;

statement	: rotate TSEMI | forward TSEMI | attack TSEMI | ifelse | ranged_attack TSEMI
;

boolean     : is_human | is_passable | is_random | is_zombie
;

ifelse      : TIF TLPAREN boolean TRPAREN TLBRACE block TRBRACE TELSE TLBRACE block TRBRACE
            { $$ = new NIfElse($3, $6, $10);}
;

rotate		: TROTATE TLPAREN numeric TRPAREN
            { $$ = new NRotate($3);}
;

forward     : TFORWARD TLPAREN TRPAREN
            { $$ = new NForward();}
;

is_human    : TISHUMAN TLPAREN numeric TRPAREN
            { $$ = new NIsHuman($3);}
;

is_passable : TISPASSABLE TLPAREN TRPAREN
            { $$ = new NIsPassable();}
;

is_random   : TISRANDOM TLPAREN TRPAREN
            { $$ = new NIsRandom();}
;

is_zombie   : TISZOMBIE TLPAREN numeric TRPAREN
            { $$ = new NIsZombie($3);}
;

attack      : TATTACK TLPAREN TRPAREN
            { $$ = new NAttack();}
;

ranged_attack: TRANGEDATTACK TLPAREN TRPAREN
            { $$ = new NRangedAttack();}
;

numeric		: TINTEGER
            { $$ = new NNumeric(*($1));}
;

%%
