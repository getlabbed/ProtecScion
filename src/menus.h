

typedef enum MenuState_t {Selecting, Prompting};
















// Fonction generale pour charger les menus
// Fonction: LoadMenu(menu, params)

// TOP LEVEL
// Menu Principal
// Key_ENTER: LoadMenu(selection, params<Mode>)
// 1: Apprentissage
// 2: Manuel
// 3: Modification
// 4: Operation


// Menu Selection
// Key_NUMBERS: Add number to prompt
// Key_ENTER: LoadMenu(wood, params<Mode, ID>)
// Key_CLEAR: LoadMenu(principal)
// 2: Prompt

// Menu Wood
// 





// Option Globale
// Bouton "CLEAR" -> ChangeMenu("back")

// Menu Principal
// 1: Apprentissage -> ChangeMenu(Menu Apprentissage)
// 2: Manuel -> ChangeMenu(Menu Manuel)
// 3: Modification -> ChangeMenu(Menu Modification)
// 4: Operation -> ChangeMenu(Menu Operation)

// Menu Selection
// PreFunction: LoadWoods(page)
// Options(3): Wood -> ChangeMenu(Modify Wood)


// Menu Apprentissage
// 1: Arreter la collecte d'information -> Sauvegarde -> ChangeMenu("back")

// Menu Manuel
// PreFunction: If specified: 
// 1: Valeur: Type de bois -> ModificationDeValeur(Type de bois)
// 2: Valeur: Vitesse d'avancement -> ModificationDeValeur(Vitesse d'avancement)
// 3: Valeur: Vitesse de la lame -> ModificationDeValeur(Vitesse de la lame)
// 4: Confirmer -> Sauvegarde -> ChangeMenu("back") 


// Menu Modification
// PreFunction: LoadWoods(page = 0)
// Options: Wood -> ChangeMenu(Menu Manuel + params: Wood_t)
// 4: Prev -> LoadWoods(page -= 1)
// 5: Next -> LoadWoods(page += 1)


// Menu Operation
//