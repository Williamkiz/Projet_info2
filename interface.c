#include "interface.h"
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include "couleurs_console.h"

/// Caractères Unicode pour bordures (Box Drawing U+2500+)
#define LIGNE_H      "─"
#define LIGNE_V      "│"
#define COIN_HG      "┌"
#define COIN_HD      "┐"
#define COIN_BG      "└"
#define COIN_BD      "┘"
#define T_GAU        "├"
#define T_DRO        "┤"
#define T_HAU        "┬"
#define T_BAS        "┴"
#define CROIX        "┼"

/// Dimensions
enum { LARGEUR = 80, LARGEUR_BARRE = 20 };

/// Trace une bordure (haut, milieu ou bas)
void tracer_bordure(const char *gauche, const char *sep, const char *droite) {
    printf("%s", gauche);
    for (int i = 0; i < LARGEUR - 2; i++)
        printf("%s", LIGNE_H);
    printf("%s\n", droite);
}

/// Affiche le titre centré d’une équipe
void afficher_entete(const char *nom) {
    int len = strlen(nom) + 2; // “[ ” et “ ]”
    int pad = (LARGEUR - 2 - len) / 2;
    printf("%s%s[ %s ]%*s%s\n",
           LIGNE_V, COULEUR(GRAS), nom,
           pad - 1, "", COULEUR(RESET));
}

/// Affiche une barre de vie proportionnelle
void afficher_barre(int actuel, int maxi) {
    int plein = (actuel * LARGEUR_BARRE) / maxi;
    putchar('[');
    for (int i = 0; i < LARGEUR_BARRE; i++)
        putchar(i < plein ? '#' : ' ');
    putchar(']');
}

/// Affiche “➤” sous le combattant qui joue ensuite
void afficher_prochain(int idxEq, int idxCbt, const Combat *c) {
    int code = c->prochain;    // ex. 10 → équipe 1, combattant 0
    int e    = code / 10;
    int i    = code % 10;
    if (e == idxEq && i == idxCbt) putchar('➤');
    else                            putchar(' ');
}

/// Affiche les effets spéciaux en cours
void afficher_effets(const Combattant *cbt) {
    for (int e = 0; e < cbt->nb_effets; e++) {
        printf("%s%s%s",
               COULEUR(CYAN),
               cbt->effets[e].symbole,
               COULEUR(RESET));
    }
}

/// Affiche une ligne résumé pour un combattant
void afficher_ligne_combattant(const Combattant *cbt,
                               int idxEq, int idxCbt,
                               const Combat *c) {
    // Bord vertical, nom et numéro
    printf("%s %s%-10s%s |%d| ",
           LIGNE_V, COULEUR(GRAS),
           cbt->nom, COULEUR(RESET),
           idxCbt + 1);

    // Effets + barre de vie + indicateur prochain
    afficher_effets(cbt);
    printf(" ");
    afficher_barre(cbt->vie, cbt->vie_max);
    printf(" ");
    afficher_prochain(idxEq, idxCbt, c);

    // Remplissage jusqu’à la bordure
    int occupe = 1 + 10 + 4 + (cbt->nb_effets * 4)
                 + 1 + LARGEUR_BARRE + 1 + 1;
    for (int s = occupe; s < LARGEUR - 2; s++)
        putchar(' ');
    putchar('\n');
}

/// Séparateur entre équipes
void afficher_separateur(void) {
    tracer_bordure(T_GAU, CROIX, T_DRO);
}

/// Affiche toute l’interface de combat
void afficher_interface(const Combat *c) {
    // Initialisation de la locale UTF-8
    setlocale(LC_ALL, "");

    // Bordure haute
    tracer_bordure(COIN_HG, T_HAU, COIN_HD);

    // Équipe 1
    afficher_entete(c->equipes[0].nom);
    for (int i = 0; i < c->equipes[0].taille; i++) {
        afficher_ligne_combattant(&c->equipes[0].combattants[i], 0, i, c);
    }

    // Séparateur
    afficher_separateur();

    // Équipe 2
    afficher_entete(c->equipes[1].nom);
    for (int i = 0; i < c->equipes[1].taille; i++) {
        afficher_ligne_combattant(&c->equipes[1].combattants[i], 1, i, c);
    }

    // Bordure basse
    tracer_bordure(COIN_BG, T_BAS, COIN_BD);

    // Techniques spéciales du combattant actif
    const Combattant *actif =
        &c->equipes[c->equipe_actuelle].combattants[c->index_actif];

    printf("\n%sTECHNIQUES SPÉCIALES de %s%s\n\n",
           COULEUR(GRAS), actif->nom, COULEUR(RESET));

    for (int t = 0; t < actif->nb_tech; t++) {
        TechniqueSpeciale *tech = &actif->techniques[t];
        printf("%s[%d]%s %s (durée : %d, recharge : %d)\n"
               "    %s\n\n",
               COULEUR(JAUNE), t + 1, COULEUR(RESET),
               tech->nom, tech->tours_actifs, tech->recharge,
               tech->description);
    }
}


