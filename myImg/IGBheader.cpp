/*
	flounder - copyright 2002 Edward J. Vigmond

	This file was modified by Stefan Fruhner (2011)

	This file is part of flounder.

	flounder is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

	flounder is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
    along with flounder; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include <string>
#include "IGBheader.hpp"

using namespace std;

/* -------------- Bits de statut pour Header_Read et Header_Write ------ */

#define     MOT_CLEF_INV    2
#define     GRANDEUR_INV    4

/*
 * Types de trames
 */
#define MIN_TRAME	0
#define	C8		0
#define	C4		1
#define	HEX		2
#define HEXEDGES	3
#define HEXBRIDGES	4
#define HEXLINES	5
#define HEX2		6
#define MAX_TRAME	6
#define NTRAMES		7

#define LF  0x0A
#define FF  0x0C
#define CR  0x0D

/*---------------------------------------------------------------------------*\

				y  x->	 1 2 3 4 5
				|
				v
					X|X|X|X|X|X
				1	-o-o-o-o-o-
					X|X|X|X|X|X
				2	-o-o-o-o-o-
	\|/				X|X|X|X|X|X
C8:	-o-			3	-o-o-o-o-o-
	/|\				X|X|X|X|X|X
				4	-o-o-o-o-o-
					X|X|X|X|X|X
				5	-o-o-o-o-o-
					X|X|X|X|X|X

\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\

				y  x->	 1 2 3 4 5
				|
				v
					 | | | | |
				1	-o-o-o-o-o-
					 | | | | |
				2	-o-o-o-o-o-
	 |				 | | | | |
C4:	-o-			3	-o-o-o-o-o-
	 |				 | | | | |
				4	-o-o-o-o-o-
					 | | | | |
				5	-o-o-o-o-o-
					 | | | | |

\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\

				y  x->	 1 2 3 4 5	 1   2   3   4   5
				|
				v			   1   2   3   4   5

					\|\|\|\|\|	\ / \ / \ / \ / \ /
				1	-o-o-o-o-o-	-o---o---o---o---o---
			\|		/|/|/|/|/|/	/ \ / \ / \ / \ / \ /
	y impair	-o-	2	-o-o-o-o-o-	---o---o---o---o---o-
			/|		\|\|\|\|\|\	\ / \ / \ / \ / \ / \
HEX:				3	-o-o-o-o-o-	-o---o---o---o---o---
			 |/		/|/|/|/|/|/	/ \ / \ / \ / \ / \ /
	y pair		-o-	4	-o-o-o-o-o-	---o---o---o---o---o-
			 |\		\|\|\|\|\|\	\ / \ / \ / \ / \ / \
				5	-o-o-o-o-o-	-o---o---o---o---o---
					/|/|/|/|/|	/ \ / \ / \ / \ / \

\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\

		       x	x->	 1 2 3 4 5	 1   2   3   4   5   6

		impair	pair				   1   2   3   4   5

				y			     1   2   3   4   5
				|
		\|		v			   1   2   3   4   5
	y%4==1	-o-	-o-
		/|			\|  \|  \|	\ /  .  \ /  .  \ /  .
				1	-o-o-o-o-o-	-o---o---o---o---o---o
		 |	  /		/|  /|  /|	/ \  .  / \  .  / \  .
	y%4==2	 o 	 o	2	 o o o o o	   o   o   o   o   o
		 |	/		 |/  |/  |/	 .  \ /  .  \ /  .  \
HEXEDGES:			3	-o-o-o-o-o-	-o---o---o---o---o---o
		 |/			 |\  |\  |\	 .  / \  .  / \  .  /
	y%4==3	-o-	-o-	4	 o o o o o	   o   o   o   o   o
		 |\			\|  \|  \|	\ /  .  \ /  .  \ /  .
				5	-o-o-o-o-o-	-o---o---o---o---o---o
		 |	\		/|  /|  /|	/ \  .  / \  .  / \  .
	y%4==0	 o 	 o	6	 o o o o o	   o   o   o   o   o
		 |	  \		 |/  |/  |	 .  \ /  .  \ /  .  \

\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\

		       x	x->	 1  2  3  4  5	 1   2   3   4   5   6


		impair	pair				   1   2   3   4   5


				y			     1   2   3   4   5
				|
				v
	y%4==1	 o	-o-				   1   2   3   4   5
							     .       .       .
				1	 o -o- o -o- o	 o  -o-  o  -o-  o  -o
							     .       .       .
		 |	  /		 |   / |   / |	  \ . . / \ . . / \ .
	y%4==2	 o 	 o	2	 o  o  o  o  o	   o   o   o   o   o
		 |	/		 | /   | /   |	  . \ / . . \ / . . \
							 .       .       .
HEXBRIDGES:			3	 o -o- o -o- o	-o-  o  -o-  o  -o-  o
							 .       .       .
			 		 | \   | \   |	  . / \ . . / \ . . /
	y%4==3	 o	-o-	4	 o  o  o  o  o	   o   o   o   o   o
			 		 |   \ |   \ |	  / . . \ / . . \ / .
							     .       .       .
				5	 o -o- o -o- o	 o  -o-  o  -o-  o  -o
							     .       .       .
		 |	\		 |   / |   / |	  \ . . / \ . . / \ .
	y%4==0	 o 	 o	6	 o  o  o  o  o	   o   o   o   o   o
		 |	  \		 | /   | /   |  . \ / . . \ / . . \

\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\

		       x	x->	 1  2  3  4  5	 1   2   3   4   5   6

		impair	pair				   1   2   3   4   5

				y			     1   2   3   4   5
				|
			\|	v			   1   2   3   4   5
	y%4==1	 o	 o
			/|		  \|  \|	    \./     \./     \./
				1	 o o o o o	 o   o   o   o   o   o
		  /	 |		  /|  /|  /	    /.\     /.\     /.\
	y%4==2	-o- 	-o-	2	-o-o-o-o-o-	---o---o---o---o---o---
		/	 |		/  |/  |/	\./     \./     \./
HEXLINES:			3	 o o o o o	 o   o   o   o   o   o
		 	 |/		   |\  |\	/.\     /.\     /.\
	y%4==3	 o	 o	4	-o-o-o-o-o-	---o---o---o---o---o---
		 	 |\		  \|  \|	    \./     \./     \./
				5	 o o o o oo	 o   o   o   o   o   o
		\	 |		  /|  /|  /	    /.\     /.\     /.\
	y%4==0	-o-	-o-	6	-o-o-o-o-o-	---o---o---o---o---o---
		  \	 |		/  |/  |/	  /     \./     \./

\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\

				x->	 1 2 3 4 5	 1   2   3   4   5   6

				y			   1   2   3   4   5
				|
				v			     1   2   3   4   5

			\|				   1   2   3   4   5
	y%4==1		-o-
			/|		\|\|\|\|\|	\ / \ / \ / \ / \ /
				1	-o-o-o-o-o-	-o---o---o---o---o---o
			 |/		/|/|/|/|/|	/ \ / \ / \ / \ / \ /
	y%4==2		-o-	2	-o-o-o-o-o-	---o---o---o---o---o-
			/|		/|/|/|/|/|/	\ / \ / \ / \ / \ / \
HEX2:				3	-o-o-o-o-o-	-o---o---o---o---o---o
			 |/		\|\|\|\|\|	/ \ / \ / \ / \ / \ /
	y%4==3		-o-	4	-o-o-o-o-o-	---o---o---o---o---o-
			 |\		\|\|\|\|\|	\ / \ / \ / \ / \ / \
				5	-o-o-o-o-o-	-o---o---o---o---o---o
			\|		/|/|/|/|/|	/ \ / \ / \ / \ / \ /
	y%4==0		-o-	6	-o-o-o-o-o-	---o---o---o---o---o-
			 |\		/|/|/|/|/|	  / \ / \ / \ / \ / \

\*---------------------------------------------------------------------------*/

/* -------------- Definitions pour conv_date --------------------------- */

#define	FRANCAIS	0
#define ANGLAIS		1
#define NUMERIQUE	2
#ifndef IGB_DATE
#define IGB_DATE		0
#endif

/* -------------- Constantes diverses ---------------------------------- */

#define	    MAXL	    80	/*  Longueur maximale d'une ligne d'entete */
#define	    N_MAX_ITEMS	    30	/*  Nombre maximal d'items optionnels */
#define	    L_MAX_ITEM	    49  /*  Longueur maximale pour un item
optionnel */
#ifndef	    VRAI
#define	    VRAI	    1
#endif

#ifndef	    FAUX
#define	    FAUX	    0
#endif

int my_fputs( FILE *, char * );

int  Header_Quiet = 0;
char Header_Message[256];

const char  *Header_Type[15] = {
	"", "byte", "char", "short", "long", "float", "double", "complex",
	"double_complex", "rgba", "structure", "pointer", "list","int","uint"
};

unsigned short   Data_Size[] = {
	0, sizeof(unsigned char), sizeof(char), sizeof(short), sizeof(long), sizeof(float),
	sizeof(double), 0, 0, 0, 0, sizeof(void *), 0, sizeof(int), sizeof(unsigned int)
};

std::string IGBheader::TypeDescription[IGB_MAX_TYPE+1] = {
	"IGB_NONE",
    "IGB_BYTE",
    "IGB_CHAR",
    "IGB_SHORT",
    "IGB_LONG",
    "IGB_FLOAT",
    "IGB_DOUBLE",
    "IGB_COMPLEX",
    "IGB_D_COMPLEX",
    "IGB_RGBA",
    "IGB_STRUCTURE",
    "IGB_POINTER",
    "IGB_LIST",
    "IGB_INT",
    "IGB_UINT"
};

long	unsigned
Header_Systeme_No[] = {
	UNIX,
	SYSTEM_V,
	IRIS,
	IRIS_3000,
	IRIS_4D,
	BSD,
	ULTRIX,
	XENIX,
	SUNOS,
	SUN,
	SUN2,
	SUN3,
	SUN4,
	SUN386,
	SOLARIS,
	SPARC,
	SOLARISPC,
	IRIX,
	IRIX3,
	IRIX4,
	IRIX5,
	IRIX6,
	VMS,
	DOS,
	DOS3,
	DOS4,
	DOS4,
	DOS5,
	OS2,
	WINNT,
	WINNTINTEL,
	WINNTMIPS,
	WINNTALPHA,
	WINNTPOWER,
	PC_LINUX,
	IGB_BIG_ENDIAN,
	IGB_LITTLE_ENDIAN
};

const char  *Header_Systeme[] = {
	"unix",
	"system_v",
	"iris",
	"iris_3000",
	"iris_4d",
	"bsd",
	"ultrix",
	"xenix",
	"sunos",
	"sun",
	"sun2",
	"sun3",
	"sun4",
	"sun386",
	"solaris",
	"sparc",
	"solarispc",
	"irix",
	"irix3",
	"irix4",
	"irix5",
	"irix6",
	"vms",
	"dos",
	"dos3",
	"dos4",
	"dos5",
	"dos6",
	"os2",
	"winnt",
	"winntintel",
	"winntmips",
	"winntalpha",
	"winntpower",
	"linux",
	"big_endian",
	"little_endian"
};

IGBheader::IGBheader( gzFile in ) {
	file = in;
	v_x = v_y = v_type = 0 ;
	v_systeme = IGB_SYSTEME;
	v_comp = v_lut = v_num = v_bin = 0 ;
	v_trame = C8 ;
	v_z = v_t = 1 ;
	v_epais = 0.0 ;
	v_inc_x = v_inc_y = v_inc_z = v_inc_t = 1.0 ;
	v_org_x = v_org_y = v_org_z = 1;
	v_org_t = 0.0 ;
	v_fac_x = v_fac_y = v_fac_z = v_fac_t = 1.0 ;
	v_vect_z = NULL ;
	v_unites_x[0] = v_unites_x[40] = '\000' ;
	v_unites_y[0] = v_unites_y[40] = '\000' ;
	v_unites_z[0] = v_unites_z[40] = '\000' ;
	v_unites_t[0] = v_unites_t[40] = '\000' ;
	v_unites[0] = v_unites[40] = '\000' ;
	v_facteur = 1.0 ;
	v_zero = 0.0 ;
	v_aut_name[0] = v_aut_name[40] = '\000' ;
	v_struct_desc[0] = v_struct_desc[40] = '\000' ;
	v_comment = (char **) malloc( NALLOC*sizeof(char *) ) ;
	v_comment[0] = NULL ;
	v_transparent = NULL;
	gzipping = true;

	bool_x = bool_y = bool_type = FAUX;
	bool_z = bool_t = FAUX;
	bool_taille = FAUX;
	bool_systeme = FAUX;
	bool_num = FAUX;
	bool_bin = FAUX;
	bool_trame = FAUX;
	bool_lut = FAUX;
	bool_comp = FAUX;
	bool_epais = FAUX;
	bool_org_x = bool_org_y = bool_org_z = bool_org_t = FAUX;
	bool_inc_x = bool_inc_y = bool_inc_z = bool_inc_t = FAUX;
	bool_dim_x = bool_dim_y = bool_dim_z = bool_dim_t = FAUX;
	bool_fac_x = bool_fac_y = bool_fac_z = bool_fac_t = FAUX;
	bool_vect_z = FAUX;
	bool_unites_x = bool_unites_y = bool_unites_z = bool_unites_t = FAUX;
	bool_unites = FAUX;
	bool_facteur = bool_zero = FAUX;
	bool_aut_name = FAUX;
	bool_comment = FAUX;
	bool_transparent = FAUX;

	if (file) read(); // read in the header
}

IGBheader::~IGBheader() {
}

// set output for gzipped output
void IGBheader::fileptr( gzFile fp ) {
	file = fp;
	gzipping = true;
}

// set output for normal unzipped output
void IGBheader::fileptr( FILE* fp ) {
	file = fp;
	gzipping = false;
}


int IGBheader::write() {
	int statut = 1;

	if (getenv("HEADER_QUIET")) Header_Quiet = VRAI;

	if (!file) {
		if (!Header_Quiet)
			cerr<< "\nError: File descriptor is NULL\n";
//			ERREUR: descripteur de fichier nul\n";
		sprintf(Header_Message, "\nERREUR: descripteur de fichier nul\n");
		return(0) ;
	}

	if (v_type<IGB_MIN_TYPE || v_type>IGB_MAX_TYPE) {
		if (!Header_Quiet)
			cerr<< "\nHeader_Write: type inconnu: "<< v_type;
		sprintf(Header_Message, "\nHeader_Write: type inconnu: %d\n",
		        v_type);
		return (0);
	}
	const char *type = Header_Type[v_type];

	if (v_type==IGB_STRUCTURE && v_taille<1) {
		if (!Header_Quiet)
			cerr << "\nHeader_Write: taille invalide:" << v_taille << endl;
		sprintf(Header_Message, "\nHeader_Write: taille invalide: %d\n",
		        v_taille);
		return (0);
	}

	if (v_trame<MIN_TRAME || v_trame>MAX_TRAME) {
		if (!Header_Quiet)
			fprintf(stderr, "\nHeader_Write: trame inconnue: %d\n", v_trame);
		sprintf(Header_Message, "\nHeader_Write: trame inconnue: %d\n",
		        v_trame);
		return (0);
	}

	char* systeme = NULL;
	int i;
	for ( i=0; i<N_SYSTEMES; i++) {
		if ( (MODELE&v_systeme) == Header_Systeme_No[i] ) {
			systeme =  (char*) Header_Systeme[i] ;
//			cout << i <<" "<< Header_Systeme[i] << endl;
			break;
		}
	}
	if (i == N_SYSTEMES) {
		if (!Header_Quiet)
			cerr << "\nHeader_Write: systeme inconnu: " << v_systeme<<endl;
		sprintf(Header_Message, "\nHeader_Write: systeme inconnu: %d\n",
		        (int)v_systeme);
		strcpy(systeme, "inconnu");
	}

	char ligne[1024];
	if (bool_t) {
		if (bool_z) {
			sprintf(ligne, "x:%d y:%d z:%d t:%d type:%s systeme:%s ",
			        v_x, v_y, v_z, v_t, type, systeme);
		} else {
			sprintf(ligne, "x:%d y:%d t:%d type:%s systeme:%s ",
			        v_x, v_y, v_t, type, systeme);
		}
	} else {
		if (bool_z) {
			sprintf(ligne, "x:%d y:%d z:%d type:%s systeme:%s ",
			        v_x, v_y, v_z, type, systeme);
		} else {
			sprintf(ligne, "x:%d y:%d type:%s systeme:%s ",
			        v_x, v_y, type, systeme);
		}
	}
	int n_car = strlen(ligne);

	int n_lignes = 1;
	int n_items = 0;
	int l_item[N_MAX_ITEMS+1];
	char items[N_MAX_ITEMS+1][L_MAX_ITEM];
	/*
	   Le mot-clef "taille" n'est ecrit que pour le type STRUCTURE mais il est
	   obligatoire pour ce cas.
	 */
	if (v_type==IGB_STRUCTURE) {
		sprintf(&items[n_items][0], "taille:%d ", v_taille);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_org_x) {
		sprintf(&items[n_items][0], "org_x:%g ", v_org_x);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_org_y) {
		sprintf(&items[n_items][0], "org_y:%g ", v_org_y);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_org_z) {
		sprintf(&items[n_items][0], "org_z:%g ", v_org_z);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_org_t) {
		sprintf(&items[n_items][0], "org_t:%g ", v_org_t);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_fac_x) {
		sprintf(&items[n_items][0], "fac_x:%g ", v_fac_x);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_fac_y) {
		sprintf(&items[n_items][0], "fac_y:%g ", v_fac_y);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_fac_z) {
		sprintf(&items[n_items][0], "fac_z:%g ", v_fac_z);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_fac_t) {
		sprintf(&items[n_items][0], "fac_t:%g ", v_fac_t);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_dim_x) {
		sprintf(&items[n_items][0], "dim_x:%g ", v_dim_x);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	} else if (bool_inc_x) {
		sprintf(&items[n_items][0], "dim_x:%g ", v_inc_x);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_dim_y) {
		sprintf(&items[n_items][0], "dim_y:%g ", v_dim_y);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	} else if (bool_inc_y) {
		sprintf(&items[n_items][0], "dim_y:%g ", v_inc_y);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_dim_z) {
		sprintf(&items[n_items][0], "dim_z:%g ", v_dim_z);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	} else if (bool_inc_z) {
		sprintf(&items[n_items][0], "dim_z:%g ", v_inc_z);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_dim_t) {
		sprintf(&items[n_items][0], "dim_t:%g ", v_dim_t);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	} else if (bool_inc_t) {
		sprintf(&items[n_items][0], "dim_t:%g ", v_inc_t);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_vect_z) {
		sprintf(&items[n_items][0], "vect_z:1 ");
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_unites_x) {
		sprintf(&items[n_items][0], "unites_x:%.40s ", v_unites_x);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_unites_y) {
		sprintf(&items[n_items][0], "unites_y:%.40s ", v_unites_y);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_unites_z) {
		sprintf(&items[n_items][0], "unites_z:%.40s ", v_unites_z);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_unites_t) {
		sprintf(&items[n_items][0], "unites_t:%.40s ", v_unites_t);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_num) {
		sprintf(&items[n_items][0], "num:%d ", v_num);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_bin) {
		sprintf(&items[n_items][0], "bin:%d ", v_bin);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_trame) {
		switch(v_trame) {
			case C8:
				sprintf(&items[n_items][0], "trame:c8 ");
				break;
			case C4:
				sprintf(&items[n_items][0], "trame:c4 ");
				break;
			case HEX:
				sprintf(&items[n_items][0], "trame:hex ");
				break;
			case HEXEDGES:
				sprintf(&items[n_items][0], "trame:hexedges ");
				break;
			case HEXBRIDGES:
				sprintf(&items[n_items][0], "trame:hexbridges ");
				break;
			case HEXLINES:
				sprintf(&items[n_items][0], "trame:hexlines ");
				break;
			case HEX2:
				sprintf(&items[n_items][0], "trame:hex2 ");
				break;
		}
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_lut) {
		sprintf(&items[n_items][0], "lut:%d ", v_lut);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_comp) {
		sprintf(&items[n_items][0], "comp:%d ", v_comp);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_epais) {
		sprintf(&items[n_items][0], "epais:%g ", v_epais);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_unites) {
		sprintf(&items[n_items][0], "unites:%.40s ", v_unites);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_facteur) {
		sprintf(&items[n_items][0], "facteur:%g ", v_facteur);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_zero) {
		sprintf(&items[n_items][0], "zero:%g ", v_zero);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if( v_transparent != NULL ) {
		char *p=(char *)v_transparent, value[MAXL];
		int a;
		for( a=0; a<Data_Size[v_type]; a++ )
			sprintf( value+a*2, "%.2x", *(p++) );
		value[2*Data_Size[v_type]] = '\0';
		sprintf(&items[n_items][0], "transparent:%s ", value );
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_struct_desc) {
		sprintf(&items[n_items][0], "struct:%.40s ", v_struct_desc);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	if (bool_aut_name) {
		sprintf(&items[n_items][0], "aut:%.40s ", v_aut_name);
		l_item[n_items] = strlen(&items[n_items][0]);
		n_items++;
	}
	int n_car_total = 0;
	int n_comment = 0;
	char **comment;
	if (bool_comment) {
		comment = v_comment;
		while (*comment != NULL) {
			n_comment++;
			n_car_total += strlen(*comment++) + 3;
		}
	}

	/*
	   Ecrit tous les items, sauf les commentaires
	 */
	for (i=0; i<n_items; i++) {
		if (n_car+l_item[i]<71) {
			/*  Ajoute a la ligne courante s'il
												reste de la place */
			strcat(ligne, &items[i][0]);
			n_car += l_item[i];
		} else {
			/*  Sinon, ecrit cette ligne et
									commence-en une autre */
			ligne[n_car++] = '\r';
			ligne[n_car++] = '\n';
			ligne[n_car]   = '\000';
			n_car_total += n_car;
			if (puts_fcn((void *)file, ligne)==-1) {
				if (!Header_Quiet) {
					fprintf(stderr, "\nHeader_Write: Erreur a l'ecriture \n");
					perror("\n *** ");
					fprintf(stderr,  "\n");
				}
				sprintf(Header_Message, "\nHeader_Write: Erreur a l'ecriture \n");
				return (0);
			}
			strcpy(ligne, &items[i][0]);
			n_car = l_item[i];
			n_lignes++;
		}
	}

	/*
	   Termine la derniere ligne
	 */
	ligne[n_car++] = '\r';
	ligne[n_car++] = '\n';
	ligne[n_car]   = '\000';
	n_car_total += n_car;
	if (puts_fcn((void *)file, ligne )==-1) {
		if (!Header_Quiet) {
			fprintf(stderr, "\nHeader_Write: Erreur a l'ecriture \n");
			perror("\n *** ");
			fprintf(stderr,  "\n");
		}
		sprintf(Header_Message, "\nHeader_Write: Erreur a l'ecriture \n");
		return (0);
	}
	n_lignes++;

	/*
	   Determine le nombre de caracteres et de lignes supplementaires
	   necessaires
	 */
	int n_blocs   = 1 + (n_car_total-1)/1024;
	int n_car_sup = n_blocs*1024 - n_car_total;
	int n_lig_sup;
	if (n_car_sup>0) {
		n_lig_sup = 1 + (n_car_sup-1)/72;
	} else {
		n_lig_sup = 0;
	}
	int n_car_dl  = 1 + (n_car_sup-1)%72;

	/**
	   Ecrit les commentaires
	   \todo This is buggy ... why ?
	 */
	if (bool_comment) {
		comment = v_comment;
		while (*comment != NULL) {
//			printf("#%.80s\r\n", *comment);
			if (gzipping)
				n_car = gzprintf((gzFile) file, "#%.80s\r\n", *comment++);
			else
				n_car = fprintf( (FILE*)file, "#%.80s\r\n", *comment++);

			if (n_car==0) {
				if (!Header_Quiet) {
					fprintf(stderr, "\nHeader_Write: Erreur a l'ecriture \n");
					perror("\n *** ");
					fprintf(stderr,  "\n");
				}
				sprintf(Header_Message, "\nHeader_Write: Erreur a l'ecriture \n");
				return (0);
			}
		}
	}

	/*
	   Complete l'entete a un multiple de 1024 caracteres
	 */
	for (i=0; i<70; i++) ligne[i] = ' ';
	ligne[70] = '\r';
	ligne[71] = '\n';
	ligne[72] = '\000';
	for (i=0; i<n_lig_sup-1; i++) {
		if (puts_fcn((void *)file, ligne)==-1) {
			if (!Header_Quiet) {
				fprintf(stderr, "\nHeader_Write: Erreur a l'ecriture \n");
				perror("\n *** ");
				fprintf(stderr,  "\n");
			}
			sprintf(Header_Message, "\nHeader_Write: Erreur a l'ecriture \n");
			return 0;
		}
	}

	/*
	   La derniere ligne se termine par un saut de page (FF)
	 */
	for (i=0; i<n_car_dl-2; i++) ligne[i] = ' ';
	if (n_car_dl>2) ligne[n_car_dl-3] = '\r';
	if (n_car_dl>1) ligne[n_car_dl-2] = '\n';
	ligne[n_car_dl-1] = FF;
	ligne[n_car_dl] = '\000';
	if (puts_fcn( (void *)file, ligne )==-1) {
		if (!Header_Quiet) {
			fprintf(stderr, "\nHeader_Write: Erreur a l'ecriture \n");
			perror("\n *** ");
			fprintf(stderr,  "\n");
		}
		sprintf(Header_Message, "\nHeader_Write: Erreur a l'ecriture \n");
		return (0);
	}

	if (n_car_total>1024) {
		if (!Header_Quiet)
			fprintf(stderr,
			        "\nHeader_Write ATTENTION: etiquette de grandeur non-standard \n");
		sprintf(Header_Message,
		        "\nHeader_Write ATTENTION: etiquette de grandeur non-standard \n");
		statut |= GRANDEUR_INV;
	} else {
		sprintf(Header_Message,
		        "\nHeader_Write: Entete transcrite sans problemes\n");
	}

	/*
	if (bool_vect_z) {
		if (v_vect_z==NULL) {
			if (!Header_Quiet)
				fprintf(stderr,"\nERREUR: vect_z nul\n");
			sprintf(Header_Message,	"\nERREUR: vect_z nul\n");
			return(0) ;
		}
		gzwrite(file, v_vect_z, sizeof(float)*v_z);
	}
	*/

	return (statut);

}


/*
 * read in a header and return 0 if all is o.k.
 */
int IGBheader::read() {
	int go=VRAI, nosup=VRAI;
	int in, com=0;
	char    str[MAXL+1];
	int statut=0;

	// Rewind if the header has been read previously
	if (gzipping)
		gzrewind( (gzFile) file ) ;
	else
		rewind( (FILE*) file ) ;

	/* --- pour toutes les lignes de l'entete (def=8) ou jusqu'a un <FF> -- */
	for ( int s=8; (s>0 || nosup) && go; s-- ) {

		/* --- lit la ligne dans le fichier --- */
		int i = 0 ;

		while(1) {

			/* --- (ligne trop longue) --> erreur --- */
			if ( i >= 80 ) {
				if (!Header_Quiet)
					fprintf(stderr,
					    "\nERREUR ligne trop longue (>80) dans l'entete !\n" ) ;
				sprintf(Header_Message,
				        "\nERREUR ligne trop longue (>80) dans l'entete !\n" ) ;
				return( 0 ) ;
			}

			/* --- lit le caractere suivant --- */
			if (gzipping)
				in = gzgetc( (gzFile) file ) ;
			else
				in = fgetc( (FILE*) file ) ;

			/* --- (EOF dans l'entete) --> erreur --- */
			if ( in == -1 ) {
				if (!Header_Quiet)
					fprintf(stderr,
					        "\nERREUR Fin de fichier dans l'entete !\n" ) ;
				sprintf(Header_Message,
				        "\nERREUR Fin de fichier dans l'entete !\n" ) ;
				return( 0 ) ;
			}

			/* --- FF --> termine le header --- */
			else if ( in == FF ) {
				str[i] = '\000' ;
				go = FAUX ;
				break ;
			}

			/* --- CR et LF --> termine la ligne --- */
			else if ( in == CR ) {
				str[i] = '\000' ;
			} else if ( in == LF ) {
				str[i] = '\000' ;
				break ;
			}

			/* --- (0x20 < caractere > 0x7E) si non --> erreur --- */
			else if ( in && ! isprint(in) ) {
				if (!Header_Quiet)
					fprintf(stderr,
					        "\nERREUR caract. non imprim. 0x%.2X dans l'entete\n", in );
				sprintf(Header_Message,
				        "\nERREUR caract. non imprim. 0x%.2X dans l'entete !\n", in );
				return( 0 ) ;
			} else {
				str[i++] = (char) in ;
			}

		}

		/* --- ----- ----- ----- sauve les commentaires ----- ----- --- */
		for ( char* pt_1=str; *pt_1; pt_1++ ) {

			/* --- '*' ou '#' = commentaires ------> dans comment --- */
			if ( *pt_1 == '*' || *pt_1 == '#') {
				if ( *(pt_1+1) ) {
					v_comment[com] = (char *)malloc( strlen(pt_1+1) + 1 ) ;
					strcpy( v_comment[com++], pt_1+1 ) ;
					if (com%NALLOC == 0)
						v_comment = (char **)
						            realloc(v_comment, (com+NALLOC)*sizeof(char *));
					v_comment[com] = NULL ;
					bool_comment = VRAI;
				}
				*pt_1 = '\000' ;
				break ;
			}

			/* --- convertit majuscule un minuscules --- */
			if (isupper(*pt_1)) *pt_1 = tolower( *pt_1 ) ;

		}


		/* --- ----- ----- ----- analyse la ligne ----- ----- ----- --- */
		for ( char *pt_1=strtok(str," ,;\t"); pt_1; pt_1=strtok(NULL," ,;\t") ) {

			/* --- separe la chaine de caract. --- */
			/* --- pt_1 pointe au key_word ---- */
			/* --- pt_2 pointe a la donne  ---- */
			char *pt_2;
			for ( pt_2 = pt_1; *pt_2 != ':'; pt_2 ++ )
				if ( ! *pt_2 ) {
					if (!Header_Quiet)
						fprintf(stderr,
						        "\nERREUR de syntaxe dans l'entete (%s)\n", pt_1);
					sprintf(Header_Message,
					        "\nERREUR de syntaxe dans l'entete (%s)\n", pt_1);
					return( 0 ) ;
				}

			*pt_2++ = '\000' ;

			/* --- recherche le mot-clef --- */
			if ( ! strcmp( pt_1, "x" ) ) {
				v_x = atoi( pt_2 ) ;
				bool_x = VRAI;

			} else if ( ! strcmp( pt_1, "y" ) ) {
				v_y = atoi( pt_2 ) ;
				bool_y = VRAI;

			} else if ( ! strcmp( pt_1, "z" ) ) {
				v_z = atoi( pt_2 ) ;
				bool_z = VRAI;

			} else if ( ! strcmp( pt_1, "t" ) ) {
				v_t = atoi( pt_2 ) ;
				bool_t = VRAI;

				/* Pour compatibilite avec les vielles images */
			} else if ( ! strcmp( pt_1, "sup" ) ) {
				s += atoi( pt_2 ) ;
				nosup = FAUX;

			} else if ( ! strcmp( pt_1, "type" ) ) {

				if ( ! strcmp( pt_2, "byte" ) ) {
					v_type = IGB_BYTE ;
				} else if ( ! strcmp( pt_2, "char" ) ) {
					v_type = IGB_CHAR ;
				} else if ( ! strcmp( pt_2, "short" ) ) {
					v_type = IGB_SHORT ;
				} else if ( ! strcmp( pt_2, "long" ) ) {
					v_type = IGB_LONG ;
				} else if ( ! strcmp( pt_2, "float" ) ) {
					v_type = IGB_FLOAT ;
				} else if ( ! strcmp( pt_2, "double" ) ) {
					v_type = IGB_DOUBLE ;
				} else if ( ! strcmp( pt_2, "complex" ) ) {
					v_type = IGB_COMPLEX ;
				} else if ( ! strcmp( pt_2, "double_complex" ) ) {
					v_type = IGB_D_COMPLEX ;
				} else if ( ! strcmp( pt_2, "rgba" ) ) {
					v_type = IGB_RGBA ;
				} else if ( ! strcmp( pt_2, "structure" ) ) {
					v_type = IGB_STRUCTURE ;
				} else if ( ! strcmp( pt_2, "int" ) ) {
					v_type = IGB_INT;
				} else if ( ! strcmp( pt_2, "uint" ) ) {
					v_type = IGB_UINT;
				}
				bool_type = VRAI;

			} else if ( ! strcmp( pt_1, "taille" ) ) {
				v_taille = atoi( pt_2 ) ;
				bool_taille = VRAI;

			} else if ( ! strcmp( pt_1, "systeme" ) ) {
				v_systeme = INCONNU;
				for (i=0; i<N_SYSTEMES; i++) {
					if ( ! strcmp( pt_2, Header_Systeme[i] ) ) {
						v_systeme = Header_Systeme_No[i] ;
						break;
					}
				}

			} else if ( ! strcmp( pt_1, "bin" ) ) {
				v_bin = atoi( pt_2 ) ;
				bool_bin = VRAI;

			} else if ( ! strcmp( pt_1, "trame" ) ) {

				if ( ! strcmp( pt_2, "c8" ) ) {
					v_trame = C8 ;
				} else if ( ! strcmp( pt_2, "c4" ) ) {
					v_trame = C4 ;
				} else if ( ! strcmp( pt_2, "hex" ) ) {
					v_trame = HEX ;
				} else if ( ! strcmp( pt_2, "hexedges" ) ) {
					v_trame = HEXEDGES ;
				} else if ( ! strcmp( pt_2, "hexbridges" ) ) {
					v_trame = HEXBRIDGES ;
				} else if ( ! strcmp( pt_2, "hexlines" ) ) {
					v_trame = HEXLINES ;
				} else if ( ! strcmp( pt_2, "hex2" ) ) {
					v_trame = HEX2 ;
				}
				bool_trame = VRAI;

			} else if ( ! strcmp( pt_1, "num" ) ) {
				v_num = atoi( pt_2 ) ;
				bool_num = VRAI;

			} else if ( ! strcmp( pt_1, "comp" ) ) {
				v_comp = atoi( pt_2 ) ;
				bool_comp = VRAI;

			} else if ( ! strcmp( pt_1, "lut" ) ) {
				v_lut = atoi( pt_2 ) ;
				bool_lut = VRAI;

			} else if ( ! strcmp( pt_1, "dim_x" ) ) {
				v_dim_x = atof( pt_2 ) ;
				bool_dim_x = VRAI;

			} else if ( ! strcmp( pt_1, "dim_y" ) ) {
				v_dim_y = atof( pt_2 ) ;
				bool_dim_y = VRAI;

			} else if ( ! strcmp( pt_1, "dim_z" ) ) {
				v_dim_z = atof( pt_2 ) ;
				bool_dim_z = VRAI;

			} else if ( ! strcmp( pt_1, "dim_t" ) ) {
				v_dim_t = atof( pt_2 ) ;
				bool_dim_t = VRAI;

			} else if ( ! strcmp( pt_1, "fac_x" ) ) {
				v_fac_x = atof( pt_2 ) ;
				bool_fac_x = VRAI;

			} else if ( ! strcmp( pt_1, "fac_y" ) ) {
				v_fac_y = atof( pt_2 ) ;
				bool_fac_y = VRAI;

			} else if ( ! strcmp( pt_1, "fac_z" ) ) {
				v_fac_z = atof( pt_2 ) ;
				bool_fac_z = VRAI;

			} else if ( ! strcmp( pt_1, "fac_t" ) ) {
				v_fac_t = atof( pt_2 ) ;
				bool_fac_t = VRAI;

			} else if ( ! strcmp( pt_1, "inc_x" ) ) {
				v_inc_x = atof( pt_2 ) ;
				bool_inc_x = VRAI;

			} else if ( ! strcmp( pt_1, "inc_y" ) ) {
				v_inc_y = atof( pt_2 ) ;
				bool_inc_y = VRAI;

			} else if ( ! strcmp( pt_1, "inc_z" ) ) {
				v_inc_z = atof( pt_2 ) ;
				bool_inc_z = VRAI;

			} else if ( ! strcmp( pt_1, "inc_t" ) ) {
				v_inc_t = atof( pt_2 ) ;
				bool_inc_t = VRAI;

			} else if ( ! strcmp( pt_1, "org_x" ) ) {
				v_org_x = atof( pt_2 ) ;
				bool_org_x = VRAI;

			} else if ( ! strcmp( pt_1, "org_y" ) ) {
				v_org_y = atof( pt_2 ) ;
				bool_org_y = VRAI;

			} else if ( ! strcmp( pt_1, "org_z" ) ) {
				v_org_z = atof( pt_2 ) ;
				bool_org_z = VRAI;

			} else if ( ! strcmp( pt_1, "org_t" ) ) {
				v_org_t = atof( pt_2 ) ;
				bool_org_t = VRAI;

			} else if ( ! strcmp( pt_1, "vect_z" ) ) {
				bool_vect_z = VRAI;

			} else if ( ! strcmp( pt_1, "unites_x" ) ) {
				strncpy( v_unites_x, pt_2, 40 ) ;
				bool_unites_x = VRAI;

			} else if ( ! strcmp( pt_1, "unites_y" ) ) {
				strncpy( v_unites_y, pt_2, 40 ) ;
				bool_unites_y = VRAI;

			} else if ( ! strcmp( pt_1, "unites_z" ) ) {
				strncpy( v_unites_z, pt_2, 40 ) ;
				bool_unites_z = VRAI;

			} else if ( ! strcmp( pt_1, "unites_t" ) ) {
				strncpy( v_unites_t, pt_2, 40 ) ;
				bool_unites_t = VRAI;

			} else if ( ! strcmp( pt_1, "epais" ) ) {
				v_epais = atof( pt_2 ) ;
				bool_epais = VRAI;

			} else if ( ! strcmp( pt_1, "unites" ) ) {
				strncpy( v_unites, pt_2, 40 ) ;
				bool_unites = VRAI;

			} else if ( ! strcmp( pt_1, "facteur" ) ) {
				v_facteur = atof( pt_2 ) ;
				bool_facteur = VRAI;

			} else if ( ! strcmp( pt_1, "zero" ) ) {
				v_zero = atof( pt_2 ) ;
				bool_zero = VRAI;

			} else if ( ! strcmp( pt_1, "struct" ) ) {
				strncpy( v_struct_desc, pt_2, 40 ) ;
				bool_struct_desc = VRAI;

			} else if ( ! strcmp( pt_1, "aut" ) ) {
				strncpy( v_aut_name, pt_2, 40 ) ;
				bool_aut_name = VRAI;

			} else if( !strcmp( pt_1, "transparent" ) ) {
				strcpy( transstr, pt_2 );
				bool_transparent = VRAI;

			} else {
				if (!Header_Quiet)
					fprintf(stderr,
					        "\nATTENTION: mot-clef %s non reconnu !\n", pt_1 ) ;
				sprintf(Header_Message,
				        "\nATTENTION: mot-clef %s non reconnu !\n", pt_1 ) ;
				statut |= MOT_CLEF_INV;
			}
		}
	}
	/* determine the transparent value if one was chosen */
	if( bool_transparent ) {
		if( (int)strlen(transstr) != 2*Data_Size[v_type] ) {
			fprintf(stderr,"ATTENTION: ignoring invalid transparent value !\n");
		} else {
			char s[3], *p, *v;
			s[2] = '\0';
			v = (char *)(v_transparent = calloc( Data_Size[v_type], 1 ));
			for( int i=0; i<Data_Size[v_type]; i++ ) {
				s[0] = transstr[i*2];
				s[1] = transstr[i*2+1];
				v[i] = strtol( s, &p, 16 );
			}
		}
	}

	/* --- l'info x y et type est obligatoire --- */
	if ( !bool_x || !bool_y || !bool_type ) {
		if (!Header_Quiet)
			fprintf(stderr, "\nERREUR x, y ou type non definis\n") ;
		sprintf(Header_Message, "\nERREUR x, y ou type non definis\n") ;
		return( 0 ) ;
	}

	/* --- calcul des inc et dim --- */

	if ( bool_dim_x )
		if ( bool_inc_x ) {
			float dim_x = v_inc_x * v_x ;
			if ( dim_x != v_dim_x ) {
				if (!Header_Quiet) {
					fprintf(stderr, "\nATTENTION:\n") ;
					fprintf(stderr,
					        "conflit entre x (%d) * inc_x (%.12g) = %.12g et dim_x (%.12g)\n",
					        v_x, v_inc_x, dim_x, v_dim_x) ;
				}
				sprintf(Header_Message,
				        "conflit entre x (%d) * inc_x (%.12g) = %.12g et dim_x (%.12g)\n",
				        v_x, v_inc_x, dim_x, v_dim_x) ;
				statut = 1 ;
			}
		} else {
			v_inc_x = v_dim_x / v_x ;
			bool_inc_x = VRAI;
		}
	else {
		v_dim_x = v_x * v_inc_x ;
		if ( bool_inc_x )
			bool_dim_x = VRAI;
	}

	if ( bool_dim_y )
		if ( bool_inc_y ) {
			float dim_y = v_inc_y * v_y ;
			if ( dim_y != v_dim_y ) {
				if (!Header_Quiet) {
					fprintf(stderr, "\nATTENTION:\n") ;
					fprintf(stderr,
					        "conflit entre y (%d) * inc_y (%.12g) = %.12g et dim_y (%.12g)\n",
					        v_y, v_inc_y, dim_y, v_dim_y) ;
				}
				sprintf(Header_Message,
				        "conflit entre y (%d) * inc_y (%.12g) = %.12g et dim_y (%.12g)\n",
				        v_y, v_inc_y, dim_y, v_dim_y) ;
				statut = 1 ;
			}
		} else {
			v_inc_y = v_dim_y / v_y ;
			if ( bool_inc_y )
				bool_inc_y = VRAI;
		}
	else {
		v_dim_y = v_y * v_inc_y ;
		bool_dim_y = VRAI;
	}

	if ( bool_dim_z )
		if ( bool_inc_z ) {
			float dim_z = v_inc_z * v_z ;
			if ( dim_z != v_dim_z ) {
				if (!Header_Quiet) {
					fprintf(stderr, "\nATTENTION:\n") ;
					fprintf(stderr,
					        "conflit entre z (%d) * inc_z (%.12g) = %.12g et dim_z (%.12g)\n",
					        v_z, v_inc_z, dim_z, v_dim_z) ;
				}
				sprintf(Header_Message,
				        "conflit entre z (%d) * inc_z (%.12g) = %.12g et dim_z (%.12g)\n",
				        v_z, v_inc_z, dim_z, v_dim_z) ;
				statut = 1 ;
			}
		} else {
			v_inc_z = v_dim_z / v_z ;
			bool_inc_z = VRAI;
		}
	else {
		v_dim_z = v_z * v_inc_z ;
		if ( bool_inc_z )
			bool_dim_z = VRAI;
	}

	if ( bool_dim_t )
		if ( bool_inc_t ) {
			float dim_t = v_inc_t * v_t ;
			if ( dim_t != v_dim_t ) {
				if (!Header_Quiet) {
					fprintf(stderr, "\nATTENTION:\n") ;
					fprintf(stderr,
					        "conflit entre t (%d) * inc_t (%.12g) = %.12g et dim_t (%.12g)\n",
					        v_t, v_inc_t, dim_t, v_dim_t) ;
				}
				sprintf(Header_Message,
				        "conflit entre t (%d) * inc_t (%.12g) = %.12g et dim_t (%.12g)\n",
				        v_t, v_inc_t, dim_t, v_dim_t) ;
				statut = 1 ;
			}
		} else {
			v_inc_t = v_dim_t / v_t ;
			bool_inc_t = VRAI;
		}
	else {
		v_dim_t = v_t * v_inc_t ;
		if ( bool_inc_t )
			bool_dim_t = VRAI;
	}

	if ( bool_taille ) {
		if (v_type!=IGB_STRUCTURE) {
			if (!Header_Quiet)
				fprintf(stderr,
				        "\nERREUR taille redefinie pour type autre que structure\n") ;
			sprintf(Header_Message,
			        "\nERREUR taille redefinie pour type autre que structure\n") ;
			return( 0 ) ;
		}
	} else {
		if (v_type==IGB_STRUCTURE) {
			if (!Header_Quiet)
				fprintf(stderr,
				        "\nERREUR taille non definie pour type structure\n") ;
			sprintf(Header_Message,
			        "\nERREUR taille non definie pour type structure\n") ;
			return( 0 ) ;
		} else {
			v_taille = Data_Size[v_type];
		}
	}
	if (gzipping){
		if (gztell((gzFile)file)!=1024) {
			gzseek((gzFile) file, 1024, SEEK_SET );
			if (!Header_Quiet) {
				fprintf(stderr,
				        "\nATTENTION: etiquette de grandeur non-standard: %d \n", (int)gztell((gzFile) file));
			}
			sprintf(Header_Message,
			        "\nATTENTION: etiquette de grandeur non-standard \n");
			statut |= GRANDEUR_INV;
		} else {
			sprintf(Header_Message,
			        "\nHeader_Read: Etiquette lue sans problemes\n");
		}

		if (bool_vect_z) {
			v_vect_z = (float *)malloc(v_z*sizeof(float));
			gzread((gzFile) file, v_vect_z, sizeof(float)*v_z);
		}

	} else { // no gzipping
		if (ftell((FILE*)file)!=1024) {
			fseek((FILE*) file, 1024, SEEK_SET );
			if (!Header_Quiet) {
				fprintf(stderr,
				        "\nATTENTION: etiquette de grandeur non-standard: %d \n", (int)ftell((FILE*) file));
			}
			sprintf(Header_Message,
			        "\nATTENTION: etiquette de grandeur non-standard \n");
			statut |= GRANDEUR_INV;
		} else {
			sprintf(Header_Message,
			        "\nHeader_Read: Etiquette lue sans problemes\n");
		}

		if (bool_vect_z) {
			v_vect_z = (float *)malloc(v_z*sizeof(float));
			size_t numread = fread(v_vect_z, sizeof(float),v_z,(FILE*)file);
			if (numread != v_z)
				fprintf(stderr,
				        "\nATTENTION: v_vect_z - Did not read excected number of items %lu/%lu \n",
				        numread, (size_t) v_z);
		}
	}

	return( statut ) ;
}

/*
 * add a comment line
 * clear comments is NULL is passed
 */
void IGBheader::comment(const char *ligne) {
	int i = 0, n_lignes = 0;

	if( ligne == NULL ) {
		while( v_comment[n_lignes] != NULL )
			free( v_comment[n_lignes++] );
		return;
	}

	while (v_comment[i++]!=NULL) n_lignes++;
	v_comment = (char **)realloc(v_comment, (n_lignes+2)*sizeof(char *));

	v_comment[n_lignes] = (char *)malloc(strlen(ligne)+1) ;
	strcpy(v_comment[n_lignes++], ligne) ;
	v_comment[n_lignes] = NULL;
	bool_comment = true;
}

const char *IGBheader::systemestr( void ) {
	int i;

	for( i=0; i<N_SYSTEMES; i++ )
		if( Header_Systeme_No[i] == v_systeme )
			break;

	if( i<N_SYSTEMES )
		return Header_Systeme[i];
	else
		return NULL;
}


void IGBheader::type(const char *datatype ) {
	int tn=IGB_MIN_TYPE;
	while ( tn<IGB_MAX_TYPE && strcmp(Header_Type[tn],datatype) )
		tn++;
	if( tn<IGB_MAX_TYPE )
		v_type = tn;
	else {
		cerr << "illegal data type specified for IGB header" << endl;
		exit(1);
	}
	bool_type = true;
}


void IGBheader::systeme( char *sys ) {
	int sn = 0;
	while( sn<N_SYSTEMES && strcmp(Header_Systeme[sn],sys) )
		sn++;
	if( sn<N_SYSTEMES )
		v_systeme = Header_Systeme_No[sn];
	else {
		cerr << "illegal system type specified for IGB header" << endl;
		exit(1);
	}
}

// simple wrapper so gzputs and fputs argument order jive
int IGBheader::puts_fcn( void* f, char* s ) {
	if( gzipping ) {
		gzFile gzf=(gzFile)f;
		return gzputs( gzf, s );
	} else {
		FILE *fptr=(FILE *)f;
		return fputs( s, fptr );
	}
}

void IGBheader :: swab( void *data) {
	unsigned char tmpb;

	if( data_size()==1 || v_type==IGB_RGBA )
		return;

	unsigned char *bp = (unsigned char *)data;
	int nd = v_x*v_y*v_z*v_t;
	switch( data_size() ) {
		case 2:
			for( int i=0; i<nd; i++ ) {
				tmpb = bp[0];
				bp[0] = bp[1];
				bp[1] = tmpb;
				bp += data_size();
			}
			break;
		case 4:
			for( int i=0; i<nd; i++ ) {
				tmpb = bp[0];
				bp[0] = bp[3];
				bp[3] = tmpb;
				tmpb = bp[1];
				bp[1] = bp[2];
				bp[2] = tmpb;
				bp += data_size();
			}
			break;
		case 8:
			for( int i=0; i<nd; i++ ) {
				tmpb = bp[0];
				bp[0] = bp[7];
				bp[7] = tmpb;
				tmpb = bp[1];
				bp[1] = bp[6];
				bp[6] = tmpb;
				tmpb = bp[2];
				bp[2] = bp[5];
				bp[5] = tmpb;
				tmpb = bp[3];
				bp[3] = bp[4];
				bp[4] = tmpb;
				bp += data_size();
			}
			break;
	}
}

size_t IGBheader::index(size_t ix, size_t iy, size_t iz, size_t it){
	if ( ix >= x()) throw Exception_IndexError(EXCEPTION_ID + "x: " + toString(ix,0) +">="+ toString(x(),0)+".");
	if ( iy >= y()) throw Exception_IndexError(EXCEPTION_ID + "y: " + toString(iy,0) +">="+ toString(y(),0)+".");
	if ( iz >= z()) throw Exception_IndexError(EXCEPTION_ID + "z: " + toString(iz,0) +">="+ toString(z(),0)+".");
	if ( it >= t()) throw Exception_IndexError(EXCEPTION_ID + "t: " + toString(it,0) +">="+ toString(t(),0)+".");
	return it*x()*y()*z() + iz*y()*z() + iy*x() + ix;
}


std::string IGBheader::TypeID2Name(int id){
	if ((id >= IGB_MIN_TYPE) && (id <= IGB_MAX_TYPE)){
		return std::string(Header_Type[id]);
	}
	return string();
}

int IGBheader::Name2TypeID(const std::string name){
	for (size_t i = IGB_MIN_TYPE; i <= IGB_MAX_TYPE; ++i){
		if (name == string(Header_Type[i])) return i;
	}
	return -1;
}

#define INFO(x) cout << "  |- " << setw(12) << #x <<" = " << this->x() << endl;
void IGBheader::info(){
	cout << " -- IGB header information:" << endl;
	INFO(x);
	INFO(y);
	INFO(z);
	INFO(t);
	INFO(type);
	cout << "  |- " << setw(16) << "(" << IGBheader::TypeID2Name(this->type()) << ")"<<endl;
	INFO(taille);
	INFO(struct_desc);
	INFO(systeme);
	INFO(dim_x);
	INFO(dim_y);
	INFO(dim_z);
	INFO(dim_t);
	INFO(fac_x);
	INFO(fac_y);
	INFO(fac_z);
	INFO(fac_t);
	INFO(org_x);
	INFO(org_y);
	INFO(org_z);
	INFO(org_t);
	INFO(inc_x);
	INFO(inc_y);
	INFO(inc_z);
	INFO(inc_t);
	INFO(facteur);
	INFO(zero);
	INFO(aut_name);
	INFO(unites);
	INFO(unites_x);
	INFO(unites_y);
	INFO(unites_z);
	INFO(unites_t );
	INFO(transparent);
	if (bool_comment){
		char **cm = this->comment();
		cout << " |- comment \t= " << *cm << endl;
	}
	cout << " --" << endl;
}

#undef INFO

bool IGBheader::compatible(IGBheader& other){
	return (		type()  == other.type()
				and x() == other.x()
				and y() == other.y()
				and z() == other.z()
				and t() == other.t());
}
